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
import screenshot from '@ohos.screenshot';
import window from '@ohos.window';
import Logger from '../common/utils/Logger';
import ScreenshotDialog from './ScreenshotDialog';
import drawUtil from '../common/utils/DrawUtil';
import offsetModel from '../model/OffsetModel';
import CommonConstants from '../common/utils/CommonConstaint';

/**
 * Custom component to determine the screenshot area.
 */
@Component
export default struct AreaScreenShot {
  @Link showScreen: boolean;
  @Link pixelMap: image.PixelMap;
  @Link scaleNum: Object;
  private screenAspectRatio: number = 0;
  dialogController: CustomDialogController = new CustomDialogController({
    builder: ScreenshotDialog({
      pixelMap: this.pixelMap,
      screenAspectRatio: this.screenAspectRatio
    }),
    alignment: DialogAlignment.BottomEnd,
    customStyle: true
  });
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private canvasRenderingContext: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private systemBarHeight: number = 0;

  aboutToAppear() {
    window.getLastWindow(getContext(this))
      .then((window) => {
        let property = window.getWindowProperties();
        this.systemBarHeight = property.windowRect.top;

        drawUtil.initDrawUtil(
          this.canvasRenderingContext,
          px2vp(property.windowRect.width),
          px2vp(property.windowRect.height)
        );

        offsetModel.initOffsetModel(
          px2vp(property.windowRect.width),
          px2vp(property.windowRect.height)
        );

        this.screenAspectRatio = px2vp(property.windowRect.height) / px2vp(property.windowRect.width);
      })
      .catch((err) => {
        Logger.error(`window loading has error: ${ JSON.stringify(err) }`);
      })
  }

  private resetParameter() {
    this.showScreen = false;
    this.scaleNum = {
      x: CommonConstants.NO_SCALE_DOWN,
      y: CommonConstants.NO_SCALE_DOWN
    };
    offsetModel.resetDefaultOffSet();
  }

  build() {
    if (this.showScreen) {
      Stack() {
        Canvas(this.canvasRenderingContext)
          .width(CommonConstants.FULL_PARAM)
          .height(CommonConstants.FULL_PARAM)
          .onReady(() => {
            drawUtil.draw();
          })

        Row() {
          Row() {
            Image($r('app.media.ic_back'))
              .height($r('app.float.toolbar_item_height'))
              .onClick(() => {
                this.resetParameter();
              })
            Button($r('app.string.cancel_areaScreenshot'))
              .backgroundColor($r('app.color.toolbar_color'))
              .fontSize($r('app.float.cancel_font_size'))
              .fontColor($r('app.color.cancel_font_color'))
              .onClick(() => {
                this.resetParameter();
              })
          }
          .backgroundColor($r('app.color.toolbar_color'))

          Image($r('app.media.ic_save'))
            .onClick(() => {
              let ScreenshotOptions = {
                screenRect: {
                  left: vp2px(offsetModel.getXLeft()),
                  top: vp2px(offsetModel.getYTop()) + this.systemBarHeight,
                  width: vp2px(offsetModel.getWidth()),
                  height: vp2px(offsetModel.getHeight())
                },
                imageSize: {
                  width: vp2px(offsetModel.getWidth()),
                  height: vp2px(offsetModel.getHeight())
                },
                rotation: 0,
                displayId: 0
              };
              screenshot.save(ScreenshotOptions, (err, data: image.PixelMap) => {
                if (err) {
                  Logger.error(`Failed to save the screenshot. Error:${JSON.stringify(err)}`);
                }
                if (this.pixelMap !== undefined) {
                  this.pixelMap.release();
                }
                this.pixelMap = data;
                this.dialogController.open();
              });
              this.resetParameter();
            })
            .height($r('app.float.toolbar_item_height'))
        }
        .backgroundColor($r('app.color.toolbar_color'))
        .width(CommonConstants.TOOLBAR_WIDTH)
        .justifyContent(FlexAlign.SpaceBetween)
      }
      .width(CommonConstants.FULL_PARAM)
      .height(CommonConstants.FULL_PARAM)
      .alignContent(Alignment.Top)
      .onTouch((event: TouchEvent) => {
        switch(event.type) {
          case TouchType.Down:
            offsetModel.setXLocationType(event.touches[0].screenX);
            offsetModel.setYLocationType(event.touches[0].screenY);
            break;
          case TouchType.Move:
            offsetModel.resetOffsetXY(event.touches[0].screenX, event.touches[0].screenY);
            drawUtil.draw();
            break;
          default:
            break;
        }
      })
    }
  }
}