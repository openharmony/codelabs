import { router, promptAction } from '@kit.ArkUI';
import { Product, CartItem, User } from '../model/DataModels';
import { RdbUtil } from '../utils/RdbUtil';

@Entry
@Component
// 商品详情页
struct ProductDetail {
  @State product: Product = new Product(0, '', 0, '', '', '', 0);
  @State isFavorite: boolean = false;
  @StorageLink('currentUser') currentUser: User | undefined = undefined;

  aboutToAppear() {
    const params = router.getParams() as Record<string, Object>;
    if (params && params['product']) {
      this.product = params['product'] as Product;
    }
    this.refreshFavoriteWrapper();
    this.addFootprintWrapper();
  }

  async addFootprint() {
    if (!this.currentUser || !this.product || this.product.id <= 0) {
      return;
    }
    await RdbUtil.addFootprint(this.currentUser.id, this.product.id);
  }

  addFootprintWrapper() {
    this.addFootprint().catch(() => {
    });
  }

  async refreshFavorite() {
    if (!this.currentUser || !this.product || this.product.id <= 0) {
      this.isFavorite = false;
      return;
    }
    this.isFavorite = await RdbUtil.isFavorite(this.currentUser.id, this.product.id);
  }

  refreshFavoriteWrapper() {
    this.refreshFavorite().catch(() => {
      this.isFavorite = false;
    });
  }

  // 切换收藏状态
  async toggleFavorite() {
    if (!this.currentUser) {
      promptAction.showToast({ message: '请先登录' });
      router.pushUrl({ url: 'pages/LoginPage' });
      return;
    }

    this.isFavorite = await RdbUtil.toggleFavorite(this.currentUser.id, this.product.id);
    promptAction.showToast({ message: this.isFavorite ? '已收藏' : '已取消收藏' });
  }

  toggleFavoriteWrapper() {
    this.toggleFavorite().catch(() => {
      promptAction.showToast({ message: '收藏操作失败' });
    });
  }

  build() {
    Column() {
      Row() {
        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Text('<')
            .fontSize(24)
            .fontColor('#333333')
        }
        .backgroundColor(Color.Transparent)
        .width(40)
        .height(40)
        .onClick(() => {
          router.back();
        })

        Text('商品详情')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 12 })
      }
      .width('100%')
      .height(56)
      .padding({ left: 10, right: 16 })
      .backgroundColor(Color.White)

      Scroll() {
        Column() {
          Image(this.product.image)
            .width('100%')
            .height(350)
            .objectFit(ImageFit.Contain)
            .backgroundColor(Color.White)

          Column() {
            Text(`¥${this.product.price}`)
              .fontSize(28)
              .fontColor(Color.Red)
              .fontWeight(FontWeight.Bold)
              .margin({ top: 16 })

            Text(this.product.name)
              .fontSize(22)
              .fontWeight(FontWeight.Bold)
              .margin({ top: 12 })

            Text(`库存: ${this.product.stock} 件`)
              .fontSize(14)
              .fontColor('#666')
              .margin({ top: 8 })

            Divider().margin({ top: 16, bottom: 16 })

            Text('商品简介')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .margin({ bottom: 8 })
            
            Text(this.product.description)
              .fontSize(15)
              .fontColor('#666666')
              .lineHeight(24)
            
            Text('规格参数')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .margin({ top: 24, bottom: 8 })
            
            Row() {
              Text('分类').fontColor('#999').width(60)
              Text(this.product.category).fontColor('#333')
            }.margin({ bottom: 8 })
            
            Row() {
              Text('服务').fontColor('#999').width(60)
              Text('7天无理由退货 · 正品保证').fontColor('#333')
            }
          }
          .padding(20)
          .width('100%')
          .alignItems(HorizontalAlign.Start)
          .backgroundColor(Color.White)
          .borderRadius({ topLeft: 20, topRight: 20 })
          .margin({ top: -20 })
        }
      }
      .layoutWeight(1)

      Row({ space: 2 }) {
        Column({ space: 1 }) {
          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Image($r('app.media.comment'))
              .width(18)
              .height(18);
          }
          .padding(0)
          .backgroundColor(Color.Transparent)
          .width(36)
          .height(36)

          Text('评论')
            .fontSize(9)
            .fontColor('#6B7280');
        }
        .width(40)
        .alignItems(HorizontalAlign.Center)
        .justifyContent(FlexAlign.Center)
        .onClick(() => {
          router.pushUrl({ url: 'pages/CommentList', params: { product: this.product } });
        })

        Column({ space: 1 }) {
          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Image($r('app.media.cart1'))
              .width(18)
              .height(18);
          }
          .padding(0)
          .backgroundColor(Color.Transparent)
          .width(36)
          .height(36)

          Text('购物车')
            .fontSize(9)
            .fontColor('#6B7280');
        }
        .width(40)
        .alignItems(HorizontalAlign.Center)
        .justifyContent(FlexAlign.Center)
        .onClick(() => {
          AppStorage.setOrCreate('indexTargetTab', 1);
          router.back();
        })

        Column({ space: 1 }) {
          Button({ type: ButtonType.Normal, stateEffect: true }) {
            Text(this.isFavorite ? '★' : '☆')
              .fontSize(16)
              .fontColor(this.isFavorite ? '#FF4D4F' : '#6B7280');
          }
          .padding(0)
          .backgroundColor(Color.Transparent)
          .width(36)
          .height(36)

          Text('收藏')
            .fontSize(9)
            .fontColor(this.isFavorite ? '#FF4D4F' : '#6B7280');
        }
        .width(40)
        .alignItems(HorizontalAlign.Center)
        .justifyContent(FlexAlign.Center)
        .onClick(() => {
          this.toggleFavoriteWrapper();
        })

        Button('加入购物车')
          .backgroundColor(this.product.stock > 0 ? '$r(app.color.mine_background)' : '#CCCCCC')
          .enabled(this.product.stock > 0)
          .fontColor(Color.White)
          .layoutWeight(1)
          .height(40)
          .onClick(async () => {
             if (!this.currentUser) {
               promptAction.showToast({ message: '请先登录' });
               router.pushUrl({ url: 'pages/LoginPage' });
               return;
             }
             await RdbUtil.addToCart(new CartItem(0, this.product.id, this.product.name, this.product.price, 1, true, this.product.image));
             promptAction.showToast({ message: '已加入购物车' });
          })

        Button('立即购买')
          .backgroundColor(this.product.stock > 0 ? Color.Red : '#CCCCCC')
          .enabled(this.product.stock > 0)
          .fontColor(Color.White)
          .layoutWeight(1)
          .height(40)
          .onClick(async () => {
             if (!this.currentUser) {
               promptAction.showToast({ message: '请先登录' });
               router.pushUrl({ url: 'pages/LoginPage' });
               return;
             }
             await RdbUtil.addToCart(new CartItem(0, this.product.id, this.product.name, this.product.price, 1, true, this.product.image));
             AppStorage.setOrCreate('indexTargetTab', 1);
             router.back();
             promptAction.showToast({ message: '请去购物车结算' });
          })
      }
      .width('100%')
      .height(60)
      .padding({ left: 10, right: 10 })
      .backgroundColor(Color.White)
      .shadow({ radius: 10, color: '#1A000000', offsetY: -2 })

    }
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}