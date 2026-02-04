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
import { Contact } from '../model/Contact';
import RdbStore from '../utils/RdbStore';

@Entry
@Component
struct MedicineList {
  @State type: string = '';
  @State medicineList: Array<Contact> = [];

  async aboutToAppear() {
    const params = router.getParams() as Record<string, string>;
    this.type = params?.type ?? '';
    await this.loadData();
  }

  async loadData() {
    let allList = await RdbStore.queryAll(1);
    this.medicineList = allList.filter(item => item.company === this.type);
  }

  build() {
    Column() {
      Row() {
        Image($r('app.media.startIcon'))
          .width(24)
          .height(24)
          .margin({ left: 16 })
          .onClick(() => {
            router.back();
          })
        Text(this.type)
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 16 })
      }
      .width('100%')
      .height(56)
      .alignItems(VerticalAlign.Center)
      .backgroundColor(Color.White)

      if (this.medicineList.length === 0) {
        Column() {
          Text('暂无药品')
            .fontSize(16)
            .fontColor('#999999')
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List() {
          ForEach(this.medicineList, (item: Contact) => {
            ListItem() {
              Row() {
                Stack({ alignContent: Alignment.Center }) {
                  Circle({ width: 40, height: 40 })
                    .fill('#E3F2FD')
                  Text(item.name.substring(0, 1))
                    .fontSize(18)
                    .fontColor('#4A90E2')
                    .fontWeight(FontWeight.Bold)
                }
                .margin({ right: 12 })

                Column() {
                  Row() {
                    Text(item.name)
                      .fontSize(16)
                      .fontWeight(FontWeight.Medium)
                      .fontColor('#333333')

                    Text(item.company)
                      .fontSize(12)
                      .fontColor(Color.White)
                      .backgroundColor('#FF9500')
                      .borderRadius(4)
                      .padding({
                        left: 6,
                        right: 6,
                        top: 2,
                        bottom: 2
                      })
                      .margin({ left: 8 })
                  }
                  .width('100%')

                  Text(`有效期至: ${item.phone}`)
                    .fontSize(14)
                    .fontColor('#999999')
                    .margin({ top: 4 })
                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start)
              }
              .width('100%')
              .padding(15)
              .backgroundColor(Color.White)
              .borderRadius(12)
              .margin({ bottom: 10 })
            }
            .onClick(() => {
              router.pushUrl({
                url: 'pages/ContactDetailPage',
                params: { contactId: item.id }
              });
            })
          }, (item: Contact) => JSON.stringify(item))
        }
        .width('92%')
        .layoutWeight(1)
        .margin({ top: 10, left: '4%', right: '4%' })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F7F8FA')
  }
}
