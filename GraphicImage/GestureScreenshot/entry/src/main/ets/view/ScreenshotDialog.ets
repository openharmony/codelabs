/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import CommonConstants from '../common/utils/CommonConstaint';

@CustomDialog
export default struct ScreenshotDialog {
  @State dialogWidth: string = CommonConstants.WIDTH_FIRST;
  controller: CustomDialogController;
  private pixelMap: PixelMap = null;
  private screenAspectRatio: number = -1;

  aboutToAppear() {
    this.getDialogWidth();
  }

  build() {
    Column() {
      Row() {
        Image($r('app.media.ic_close'))
          .height($r('app.float.close_height'))
          .onClick(() => {
            this.controller.close();
          })
      }
      .width(CommonConstants.FULL_PARAM)
      .justifyContent(FlexAlign.End)
      Row() {
        Image(this.pixelMap)
          .width(CommonConstants.IMAGE_ON_DIALOG)
          .margin(CommonConstants.IMAGE_MARGIN)
          .objectFit(ImageFit.Contain)
          .borderRadius($r('app.float.image_border_radius'))
      }
      .backgroundColor($r('app.color.image_area_background'))
      .borderRadius($r('app.float.image_border_radius'))
      .width(CommonConstants.FULL_PARAM)
    }
    .backgroundColor($r('app.color.dialog_background'))
    .margin(CommonConstants.SCREENSHOT_DIALOG_MARGIN)
    .width(this.dialogWidth)
  }

  private async getDialogWidth() {
    let info = await this.pixelMap.getImageInfo();
    let pixelMapAspectRatio = info.size.height / info.size.width;

    if ((this.screenAspectRatio !== -1) && (pixelMapAspectRatio > this.screenAspectRatio)) {
      let width = CommonConstants.HEIGHT_FIRST / pixelMapAspectRatio * this.screenAspectRatio;
      this.dialogWidth = width + '%';
    } else {
      this.dialogWidth = CommonConstants.WIDTH_FIRST;
    }
  }
}