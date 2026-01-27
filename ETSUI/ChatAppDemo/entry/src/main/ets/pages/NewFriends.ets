import { router, promptAction } from '@kit.ArkUI';

interface FriendRequest {
  id: string;
  name: string;
  avatar: Resource;
  message: string;
  status: number; // 0: 待处理, 1: 已添加, 2: 已拒绝
}

@Entry@Component
struct NewFriends {
  @State requests: FriendRequest[] = [
    { id: '1', name: '李华', avatar: $r('app.media.user9'), message: '我是三年级二班李华', status: 0 },
    { id: '2', name: '张三', avatar: $r('app.media.user8'), message: '你好，想请教个问题', status: 1 },
    { id: '3', name: '王五', avatar: $r('app.media.user7'), message: '扩列', status: 0 },
  ];

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
        ForEach(this.requests, (item: FriendRequest, index: number) => {
          ListItem() {
            Row() {
              Image(item.avatar).width(50).height(50).borderRadius(5)
              Column() {
                Text(item.name).fontSize(16).fontWeight(FontWeight.Medium)
                Text(item.message).fontSize(14).fontColor(Color.Gray).margin({ top: 5 })
              }.alignItems(HorizontalAlign.Start).margin({ left: 10 }).layoutWeight(1)

              if (item.status === 0) {
                Button('接受').height(30).fontSize(14).backgroundColor('#07C160').margin(5).onClick(() => {
                  this.requests[index].status = 1;
                  promptAction.showToast({ message: '已添加' });
                })
                Button('拒绝').height(30).fontSize(14).backgroundColor('#CD0000').margin(5).onClick(() => {
                  this.requests[index].status = 1;
                  promptAction.showToast({ message: '已拒绝' });
                })
              } else {
                Text(item.status === 1 ? '已添加' : '已拒绝').fontColor(Color.Gray).fontSize(14)
              }
            }
            .width('100%').padding(15)
          }
        })
      }
      .divider({ strokeWidth: 0.5, color: '#EEEEEE', startMargin: 75 }).backgroundColor(Color.White)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}
