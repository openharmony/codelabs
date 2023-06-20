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

import { CommonConstants } from '../common/constants/CommonConstants';

@CustomDialog
export struct NoPermissionDialog {
  controller: CustomDialogController;

  build() {
    Column() {
      Text($r('app.string.no_permission_title'))
        .width(CommonConstants.PERMISSION_TEXT_WIDTH)
        .fontSize($r('app.float.permission_text_size'))
        .fontWeight(FontWeight.Bold)
        .fontColor(Color.Black)
        .textAlign(TextAlign.Start)
        .margin({ top: $r('app.float.permission_text_top') })
      Text($r('app.string.clear_permission'))
        .width(CommonConstants.PERMISSION_TEXT_WIDTH)
        .fontSize($r('app.float.permission_text_size'))
        .fontWeight(FontWeight.Normal)
        .fontColor($r('app.color.permission_text_color'))
        .textAlign(TextAlign.Start)
        .margin({ top: $r('app.float.permission_clear_permission_text_top') })
      Text($r('app.string.dialog_confirm'))
        .fontSize($r('app.float.permission_text_size'))
        .fontColor($r('app.color.permission_confirm_color'))
        .margin({
          top: $r('app.float.dialog_text_confirm_top'),
          bottom: $r('app.float.dialog_text_confirm_bottom')
        })
        .onClick(() => {
          this.controller.close();
        })
    }
    .width(CommonConstants.FULL_PERCENT)
    .backgroundColor(Color.White)
    .borderRadius($r('app.float.border_radius'))
  }
}