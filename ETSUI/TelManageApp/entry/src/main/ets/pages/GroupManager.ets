/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * 联系人分组管理页面
 * 提供分组创建、编辑、删除、联系人分配等完整功能
 */

import { contact } from '@kit.ContactsKit';
import { promptAction, router } from '@kit.ArkUI';
import { abilityAccessCtrl, Context } from '@kit.AbilityKit';
import { ContactData } from '../model/ContactData';
import {
  ContactGroupModel,
  GROUP_COLORS,
  GROUP_ICONS,
  GroupManager,
  GroupSortOrder,
  GroupStatistics,
  GroupType,
  SmartGroupGenerator
} from '../model/ContactGroup';

/**
 * 视图模式枚举
 */
enum ViewMode {
  GROUP_LIST = 'GROUP_LIST', // 分组列表
  GROUP_DETAIL = 'GROUP_DETAIL', // 分组详情（查看分组内联系人）
  GROUP_EDIT = 'GROUP_EDIT', // 编辑分组
  GROUP_CREATE = 'GROUP_CREATE', // 创建分组
  CONTACT_SELECT = 'CONTACT_SELECT', // 选择联系人
  SMART_GROUP = 'SMART_GROUP' // 智能分组
}

/**
 * 智能分组类型
 */
enum SmartGroupType {
  BY_LAST_NAME = 'BY_LAST_NAME',
  BY_FIRST_LETTER = 'BY_FIRST_LETTER',
  BY_EMAIL_DOMAIN = 'BY_EMAIL_DOMAIN',
  BY_PHONE_AREA = 'BY_PHONE_AREA',
  BY_ORGANIZATION = 'BY_ORGANIZATION'
}

/**
 * 排序选项数据接口
 */
interface SortOptionItem {
  order: GroupSortOrder;
  label: string;
}

/**
 * 智能分组选项数据接口
 */
interface SmartGroupOptionItem {
  type: SmartGroupType;
  label: string;
  desc?: string;
}

@Entry
@Component
struct GroupManagerPage {
  // 上下文与数据层
  @Provide('context') context: Context = getContext(this);
  private contactData: ContactData = new ContactData(this.context);
  private groupManager: GroupManager = new GroupManager();
  // 预定义的排序选项数组
  private sortOptions: SortOptionItem[] = [
    { order: GroupSortOrder.CUSTOM, label: '默认' },
    { order: GroupSortOrder.NAME_ASC, label: '名称' },
    { order: GroupSortOrder.COUNT_DESC, label: '人数' }
  ];
  // 智能分组选项数组
  private smartGroupOptions: SmartGroupOptionItem[] = [
    { type: SmartGroupType.BY_LAST_NAME, label: '按姓氏分组', desc: '将相同姓氏的联系人分为一组' },
    { type: SmartGroupType.BY_FIRST_LETTER, label: '按首字母分组', desc: '按联系人姓名首字母分组' },
    { type: SmartGroupType.BY_EMAIL_DOMAIN, label: '按邮箱域名分组', desc: '相同邮箱域名的联系人分为一组' },
    { type: SmartGroupType.BY_PHONE_AREA, label: '按电话区号分组', desc: '按电话号码归属地分组' },
    { type: SmartGroupType.BY_ORGANIZATION, label: '按公司/组织分组', desc: '相同公司的联系人分为一组' }
  ];
  // 智能分组对话框选项数组
  private smartGroupDialogOptions: SmartGroupOptionItem[] = [
    { type: SmartGroupType.BY_LAST_NAME, label: '按姓氏' },
    { type: SmartGroupType.BY_FIRST_LETTER, label: '按首字母' },
    { type: SmartGroupType.BY_EMAIL_DOMAIN, label: '按邮箱域名' },
    { type: SmartGroupType.BY_PHONE_AREA, label: '按电话区号' },
    { type: SmartGroupType.BY_ORGANIZATION, label: '按公司' }
  ];
  // 页面状态
  @State viewMode: ViewMode = ViewMode.GROUP_LIST;
  @State isLoading: boolean = false;
  @State errorMessage: string = '';
  // 数据状态
  @State allContacts: contact.Contact[] = [];
  @State groups: ContactGroupModel[] = [];
  @State selectedGroup: ContactGroupModel | null = null;
  @State groupContacts: contact.Contact[] = [];
  @State statistics: GroupStatistics | null = null;
  // 创建/编辑分组状态
  @State editingGroupName: string = '';
  @State editingGroupDescription: string = '';
  @State editingGroupColor: string = GROUP_COLORS[0];
  @State editingGroupIcon: string = GROUP_ICONS[0];
  @State isEditMode: boolean = false;
  // 联系人选择状态
  @State selectedContactKeys: Set<string> = new Set();
  @State searchText: string = '';
  @State filteredContacts: contact.Contact[] = [];
  // 排序状态
  @State currentSortOrder: GroupSortOrder = GroupSortOrder.CUSTOM;
  // 弹窗状态
  @State showDeleteConfirm: boolean = false;
  @State showSmartGroupDialog: boolean = false;
  @State selectedSmartGroupType: SmartGroupType = SmartGroupType.BY_LAST_NAME;
  @State showSaveConfirm: boolean = false;
  @State showCompleteConfirm: boolean = false;
  @State isGroupManagerInitialized: boolean = false;

