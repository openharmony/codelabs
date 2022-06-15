/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import {GoodsData} from '../model/GoodsData'
import {initializeOnStartup} from '../model/GoodsDataModels'
import prompt from '@system.prompt';

@Entry
@Component
 export struct ShoppingCart {
  @Provide totalPrice: number = 0
  private goodsItems: GoodsData[] = initializeOnStartup()

  build() {
    Column() {
      ShopCartList({ goodsItems: this.goodsItems });
      ShopCartBottom()
    }
    .height('100%')
    .width('100%')
    .alignItems(HorizontalAlign.Start)
  }
}

@Component
struct ShopCartList {
  private goodsItems: GoodsData[]

  build() {
    Column() {
      List() {
        ForEach(this.goodsItems, item => {
          ListItem() {
            ShopCartListItem({ goodsItem: item })
          }
        }, item => item.id.toString())
      }
      .height('100%')
      .width('100%')
      .align(Alignment.Top)
      .margin({ top: 5 })
    }
    .height('95%')
  }
}

@Component
struct ShopCartListItem {
  @Consume totalPrice: number
  private goodsItem: GoodsData

  build() {
    Row() {
      Toggle({ type: ToggleType.Checkbox })
        .width(13)
        .height(13)
        .onChange((isOn: boolean) => {
        if (isOn) {
          this.totalPrice += parseInt(this.goodsItem.price + '', 0)
        } else {
          this.totalPrice -= parseInt(this.goodsItem.price + '', 0)
        }
      })
      Image(this.goodsItem.imgSrc)
        .objectFit(ImageFit.ScaleDown)
        .height(130)
        .width(100)
        .renderMode(ImageRenderMode.Original)
      Column() {
        Text(this.goodsItem.title)
          .fontSize(18)
        Text('￥' + this.goodsItem.price)
          .fontSize(18)
          .fontColor(Color.Red)
      }
      .margin({left:40})
    }
    .height(100)
    .width('100%')
//    .margin({ left: 20 })
    .alignItems(VerticalAlign.Center)
    .backgroundColor(Color.White)
  }
}

@Component
struct ShopCartBottom {
  @Consume totalPrice: number

  build() {
    Flex({direction:FlexDirection.Row,wrap:FlexWrap.NoWrap,justifyContent:FlexAlign.SpaceBetween}) {
      Text('Total:  ￥' + this.totalPrice)
        .fontColor(Color.Red)
        .fontSize(18)
        .margin({ left: 20 })
        .width(150)
      Text('Check Out')
        .fontColor(Color.Black)
        .fontSize(18)
        .margin({ right: 20 })
        .onClick(() => {
        prompt.showToast({
          message: 'Checking Out',
          duration: 10,
          bottom: 100
        })
      })
    }
    .height(30)
    .width('100%')
    .backgroundColor('#FF7FFFD4')
  }
}
