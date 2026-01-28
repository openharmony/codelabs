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

  // 关键修改点 1：将类型显式改为 ResourceStr（兼容 Resource 和 string）
  @State targetAvatar: ResourceStr = $r('app.media.user_default');

  private scroller: Scroller = new Scroller();

  async aboutToAppear() {
    // 获取路由参数
    const params = router.getParams() as Record<string, Object>;
    if (params && params.contact) {
      // 关键修改点 2：将 contact 断言为 Message 类型
      // 既然 Message.avatar 已经是 ResourceStr，这里的赋值就不会报错了
      const contact = params.contact as Message;
      this.chatTarget = contact.nickName;
      this.targetAvatar = contact.avatar;

      if (dbManager.chat) {
        // 从数据库加载历史记录
        this.messageList = await dbManager.chat.queryHistoryByContact(this.chatTarget);
      }
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
        Text(this.chatTarget).fontSize(18).fontWeight(FontWeight.Bold).margin({ left: 10 })
        Blank()
      }.width('100%').height(56).backgroundColor('#F1F3F5')

      // 2. 聊天内容区
      List({ scroller: this.scroller }) {
        ForEach(this.messageList, (msg: Message) => {
          ListItem() {
            this.MessageBubble(msg)
          }
        }, (item: Message) => item.id)
      }
      .layoutWeight(1).width('100%').backgroundColor('#EDEDED')

      // 3. 底部输入栏
      Row() {
        TextInput({ text: this.inputText, placeholder: '请输入...' })
          .layoutWeight(1).height(40).backgroundColor(Color.White)
          .onChange((v) => this.inputText = v)
        Button('发送').margin({ left: 10 })
          .onClick(() => this.sendMessage())
      }.padding(10).backgroundColor('#F7F7F7')
    }.width('100%').height('100%')
  }

  @Builder MessageBubble(msg: Message) {
    Row() {
      if (msg.isSelf) {
        // 自己发的消息
        Row() {
          Blank()
          Text(msg.content).padding(10).backgroundColor('#95EC69').borderRadius(6).margin({ right: 10 })
          // 自己的头像
          Image($r('app.media.user_default')).width(42).height(42).borderRadius(4)
        }.width('100%').padding({ left: 50, right: 10 }).alignItems(VerticalAlign.Top)
      } else {
        // 对方发的消息
        Row() {
          // 使用动态获取的对方头像
          Image(this.targetAvatar).width(42).height(42).borderRadius(4)
          Text(msg.content).padding(10).backgroundColor(Color.White).borderRadius(6).margin({ left: 10 })
          Blank()
        }.width('100%').padding({ left: 10, right: 50 }).alignItems(VerticalAlign.Top)
      }
    }.width('100%').margin({ top: 12 })
  }

  // entry/src/main/ets/pages/Chat.ets

  async sendMessage() {
    // 1. 非空校验
    if (this.inputText.trim().length === 0) {
      return;
    }

    try {
      const now = new Date();
      const timeStr = `${now.getHours().toString().padStart(2, '0')}:${now.getMinutes().toString().padStart(2, '0')}`;
      const msgId = Date.now().toString();

      // 2. 构造存入数据库的对象（严格按照字段一个一个赋值，不使用 ... 展开）
      // 注意：这里存入的是对方的头像和昵称，以便首页 ChatList 能正确显示
      const dbEntry: Message = {
        id: msgId,
        avatar: this.targetAvatar,
        nickName: this.chatTarget,
        content: this.inputText,
        time: timeStr,
        isSelf: true
      };

      // 3. 先执行数据库写入
      if (dbManager.chat) {
        await dbManager.chat.insertMessage(dbEntry);
        console.info('消息已成功存入数据库');
      }

      // 4. 构造 UI 显示的消息（自己发的消息，头像显示默认头像/自己头像）
      const uiMsg: Message = {
        id: msgId,
        avatar: $r('app.media.user_default'), // 自己的头像
        nickName: '我',
        content: this.inputText,
        time: timeStr,
        isSelf: true
      };

      // 5. 更新状态变量触发 UI 渲染
      this.messageList.push(uiMsg);

      // 6. 清空输入框
      this.inputText = '';

      // 7. 滚动到底部
      setTimeout(() => {
        this.scroller.scrollEdge(Edge.Bottom);
      }, 100);

    } catch (error) {
      console.error('发送消息失败:', JSON.stringify(error));
    }
  }
}