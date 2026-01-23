/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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
// src/main/ets/view/form/TextInputItem.ets
import { StyleConstants } from '../../common/constants/StyleConstants';

/**
 * 封装的输入框组件
 * 带标题、输入框、下划线
 */
@Component
export struct TextInputItem {
  @Prop title: string | Resource = '';
  @Prop placeholder: string | Resource = '';
  @Link text: string;
  @Prop inputType: InputType = InputType.Normal;
  /** 是否是密码模式 */
  @Prop isPassword: boolean = false;

  build() {
    Column() {
      // 标题
      Text(this.title)
        .fontSize(StyleConstants.FONT_SIZE_MD)
        .fontColor(StyleConstants.COLOR_TEXT_SECONDARY)
        .margin({ bottom: StyleConstants.SPACE_XS })
        .width('100%')

      // 输入框容器
      Row() {
        TextInput({
          text: this.text,
          placeholder: this.placeholder
        })
          .type(this.inputType)
          .fontSize(StyleConstants.FONT_SIZE_LG)
          .fontColor(StyleConstants.COLOR_TEXT_PRIMARY)
          .placeholderColor(StyleConstants.COLOR_TEXT_SECONDARY)
          .backgroundColor(Color.Transparent)
          .padding({ left: 0 })
          .height(StyleConstants.HEIGHT_INPUT)
          .width('100%')
          .showPasswordIcon(this.isPassword)
          .onChange((value) => {
            this.text = value;
          })
      }
      .width('100%')
      .border({
        width: { bottom: 1 },
        color: StyleConstants.COLOR_DIVIDER
      })
    }
    .width('100%')
    .padding({
      top: StyleConstants.SPACE_MD,
      bottom: StyleConstants.SPACE_MD
    })
  }
}