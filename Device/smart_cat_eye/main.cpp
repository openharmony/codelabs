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

#include "input/camera_input.h"
#include "input/camera_manager.h"
#include "media_log.h"
#include "surface.h"
#include "h264_rtsp_server.h"
#include "test_common.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <securec.h>

using namespace std;
using namespace OHOS;
using namespace OHOS::CameraStandard;

static sptr<CameraManager> camManagerObj;
static sptr<CaptureSession> captureSession;
static sptr<Surface> videoSurface;
static sptr<Surface> previewSurface;
static sptr<CaptureOutput> videoOutput;
static sptr<CaptureInput> cameraInput;
static int32_t sVideoFd = -1;

enum class SaveVideoMode {
    CREATE = 0,
    APPEND,
    CLOSE
};

namespace {
const std::int32_t FILE_PERMISSION_FLAG = 00766;
}

class CameraMgrCb : public CameraManagerCallback {
public:
    void OnCameraStatusChanged(const std::string &cameraID, const CameraDeviceStatus cameraStatus) const override
    {
        SAMPLE_INFO("OnCameraStatusChanged() is called, cameraID: %s, cameraStatus: %d",
            cameraID.c_str(), cameraStatus);
        return;
    }

    void OnFlashlightStatusChanged(const std::string &cameraID, const FlashlightStatus flashStatus) const override
    {
        SAMPLE_INFO("OnFlashlightStatusChanged() is called cameraID: %s, flashStatus: %d",
            cameraID.c_str(), flashStatus);
        return;
    }
};

class DeviceErrorCb : public ErrorCallback {
public:
    void OnError(const int32_t errorType, const int32_t errorMsg) const override
    {
        SAMPLE_INFO("OnError() is called, errorType: %d, errorMsg: %d", errorType, errorMsg);
        return;
    }
};

class PreviewOutputCb : public PreviewCallback {
    void OnFrameStarted() const override
    {
        SAMPLE_INFO("PreviewOutputCb:OnFrameStarted() is called!");
    }
    void OnFrameEnded(const int32_t frameCount) const override
    {
        SAMPLE_INFO("PreviewOutputCb:OnFrameEnded() is called!, frameCount: %d", frameCount);
    }
    void OnError(const int32_t errorCode) const override
    {
        SAMPLE_INFO("PreviewOutputCb:OnError() is called!, errorCode: %d", errorCode);
    }
};

class VideoOutputCb : public VideoCallback {
    void OnFrameStarted() const override
    {
        SAMPLE_INFO("VideoOutputCb:OnFrameStarted() is called!");
    }
    void OnFrameEnded(const int32_t frameCount) const override
    {
        SAMPLE_INFO("VideoOutputCb:OnFrameEnded() is called!, frameCount: %d", frameCount);
    }
    void OnError(const int32_t errorCode) const override
    {
        SAMPLE_INFO("VideoOutputCb:OnError() is called!, errorCode: %d", errorCode);
    }
};

static int32_t SaveVideoFile(const char *buffer, int32_t size, SaveVideoMode operationMode)
{
    if (operationMode == SaveVideoMode::CREATE) {
        char path[255] = {0};
        system("mkdir -p /data/media/video/");
        (void)sprintf_s(path, sizeof(path) / sizeof(path[0]),
                        "/data/media/video/test.h264");
        SAMPLE_INFO("%s, save video to file %s", __FUNCTION__, path);
        sVideoFd = open(path, O_RDWR | O_CREAT, FILE_PERMISSION_FLAG);
        if (sVideoFd == -1) {
            std::cout << "open file failed, errno = " << strerror(errno) << std::endl;
            return -1;
        }
    } else if (operationMode == SaveVideoMode::APPEND && sVideoFd != -1) {
        int32_t ret = write(sVideoFd, buffer, size);
        if (ret == -1) {
            std::cout << "write file failed, error = " << strerror(errno) << std::endl;
            close(sVideoFd);
            return -1;
        }
    } else {
        if (sVideoFd != -1) {
            close(sVideoFd);
            sVideoFd = -1;
        }
    }
    return 0;
}

class VideoSurfaceListener : public IBufferConsumerListener {
public:
    sptr<Surface> surface_;

    void OnBufferAvailable() override
    {
        if (sVideoFd == -1) {
            // Create video file
            SaveVideoFile(nullptr, 0, SaveVideoMode::CREATE);
        }
        int32_t flushFence = 0;
        int64_t timestamp = 0;
        OHOS::Rect damage;
        SAMPLE_INFO("VideoSurfaceListener OnBufferAvailable");
        OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
        surface_->AcquireBuffer(buffer, flushFence, timestamp, damage);
        if (buffer != nullptr) {
            char *addr = static_cast<char *>(buffer->GetVirAddr());
            int32_t size = buffer->GetSize();
            SAMPLE_INFO("SaveVideoFile size->%d", size);
            SaveVideoFile(addr, size, SaveVideoMode::APPEND);
            surface_->ReleaseBuffer(buffer, -1);
        } else {
            SAMPLE_INFO("AcquireBuffer failed!");
        }
    }
};

class SurfaceListener : public IBufferConsumerListener {
public:
    sptr<Surface> surface_;

    void OnBufferAvailable() override
    {
        SAMPLE_INFO("Calling SaveYUV");
    }
};

