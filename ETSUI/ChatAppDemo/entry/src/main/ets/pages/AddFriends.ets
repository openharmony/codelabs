import { router, promptAction } from '@kit.ArkUI';

interface UserResult {
  name: string;
  wechatId: string;
  avatar: Resource;
}

@Entry
@Component struct AddFriends {
  @State searchText: string = '';
  @State searchResult: UserResult | null = null;

  // 模拟数据库
  private mockUsers: UserResult[] = [
    { name: 'HarmonyOS高手', wechatId: 'user2', avatar: $r('app.media.user12') },
    { name: '华为开发者', wechatId: 'user1', avatar: $r('app.media.user11') }
  ];

  handleSearch() {
    // 模拟搜索逻辑
    const result = this.mockUsers.find(u => u.wechatId === this.searchText || u.name.includes(this.searchText));
    if (result) {
      this.searchResult = result;
    } else {
      this.searchResult = null;
      if (this.searchText.length > 0) {
        promptAction.showToast({ message: '未找到该用户' });
      }
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

      Text(`我的微信号：wx_123456`).fontSize(14).fontColor(Color.Gray).margin({ bottom: 20 })

      // 搜索结果展示
      if (this.searchResult) {
        Column() {
          Row() {Image(this.searchResult.avatar).width(50).height(50).borderRadius(5)
            Column() {
              Text(this.searchResult.name).fontSize(16).fontWeight(FontWeight.Bold)
              Text(`微信号: ${this.searchResult.wechatId}`).fontSize(14).fontColor(Color.Gray)
            }.alignItems(HorizontalAlign.Start).margin({ left: 15 }).layoutWeight(1)

            Button('添加')
              .height(30)
              .backgroundColor('#07C160')
              .onClick(() => promptAction.showToast({ message: '申请已发送' }))
          }
          .width('100%')
          .padding(15)
          .backgroundColor(Color.White)
        }
        .width('100%')
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
