// NewFriends.ets
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
        Text('新的朋友').fontSize(18).fontWeight(FontWeight.Bold).margin({ left: 15 })
        Blank()
        Text('添加朋友')
          .fontColor('#576B95')
          .fontSize(16)          .onClick(() => router.pushUrl({ url: 'pages/AddFriends' }))
      }
      .width('100%').padding(15).backgroundColor('#F1F3F5')

      // 搜索入口
      Search({ placeholder: '账号/手机号' })        .margin(10)
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
                  Button('接受').height(30).fontSize(14).backgroundColor('#07C160').margin({ right: 5 })
                    .onClick(async () => {
                      await this.handleAction(item, 1, index);
                    })
                  Button('拒绝').height(30).fontSize(14).backgroundColor('#CD0000')
                    .onClick(async () => {
                      await this.handleAction(item, 2, index);
                    })
                }
              } else {
                // 已处理状态（1:已添加, 2:已拒绝）
                Text(item.status === 1 ? '已添加' : '已拒绝')
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

      promptAction.showToast({ message: newStatus === 1 ? '已通过申请' : '已拒绝' });

      if (newStatus === 1) { // 如果是接受好友
        // 发送更新通知
        emitter.emit({ eventId: 10001 });
      }
    } catch (e) {
      promptAction.showToast({ message: '操作失败' });
    }

  }
}
