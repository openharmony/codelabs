# 联系人管理系统（ArkTS）
## 介绍
本TelManageApp中，我们会构建一个具有增、删、改、查功能的联系人管理系统。应用包含两级页面，分别是主页（联系人添加、查找、删除），以及联系人信息编辑页面。效果如下图所示：
- 图1：添加联系人。
- 图2：修改联系人信息。
- 图3：根据姓名字段查找联系人。
- 图4：删除联系人。


![](demo-gif/AddContact.gif)
![](demo-gif/EditContact.gif)
![](demo-gif/SearchContact.gif)
![](demo-gif/DeleteContact.gif)
# 搭建OpenHarmony环境
## 软件要求
- [DevEco Studio](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)版本：DevEco Studio 5.0.5。
- OpenHarmony SDK版本：API version 13。

## 硬件要求

- 开发板类型：[润和DAYU200开发板](https://gitcode.com/hihope/DAYU200/blob/main/README.md)。
- OpenHarmony系统：5.0.1 Release。

## 环境搭建
完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**DAYU200**开发板为例，参照以下步骤进行：

1.  [获取OpenHarmony系统版本](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）

    以5.0.1版本为例：

    ![](Figures/f1.png)

2.  搭建烧录环境

    1.  [完成DevEco Device Tool的安装](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
    2.  [完成DAYU200开发板的烧录](https://gitcode.com/hihope/DAYU200/blob/main/%E7%83%A7%E5%86%99%E5%B7%A5%E5%85%B7%E5%8F%8A%E6%8C%87%E5%8D%97/windows/%E7%83%A7%E5%BD%95%E6%8C%87%E5%AF%BC%E6%96%87%E6%A1%A3.md)
3.  搭建开发环境

    1.  开始前请参考[工具准备](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-fa.md#%E5%88%9B%E5%BB%BAets%E5%B7%A5%E7%A8%8B)创建工程（模板选择“Empty Ability”），选择JS或者eTS语言开发。
    3.  工程创建完成后，选择使用[真机进行调测](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-fa.md#%E4%BD%BF%E7%94%A8%E7%9C%9F%E6%9C%BA%E8%BF%90%E8%A1%8C%E5%BA%94%E7%94%A8)。


# 代码结构解读
本篇Codelab只对核心代码进行讲解，对于完整代码，我们会在gitcode中提供。
```typescript
├──entry/src/main/ets                     // 代码区
│  ├──entryability
│  │  └──EntryAbility.ets                // 程序入口类
│  ├──entrybackupability
│  │  └──EntryBackupAbility.ets          // 数据备份与恢复类
│  ├──model
│  │  ├──ContactData.ets                 // 联系人数据管理类
│  │  └──BusinessCardData.ts                 // 我的名片管理
│  │  └──FavoriteManager.ts                 // 我的收藏管理
│  ├──pages
│  │  ├──EditContact.ets                 // 编辑联系人页面
│  │  ├──AddContact.ets                 // 添加联系人
│  │  ├──FavoriteManagement.ets                 // 收藏管理
│  │  ├──FavoritesDisplay.ets                 // 收藏展示
│  │  ├──ImportExport.ets                 // 导入导出
│  │  ├──MyBusinessCard.ets                 // 我的名片
│  │  ├──SelectFavorites.ets                 // 选择收藏
│  │  └──Index.ets                      //主页面的UI设计
│  └──viewmodel    
│     ├──ContactItem.ets                 // 联系人信息类
│     └──ContactViewModel.ets            // 联系人页面数据处理类
│——entry/src/main/resource                // 应用静态资源目录
└──entry/src/main/module.json5            //配置文件
```

## 联系人管理系统主页面
  联系人管理系统的主页面主要由三部分组成：顶栏、搜索框和联系人列表。

1.  顶栏

    以下代码实现了顶栏布局,使用Row容器来水平排列“联系人管理”标题和“添加”按钮,设置容器内边距为20像素。其中，使用Text组件来显示“联系人管理”标题，使用Button组件来实现“添加”按钮。
    ```typescript
    Row() {
      Text('联系人管理')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Center)

      Blank()

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
    ```
2.  搜索框

    以下代码使用TextInput组件实现了搜索框布局，支持按姓名或电话号码搜索联系人。设置宽度占90%，高度为40像素，并使用onChange()方法监听用户输入的变化，将输入值实时绑定到状态变量searchText。
    ```typescript
    TextInput({ placeholder: '搜索联系人姓名或电话号码' })
      .width('90%')
      .height(40)
      .margin(10)
      .onChange((value: string) => {
        this.searchText = value;
      })
      .onSubmit(() => {
        this.searchContacts();
      })
    ```
3.  联系人列表
    位于页面中间，使用List容器来实现垂直排列的滚动列表，并动态遍历状态变量this.contacts，为每个联系人生成一个列表项。设置列表项之间的垂直间距为10像素，列表宽度占父容器的100%。
    ```typescript
    List({ space: 10 }) {
      ForEach(this.contacts, (item: contact.Contact, index: number) => {
        ListItem() {
          this.contactItem(item)
        }
      }, (item: contact.Contact) => item.key || item.id?.toString() || '')
    }
    .width('100%')
    .layoutWeight(1)
    ```

  在联系人列表中，每个列表项由头像区、信息区（姓名/电话/邮箱）和操作区（编辑/删除）三部分组成，整体使用水平 Row 布局。

  头像使用Column()作为容器，右侧留出 15 像素外边距，与中间信息区拉开间距。若无头像时，则展示姓名首字母作为默认占位。

  中间信息区使用Column()作为容器，姓名加粗并左对齐，作为主标题。电话号码作为次级信息，以5像素的外边距与姓名分隔。邮箱层次最弱，以2像素的外边距与电话分隔。

  右侧操作区使用Row()作为容器，水平放置“编辑”和“删除”两个按钮。其中删除按钮设为红底白字，强调危险动作。
  ```typescript
    @Builder
    contactItem(contactItem: contact.Contact) {
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
  ```
## 增加联系人
### 新增联系人界面设计
整体采用可滚动纵向布局来承载整个表单内容，即Scroll + 垂直Column布局。页面被分为三大部分：顶栏、头像区域与基本信息表单卡片。
1.  顶栏

    包括取消按钮、“添加联系人”标题和保存按钮。

    ```typescript
    build() {
        Column() {
          // 标题栏
          Row() {
            Button('取消')
              .fontSize(16)
              .backgroundColor(Color.Transparent)
              .fontColor('#007DFF')
              .onClick(() => {
                router.back();
              })

            Text('添加联系人')
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .textAlign(TextAlign.Center)
              .layoutWeight(1)

            Button('保存')
              .fontSize(16)
              .backgroundColor(Color.Transparent)
              .fontColor(this.name && this.phone ? '#007DFF' : '#999')
              .enabled(!!this.name && !!this.phone && !this.isSaving)
              .onClick(() => {
                this.saveContact();
              })
          }
          .width('100%')
          .padding(20)
          .backgroundColor('#f0f0f0')
        }
    }

    ```
2.  头像区域

    使用Column容器，设置内容中向居中，并使用头像占位字符，若已输入姓名，取首字母并转大写当作头像字符；否则显示“+”表示新增。
    ```typescript
    // 表单内容
          Scroll() {
            Column() {
              // 头像区域
              Column() {
                Text(this.name ? this.name.charAt(0).toUpperCase() : '+')
                  .fontSize(40)
                  .fontColor(Color.White)
                  .textAlign(TextAlign.Center)
                  .width(80)
                  .height(80)
                  .borderRadius(40)
                  .backgroundColor('#007DFF')
              }
              .width('100%')
              .padding(30)
              .justifyContent(FlexAlign.Center)
    ```

3.  基本信息

    外层容器为Column，设置外边距为左右各 20，视觉上让白卡片悬浮在灰色背景上，表单标题为“基本信息”，表单项分别为姓名（必填）、电话（必填）、邮箱（选填），TextInput 的 onChange 与状态 this.name/phone/email 绑定，表单项之间以Divider分隔。
    ```typescript
    // 基本信息表单
    Column() {
      Text('基本信息')
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .textAlign(TextAlign.Start)
        .width('100%')
        .margin({ bottom: 15 })

      // 姓名输入框
      Column() {
        Text('姓名')
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ bottom: 5 })

        TextInput({ placeholder: '请输入姓名（必填）' })
          .width('100%')
          .height(50)
          .fontSize(16)
          .backgroundColor('#f8f8f8')
          .borderRadius(5)
          .padding({ left: 10, right: 10 })
          .onChange((value: string) => {
            this.name = value.trim();
          })
      }
      .width('100%')
      .margin({ bottom: 15 })

      Divider()
        .width('100%')
        .strokeWidth(1)
        .color('#eee')

      // 电话输入框
      Column() {
        Text('电话号码')
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ bottom: 5, top: 15 })

        TextInput({ placeholder: '请输入电话号码（必填）' })
          .width('100%')
          .height(50)
          .fontSize(16)
          .backgroundColor('#f8f8f8')
          .borderRadius(5)
          .padding({ left: 10, right: 10 })
          .type(InputType.Number)
          .onChange((value: string) => {
            this.phone = value.trim();
          })
      }
      .width('100%')
      .margin({ bottom: 15 })

      Divider()
        .width('100%')
        .strokeWidth(1)
        .color('#eee')

      // 邮箱输入框
      Column() {
        Text('邮箱地址')
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ bottom: 5, top: 15 })

        TextInput({ placeholder: '请输入邮箱地址（可选）' })
          .width('100%')
          .height(50)
          .fontSize(16)
          .backgroundColor('#f8f8f8')
          .borderRadius(5)
          .padding({ left: 10, right: 10 })
          .onChange((value: string) => {
            this.email = value.trim();
          })
      }
      .width('100%')
    }
    ```

### 新增联系人逻辑
点击顶栏的“添加”按钮后，将进入“新增模式”。在新增联系人核心逻辑 saveContact组件中，先校验姓名和电话必填，未填则直接返回。接着将 isSaving 置为 true 显示提交中状态，随后根据表单值构造联系人对象并写入系统通讯录。联系人对象保存成功后返回上一页。
```typescript
async saveContact() {
    if (!this.name || !this.phone) {
      return;
    }

    this.isSaving = true;

    try {
      const newContact = this.contactData.createContact(this.name, this.phone, this.email);
      await this.contactData.addContact(newContact);
      console.error('添加联系人成功');
      router.back();
    } catch (error) {
      console.error('添加联系人失败:', error);
    } finally {
      this.isSaving = false;
    }
  }
```

## 修改联系人信息
### 修改联系人界面设计
整体采用与新增页面一致的纵向结构：顶部标题栏、可滚动内容区（头像占位 + 基本信息表单），并在表单下方提供“删除联系人”入口。
1. 顶栏

    包含“取消”、“编辑联系人”标题与“保存”按钮，保存按钮在姓名与电话非空且非保存中时可点击。

    ```typescript
    // EditContact.ets 标题栏
    Row() {
      Button('取消')
        .fontSize(16)
        .backgroundColor(Color.Transparent)
        .fontColor('#007DFF')
        .onClick(() => {
          router.back();
        })

      Text('编辑联系人')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Center)
        .layoutWeight(1)

      Button('保存')
        .fontSize(16)
        .backgroundColor(Color.Transparent)
        .fontColor(this.name && this.phone && !this.isSaving ? '#007DFF' : '#999')
        .enabled(!!this.name && !!this.phone && !this.isSaving)
        .onClick(() => {
          this.saveContact();
        })
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#f0f0f0')
    ```

2. 头像区域

    显示姓名首字母作为占位头像。

    ```typescript
    // 头像占位
    Column() {
      Text(this.name ? this.name.charAt(0).toUpperCase() : '?')
        .fontSize(40)
        .fontColor(Color.White)
        .textAlign(TextAlign.Center)
        .width(80)
        .height(80)
        .borderRadius(40)
        .backgroundColor('#007DFF')
    }
    .width('100%')
    .padding(30)
    .justifyContent(FlexAlign.Center)
    ```

3. 基本信息

    表单包含姓名、电话号码、邮箱三项；已填值通过 `text` 属性回显。

    ```typescript
    // 基本信息卡片（节选）
    Column() {
      Text('基本信息')
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .textAlign(TextAlign.Start)
        .width('100%')
        .margin({ bottom: 15 })

      // 姓名
      Column() {
        Text('姓名')
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ bottom: 5 })

        TextInput({ text: this.name, placeholder: '请输入姓名' })
          .width('100%')
          .height(50)
          .fontSize(16)
          .backgroundColor('#f8f8f8')
          .borderRadius(5)
          .onChange((value: string) => {
            this.name = value.trim();
          })
      }
      .width('100%')
      .margin({ bottom: 15 })

      // 电话
      Column() {
        Text('电话号码')
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ bottom: 5 })

        TextInput({ text: this.phone, placeholder: '请输入电话号码' })
          .width('100%')
          .height(50)
          .fontSize(16)
          .backgroundColor('#f8f8f8')
          .borderRadius(5)
          .type(InputType.Number)
          .onChange((value: string) => {
            this.phone = value.trim();
          })
      }
      .width('100%')
      .margin({ bottom: 15 })

      // 邮箱
      Column() {
        Text('邮箱地址')
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ bottom: 5 })

        TextInput({ text: this.email, placeholder: '请输入邮箱地址（可选）' })
          .width('100%')
          .height(50)
          .fontSize(16)
          .backgroundColor('#f8f8f8')
          .borderRadius(5)
          .onChange((value: string) => {
            this.email = value.trim();
          })
      }
      .width('100%')
    }
    .width('100%')
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(10)
    .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
    .margin({ bottom: 20 })
    ```

4. 删除入口

    编辑页底部提供“删除联系人”按钮，点击后显示二次确认遮罩弹层（删除逻辑详见“删除联系人”章节）。

    ```typescript
    Button('删除联系人')
      .width('100%')
      .height(50)
      .backgroundColor('#ff3b30')
      .fontColor(Color.White)
      .fontSize(16)
      .borderRadius(10)
      .onClick(() => {
        this.showDeleteConfirm = true;
      })
    ```

### 修改联系人逻辑
编辑页在生命周期中获取参数并加载详情，保存时构造更新对象并提交。

```typescript
// 1) 获取参数并加载联系人
aboutToAppear() {
  this.contactData.setContext(getContext(this));
  const params = router.getParams() as Record<string, string>;
  this.contactKey = params?.contactKey || '';
  if (this.contactKey) {
    this.loadContact();
  } else {
    this.isLoading = false;
  }
}

async loadContact() {
  this.isLoading = true;
  this.contactInfo = await this.contactData.queryContactById(this.contactKey);
  if (this.contactInfo) {
    this.name = this.contactInfo.name?.fullName || '';
    this.phone = this.contactInfo.phoneNumbers?.[0]?.phoneNumber || '';
    this.email = this.contactInfo.emails?.[0]?.email || '';
  }
  this.isLoading = false;
}

// 2) 保存更新
async saveContact() {
  if (!this.name || !this.phone || !this.contactInfo) {
    return;
  }

  this.isSaving = true;
  try {
    const updatedContact: contact.Contact = {
      id: this.contactInfo.id,
      key: this.contactInfo.key,
      name: { fullName: this.name },
      phoneNumbers: [{ phoneNumber: this.phone, labelId: contact.PhoneNumber.NUM_MOBILE }],
      emails: this.email ? [{ email: this.email, labelId: contact.Email.EMAIL_HOME }] : []
    };

    // 保留未在本页编辑但已存在的字段
    if (this.contactInfo.portrait) updatedContact.portrait = this.contactInfo.portrait;
    if (this.contactInfo.organization) updatedContact.organization = this.contactInfo.organization;
    if (this.contactInfo.note) updatedContact.note = this.contactInfo.note;
    if (this.contactInfo.relations) updatedContact.relations = this.contactInfo.relations;
    if (this.contactInfo.imAddresses) updatedContact.imAddresses = this.contactInfo.imAddresses;
    if (this.contactInfo.websites) updatedContact.websites = this.contactInfo.websites;
    if (this.contactInfo.events) updatedContact.events = this.contactInfo.events;
    if (this.contactInfo.postalAddresses) updatedContact.postalAddresses = this.contactInfo.postalAddresses;

    await this.contactData.updateContact(updatedContact);
    router.back();
  } catch (error) {
    console.error('更新联系人失败:');
  } finally {
    this.isSaving = false;
  }
}
```

## 查找联系人
### 查找联系人界面设计
搜索输入框位于主页顶栏下方，输入关键字（姓名）后提交触发搜索；列表区域根据状态显示加载指示、空态或联系人列表。

```typescript
// 搜索框（Index.ets）
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

// 列表/空态/加载态（节选）
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
```

### 查找联系人逻辑
搜索逻辑位于 `searchContacts()`：空关键字回退到全量列表；申请权限后全量拉取并按姓名包含过滤；最终更新列表数据。

```typescript
async searchContacts() {
  console.log('Index: 开始搜索联系人，关键词:', this.searchText);
  if (this.searchText.trim() === '') {
    console.log('Index: 搜索关键词为空，加载所有联系人');
    this.loadContacts();
    return;
  }

  this.isLoading = true;
  try {
    let permissionRequestResult = await abilityAccessCtrl.createAtManager().requestPermissionsFromUser(this.context,
      [
        'ohos.permission.READ_CONTACTS',
        'ohos.permission.WRITE_CONTACTS'
      ]);
    if (permissionRequestResult.authResults[0] === 0) {
      let results = await this.contactData.queryContactsByPhone(this.searchText);
      console.log('Index: 按电话号码搜索到结果数量:', results.length);

      if (results.length === 0) {
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
```

## 删除联系人
### 删除联系人界面设计
删除入口在主页列表项与编辑页均可触达，并均提供二次确认。

1. 主页删除入口与确认对话框

    ```typescript
    // 列表项中的删除按钮（Index.ets 节选）
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
    ```

    ```typescript
    // 主页的 AlertDialog 确认（Index.ets）
    showDeleteConfirmDialog(contactItem: contact.Contact) {
      if (!contactItem.key) return;

      const contactName = contactItem.name?.fullName || '未知联系人';

      AlertDialog.show(
        {
          title: '删除联系人',
          message: `确定要删除联系人"${contactName}"吗？此操作不可撤销。`,
          autoCancel: true,
          alignment: DialogAlignment.Center,
          primaryButton: { value: '取消', action: () => {} },
          secondaryButton: {
            value: '删除',
            fontColor: '#ff3b30',
            action: () => {
              this.deleteContact(contactItem.key!);
            }
          }
        }
      );
    }
    ```

2. 编辑页删除入口与确认遮罩

    ```typescript
    // 删除入口按钮（EditContact.ets）
    Button('删除联系人')
      .width('100%')
      .height(50)
      .backgroundColor('#ff3b30')
      .fontColor(Color.White)
      .fontSize(16)
      .borderRadius(10)
      .onClick(() => {
        this.showDeleteConfirm = true;
      })
    ```

    ```typescript
    // 遮罩确认对话框（EditContact.ets 节选）
    if (this.showDeleteConfirm) {
      Column() {
        Blank()
          .width('100%')
          .height('100%')
          .backgroundColor('#000000')
          .opacity(0.5)
          .onClick(() => {
            this.showDeleteConfirm = false;
          })

        Column() {
          Text('删除联系人')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 10 })

          Text('确定要删除这个联系人吗？此操作不可撤销。')
            .fontSize(14)
            .textAlign(TextAlign.Center)
            .margin({ bottom: 20 })

          Row() {
            Button('取消')
              .layoutWeight(1)
              .height(40)
              .backgroundColor('#f0f0f0')
              .fontColor('#333')
              .onClick(() => {
                this.showDeleteConfirm = false;
              })

            Button('删除')
              .layoutWeight(1)
              .height(40)
              .margin({ left: 10 })
              .backgroundColor('#ff3b30')
              .fontColor(Color.White)
              .onClick(() => {
                this.showDeleteConfirm = false;
                this.deleteContact();
              })
          }
          .width('100%')
          .justifyContent(FlexAlign.SpaceBetween)
        }
        .width('80%')
        .padding(20)
        .backgroundColor(Color.White)
        .borderRadius(10)
        .shadow({ radius: 10, color: '#40000000', offsetX: 0, offsetY: 5 })
      }
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)
      .position({ x: 0, y: 0 })
    }
    ```

### 删除联系人逻辑
- 主页路径：确认后调用 `deleteContact(key)` 删除并刷新列表。

  ```typescript
  // Index.ets
  async deleteContact(key: string) {
    console.log('Index: 开始删除联系人，key:', key);
    try {
      await this.contactData.deleteContact(key);
      console.log('Index: 删除联系人成功，重新加载列表');
      this.loadContacts();
    } catch (error) {
      console.error('Index: 删除联系人失败:', error);
    }
  }
  ```

- 编辑页路径：确认后调用 `deleteContact()` 删除并返回上一页。

  ```typescript
  // EditContact.ets
  async deleteContact() {
    try {
      await this.contactData.deleteContact(this.contactKey);
      router.back();
    } catch (error) {
      console.error('删除联系人失败:');
    }
  }
  ```

- 底层实现：`ContactData.deleteContact(key)` 调用系统通讯录接口删除指定联系人。