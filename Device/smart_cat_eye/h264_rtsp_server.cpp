/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "h264_rtsp_server.h"
#include "test_common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/string.h>
#include <cstring>
#include <stdio.h>
#include "securec.h"
#include <iostream>
#include <cstring>

static int CreateTcpSocket()
{
    int fd;
    int on = 1;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

    return fd;
}

static int CreateUdpSocket()
{
    int fd;
    int on = 1;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return -1;
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

    return fd;
}

static int BindSocket(int fd, const char* ip, int port)
{
    const int on = 1;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    // 允许重复绑定
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        SAMPLE_INFO("setsockopt error");
    }
    if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0)
        return -1;

    return 0;
}

static int AcceptClient(int fd, char* ip, int* port)
{
    int ret;
    int clientfd;
    socklen_t len;
    struct sockaddr_in addr;

    ret = memset_s(&addr, sizeof(addr), 0, sizeof(addr));
    if (ret) {
        return -1;
    }
    len = sizeof(addr);

    clientfd = accept(fd, (struct sockaddr *)&addr, &len);
    if (clientfd < 0) {
        return -1;
    }

    ret = strcpy_s(ip, CLIENT_IP_MAX, inet_ntoa(addr.sin_addr));
    if (ret) {
        return -1;
    }
    *port = ntohs(addr.sin_port);

    return clientfd;
}

static inline int FrameStart(char* frame, int size)
{
    if (size < START_CODE_SIZE) {
        return -1;
    }
    if (frame[FRAME_POSITION_0] == 0 && frame[FRAME_POSITION_1] == 0
        && frame[FRAME_POSITION_2] == 0 && frame[FRAME_POSITION_3] == 1) {
        return 1;
    } else {
        return 0;
    }
}

static char* FindNextStartCode(char* frame, int len)
{
    int i;

    if (len < START_CODE_SIZE) {
        return NULL;
    }

    for (i = 0; i < len - START_CODE_SIZE; ++i) {
        if (FrameStart(frame, len)) {
            return frame;
        }
        ++frame;
    }

    return NULL;
}

int GetFrameFromH264File(int fd, char* frame, int size)
{
    int rSize, frameSize;
    char* nextStartCode = NULL;

    if (fd < 0) {
        return fd;
    }

    rSize = read(fd, frame, size);
    if (!FrameStart(frame, size)) {
        return -1;
    }
    // 寻找下一个NALU头地址
    nextStartCode = FindNextStartCode(frame + START_CODE_SIZE, rSize - START_CODE_SIZE);
    if (!nextStartCode) {
        return -1;
    } else {
        // Nalu的长度
        frameSize = (nextStartCode - frame);
        lseek(fd, frameSize - rSize, SEEK_CUR);
    }
    return frameSize;
}

// nalu长度小于最大包场：分片模式
static void PacketFragmentation(struct RtpPacket* rtpPacket, int &sendBytes, int socket, const char* ip,
    int16_t port, uint8_t* frame, uint32_t frameSize)
{
    int pktNum = frameSize / RTP_PKT_MAX_SIZE;       // 有几个完整的包
    int remainSize = frameSize % RTP_PKT_MAX_SIZE; // 剩余不完整包的大小
    int i;
    int pos = 1;
    int ret;
    int naluType = frame[0];

    /* 发送完整的包 */
    for (i = 0; i < pktNum; i++) {
        rtpPacket->payload[0] = (naluType & 0x60) | 0x1C;
        rtpPacket->payload[1] = naluType & 0x1F;

        if (i == 0) { // 第一包数据
        rtpPacket->payload[1] |= 0x80; // start
        } else if (remainSize == 0 && i == pktNum - 1) { // 最后一包数据
            rtpPacket->payload[1] |= 0x40; // end
        }
        if (memcpy_s(rtpPacket->payload + PKT_HEAD_SIZE, RTP_PKT_MAX_SIZE, frame + pos, RTP_PKT_MAX_SIZE)) {
            return;
        }
        ret = RtpSendData(socket, ip, port, rtpPacket, RTP_PKT_MAX_SIZE + PKT_HEAD_SIZE);
        if (ret < 0) {
            return;
        }

        rtpPacket->rtpHeader.seq++;
        sendBytes += ret;
        pos += RTP_PKT_MAX_SIZE;
    }

    /* 发送剩余的数据 */
    if (remainSize > 0) {
        rtpPacket->payload[0] = (naluType & 0x60) | 0x1C;
        rtpPacket->payload[1] = naluType & 0x1F;
        rtpPacket->payload[1] |= 0x40; // end

        if (memcpy_s(rtpPacket->payload + PKT_HEAD_SIZE, RTP_PKT_MAX_SIZE,
            frame + pos, remainSize + PKT_HEAD_SIZE)) {
            return;
        }
    }
    ret = RtpSendData(socket, ip, port, rtpPacket, remainSize + PKT_HEAD_SIZE);
    if (ret < 0) {
    return;
    }

    rtpPacket->rtpHeader.seq++;
    sendBytes += ret;
}

