/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import router from '@ohos.router';
import { MedicineItem, MockData } from '../model/MockData';

@Entry
@Component
struct ExpiryWarningPage {
  @State expiredList: MedicineItem[] = [];

  aboutToAppear() {
    this.expiredList = MockData.getExpiredOrNearMedicines();
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Button({ type: ButtonType.Circle }) {
          Text('<').fontSize(20).fontWeight(FontWeight.Bold)
        }
        .width(40).height(40)
        .backgroundColor('#F5F5F5')
        .onClick(() => router.back())

        Text('临期/过期预警')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 16 })
      }
      .width('100%')
      .padding(16)

      // 警告横幅
      Row() {
        Text('⚠️ 以下药品即将过期或已过期，请及时处理')
          .fontColor('#D9534F')
          .fontSize(14)
      }
      .width('100%')
      .padding(10)
      .backgroundColor('#FFECEC')

      // 列表
      List({ space: 12 }) {
        ForEach(this.expiredList, (item: MedicineItem) => {
          ListItem() {
            Row() {
              // 修复点：使用 Stack 替代 overlay
              Stack({ alignContent: Alignment.Center }) {
                Circle({ width: 40, height: 40 })
                  .fill(item.isExpired ? '#FFEBEB' : '#E3F2FD')
                Text(item.name.substring(0, 1))
                  .fontSize(16)
                  .fontColor(item.isExpired ? 'Red' : 'Blue')
              }
              .margin({ right: 12 })

              Column() {
                Row() {
                  Text(item.name).fontSize(16).fontWeight(FontWeight.Bold)
                  Text(item.category)
                    .fontSize(10)
                    .fontColor(Color.White)
                    .backgroundColor('#FFA726')
                    .padding({
                      left: 4,
                      right: 4,
                      top: 2,
                      bottom: 2
                    })
                    .borderRadius(4)
                    .margin({ left: 8 })
                }
                .alignItems(VerticalAlign.Center)

                Text('有效期至: ' + item.expiryDate)
                  .fontSize(14)
                  .fontColor('#999999')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)
            }
            .width('100%')
            .padding(16)
            .backgroundColor(Color.White)
            .borderRadius(12)
          }
        })
      }
      .layoutWeight(1)
      .padding(16)
      .width('100%')
      .backgroundColor('#F5F6FA')
    }
    .height('100%')
    .backgroundColor(Color.White)
  }
}