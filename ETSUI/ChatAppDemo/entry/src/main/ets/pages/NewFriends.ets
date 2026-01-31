/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { router, promptAction } from '@kit.ArkUI';
import { dbManager } from '../utils/DatabaseManager';
import { FriendRequestModel } from '../viewmodel/FriendRequestModel';
import { UserModel } from '../viewmodel/UserModel';
import { emitter } from '@kit.BasicServicesKit';

@Entry
@Component
struct NewFriends {
  // 获取当前登录用户
  @StorageProp('currentUser') currentUser: UserModel | null = null;
  // 存放从数据库查到的申请列表
  @State requests: FriendRequestModel[] = [];

  // 页面每次显示时都刷新列表（比 aboutToAppear 更稳妥，因为从添加页返回时也需要刷新）
  onPageShow() {
    this.loadRequests();
  }

  async loadRequests() {
    if (this.currentUser) {
      // 获取发给我的所有申请
      const result = await dbManager.friend?.getMyRequests(this.currentUser.phoneNumber);
      this.requests = result || [];
    }
  }

  build() {
    Column() {
      // 顶部导航栏
      Row() {
        Image($r('sys.media.ohos_ic_back'))
          .width(24).height(24)
          .onClick(() => router.back())
        Text($r('app.string.label_new_friends')).fontSize(18).fontWeight(FontWeight.Bold).margin({ left: 15 })
        Blank()
        Text($r('app.string.title_add_friends'))
          .fontColor('#576B95')
          .fontSize(16)          .onClick(() => router.pushUrl({ url: 'pages/AddFriends' }))
      }
      .width('100%').padding(15).backgroundColor('#F1F3F5')

      // 搜索入口
      Search({ placeholder: $r('app.string.hint_account_phone') })        .margin(10)
        .onClick(() => router.pushUrl({ url: 'pages/AddFriends' }))

      List() {
        ForEach(this.requests, (item: FriendRequestModel, index: number) => {
          ListItem() {
            Row() {
              // 注意：这里的头像如果是对方的，目前我们申请表只存了昵称，
              // 建议后续给 FriendRequest 表增加 fromAvatar 字段。
              // 现在先用默认头像演示。
              Image($r('app.media.user_default'))
                .width(50).height(50).borderRadius(5)

              Column() {
                Text(item.fromName).fontSize(16).fontWeight(FontWeight.Medium)
                Text(item.message).fontSize(14).fontColor(Color.Gray).margin({ top: 5 })
              }.alignItems(HorizontalAlign.Start).margin({ left: 10 }).layoutWeight(1)

              if (item.status === 0) {
                // 待处理状态
                Row() {
                  Button($r('app.string.btn_accept')).height(30).fontSize(14).backgroundColor('#07C160').margin({ right: 5 })
                    .onClick(async () => {
                      await this.handleAction(item, 1, index);
                    })
                  Button($r('app.string.btn_reject')).height(30).fontSize(14).backgroundColor('#CD0000')
                    .onClick(async () => {
                      await this.handleAction(item, 2, index);
                    })
                }
              } else {
                // 已处理状态（1:已添加, 2:已拒绝）
                Text(item.status === 1 ? $r('app.string.label_added') : $r('app.string.label_rejected'))
                  .fontColor(Color.Gray)
                  .fontSize(14)
              }
            }
            .width('100%').padding(15)
          }
        })
      }
      .divider({ strokeWidth: 0.5, color: '#EEEEEE', startMargin: 75 })
      .backgroundColor(Color.White)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }

  async handleAction(item: FriendRequestModel, newStatus: number, index: number) {
    if (!this.currentUser || !item.id) return;

    try {
      // 调用 FriendHandler 处理申请
      // 如果 status 是 1 (接受)，Handler 内部会自动往 Friend 表插两条记录
      await dbManager.friend?.handleRequest(
        item.id,
        newStatus,
        item.fromPhone,
        this.currentUser.phoneNumber
      );

      // 更新本地状态刷新 UI
      this.requests[index].status = newStatus;
      // ArkTS 的数组索引更新有时不触发渲染，保险做法是整体重新赋值一次
      this.requests = [...this.requests];

      promptAction.showToast({ message: newStatus === 1 ? $r('app.string.toast_approved') : $r('app.string.label_rejected') });

      if (newStatus === 1) { // 如果是接受好友
        // 发送更新通知
        emitter.emit({ eventId: 10001 });
      }
    } catch (e) {
      promptAction.showToast({ message: $r('app.string.toast_op_fail') });
    }

  }
}