int RtpSendH264Frame(int socket, const char* ip, int16_t port,
    struct RtpPacket* rtpPacket, uint8_t* frame, uint32_t frameSize)
{
    uint8_t naluType = frame[0]; // nalu第一个字节
    int sendBytes = 0;
    int ret;

    if (frameSize <= RTP_PKT_MAX_SIZE) { // nalu长度小于最大包场：单一NALU单元模式
        if (memcpy_s(rtpPacket->payload, RTP_PKT_MAX_SIZE, frame, frameSize) != EOK) {
            return -1;
        }
        SAMPLE_INFO("rtpPacket->payload[0] = %d,frameSize = %d", rtpPacket->payload[0], frameSize);
        ret = RtpSendData(socket, ip, port, rtpPacket, frameSize);
        if (ret < 0) {
            return -1;
        }
        rtpPacket->rtpHeader.seq++;
        sendBytes += ret;
        if ((naluType & 0x1F) == SPS_TYPE || (naluType & 0x1F) == PPS_TYPE) {
            return sendBytes;
        } // 如果是SPS、PPS就不需要加时间戳
    } else {
        PacketFragmentation(rtpPacket, sendBytes, socket, ip, port, frame, frameSize);
    }
    return sendBytes;
}

static char* GetLineFromRecv(char* buf, char* line, int lineSize)
{
    while (*buf != '\n') {
        *line = *buf;
        line++;
        buf++;
    }

    *line = '\n';
    ++line;
    *line = '\0';

    ++buf;
    SAMPLE_INFO("buf->%s", buf);
    return buf;
}

static int HandleCmdOptions(char* result, int cseq)
{
    int ret = sprintf_s(result, BUF_MAX_SIZE, "RTSP/1.0 200 OK\r\n"
        "CSeq: %d\r\n"
        "Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
        "\r\n",
        cseq);
    if (ret < 1) {
        return -1;
    }
    return 0;
}

static int HandleCmdDescribe(char* result, int cseq, char* url)
{
    char sdp[SDP_MAX];

    int ret = sprintf_s(sdp, sizeof(sdp), "v=0\r\n"
        "o=- 9%ld 1 IN IP4 %s\r\n"
        "t=0 0\r\n"
        "a=control:*\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=control:track0\r\n",
        (long)time(NULL), "192.168.1.88");
    if (ret < 1) {
        SAMPLE_INFO("sdp err\n");
        return -1;
    }

    ret = sprintf_s(result, BUF_MAX_SIZE, "RTSP/1.0 200 OK\r\nCSeq: %d\r\n"
        "Content-Base: %s\r\n"
        "Content-type: application/sdp\r\n"
        "Content-length: %d\r\n\r\n"
        "%s",
        cseq,
        "192.168.1.88",
        strlen(sdp),
        sdp);
    if (ret < 1) {
        SAMPLE_INFO("result err\n");
        return -1;
    }
    return 0;
}

