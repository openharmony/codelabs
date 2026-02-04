/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import { common } from '@kit.AbilityKit';
import { camera } from '@kit.CameraKit';
import { media } from '@kit.MediaKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { fileIo as fs, fileUri } from '@kit.CoreFileKit';
import { photoAccessHelper } from '@kit.MediaLibraryKit';

// 声明全局录制器引用
export let avRecorder: media.AVRecorder | undefined = undefined;

// 视频录制主函数
export async function videoRecording(context: common.Context): Promise<void> {
  // 创建avRecorder对象
  try {
    avRecorder = await media.createAVRecorder();
    console.log("type1");
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to create avRecorder, error code: ${err.code}, message: ${err.message}`);
    return;
  }

  // 注册avRecorder回调函数。
  try {
    // 状态机变化回调函数。
    avRecorder.on('stateChange', (state: media.AVRecorderState, reason: media.StateChangeReason) => {
      console.info(`AVRecorder state is changed to ${state}, reason: ${reason}`);
    });
    // 错误上报回调函数。
    avRecorder.on('error', (error: BusinessError) => {
      console.error(`Error occurred in avRecorder, error code: ${error.code}, message: ${error.message}`);
    });
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to set avRecorder callback, error code: ${err.code}, message: ${err.message}`);
  }

  // 配置录制参数完成准备工作。
  let avProfile: media.AVRecorderProfile = {
    fileFormat: media.ContainerFormatType.CFT_MPEG_4, // 视频文件封装格式，只支持MP4。
    videoBitrate: 100000, // 视频比特率。
    videoCodec: media.CodecMimeType.VIDEO_AVC, // 视频文件编码格式，支持avc格式。
    videoFrameWidth: 640, // 视频分辨率的宽。
    videoFrameHeight: 480, // 视频分辨率的高。
    videoFrameRate: 30 // 视频帧率。
  };
  let videoMetaData: media.AVMetadata = {
    videoOrientation: '0' // 视频旋转角度，默认为0不旋转，支持的值为0、90、180、270。
  };
  let avConfig: media.AVRecorderConfig = {
    videoSourceType: media.VideoSourceType.VIDEO_SOURCE_TYPE_SURFACE_YUV, // 视频源类型，支持YUV和ES两种格式。
    profile: avProfile,
    url: 'fd://35', //  参考应用文件访问与管理开发示例新建并读写一个文件。
    metadata: videoMetaData
  };

  // 创建文件以及设置avConfig.url。
  let filePath: string = ''; // 文件路径。
  let videoFile: fs.File | undefined = undefined;
  try {
    filePath = context.filesDir + '/example.mp4'; // 文件沙箱路径，文件后缀名应与封装格式对应。
    videoFile = fs.openSync(filePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE); // 打开文件。
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to open file, error code: ${err.code}, message: ${err.message}`);
  }
  if (videoFile !== undefined) {
    avConfig.url = 'fd://' + videoFile.fd; // 更新url。
  }

  // 配置录制参数完成准备工作。
  try {
    if (avRecorder.state === 'idle' || avRecorder.state === 'stopped') { // 仅在idle或者stopped状态下调用prepare为合理状态切换。
      await avRecorder.prepare(avConfig);
    }
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to prepare avRecorder, error code: ${err.code}, message: ${err.message}`);
  }

  // 完成相机相关准备工作。
  let cameraManager: camera.CameraManager = camera.getCameraManager(context);
  let videoOutSurfaceId: string = await avRecorder.getInputSurface();
  await prepareCamera(cameraManager, videoOutSurfaceId);

  // 启动录制。
  try {
    if (avRecorder.state === 'prepared') { // 仅在prepared状态下调用start为合理状态切换。
      await startCameraOutput(); // 启动相机出流。
      await avRecorder.start();
    }
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to start avRecorder, error code: ${err.code}, message: ${err.message}`);
  }

  // 暂停录制。
  try {
    if (avRecorder.state === 'started') { // 仅在started状态下调用pause为合理状态切换。
      await avRecorder.pause();
      await stopCameraOutput(); // 停止相机出流。
    }
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to pause avRecorder, error code: ${err.code}, message: ${err.message}`);
  }

  // 恢复录制。
  try {
    if (avRecorder.state === 'paused') { // 仅在paused状态下调用resume为合理状态切换。
      await startCameraOutput(); // 启动相机出流。
      await avRecorder.resume();
    }
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to resume avRecorder, error code: ${err.code}, message: ${err.message}`);
  }

  // 停止录制
  try {
    if (avRecorder && (avRecorder.state === 'started' || avRecorder.state === 'paused')) {
      await avRecorder.stop();
      await stopCameraOutput();
      console.log('视频录制已正常停止');
    }
  } catch (error) {
    let err = error as BusinessError;
    console.error(`停止录制失败，错误码: ${err.code}, 信息: ${err.message}`);
  }

  // 重置。
  try {
    await avRecorder.reset();
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to reset avRecorder, error code: ${err.code}, message: ${err.message}`);
  }

  // 释放录制实例
  try {
    if (avRecorder) {
      await avRecorder.release();
      avRecorder = undefined;
      console.log('录制器资源已释放');
    }
  } catch (error) {
    let err = error as BusinessError;
    console.error(`释放录制器失败，错误码: ${err.code}, 信息: ${err.message}`);
  }

  // 关闭录制文件fd。
  try {
    if (videoFile !== undefined) {
      await fs.close(videoFile.fd);
    }
  } catch (error) {
    let err = error as BusinessError;
    console.error(`Failed to close fd, error code: ${err.code}, message: ${err.message}`);
  }

  // 释放相机相关实例。
  await releaseCamera();

  // 安全控件保存媒体资源至图库。
  let phAccessHelper: photoAccessHelper.PhotoAccessHelper = photoAccessHelper.getPhotoAccessHelper(context);
  // 需要确保uriPath对应的资源存在。
  let uriPath: string = fileUri.getUriFromPath(filePath); // 获取录制文件的uri，用于安全控件保存至图库。
  let assetChangeRequest: photoAccessHelper.MediaAssetChangeRequest =
    photoAccessHelper.MediaAssetChangeRequest.createVideoAssetRequest(context, uriPath);
  await phAccessHelper.applyChanges(assetChangeRequest);
}

// 相机相关准备工作。
async function prepareCamera(cameraManager: camera.CameraManager, videoOutSurfaceId: string) {
  // 具体实现查看相机资料。
}

// 启动相机出流。
async function startCameraOutput() {
  // 调用VideoOutput的start接口开始录像输出。
}

// 停止相机出流。
async function stopCameraOutput() {
  // 调用VideoOutput的stop接口停止录像输出。
}

// 释放相机实例。
async function releaseCamera() {
  // 释放相机准备阶段创建出的实例。
}

// 停止录制并释放资源的函数
export async function stopRecording(): Promise<void> {
  try {
    if (avRecorder && (avRecorder.state === 'started' || avRecorder.state === 'paused')) {
      await avRecorder.stop();
      await avRecorder.release();
      avRecorder = undefined;
      console.log("视频录制已停止");
    } else {
      console.log("当前没有进行中的录制");
    }
  } catch (error) {
    let err = error as BusinessError;
    console.error(`停止录制失败，错误码: ${err.code}, 信息: ${err.message}`);
  }
}