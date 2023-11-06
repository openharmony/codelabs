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

import { HomeDialogModel } from '../viewmodel/HomeDialogModel';
import { CommonConstants } from '../common/constants/CommonConstants';
import { HomeConstants } from '../common/constants/HomeConstants';
import { HomeTabModel } from '../common/model/HomeTabModel';

@CustomDialog
export struct HomeTabContentDialog {
  private homeDialogModel?: HomeDialogModel | null = null;;
  @Consume homeTabModel: HomeTabModel;
  controller: CustomDialogController = new CustomDialogController({
    builder:HomeTabContentDialog({homeDialogModel:this.homeDialogModel!})
  });

  build() {
    Column() {
      TextInput({ placeholder: $r('app.string.link_placeholder'), text: this.homeTabModel.src })
        .height(HomeConstants.TEXT_HEIGHT)
        .width(CommonConstants.NINETY_PERCENT)
        .margin({ top: HomeConstants.TEXT_MARGIN_TOP })
        .onChange((value: string) => {
          this.homeTabModel.src = value;
        })
      TextInput({ placeholder: $r('app.string.name_placeholder'), text: this.homeTabModel.name })
        .height(HomeConstants.TEXT_HEIGHT)
        .width(CommonConstants.NINETY_PERCENT)
        .margin({ top: HomeConstants.TEXT_MARGIN_TOP })
        .onChange((value: string) => {
          this.homeTabModel.name = value;
        })
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Text(this.homeTabModel.linkCheck)
          .fontSize($r('app.float.dialog_font_size'))
          .fontColor(this.homeTabModel.loadColor)
          .onClick(() => {
            if (this.homeDialogModel!.checkSrcNull()) {
              this.homeDialogModel!.checkSrcValidity(0);
            }
          })
        Divider()
          .vertical(true)
          .height($r('app.float.tab_dialog_divider_height'))
          .color($r('app.color.divider_color'))
          .opacity($r('app.float.tab_dialog_divider_opacity'))
          .margin({
            left: $r('app.float.dialog_divider_margin_left'),
            right: $r('app.float.dialog_divider_margin_left')
          })
        Text(this.homeTabModel.confirmAdd)
          .fontSize($r('app.float.dialog_font_size'))
          .fontColor(this.homeTabModel.loadColor)
          .onClick(() => {
            if (this.homeDialogModel!.checkSrcNull() && this.homeDialogModel!.checkNameNull()) {
              this.homeDialogModel!.checkSrcValidity(1);
            }
          })
      }
      .margin({
        top: $r('app.float.dialog_column_margin_top'),
        bottom: $r('app.float.dialog_column_margin_bottom')
      })
    }
  }
}