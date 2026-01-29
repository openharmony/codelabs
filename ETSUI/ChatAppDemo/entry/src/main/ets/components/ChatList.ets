// entry/src/main/ets/components/ChatList.ets
import Message from '../viewmodel/Message';
import { NavTitle } from './NavTitle';
import { dbManager } from '../utils/DatabaseManager';
import router from '@ohos.router';
import { emitter } from '@kit.BasicServicesKit';
import { UserModel } from '../viewmodel/UserModel'; // 确保引入 UserModel

@Component
export struct ChatList {
  @StorageProp('currentUser') currentUser: UserModel | null = null;
  @State messages: Message[] = [];

  // 显式声明返回类型为 Promise<void>
  async aboutToAppear(): Promise<void> {
    await this.refreshData();

    // 订阅 10002 刷新事件
    // 回调函数也要补齐类型，或者声明为 async (): Promise<void>
    emitter.on({ eventId: 10002 }, async (): Promise<void> => {
      console.info('ChatList 监听到 emitter 事件');
      await this.refreshData();
    });
  }

  // 普通函数声明返回类型为 void
  aboutToDisappear(): void {
    emitter.off(10002);
  }

  // 显式声明返回类型为 Promise<void>
  // entry/src/main/ets/components/ChatList.ets

  async refreshData(): Promise<void> {
    if (dbManager.chat && this.currentUser) {
      console.info('ChatList 开始刷新数据...');
      const newList = await dbManager.chat.queryChatList(this.currentUser.phoneNumber);

      // 强制触发 UI 更新：先赋值为空，再赋值新数组（可选，如果数据量大不建议）
      // 或者直接赋值，但确保 ForEach 的 Key (item.id) 对应的是最新消息的 ID
      this.messages = newList;

      console.info(`刷新完成，最新一条内容为: ${this.messages[0]?.content}`);
    }
  }

  build() {
    Column() {
      NavTitle({ title: '聊天' })

      List() {
        ForEach(this.messages, (msg: Message) => {
          ListItem() {
            Row() {
              // 这里的 avatar 已经是数据库里存的 ResourceStr
              Image(msg.avatar).width(48).height(48).borderRadius(6)
                .alt($r('app.media.user_default')) // 增加兜底
              Column() {
                Row() {
                  Text(msg.nickName).fontSize(16).fontWeight(700).fontColor('#181818')
                  Blank()
                  Text(msg.time).fontSize(12).fontColor('#CCC')
                }.width('100%')
                Text(msg.content).fontSize(14).fontColor('#999').margin({ top: 4 }).maxLines(1)
              }.layoutWeight(1).margin({ left: 12 }).alignItems(HorizontalAlign.Start)
            }
            .width('100%').padding(16).backgroundColor(Color.White)
            .onClick(() => {
              console.info('准备跳转，当前点击的消息对象:', JSON.stringify(msg));
              router.pushUrl({
                url: 'pages/Chat',
                params: {
                  contact: {
                    // 关键点：ChatList 里的 msg.id 必须是对方的手机号
                    id: msg.id,
                    nickName: msg.nickName,
                    avatar: msg.avatar,
                    // ... 其他字段
                  } as Message
                }
              });
            })
          }
        }, (item: Message) => item.id)
      }
      .divider({ strokeWidth: 0.5, color: '#EEEEEE', startMargin: 76 })
      .layoutWeight(1)
    }
    .width('100%').height('100%').backgroundColor('#F7F7F7')
  }
}