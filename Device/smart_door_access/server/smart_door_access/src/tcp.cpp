/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tcp.h"
#include "common.h"
#include "audio.h"

bool g_islock = false;

static int CreateTcpSocket()
{
    int fd;
    int on = 1;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));

    return fd;
}

static int BindSocket(int fd, const char *ip, int port)
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

static int AcceptClient(int fd, char *ip, int *port)
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

static void Finish(int &clientSockfd, char *recvBuffer, char *sendBuffer)
{
    close(clientSockfd);
    free(recvBuffer);
    free(sendBuffer);
    SAMPLE_INFO("finish\n");
}

static void DoClient(int clientSockfd, const char *clientIP, int clientPort)
{
    char *recvBuffer = (char *)malloc(BUF_MAX_SIZE);
    char *sendBuffer = (char *)malloc(BUF_MAX_SIZE);
    const char sendClose[6] = "close";
    const char sendOpen[5] = "open";
    const char sendError[6] = "error";

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
        if (!strcmp(recvBuffer, "close")) {
            if (!g_islock) {
                SAMPLE_INFO("---------------S->C--------------\n");
                SAMPLE_INFO("%s", sendClose);
                send(clientSockfd, sendClose, strlen(sendClose), 0);
                PlayAudio(PlayCmd::CLOSE);
                g_islock = true;
            }
        } else if (!strcmp(recvBuffer, "open")) {
            if (g_islock) {
                SAMPLE_INFO("---------------S->C--------------\n");
                SAMPLE_INFO("%s", sendOpen);
                send(clientSockfd, sendOpen, strlen(sendOpen), 0);
                PlayAudio(PlayCmd::OPEN);
                g_islock = false;
            }
        } else {
            SAMPLE_INFO("cmd err");
            send(clientSockfd, sendError, strlen(sendError), 0);
        }
    }
}

int TcpServerThread()
{
    int fd;
    fd = CreateTcpSocket();
    if (fd < 0) {
        SAMPLE_INFO("failed to create tcp socket\n");
        return -1;
    }

    if (BindSocket(fd, "192.168.1.2", SERVER_PORT) < 0) {
        SAMPLE_INFO("failed to bind addr\n");
        return -1;
    }

    if (listen(fd, LISTEN_BACK_LOG) < 0) {
        SAMPLE_INFO("failed to listen\n");
        return -1;
    }

    SAMPLE_INFO("tcp server: 192.168.1.2:%d\n", SERVER_PORT);

    while (1) {
        int clientSockfd;
        char clientIp[CLIENT_IP_MAX];
        int clientPort;

        clientSockfd = AcceptClient(fd, clientIp, &clientPort);
        if (clientSockfd < 0) {
            SAMPLE_INFO("failed to accept client\n");
            return -1;
        }

        SAMPLE_INFO("accept client;client ip:%s,client port:%d\n", clientIp, clientPort);

        DoClient(clientSockfd, clientIp, clientPort);
    }
    return 0;
}