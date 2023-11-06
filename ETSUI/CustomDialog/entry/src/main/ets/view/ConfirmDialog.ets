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

import StyleConstants from '../common/constants/StyleConstants';

@CustomDialog
export default struct ConfirmDialog {
  controller?: CustomDialogController;
  cancel: () => void = () => {
  };
  confirm: () => void = () => {
  };

  build() {
    Column() {
      Image($r('app.media.icon'))
        .width($r('app.float.image_size'))
        .height($r('app.float.image_size'))
        .margin({ top: $r('app.float.icon_margin_top') })
      Text($r('app.string.custom_dialog_message'))
        .fontSize($r('app.float.font_size_normal'))
        .margin({ top: $r('app.float.dialog_message_margin_top') })
      Flex({ justifyContent: FlexAlign.SpaceAround, alignItems: ItemAlign.Center }) {
        Button($r('app.string.cancel_txt'))
          .onClick(() => {
            if (!this.controller) {
              return;
            }
            this.controller.close();
            this.cancel();
          })
          .backgroundColor($r('app.color.dialog_button_background_color'))
          .fontColor($r('app.color.font_color_blue'))
          .width($r('app.float.dialog_button_width'))
          .height($r('app.float.dialog_button_height'))
          .margin({ left: $r('app.float.dialog_button_margin_left') })
        Divider()
          .vertical(true)
          .height($r('app.float.diver_height'))
        Button($r('app.string.custom_dialog_confirm_txt'))
          .onClick(() => {
            if (!this.controller) {
              return;
            }
            this.controller.close();
            this.confirm();
          })
          .backgroundColor($r('app.color.dialog_button_background_color'))
          .fontColor($r('app.color.font_color_red'))
          .width($r('app.float.dialog_button_width'))
          .height($r('app.float.dialog_button_height'))
          .margin({ right: $r('app.float.dialog_button_margin_right') })
      }
      .margin({ top: $r('app.float.dialog_button_margin_top') })
    }
    .width(StyleConstants.CUSTOM_DIALOG_WIDTH)
    .height(StyleConstants.CUSTOM_DIALOG_HEIGHT)
    .backgroundColor($r('app.color.start_window_background'))
    .borderRadius($r('app.float.dialog_border_radius'))
  }
}