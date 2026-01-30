// entry/src/main/ets/pages/Chat.ets
import { router } from '@kit.ArkUI';
import { emitter } from '@kit.BasicServicesKit';
import { UserModel } from '../viewmodel/UserModel'; // 确保引入了 UserModel
import Message from '../viewmodel/Message';
import { dbManager } from '../utils/DatabaseManager';
import { promptAction } from '@kit.ArkUI';

@Entry
@Component
struct Chat {
  @StorageProp('currentUser') currentUser: UserModel | null = null;
  @State chatTarget: string = '';
  @State targetPhone: string = '';
  @State targetAvatar: ResourceStr = $r('app.media.user_default');
  @State messageList: Message[] = [];
  @State inputText: string = '';
  private scroller: Scroller = new Scroller();
  @State isFriend: boolean = true; // 新增：记录好友状态

  // 显式声明返回类型为 Promise<void>
  // entry/src/main/ets/pages/Chat.ets

  async aboutToAppear(): Promise<void> {
    const params = router.getParams() as Record<string, Object>;
    if (params && params.contact && this.currentUser) {
      const contact = params.contact as Message;

      // 1. 接收从 ChatList 传过来的 id (即手机号)
      this.targetPhone = contact.id;
      this.chatTarget = contact.nickName;
      this.targetAvatar = contact.avatar;

      // 【新增】进入页面时检查好友关系
      if (dbManager.friend) {
        this.isFriend = await dbManager.friend.isFriend(this.currentUser.phoneNumber, this.targetPhone);
      }

      console.info(`正在查询历史：我的手机号[${this.currentUser.phoneNumber}], 对方手机号[${this.targetPhone}]`);

      if (this.isFriend &&dbManager.chat) {
        // 2. 调用查询函数
        const history: Message[] = await dbManager.chat.queryHistoryByContact(
          this.targetPhone,
          this.currentUser.phoneNumber
        );

        this.messageList = history;
        console.info(`查询到历史消息数量: ${this.messageList.length}`);

        // 3. 滚动到底部
        setTimeout((): void => {
          this.scroller.scrollEdge(Edge.Bottom);
        }, 100);
      }
    }
  }

  // 核心修改：增加 : Promise<void>
  async sendMessage(): Promise<void> {
    if (this.inputText.trim().length === 0 || !this.currentUser || !dbManager.chat) return;
    if (!this.isFriend) {
      promptAction.showToast({ message: $r('app.string.toast_not_friend_fail') });
      return;
    }
    const myPhone: ResourceStr = this.currentUser.phoneNumber;
    const myName: ResourceStr = this.currentUser.nickName;
    const baseId: ResourceStr = Date.now().toString();
    const timeStr: ResourceStr = `${new Date().getHours()}:${new Date().getMinutes().toString().padStart(2, '0')}`;

    // 数据库插入逻辑...
    await dbManager.chat.insertMessage({
      id: baseId + "_me",
      avatar: this.targetAvatar,
      nickName: this.chatTarget,
      content: this.inputText,
      time: timeStr,
      isSelf: true
    }, myPhone, this.targetPhone);

    await dbManager.chat.insertMessage({
      id: baseId + "_ta",
      avatar: this.currentUser.avatarPath || $r('app.media.user_default'),
      nickName: myName,
      content: this.inputText,
      time: timeStr,
      isSelf: false
    }, this.targetPhone, myPhone);

    // 发送 emitter 刷新通知
    emitter.emit({ eventId: 10002 });

    this.messageList.push({
      id: baseId + "_me",
      avatar: this.currentUser.avatarPath || $r('app.media.user_default'),
      nickName: '我',
      content: this.inputText,
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
        if (this.isFriend) {
          // 是好友：正常显示输入框
          TextInput({ text: this.inputText, placeholder: $r('app.string.hint_enter_msg') })
            .layoutWeight(1).height(40).backgroundColor(Color.White)
            .onChange((v: string) => this.inputText = v)
          Button('发送').margin({ left: 10 })
            .onClick((): void => { this.sendMessage() })
        } else {
          // 不是好友：显示提示文本或禁用状态
          Row() {
            Text($r('app.string.toast_cant_send_msg'))
              .fontSize(14).fontColor(Color.Gray)
          }
          .justifyContent(FlexAlign.Center)
          .width('100%')
          .height(40)
        }
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