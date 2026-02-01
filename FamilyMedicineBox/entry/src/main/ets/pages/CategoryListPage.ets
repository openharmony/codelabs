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
struct CategoryListPage {
  @State categoryName: string = '';
  @State medicineList: MedicineItem[] = [];

  aboutToAppear() {
    const params = router.getParams() as Record<string, string>;
    if (params && params['category']) {
      this.categoryName = params['category'];
      this.medicineList = MockData.getMedicinesByCategory(this.categoryName);
    }
  }

  build() {
    Column() {
      // 导航栏
      Row() {
        Button('<')
          .onClick(() => router.back())
          .backgroundColor(Color.Transparent)
          .fontColor(Color.Black)
          .fontSize(24)

        Text(this.categoryName)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 10 })
      }
      .width('100%')
      .padding(16)

      // 列表
      List({ space: 10 }) {
        if (this.medicineList.length === 0) {
          ListItem() {
            Column() {
              Text('暂无该分类药品').fontSize(16).fontColor('#999')
            }
            .width('100%')
            .margin({ top: 50 })
          }
        } else {
          ForEach(this.medicineList, (item: MedicineItem) => {
            ListItem() {
              Row() {
                // 修复点：使用 Stack 替代 overlay
                Stack({ alignContent: Alignment.Center }) {
                  Circle({ width: 44, height: 44 }).fill('#F0F4FF')
                  Text(item.id.toString())
                    .fontColor('#4A90E2')
                    .fontWeight(FontWeight.Bold)
                }
                .margin({ right: 12 })

                Column() {
                  Row() {
                    Text(item.name).fontSize(16).fontWeight(FontWeight.Medium)
                    Text(item.category)
                      .fontSize(10)
                      .fontColor(Color.White)
                      .backgroundColor('#FFB74D') // Orange tag
                      .borderRadius(4)
                      .padding({
                        left: 6,
                        right: 6,
                        top: 2,
                        bottom: 2
                      })
                      .margin({ left: 8 })
                  }

                  Text('有效期至: ' + item.expiryDate)
                    .fontSize(13)
                    .fontColor('#888888')
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
      }
      .width('100%')
      .layoutWeight(1)
      .padding(16)
      .backgroundColor('#F5F6FA')
    }
    .height('100%')
  }
}