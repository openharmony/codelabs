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

import image from '@ohos.multimedia.image';
import window from '@ohos.window';
import { IconStatus } from '../viewModel/IconListViewModel';
import AdjustContentView from '../view/AdjustContentView';
import { cropIconChangeList, menuIconList } from '../viewModel/IconListViewModel';
import Logger from '../utils/LoggerUtil';
import { RotateType, CropType, MainTabId } from '../viewModel/OptionViewModel';
import { encode } from '../utils/EncodeUtil';
import { getPixelMap } from '../utils/DecodeUtil';
import { CommonConstants } from '../common/constant/CommonConstants';
import { ImageSelect } from '../view/ImageSelect';
import { ImageSizeItem } from '../common/bean/ImageSizeItem';
import { CropRatioType } from '../common/bean/CropType';

const TAG: string = 'imageEdit';

@Entry
@Component
struct HomePage {
  @State @Watch('listenCropChange') currentIndex: number = 0;
  @State currentCropIndex: number = 0;
  @Provide pixelMap: image.PixelMap = undefined;
  @Provide imageInfo: image.ImageInfo = undefined;
  @Provide currentAdjustData: Array<number> = CommonConstants.ADJUST_SLIDER_VALUE.map((item) => item);
  @Provide resetShow: boolean = true;
  @Provide showCanvas: boolean = false;
  @Provide imageWidth: number = 0;
  @Provide imageHeight: number = 0;
  @Provide cropRatio: CropRatioType = CropRatioType.RATIO_TYPE_FREE;
  @Provide isCrop: boolean = this.currentIndex === 0;
  @Provide screenWidth: number = 0;
  @Provide screenHeight: number = 0;
  @State imageSize: ImageSizeItem = { width: 0, height: 0 };
  @Provide imageOldSize: ImageSizeItem = { width: 0, height: 0 };
  @Provide imageScale: number = 1;
  private menuIconChangeList = menuIconList;
  private cropIconChange = cropIconChangeList;
  private statusBar: number = 0;
  @Provide isPixelMapChange: boolean = false;
  @Provide isSave: boolean = false;
  @Provide isSaveFresh: boolean = false;

  aboutToAppear() {
    this.pixelInit();
    window.getLastWindow(getContext(this), (err, window) => {
      if (err) {
        Logger.error(0x0000, 'testTag', '%{public}s',`window loading has error: ${JSON.stringify(err)}`);
      }
      this.statusBar = window.getWindowProperties().windowRect?.top;
    });
  }

