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

import screenshot from '@ohos.screenshot';
import image from '@ohos.multimedia.image';
import AreaScreenshot from '../view/AreaScreenshot';
import Logger from '../common/utils/Logger';
import ScreenshotDialog from '../view/ScreenshotDialog';
import CommonConstant from '../common/utils/CommonConstaint';
import CommonConstants from '../common/utils/CommonConstaint';

@Entry
@Component
struct GestureScreenshot {
  @State pixelMap: image.PixelMap = undefined;
  @State showScreen: boolean = false;
  @State scaleNum: Object = {
    x: CommonConstant.NO_SCALE_DOWN,
    y: CommonConstant.NO_SCALE_DOWN
  };

  dialogController: CustomDialogController = new CustomDialogController({
    builder: ScreenshotDialog({ pixelMap: this.pixelMap }),
    alignment: DialogAlignment.BottomEnd,
    customStyle: true
  });

  build() {
    Stack() {
      Column() {
        Text($r('app.string.title_content'))
          .fontSize($r('app.float.title_font_size'))
          .fontWeight(CommonConstant.TITLE_FONT_WEIGHT)
          .width(CommonConstants.FULL_PARAM)
          .textAlign(TextAlign.Start)
        Image($r('app.media.ic_mountain'))
          .width(CommonConstants.FULL_PARAM)
          .margin({
            top: $r('app.float.image_margin_size'),
            bottom: $r('app.float.image_margin_size')
          })
        Text($r('app.string.text_context'))
          .fontSize($r('app.float.text_font_size'))
          .lineHeight($r('app.float.text_height'))
      }
      .backgroundColor($r('app.color.main_area'))
      .width(CommonConstant.FULL_PARAM)
      .height(CommonConstant.FULL_PARAM)
      .padding({
        left: $r('app.float.main_margin_size'),
        right: $r('app.float.main_margin_size')
      })
      .gesture(
        // fingers：The number of Trigger Fingers,direction：Triggering direction,distance:Trigger sliding distance.
        PanGesture({ fingers: 1, direction: PanDirection.Down, distance: CommonConstants.MINIMUM_FINGER_DISTANCE })
          // Trigger Start Callback.
          .onActionStart(() => {
            // Screenshot Parameters and APIs.
            let ScreenshotOptions = {
              rotation: 0
            };
            screenshot.save(ScreenshotOptions, (err, data: image.PixelMap) => {
              if (err) {
                Logger.error(`Failed to save the screenshot. Error:${ JSON.stringify(err) }`);
              }
              if (this.pixelMap !== undefined) {
                this.pixelMap.release();
              }
              this.pixelMap = data;
              this.dialogController.open();
            });
          })
      )
      .scale(this.scaleNum)

      AreaScreenshot({ showScreen: $showScreen, pixelMap: $pixelMap, scaleNum: $scaleNum })
    }
    .backgroundColor($r('app.color.black_area'))
    .gesture(
      TapGesture({ count: 2 })
        .onAction(() => {
          this.showScreen = true;
          this.scaleNum = {
            x: CommonConstant.X_SCALE_DOWN,
            y: CommonConstant.Y_SCALE_DOWN
          }
        })
    )
  }
}