import { CartItem, User } from '../model/DataModels';
import { RdbUtil } from '../utils/RdbUtil';
import { promptAction, router } from '@kit.ArkUI';

@Component
// 购物车视图
export struct CartView {
  @State cartList: CartItem[] = [];
  @State isManaging: boolean = false;
  @Link currentIndex: number;
  @StorageLink('currentUser') currentUser: User | undefined = undefined;
  public onSwitchTab: (index: number) => void = (index: number) => {};

  async aboutToAppear() {
    if (!this.currentUser) {
      this.cartList = [];
      promptAction.showToast({ message: '请先登录' });
      this.onSwitchTab(0);
      router.pushUrl({ url: 'pages/LoginPage' });
      return;
    }
    await this.refreshCart();
  }

  // 刷新购物车数据
  async refreshCart() {
    if (!this.currentUser) {
      this.cartList = [];
      return;
    }
    this.cartList = await RdbUtil.getCartItems();
  }

  // 当前是否处于“全选”（底部全选 Checkbox 的展示状态）
  isAllSelected(): boolean {
    return this.cartList.length > 0 && this.cartList.every(i => i.selected);
  }

  getSelectedTotal(): number {
    return this.cartList.filter(i => i.selected).reduce((sum, i) => sum + i.price * i.count, 0);
  }

  getSelectedCount(): number {
    return this.cartList.filter(i => i.selected).length;
  }

  toggleManage() {
    this.isManaging = !this.isManaging;
  }

  // 删除选中的商品
  async deleteSelected() {
    const selected = this.cartList.filter(i => i.selected);
    if (selected.length === 0) {
      promptAction.showToast({ message: '请选择要删除的商品' });
      return;
    }

    for (const it of selected) {
      await RdbUtil.deleteCartItem(it.id);
    }
    await this.refreshCart();
    promptAction.showToast({ message: '已删除' });
  }

  @Builder
  CartItemSwipe(item: CartItem) {
    Row() {
      Button('删除')
        .type(ButtonType.Normal)
        .backgroundColor(Color.Red)
        .fontColor(Color.White)
        .width(60)
        .height('100%')
        .onClick(async () => {
          await RdbUtil.deleteCartItem(item.id);
          await this.refreshCart();
        })
    }
    .height('100%')
  }

  build() {
    Column() {
      Row() {
        Text(`购物车(${this.cartList.length})`)
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827');
        Blank();
        Button(this.isManaging ? '完成' : '管理')
          .type(ButtonType.Normal)
          .height(32)
          .fontSize(13)
          .fontColor('#6B7280')
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.toggleManage();
          });
      }
      .width('100%')
      .height(56)
      .padding({ left: 12, right: 12 })
      .backgroundColor(Color.White)

