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

import { router, promptAction } from '@kit.ArkUI';
import { User } from '../model/DataModels';
import { RdbUtil } from '../utils/RdbUtil';

@Component
// 个人中心视图
export struct MineView {
  @StorageLink('currentUser') currentUser: User | undefined = undefined;

  @Builder
  SectionHeader(title: string, rightText?: string, onRightClick?: () => void) {
    Row({ space: 10 }) {
      Text(title)
        .fontSize(15)
        .fontWeight(FontWeight.Bold)
        .fontColor('#0F172A');

      Blank();

      if (rightText) {
        Text(rightText)
          .fontSize(12)
          .fontColor('#0F172A')
          .backgroundColor('#EEF2FF')
          .padding({ left: 10, right: 10, top: 6, bottom: 6 })
          .borderRadius(14)
          .onClick(() => {
            if (onRightClick) onRightClick();
          });
      }
    }
    .width('100%');
  }

  @Builder
  QuickItem(label: string, iconText: string, onClick?: () => void) {
    Column({ space: 8 }) {
      Column() {
        Text(iconText)
          .fontSize(14)
          .fontWeight(FontWeight.Bold)
          .fontColor($r('app.color.mine_background'));
      }
      .width(38)
      .height(38)
      .borderRadius(12)
      .backgroundColor($r('app.color.mine_icon_bg'))
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center);

      Text(label)
        .fontSize(12)
        .fontColor('#334155');
    }
    .width('20%')
    .onClick(() => {
      if (onClick) onClick();
    });
  }

  @Builder
  ToolRow(label: string, iconText: string, onClick?: () => void) {
    Row({ space: 12 }) {
      Column() {
        Text(iconText)
          .fontSize(14)
          .fontWeight(FontWeight.Bold)
          .fontColor($r('app.color.mine_background'));
      }
      .width(34)
      .height(34)
      .borderRadius(12)
      .backgroundColor($r('app.color.mine_icon_bg'))
      .border({ width: 1, color: '#E5E7EB' })
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center);

      Text(label)
        .fontSize(14)
        .fontColor('#0F172A');

      Blank();

      Text('>')
        .fontSize(14)
        .fontColor('#94A3B8');
    }
    .padding({ left: 14, right: 14, top: 14, bottom: 14 })
    .backgroundColor(Color.White)
    .borderRadius(16)
    .border({ width: 1, color: '#EEF2F7' })
    .onClick(() => {
      if (onClick) onClick();
    });
  }

  build() {
    Column() {
      Column() {
        Column() {
          Row() {
            Image($r('app.media.user'))
              .width(54)
              .height(54)
              .borderRadius(14)
              .backgroundColor($r('app.color.mine_icon_bg'))
              .border({ width: 2, color: Color.White })
              .shadow({ radius: 10, color: '#1A000000', offsetY: 2 });

            Column({ space: 6 }) {
              Text(this.currentUser ? this.currentUser.username : '未登录')
                .fontSize(18)
                .fontWeight(FontWeight.Bold)
                .fontColor(Color.White);

              if (this.currentUser) {
                Text(`代金券余额 ¥${this.currentUser.voucherBalance.toFixed(2)}`)
                  .fontSize(12)
                  .fontColor('#E5E7EB');
              }
            }
            .alignItems(HorizontalAlign.Start)
            .margin({ left: 12 });

            Blank();

            Button('设置')
              .type(ButtonType.Normal)
              .height(32)
              .fontSize(12)
              .fontColor('#0F172A')
              .backgroundColor(Color.White)
              .border({ width: 1, color: '#DDE7FF' })
              .borderRadius(16)
              .onClick(() => {
                promptAction.showToast({ message: '设置功能可后续接入' });
              });
          }
          .width('100%')
          .padding({ left: 14, right: 14, top: 14, bottom: 14 })
          .backgroundColor($r('app.color.mine_background'))
          .borderRadius(22);
        }
        .padding(2)
        .backgroundColor($r('app.color.mine_icon_bg'))
        .borderRadius(24)
        .shadow({ radius: 12, color: '#12000000', offsetY: 4 });
      }
      .width('100%')
      .padding({ left: 0, right: 0, top: 14, bottom: 10 });

      Scroll() {
        Column({ space: 12 }) {
          if (!this.currentUser) {
            Column({ space: 10 }) {
              Text('你还没有登录')
                .fontSize(14)
                .fontColor($r('app.color.mine_text_secondary'));
              Button('去登录 / 注册')
                .height(44)
                .fontSize(14)
                .fontColor(Color.White)
                .backgroundColor($r('app.color.mine_background'))
                .borderRadius(22)
                .onClick(() => {
                  router.pushUrl({ url: 'pages/LoginPage' });
                });
            }
            .padding(16)
            .backgroundColor(Color.White)
            .borderRadius(16)
            .margin({ top: 12 })
            .alignItems(HorizontalAlign.Center);
          }

          Column({ space: 12 }) {
            this.SectionHeader('订单管理', '全部 >', () => {
              if (!this.currentUser) {
                promptAction.showToast({ message: '请先登录' });
                router.pushUrl({ url: 'pages/LoginPage' });
                return;
              }
              router.pushUrl({ url: 'pages/OrderList', params: { tab: 0 } });
            });

            Row() {
              this.QuickItem('待付款', '付', () => {
                if (!this.currentUser) {
                  promptAction.showToast({ message: '请先登录' });
                  router.pushUrl({ url: 'pages/LoginPage' });
                  return;
                }
                router.pushUrl({ url: 'pages/OrderList', params: { tab: 1 } });
              });
              this.QuickItem('待发货', '发', () => {
                if (!this.currentUser) {
                  promptAction.showToast({ message: '请先登录' });
                  router.pushUrl({ url: 'pages/LoginPage' });
                  return;
                }
                router.pushUrl({ url: 'pages/OrderList', params: { tab: 2 } });
              });
              this.QuickItem('待收货', '收', () => {
                if (!this.currentUser) {
                  promptAction.showToast({ message: '请先登录' });
                  router.pushUrl({ url: 'pages/LoginPage' });
                  return;
                }
                router.pushUrl({ url: 'pages/OrderList', params: { tab: 3 } });
              });
              this.QuickItem('待评价', '评', () => {
                if (!this.currentUser) {
                  promptAction.showToast({ message: '请先登录' });
                  return;
                }
                router.pushUrl({ url: 'pages/OrderList', params: { tab: 4 } });
              });
              this.QuickItem('退换/售后', '退', () => {
                if (!this.currentUser) {
                  promptAction.showToast({ message: '请先登录' });
                  router.pushUrl({ url: 'pages/LoginPage' });
                  return;
                }
                router.pushUrl({ url: 'pages/OrderList', params: { tab: 5 } });
              });
            }
            .width('100%');
          }
          .padding(16)
          .backgroundColor(Color.White)
          .borderRadius(18)
          .border({ width: 1, color: '#EEF2F7' })
          .shadow({ radius: 10, color: '#0F000000', offsetY: 2 });

          Column({ space: 10 }) {
            this.SectionHeader('常用工具');

            this.ToolRow('收货地址', '址', () => {
              if (!this.currentUser) {
                promptAction.showToast({ message: '请先登录' });
                router.pushUrl({ url: 'pages/LoginPage' });
                return;
              }
              router.pushUrl({ url: 'pages/AddressManage' });
            });

            this.ToolRow('我的收藏', '藏', () => {
              if (!this.currentUser) {
                promptAction.showToast({ message: '请先登录' });
                router.pushUrl({ url: 'pages/LoginPage' });
                return;
              }
              router.pushUrl({ url: 'pages/FavoriteList' });
            });

            this.ToolRow('我的足迹', '迹', () => {
              if (!this.currentUser) {
                promptAction.showToast({ message: '请先登录' });
                router.pushUrl({ url: 'pages/LoginPage' });
                return;
              }
              router.pushUrl({ url: 'pages/FootprintList' });
            });

            if (this.currentUser) {
              Button('退出登录')
                .height(44)
                .fontSize(14)
                .fontColor(Color.White)
                .backgroundColor('#FF4D4F')
                .borderRadius(22)
                .margin({ top: 6 })
                .onClick(async () => {
                  await RdbUtil.setSessionUserId(0);
                  AppStorage.setOrCreate('currentUser', undefined);
                  promptAction.showToast({ message: '已退出' });
                });
            }
          }
          .padding(16)
          .backgroundColor(Color.White)
          .borderRadius(18)
          .border({ width: 1, color: '#EEF2F7' })
          .shadow({ radius: 10, color: '#0F000000', offsetY: 2 });

          Column() {
          }
          .height(14);
        }
        .padding({ left: 12, right: 12, top: 12, bottom: 12 });
      }
      .layoutWeight(1)
      .backgroundColor($r('app.color.mine_page_bg'));
    }
    .width('100%')
    .height('100%')
    .backgroundColor($r('app.color.mine_page_bg'));
  }
}