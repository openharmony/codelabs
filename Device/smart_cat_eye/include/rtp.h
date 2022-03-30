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

#ifndef RTP_H
#define RTP_H
#include <stdint.h>
#define RTP_VESION              2

#define RTP_PAYLOAD_TYPE_H264   96

#define RTP_HEADER_SIZE         12
#define RTP_PKT_MAX_SIZE        1400

struct RtpHeader {
    /* byte 0 */
    uint8_t cc : 4;
    uint8_t extension : 1;
    uint8_t padding : 1;
    uint8_t version : 2;

    /* byte 1 */
    uint8_t pt : 7;
    uint8_t marker : 1;

    /* bytes 2,3 */
    uint16_t seq;

    /* bytes 4-7 */
    uint32_t timestamp;

    /* bytes 8-11 */
    uint32_t ssrc;
};

struct RtpPacket {
    struct RtpHeader rtpHeader;
    uint8_t payload[0];
};

void RtpHeaderInit(struct RtpPacket* rtpPacket, uint8_t cc, uint8_t extension,
                   uint8_t padding, uint8_t version, uint8_t pt, uint8_t marker,
                   uint16_t seq, uint32_t timestamp, uint32_t ssrc);
int RtpSendData(int socket, const char* ip, int16_t port, struct RtpPacket* rtpPacket, uint32_t dataSize);

#endif // RTP_H_
