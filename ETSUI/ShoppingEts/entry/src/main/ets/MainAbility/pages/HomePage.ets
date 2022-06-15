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
import { GoodsData, IconImage } from '../model/GoodsData'
import { initializeOnStartup, getIconPath, getIconPathSelect } from '../model/GoodsDataModels'
import { ShoppingCart } from './ShoppingCartPage'
import { MyInfo } from './MyPage'
import router from '@system.router';

@Entry
@Component
struct Index {
  @Provide currentPage: number = 1
  private goodsItems: GoodsData[] = initializeOnStartup()
  @State Build: Array<Object> = [
    {
      icon: $r('app.media.icon_home'),
      icon_after: $r('app.media.icon_buy1'),
      text: '首页',
      num: 0
    },
    {
      icon: $r('app.media.icon_shopping_cart'),
      icon_after: $r('app.media.icon_shopping_cart_select'),
      text: '购物车',
      num: 1
    },
    {
      icon: $r('app.media.icon_my'),
      icon_after: $r('app.media.icon_my_select'),
      text: '我的',
      num: 2
    }
  ]

  @Builder NavigationToolbar() {
    Flex({direction:FlexDirection.Row,wrap:FlexWrap.NoWrap,justifyContent:FlexAlign.SpaceAround}) {
      ForEach(this.Build, item => {
        Column() {
          Image(this.currentPage == item.num ? item.icon_after : item.icon)
            .width(35)
            .height(35)
            .margin({bottom:2,top:2})
          Text(item.text)
            .fontColor(this.currentPage == item.num ? "#ff7500" : "#000000")
            .fontSize(15)
            .margin({bottom:5})
        }
        .height(80)
        .onClick(() => {
          this.currentPage = item.num
        })
      })
    }
  }

  build() {
    Column() {
      Navigation() {
        Flex() {
          if (this.currentPage == 0) {
            GoodsHome({ goodsItems: this.goodsItems })
          }
          if (this.currentPage == 1) {
            ShoppingCart() //购物车列表
          }
          if (this.currentPage == 2) {
             MyInfo() //我的
          }
        }
        .width('100%')
        .height('100%')
      }
      .toolBar(this.NavigationToolbar)
      .title("购物车")
      .hideTitleBar(this.currentPage == 1 ? false : true)
      .hideBackButton(true)
    }
  }
}

@Component
struct GoodsList {
  private goodsItems: GoodsData[]

  build() {
    Column() {
      List() {
        ForEach(this.goodsItems, item => {
          ListItem() {
            GoodsListItem({ goodsItem: item })
          }
        }, item => item.id.toString())
      }
      .height('100%')
      .width('100%')
      .align(Alignment.Top)
      .margin({ top: 10 })
    }
  }
}

@Component
struct GoodsHome {
  private goodsItems: GoodsData[]

  build() {
    Column() {
      Tabs() {
        TabContent() {
          GoodsList({ goodsItems: this.goodsItems });
        }
        .tabBar("Top Sellers")
        .backgroundColor(Color.White)

        TabContent() {
          GoodsList({ goodsItems: this.goodsItems });
        }
        .tabBar("Recommended")
        .backgroundColor(Color.White)

        TabContent() {
          GoodsList({ goodsItems: this.goodsItems });
        }
        .tabBar("Lifestyle")
        .backgroundColor(Color.White)

        TabContent() {
          GoodsList({ goodsItems: this.goodsItems });
        }
        .tabBar("Deals")
        .backgroundColor(Color.White)
      }
      .barWidth(540)
      .barHeight(50)
      .scrollable(true)
      .barMode(BarMode.Scrollable)
      .backgroundColor('#007DFF')
      .height('100%')
    }
    .alignItems(HorizontalAlign.Start)
  }
}

@Component
struct GoodsListItem {
  private goodsItem: GoodsData

  build() {
    Navigator({ target: 'pages/ShoppingDetail' }) {
      Row() {
        Column() {
          Text(this.goodsItem.title)
            .fontSize(18)
          Text(this.goodsItem.content)
            .fontSize(14)
          Text('￥' + this.goodsItem.price)
            .fontSize(18)
            .fontColor(Color.Red)
        }
        .height(130)
        .width('60%')
        .margin({ left: 20 })
        .alignItems(HorizontalAlign.Start)

        Image(this.goodsItem.imgSrc)
          .objectFit(ImageFit.ScaleDown)
          .height(130)
          .width('30%')
          .renderMode(ImageRenderMode.Original)
          .margin({ right: 10, left: 10 })

      }
      .backgroundColor(Color.White)

    }
    .params({ goodsData: this.goodsItem })
    .margin({ right: 5 })
  }
}