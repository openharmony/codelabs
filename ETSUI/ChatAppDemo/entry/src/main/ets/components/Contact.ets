import { router, promptAction } from '@kit.ArkUI';
import { ContactGroup } from './ContactGroup'
import { NavTitle } from './NavTitle'
import { ContactModel } from '../viewmodel/ContactModel'
import { UserModel } from '../viewmodel/UserModel';
import { dbManager } from '../utils/DatabaseManager';
import { emitter } from '@kit.BasicServicesKit';
import { PINYIN_MAP } from '../utils/PinyinData';

@Component
export struct Contact {
  @StorageProp('currentUser') currentUser: UserModel | null = null;
  @State contactGroups: ContactGroup[] = [];
  private readonly EVENT_ID_UPDATE_CONTACT = 10001;
  private scroller: Scroller = new Scroller()

  async aboutToAppear() {
    await this.loadContacts();
    emitter.on({ eventId: this.EVENT_ID_UPDATE_CONTACT }, () => {
      console.info('收到更新通知，正在刷新通讯录...');
      this.loadContacts();
    });
  }

  aboutToDisappear() {
    emitter.off(this.EVENT_ID_UPDATE_CONTACT);
  }

  async loadContacts() {
    if (!this.currentUser) return;
    const result = await dbManager.friend?.getFriends(this.currentUser.phoneNumber);
    if (result) {
      this.processContacts(result);
    } else {
      this.contactGroups = [];
    }
  }

  processContacts(friends: UserModel[]) {
    const groups: Map<string, ContactModel[]> = new Map();

    friends.forEach(user => {
      let nick = user.nickName.trim();
      let firstChar = '#';

      if (nick.length > 0) {
        const char = nick.charAt(0).toUpperCase();
        if (/^[A-Z]$/.test(char)) {
          firstChar = char;
        } else {
          firstChar = this.getLetterByChinese(nick.charAt(0));
        }
      }

      if (!groups.has(firstChar)) groups.set(firstChar, []);
      groups.get(firstChar)?.push({
        avatar: user.avatarPath || $r('app.media.user_default'),
        name: user.nickName,
        phoneNumber: user.phoneNumber
      } as ContactModel);
    });

    const sortedGroups: ContactGroup[] = Array.from(groups.keys())
      .sort((a, b) => {
        if (a === '#') return 1;
        if (b === '#') return -1;
        return a.localeCompare(b);
      })
      .map((key: string): ContactGroup => {
        return {
          index: key,
          contacts: groups.get(key) || []
        } as ContactGroup;
      });

    this.contactGroups = sortedGroups;
  }

  getLetterByChinese(char: string): string {
    const keys = Object.keys(PINYIN_MAP);
    for (let i = 0; i < keys.length; i++) {
      const key = keys[i];
      if (PINYIN_MAP[key].includes(char)) {
        return key;
      }
    }
    return '#';
  }

  // 修改 image 类型为 ResourceStr 以匹配数据库路径和本地资源
  @Builder
  buildItem(image: ResourceStr, title: ResourceStr, onClick?: () => void) {
    Row() {
      Image(image)
        .width(40)
        .height(40)
        .borderRadius(6)
        .margin({ left: 10 })
        .alt($r('app.media.user_default'))
      Text(title)
        .fontWeight(500)
        .margin({ left: 10 })
      Blank()
      Image($r('sys.media.ohos_ic_public_arrow_right'))
        .width(20)
        .height(20)
        .margin({ right: 10 })
        .opacity(0.3)
    }
    .width('100%')
    .height(55)
    .backgroundColor(Color.White)
    .onClick(() => {
      if (onClick) onClick();
    })
  }

  @Builder
  buildListItemHeader(index: ResourceStr) {
    Row() {
      Text(index)
        .fontWeight(700)
        .margin({ left: 15 })
        .fontSize(14)
    }
    .width('100%')
    .height(30)
    .backgroundColor('#F1F3F5')
  }

  build() {
    Stack({ alignContent: Alignment.End }) {
      Column() {
        NavTitle({ title: $r("app.string.tab_contacts") })

        List({ scroller: this.scroller }) {
          // 1. 固定功能项
          ListItemGroup() {
            ListItem() {
              Column() {
                this.buildItem($r('app.media.new_friend'), $r('app.string.label_new_friends'), () => {
                  router.pushUrl({ url: 'pages/NewFriends' })
                })
                this.buildItem($r('app.media.group_chat'), $r('app.string.label_group_chat'), () => {
                  promptAction.showToast({ message: $r('app.string.toast_not_developed') });
                })
                this.buildItem($r('app.media.tag'), $r('app.string.label_tags'), () => {
                  promptAction.showToast({ message: $r('app.string.toast_not_developed') });
                })
                this.buildItem($r('app.media.official_account'), $r('app.string.label_official_accounts'), () => {
                  promptAction.showToast({ message: $r('app.string.toast_not_developed') });
                })
              }
            }
          }.divider({ strokeWidth: 1, startMargin: 60, color: '#F1F3F5' })

          // 2. 动态联系人列表
          ForEach(this.contactGroups, (contactGroup: ContactGroup) => {
            ListItemGroup({ header: this.buildListItemHeader(contactGroup.index) }) {
              ForEach(contactGroup.contacts, (contact: ContactModel) => {
                ListItem() {
                  // this.buildItem(contact.avatar, contact.name, () => {
                  //   // 跳转逻辑：传递 Message 结构参数
                  //   router.pushUrl({
                  //     url: 'pages/Chat',
                  //     params: {
                  //       contact: {
                  //         id: contact.phoneNumber,
                  //         avatar: contact.avatar,
                  //         nickName: contact.name,
                  //         content: '',
                  //         time: '',
                  //         isSelf: false
                  //       } as Message
                  //     }
                  //   })
                  // })
                  this.buildItem(contact.avatar, contact.name, async () => {
                    // 1. 先通过手机号查询完整的用户信息（包含地区等）
                    if (dbManager.user) {
                      const userDetail = await dbManager.user.queryUserByPhone(contact.phoneNumber);
                      if (userDetail) {
                        // 2. 跳转到好友详情页
                        router.pushUrl({
                          url: 'pages/FriendDetail',
                          params: { friend: userDetail }
                        });
                      }
                    }
                  })
                }
              }, (item: ContactModel) => item.phoneNumber)
            }
          }, (item: ContactGroup) => item.index.toString())
        }
        .width('100%')
        .layoutWeight(1)
        .sticky(StickyStyle.Header)
        .divider({ strokeWidth: 1, startMargin: 60, color: '#F1F3F5' })
      }
      .width('100%')
      .height('100%')

      // 3. 索引栏
      if (this.contactGroups.length > 0) {
        AlphabetIndexer({
          arrayValue: this.contactGroups.map(g => g.index.toString()),
          selected: 0
        })
          .color(Color.Black)
          .selectedColor(0xFF007DFF)
          .margin({ right: 5 })
          .onSelect((index: number) => {
            // 索引跳转逻辑，+1 是因为第0个是固定菜单项
            this.scroller.scrollToIndex(index + 1)
          })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }
}