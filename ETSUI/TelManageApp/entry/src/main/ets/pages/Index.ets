// 数据层：封装联系人查询/删除等能力
import { ContactData } from '../model/ContactData';
// 类型与系统接口：联系人模型
import { contact } from '@kit.ContactsKit';
// 页面路由：跳转添加/编辑页面
import { router } from '@kit.ArkUI';
// 权限与上下文：请求联系人权限、提供页面上下文
import { abilityAccessCtrl, Context } from '@kit.AbilityKit';

@Entry
@Component
struct Index {
  // 提供上下文给子组件与路由参数，数据层依赖此上下文调用系统接口
  @Provide('context') context: Context = getContext(this);
  // 数据层实例（使用页面上下文初始化）
  private contactData: ContactData = new ContactData(this.context);
  // 列表数据、搜索关键字与加载态
  @State contacts: contact.Contact[] = [];
  @State searchText: string = '';
  @State isLoading: boolean = true;

  aboutToAppear() {
    // 生命周期：页面将显示时拉取联系人，适配初次进入
    console.log('Index: 页面即将显示，开始加载联系人');
    this.loadContacts();
  }

  onPageShow() {
    // 生命周期：页面显示（前后台切换/返回）时刷新列表，保证数据一致
    console.log('Index: 页面显示，刷新联系人列表');
    this.loadContacts();
  }

  async loadContacts() {
    // 拉取全量联系人并更新加载态与列表数据
    console.log('Index: 开始加载联系人列表');
    this.isLoading = true;
    try {
      this.contacts = await this.contactData.queryAllContacts();
      console.log('Index: 联系人列表加载完成，数量:', this.contacts.length);

      // 检查联系人数据
      if (this.contacts.length > 0) {
        console.log('Index: 第一个联系人信息:', JSON.stringify(this.contacts[0]));
      }
    } catch (error) {
      console.error('Index: 加载联系人失败:', error);
    } finally {
      this.isLoading = false;
      console.log('Index: 加载状态设置为false');
    }
  }

