// entry/src/main/ets/components/ChatList.ets
import Message from '../viewmodel/Message';
import { NavTitle } from './NavTitle';
import { dbManager } from '../utils/DatabaseManager';
import router from '@ohos.router';

@Component
export struct ChatList {
  @State messages: Message[] = [];

  async aboutToAppear() {
    await this.refresh();
  }

  async refresh() {
    if (dbManager.chat) {
      // 完全从数据库读取，不再有任何硬编码图片
      this.messages = await dbManager.chat.queryChatList();
    }
  }

  build() {
    Column() {
      NavTitle({ title: '微信' })

      List() {
        ForEach(this.messages, (msg: Message) => {
          ListItem() {
            Row() {
              Image(msg.avatar).width(48).height(48).borderRadius(6)
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