import Message from '../viewmodel/Message'
import { NavTitle } from './NavTitle'

@Component
export struct ChatList {
  // 显式声明类型，防止 ArkTS 无法推断类型
  @State messages: Message[] = []

  aboutToAppear() {
    // 补全所有符合 Message 接口定义的字段
    this.messages = [
      {
        id: '1',
        avatar: $r('app.media.user1'),
        nickName: '桑良翰',
        content: '你在干啥', // 对应新接口的 content
        time: '晚上6:23',
        isSelf: false
      },
      {
        id: '2',
        avatar: $r('app.media.user2'),
        nickName: '张廖初蝶',
        content: '好的,我一会儿就去!',
        time: '晚上6:28',
        isSelf: false
      },
      {
        id: '3',
        avatar: $r('app.media.user3'),
        nickName: '宇文雅彤',
        content: '集合,打球了,快点,就等你了',
        time: '晚上5:28',
        isSelf: false
      },
      {
        id: '4',
        avatar: $r('app.media.user4'),
        nickName: '那拉尔柳',
        content: '昨天吃的面',
        time: '晚上5:18',
        isSelf: false
      },
      {
        id: '5',
        avatar: $r('app.media.user5'),
        nickName: '楼暖姝',
        content: '这个剧很好看哦~',
        time: '晚上4:18',
        isSelf: false
      },
      {
        id: '6',
        avatar: $r('app.media.user6'),
        nickName: '谭晴丽',
        content: '上次喝的那个酒叫什么名字?',
        time: '晚上4:18',
        isSelf: false
      },
      {
        id: '7',
        avatar: $r('app.media.user7'),
        nickName: '詹凝梦',
        content: '昨天去大舅家了',
        time: '晚上3:18',
        isSelf: false
      },
      {
        id: '8',
        avatar: $r('app.media.user8'),
        nickName: '傅欣笑',
        content: '有纯血鸿蒙的测试资格么',
        time: '晚上3:08',
        isSelf: false
      },
      {
        id: '9',
        avatar: $r('app.media.user9'),
        nickName: '幸宛畅',
        content: '斗破苍穹又又又更新了,快点看',
        time: '昨天23:08',
        isSelf: false
      },
      {
        id: '10',
        avatar: $r('app.media.user10'),
        nickName: '轩辕雨琴',
        content: '身骑白马超好听',
        time: '昨天23:02',
        isSelf: false
      },
      {
        id: '11',
        avatar: $r('app.media.user11'),
        nickName: '郑怡嘉',
        content: '我很喜欢古风的歌曲!',
        time: '昨天22:02',
        isSelf: false
      },
      {
        id: '12',
        avatar: $r('app.media.user12'),
        nickName: '北芳馨',
        content: '南边新开了个小酒馆!',
        time: '昨天21:22',
        isSelf: false
      }
    ]
  }

  build() {
    Column() {
      NavTitle({ title: '微信' })

      Column() {
        // 这里的 space 是 ListItem 之间的间距
        List() {
          ForEach(this.messages, (message: Message) => {
            ListItem() {
              Row() {
                Image(message.avatar)
                  .width(48) // 稍微加大了一点，更接近微信比例
                  .height(48)
                  .borderRadius(6)

                Column() {
                  Text(message.nickName)
                    .fontWeight(700)
                    .fontSize(16)
                    .fontColor('#181818')

                  Text(message.content) // 使用 content 字段
                    .fontSize(14)
                    .fontColor('#999999')
                    .maxLines(1)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })

                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start)
                .justifyContent(FlexAlign.Center)
                .margin({ left: 12 })

                Column() {
                  Text(message.time)
                    .fontSize(12)
                    .fontColor('#CCCCCC')
                }
                .justifyContent(FlexAlign.Start)
                .alignItems(HorizontalAlign.End)
                .height(40)

              }
              .width('100%')
              .padding({ left: 16, right: 16, top: 12, bottom: 12 })
            }
          }, (item: Message) => item.id) // 使用 id 作为 Key
        }
        .width('100%')
        .divider({
          strokeWidth: 0.5,
          color: '#EEEEEE',
          startMargin: 76 // 这里的边距让分割线不穿过头像
        })
      }
      .backgroundColor(Color.White)
      .width('100%')
      .layoutWeight(1)

    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F7F7F7')
  }
}