  aboutToAppear() {
    console.log('GroupManagerPage: 页面即将显示');
    this.checkPermissionsAndLoad();
  }

  /**
   * 检查权限并加载数据
   */
  async checkPermissionsAndLoad() {
    const atManager = abilityAccessCtrl.createAtManager();
    try {
      const result = await atManager.requestPermissionsFromUser(this.context, [
        'ohos.permission.READ_CONTACTS',
        'ohos.permission.WRITE_CONTACTS'
      ]);

      if (result.authResults[0] === 0) {
        await this.loadContacts();
        await this.initializeGroupManager();
        this.loadGroups();
      } else {
        this.errorMessage = '需要通讯录权限';
      }
    } catch (err) {
      console.error('GroupManagerPage: 权限请求失败', err);
    }
  }

  /**
   * 初始化分组管理器（从存储加载数据）
   */
  async initializeGroupManager() {
    if (!this.isGroupManagerInitialized) {
      this.isLoading = true;
      try {
        await this.groupManager.initialize(this.context);
        this.isGroupManagerInitialized = true;
        console.log('GroupManagerPage: 分组管理器初始化完成');
      } catch (error) {
        console.error('GroupManagerPage: 分组管理器初始化失败', error);
      } finally {
        this.isLoading = false;
      }
    }
  }

  /**
   * 保存分组数据到存储
   */
  async saveGroupData() {
    try {
      await this.groupManager.saveToStorage();
    } catch (error) {
      console.error('GroupManagerPage: 保存分组数据失败', error);
    }
  }

  /**
   * 加载联系人
   */
  async loadContacts() {
    this.isLoading = true;
    try {
      this.allContacts = await this.contactData.queryAllContacts();
      this.filteredContacts = this.allContacts.slice();
      console.log(`GroupManagerPage: 已加载 ${this.allContacts.length} 个联系人`);
    } catch (error) {
      console.error('GroupManagerPage: 加载联系人失败', error);
    } finally {
      this.isLoading = false;
    }
  }

  /**
   * 加载分组
   */
  loadGroups() {
    this.groups = this.groupManager.getAllGroups(this.currentSortOrder);
    this.statistics = this.groupManager.getStatistics(
      this.allContacts.map(c => c.key || '')
    );
  }

  /**
   * 查看分组详情
   */
  viewGroupDetail(group: ContactGroupModel) {
    this.selectedGroup = group;
    this.loadGroupContacts();
    this.viewMode = ViewMode.GROUP_DETAIL;
  }

  /**
   * 加载分组内的联系人
   */
  loadGroupContacts() {
    if (!this.selectedGroup) {
      return;
    }

    this.groupContacts = this.allContacts.filter(c =>
    c.key && this.selectedGroup!.hasContact(c.key)
    );
  }

  /**
   * 创建新分组
   */
  async createGroup() {
    if (!this.editingGroupName.trim()) {
      promptAction.showToast({ message: '请输入分组名称' });
      return;
    }

    const result = this.groupManager.createGroup(this.editingGroupName.trim());

    if (result.success && result.affectedGroupId) {
      const group = this.groupManager.getGroup(result.affectedGroupId);
      if (group) {
        group.description = this.editingGroupDescription;
        group.color = this.editingGroupColor;
        group.icon = this.editingGroupIcon;
      }

      await this.saveGroupData();
      promptAction.showToast({ message: '分组创建成功' });
      this.loadGroups();
      this.resetEditingState();
      this.viewMode = ViewMode.GROUP_LIST;
    } else {
      promptAction.showToast({ message: result.message });
    }
  }

  /**
   * 更新分组
   */
  async updateGroup() {
    if (!this.selectedGroup) {
      return;
    }

    if (!this.editingGroupName.trim()) {
      promptAction.showToast({ message: '请输入分组名称' });
      return;
    }

    const result = this.groupManager.updateGroup(this.selectedGroup.groupId, {
      name: this.editingGroupName.trim(),
      description: this.editingGroupDescription,
      color: this.editingGroupColor,
      icon: this.editingGroupIcon
    });

    if (result.success) {
      await this.saveGroupData();
      promptAction.showToast({ message: '分组已更新' });
      this.loadGroups();
      this.resetEditingState();
      this.viewMode = ViewMode.GROUP_LIST;
    } else {
      promptAction.showToast({ message: result.message });
    }
  }

  /**
   * 删除分组
   */
  async deleteGroup() {
    if (!this.selectedGroup) {
      return;
    }

    const result = this.groupManager.deleteGroup(this.selectedGroup.groupId);

    if (result.success) {
      await this.saveGroupData();
      promptAction.showToast({ message: '分组已删除' });
      this.loadGroups();
      this.selectedGroup = null;
      this.showDeleteConfirm = false;
      this.viewMode = ViewMode.GROUP_LIST;
    } else {
      promptAction.showToast({ message: result.message });
    }
  }

