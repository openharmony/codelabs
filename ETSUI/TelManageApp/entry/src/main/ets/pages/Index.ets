/*
 * 模块功能：首页联系人列表。
 * 作用：提供联系人搜索、字母索引、批量删除与二维码入口。
 * 输出：列表 UI 与操作入口。
 */

import { ContactData, BatchDeleteResult } from '../model/ContactData';
import { contact } from '@kit.ContactsKit';
import { router } from '@kit.ArkUI';
import { abilityAccessCtrl, Context } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { AlphabetIndexBar } from '../view/AlphabetIndexBar';
import { ContactGroup, LetterIndexUtil } from '../utils/LetterIndexUtil';

@Entry
@Component
struct Index {
  @Provide('context') context: Context = getContext(this);
  private contactData: ContactData = new ContactData(this.context);
  private listScroller: Scroller = new Scroller();
  private indexMap: Map<string, number> = new Map();

  @State contacts: contact.Contact[] = [];
  @State groupedContacts: ContactGroup[] = [];
  @State indexLetters: string[] = [];
  @State @Watch('onSelectedLetterChanged') selectedLetter: string = '';
  @State searchText: string = '';
  @State isLoading: boolean = true;
  @State isBatchMode: boolean = false;
  @State selectedKeys: string[] = [];
  @State isBatchDeleting: boolean = false;

  /**
   * 生命周期：页面即将显示时加载联系人。
   * 输入：无。
   * 输出：无。
   */
  aboutToAppear(): void {
    this.loadContacts();
  }

  /**
   * 生命周期：页面显示时刷新联系人列表。
   * 输入：无。
   * 输出：无。
   */
  onPageShow(): void {
    this.loadContacts();
  }

  /**
   * 加载全部联系人并更新列表状态。
   * 输入：无。
   * 输出：无。
   */
  async loadContacts(): Promise<void> {
    this.isLoading = true;
    try {
      const contacts = await this.contactData.queryAllContacts();
      this.applyContacts(contacts);
    } catch (error) {
      const err = error as BusinessError;
      // 如果依然 201，说明是签名 ACL 没过，而不只是用户没点同意
      if (err.code === 201) {
        console.error('Index: 严重错误 - 签名未包含受限权限 ACL');
      }
    } finally {
      this.isLoading = false;
    }
  }
  // async loadContacts() {
  //   // 拉取全量联系人并更新加载态与列表数据
  //   console.log('Index: 开始加载联系人列表');
  //   this.isLoading = true;
  //   try {
  //     this.contacts = await this.contactData.queryAllContacts();
  //     console.log('Index: 联系人列表加载完成，数量:', this.contacts.length);
  //
  //     // 检查联系人数据
  //     if (this.contacts.length > 0) {
  //       console.log('Index: 第一个联系人信息:', JSON.stringify(this.contacts[0]));
  //     }
  //   } catch (error) {
  //     console.error('Index: 加载联系人失败:', error);
  //   } finally {
  //     this.isLoading = false;
  //     console.log('Index: 加载状态设置为false');
  //   }
  // }

