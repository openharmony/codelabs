import { router,promptAction } from '@kit.ArkUI'; // 引入路由模块
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
  // 将 contactGroups 改为状态变量，初始为空
  @State contactGroups: ContactGroup[] = [];
  // 定义一个唯一的事件 ID
  private readonly EVENT_ID_UPDATE_CONTACT = 10001;

  private letters: string[] = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']
  private scroller: Scroller = new Scroller()

  // 页面显示时加载数据
  async aboutToAppear() {
    await this.loadContacts();

    // 订阅事件：一旦有地方发送这个 ID，就执行回调
    emitter.on({ eventId: this.EVENT_ID_UPDATE_CONTACT }, () => {
      console.info('收到更新通知，正在刷新通讯录...');
      this.loadContacts();
    });
  }

  aboutToDisappear() {
    // 组件销毁时取消订阅，防止内存泄漏
    emitter.off(this.EVENT_ID_UPDATE_CONTACT);
  }

  async loadContacts() {
    if (!this.currentUser) return;

    // 从数据库获取好友详情（之前在 FriendHandler 定义的方法）
    // 先获取原始返回结果（此时类型可能是 UserModel[] | undefined）
    const result = await dbManager.friend?.getFriends(this.currentUser.phoneNumber);

    // 显式进行空判断，并收紧类型
    if (result) {
      // 此时在 if 分支内，result 被自动识别为 UserModel[]
      this.processContacts(result);
    } else {
      // 处理无好友或加载失败的情况
      this.contactGroups = [];
    }

  }

  /**
   * 核心逻辑：将查出来的 UserModel 数组 转换成 ContactGroup[] 结构
   */
  processContacts(friends: UserModel[]) {
    const groups: Map<string, ContactModel[]> = new Map();

    friends.forEach(user => {
      let nick = user.nickName.trim();
      let firstChar = '#';

      if (nick.length > 0) {
        const char = nick.charAt(0).toUpperCase();
        // 如果是 A-Z 直接用
        if (/^[A-Z]$/.test(char)) {
          firstChar = char;
        } else {
          // 如果是中文或其他，查字典表
          firstChar = this.getLetterByChinese(nick.charAt(0));
        }
      }

      if (!groups.has(firstChar)) groups.set(firstChar, []);
      groups.get(firstChar)?.push({
        avatar: user.avatarPath,
        name: user.nickName,
        phoneNumber: user.phoneNumber
      } as ContactModel);
    });

    // 将 Map 转回数组并按字母表 A-Z 排序
    const sortedGroups: ContactGroup[] = Array.from(groups.keys())
      .sort((a, b) => {
        if (a === '#') return 1;
        if (b === '#') return -1;
        return a.localeCompare(b);
      })
      .map((key: string): ContactGroup => {
        // 关键修正：这里必须是一个符合 ContactGroup 定义的对象
        // 假设你的 ContactGroup 定义是 { index: string, contacts: ContactModel[] }
        const group: ContactGroup = {
          index: key,
          contacts: groups.get(key) || []
        };
        return group;
      });

    this.contactGroups = sortedGroups;
  }

  getLetterByChinese(char: string): string {
    // 遍历映射表
    const keys = Object.keys(PINYIN_MAP);
    for (let i = 0; i < keys.length; i++) {
      const key = keys[i];
      if (PINYIN_MAP[key].includes(char)) {
        return key;
      }
    }
    return '#'; // 没查到归类到 #
  }

  @Builder
  buildItem(image: Resource | string, title: string, onClick?: () => void) {
    Row() {
      Image(image)
        .width(40)
        .height(40)
        .borderRadius(6)
        .margin({ left: 10 })
        .alt($r('app.media.user_default')) // 加个兜底图，防止路径失效导致空白
      Text(title)
        .fontWeight(500)
        .margin({ left: 10 })
      Blank()
      Image($r('sys.media.ohos_ic_public_arrow_right')) // 使用系统右箭头
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
              // 修改点：添加跳转到 NewFriends 页面
              this.buildItem($r('app.media.new_friend'), '新的朋友', () => {
                router.pushUrl({ url: 'pages/NewFriends' })
              })
            }

            ListItem() {
              this.buildItem($r('app.media.group_chat'), '群聊', () => {
                promptAction.showToast({ message: '此功能尚未开发' });
              })
            }

            ListItem() {
              this.buildItem($r('app.media.tag'), '标签', () => {
                promptAction.showToast({ message: '此功能尚未开发' });
              })
            }

            ListItem() {
              this.buildItem($r('app.media.official_account'), '公众号', () => {
                promptAction.showToast({ message: '此功能尚未开发' });
              })
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

        AlphabetIndexer({
          arrayValue: this.contactGroups.map(g => g.index), // 动态生成存在的索引
          selected: 0
        })
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