  /**
   * 进入编辑模式
   */
  enterEditMode(group: ContactGroupModel) {
    this.selectedGroup = group;
    this.editingGroupName = group.name;
    this.editingGroupDescription = group.description;
    this.editingGroupColor = group.color;
    this.editingGroupIcon = group.icon;
    this.isEditMode = true;
    this.viewMode = ViewMode.GROUP_EDIT;
  }

  /**
   * 进入创建模式
   */
  enterCreateMode() {
    this.resetEditingState();
    this.isEditMode = false;
    this.viewMode = ViewMode.GROUP_CREATE;
  }

  /**
   * 重置编辑状态
   */
  resetEditingState() {
    this.editingGroupName = '';
    this.editingGroupDescription = '';
    this.editingGroupColor = GROUP_COLORS[0];
    this.editingGroupIcon = GROUP_ICONS[0];
    this.isEditMode = false;
  }

  /**
   * 进入联系人选择模式
   */
  enterContactSelectMode() {
    if (!this.selectedGroup) {
      return;
    }

    // 初始化已选中的联系人
    this.selectedContactKeys = new Set(this.selectedGroup.contactKeys);
    this.searchText = '';
    this.filteredContacts = this.allContacts.slice();
    this.viewMode = ViewMode.CONTACT_SELECT;
  }

  /**
   * 切换联系人选中状态
   */
  toggleContactSelection(contactKey: string) {
    if (this.selectedContactKeys.has(contactKey)) {
      this.selectedContactKeys.delete(contactKey);
    } else {
      this.selectedContactKeys.add(contactKey);
    }
    // 触发UI更新
    this.selectedContactKeys = new Set(this.selectedContactKeys);
  }

  /**
   * 保存联系人选择
   */
  async saveContactSelection() {
    if (!this.selectedGroup) {
      return;
    }

    // 计算添加和移除的联系人
    const currentKeys = new Set(this.selectedGroup.contactKeys);
    const newKeys = this.selectedContactKeys;

    // 移除不再选中的
    currentKeys.forEach(key => {
      if (!newKeys.has(key)) {
        this.groupManager.removeContactFromGroup(this.selectedGroup!.groupId, key);
      }
    });

    // 添加新选中的
    newKeys.forEach(key => {
      if (!currentKeys.has(key)) {
        this.groupManager.addContactToGroup(this.selectedGroup!.groupId, key);
      }
    });

    await this.saveGroupData();
    promptAction.showToast({ message: '联系人已更新' });
    this.loadGroups();
    this.loadGroupContacts();
    this.viewMode = ViewMode.GROUP_DETAIL;
  }

  /**
   * 搜索联系人
   */
  searchContacts() {
    if (!this.searchText.trim()) {
      this.filteredContacts = this.allContacts.slice();
      return;
    }

    const keyword = this.searchText.toLowerCase();
    this.filteredContacts = this.allContacts.filter(c => {
      const name = c.name?.fullName?.toLowerCase() || '';
      const phone = c.phoneNumbers?.[0]?.phoneNumber || '';
      return name.includes(keyword) || phone.includes(keyword);
    });
  }

  /**
   * 生成智能分组
   */
  async generateSmartGroups() {
    let groupsMap: Map<string, string[]>;

    switch (this.selectedSmartGroupType) {
      case SmartGroupType.BY_LAST_NAME:
        groupsMap = SmartGroupGenerator.groupByLastName(this.allContacts);
        break;
      case SmartGroupType.BY_FIRST_LETTER:
        groupsMap = SmartGroupGenerator.groupByFirstLetter(this.allContacts);
        break;
      case SmartGroupType.BY_EMAIL_DOMAIN:
        groupsMap = SmartGroupGenerator.groupByEmailDomain(this.allContacts);
        break;
      case SmartGroupType.BY_PHONE_AREA:
        groupsMap = SmartGroupGenerator.groupByPhoneAreaCode(this.allContacts);
        break;
      case SmartGroupType.BY_ORGANIZATION:
        groupsMap = SmartGroupGenerator.groupByOrganization(this.allContacts);
        break;
      default:
        groupsMap = new Map();
    }

    // 创建分组
    let createdCount = 0;
    groupsMap.forEach((contactKeys, groupName) => {
      if (contactKeys.length > 0) {
        const result = this.groupManager.createGroup(`${groupName}`, GroupType.SMART);
        if (result.success && result.affectedGroupId) {
          this.groupManager.addContactsToGroup(result.affectedGroupId, contactKeys);
          createdCount++;
        }
      }
    });

    await this.saveGroupData();
    promptAction.showToast({ message: `已创建 ${createdCount} 个智能分组` });
    this.loadGroups();
    this.showSmartGroupDialog = false;
    this.viewMode = ViewMode.GROUP_LIST;
  }

