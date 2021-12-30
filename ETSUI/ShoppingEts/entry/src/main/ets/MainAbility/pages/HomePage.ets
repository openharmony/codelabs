import {GoodsData,IconImage} from '../model/GoodsData'
import {initializeOnStartup, getIconPath, getIconPathSelect} from '../model/GoodsDataModels'
import {ShoppingCart} from './ShoppingCartPage.ets'
import {MyInfo} from './MyPage.ets'
import router from '@system.router';

@Entry
@Component
struct Index {
  @Provide currentPage: number = 1
  private goodsItems: GoodsData[] = initializeOnStartup()
  build() {
    Column() {
      Scroll(){
        Column() {
          if (this.currentPage == 1) {
            GoodsHome({ goodsItems: this.goodsItems })
          } else if (this.currentPage == 2) {
            //购物车列表
            ShoppingCart()
          } else {
            //我的
            MyInfo()
          }
        }
        .width('100%')
        .flexGrow(1)
        }
      .scrollable(ScrollDirection.Vertical)
      HomeBottom()
    }
    .height('93%')
    .width('100%')
    .backgroundColor("white")

  }
}


@Component
struct HomeBottom {
  @Consume currentPage: number
  private iconPathTmp: IconImage[] = getIconPath()
  private iconPathSelectsTmp: IconImage[] = getIconPathSelect()
  @State iconPath: IconImage[] = getIconPath()

  build() {
    Row() {
      Image(this.iconPath[0].imgSrc)
        .objectFit(ImageFit.Contain)
        .height(120)
        .width(120)
        .margin({left:80,top: 20})
        .renderMode(ImageRenderMode.Original)
        .onClick(()=>{
          this.iconPath[0] = this.iconPathTmp[0]
          this.iconPath[1] = this.iconPathTmp[1]
          this.iconPath[2] = this.iconPathTmp[2]
          this.currentPage = 1
        })
      Image(this.iconPath[1].imgSrc)
        .objectFit(ImageFit.Contain)
        .height(120)
        .width(120)
        .margin({left:80,top: 20})
        .renderMode(ImageRenderMode.Original)
        .onClick(()=>{
          this.iconPath[0] = this.iconPathSelectsTmp[0]
          this.iconPath[1] = this.iconPathSelectsTmp[1]
          this.iconPath[2] = this.iconPathTmp[2]
          this.currentPage = 2
        })
      Image(this.iconPath[2].imgSrc)
        .objectFit(ImageFit.Contain)
        .height(120)
        .width(120)
        .margin({left:80,top: 20})
        .renderMode(ImageRenderMode.Original)
        .onClick(()=>{
          this.iconPath[0] = this.iconPathSelectsTmp[0]
          this.iconPath[1] = this.iconPathTmp[1]
          this.iconPath[2] = this.iconPathSelectsTmp[2]
          this.currentPage = 3
        })

    }
    .backgroundColor(Color.White)
    .alignItems(VerticalAlign.Bottom)
    .width('100%')
    .height('10%')
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