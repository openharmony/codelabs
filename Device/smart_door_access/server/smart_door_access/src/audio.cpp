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

#include "audio.h"
#include "common.h"
#include "audio_renderer.h"

using namespace OHOS;
using namespace OHOS::AudioStandard;

static int32_t InitializeRenderer(std::unique_ptr<AudioRenderer> &audioRenderer)
{
    AudioRendererParams rendererParams;
    rendererParams.sampleFormat = SAMPLE_S16LE;
    rendererParams.sampleRate = SAMPLE_RATE_44100;
    rendererParams.channelCount = STEREO;
    rendererParams.encodingType = ENCODING_PCM;

    return audioRenderer->SetParams(rendererParams);
}

static void OpenAudioFile(std::unique_ptr<AudioRenderer> &audioRenderer, std::string path)
{
    const int32_t WRITE_BUFFERS_COUNT = 500;
    int32_t ret = -1;
    size_t bufferLen;
    ret = audioRenderer->GetBufferSize(bufferLen);
    if (ret != 0) {
        SAMPLE_INFO("GetBufferSize error");
    }
    FILE *wavFile = fopen(path.c_str(), "rb");
    uint8_t *buffer = (uint8_t *)malloc(bufferLen);
    size_t bytesToWrite = 0;
    int32_t bytesWritten = 0;
    size_t minBytes = 4;
    int32_t numBuffersToRender = WRITE_BUFFERS_COUNT;
    while (numBuffersToRender) {
        bytesToWrite = fread(buffer, 1, bufferLen, wavFile);
        bytesWritten = 0;
        while ((static_cast<size_t>(bytesWritten) < bytesToWrite) &&
            ((static_cast<size_t>(bytesToWrite) - bytesWritten) > minBytes)) {
            bytesWritten += audioRenderer->Write(buffer + static_cast<size_t>(bytesWritten),
                bytesToWrite - static_cast<size_t>(bytesWritten));
            if (bytesWritten < 0) {
                break;
            }
        }
        numBuffersToRender--;
    }
    audioRenderer->Drain();
    audioRenderer->Stop();
    audioRenderer->Release();

    free(buffer);
    fclose(wavFile);
}

void PlayAudio(int cmd)
{
    int ret;
    const std::string OPEN_FILE_PATH = "/system/data/openLock.wav";
    const std::string CLOSE_FILE_PATH = "/system/data/closeLock.wav";
    AudioStreamType streamType = STREAM_MUSIC; // 流类型示例
    std::unique_ptr<AudioRenderer> audioRenderer = AudioRenderer::Create(streamType);

    InitializeRenderer(audioRenderer);
    ret = audioRenderer->SetVolume(1.0);
    if (ret != 0) {
        SAMPLE_INFO("SetVolume error");
    }
    audioRenderer->Start();

    switch (cmd) {
        case PlayCmd::CLOSE:
            OpenAudioFile(audioRenderer, CLOSE_FILE_PATH);
            break;
        case PlayCmd::OPEN:
            OpenAudioFile(audioRenderer, OPEN_FILE_PATH);
            break;
        default:
            break;
    }
}