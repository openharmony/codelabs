// entry/src/main/ets/pages/Chat.ets
import Message from '../viewmodel/Message'
import router from '@ohos.router'

@Entry
@Component
struct Chat {
  @State messageList: Message[] = [
    { id: '1', avatar: $r('app.media.user1'), nickName: '桑良翰', content: '你在干啥', time: '18:23', isSelf: false },
    { id: '2', avatar: $r('app.media.user2'), nickName: '自己', content: '在写鸿蒙代码呢', time: '18:24', isSelf: true }
  ]
  @State inputText: string = ''
  private scroller: Scroller = new Scroller()

  // 发送消息方法
  sendMessage() {
    if (this.inputText.trim().length > 0) {
      const newMsg: Message = {
        id: Date.now().toString(),
        avatar: $r('app.media.user2'), // 假设这是用户的头像
        nickName: '自己',
        content: this.inputText,
        time: '18:25',
        isSelf: true
      }
      this.messageList.push(newMsg)
      this.inputText = ''
      // 发送后滚动到底部
      setTimeout(() => {
        this.scroller.scrollToIndex(this.messageList.length - 1)
      }, 100)
    }
  }

  build() {
    Column() {
      // 1. 顶部导航栏
      Row() {
        Image($r('app.media.back')) // 需自备返回图标
          .width(24).height(24)
          .onClick(() => router.back())
        Text('桑良翰')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ left: 15 })
        Blank()
      }
      .width('100%')
      .height(56)
      .padding({ left: 15, right: 15 })
      .backgroundColor('#F1F1F1')

      // 2. 消息列表区域
      List({ scroller: this.scroller }) {
        ForEach(this.messageList, (msg: Message) => {
          ListItem() {
            this.ChatBubble(msg)
          }
        })
      }
      .layoutWeight(1)
      .width('100%')
      .padding({ left: 10, right: 10 })
      .backgroundColor('#EDEDED')
      .edgeEffect(EdgeEffect.Spring) // 列表回弹效果

      // 3. 底部输入区域
      Row() {
        TextInput({ text: this.inputText, placeholder: '输入消息...' })
          .layoutWeight(1)
          .height(40)
          .backgroundColor(Color.White)
          .onChange((value) => this.inputText = value)
          .onSubmit(() => this.sendMessage())

        Button('发送')
          .margin({ left: 10 })
          .height(35)
          .width(70)
          .onClick(() => this.sendMessage())
          .enabled(this.inputText.trim().length > 0)
      }
      .width('100%')
      .padding(10)
      .backgroundColor('#F7F7F7')
    }
    .width('100%')
    .height('100%')
  }

  // 消息气泡组件
  @Builder ChatBubble(msg: Message) {
    Row() {
      if (msg.isSelf) {
        Blank() // 自己发的消息，头像在右侧
        Row() {
          Text(msg.content)
            .fontSize(16)
            .fontColor(Color.Black)
            .backgroundColor('#95EC69') // 微信绿
            .padding(10)
            .borderRadius(4)
        }
        .margin({ right: 10 })
        .constraintSize({ maxWidth: '70%' })

        Image(msg.avatar)
          .width(40).height(40).borderRadius(4)
      } else {
        // 对方发的消息
        Image(msg.avatar)
          .width(40).height(40).borderRadius(4)
        Row() {
          Text(msg.content)
            .fontSize(16)
            .fontColor(Color.Black)
            .backgroundColor(Color.White)
            .padding(10)
            .borderRadius(4)
        }
        .margin({ left: 10 })
        .constraintSize({ maxWidth: '70%' })
        Blank()
      }
    }
    .width('100%')
    .margin({ top: 10, bottom: 10 })
    .alignItems(VerticalAlign.Top)
  }
}