  async searchContacts() {
    // 搜索流程：空关键字回退全量；申请权限后号码优先；无结果再姓名包含过滤
    console.log('Index: 开始搜索联系人，关键词:', this.searchText);
    if (this.searchText.trim() === '') {
      console.log('Index: 搜索关键词为空，加载所有联系人');
      this.loadContacts();
      return;
    }

    this.isLoading = true;
    try {
      // 先尝试按电话号码搜索
      let permissionRequestResult = await abilityAccessCtrl.createAtManager().requestPermissionsFromUser(this.context,
        [
          'ohos.permission.READ_CONTACTS',
          'ohos.permission.WRITE_CONTACTS'
        ]);
      // 如果权限列表中有-1，说明用户拒绝了授权
      if (permissionRequestResult.authResults[0] === 0) {
        // A: 先按电话号码查询
        let results = await this.contactData.queryContactsByPhone(this.searchText);
        console.log('Index: 按电话号码搜索到结果数量:', results.length);

        // 如果没有结果，按姓名过滤
        if (results.length === 0) {
          // B: 回退为姓名包含过滤（大小写不敏感）
          console.log('Index: 开始按姓名过滤');
          const allContacts = await this.contactData.queryAllContacts();
          results = allContacts.filter(contact =>
          contact.name?.fullName?.toLowerCase().includes(this.searchText.toLowerCase())
          );
          console.log('Index: 按姓名过滤后结果数量:', results.length);
        }

        this.contacts = results;
      }
    } catch (error) {
      console.error('Index: 搜索联系人失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  async deleteContact(key: string) {
    // 删除联系人并在成功后重新拉取列表
    console.log('Index: 开始删除联系人，key:', key);
    try {
      await this.contactData.deleteContact(key);
      console.log('Index: 删除联系人成功，重新加载列表');
      this.loadContacts(); // 重新加载列表
    } catch (error) {
      console.error('Index: 删除联系人失败:', error);
    }
  }

  // 添加二次确认弹窗方法
  showDeleteConfirmDialog(contactItem: contact.Contact) {
    // 二次确认对话框：避免误删，含遮障层点击关闭与取消/删除两按钮
    if (!contactItem.key) return;

    const contactName = contactItem.name?.fullName || '未知联系人';

    AlertDialog.show(
      {
        title: '删除联系人',
        message: `确定要删除联系人"${contactName}"吗？此操作不可撤销。`,
        autoCancel: true, // 点击遮障层时关闭弹窗
        alignment: DialogAlignment.Center, // 弹窗居中显示
        primaryButton: {
          value: '取消',
          action: () => {
            console.log('用户取消删除操作');
          }
        },
        secondaryButton: {
          value: '删除',
          fontColor: '#ff3b30', // 红色字体强调危险操作
          action: () => {
            console.log('用户确认删除联系人:', contactName);
            this.deleteContact(contactItem.key!);
          }
        },
        cancel: () => {
          console.log('用户通过遮障层取消删除');
        }
      }
    );
  }

  // 我的名片按钮点击事件
  onMyBusinessCardClick() {
    console.log('Index: 点击我的名片按钮，跳转到我的名片页面');

    // 跳转到我的名片页面
    router.pushUrl({
      url: 'pages/MyBusinessCard'
    });
  }

  build() {
    /**  页面结构：
    1) 顶部标题栏（联系人管理 + 添加）
    2) 搜索输入框（姓名/电话号码）
    3) 我的名片功能按钮
    4) 列表区：加载指示/空态/联系人列表
     **/
    Column() {
      // 标题栏
      Row() {
        Text('联系人')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        // 收藏按钮
        Button('收藏')
          .fontSize(14)
          .margin({ right: 10 })
          .onClick(() => {
            console.log('Index: 点击收藏按钮');
            router.pushUrl({
              url: 'pages/FavoritesDisplay'
            });
          })

        // 添加导入导出按钮
        Button('导出')
          .fontSize(14)
          .margin({ right: 10 })
          .onClick(() => {
            console.log('Index: 点击导入导出按钮');
            router.pushUrl({
              url: 'pages/ImportExport'
            });
          })

        Button('添加')
          .onClick(() => {
            console.log('Index: 点击添加按钮，跳转到添加页面');
            router.pushUrl({
              url: 'pages/AddContact'
            });
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      // 搜索框
      TextInput({ placeholder: '搜索联系人姓名' })
        .width('90%')
        .height(40)
        .margin(10)
        .onChange((value: string) => {
          this.searchText = value;
        })
        .onSubmit(() => {
          this.searchContacts();
        })

      // 我的名片功能按钮区域
      Row() {
        Button('我的名片')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .width('90%')
          .height(45)
          .backgroundColor('#007DFF')
          .fontColor(Color.White)
          .borderRadius(8)
          .onClick(() => {
            this.onMyBusinessCardClick();
          })
      }
      .width('100%')
      .justifyContent(FlexAlign.Center)
      .margin({ bottom: 15 })

      // // 我的名片功能按钮区域
      // Row() {
      //   Button('我的收藏')
      //     .fontSize(16)
      //     .fontWeight(FontWeight.Medium)
      //     .width('90%')
      //     .height(45)
      //     .backgroundColor('#007DFF')
      //     .fontColor(Color.White)
      //     .borderRadius(8)
      //     .onClick(() => {
      //       this.onMyBusinessCardClick();
      //     })
      // }
      // .width('100%')
      // .justifyContent(FlexAlign.Center)
      // .margin({ bottom: 15 })

      // 联系人列表
      if (this.isLoading) {
        LoadingProgress()
          .width(50)
          .height(50)
          .margin({ top: 100 })
      } else if (this.contacts.length === 0) {
        Column() {
          Text(this.searchText ? '未找到匹配的联系人' : '暂无联系人')
            .fontSize(18)
            .fontColor('#666')
        }
        .width('100%')
        .height('70%')
        .justifyContent(FlexAlign.Center)
      } else {
        List({ space: 10 }) {
          ForEach(this.contacts, (item: contact.Contact, index: number) => {
            ListItem() {
              this.contactItem(item)
            }
          }, (item: contact.Contact) => item.key || item.id?.toString() || '')
        }
        .width('100%')
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .onAppear(() => {
      // 页面可见时的日志记录
      console.log('Index: 界面已显示');
    })
  }

  @Builder
  contactItem(contactItem: contact.Contact) {
    // 列表项结构：头像区 + 信息区（姓名/电话/邮箱）+ 操作区（编辑/删除）
    Row() {
      Column() {
        // 头像
        if (contactItem.portrait?.uri) {
          Image(contactItem.portrait.uri)
            .width(50)
            .height(50)
            .borderRadius(25)
        } else {
          // 默认头像
          Text(contactItem.name?.fullName?.charAt(0) || '?')
            .fontSize(20)
            .fontColor(Color.White)
            .textAlign(TextAlign.Center)
            .width(50)
            .height(50)
            .borderRadius(25)
            .backgroundColor('#007DFF')
        }
      }
      .margin({ right: 15 })

      Column() {
        // 姓名
        Text(contactItem.name?.fullName || '未知')
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Start)
          .width('100%')

        // 电话号码
        if (contactItem.phoneNumbers && contactItem.phoneNumbers.length > 0) {
          Text(contactItem.phoneNumbers[0].phoneNumber)
            .fontSize(14)
            .fontColor('#666')
            .textAlign(TextAlign.Start)
            .width('100%')
            .margin({ top: 5 })
        }

        // 邮箱
        if (contactItem.emails && contactItem.emails.length > 0) {
          Text(contactItem.emails[0].email)
            .fontSize(12)
            .fontColor('#999')
            .textAlign(TextAlign.Start)
            .width('100%')
            .margin({ top: 2 })
        }
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)

      // 操作按钮
      Row() {
        Button('编辑')
          .fontSize(12)
          .width(60)
          .height(30)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/EditContact',
              params: {
                contactKey: contactItem.key || '',
                parentContext: this.context
              }
            });
          })

        Button('删除')
          .fontSize(12)
          .width(60)
          .height(30)
          .margin({ left: 10 })
          .backgroundColor('#ff3b30')
          .fontColor(Color.White)
          .onClick(() => {
            this.showDeleteConfirmDialog(contactItem);
          })
      }
    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(10)
    .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
  }
}