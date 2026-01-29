// entry/src/main/ets/components/ChatList.ets
import Message from '../viewmodel/Message';
import { NavTitle } from './NavTitle';
import { dbManager } from '../utils/DatabaseManager';
import router from '@ohos.router';
import { UserModel } from '../viewmodel/UserModel'; // 确保引入 UserModel

@Component
export struct ChatList {
  // 1. 获取当前登录用户信息，解决账号混淆的关键
  @StorageProp('currentUser') currentUser: UserModel | null = null;
  @StorageProp('refresh_flag') @Watch('refresh') flag: number = 0;
  @State messages: Message[] = [];

  async aboutToAppear() {
    await this.refresh();
  }

  async refresh() {
    if (dbManager.chat && this.currentUser) {
      // 核心：只查 owner 为当前登录人的列表
      this.messages = await dbManager.chat.queryChatList(this.currentUser.phoneNumber);
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
              router.pushUrl({
                url: 'pages/Chat',
                params: {
                  contact: {
                    id: msg.nickName, // 此处的 nickName 在库中存的是手机号
                    nickName: msg.nickName,
                    avatar: msg.avatar,
                    // ...其他字段
                  }
                }
              })
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