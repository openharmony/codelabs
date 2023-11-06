/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

import display from '@ohos.display';
import image from '@ohos.multimedia.image';
import mediaLibrary from '@ohos.multimedia.mediaLibrary';
import fs from '@ohos.file.fs';
import Logger from '../utils/LoggerUtil';
import { CommonConstants } from '../common/constant/CommonConstants';
import { ImageSizeItem } from '../viewmodel/ImageSizeItem';
import { ImageEditCrop } from '../viewModel/ImageEditCrop';
import { PixelMapWrapper } from '../viewmodel/PixelMapWrapper';
import Log from '../utils/LoggerUtil';
import { ScreenManager } from '../viewModel/ScreenManager';
import { CropRatioType } from '../viewmodel/CropType';

const TAG = 'ImageSelect';

@Component
export struct ImageSelect {
  @State cropEdit: ImageEditCrop | undefined = undefined;
  @State pinchValue: number = 1;
  @State isVerticalScreen: boolean = true;
  @Consume imageInfo: image.ImageInfo;
  @Consume imageWidth: number;
  @Consume imageHeight: number;
  @Consume pixelMap: image.PixelMap | undefined;
  @Consume showCanvas: boolean;
  @Consume imageOldSize: ImageSizeItem;
  @Consume @Watch('listenCrop') isCrop: boolean;
  @Consume imageScale: number;
  @Consume @Watch('listenRatio') cropRatio: CropRatioType;
  @Consume screenWidth: number;
  @Consume screenHeight: number;
  @Consume resetShow: boolean;
  @Consume @Watch('listenSave') isSave: boolean;
  @Consume isSaveFresh: boolean;
  private statusBar: number = 0;
  private titleHeight: number = CommonConstants.TITLE_HEIGHT;
  private prePinch: number = 0;
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private stateMenuSize: number = ScreenManager.getInstance().getStatusBarHeight();
  private defaultDisplay: display.Display = {} as display.Display;

  listenSave() {
    if (this.isSave && this.cropEdit !== undefined && this.pixelMap !== undefined) {
      let filter = this.cropEdit.exit();
      if (filter === undefined) {
        this.encode(this.pixelMap);
        this.isSaveFresh = !this.isSaveFresh;
        this.isSave = false;
        return;
      }
      filter.readerNew(new PixelMapWrapper(this.pixelMap, this.imageOldSize.width, this.imageOldSize.height), () => {
        let temp = this.pixelMap;
        this.pixelMap = undefined;
        this.pixelMap = temp;
        this.encode(this.pixelMap);
        if (this.cropEdit !== undefined) {
          this.cropEdit.setCanvasContext(this.context);
        }
        this.updateImageInfo();
        this.isSaveFresh = !this.isSaveFresh;
      });
    }
    this.isSave = false;
  }

  aboutToAppear() {
    this.initImageSize();
  }

  build() {
    Column() {
      Canvas(this.context)
        .width(CommonConstants.LAYOUT_FULL_SCREEN)
        .height(CommonConstants.LAYOUT_FULL_SCREEN)
        .onReady(() => {
          if (this.cropEdit !== undefined) {
            this.cropEdit.setCanvasReady(true);
            this.cropEdit.setCanvasContext(this.context);
          }
        })
    }
    .width(CommonConstants.LAYOUT_FULL_SCREEN)
    .height(CommonConstants.LAYOUT_FULL_SCREEN)
    .onTouch((event) => {
      this.onTouchStart(event);
    })
    .gesture(
      PinchGesture()
        .onActionStart((event: GestureEvent) => {
          this.onPinchGestureStart(event);
        })
        .onActionUpdate((event: GestureEvent) => {
          this.onPinchGestureUpdate(event);
        })
        .onActionEnd(() => {
          this.onPinchGestureEnd();
        })
    )
  }

  async listenCrop() {
    if (!this.isCrop && this.cropEdit !== undefined && this.pixelMap !== undefined) {
      let filter = this.cropEdit.exit();
      if (filter === undefined) {
        return;
      }
      filter.readerNew(new PixelMapWrapper(this.pixelMap, this.imageOldSize.width, this.imageOldSize.height), () => {
        let temp = this.pixelMap;
        this.pixelMap = undefined;
        this.pixelMap = temp;
        this.updateImageInfo();
      });
    }
  }

  updateImageInfo() {
    if (this.pixelMap !== undefined) {
      this.pixelMap.getImageInfo().then((imageInfo) => {
        this.imageInfo = imageInfo;
        this.imageWidth = px2vp(this.imageInfo.size.width);
        this.imageHeight = px2vp(this.imageInfo.size.height);
        this.imageOldSize = {
          width: this.imageWidth,
          height: this.imageHeight
        };
        this.showCanvas = true;
        this.calcScale();
        Logger.info('update image width height: ' + this.imageWidth + " " + this.imageHeight);
      });
    }
  }