static int HandleCmdSetup(char* result, int cseq, int clientRtpPort)
{
    int ret = sprintf_s(result, BUF_MAX_SIZE, "RTSP/1.0 200 OK\r\n"
        "CSeq: %d\r\n"
        "Transport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d\r\n"
        "Session: 66334873\r\n"
        "\r\n",
        cseq,
        clientRtpPort,
        clientRtpPort + 1,
        SERVER_RTP_PORT,
        SERVER_RTCP_PORT);
    if (ret < 1) {
        return -1;
    }
    return 0;
}

static int HandleCmdPlay(char* result, int cseq)
{
    int ret = sprintf_s(result, BUF_MAX_SIZE, "RTSP/1.0 200 OK\r\n"
        "CSeq: %d\r\n"
        "Range: npt=0.000-\r\n"
        "Session: 66334873; timeout=60\r\n\r\n",
        cseq);
    if (ret < 1) {
        return -1;
    }
    return 0;
}

static void ParseClientPort(char * bufPtr, char* line, int &clientRtpPort, int &clientRtcpPort)
{
    while (1) {
        bufPtr = GetLineFromRecv(bufPtr, line, sizeof(line));
        if (!strncmp(line, "Transport:", strlen("Transport:"))) {
            if (sscanf_s(line, "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n",
                &clientRtpPort, &clientRtcpPort) != PORT_NUM) {
                SAMPLE_INFO("parse err\n");
            }
            break;
        }
    }
}

static void DoPlay(const char* clientIP, int &clientRtpPort, int &rtpFd)
{
    int frameSize;
    int startCode;
    char *frame = (char*)malloc(FRAME_MAX_SIZE);
    struct RtpPacket* rtpPacket = (struct RtpPacket*)malloc(FRAME_MAX_SIZE);
    int fd = open(H264_FILE_NAME, O_RDONLY);
    if (fd < 0) {
        SAMPLE_INFO("open err\n");
        return;
    }
    RtpHeaderInit(rtpPacket, 0, 0, 0, RTP_VESION, RTP_PAYLOAD_TYPE_H264, 0,
        0, 0, 0x88923423);

    SAMPLE_INFO("start play\n");
    SAMPLE_INFO("client ip:%s\n", clientIP);
    SAMPLE_INFO("client port:%d\n", clientRtpPort);
    sleep(1);
    while (1) {
        frameSize = GetFrameFromH264File(fd, frame, FRAME_MAX_SIZE);
        SAMPLE_INFO("frameSize->%d", frameSize);
        if (frameSize < 0) {
            break;
        }
        startCode = START_CODE_SIZE;
        frameSize -= startCode;
        RtpSendH264Frame(rtpFd, clientIP, clientRtpPort,
            rtpPacket, (uint8_t *)(frame + startCode), frameSize);
        rtpPacket->rtpHeader.timestamp += FRAME_TIME_ADD;
        
        usleep(FRAME_WAITE_TIME);
    }
    free(frame);
    free(rtpPacket);
}

static void Finish(int &clientSockfd, char* recvBuffer, char* sendBuffer)
{
    SAMPLE_INFO("finish\n");
    close(clientSockfd);
    free(recvBuffer);
    free(sendBuffer);
}

static int MethodSwitch(char* method, char* sendBuffer, int cseq, char* url, int clientRtpPort)
{
    if (!strcmp(method, "OPTIONS")) {
        if (HandleCmdOptions(sendBuffer, cseq)) {
            SAMPLE_INFO("failed to handle options\n");
            return -1;
        }
    } else if (!strcmp(method, "DESCRIBE")) {
        if (HandleCmdDescribe(sendBuffer, cseq, url)) {
            SAMPLE_INFO("failed to handle describe\n");
            return -1;
        }
    } else if (!strcmp(method, "SETUP")) {
        if (HandleCmdSetup(sendBuffer, cseq, clientRtpPort)) {
            SAMPLE_INFO("failed to handle setup\n");
            return -1;
        }
    } else if (!strcmp(method, "PLAY")) {
        if (HandleCmdPlay(sendBuffer, cseq)) {
            SAMPLE_INFO("failed to handle play\n");
            return -1;
        }
    } else {
        return -1;
    }
    return 0;
}

