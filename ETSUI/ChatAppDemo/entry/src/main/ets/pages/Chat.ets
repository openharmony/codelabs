// entry/src/main/ets/pages/Chat.ets
import router from '@ohos.router';
import { UserModel } from '../viewmodel/UserModel'; // 确保引入了 UserModel
import Message from '../viewmodel/Message';
import { dbManager } from '../utils/DatabaseManager';

@Entry
@Component
struct Chat {
  @StorageProp('currentUser') currentUser: UserModel | null = null;

  @State chatTarget: string = '聊天';
  @State targetAvatar: ResourceStr = $r('app.media.user_default');
  @State targetPhone: string = ''; // 新增：保存对方的手机号用于数据库查询
  @State messageList: Message[] = [];
  @State inputText: string = '';
  private scroller: Scroller = new Scroller();

  async aboutToAppear() {
    const params = router.getParams() as Record<string, Object>;
    if (params && params.contact) {
      const contact = params.contact as Message;
      this.chatTarget = contact.nickName;
      this.targetAvatar = contact.avatar;
      this.targetPhone = contact.id; // 【修复 2】将路由带过来的 id 赋值给 targetPhone

      if (dbManager.chat && this.currentUser) {
        // 加载历史记录时，传入当前用户和目标用户，防止混淆
        // 注意：这里的 queryHistoryByContact 逻辑需要匹配 ChatHandler
        this.messageList = await dbManager.chat.queryHistoryByContact(this.targetPhone);
      }
    }
  }

  // 发送消息逻辑
  async sendMessage() {
    if (this.inputText.trim().length === 0 || !this.currentUser) return;

    const content = this.inputText;
    const timeStr = `${new Date().getHours().toString().padStart(2, '0')}:${new Date().getMinutes().toString().padStart(2, '0')}`;

    // 生成两个不同的 ID，防止主键冲突
    const baseId = Date.now().toString();
    const myPhone = this.currentUser.phoneNumber;
    const targetPhone = this.targetPhone; // 确保这是对方手机号

    if (dbManager.chat) {
      // 1. 存给我自己
      await dbManager.chat.insertMessage({
        id: baseId + "_me",      // 唯一 ID
        avatar: this.targetAvatar,
        nickName: targetPhone,   // 列表按对方手机号分组
        content: content,
        time: timeStr,
        isSelf: true
      }, myPhone);

      // 2. 存给对方
      await dbManager.chat.insertMessage({
        id: baseId + "_ta",      // 唯一 ID
        avatar: this.currentUser.avatarPath || $r('app.media.user_default'),
        nickName: myPhone,       // 对方列表显示我的手机号
        content: content,
        time: timeStr,
        isSelf: false
      }, targetPhone);

      console.info(`消息已存入。发送者:${myPhone}, 接收者:${targetPhone}`);
    }

    // UI 更新...
    this.messageList.push({
      id: baseId + "_me",
      avatar: this.currentUser.avatarPath || $r('app.media.user_default'),
      nickName: '我',
      content: content,
      time: timeStr,
      isSelf: true
    });
    this.inputText = '';
    this.scroller.scrollEdge(Edge.Bottom);
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
      // 逻辑：如果 msg.isSelf 为 true，说明这条消息是当前登录账号发出的
      if (msg.isSelf) {
        Row() {
          Blank()
          Text(msg.content).padding(10).backgroundColor('#95EC69').borderRadius(6).margin({ right: 10 })
          // 显示“我”的头像（即当前登录账号的头像）
          Image(this.currentUser?.avatarPath || $r('app.media.user_default'))
            .width(42).height(42).borderRadius(4)
        }.width('100%').padding({ left: 50, right: 10 }).alignItems(VerticalAlign.Top)
      } else {
        // 逻辑：如果 msg.isSelf 为 false，说明是对方发来的
        Row() {
          // 显示“对方”的头像（即从通讯录传进来的 targetAvatar）
          Image(this.targetAvatar)
            .width(42).height(42).borderRadius(4)
            .alt($r('app.media.user_default'))
          Text(msg.content).padding(10).backgroundColor(Color.White).borderRadius(6).margin({ left: 10 })
          Blank()
        }.width('100%').padding({ left: 10, right: 50 }).alignItems(VerticalAlign.Top)
      }
    }.width('100%').margin({ top: 12 })
  }
}