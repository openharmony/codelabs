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

#ifndef H264_RTSP_SERVER_H
#define H264_RTSP_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string>

#include "rtp.h"
#define H264_FILE_NAME   "/data/media/video/test.h264"
#define SERVER_PORT      8554
#define SERVER_RTP_PORT  55532
#define SERVER_RTCP_PORT 55533
#define LISTEN_BACK_LOG 10
#define FRAME_POSITION_0 0
#define FRAME_POSITION_1 1
#define FRAME_POSITION_2 2
#define FRAME_POSITION_3 3
#define PKT_HEAD_SIZE 2
#define SPS_TYPE 7
#define PPS_TYPE 8
#define START_CODE_SIZE 4
#define R_BUFF_NUM 3
#define CLIENT_IP_MAX 40
#define SDP_MAX 500
#define PORT_NUM 2
#define FRAME_MAX_SIZE 500000
#define FRAME_TIME_ADD (90000 / 20)
#define FRAME_WAITE_TIME (1000 * 1000 / 20)
#define BUF_MAX_SIZE (1024 * 1024)

int RtspServerThread();

#endif