  build() {
    Column() {
      Row() {
        Row() {
          Text($r('app.string.image_edit'))
            .fontColor(Color.White)
            .fontSize($r('app.float.title_font_size'))
            .margin({ left: $r('app.float.home_page_title_margin') })
        }

        Blank()

        Row({ space: CommonConstants.TITLE_SPACE }) {
          Button() {
            Image($r("app.media.ic_reset"))
              .width($r('app.float.title_image_width'))
              .height($r('app.float.title_image_height'))
          }
          .visibility(this.resetShow ? Visibility.Visible : Visibility.Hidden)
          .height(CommonConstants.LAYOUT_FULL_SCREEN)
          .type(ButtonType.Normal)
          .aspectRatio(1)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.pixelInit();
          })
          .margin({ right: $r('app.float.home_page_title_margin') })

          Button() {
            Image($r("app.media.ic_save"))
              .width($r('app.float.title_image_width'))
              .height($r('app.float.title_image_height'))
          }
          .visibility(this.resetShow ? Visibility.Visible : Visibility.Hidden)
          .height(CommonConstants.LAYOUT_FULL_SCREEN)
          .type(ButtonType.Normal)
          .aspectRatio(1)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            AlertDialog.show({
              title: $r('app.string.save_image'),
              message: $r('app.string.confirm_save'),
              alignment: DialogAlignment.Center,
              primaryButton: {
                value: $r('app.string.save'),
                action: () => {
                  this.isSave = true;
                }
              },
              secondaryButton: {
                value: $r('app.string.cancel'),
                action: () => {
                  Logger.info(TAG, `cancel`);
                }
              }
            })
          })
          .margin({ right: $r('app.float.home_page_title_margin') })
        }
      }
      .width(CommonConstants.LAYOUT_FULL_SCREEN)
      .height($r('app.float.home_page_title_height'))
      .margin({ top: $r('app.float.home_page_title_margin') })

      Column() {
        if (this.isCrop && this.showCanvas && this.statusBar > 0) {
          if (this.isSaveFresh) {
            ImageSelect({
              statusBar: this.statusBar
            })
          } else {
            ImageSelect({
              statusBar: this.statusBar
            })
          }
        } else {
          if (this.isPixelMapChange) {
            Image(this.pixelMap)
              .scale({ x: this.imageScale, y: this.imageScale, z: 1 })
              .objectFit(ImageFit.None)
          } else {
            Image(this.pixelMap)
              .scale({ x: this.imageScale, y: this.imageScale, z: 1 })
              .objectFit(ImageFit.None)
          }
        }
      }
      .clip(true)
      .width(CommonConstants.LAYOUT_FULL_SCREEN)
      .height(CommonConstants.IMAGE_SHOW_HEIGHT)

      Column() {
        Tabs({ barPosition: BarPosition.End }) {
          TabContent() {
            Row() {
              ForEach(this.cropIconChange, (item: IconStatus, index: number) => {
                Image(this.currentCropIndex === index ? item?.chosen : item?.normal)
                  .width($r('app.float.crop_image_width'))
                  .height($r('app.float.crop_image_height'))
                  .onClick(() => {
                    this.cropImage(index);
                  })
              }, index => index)
            }
            .width(CommonConstants.LAYOUT_FULL_SCREEN)
            .height(CommonConstants.LAYOUT_FULL_SCREEN)
            .justifyContent(FlexAlign.SpaceAround)
          }
          .tabBar(this.TabBuilderMenu(MainTabId.CROP, $r('app.string.crop')))

          TabContent() {
            Row() {
              Image($r('app.media.ic_clockwise'))
                .width($r('app.float.rotate_image_width'))
                .height($r('app.float.rotate_image_height'))
                .margin({ right: CommonConstants.EDIT_PAGE_HEIGHT })
                .onClick(() => {
                  this.rotateImage(RotateType.CLOCKWISE);
                })
              Image($r('app.media.ic_anti_clockwise'))
                .width($r('app.float.rotate_image_width'))
                .height($r('app.float.rotate_image_height'))
                .onClick(async () => {
                  this.rotateImage(RotateType.ANTI_CLOCK);
                })
            }
            .justifyContent(FlexAlign.Center)
            .width(CommonConstants.LAYOUT_FULL_SCREEN)
            .height(CommonConstants.LAYOUT_FULL_SCREEN)
          }
          .tabBar(this.TabBuilderMenu(MainTabId.ROTATE, $r('app.string.rotate')))

          TabContent() {
            AdjustContentView()
          }
          .tabBar(this.TabBuilderMenu(MainTabId.ADJUST, $r('app.string.adjust')))
        }
        .scrollable(false)
        .onChange((index: number) => {
          this.currentIndex = index;
        })
      }
      .align(Alignment.End)
      .width(CommonConstants.LAYOUT_FULL_SCREEN)
      .height(CommonConstants.EDIT_PAGE_HEIGHT)
      .backgroundColor(Color.Black)
    }
    .width(CommonConstants.LAYOUT_FULL_SCREEN)
    .height(CommonConstants.LAYOUT_FULL_SCREEN)
    .backgroundColor(Color.Black)
  }

  @Builder TabBuilderMenu(index: number, name: string | Resource) {
    Column() {
      Image(this.currentIndex === index ? this.menuIconChangeList[index]?.chosen :
        this.menuIconChangeList[index]?.normal)
        .width(CommonConstants.TAB_MENU_WIDTH)
        .height(CommonConstants.TAB_MENU_WIDTH)

      Text(name)
        .fontColor(this.currentIndex === index ? Color.Blue : Color.White)
        .fontSize($r('app.float.category_font_size'))
        .margin({ top: $r('app.float.category_margin_top') })
    }
    .width(CommonConstants.LAYOUT_FULL_SCREEN)
  }

  cropImage(index: CropType) {
    this.currentCropIndex = index;
    switch (this.currentCropIndex) {
      case CropType.ORIGINAL_IMAGE:
        this.cropRatio = CropRatioType.RATIO_TYPE_FREE;
        break;
      case CropType.SQUARE:
        this.cropRatio = CropRatioType.RATIO_TYPE_1_1;
        break;
      case CropType.BANNER:
        this.cropRatio = CropRatioType.RATIO_TYPE_4_3;
        break;
      case CropType.RECTANGLE:
        this.cropRatio = CropRatioType.RATIO_TYPE_16_9;
        break;
      default:
        this.cropRatio = CropRatioType.RATIO_TYPE_FREE;
        break;
    }
  }

  rotateImage(rotateType: RotateType) {
    if (rotateType === RotateType.CLOCKWISE) {
      try {
        this.pixelMap.rotate(CommonConstants.CLOCK_WISE)
          .then(() => {
            this.flushPixelMapNew();
          })
      } catch (error) {
        Logger.error(TAG, `there is a error in rotate process with ${error?.code}`);
      }
    }
    if (rotateType === RotateType.ANTI_CLOCK) {
      try {
        this.pixelMap.rotate(CommonConstants.ANTI_CLOCK)
          .then(() => {
            this.flushPixelMapNew();
          })
      } catch (error) {
        Logger.error(TAG, `there is a error in rotate process with ${error?.code}`);
      }
    }
  }

  flushPixelMap() {
    const temp = this.pixelMap;
    this.pixelMap = undefined;
    this.pixelMap = temp;
    this.updateImageInfo();
  }

  flushPixelMapNew() {
    this.isPixelMapChange = !this.isPixelMapChange;
    this.updateImageInfo();
  }

  updateImageInfo() {
    this.pixelMap.getImageInfo().then((imageInfo) => {
      this.imageInfo = imageInfo;
      this.imageWidth = px2vp(this.imageInfo.size.width);
      this.imageHeight = px2vp(this.imageInfo.size.height);
      this.imageOldSize = {
        width: this.imageWidth,
        height: this.imageHeight
      }
      this.showCanvas = true;
      this.calcScale();
    });
  }

  calcScale() {
    let imageWidthScale = (this.screenWidth - CommonConstants.DOUBLE * CommonConstants.SCREEN_DISPLAY_MARGIN) /
    this.imageOldSize.width;
    let imageHeightScale = (this.screenHeight - CommonConstants.DOUBLE * CommonConstants.SCREEN_DISPLAY_MARGIN) /
    this.imageOldSize.height;
    this.imageScale = imageWidthScale > imageHeightScale ? imageHeightScale : imageWidthScale;
  }

  pixelInit() {
    this.showCanvas = false;
    getPixelMap(CommonConstants.RAW_FILE_NAME).then(pixelMap => {
      this.pixelMap = pixelMap;
      this.currentCropIndex = 0;
      this.currentAdjustData = CommonConstants.ADJUST_SLIDER_VALUE.map((item) => item);
      this.updateImageInfo();
      this.cropRatio = CropRatioType.RATIO_TYPE_FREE;
    });
  }

  listenCropChange() {
    this.isCrop = this.currentIndex === 0;
    if (this.isCrop) {
      this.resetShow = true;
    } else {
      this.resetShow = false;
    }
  }
}