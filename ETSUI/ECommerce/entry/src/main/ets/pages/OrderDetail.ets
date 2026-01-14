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
// 订单详情页
struct OrderDetail {
  @StorageLink('currentUser') currentUser: User | undefined = undefined;

  @State orderId: string = '';
  @State detail: OrderWithItems | undefined = undefined;

  aboutToAppear() {
    if (!this.currentUser) {
      promptAction.showToast({ message: '请先登录' });
      router.back();
      return;
    }

    const params = router.getParams() as Record<string, Object>;
    const oid = params && params['orderId'] ? String(params['orderId']) : '';
    this.orderId = oid;

    this.refreshWrapper();
  }

  private statusText(status: number): string {
    if (status === 0) return '待付款';
    if (status === 1) return '待发货';
    if (status === 2) return '待收货';
    if (status === 3) return '待评价';
    if (status === 4) return '退换/售后';
    if (status === 5) return '已取消';
    if (status === 6) return '已完成';
    return '处理中';
  }

  private formatDate(ts: number): string {
    const d = new Date(ts);
    const y = d.getFullYear();
    const m = `${d.getMonth() + 1}`.padStart(2, '0');
    const day = `${d.getDate()}`.padStart(2, '0');
    const hh = `${d.getHours()}`.padStart(2, '0');
    const mm = `${d.getMinutes()}`.padStart(2, '0');
    return `${y}-${m}-${day} ${hh}:${mm}`;
  }

  // 刷新订单详情
  private async refresh() {
    if (!this.currentUser || !this.orderId) {
      this.detail = undefined;
      return;
    }
    this.detail = await RdbUtil.getOrderDetail(this.orderId, this.currentUser.id);
  }

  private refreshWrapper() {
    this.refresh().catch(() => {
      this.detail = undefined;
      promptAction.showToast({ message: '订单详情加载失败' });
    });
  }

  // 更新订单状态
  private async setStatus(nextStatus: number, okMsg: string) {
    if (!this.detail) return;
    const ok = await RdbUtil.updateOrderStatus(this.detail.order.id, nextStatus);
    if (ok) {
      promptAction.showToast({ message: okMsg });
      this.refreshWrapper();
      return;
    }
    promptAction.showToast({ message: '操作失败' });
  }

  private setStatusWrapper(nextStatus: number, okMsg: string) { 
    this.setStatus(nextStatus, okMsg).catch(() => {
      promptAction.showToast({ message: '操作失败' });
    });
  }

  @Builder
  ItemRow(it: OrderItem) {
    Row({ space: 10 }) {
      Image(it.image)
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
            .fontColor(Color.Red);

          Blank();

          Text(`x${it.count}`)
            .fontSize(12)
            .fontColor('#6B7280');
        }
      }
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Start);
    }
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
        .onClick(() => router.back());

        Text('订单详情')
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
          .onClick(() => this.refreshWrapper());
      }
      .width('100%')
      .height(56)
      .padding({ left: 6, right: 12 })
      .backgroundColor(Color.White);

      if (!this.detail) {
        Column({ space: 12 }) {
          Text('订单不存在或无权限查看')
            .fontSize(16)
            .fontColor('#6B7280');

          Button('返回')
            .height(44)
            .width('60%')
            .backgroundColor('#3B82F6')
            .fontColor(Color.White)
            .borderRadius(22)
            .onClick(() => router.back());
        }
        .layoutWeight(1)
        .width('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .backgroundColor('#F5F5F5');
      } else {
        Scroll() {
          Column({ space: 12 }) {
            Column({ space: 10 }) {
              Row({ space: 8 }) {
                Text(this.statusText(this.detail!.order.status))
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#3B82F6')
                  .backgroundColor('#DBEAFE')
                  .padding({ left: 10, right: 10, top: 6, bottom: 6 })
                  .borderRadius(14);

                Blank();

                Text(`下单时间 ${this.formatDate(this.detail!.order.createTime)}`)
                  .fontSize(12)
                  .fontColor('#6B7280');
              }

              Text(`订单号 ${this.detail!.order.id}`)
                .fontSize(12)
                .fontColor('#6B7280');
            }
            .padding(12)
            .backgroundColor(Color.White)
            .borderRadius(14);

            Column({ space: 10 }) {
              ForEach(this.detail!.items, (it: OrderItem) => {
                this.ItemRow(it);
              });
            }
            .padding(12)
            .backgroundColor(Color.White)
            .borderRadius(14);

            Row() {
              Text('实付款')
                .fontSize(12)
                .fontColor('#6B7280');

              Blank();

              Text(`¥${this.detail!.order.totalAmount.toFixed(2)}`)
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .fontColor(Color.Red);
            }
            .padding(12)
            .backgroundColor(Color.White)
            .borderRadius(14);

            Column() {
            }
            .height(14);
          }
          .padding(12);
        }
        .layoutWeight(1)
        .backgroundColor('#F5F5F5');

        Row({ space: 10 }) {
          Blank();

          if (this.detail!.order.status === 0) {
            Button('取消订单')
              .type(ButtonType.Normal)
              .height(40)
              .fontSize(13)
              .fontColor('#111827')
              .backgroundColor('#F3F4F6')
              .borderRadius(20)
              .onClick(() => this.setStatusWrapper(5, '已取消订单'));

            Button('去支付')
              .type(ButtonType.Normal)
              .height(40)
              .fontSize(13)
              .fontColor(Color.White)
              .backgroundColor('#3B82F6')
              .borderRadius(20)
              .onClick(() => this.setStatusWrapper(1, '支付成功'));
          } else if (this.detail!.order.status === 1) {
            Button('申请退款')
              .type(ButtonType.Normal)
              .height(40)
              .fontSize(13)
              .fontColor('#111827')
              .backgroundColor('#F3F4F6')
              .borderRadius(20)
              .onClick(() => this.setStatusWrapper(4, '已发起退款'));

            Button('催发货')
              .type(ButtonType.Normal)
              .height(40)
              .fontSize(13)
              .fontColor(Color.White)
              .backgroundColor('#3B82F6')
              .borderRadius(20)
              .onClick(() => {
                promptAction.showToast({ message: '已提醒商家发货' });
              });
          } else if (this.detail!.order.status === 2) {
            Button('申请售后')
              .type(ButtonType.Normal)
              .height(40)
              .fontSize(13)
              .fontColor('#111827')
              .backgroundColor('#F3F4F6')
              .borderRadius(20)
              .onClick(() => this.setStatusWrapper(4, '已发起售后'));

            Button('确认收货')
              .type(ButtonType.Normal)
              .height(40)
              .fontSize(13)
              .fontColor(Color.White)
              .backgroundColor('#3B82F6')
              .borderRadius(20)
              .onClick(() => this.setStatusWrapper(3, '已确认收货'));
          } else if (this.detail!.order.status === 3) {
            Button('完成评价')
              .type(ButtonType.Normal)
              .height(40)
              .fontSize(13)
              .fontColor(Color.White)
              .backgroundColor('#3B82F6')
              .borderRadius(20)
              .onClick(() => this.setStatusWrapper(6, '已完成评价'));
          }
        }
        .width('100%')
        .padding({ left: 12, right: 12, top: 10, bottom: 10 })
        .backgroundColor(Color.White)
        .shadow({ radius: 10, color: '#1A000000', offsetY: -2 });
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5');
  }
}