  /**
   * 从分组移除联系人
   */
  async removeContactFromGroup(contactKey: string) {
    if (!this.selectedGroup) {
      return;
    }

    const result = this.groupManager.removeContactFromGroup(
      this.selectedGroup.groupId,
      contactKey
    );

    if (result.success) {
      await this.saveGroupData();
      promptAction.showToast({ message: '已从分组移除' });
      this.loadGroups();
      this.loadGroupContacts();
    }
  }

  build() {
    Stack() {
      Column() {
        // 标题栏
        this.TitleBar()

        // 根据视图模式显示不同内容
        if (this.viewMode === ViewMode.GROUP_LIST) {
          this.GroupListView()
        } else if (this.viewMode === ViewMode.GROUP_DETAIL) {
          this.GroupDetailView()
        } else if (this.viewMode === ViewMode.GROUP_EDIT ||
          this.viewMode === ViewMode.GROUP_CREATE) {
          this.GroupEditView()
        } else if (this.viewMode === ViewMode.CONTACT_SELECT) {
          this.ContactSelectView()
        } else if (this.viewMode === ViewMode.SMART_GROUP) {
          this.SmartGroupView()
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#f5f5f5')

      // 删除确认弹窗
      if (this.showDeleteConfirm) {
        this.DeleteConfirmDialog()
      }

      // 智能分组弹窗
      if (this.showSmartGroupDialog) {
        this.SmartGroupDialog()
      }

      // 加载遮罩
      if (this.isLoading) {
        this.LoadingOverlay()
      }

      // 保存确认弹窗
      if (this.showSaveConfirm) {
        this.SaveConfirmDialog()
      }

      // 完成确认弹窗
      if (this.showCompleteConfirm) {
        this.CompleteConfirmDialog()
      }
    }
    .width('100%')
    .height('100%')
  }

  /**
   * 标题栏
   */
  @Builder
  TitleBar() {
    Row() {
      // 返回按钮
      Button(this.viewMode === ViewMode.GROUP_LIST ? '返回' : '取消')
        .fontSize(16)
        .backgroundColor(Color.Transparent)
        .fontColor('#007DFF')
        .onClick(() => {
          if (this.viewMode === ViewMode.GROUP_LIST) {
            router.back();
          } else if (this.viewMode === ViewMode.GROUP_DETAIL) {
            this.viewMode = ViewMode.GROUP_LIST;
            this.selectedGroup = null;
          } else {
            this.viewMode = this.selectedGroup ? ViewMode.GROUP_DETAIL : ViewMode.GROUP_LIST;
          }
        })

      Text(this.getTitleText())
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Center)
        .layoutWeight(1)

      // 右侧按钮
      if (this.viewMode === ViewMode.GROUP_LIST) {
        Button('新建')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => this.enterCreateMode())
      } else if (this.viewMode === ViewMode.GROUP_DETAIL) {
        Button('编辑')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => this.enterEditMode(this.selectedGroup!))
      } else if (this.viewMode === ViewMode.GROUP_EDIT ||
        this.viewMode === ViewMode.GROUP_CREATE) {
        Button('保存')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => {
            this.showSaveConfirm = true;
          })
      } else if (this.viewMode === ViewMode.CONTACT_SELECT) {
        Button('完成')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => {
            this.showCompleteConfirm = true;
          })
      } else {
        Text('').width(60)
      }
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .backgroundColor('#f0f0f0')
  }

  /**
   * 获取标题文本
   */
  getTitleText(): string {
    switch (this.viewMode) {
      case ViewMode.GROUP_DETAIL:
        return this.selectedGroup?.name || '分组详情';
      case ViewMode.GROUP_EDIT:
        return '编辑分组';
      case ViewMode.GROUP_CREATE:
        return '新建分组';
      case ViewMode.CONTACT_SELECT:
        return '选择联系人';
      case ViewMode.SMART_GROUP:
        return '智能分组';
      default:
        return '分组管理';
    }
  }

  /**
   * 分组列表视图
   */
  @Builder
  GroupListView() {
    Column() {
      // 统计信息
      if (this.statistics) {
        this.StatisticsCard()
      }

      // 智能分组入口
      this.SmartGroupEntry()

      // 排序选项
      this.SortOptions()

      // 分组列表
      if (this.groups.length === 0) {
        this.EmptyGroupView()
      } else {
        List({ space: 10 }) {
          ForEach(this.groups, (group: ContactGroupModel) => {
            ListItem() {
              this.GroupCard(group)
            }
          }, (group: ContactGroupModel) => group.groupId)
        }
        .width('100%')
        .layoutWeight(1)
        .padding({ left: 16, right: 16, bottom: 16 })
      }
    }
    .width('100%')
    .layoutWeight(1)
  }

  /**
   * 统计信息卡片
   */
  @Builder
  StatisticsCard() {
    Column() {
      Row() {
        this.StatItem('分组数', this.statistics!.totalGroups.toString())
        this.StatItem('已分组', this.statistics!.totalContactsInGroups.toString())
        this.StatItem('未分组', this.statistics!.ungroupedContacts.toString())
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceAround)
    }
    .width('100%')
    .padding(16)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .margin({ left: 16, right: 16, top: 16 })
    .shadow({ radius: 4, color: '#10000000', offsetY: 2 })
  }

  /**
   * 统计项
   */
  @Builder
  StatItem(label: string, value: string) {
    Column() {
      Text(value)
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor('#007DFF')

      Text(label)
        .fontSize(12)
        .fontColor('#666')
        .margin({ top: 4 })
    }
  }

  /**
   * 智能分组入口
   */
  @Builder
  SmartGroupEntry() {
    Row() {
      Text('✨')
        .fontSize(20)

      Text('智能分组')
        .fontSize(15)
        .margin({ left: 8 })

      Text('自动按规则分组联系人')
        .fontSize(12)
        .fontColor('#666')
        .margin({ left: 8 })
        .layoutWeight(1)

      Text('>')
        .fontSize(16)
        .fontColor('#999')
    }
    .width('100%')
    .height(50)
    .padding({ left: 16, right: 16 })
    .margin({ left: 16, right: 16, top: 12 })
    .backgroundColor(Color.White)
    .borderRadius(10)
    .onClick(() => {
      this.showSmartGroupDialog = true;
    })
  }

  /**
   * 排序选项
   */
  @Builder
  SortOptions() {
    Row() {
      Text('排序:')
        .fontSize(12)
        .fontColor('#666')

      ForEach(this.sortOptions, (item: SortOptionItem) => {
        Text(item.label)
          .fontSize(12)
          .fontColor(this.currentSortOrder === item.order ? '#007DFF' : '#999')
          .padding({ left: 12, right: 12 })
          .onClick(() => {
            this.currentSortOrder = item.order;
            this.loadGroups();
          })
      }, (item: SortOptionItem) => item.order)
    }
    .width('100%')
    .padding({
      left: 16,
      right: 16,
      top: 12,
      bottom: 8
    })
  }

  /**
   * 分组卡片
   */
  @Builder
  GroupCard(group: ContactGroupModel) {
    Row() {
      // 分组图标
      Text(this.getGroupIcon(group.icon))
        .fontSize(24)
        .width(44)
        .height(44)
        .textAlign(TextAlign.Center)
        .borderRadius(22)
        .backgroundColor(group.color + '20')

      Column() {
        Row() {
          Text(group.name)
            .fontSize(16)
            .fontWeight(FontWeight.Medium)

          if (group.isDefault) {
            Text('默认')
              .fontSize(10)
              .fontColor('#666')
              .padding({
                left: 6,
                right: 6,
                top: 2,
                bottom: 2
              })
              .backgroundColor('#f0f0f0')
              .borderRadius(4)
              .margin({ left: 8 })
          }

          if (group.type === GroupType.SMART) {
            Text('智能')
              .fontSize(10)
              .fontColor('#007DFF')
              .padding({
                left: 6,
                right: 6,
                top: 2,
                bottom: 2
              })
              .backgroundColor('#007DFF20')
              .borderRadius(4)
              .margin({ left: 8 })
          }
        }

        Text(`${group.getContactCount()} 位联系人`)
          .fontSize(12)
          .fontColor('#666')
          .margin({ top: 4 })

        if (group.description) {
          Text(group.description)
            .fontSize(11)
            .fontColor('#999')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
            .margin({ top: 2 })
        }
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
      .margin({ left: 12 })

      Text('>')
        .fontSize(18)
        .fontColor('#ccc')
    }
    .width('100%')
    .padding(16)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({ radius: 2, color: '#10000000', offsetY: 1 })
    .onClick(() => this.viewGroupDetail(group))
  }

  /**
   * 获取分组图标
   */
  getGroupIcon(iconName: string): string {
    const iconMap: Record<string, string> = {
      'people': '👥',
      'star': '⭐',
      'work': '💼',
      'home': '🏠',
      'school': '🎓',
      'favorite': '❤️',
      'phone': '📞',
      'mail': '📧',
      'location': '📍',
      'event': '📅',
      'business': '🏢',
      'groups': '👨‍👩‍👧‍👦'
    };
    return iconMap[iconName] || '📁';
  }

  /**
   * 空分组视图
   */
  @Builder
  EmptyGroupView() {
    Column() {
      Text('📂')
        .fontSize(60)
        .margin({ bottom: 16 })

      Text('暂无分组')
        .fontSize(18)
        .fontColor('#666')

      Text('点击右上角"新建"创建分组')
        .fontSize(14)
        .fontColor('#999')
        .margin({ top: 8 })
    }
    .width('100%')
    .layoutWeight(1)
    .justifyContent(FlexAlign.Center)
  }

  /**
   * 分组详情视图
   */
  @Builder
  GroupDetailView() {
    if (this.selectedGroup) {
      Column() {
        // 分组信息头部
        Column() {
          Row() {
            Text(this.getGroupIcon(this.selectedGroup.icon))
              .fontSize(40)
              .width(60)
              .height(60)
              .textAlign(TextAlign.Center)
              .borderRadius(30)
              .backgroundColor(this.selectedGroup.color + '30')

            Column() {
              Text(this.selectedGroup.name)
                .fontSize(20)
                .fontWeight(FontWeight.Bold)

              Text(`${this.selectedGroup.getContactCount()} 位联系人`)
                .fontSize(14)
                .fontColor('#666')
                .margin({ top: 4 })

              if (this.selectedGroup.description) {
                Text(this.selectedGroup.description)
                  .fontSize(12)
                  .fontColor('#999')
                  .margin({ top: 4 })
              }
            }
            .alignItems(HorizontalAlign.Start)
            .margin({ left: 16 })
            .layoutWeight(1)
          }
          .width('100%')

          // 操作按钮
          Row() {
            Button('添加成员')
              .layoutWeight(1)
              .height(40)
              .backgroundColor('#007DFF')
              .onClick(() => this.enterContactSelectMode())

            if (!this.selectedGroup.isDefault) {
              Button('删除分组')
                .layoutWeight(1)
                .height(40)
                .backgroundColor('#ff3b30')
                .margin({ left: 12 })
                .onClick(() => {
                  this.showDeleteConfirm = true;
                })
            }
          }
          .width('100%')
          .margin({ top: 16 })
        }
        .width('100%')
        .padding(20)
        .backgroundColor(Color.White)
        .borderRadius(12)
        .margin({ left: 16, right: 16, top: 16 })

        // 成员列表
        Text('分组成员')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .width('100%')
          .padding({ left: 16, top: 16, bottom: 8 })

        if (this.groupContacts.length === 0) {
          Column() {
            Text('暂无成员')
              .fontSize(14)
              .fontColor('#999')

            Text('点击"添加成员"将联系人加入分组')
              .fontSize(12)
              .fontColor('#ccc')
              .margin({ top: 4 })
          }
          .width('100%')
          .padding(40)
        } else {
          List({ space: 8 }) {
            ForEach(this.groupContacts, (contactItem: contact.Contact) => {
              ListItem() {
                this.ContactInGroupCard(contactItem)
              }
            }, (item: contact.Contact) => item.key || '')
          }
          .width('100%')
          .layoutWeight(1)
          .padding({ left: 16, right: 16 })
        }
      }
      .width('100%')
      .layoutWeight(1)
    }
  }

  /**
   * 分组中的联系人卡片
   */
  @Builder
  ContactInGroupCard(contactItem: contact.Contact) {
    Row() {
      // 头像
      Text(contactItem.name?.fullName?.charAt(0) || '?')
        .fontSize(16)
        .fontColor(Color.White)
        .textAlign(TextAlign.Center)
        .width(40)
        .height(40)
        .borderRadius(20)
        .backgroundColor('#007DFF')

      Column() {
        Text(contactItem.name?.fullName || '未知')
          .fontSize(15)
          .fontWeight(FontWeight.Medium)

        if (contactItem.phoneNumbers && contactItem.phoneNumbers.length > 0) {
          Text(contactItem.phoneNumbers[0].phoneNumber || '')
            .fontSize(12)
            .fontColor('#666')
            .margin({ top: 2 })
        }
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
      .margin({ left: 12 })

      // 移除按钮
      Button('移除')
        .fontSize(12)
        .height(28)
        .backgroundColor('#ff3b30')
        .onClick(() => {
          if (contactItem.key) {
            this.removeContactFromGroup(contactItem.key);
          }
        })
    }
    .width('100%')
    .padding(12)
    .backgroundColor(Color.White)
    .borderRadius(8)
  }

  /**
   * 分组编辑视图
   */
  @Builder
  GroupEditView() {
    Scroll() {
      Column() {
        // 分组名称
        Column() {
          Text('分组名称')
            .fontSize(14)
            .fontColor('#666')
            .width('100%')
            .margin({ bottom: 8 })

          TextInput({
            text: this.editingGroupName,
            placeholder: '请输入分组名称'
          })
            .width('100%')
            .height(48)
            .fontSize(16)
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.editingGroupName = value;
            })
        }
        .width('100%')
        .padding(16)
        .backgroundColor(Color.White)
        .borderRadius(12)
        .margin({ top: 16 })

        // 分组描述
        Column() {
          Text('分组描述（可选）')
            .fontSize(14)
            .fontColor('#666')
            .width('100%')
            .margin({ bottom: 8 })

          TextArea({
            text: this.editingGroupDescription,
            placeholder: '请输入分组描述'
          })
            .width('100%')
            .height(80)
            .fontSize(14)
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.editingGroupDescription = value;
            })
        }
        .width('100%')
        .padding(16)
        .backgroundColor(Color.White)
        .borderRadius(12)
        .margin({ top: 12 })

        // 分组颜色
        Column() {
          Text('分组颜色')
            .fontSize(14)
            .fontColor('#666')
            .width('100%')
            .margin({ bottom: 12 })

          Flex({ wrap: FlexWrap.Wrap }) {
            ForEach(GROUP_COLORS, (color: string) => {
              Stack() {
                Circle()
                  .width(36)
                  .height(36)
                  .fill(color)

                if (this.editingGroupColor === color) {
                  Text('✓')
                    .fontSize(18)
                    .fontColor(Color.White)
                }
              }
              .width(44)
              .height(44)
              .margin(4)
              .onClick(() => {
                this.editingGroupColor = color;
              })
            }, (color: string) => color)
          }
          .width('100%')
        }
        .width('100%')
        .padding(16)
        .backgroundColor(Color.White)
        .borderRadius(12)
        .margin({ top: 12 })

        // 分组图标
        Column() {
          Text('分组图标')
            .fontSize(14)
            .fontColor('#666')
            .width('100%')
            .margin({ bottom: 12 })

          Flex({ wrap: FlexWrap.Wrap }) {
            ForEach(GROUP_ICONS, (icon: string) => {
              Text(this.getGroupIcon(icon))
                .fontSize(24)
                .textAlign(TextAlign.Center)
                .width(44)
                .height(44)
                .borderRadius(8)
                .margin(4)
                .backgroundColor(this.editingGroupIcon === icon ? '#007DFF20' : '#f5f5f5')
                .border({
                  width: this.editingGroupIcon === icon ? 2 : 0,
                  color: '#007DFF'
                })
                .onClick(() => {
                  this.editingGroupIcon = icon;
                })
            }, (icon: string) => icon)
          }
          .width('100%')
        }
        .width('100%')
        .padding(16)
        .backgroundColor(Color.White)
        .borderRadius(12)
        .margin({ top: 12, bottom: 16 })
      }
      .width('100%')
      .padding({ left: 16, right: 16 })
    }
    .layoutWeight(1)
  }

  /**
   * 联系人选择视图
   */
  @Builder
  ContactSelectView() {
    Column() {
      // 搜索框
      TextInput({ placeholder: '搜索联系人' })
        .width('100%')
        .height(40)
        .margin({ left: 16, right: 16, top: 12 })
        .onChange((value: string) => {
          this.searchText = value;
          this.searchContacts();
        })

      // 已选数量
      Text(`已选择 ${this.selectedContactKeys.size} 位联系人`)
        .fontSize(14)
        .fontColor('#666')
        .width('100%')
        .padding({ left: 16, top: 12, bottom: 8 })

      // 联系人列表
      List({ space: 6 }) {
        ForEach(this.filteredContacts, (contactItem: contact.Contact) => {
          ListItem() {
            this.SelectableContactCard(contactItem)
          }
        }, (item: contact.Contact) => item.key || '')
      }
      .width('100%')
      .layoutWeight(1)
      .padding({ left: 16, right: 16 })
    }
    .width('100%')
    .layoutWeight(1)
  }

  /**
   * 可选联系人卡片
   */
  @Builder
  SelectableContactCard(contactItem: contact.Contact) {
    Row() {
      // 复选框
      Checkbox()
        .select(this.selectedContactKeys.has(contactItem.key || ''))
        .onChange((isSelected: boolean) => {
          if (contactItem.key) {
            this.toggleContactSelection(contactItem.key);
          }
        })

      // 头像
      Text(contactItem.name?.fullName?.charAt(0) || '?')
        .fontSize(14)
        .fontColor(Color.White)
        .textAlign(TextAlign.Center)
        .width(36)
        .height(36)
        .borderRadius(18)
        .backgroundColor('#007DFF')
        .margin({ left: 12 })

      Column() {
        Text(contactItem.name?.fullName || '未知')
          .fontSize(15)

        if (contactItem.phoneNumbers && contactItem.phoneNumbers.length > 0) {
          Text(contactItem.phoneNumbers[0].phoneNumber || '')
            .fontSize(12)
            .fontColor('#666')
            .margin({ top: 2 })
        }
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
      .margin({ left: 10 })
    }
    .width('100%')
    .padding(10)
    .backgroundColor(Color.White)
    .borderRadius(8)
    .onClick(() => {
      if (contactItem.key) {
        this.toggleContactSelection(contactItem.key);
      }
    })
  }

  /**
   * 智能分组视图
   */
  @Builder
  SmartGroupView() {
    Column() {
      Text('选择分组规则')
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .width('100%')
        .padding({ left: 16, top: 16, bottom: 12 })

      ForEach(this.smartGroupOptions, (item: SmartGroupOptionItem) => {
        this.SmartGroupOption(item.type, item.label, item.desc || '')
      }, (item: SmartGroupOptionItem) => item.type)

      Blank()

      Button('生成智能分组')
        .width('90%')
        .height(48)
        .backgroundColor('#007DFF')
        .margin({ bottom: 20 })
        .onClick(() => this.generateSmartGroups())
    }
    .width('100%')
    .layoutWeight(1)
  }

  /**
   * 智能分组选项
   */
  @Builder
  SmartGroupOption(type: SmartGroupType, label: string, desc: string) {
    Row() {
      Radio({ value: type, group: 'smartGroup' })
        .checked(this.selectedSmartGroupType === type)
        .onChange((isChecked: boolean) => {
          if (isChecked) {
            this.selectedSmartGroupType = type;
          }
        })

      Column() {
        Text(label)
          .fontSize(15)
          .fontWeight(FontWeight.Medium)

        Text(desc)
          .fontSize(12)
          .fontColor('#666')
          .margin({ top: 2 })
      }
      .alignItems(HorizontalAlign.Start)
      .margin({ left: 12 })
      .layoutWeight(1)
    }
    .width('100%')
    .padding(16)
    .margin({ left: 16, right: 16, bottom: 8 })
    .backgroundColor(Color.White)
    .borderRadius(10)
    .onClick(() => {
      this.selectedSmartGroupType = type;
    })
  }

  /**
   * 删除确认弹窗
   */
  @Builder
  DeleteConfirmDialog() {
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
        Text('确认删除')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 12 })

        Text(`确定要删除分组"${this.selectedGroup?.name}"吗？`)
          .fontSize(14)
          .textAlign(TextAlign.Center)

        Text('分组内的联系人不会被删除')
          .fontSize(12)
          .fontColor('#666')
          .margin({ top: 8 })

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
            .backgroundColor('#ff3b30')
            .margin({ left: 12 })
            .onClick(() => this.deleteGroup())
        }
        .width('100%')
        .margin({ top: 20 })
      }
      .width('80%')
      .padding(20)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
  }

  /**
   * 智能分组弹窗
   */
  @Builder
  SmartGroupDialog() {
    Column() {
      Blank()
        .width('100%')
        .height('100%')
        .backgroundColor('#000000')
        .opacity(0.5)
        .onClick(() => {
          this.showSmartGroupDialog = false;
        })

      Column() {
        Text('智能分组')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 16 })

        ForEach(this.smartGroupDialogOptions, (item: SmartGroupOptionItem) => {
          Row() {
            Radio({ value: item.type, group: 'smartGroupDialog' })
              .checked(this.selectedSmartGroupType === item.type)
              .onChange((isChecked: boolean) => {
                if (isChecked) {
                  this.selectedSmartGroupType = item.type;
                }
              })

            Text(item.label)
              .fontSize(14)
              .margin({ left: 8 })
          }
          .width('100%')
          .padding(8)
        }, (item: SmartGroupOptionItem) => item.type)

        Row() {
          Button('取消')
            .layoutWeight(1)
            .height(40)
            .backgroundColor('#f0f0f0')
            .fontColor('#333')
            .onClick(() => {
              this.showSmartGroupDialog = false;
            })

          Button('生成')
            .layoutWeight(1)
            .height(40)
            .backgroundColor('#007DFF')
            .margin({ left: 12 })
            .onClick(() => this.generateSmartGroups())
        }
        .width('100%')
        .margin({ top: 16 })
      }
      .width('80%')
      .padding(20)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
  }

  /**
   * 加载遮罩
   */
  @Builder
  LoadingOverlay() {
    Column() {
      LoadingProgress()
        .width(50)
        .height(50)

      Text('加载中...')
        .fontSize(16)
        .fontColor(Color.White)
        .margin({ top: 12 })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .backgroundColor('rgba(0, 0, 0, 0.6)')
    .position({ x: 0, y: 0 })
  }

  /**
   * 保存确认弹窗
   */
  @Builder
  SaveConfirmDialog() {
    Column() {
      Column() {
        Text(this.isEditMode ? '确认保存修改' : '确认创建分组')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 16 })

        Text(this.isEditMode ?
          `确定要保存对分组 "${this.editingGroupName}" 的修改吗？` :
          `确定要创建分组 "${this.editingGroupName}" 吗？`)
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Center)
          .margin({ bottom: 24 })

        Row() {
          Button('取消')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#f0f0f0')
            .fontColor('#333')
            .onClick(() => {
              this.showSaveConfirm = false;
            })

          Button('确认')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#007DFF')
            .fontColor(Color.White)
            .margin({ left: 12 })
            .onClick(() => {
              this.showSaveConfirm = false;
              if (this.isEditMode) {
                this.updateGroup();
              } else {
                this.createGroup();
              }
            })
        }
        .width('100%')
      }
      .width('80%')
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(16)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
  }

  /**
   * 完成确认弹窗（选择联系人后）
   */
  @Builder
  CompleteConfirmDialog() {
    Column() {
      Column() {
        Text('确认完成选择')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 16 })

        Text(`已选择 ${this.selectedContactKeys.size} 位联系人，确定要完成吗？`)
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Center)
          .margin({ bottom: 24 })

        Row() {
          Button('取消')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#f0f0f0')
            .fontColor('#333')
            .onClick(() => {
              this.showCompleteConfirm = false;
            })

          Button('确认完成')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#007DFF')
            .fontColor(Color.White)
            .margin({ left: 12 })
            .onClick(() => {
              this.showCompleteConfirm = false;
              this.saveContactSelection();
            })
        }
        .width('100%')
      }
      .width('80%')
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(16)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
  }
}
