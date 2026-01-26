import { ContactGroup } from './ContactGroup'
import { NavTitle } from './NavTitle'
import { ContactModel } from '../viewmodel/ContactModel'

@Component
export struct Contact {
  private contactGroups: ContactGroup[] = [
    {
      index: 'A',
      contacts: [
        {
          avatar: $r('app.media.user1'),
          name: '大阿联'
        }] },
    {
      index: 'B',
      contacts: [{
        avatar: $r('app.media.user2'),
        name: '博文视点'
      }]
    },
    {
      index: 'C',
      contacts:
      [
        {
          avatar: $r('app.media.user3'),
          name: '公孙离'
        }
      ]
    },
    {
      index: 'D',
      contacts: [
        {
          avatar: $r('app.media.user4'),
          name: '大哥'
        }
      ]
    },
    {
      index: 'F',
      contacts: [
        {
          avatar: $r('app.media.user5'),
          name: '房祖名'
        }
      ]
    },
    {
      index: 'H',
      contacts: [
        {
          avatar: $r('app.media.user6'),
          name: '黄龙玉'
        }
      ]
    },
    {
      index: 'L',
      contacts: [
        {
          avatar: $r('app.media.user7'),
          name: '梁晓静'
        },
        {
          avatar: $r('app.media.user8'),
          name: '李欣'
        }
      ]
    },
    {
      index: 'N',
      contacts: [
        {
          avatar: $r('app.media.user9'),
          name: '妞妞'
        }
      ]
    },
    {
      index: 'X',
      contacts: [
        {
          avatar: $r('app.media.user10'),
          name: '夏梦'
        }
      ]
    },
    {
      index: 'Y',
      contacts: [
        {
          avatar: $r('app.media.user11'),
          name: '杨紫'
        },
        {
          avatar: $r('app.media.user12'),
          name: '叶晨曦'
        }
      ]
    },
  ]
  private letters: string[] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
  private scroller: Scroller = new Scroller()

  @Builder
  buildItem(image: Resource, title: string) {
    Row() {
      Image(image)
        .width(40)
        .height(40)
        .borderRadius(6)
        .margin({
          left: 10
        })
      Text(title)
        .fontWeight(500)
        .margin({
          left: 10
        })
    }.width('96%')
    .height(50)
  }

  @Builder
  buildListItemHeader(index: string) {
    Row() {
      Text(index)
        .fontWeight(700)
        .margin({ left: 10 })
    }
    .width('104%')
    .height(40)
    .alignItems(VerticalAlign.Center)
    .backgroundColor(Color.Grey)
    .opacity(0.5)
  }

  build() {
    Row() {
      Stack({ alignContent: Alignment.End }) {
        Column() {
          NavTitle({ title: '通讯录' })
          List({ space: 10 }) {
            ListItem() {
              this.buildItem($r('app.media.new_friend'), '新的朋友')
            }

            ListItem() {
              this.buildItem($r('app.media.group_chat'), '群聊')
            }

            ListItem() {
              this.buildItem($r('app.media.tag'), '标签')
            }

            ListItem() {
              this.buildItem($r('app.media.official_account'), '公众号')
            }
          }.width('100%')
          .divider({
            strokeWidth: 2,
            startMargin: 50
          })

          List({ space: 10, scroller: this.scroller }) {
            ForEach(this.contactGroups, (contactGroup: ContactGroup) => {
              ListItemGroup({ header: this.buildListItemHeader(contactGroup.index) }) {
                ForEach(contactGroup.contacts, (contact: ContactModel) => {
                  this.buildItem(contact.avatar, contact.name)
                })
              }
            })
          }.width('100%')
          .height(400)
          .divider({
            strokeWidth: 2,
            startMargin: 50
          })
          .sticky(StickyStyle.Header | StickyStyle.Footer)
        }
        .width('100%')

        AlphabetIndexer({ arrayValue: this.letters, selected: 0 })
          .color(Color.Black)
          .alignStyle(IndexerAlign.Right)
          .align(Alignment.End)

          .onSelect((index: number) => {
            const letter = this.letters[index]
            for (let i = 0; i < this.contactGroups.length; i++) {
              const element = this.contactGroups[i];
              if (element.index === letter) {
                this.scroller.scrollToIndex(i)
              }
            }

          })

      }.width('100%')
      .height('100%')
    }
    .height('100%')
  }
}