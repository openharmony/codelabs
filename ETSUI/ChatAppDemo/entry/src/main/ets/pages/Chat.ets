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
    // 获取从 ChatList 传递过来的联系人参数
    const params = router.getParams() as Record<string, Object>;
    if (params && params.contact) {
      const contact = params.contact as Message;
      this.chatTarget = contact.nickName;

      // 加载历史记录（目前演示加载该联系人的初始消息）
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
          .onClick(() => {
            router.back();
          })

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
      .padding({ left: 10, right: 10 })
      .edgeEffect(EdgeEffect.Spring)

      // 3. 底部输入区域
      Row() {
        TextInput({ text: this.inputText, placeholder: '请输入消息...' })
          .layoutWeight(1)
          .height(40)
          .backgroundColor(Color.White)
          .onChange((value: string) => this.inputText = value)

        Button('发送')
          .margin({ left: 10 })
          .onClick(() => {
            this.sendMessage();
          })
      }
      .width('100%')
      .padding(10)
      .backgroundColor('#F7F7F7')
    }
    .width('100%')
    .height('100%')
  }

  @Builder MessageBubble(msg: Message) {
    Row() {
      if (msg.isSelf) Blank()

      Row() {
        if (!msg.isSelf) Image(msg.avatar).width(40).height(40).borderRadius(5)

        Column() {
          Text(msg.content)
            .fontSize(16)
            .padding(10)
            .backgroundColor(msg.isSelf ? '#95EC69' : Color.White)
            .borderRadius(8)
        }
        .alignItems(msg.isSelf ? HorizontalAlign.End : HorizontalAlign.Start)
        .margin({ left: 8, right: 8 })

        if (msg.isSelf) Image(msg.avatar).width(40).height(40).borderRadius(5)
      }
      .alignItems(VerticalAlign.Top)
      .margin({ top: 10, bottom: 10 })

      if (!msg.isSelf) Blank()
    }
    .width('100%')
  }

  // 发送逻辑
  async sendMessage() {
    if (this.inputText.trim().length > 0) {
      const now = new Date();
      const timeStr = `${now.getHours()}:${now.getMinutes()}`;

      // 创建 UI 显示的消息对象
      const newMsg: Message = {
        id: Date.now().toString(),
        avatar: $r('app.media.user_default'), // 确保资源存在
        nickName: '我',
        content: this.inputText,
        time: timeStr,
        isSelf: true
      };

      // 1. 更新 UI
      this.messageList.push(newMsg);

      // 2. 持久化到数据库
      if (dbManager.chat) {
        // 【核心修复】：不再使用 ...newMsg 展开运算符
        // 手动构建用于存入数据库的消息对象，并将 nickName 设为当前聊天对象
        const dbMsg: Message = {
          id: newMsg.id,
          avatar: newMsg.avatar,
          nickName: this.chatTarget, // 这里存入对方名字，确保 ChatList 分组正确
          content: newMsg.content,
          time: newMsg.time,
          isSelf: newMsg.isSelf
        };
        await dbManager.chat.insertMessage(dbMsg);
      }

      this.inputText = '';
      setTimeout(() => {
        this.scroller.scrollEdge(Edge.Bottom);
      }, 100);
    }
  }
}