static void DoClient(int clientSockfd, const char* clientIP, int clientPort,
    int rtpFd, int rtcpFd)
{
    char method[40];
    char url[100];
    char version[40];
    int cseq;
    int clientRtpPort, clientRtcpPort;
    char * bufPtr = NULL;
    char* recvBuffer = (char*)malloc(BUF_MAX_SIZE);
    char* sendBuffer = (char*)malloc(BUF_MAX_SIZE);
    char line[400];

    while (1) {
        int recvSize;

        recvSize = recv(clientSockfd, recvBuffer, BUF_MAX_SIZE, 0);
        if (recvSize <= 0) {
            Finish(clientSockfd, recvBuffer, sendBuffer);
            return;
        }
        recvBuffer[recvSize] = '\0';
        SAMPLE_INFO("---------------C->S--------------\n");
        SAMPLE_INFO("%s", recvBuffer);

        /* 解析方法 */
        bufPtr = GetLineFromRecv(recvBuffer, line, sizeof(line));
        if (sscanf_s(line, "%s ", method) != 1) {
            SAMPLE_INFO("parse err\n");
        }
        SAMPLE_INFO("method->%s,url->%s,version->%s", method, url, version);

        /* 解析序列号 */
        bufPtr = GetLineFromRecv(bufPtr, line, sizeof(line));
        if (sscanf_s(line, "CSeq: %d\r\n", &cseq) != 1) {
            SAMPLE_INFO("parse err\n");
            Finish(clientSockfd, recvBuffer, sendBuffer);
            return;
        }

        /* 如果是SETUP，那么就再解析client_port */
        if (!strcmp(method, "SETUP")) {
            ParseClientPort(bufPtr, line, clientRtpPort, clientRtcpPort);
        }

        if (MethodSwitch(method, sendBuffer, cseq, url, clientRtpPort)) {
            SAMPLE_INFO("MethodSwitch err\n");
            Finish(clientSockfd, recvBuffer, sendBuffer);
            return;
        }

        SAMPLE_INFO("---------------S->C--------------\n");
        SAMPLE_INFO("%s", sendBuffer);
        send(clientSockfd, sendBuffer, strlen(sendBuffer), 0);

        /* 开始播放，发送RTP包 */

        if (!strcmp(method, "PLAY")) {
            DoPlay(clientIP, clientRtpPort, rtpFd);
            Finish(clientSockfd, recvBuffer, sendBuffer);
            return;
        }
    }
}

int RtspServerThread()
{
    int rtspFd;
    int rtpFd, rtcpFd;

    rtspFd = CreateTcpSocket();
    if (rtspFd < 0) {
        SAMPLE_INFO("failed to create tcp socket\n");
        return -1;
    }

    if (BindSocket(rtspFd, "0.0.0.0", SERVER_PORT) < 0) {
        SAMPLE_INFO("failed to bind addr\n");
        return -1;
    }

    if (listen(rtspFd, LISTEN_BACK_LOG) < 0) {
        SAMPLE_INFO("failed to listen\n");
        return -1;
    }

    rtpFd = CreateUdpSocket();
    rtcpFd = CreateUdpSocket();
    if (rtpFd < 0 || rtcpFd < 0) {
        SAMPLE_INFO("failed to create udp socket\n");
        return -1;
    }

    if (BindSocket(rtpFd, "0.0.0.0", SERVER_RTP_PORT) < 0 ||
            BindSocket(rtcpFd, "0.0.0.0", SERVER_RTCP_PORT) < 0) {
        SAMPLE_INFO("failed to bind addr\n");
        return -1;
    }

    SAMPLE_INFO("rtsp://192.168.1.88:%d\n", SERVER_PORT);

    while (1) {
        int clientSockfd;
        char clientIp[CLIENT_IP_MAX];
        int clientPort;

        clientSockfd = AcceptClient(rtspFd, clientIp, &clientPort);
        if (clientSockfd < 0) {
            SAMPLE_INFO("failed to accept client\n");
            return -1;
        }

        SAMPLE_INFO("accept client;client ip:%s,client port:%d\n", clientIp, clientPort);

        DoClient(clientSockfd, clientIp, clientPort, rtpFd, rtcpFd);
    }
    return 0;
}
