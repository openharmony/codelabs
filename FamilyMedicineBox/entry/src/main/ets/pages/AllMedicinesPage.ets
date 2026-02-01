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

iimport
router
from
'@ohos.router';
import { MedicineItem, MockData } from '../model/MockData';

@Entry
@Component
struct AllMedicinesPage {
  @State allList: MedicineItem[] = [];
  @State searchKeyword: string = '';

  // 每次显示页面时刷新列表 (确保添加或删除后列表同步)
  onPageShow() {
    this.refreshList();
  }

  refreshList() {
    if (this.searchKeyword === '') {
      this.allList = MockData.getAllMedicines();
    } else {
      this.allList = MockData.getAllMedicines().filter(item => item.name.includes(this.searchKeyword));
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Button({ type: ButtonType.Circle }) {
          Image($r('app.media.startIcon'))
            .width(20).height(20)
            .rotate({ angle: 180 })
        }
        .width(32)
        .height(32)
        .backgroundColor('#F0F0F0')
        .onClick(() => router.back())
        .margin({ right: 10 })

        Text('库存总览')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
      .padding({
        left: 16,
        right: 16,
        top: 10,
        bottom: 10
      })

      // 搜索框
      Row() {
        Image($r('app.media.startIcon')).width(18).height(18).margin({ right: 8 })
        TextInput({ placeholder: '搜索库存药品...', text: this.searchKeyword })
          .layoutWeight(1)
          .backgroundColor(Color.Transparent)
          .onChange((value: string) => {
            this.searchKeyword = value;
            this.refreshList();
          })
      }
      .width('92%')
      .height(46)
      .backgroundColor('#F5F6FA')
      .borderRadius(12)
      .padding({ left: 12 })
      .margin({ bottom: 16 })

      // 药品列表
      List({ space: 12 }) {
        ForEach(this.allList, (item: MedicineItem) => {
          ListItem() {
            Row() {
              // 左侧图标
              Stack({ alignContent: Alignment.Center }) {
                Circle({ width: 44, height: 44 }).fill('#E3F2FD')
                Text(item.name[0])
                  .fontSize(18)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#1976D2')
              }
              .margin({ right: 12 })

              // 中间信息
              Column() {
                Row() {
                  Text(item.name).fontSize(16).fontWeight(FontWeight.Bold)
                  Text(item.category)
                    .fontSize(10)
                    .fontColor(Color.White)
                    .backgroundColor(item.category === '感冒发烧' ? '#FFB74D' : '#64B5F6')
                    .padding({
                      left: 6,
                      right: 6,
                      top: 2,
                      bottom: 2
                    })
                    .borderRadius(4)
                    .margin({ left: 8 })
                }
                .alignItems(VerticalAlign.Center)

                Text('有效期至: ' + item.expiryDate)
                  .fontSize(13)
                  .fontColor('#888888')
                  .margin({ top: 4 })
              }
              .layoutWeight(1)
              .alignItems(HorizontalAlign.Start)

              // 右侧箭头
              Text('>')
                .fontSize(18)
                .fontColor('#CCCCCC')
            }
            .width('100%')
            .padding(16)
            .backgroundColor(Color.White)
            .borderRadius(16)
            .onClick(() => {
              // 点击跳转到详情页，并传递药品ID
              router.pushUrl({
                url: 'pages/MedicineDetailPage',
                params: { id: item.id }
              });
            })
          }
        })
      }
      .layoutWeight(1)
      .width('100%')
      .padding({ left: 16, right: 16 })
      .backgroundColor('#F5F6FA')
    }
    .height('100%')
    .backgroundColor(Color.White)
  }
}