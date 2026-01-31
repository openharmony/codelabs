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
import { UserModel } from '../viewmodel/UserModel';

@Entry
@Component struct AddFriends {
  @State searchText: string = '';
  // 搜索结果直接使用 UserModel 类型
  @State searchResult: UserModel | null = null;

  // 获取当前登录用户，作为“申请人”
  @StorageProp('currentUser') currentUser: UserModel | null = null;

  async handleSearch() {
    if (this.searchText.trim() === '') return;

    // 1. 调用 UserHandler 里的查询方法
    const user = await dbManager.user?.queryUserByPhone(this.searchText);

    if (user) {
      // 2. 检查是不是搜到了自己（不能加自己为好友）
      if (user.phoneNumber === this.currentUser?.phoneNumber) {
        promptAction.showToast({ message: $r('app.string.toast_cant_add_self') });
        this.searchResult = null;
        return;
      }
      this.searchResult = user;
    } else {
      this.searchResult = null;
      promptAction.showToast({ message: $r('app.string.toast_user_not_found') });
    }
  }

  build() {
    Column() {
      // 头部
      Row() {
        Image($r('sys.media.ohos_ic_back'))
          .width(24).height(24)
          .onClick(() => router.back())
        Text($r('app.string.title_add_friends')).fontSize(18).fontWeight(FontWeight.Bold).margin({ left: 15 })
      }
      .width('100%').padding(15)

      // 搜索框
      Search({ placeholder: $r('app.string.hint_account_phone'), value: this.searchText })
        .margin(15)
        .onChange((val) => this.searchText = val)
        .onSubmit(() => this.handleSearch())

      Text(`我的手机号：${this.currentUser?.phoneNumber || $r('app.string.label_not_logged_in')}`)
        .fontSize(14)
        .fontColor(Color.Gray)
        .margin({ bottom: 20 })

      // 搜索结果展示
      if (this.searchResult) {
        Column() {
          Row() {
            // 头像使用数据库存的路径，alt 是兜底图
            Image(this.searchResult.avatarPath)
              .width(50).height(50).borderRadius(5)
              .alt($r('app.media.user_default'))

            Column() {
              Text(this.searchResult.nickName).fontSize(16).fontWeight(FontWeight.Bold)
              Text(`手机号: ${this.searchResult.phoneNumber}`).fontSize(14).fontColor(Color.Gray)
            }.alignItems(HorizontalAlign.Start).margin({ left: 15 }).layoutWeight(1)

            Button($r('app.string.btn_add'))
              .height(30)
              .backgroundColor('#07C160')
              .onClick(async () => {
                if (!this.currentUser) {
                  promptAction.showToast({ message: $r('app.string.toast_login_exception') });
                  return;
                }

                // 拦截：是否已经是好友
                const alreadyFriend = await dbManager.friend?.isFriend(this.currentUser.phoneNumber, this.searchResult!.phoneNumber);
                if (alreadyFriend) {
                  promptAction.showToast({ message: $r('app.string.toast_already_friend') });
                  return;
                }

                // 拦截：是否已经发送过申请且对方未处理
                const isPending = await dbManager.friend?.checkRequestExists(this.currentUser.phoneNumber, this.searchResult!.phoneNumber);
                if (isPending) {
                  promptAction.showToast({ message: $r('app.string.toast_request_sent') });
                  return;
                }

                // 调用 FriendHandler 发送申请
                try {
                  await dbManager.friend?.sendRequest({
                    fromPhone: this.currentUser.phoneNumber,
                    toPhone: this.searchResult!.phoneNumber,
                    fromName: this.currentUser.nickName,
                    message: "你好,我是" + this.currentUser.nickName,
                    status: 0
                  });
                  promptAction.showToast({ message: $r('app.string.toast_sent_success') });
                  // 发送完可以考虑延迟返回或清空结果
                } catch (e) {
                  promptAction.showToast({ message: $r('app.string.toast_send_fail') });
                }
              })
          }
          .width('100%')
          .padding(15)
          .backgroundColor(Color.White)
        }
      } else if (this.searchText === '') {
        // 展示一些默认的入口
        this.buildMenuOption($r('app.media.qrcode'), $r('app.string.label_scan'), $r('app.string.label_scan_desc'))
        this.buildMenuOption($r('app.media.contactpp'), $r('app.string.label_phone_contacts'), $r('app.string.label_phone_contacts_desc'))
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')  }

  @Builder
  buildMenuOption(icon: Resource, title: ResourceStr, subTitle: ResourceStr) {
    Row() {
      Image(icon).width(25).height(25).margin({ left: 15 })
      Column() {
        Text(title).fontSize(16)
        Text(subTitle).fontSize(12).fontColor(Color.Gray).margin({ top: 2 })
      }.alignItems(HorizontalAlign.Start).margin({ left: 15 })
    }
    .width('100%')
    .height(70)
    .backgroundColor(Color.White)
    .margin({ top: 1 })
  }
}