  listenRatio() {
    if (this.cropEdit !== undefined) {
      this.cropEdit.onFixedRatioChange(this.cropRatio);
    }
  }

  calcScale() {
    let imageWidthScale = (this.screenWidth - CommonConstants.DOUBLE * CommonConstants.SCREEN_DISPLAY_MARGIN) /
    this.imageOldSize.width;
    let imageHeightScale = (this.screenHeight - CommonConstants.DOUBLE * CommonConstants.SCREEN_DISPLAY_MARGIN) /
    this.imageOldSize.height;
    this.imageScale = imageWidthScale > imageHeightScale ? imageHeightScale : imageWidthScale;
  }

  initImageSize() {
    try {
      this.defaultDisplay = display.getDefaultDisplaySync();
      this.screenWidth = px2vp(this.defaultDisplay.width);
      this.screenHeight = px2vp((this.defaultDisplay.height - this.statusBar * 2) * CommonConstants.EDIT_SCREEN_USAGE);
      this.calcScale();
      this.cropEdit = new ImageEditCrop();
      if (this.pixelMap !== undefined) {
        this.cropEdit.entry(new PixelMapWrapper(this.pixelMap, this.imageOldSize.width, this.imageOldSize.height));
      }
      this.cropEdit.setCanvasSize(this.screenWidth, this.screenHeight);
    } catch (exception) {
      Logger.error(`Failed to obtain the default display object. Code:  ${JSON.stringify(this.defaultDisplay)}`);
    }
  }

  onTouchStart(event: TouchEvent): void {
    if (this.cropEdit !== undefined) {
      let x = event.touches[0].x;
      let y = event.touches[0].y;
      if (event.type === TouchType.Down) {
        this.cropEdit.onTouchStart(x, y);
      } else if (event.type === TouchType.Move) {
        this.cropEdit.onTouchMove(x, y);
      } else if (event.type === TouchType.Up || event.type === TouchType.Cancel) {
        this.cropEdit.onTouchEnd();
      } else {
        Logger.info('touch other event');
      }
    }
  }

  onPinchGestureStart(event: GestureEvent): void {
    Log.debug(TAG, 'cropMode onPinchGestureStart called');
    this.prePinch = new Date().getTime();
    let x = 0;
    let y = 0;
    if (this.isVerticalScreen) {
      x = event.pinchCenterX;
      y = event.pinchCenterY - CommonConstants.TOP_BAR_SIZE - this.stateMenuSize;
    } else {
      x = event.pinchCenterX - CommonConstants.TOOL_BAR_SIZE;
      y = event.pinchCenterY - CommonConstants.TOP_BAR_SIZE - this.stateMenuSize;
    }
    if (this.cropEdit !== undefined) {
      this.cropEdit.onPinchStart(x, y, event.scale);
    }
  }

  onPinchGestureUpdate(event: GestureEvent): void {
    let now = new Date().getTime();
    if (now - this.prePinch >= CommonConstants.TIMEOUT) {
      this.prePinch = now;
      if (this.cropEdit !== undefined) {
        this.cropEdit.onPinchUpdate(event.scale);
      }
    }
  }

  onPinchGestureEnd(): void {
    Log.debug(TAG, 'cropMode onPinchGestureEnd called');
    if (this.cropEdit !== undefined) {
      this.cropEdit.onPinchEnd();
    }
  }

  async encode(pixelMap: PixelMap | undefined) {
    if (pixelMap === undefined) {
      return;
    }

    const newPixelMap = pixelMap;
    // Packing image.
    const imagePackerApi = image.createImagePacker();
    const packOptions: image.PackingOption = {
      format: CommonConstants.ENCODE_FORMAT,
      quality: CommonConstants.ENCODE_QUALITY
    }
    const imageData = await imagePackerApi.packing(newPixelMap, packOptions);
    Logger.info(TAG, `imageData's length is ${imageData.byteLength}`);
    // Get album's path.
    const context = getContext(this);
    const media = mediaLibrary.getMediaLibrary(context);
    const publicPath = await media.getPublicDirectory(mediaLibrary.DirectoryType.DIR_IMAGE);
    const currentTime = new Date().getTime();
    // Create image asset.
    const imageAssetInfo = await media.createAsset(
      mediaLibrary.MediaType.IMAGE,
      `${CommonConstants.IMAGE_PREFIX}_${currentTime}${CommonConstants.IMAGE_FORMAT}`,
      publicPath
    );
    const imageFd = await imageAssetInfo.open(CommonConstants.ENCODE_FILE_PERMISSION);
    await fs.write(imageFd, imageData);
    // Image resource release.
    await imageAssetInfo.close(imageFd);
    imagePackerApi.release();
    await media.release();
  }
}