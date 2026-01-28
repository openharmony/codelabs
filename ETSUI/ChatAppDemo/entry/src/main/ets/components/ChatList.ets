// entry/src/main/ets/components/ChatList.ets
import Message from '../viewmodel/Message';
import { NavTitle } from './NavTitle';
import { dbManager } from '../utils/DatabaseManager';
import router from '@ohos.router';
import { UserModel } from '../viewmodel/UserModel'; // 确保引入 UserModel

@Component
export struct ChatList {
  // 1. 获取当前登录用户信息，解决账号混淆的关键
  @StorageProp('refresh_list_flag') @Watch('onRefresh') flag: number = 0;
  @StorageProp('currentUser') currentUser: UserModel | null = null;
  @State messages: Message[] = [];

  onRefresh() {
    this.refreshData();
  }

  async refreshData() {
    if (dbManager.chat && this.currentUser) {
      this.messages = await dbManager.chat.queryChatList(this.currentUser.phoneNumber);
    }
  }

  async aboutToAppear() {
    this.refreshData();
  }

  build() {
    Column() {
      NavTitle({ title: '微信' })

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
                params: { contact: msg }
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