  /**
   * 搜索联系人并更新列表。
   * 输入：无。
   * 输出：无。
   */
  async searchContacts(): Promise<void> {
    if (this.searchText.trim() === '') {
      await this.loadContacts();
      return;
    }

    this.isLoading = true;
    try {
      const hasPermission = await this.requestContactPermission();
      if (!hasPermission) {
        this.isLoading = false;
        return;
      }

      const results = await this.searchByPhoneThenName(this.searchText);
      this.applyContacts(results);
    } catch (error) {
      console.error('Index: 搜索联系人失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  /**
   * 删除单个联系人。
   * 输入：key 联系人 key。
   * 输出：无。
   */
  async deleteContact(key: string): Promise<void> {
    try {
      await this.contactData.deleteContact(key);
      await this.loadContacts();
    } catch (error) {
      console.error('Index: 删除联系人失败:', error);
    }
  }

  /**
   * 显示删除确认弹窗（单个）。
   * 输入：contactItem 联系人对象。
   * 输出：无。
   */
  showDeleteConfirmDialog(contactItem: contact.Contact): void {
    if (!contactItem.key) {
      return;
    }

    const contactName = contactItem.name?.fullName || '未知联系人';
    AlertDialog.show({
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
    });
  }

  /**
   * 批量删除确认弹窗。
   * 输入：无。
   * 输出：无。
   */
  showBatchDeleteConfirm(): void {
    if (this.selectedKeys.length === 0) {
      return;
    }

    AlertDialog.show({
      title: '批量删除',
      message: `即将删除 ${this.selectedKeys.length} 个联系人，是否继续？`,
      autoCancel: true,
      alignment: DialogAlignment.Center,
      primaryButton: { value: '取消', action: () => {} },
      secondaryButton: {
        value: '继续',
        fontColor: '#ff3b30',
        action: () => {
          this.showBatchDeleteFinalConfirm();
        }
      }
    });
  }

  /**
   * 二次确认批量删除。
   * 输入：无。
   * 输出：无。
   */
  private showBatchDeleteFinalConfirm(): void {
    AlertDialog.show({
      title: '再次确认',
      message: '删除后无法恢复，请确认是否执行删除。',
      autoCancel: true,
      alignment: DialogAlignment.Center,
      primaryButton: { value: '取消', action: () => {} },
      secondaryButton: {
        value: '删除',
        fontColor: '#ff3b30',
        action: () => {
          this.performBatchDelete();
        }
      }
    });
  }

  /**
   * 执行批量删除流程。
   * 输入：无。
   * 输出：无。
   */
  async performBatchDelete(): Promise<void> {
    if (this.selectedKeys.length === 0) {
      return;
    }

    this.isBatchDeleting = true;
    try {
      const result = await this.contactData.deleteContacts(this.selectedKeys);
      this.handleBatchDeleteResult(result);
      await this.loadContacts();
    } catch (error) {
      console.error('Index: 批量删除失败:', error);
    } finally {
      this.isBatchDeleting = false;
      this.exitBatchMode();
    }
  }

  /**
   * 处理批量删除结果。
   * 输入：result 批量删除结果。
   * 输出：无。
   */
  private handleBatchDeleteResult(result: BatchDeleteResult): void {
    const failedCount = result.failedKeys.length;
    const message = failedCount === 0
      ? `已删除 ${result.successCount} 个联系人`
      : `成功 ${result.successCount} 个，失败 ${failedCount} 个`;

    AlertDialog.show({
      title: '删除结果',
      message: message,
      alignment: DialogAlignment.Center,
      primaryButton: { value: '确定', action: () => {} }
    });
  }

  /**
   * 进入或退出批量模式。
   * 输入：无。
   * 输出：无。
   */
  toggleBatchMode(): void {
    if (this.isBatchMode) {
      this.exitBatchMode();
      return;
    }
    this.enterBatchMode();
  }

  /**
   * 进入批量选择模式。
   * 输入：无。
   * 输出：无。
   */
  private enterBatchMode(): void {
    this.isBatchMode = true;
    this.selectedKeys = [];
  }

  /**
   * 退出批量选择模式。
   * 输入：无。
   * 输出：无。
   */
  private exitBatchMode(): void {
    this.isBatchMode = false;
    this.selectedKeys = [];
  }

  /**
   * 切换联系人选中状态。
   * 输入：key 联系人 key。
   * 输出：无。
   */
  toggleSelection(key: string): void {
    if (!key) {
      return;
    }

    if (this.selectedKeys.includes(key)) {
      this.selectedKeys = this.selectedKeys.filter((item) => item !== key);
      return;
    }

    this.selectedKeys = [...this.selectedKeys, key];
  }

  /**
   * 获取可选联系人 key 列表。
   * 输入：无。
   * 输出：联系人 key 数组。
   */
  private getSelectableKeys(): string[] {
    try {
      return this.contacts
        .map((item) => item.key || '')
        .filter((key) => key !== '');
    } catch (error) {
      console.error('Index: 生成可选 key 失败:', error);
      return [];
    }
  }

  /**
   * 判断是否已全选。
   * 输入：无。
   * 输出：是否全选。
   */
  private isAllSelected(): boolean {
    const keys = this.getSelectableKeys();
    return keys.length > 0 && keys.every((key) => this.selectedKeys.includes(key));
  }

  /**
   * 全选联系人。
   * 输入：无。
   * 输出：无。
   */
  private selectAllContacts(): void {
    try {
      const keys = this.getSelectableKeys();
      this.selectedKeys = keys;
    } catch (error) {
      console.error('Index: 全选失败:', error);
    }
  }

  /**
   * 清空选择。
   * 输入：无。
   * 输出：无。
   */
  private clearSelection(): void {
    this.selectedKeys = [];
  }

  /**
   * 切换全选/清空。
   * 输入：无。
   * 输出：无。
   */
  private toggleSelectAll(): void {
    if (this.isAllSelected()) {
      this.clearSelection();
      return;
    }
    this.selectAllContacts();
  }

  /**
   * 判断联系人是否已选中。
   * 输入：key 联系人 key。
   * 输出：是否选中。
   */
  isSelected(key: string): boolean {
    return this.selectedKeys.includes(key);
  }

  /**
   * 跳转到名片页面。
   * 输入：无。
   * 输出：无。
   */
  onMyBusinessCardClick(): void {
    router.pushUrl({ url: 'pages/MyBusinessCard' });
  }

  /**
   * 跳转到密令导入页面。
   * 输入：无。
   * 输出：无。
   */
  onSecretImportClick(): void {
    router.pushUrl({ url: 'pages/SecretImport' });
  }

  /**
   * 跳转到密令分享页面。
   * 输入：contactKey 联系人 key。
   * 输出：无。
   */
  onSecretShareClick(contactKey: string): void {
    router.pushUrl({ url: 'pages/SecretShare', params: { contactKey } });
  }

  /**
   * 触发字母索引滚动。
   * 输入：letter 字母。
   * 输出：无。
   */
  onSelectedLetterChanged(): void {
    if (!this.selectedLetter) {
      return;
    }
    const index = this.indexMap.get(this.selectedLetter);
    if (index === undefined) {
      return;
    }
    this.listScroller.scrollToIndex(index);
  }

  /**
   * 请求联系人读写权限。
   * 输入：无。
   * 输出：是否授权。
   */
  private async requestContactPermission(): Promise<boolean> {
    const atManager = abilityAccessCtrl.createAtManager();
    const result = await atManager.requestPermissionsFromUser(this.context, [
      'ohos.permission.READ_CONTACTS',
      'ohos.permission.WRITE_CONTACTS'
    ]);
    return result.authResults[0] === 0;
  }

  /**
   * 按电话与姓名进行搜索。
   * 输入：keyword 搜索关键字。
   * 输出：联系人列表。
   */
  private async searchByPhoneThenName(keyword: string): Promise<contact.Contact[]> {
    const phoneResults = await this.contactData.queryContactsByPhone(keyword);
    if (phoneResults.length > 0) {
      return phoneResults;
    }

    const allContacts = await this.contactData.queryAllContacts();
    return allContacts.filter((item) =>
      item.name?.fullName?.toLowerCase().includes(keyword.toLowerCase())
    );
  }

  /**
   * 应用联系人数据并刷新索引状态。
   * 输入：contacts 联系人列表。
   * 输出：无。
   */
  private applyContacts(contacts: contact.Contact[]): void {
    this.contacts = contacts;
    this.updateGroups(contacts);
  }

  /**
   * 更新分组数据与索引字母。
   * 输入：contacts 联系人列表。
   * 输出：无。
   */
  private updateGroups(contacts: contact.Contact[]): void {
    const groups = LetterIndexUtil.groupContacts(contacts);
    this.groupedContacts = groups;
    this.indexLetters = groups.map((group) => group.letter);
    this.indexMap = LetterIndexUtil.buildIndexMap(groups);
    this.selectedLetter = this.indexLetters[0] ?? '';
  }

        // 更多功能按钮组
        Row() {
          Button('添加')
            .fontSize(14)
            .height(32)
            .backgroundColor('#007DFF')
            .onClick(() => {
              console.log('Index: 点击添加按钮，跳转到添加页面');
              router.pushUrl({
                url: 'pages/AddContact'
              });
            })

          Button('去重')
            .fontSize(14)
            .height(32)
            .backgroundColor('#FF6D00')
            .margin({ left: 8 })
            .onClick(() => {
              console.log('Index: 点击去重按钮，跳转到去重管理页面');
              router.pushUrl({
                url: 'pages/DuplicateManager'
              });
            })

          Button('分组')
            .fontSize(14)
            .height(32)
            .backgroundColor('#00C853')
            .margin({ left: 8 })
            .onClick(() => {
              console.log('Index: 点击分组按钮，跳转到分组管理页面');
              router.pushUrl({
                url: 'pages/GroupManager'
              });
            })
        }
  /**
   * 判断是否展示索引条。
   * 输入：无。
   * 输出：是否显示索引条。
   */
  private shouldShowIndexBar(): boolean {
    return this.searchText.trim() === '' && this.indexLetters.length > 0 && !this.isBatchMode;
  }

  /**
   * 构建页面 UI。
   * 输入：无。
   * 输出：无。
   */
  build(): void {
    Stack() {
      Column() {
        this.buildHeader();
        this.buildSearchBar();
        this.buildShortcutButtons();
        this.buildContactSection();
      }
      .width('100%')
      .height('100%')

      if (this.isBatchMode) {
        this.buildBatchBar();
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  /**
   * 构建顶部标题栏。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildHeader(): void {
    Row() {
      this.buildHeaderTitle();
      this.buildHeaderActions();
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#f0f0f0')
  }

  /**
   * 构建标题栏标题。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildHeaderTitle(): void {
    Column() {
      Text('联系人管理')
        .fontSize(22)
        .fontWeight(FontWeight.Bold)

      Text(`共 ${this.contacts.length} 个联系人`)
        .fontSize(12)
        .fontColor('#666')
        .margin({ top: 4 })
    }
    .layoutWeight(1)
  }

  /**
   * 构建标题栏操作按钮。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildHeaderActions(): void {
    Button(this.isBatchMode ? '退出批量' : '批量删除')
      .fontSize(14)
      .margin({ right: 8 })
      .onClick(() => {
        this.toggleBatchMode();
      })

    Button('导入/导出')
      .fontSize(14)
      .margin({ right: 8 })
      .onClick(() => {
        router.pushUrl({ url: 'pages/ImportExport' });
      })

    Button('添加')
      .onClick(() => {
        router.pushUrl({ url: 'pages/AddContact' });
      })
  }

  /**
   * 构建搜索栏。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildSearchBar(): void {
    Column() {
      TextInput({ placeholder: '搜索联系人姓名或电话号码' })
        .width('100%')
        .height(40)
        .backgroundColor('#f4f4f4')
        .borderRadius(8)
        .padding({ left: 12, right: 12 })
        .onChange((value: string) => {
          this.searchText = value;
        })
        .onSubmit(() => {
          this.searchContacts();
        })
    }
    .width('90%')
    .padding(10)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
    .margin({ bottom: 12 })
  }

  /**
   * 构建快捷入口按钮区域。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildShortcutButtons(): void {
    Row() {
      Button('我的名片')
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .width('43%')
        .height(45)
        .backgroundColor('#007DFF')
        .fontColor(Color.White)
        .borderRadius(8)
        .onClick(() => {
          this.onMyBusinessCardClick();
        })

      Button('密令导入')
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .width('43%')
        .height(45)
        .margin({ left: 12 })
        .backgroundColor('#34C759')
        .fontColor(Color.White)
        .borderRadius(8)
        .onClick(() => {
          this.onSecretImportClick();
        })
    }
    .width('100%')
    .justifyContent(FlexAlign.Center)
    .margin({ bottom: 15 })
  }

  /**
   * 构建联系人列表区域。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildContactSection(): void {
    if (this.isLoading) {
      this.buildLoadingIndicator();
    } else if (this.contacts.length === 0) {
      this.buildEmptyState();
    } else {
      this.buildContactListWithIndex()
    }
  }

  /**
   * 构建加载状态。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildLoadingIndicator(): void {
    LoadingProgress()
      .width(50)
      .height(50)
      .margin({ top: 100 })
  }

  /**
   * 构建空列表状态。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildEmptyState(): void {
    Column() {
      Text(this.searchText ? '未找到匹配的联系人' : '暂无联系人')
        .fontSize(18)
        .fontColor('#666')
    }
    .width('100%')
    .height('70%')
    .justifyContent(FlexAlign.Center)
  }

  /**
   * 构建带索引条的联系人列表。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildContactListWithIndex(): void {
    Row() {
      this.buildGroupedList()

      if (this.shouldShowIndexBar()) {
        AlphabetIndexBar({
          letters: this.indexLetters,
          selectedLetter: $selectedLetter
        })
        .height('100%')
        .margin({ right: 6 })
      }
    }
    .width('100%')
    .layoutWeight(1)
    .padding({ left: 12, right: 6, bottom: 12 })
  }

  /**
   * 构建分组列表。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildGroupedList(): void {
    List({ space: 6, scroller: this.listScroller }) {
      ForEach(this.groupedContacts, (group: ContactGroup) => {
        ListItemGroup({ header: this.groupHeader(group.letter) }) {
          ForEach(group.contacts, (item: contact.Contact) => {
            ListItem() {
              this.contactItem(item);
            }
          }, (item: contact.Contact) => item.key || item.id?.toString() || '')
        }
      }, (group: ContactGroup) => group.letter)
    }
    .width('100%')
    .layoutWeight(1)
  }

  /**
   * 构建分组头部。
   * 输入：letter 分组字母。
   * 输出：无。
   */
  @Builder
  private groupHeader(letter: string): void {
    Text(letter)
      .width('100%')
      .height(28)
      .lineHeight(28)
      .fontSize(14)
      .fontWeight(FontWeight.Bold)
      .fontColor('#666')
      .backgroundColor('#EFEFEF')
      .padding({ left: 12 })
  }

  /**
   * 构建批量删除底部工具条。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildBatchBar(): void {
    Row() {
      this.buildBatchInfo();
      this.buildBatchActions();
    }
    .width('100%')
    .padding(12)
    .backgroundColor(Color.White)
    .position({ x: 0, y: '86%' })
  }

  /**
   * 构建批量状态信息。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildBatchInfo(): void {
    Text(`已选 ${this.selectedKeys.length}`)
      .fontSize(12)
      .fontColor('#666')
      .margin({ right: 10 })

    Button(this.isAllSelected() ? '清空' : '全选')
      .height(32)
      .margin({ right: 10 })
      .backgroundColor('#eaeaea')
      .fontColor('#333')
      .onClick(() => {
        this.toggleSelectAll();
      })
  }

  /**
   * 构建批量操作按钮。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildBatchActions(): void {
    Button(this.isBatchDeleting ? '删除中...' : `删除(${this.selectedKeys.length})`)
      .layoutWeight(1)
      .height(44)
      .backgroundColor(this.selectedKeys.length > 0 ? '#ff3b30' : '#ccc')
      .fontColor(Color.White)
      .enabled(this.selectedKeys.length > 0 && !this.isBatchDeleting)
      .onClick(() => {
        this.showBatchDeleteConfirm();
      })

    Button('取消')
      .layoutWeight(1)
      .height(44)
      .margin({ left: 10 })
      .backgroundColor('#e0e0e0')
      .fontColor('#333')
      .onClick(() => {
        this.exitBatchMode();
      })
  }

  /**
   * 构建联系人列表项。
   * 输入：contactItem 联系人对象。
   * 输出：无。
   */
  @Builder
  private contactItem(contactItem: contact.Contact): void {
    Row() {
      if (this.isBatchMode) {
        Toggle({ type: ToggleType.Checkbox, isOn: this.isSelected(this.getContactKey(contactItem)) })
          .enabled(false)
          .margin({ right: 8 })
      }

      this.buildAvatar(contactItem);
      this.buildContactInfo(contactItem);
      this.buildActionButtons(contactItem);
    }
    .width('100%')
    .padding(12)
    .backgroundColor(Color.White)
    .borderRadius(10)
    .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
    .onClick(() => {
      this.handleItemClick(contactItem);
    })
    .gesture(
      LongPressGesture()
        .onAction(() => {
          this.handleItemLongPress(contactItem);
        })
    )
  }

  /**
   * 构建联系人头像区域。
   * 输入：contactItem 联系人对象。
   * 输出：无。
   */
  @Builder
  private buildAvatar(contactItem: contact.Contact): void {
    Column() {
      if (contactItem.portrait?.uri) {
        Image(contactItem.portrait.uri)
          .width(50)
          .height(50)
          .borderRadius(25)
      } else {
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
    .margin({ right: 12 })
  }

  /**
   * 构建联系人信息区域。
   * 输入：contactItem 联系人对象。
   * 输出：无。
   */
  @Builder
  private buildContactInfo(contactItem: contact.Contact): void {
    Column() {
      Text(contactItem.name?.fullName || '未知')
        .fontSize(18)
        .fontWeight(FontWeight.Medium)
        .textAlign(TextAlign.Start)
        .width('100%')

      if (contactItem.phoneNumbers && contactItem.phoneNumbers.length > 0) {
        Text(contactItem.phoneNumbers[0].phoneNumber)
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ top: 5 })
      }

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
  }

  /**
   * 构建操作按钮区域。
   * 输入：contactItem 联系人对象。
   * 输出：无。
   */
  @Builder
  private buildActionButtons(contactItem: contact.Contact): void {
    if (this.isBatchMode) {
      Blank();
    } else {
      Row() {
        Button('密令')
          .fontSize(12)
          .width(60)
          .height(30)
          .margin({ right: 8 })
          .onClick(() => {
            this.onSecretShareClick(this.getContactKey(contactItem));
          })

        Button('编辑')
          .fontSize(12)
          .width(60)
          .height(30)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/EditContact',
              params: { contactKey: this.getContactKey(contactItem), parentContext: this.context }
            });
          })

        Button('删除')
          .fontSize(12)
          .width(60)
          .height(30)
          .margin({ left: 8 })
          .backgroundColor('#ff3b30')
          .fontColor(Color.White)
          .onClick(() => {
            this.showDeleteConfirmDialog(contactItem);
          })
      }
    }
  }

  /**
   * 处理列表项点击事件。
   * 输入：contactItem 联系人对象。
   * 输出：无。
   */
  private handleItemClick(contactItem: contact.Contact): void {
    if (this.isBatchMode) {
      this.toggleSelection(this.getContactKey(contactItem));
      return;
    }
    router.pushUrl({
      url: 'pages/EditContact',
      params: { contactKey: this.getContactKey(contactItem), parentContext: this.context }
    });
  }

  /**
   * 处理列表项长按事件。
   * 输入：contactItem 联系人对象。
   * 输出：无。
   */
  private handleItemLongPress(contactItem: contact.Contact): void {
    if (!this.isBatchMode) {
      this.enterBatchMode();
    }
    this.toggleSelection(this.getContactKey(contactItem));
  }

  /**
   * 获取联系人 key。
   * 输入：contactItem 联系人对象。
   * 输出：联系人 key 字符串。
   */
  private getContactKey(contactItem: contact.Contact): string {
    return contactItem.key || '';
  }
}