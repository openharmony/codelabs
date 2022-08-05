/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

@Component
export struct InputItem {
  @Link inputValue: string
  private placeholderName
  private typeFlag
  private imageRes
  private inputType

  build() {
    Stack({ alignContent: Alignment.Center }) {
      TextInput({ placeholder: this.placeholderName })
        .type(this.typeFlag ? InputType.Password : InputType.Normal)
        .placeholderColor(Color.Gray)
        .fontSize(30)
        .caretColor(Color.Gray)
        .width('60%')
        .height('55%')
        .maxLength(14)
        .margin({ left: '8%' })
        .onChange((value) => {
          this.inputValue = this.getReturnType(this.inputType) + ':' + value.trim();
        });

      // 输入框左边的小图标
      Image(this.imageRes)
        .objectFit(ImageFit.Contain)
        .height('33%')
        .offset({ x: '-30%', y: '0%' })
    }
    .height('10%')
    .width('100%')
  }

  // 获取输入框的返回类型，用来区分输入框中输入的值是用户名还是密码
  getReturnType(inputType) {
    return inputType;
  }
}