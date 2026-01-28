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
        promptAction.showToast({ message: '不能添加自己为好友哦' });
        this.searchResult = null;
        return;
      }
      this.searchResult = user;
    } else {
      this.searchResult = null;
      promptAction.showToast({ message: '未找到该用户，请检查手机号' });
    }
  }

  build() {
    Column() {
      // 头部
      Row() {
        Image($r('sys.media.ohos_ic_back'))
          .width(24).height(24)
          .onClick(() => router.back())
        Text('添加朋友').fontSize(18).fontWeight(FontWeight.Bold).margin({ left: 15 })
      }
      .width('100%').padding(15)

      // 搜索框
      Search({ placeholder: '账号/手机号', value: this.searchText })
        .margin(15)
        .onChange((val) => this.searchText = val)
        .onSubmit(() => this.handleSearch())

      Text(`我的手机号：${this.currentUser?.phoneNumber || '未登录'}`)
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

            Button('添加')
              .height(30)
              .backgroundColor('#07C160')
              .onClick(async () => {
                if (!this.currentUser) {
                  promptAction.showToast({ message: '登录状态异常' });
                  return;
                }

                // 拦截：是否已经是好友
                const alreadyFriend = await dbManager.friend?.isFriend(this.currentUser.phoneNumber, this.searchResult!.phoneNumber);
                if (alreadyFriend) {
                  promptAction.showToast({ message: '对方已经是你的好友了' });
                  return;
                }

                // 拦截：是否已经发送过申请且对方未处理
                const isPending = await dbManager.friend?.checkRequestExists(this.currentUser.phoneNumber, this.searchResult!.phoneNumber);
                if (isPending) {
                  promptAction.showToast({ message: '申请已发送，请耐心等待对方通过' });
                  return;
                }

                // 调用 FriendHandler 发送申请
                try {
                  await dbManager.friend?.sendRequest({
                    fromPhone: this.currentUser.phoneNumber,
                    toPhone: this.searchResult!.phoneNumber,
                    fromName: this.currentUser.nickName,
                    message: '你好，我是' + this.currentUser.nickName,
                    status: 0
                  });
                  promptAction.showToast({ message: '申请已发送' });
                  // 发送完可以考虑延迟返回或清空结果
                } catch (e) {
                  promptAction.showToast({ message: '发送失败，请重试' });
                }
              })
          }
          .width('100%')
          .padding(15)
          .backgroundColor(Color.White)
        }
      } else if (this.searchText === '') {
        // 展示一些默认的入口
        this.buildMenuOption($r('app.media.qrcode'), '扫一扫', '扫描二维码名片')
        this.buildMenuOption($r('app.media.contactpp'), '手机联系人', '添加或邀请通讯录中的朋友')
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')  }

  @Builder
  buildMenuOption(icon: Resource, title: string, subTitle: string) {
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
