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

import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import PermissionUtil from '../common/util/PermissionUtil';
import { NoPermissionDialog } from '../view/NoPermissionDialog';
import { DeviceListDialog } from '../view/DeviceListDialog';
import { CommonConstants } from '../common/constants/CommonConstants';

@Entry
@Component
struct HomePage {
  @State authorizedImage: Resource = $r('app.media.ic_unauthorized');
  @State permissionDescribe: Resource = $r('app.string.no_permission');
  noPermissionDialog: CustomDialogController = new CustomDialogController({
    builder: NoPermissionDialog(),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
    offset: {
      dx: CommonConstants.DIALOG_OFFSET_X,
      dy: CommonConstants.DIALOG_OFFSET_Y
    }
  });
  deviceListDialog: CustomDialogController = new CustomDialogController({
    builder: DeviceListDialog(),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
    offset: {
      dx: CommonConstants.DIALOG_OFFSET_X,
      dy: CommonConstants.DIALOG_OFFSET_Y
    }
  })

  async aboutToAppear() {
    let result = await PermissionUtil.applyPermission();
    if (result === 0) {
      this.permissionDescribe = $r('app.string.has_permission');
      this.authorizedImage = $r('app.media.ic_authorized');
    } else {
      this.permissionDescribe = $r('app.string.no_permission');
    }
  }

  build() {
    Column() {
      Image(this.authorizedImage)
        .width(CommonConstants.FULL_PERCENT)
        .height(CommonConstants.HOME_IMAGE_HEIGHT)
      Text(this.permissionDescribe)
        .fontSize($r('app.float.home_text_size'))
        .fontWeight(FontWeight.Normal)
        .fontColor($r('app.color.home_text_color'))
      Column() {
        Button($r('app.string.button_text'), { type: ButtonType.Capsule, stateEffect: true })
          .width(CommonConstants.HOME_BUTTON_WIDTH)
          .height($r('app.float.home_button_height'))
          .backgroundColor($r('app.color.home_button_background_color'))
          .fontSize($r('app.float.home_button_size'))
          .margin({ bottom: $r('app.float.home_button_bottom') })
          .onClick(async () => {
            let result = await PermissionUtil.checkPermission();
            if(result === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
              this.deviceListDialog.open();
            } else {
              this.noPermissionDialog.open();
            }
          })
      }
      .flexGrow(1)
      .justifyContent(FlexAlign.End)
    }
    .width(CommonConstants.FULL_PERCENT)
    .height(CommonConstants.FULL_PERCENT)
  }
}
