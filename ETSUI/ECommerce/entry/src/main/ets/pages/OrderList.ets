/* 
  * Copyright (c) 2026 Huawei Device Co., Ltd. 
  * Licensed under the Apache License, Version 2.0 (the "License"); 
  * you may not use this file except in compliance with the License. 
  * You may obtain a copy of the License at 
  * 
  *     `http://www.apache.org/licenses/LICENSE-2.0`  
  * 
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
  * See the License for the specific language governing permissions and 
  * limitations under the License. 
*/

import { promptAction, router } from '@kit.ArkUI';
import { OrderItem, OrderWithItems, User } from '../model/DataModels';
import { RdbUtil } from '../utils/RdbUtil';

@Entry
@Component
// 订单列表页
struct OrderList {
  @StorageLink('currentUser') currentUser: User | undefined = undefined;
  @State tab: number = 0;
  @State orders: OrderWithItems[] = [];

  private readonly tabs: string[] = ['全部', '待付款', '待发货', '待收货', '待评价', '退换/售后'];

  aboutToAppear() {
    if (!this.currentUser) {
      promptAction.showToast({ message: '请先登录' });
      router.back();
      return;
    }

    const params = router.getParams() as Record<string, Object>;
    const t = params && params['tab'] ? Number(params['tab']) : 0;
    this.tab = Number.isFinite(t) ? t : 0;

    this.refreshWrapper();
  }

  private getStatusFilter(): number | undefined {
    if (this.tab === 1) return 0;
    if (this.tab === 2) return 1;
    if (this.tab === 3) return 2;
    if (this.tab === 4) return 3;
    if (this.tab === 5) return 4;
    return undefined;
  }

  private statusText(status: number): string {
    if (status === 0) return '待付款';
    if (status === 1) return '待发货';
    if (status === 2) return '待收货';
    if (status === 3) return '待评价';
    if (status === 4) return '退货/售后';
    if (status === 5) return '已取消';
    if (status === 6) return '已完成';
    return '处理中';
  }

  private itemCount(o: OrderWithItems): number {
    return o.items.reduce((sum, it) => sum + it.count, 0);
  }

  // 刷新订单列表
  async refresh() {
    if (!this.currentUser) {
      this.orders = [];
      return;
    }
    const status = this.getStatusFilter();
    this.orders = (await RdbUtil.getOrderDetails(this.currentUser.id, status)).slice();
  }

  refreshWrapper() {
    this.refresh().catch(() => {
      this.orders = [];
      promptAction.showToast({ message: '订单加载失败' });
    });
  }

  private async requestRefund(orderId: string) {
    const ok = await RdbUtil.updateOrderStatus(orderId, 4);
    if (ok) {
      promptAction.showToast({ message: '已发起退款' });
      this.refreshWrapper();
      return;
    }
    promptAction.showToast({ message: '退款失败' });
  }

  private requestRefundWrapper(orderId: string) {
    this.requestRefund(orderId).catch(() => {
      promptAction.showToast({ message: '退款失败' });
    });
  }

  private async payOrder(orderId: string) {
    const ok = await RdbUtil.updateOrderStatus(orderId, 1);
    if (ok) {
      promptAction.showToast({ message: '支付成功' });
      this.refreshWrapper();
      return;
    }
    promptAction.showToast({ message: '支付失败' });
  }

  private payOrderWrapper(orderId: string) {
    this.payOrder(orderId).catch(() => {
      promptAction.showToast({ message: '支付失败' });
    });
  }

  private async confirmReceive(orderId: string) {
    const ok = await RdbUtil.updateOrderStatus(orderId, 3);
    if (ok) {
      promptAction.showToast({ message: '已确认收货' });
      this.refreshWrapper();
      return;
    }
    promptAction.showToast({ message: '确认收货失败' });
  }

  private confirmReceiveWrapper(orderId: string) {
    this.confirmReceive(orderId).catch(() => {
      promptAction.showToast({ message: '确认收货失败' });
    });
  }

  private async finishReview(orderId: string) {
    const ok = await RdbUtil.updateOrderStatus(orderId, 6);
    if (ok) {
      promptAction.showToast({ message: '已完成评价' });
      this.refreshWrapper();
      return;
    }
    promptAction.showToast({ message: '操作失败' });
  }

  private finishReviewWrapper(orderId: string) {
    this.finishReview(orderId).catch(() => {
      promptAction.showToast({ message: '操作失败' });
    });
  }

  @Builder
  TabChip(label: string, idx: number) {
    Text(label)
      .fontSize(13)
      .fontColor(this.tab === idx ? '#3B82F6' : '#111827')
      .padding({ left: 12, right: 12, top: 8, bottom: 8 })
      .backgroundColor(this.tab === idx ? '#DBEAFE' : '#F3F4F6')
      .borderRadius(16)
      .onClick(() => {
        this.tab = idx;
        this.refreshWrapper();
      });
  }

