/*
 * Copyright (c) 2026 CoinNote
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

@CustomDialog
struct AAConfirmDialog {
  controller: CustomDialogController
  confirm: (amount: number) => void = (amount: number) => {}
  @State inputValue: string = ""

  build() {
    Column() {
      Text("AA收款确认")
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 24, bottom: 16 })

      Text("请输入收到的金额（将从账单中扣除）：")
        .fontSize(14)
        .fontColor("#666666")
        .margin({ bottom: 12 })
        .width("90%")

      TextInput({ placeholder: "0.00", text: this.inputValue })
        .type(InputType.Number)
        .onChange((value: string) => {
          this.inputValue = value
        })
        .width("90%")
        .height(48)
        .backgroundColor("#F5F7FA")
        .margin({ bottom: 24 })

      Row() {
        Button("取消")
          .onClick(() => {
            this.controller.close()
          })
          .backgroundColor("#F5F7FA")
          .fontColor("#666666")
          .layoutWeight(1)
          .margin({ right: 12 })
        
        Button("确认")
          .onClick(() => {
            const val = parseFloat(this.inputValue);
            if (!isNaN(val) && val >= 0) {
              this.confirm(val);
              this.controller.close();
            } else {
              promptAction.showToast({ message: '请输入有效金额' });
            }
          })
          .backgroundColor("#4A6CF7")
          .fontColor(Color.White)
          .layoutWeight(1)
      }
      .width("90%")
      .margin({ bottom: 24 })
    }
    .backgroundColor(Color.White)
    .borderRadius(16)
    .width("80%")
  }
}