      if (this.cartList.length === 0) {
        Column({ space: 12 }) {
          Image($r('app.media.cart2'))
            .width(80)
            .height(80)

          Text('购物车还是空的')
            .fontSize(16)
            .fontColor('#6B6B6B')

          Button('去逛逛')
            .width('50%')
            .height(44)
            .backgroundColor('#1698CE')
            .fontColor(Color.White)
            .onClick(() => {
              this.onSwitchTab(0);
            })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        Column({ space: 10 }) {
          Row({ space: 6 }) {
            Text('官方自营')
              .fontSize(14)
              .fontWeight(FontWeight.Bold)
              .fontColor('#111827');
            Text('>')
              .fontSize(14)
              .fontColor('#9CA3AF');
            Blank();
          }
          .padding({ left: 12, right: 12, top: 10, bottom: 2 })

          List({ space: 10 }) {
            ForEach(this.cartList, (item: CartItem) => {
              ListItem() {
                Row({ space: 10 }) {
                  Checkbox()
                    .select(item.selected)
                    .onChange(async (val) => {
                      item.selected = val;
                      await RdbUtil.updateCartItem(item);
                      await this.refreshCart();
                    });

                  Image(RdbUtil.resolveImageSource(item.image))
                    .width(88)
                    .height(88)
                    .borderRadius(10)
                    .backgroundColor('#F3F4F6');

                  Column({ space: 6 }) {
                    Row() {
                      Text(item.productName)
                        .fontSize(14)
                        .fontWeight(FontWeight.Bold)
                        .fontColor('#111827')
                        .maxLines(2)
                        .textOverflow({ overflow: TextOverflow.Ellipsis })
                        .layoutWeight(1);

                      Text(`x${item.count}`)
                        .fontSize(12)
                        .fontColor('#6B7280')
                        .margin({ left: 8 });
                    }

                    Row({ space: 6 }) {
                      Text('规格 默认')
                        .fontSize(10)
                        .fontColor('#6B7280')
                        .backgroundColor('#F3F4F6')
                        .padding({ left: 6, right: 6, top: 2, bottom: 2 })
                        .borderRadius(10);

                      Text('7天无理由退换')
                        .fontSize(10)
                        .fontColor('#FF5000')
                        .backgroundColor('#FFF2E8')
                        .padding({ left: 6, right: 6, top: 2, bottom: 2 })
                        .borderRadius(10);

                      Blank();
                    }

                    Row() {
                      Text(`¥${item.price}`)
                        .fontSize(16)
                        .fontWeight(FontWeight.Bold)
                        .fontColor('#FF5000');

                      Blank();

                      if (!this.isManaging) {
                        Row({ space: 2 }) {
                          Button('-')
                            .width(26)
                            .height(26)
                            .fontSize(14)
                            .fontColor('#111827')
                            .backgroundColor('#F3F4F6')
                            .borderRadius(6)
                            .padding(0)
                            .onClick(async () => {
                              if (item.count > 1) {
                                item.count--;
                                await RdbUtil.updateCartItem(item);
                                await this.refreshCart();
                              } else {
                                await RdbUtil.deleteCartItem(item.id);
                                await this.refreshCart();
                              }
                            });

                          Text(`${item.count}`)
                            .fontSize(14)
                            .constraintSize({ minWidth: 30 })
                            .textAlign(TextAlign.Center);

                          Button('+')
                            .width(26)
                            .height(26)
                            .fontSize(14)
                            .fontColor('#111827')
                            .backgroundColor('#F3F4F6')
                            .borderRadius(6)
                            .padding(0)
                            .onClick(async () => {
                              item.count++;
                              await RdbUtil.updateCartItem(item);
                              await this.refreshCart();
                            });
                        }
                      }
                    }
                  }
                  .alignItems(HorizontalAlign.Start)
                  .layoutWeight(1)
                }
                .padding(12)
                .backgroundColor(Color.White)
                .borderRadius(14)
              }
              .swipeAction(this.isManaging ? undefined : { end: this.CartItemSwipe(item) })
            })
          }
          .width('100%')
          .layoutWeight(1)
          .padding({ left: 12, right: 12 })

          Row() {
            Row({ space: 6 }) {
              Checkbox({ name: 'all', group: 'cart_all' })
                .select(this.isAllSelected())
                .onChange(async (val) => {
                  if (val === this.isAllSelected()) {
                    return;
                  }

                  for (let it of this.cartList) {
                    it.selected = val;
                    await RdbUtil.updateCartItem(it);
                  }
                  await this.refreshCart();
                });
              Text('全选')
                .fontSize(14)
                .fontColor('#111827');
            }

            Blank();

            if (this.isManaging) {
              Button(`删除(${this.getSelectedCount()})`)
                .fontSize(14)
                .height(40)
                .backgroundColor(this.getSelectedCount() > 0 ? '#FF4D4F' : '#D1D5DB')
                .fontColor(Color.White)
                .borderRadius(20)
                .enabled(this.getSelectedCount() > 0)
                .onClick(async () => {
                  await this.deleteSelected();
                });
            } else {
              Row({ space: 10 }) {
                Text('合计')
                  .fontSize(14)
                  .fontColor('#111827');

                Text(`¥${this.getSelectedTotal().toFixed(2)}`)
                  .fontSize(18)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(Color.Red);

                Button('去结算')
                  .fontSize(14)
                  .height(40)
                  .backgroundColor(this.getSelectedCount() > 0 ? $r('app.color.mine_background') : '#D1D5DB')
                  .fontColor(Color.White)
                  .borderRadius(20)
                  .enabled(this.getSelectedCount() > 0)
                  .onClick(async () => {
                    if (this.getSelectedCount() === 0) {
                      promptAction.showToast({ message: '请先选择要结算的商品' });
                      return;
                    }

                    if (!this.currentUser) {
                      promptAction.showToast({ message: '请先登录' });
                      this.onSwitchTab(2);
                      return;
                    }

                    const selectedItems = this.cartList.filter(i => i.selected);
                    const success = await RdbUtil.checkout(selectedItems, this.currentUser.id);
                    if (success) {
                      const nextUser = await RdbUtil.getUserById(this.currentUser.id);
                      if (nextUser) {
                        AppStorage.setOrCreate('currentUser', nextUser);
                      }

                      promptAction.showToast({ message: '支付成功！订单已生成' });
                      await this.refreshCart();
                    } else {
                      promptAction.showToast({ message: '支付失败，可能库存不足或代金券余额不足' });
                    }
                  });
              }
            }
          }
          .width('100%')
          .padding({ left: 12, right: 12, top: 10, bottom: 10 })
          .backgroundColor(Color.White)
          .shadow({ radius: 10, color: '#1A000000', offsetY: -2 })
        }
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
    .onVisibleAreaChange([0.0, 1.0], (isVisible: boolean, currentRatio: number) => {
      if (isVisible) {
        this.refreshCart().catch(() => {
          promptAction.showToast({ message: '刷新购物车失败' });
        });
      }
    })
  }
}