  @Builder
  OrderCard(o: OrderWithItems) {
    Column({ space: 10 }) {
      Row() {
        Text('官方自营')
          .fontSize(14)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827');

        Text('>')
          .fontSize(14)
          .fontColor('#9CA3AF')
          .margin({ left: 6 });

        Blank();

        Text(this.statusText(o.order.status))
          .fontSize(13)
          .fontColor('#3B82F6');
      }

      if (o.items.length > 0) {
        Column({ space: 10 }) {
          ForEach(o.items, (it: OrderItem) => {
            Row({ space: 10 }) {
              Image(RdbUtil.resolveImageSource(it.image))
                .width(76)
                .height(76)
                .borderRadius(12)
                .backgroundColor('#F3F4F6');

              Column({ space: 6 }) {
                Text(it.productName)
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111827')
                  .maxLines(2)
                  .textOverflow({ overflow: TextOverflow.Ellipsis });

                Row() {
                  Text(`¥${it.price}`)
                    .fontSize(14)
                    .fontWeight(FontWeight.Bold)
                    .fontColor(Color.Red)
                    .margin({ right: 8 });

                  Blank();

                  Text(`x${it.count}`)
                    .fontSize(12)
                    .fontColor('#6B7280');
                }
              }
              .layoutWeight(1)
              .alignItems(HorizontalAlign.Start);
            }
          });
        }
      }

      Row() {
        Blank();
        Text(`共${this.itemCount(o)}件商品  实付款 `)
          .fontSize(12)
          .fontColor('#6B7280');

        Text(`¥${o.order.totalAmount.toFixed(2)}`)
          .fontSize(14)
          .fontWeight(FontWeight.Bold)
          .fontColor(Color.Red);
      }

      Row({ space: 10 }) {
        Blank();

        if (o.order.status === 1) {
          Button('退款')
            .type(ButtonType.Normal)
            .height(32)
            .fontSize(13)
            .fontColor('#111827')
            .backgroundColor('#F3F4F6')
            .borderRadius(16)
            .onClick(() => {
              this.requestRefundWrapper(o.order.id);
            });

          Button('催发货')
            .type(ButtonType.Normal)
            .height(32)
            .fontSize(13)
            .fontColor('#111827')
            .backgroundColor('#F3F4F6')
            .borderRadius(16)
            .onClick(() => {
              promptAction.showToast({ message: '已提醒商家发货' });
            });

          Button('修改地址')
            .type(ButtonType.Normal)
            .height(32)
            .fontSize(13)
            .fontColor('#111827')
            .backgroundColor('#F3F4F6')
            .borderRadius(16)
            .onClick(() => {
              promptAction.showToast({ message: '修改地址可后续接入' });
            });
        } else if (o.order.status === 2) {
          Button('确认收货')
            .type(ButtonType.Normal)
            .height(32)
            .fontSize(13)
            .fontColor(Color.White)
            .backgroundColor('#3B82F6')
            .borderRadius(16)
            .onClick(() => {
              this.confirmReceiveWrapper(o.order.id);
            });
        } else if (o.order.status === 0) {
          Button('去支付')
            .type(ButtonType.Normal)
            .height(32)
            .fontSize(13)
            .fontColor(Color.White)
            .backgroundColor('#3B82F6')
            .borderRadius(16)
            .onClick(() => {
              this.payOrderWrapper(o.order.id);
            });
        } else if (o.order.status === 3) {
          Button('去评价')
            .type(ButtonType.Normal)
            .height(32)
            .fontSize(13)
            .fontColor(Color.White)
            .backgroundColor('#3B82F6')
            .borderRadius(16)
            .onClick(() => {
              this.finishReviewWrapper(o.order.id);
            });
        }
      }
    }
    .padding(12)
    .backgroundColor(Color.White)
    .borderRadius(14)
    .onClick(() => {
      router.pushUrl({ url: 'pages/OrderDetail', params: { orderId: o.order.id } });
    });
  }

  build() {
    Column() {
      Row() {
        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Text('<')
            .fontSize(24)
            .fontColor('#111827');
        }
        .backgroundColor(Color.Transparent)
        .width(44)
        .height(44)
        .onClick(() => {
          router.back();
        });

        Text('全部订单')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827');

        Blank();

        Button('刷新')
          .type(ButtonType.Normal)
          .height(32)
          .fontSize(13)
          .fontColor('#6B7280')
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.refreshWrapper();
          });
      }
      .width('100%')
      .height(56)
      .padding({ left: 6, right: 12 })
      .backgroundColor(Color.White);

      Scroll() {
        Row({ space: 10 }) {
          ForEach(this.tabs, (t: string, idx: number) => {
            this.TabChip(t, idx);
          });
        }
        .padding({ left: 12, right: 12, top: 10, bottom: 10 });
      }
      .scrollable(ScrollDirection.Horizontal)
      .backgroundColor('#F5F5F5');

      if (this.orders.length === 0) {
        Column({ space: 12 }) {
          Text('暂无订单')
            .fontSize(16)
            .fontColor('#6B7280');

          Button('去逛逛')
            .height(44)
            .width('60%')
            .backgroundColor('#3B82F6')
            .fontColor(Color.White)
            .borderRadius(22)
            .onClick(() => {
              AppStorage.setOrCreate('indexTargetTab', 0);
              router.back();
            });
        }
        .layoutWeight(1)
        .width('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .backgroundColor('#F5F5F5');
      } else {
        List({ space: 10 }) {
          ForEach(this.orders, (o: OrderWithItems) => {
            ListItem() {
              this.OrderCard(o);
            }
          });
        }
        .padding({ left: 12, right: 12, top: 10, bottom: 10 })
        .layoutWeight(1)
        .backgroundColor('#F5F5F5');
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5');
  }
}