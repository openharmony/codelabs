// entry/src/main/ets/pages/FriendDetail.ets
import { router, promptAction } from '@kit.ArkUI';
import { emitter } from '@kit.BasicServicesKit';
import { UserModel } from '../viewmodel/UserModel';
import { dbManager } from '../utils/DatabaseManager';
import Message from '../viewmodel/Message';

@Entry
@Component
struct FriendDetail {
  @StorageProp('currentUser') currentUser: UserModel | null = null;
  @State friend: UserModel | null = null;

  async aboutToAppear(): Promise<void> {
    const params = router.getParams() as Record<string, Object>;
    if (params && params.friend) {
      this.friend = params.friend as UserModel;
    }
  }

  // 删除好友操作
  async handleDelete(): Promise<void> {
    if (!this.currentUser || !this.friend || !dbManager.friend) return;

    await dbManager.friend.deleteFriend(this.currentUser.phoneNumber, this.friend.phoneNumber);

    // 发送信号刷新通讯录 (10001)
    emitter.emit({ eventId: 10001 });

    promptAction.showToast({ message: '已成功删除好友' });
    router.back();
  }

  build() {
    Column() {
      // 1. 导航栏
      Row() {
        Image($r('app.media.back'))
          .width(24).height(24)
          .onClick(() => router.back())
      }.width('100%').padding(15)

      // 2. 好友基本信息卡片
      Row() {
        Image(this.friend?.avatarPath || $r('app.media.user_default'))
          .width(70).height(70).borderRadius(10)

        Column() {
          Text(this.friend?.nickName || '未知用户')
            .fontSize(22).fontWeight(FontWeight.Bold)
          Text(`手机号: ${this.friend?.phoneNumber || ''}`)
            .fontSize(14).fontColor(Color.Gray).margin({ top: 5 })
          Text(`地区: ${this.friend?.country || '未知'}`)
            .fontSize(14).fontColor(Color.Gray).margin({ top: 2 })
        }
        .alignItems(HorizontalAlign.Start).margin({ left: 20 })
      }
      .width('100%').padding(20).backgroundColor(Color.White)

      Divider().color('#F1F3F5').strokeWidth(10)

      // 3. 底部功能按钮
      Column() {
        // 发消息按钮
        Button('发消息', { type: ButtonType.Normal })
          .width('100%').height(50).backgroundColor(Color.White)
          .fontColor('#576B95').fontWeight(600)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/Chat',
              params: {
                contact: {
                  id: this.friend?.phoneNumber,
                  nickName: this.friend?.nickName,
                  avatar: this.friend?.avatarPath || $r('app.media.user_default'),
                  content: '', time: '', isSelf: false
                } as Message
              }
            })
          })

        Divider().color('#F1F3F5').strokeWidth(1).margin({ left: 20, right: 20 })

        // 删除好友按钮
        Button('删除好友', { type: ButtonType.Normal })
          .width('100%').height(50).backgroundColor(Color.White)
          .fontColor('#FA5151').fontWeight(600)
          .onClick(() => {
            this.handleDelete();
          })
      }
      .margin({ top: 20 })
    }
    .width('100%').height('100%').backgroundColor('#F1F3F5')
  }
}