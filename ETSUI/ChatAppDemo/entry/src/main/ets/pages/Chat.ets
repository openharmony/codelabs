// entry/src/main/ets/pages/Chat.ets
import router from '@ohos.router';
import Message from '../viewmodel/Message';
import { dbManager } from '../utils/DatabaseManager';

@Entry
@Component
struct Chat {
  @State chatTarget: string = '聊天';
  @State messageList: Message[] = [];
  @State inputText: string = '';
  private scroller: Scroller = new Scroller();

  async aboutToAppear() {
    const params = router.getParams() as Record<string, Object>;
    if (params && params.contact) {
      const contact = params.contact as Message;
      this.chatTarget = contact.nickName;

      // 初始加载一条对方的消息作为演示
      const initMsg: Message = {
        id: 'init_' + contact.id,
        avatar: contact.avatar,
        nickName: contact.nickName,
        content: contact.content,
        time: contact.time,
        isSelf: false
      };
      this.messageList.push(initMsg);
    }
  }

  build() {
    Column() {
      // 1. 顶部导航栏
      Row() {
        Image($r('app.media.back'))
          .width(24).height(24)
          .margin({ left: 15 })
          .onClick(() => router.back())

        Text(this.chatTarget)
          .fontSize(18).fontWeight(FontWeight.Bold).margin({ left: 10 })
        Blank()
      }
      .width('100%').height(56).backgroundColor('#F1F3F5')

      // 2. 消息列表区域
      List({ scroller: this.scroller }) {
        ForEach(this.messageList, (msg: Message) => {
          ListItem() {
            this.MessageBubble(msg)
          }
        }, (item: Message) => item.id)
      }
      .layoutWeight(1)
      .width('100%')
      .backgroundColor('#EDEDED') // 类似微信的聊天背景色
      .edgeEffect(EdgeEffect.Spring)

      // 3. 底部输入区域
      Row() {
        TextInput({ text: this.inputText, placeholder: '请输入...' })
          .layoutWeight(1)
          .height(40)
          .backgroundColor(Color.White)
          .borderRadius(4)
          .onChange((value: string) => this.inputText = value)

        Button('发送')
          .margin({ left: 10 })
          .onClick(() => this.sendMessage())
      }
      .width('100%')
      .padding(10)
      .backgroundColor('#F7F7F7')
    }
    .width('100%')
    .height('100%')
  }

  // 精修版气泡组件
  @Builder MessageBubble(msg: Message) {
    Row() {
      if (msg.isSelf) {
        // --- 自己发送的消息（靠右） ---
        Row() {
          Blank() // 占据左侧空间
          Column() {
            Text(msg.content)
              .fontSize(16)
              .fontColor('#181818')
              .padding(10)
              .backgroundColor('#95EC69') // 微信绿
              .borderRadius(6)
          }
          .margin({ right: 10 })
          .constraintSize({ maxWidth: '70%' }) // 限制气泡最大宽度

          Image(msg.avatar)
            .width(42).height(42).borderRadius(4)
        }
        .width('100%')
        .alignItems(VerticalAlign.Top)
        .padding({ left: 50, right: 15 }) // 左侧留白给对方头像位
      } else {
        // --- 对方发送的消息（靠左） ---
        Row() {
          Image(msg.avatar)
            .width(42).height(42).borderRadius(4)

          Column() {
            Text(msg.content)
              .fontSize(16)
              .fontColor('#181818')
              .padding(10)
              .backgroundColor(Color.White) // 微信白
              .borderRadius(6)
          }
          .margin({ left: 10 })
          .constraintSize({ maxWidth: '70%' }) // 限制气泡最大宽度
          .alignItems(HorizontalAlign.Start)

          Blank() // 占据右侧空间
        }
        .width('100%')
        .alignItems(VerticalAlign.Top)
        .padding({ left: 15, right: 50 }) // 右侧留白给对方头像位
      }
    }
    .width('100%')
    .margin({ top: 12, bottom: 12 })
  }

  async sendMessage() {
    if (this.inputText.trim().length > 0) {
      const now = new Date();
      const timeStr = `${now.getHours()}:${now.getMinutes()}`;

      const newMsg: Message = {
        id: Date.now().toString(),
        avatar: $r('app.media.user_default'), // 确保资源存在
        nickName: '我',
        content: this.inputText,
        time: timeStr,
        isSelf: true
      };

      this.messageList.push(newMsg);

      if (dbManager.chat) {
        const dbMsg: Message = {
          id: newMsg.id,
          avatar: newMsg.avatar,
          nickName: this.chatTarget,
          content: newMsg.content,
          time: newMsg.time,
          isSelf: newMsg.isSelf
        };
        await dbManager.chat.insertMessage(dbMsg);
      }

      this.inputText = '';
      this.scroller.scrollEdge(Edge.Bottom);
    }
  }
}