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
import ConfirmDialog from '../view/ConfirmDialog';
import CustomAlertDialog from '../view/CustomAlertDialog';
import Logger from '../common/utils/Logger';

@Entry
@Component
struct DialogPage {
  dialogControllerExample: CustomDialogController = new CustomDialogController({
    builder: ConfirmDialog({ cancel: this.onCancel, confirm: this.onAccept }),
    cancel: this.existApp,
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    customStyle: true,
    offset: { dx: $r('app.float.dialog_offset_x'), dy: $r('app.float.dialog_offset_y') }
  });
  dialogControllerAlert: CustomDialogController = new CustomDialogController({
    builder: CustomAlertDialog({ cancel: this.onCancel, confirm: this.onAccept }),
    cancel: this.existApp,
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    customStyle: true,
    offset: { dx: $r('app.float.dialog_offset_x'), dy: $r('app.float.dialog_offset_y') }
  });

  onCancel() {
    Logger.info('Callback when the cancel button is clicked');
  }

  onAccept() {
    Logger.info('Callback when the confirm button is clicked');
  }

  existApp() {
    Logger.info('Click the callback in the blank area');
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Button($r('app.string.one_button_dialog'))
        .onClick(() => {
          AlertDialog.show(
            {
              message: $r('app.string.dialog_message'),
              offset: { dx: $r('app.float.dialog_offset_x'), dy: $r('app.float.dialog_offset_y') },
              alignment: DialogAlignment.Bottom,
              confirm: {
                value: $r('app.string.confirm_txt'),
                action: () => {
                  Logger.info('Button clicking callback');
                }
              },
              cancel: () => {
                Logger.info('Closed callbacks');
              }
            }
          );
        })
        .height(StyleConstants.BUTTON_HEIGHT)
        .width(StyleConstants.BUTTON_WIDTH)
      Button($r('app.string.two_button_dialog'))
        .onClick(() => {
          this.dialogControllerAlert.open();
        })
        .margin({ top: $r('app.float.button_margin_top') })
        .height(StyleConstants.BUTTON_HEIGHT)
        .width(StyleConstants.BUTTON_WIDTH)
      Button($r('app.string.customization_dialog'))
        .onClick(() => {
          this.dialogControllerExample.open();
        })
        .margin({ top: $r('app.float.button_margin_top') })
        .height(StyleConstants.BUTTON_HEIGHT)
        .width(StyleConstants.BUTTON_WIDTH)
    }
    .width(StyleConstants.FULL_PERCENT)
    .height(StyleConstants.FULL_PERCENT)
  }
}