static void CameraMgrInit()
{
    camManagerObj = CameraManager::GetInstance();
    std::shared_ptr<CameraMgrCb> cameraMngrCallback = make_shared<CameraMgrCb>();
    camManagerObj->SetCallback(cameraMngrCallback);
    std::vector<sptr<CameraInfo>> cameraObjList = camManagerObj->GetCameras();
    if (cameraObjList.size() <= 0) {
        SAMPLE_INFO("Get camera error");
    }
    captureSession = camManagerObj->CreateCaptureSession();
    if (captureSession == nullptr) {
        SAMPLE_INFO("Failed to create capture session");
    }
    cameraInput = camManagerObj->CreateCameraInput(cameraObjList[0]);
    if (cameraInput == nullptr) {
        SAMPLE_INFO("Failed to create Camera Input");
    }
}

static void PreviewInit()
{
    const std::int32_t PREVIEW_DEFAULT_WIDTH = 640;
    const std::int32_t PREVIEW_DEFAULT_HEIGHT = 480;
    int32_t previewWidth = PREVIEW_DEFAULT_WIDTH;
    int32_t previewHeight = PREVIEW_DEFAULT_HEIGHT;
    int32_t intResult;
    previewSurface = Surface::CreateSurfaceAsConsumer();
    previewSurface->SetDefaultWidthAndHeight(previewWidth, previewHeight);
    sptr<SurfaceListener> listener = new SurfaceListener();
    listener->surface_ = previewSurface;
    previewSurface->RegisterConsumerListener((sptr<IBufferConsumerListener> &)listener);
    sptr<CaptureOutput> previewOutput = camManagerObj->CreatePreviewOutput(previewSurface);
    if (previewOutput == nullptr) {
        SAMPLE_INFO("Failed to create previewOutput");
    }
    SAMPLE_INFO("Setting preview callback");
    std::shared_ptr previewCallback = std::make_shared<PreviewOutputCb>();
    ((sptr<PreviewOutput> &)previewOutput)->SetCallback(previewCallback);
    intResult = captureSession->AddOutput(previewOutput);
    if (intResult != 0) {
        SAMPLE_INFO("Failed to Add output to session, intResult: %d", intResult);
    }
}

static void VideoInit()
{
    const std::int32_t VIDEO_DEFAULT_WIDTH = 640;
    const std::int32_t VIDEO_DEFAULT_HEIGHT = 360;
    int32_t videoWidth = VIDEO_DEFAULT_WIDTH;
    int32_t videoHeight = VIDEO_DEFAULT_HEIGHT;
    int32_t intResult;
    videoSurface = Surface::CreateSurfaceAsConsumer();
    videoSurface->SetDefaultWidthAndHeight(videoWidth, videoHeight);
    sptr<VideoSurfaceListener> videoListener = new VideoSurfaceListener();
    videoListener->surface_ = videoSurface;
    videoSurface->RegisterConsumerListener((sptr<IBufferConsumerListener> &)videoListener);
    videoOutput = camManagerObj->CreateVideoOutput(videoSurface);
    if (videoOutput == nullptr) {
        SAMPLE_INFO("Failed to create video output");
    }
    SAMPLE_INFO("Setting preview callback");
    std::shared_ptr videoCallback = std::make_shared<VideoOutputCb>();
    ((sptr<VideoOutput> &)videoOutput)->SetCallback(videoCallback);
    intResult = captureSession->AddOutput(videoOutput);
    if (intResult != 0) {
        SAMPLE_INFO("Failed to Add output to session, intResult: %d", intResult);
    }
}

static void StartCamera()
{
    const std::int32_t VIDEO_CAPTURE_DURATION = 10; // Sleep for 10 sec
    const std::int32_t PREVIEW_VIDEO_GAP = 2; // Sleep for 2 sec
    int32_t intResult;
    // Default sizes for PreviewOutput and VideoOutput
    CameraMgrInit();
    captureSession->BeginConfig();
    std::shared_ptr<DeviceErrorCb> deviceCallback = make_shared<DeviceErrorCb>();
    ((sptr<CameraInput> &)cameraInput)->SetErrorCallback(deviceCallback);
    intResult = captureSession->AddInput(cameraInput);
    if (intResult != 0) {
        SAMPLE_INFO("Add input to session is failed, intResult: %d", intResult);
    }
    PreviewInit();
    VideoInit();
    intResult = captureSession->CommitConfig();
    if (intResult != 0) {
        SAMPLE_INFO("Failed to Commit config, intResult: %d", intResult);
    }
    intResult = captureSession->Start();
    if (intResult != 0) {
        SAMPLE_INFO("Failed to start, intResult: %d", intResult);
    }
    SAMPLE_INFO("Preview started");
    sleep(PREVIEW_VIDEO_GAP);
    SAMPLE_INFO("Start video recording");
    ((sptr<VideoOutput> &)videoOutput)->Start();
    sleep(VIDEO_CAPTURE_DURATION);
    SAMPLE_INFO("Stop video recording");
    ((sptr<VideoOutput> &)videoOutput)->Stop();
    SAMPLE_INFO("Closing the session");
    captureSession->Stop();
    captureSession->Release();
    // Close video file
    SaveVideoFile(nullptr, 0, SaveVideoMode::CLOSE);
    camManagerObj->SetCallback(nullptr);
    SAMPLE_INFO("Camera new sample end.");
}

int main()
{
    StartCamera(); // 启动相机
    RtspServerThread(); // 启动rtsp服务
}
