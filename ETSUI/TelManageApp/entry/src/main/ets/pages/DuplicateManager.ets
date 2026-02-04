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
 * 联系人去重管理页面
 * 提供重复联系人检测、预览、合并、忽略等完整功能
 */

import { contact } from '@kit.ContactsKit';
import { promptAction, router } from '@kit.ArkUI';
import { abilityAccessCtrl, Context } from '@kit.AbilityKit';
import { ContactData } from '../model/ContactData';
import { DuplicateService } from '../model/DuplicateDetector';
import { DuplicateGroupModel, DuplicateStatistics, MergePreview } from '../model/MergeResult';
import {
  DEFAULT_DUPLICATE_STRATEGY_CONFIG,
  DuplicateStrategyConfig,
  DuplicateStrategyPresets,
  DuplicateStrategyType,
  DuplicateStrictLevel,
  STRATEGY_TYPE_DESCRIPTIONS,
  STRICT_LEVEL_DESCRIPTIONS
} from '../model/DuplicateStrategy';

/**
 * 辅助函数：复制配置对象（避免使用展开运算符）
 */
function copyStrategyConfig(config: DuplicateStrategyConfig): DuplicateStrategyConfig {
  const copied: DuplicateStrategyConfig = {
    strategyType: config.strategyType,
    strictLevel: config.strictLevel,
    phoneSimThreshold: config.phoneSimThreshold,
    nameSimThreshold: config.nameSimThreshold,
    emailSimThreshold: config.emailSimThreshold,
    overallSimThreshold: config.overallSimThreshold,
    fieldWeights: {
      phoneWeight: config.fieldWeights.phoneWeight,
      nameWeight: config.fieldWeights.nameWeight,
      emailWeight: config.fieldWeights.emailWeight,
      organizationWeight: config.fieldWeights.organizationWeight
    },
    conflictResolution: config.conflictResolution,
    ignorePhoneFormat: config.ignorePhoneFormat,
    ignoreCaseInName: config.ignoreCaseInName,
    ignoreWhitespace: config.ignoreWhitespace,
    enablePinyinMatch: config.enablePinyinMatch
  };
  return copied;
}

/**
 * 扫描状态枚举
 */
enum ScanState {
  IDLE = 'IDLE',
  SCANNING = 'SCANNING',
  COMPLETED = 'COMPLETED',
  ERROR = 'ERROR'
}

/**
 * 视图模式枚举
 */
enum ViewMode {
  LIST = 'LIST', // 列表视图
  DETAIL = 'DETAIL', // 详情视图
  SETTINGS = 'SETTINGS' // 设置视图
}

@Entry
@Component
struct DuplicateManager {
  // 上下文与数据层
  @Provide('context') context: Context = getContext(this);
  private contactData: ContactData = new ContactData(this.context);
  private duplicateService: DuplicateService = DuplicateService.getInstance();
  // 页面状态
  @State scanState: ScanState = ScanState.IDLE;
  @State viewMode: ViewMode = ViewMode.LIST;
  @State isLoading: boolean = false;
  @State errorMessage: string = '';
  // 数据状态
  @State allContacts: contact.Contact[] = [];
  @State duplicateGroups: DuplicateGroupModel[] = [];
  @State selectedGroup: DuplicateGroupModel | null = null;
  @State mergePreview: MergePreview | null = null;
  @State statistics: DuplicateStatistics | null = null;
  // 配置状态
  @State currentConfig: DuplicateStrategyConfig = copyStrategyConfig(DEFAULT_DUPLICATE_STRATEGY_CONFIG);
  @State selectedStrategyType: DuplicateStrategyType = DuplicateStrategyType.SMART_DETECT;
  @State selectedStrictLevel: DuplicateStrictLevel = DuplicateStrictLevel.NORMAL;
  // 批量操作状态
  @State showBatchConfirm: boolean = false;
  @State batchProcessing: boolean = false;
  @State processedCount: number = 0;
  // 详情页状态
  @State showMergeConfirm: boolean = false;
  @State showIgnoreConfirm: boolean = false;

  aboutToAppear() {
    console.log('DuplicateManager: 页面即将显示');
    this.duplicateService.setContactData(this.contactData);
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
        console.log('DuplicateManager: 权限已获取');
        await this.loadContacts();
      } else {
        this.errorMessage = '需要通讯录权限才能使用去重功能';
        this.scanState = ScanState.ERROR;
      }
    } catch (err) {
      console.error('DuplicateManager: 权限请求失败', err);
      this.errorMessage = '权限请求失败';
      this.scanState = ScanState.ERROR;
    }
  }

  /**
   * 加载联系人
   */
  async loadContacts() {
    this.isLoading = true;
    try {
      this.allContacts = await this.contactData.queryAllContacts();
      console.log(`DuplicateManager: 已加载 ${this.allContacts.length} 个联系人`);
    } catch (error) {
      console.error('DuplicateManager: 加载联系人失败', error);
      this.errorMessage = '加载联系人失败';
    } finally {
      this.isLoading = false;
    }
  }

  /**
   * 执行扫描
   */
  async startScan() {
    if (this.allContacts.length === 0) {
      promptAction.showToast({ message: '没有联系人可扫描' });
      return;
    }

    this.scanState = ScanState.SCANNING;
    this.errorMessage = '';

    try {
      // 更新配置
      this.duplicateService.updateConfig(this.currentConfig);

      // 执行扫描
      this.duplicateGroups = await this.duplicateService.scan(this.allContacts, true);

      // 计算统计信息
      this.statistics = this.duplicateService.getDetector()
        .getStatistics(this.allContacts, this.duplicateGroups);

      this.scanState = ScanState.COMPLETED;
      console.log(`DuplicateManager: 扫描完成，发现 ${this.duplicateGroups.length} 组重复`);

      if (this.duplicateGroups.length === 0) {
        promptAction.showToast({ message: '未发现重复联系人' });
      }
    } catch (error) {
      console.error('DuplicateManager: 扫描失败', error);
      this.errorMessage = '扫描过程中发生错误';
      this.scanState = ScanState.ERROR;
    }
  }

  /**
   * 查看重复组详情
   */
  viewGroupDetail(group: DuplicateGroupModel) {
    this.selectedGroup = group;
    try {
      this.mergePreview = this.duplicateService.getDetector().generateMergePreview(group);
    } catch (error) {
      console.error('DuplicateManager: 生成预览失败', error);
    }
    this.viewMode = ViewMode.DETAIL;
  }

  /**
   * 合并选中的组
   */
  async mergeSelectedGroup() {
    if (!this.selectedGroup) {
      return;
    }

    this.isLoading = true;
    try {
      const success = await this.duplicateService.mergeGroup(this.selectedGroup);
      if (success) {
        promptAction.showToast({ message: '合并成功' });
        this.duplicateGroups = this.duplicateService.getCachedGroups();
        this.viewMode = ViewMode.LIST;
        this.selectedGroup = null;
        this.mergePreview = null;

        // 刷新联系人列表
        await this.loadContacts();
      } else {
        promptAction.showToast({ message: '合并失败' });
      }
    } catch (error) {
      console.error('DuplicateManager: 合并失败', error);
      promptAction.showToast({ message: '合并过程中发生错误' });
    } finally {
      this.isLoading = false;
      this.showMergeConfirm = false;
    }
  }

  /**
   * 忽略选中的组
   */
  ignoreSelectedGroup() {
    if (!this.selectedGroup) {
      return;
    }

    this.duplicateService.ignoreGroup(this.selectedGroup);
    this.duplicateGroups = this.duplicateService.getCachedGroups();
    promptAction.showToast({ message: '已忽略该组' });
    this.viewMode = ViewMode.LIST;
    this.selectedGroup = null;
    this.mergePreview = null;
    this.showIgnoreConfirm = false;
  }

  /**
   * 批量合并所有
   */
  async batchMergeAll() {
    this.batchProcessing = true;
    this.processedCount = 0;

    try {
      const result = await this.duplicateService.mergeAll();

      if (result.success) {
        promptAction.showToast({
          message: `成功合并 ${result.mergedContacts} 个联系人`
        });
      } else {
        promptAction.showToast({
          message: `部分合并失败: ${result.errors.join(', ')}`
        });
      }

      this.duplicateGroups = this.duplicateService.getCachedGroups();
      await this.loadContacts();
    } catch (error) {
      console.error('DuplicateManager: 批量合并失败', error);
      promptAction.showToast({ message: '批量合并失败' });
    } finally {
      this.batchProcessing = false;
      this.showBatchConfirm = false;
    }
  }

  /**
   * 应用预设配置
   */
  applyPreset(preset: 'strict' | 'normal' | 'loose' | 'phoneOnly') {
    switch (preset) {
      case 'strict':
        this.currentConfig = DuplicateStrategyPresets.getStrictConfig();
        break;
      case 'loose':
        this.currentConfig = DuplicateStrategyPresets.getLooseConfig();
        break;
      case 'phoneOnly':
        this.currentConfig = DuplicateStrategyPresets.getPhoneOnlyConfig();
        break;
      case 'normal':
      default:
        this.currentConfig = copyStrategyConfig(DEFAULT_DUPLICATE_STRATEGY_CONFIG);
    }

    this.selectedStrategyType = this.currentConfig.strategyType;
    this.selectedStrictLevel = this.currentConfig.strictLevel;

    promptAction.showToast({ message: '配置已更新' });
  }

  build() {
    Stack() {
      // 主内容
      Column() {
        // 标题栏
        this.TitleBar()

        // 根据视图模式显示不同内容
        if (this.viewMode === ViewMode.LIST) {
          this.ListView()
        } else if (this.viewMode === ViewMode.DETAIL) {
          this.DetailView()
        } else if (this.viewMode === ViewMode.SETTINGS) {
          this.SettingsView()
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#f5f5f5')

      // 合并确认弹窗
      if (this.showMergeConfirm) {
        this.MergeConfirmDialog()
      }

      // 忽略确认弹窗
      if (this.showIgnoreConfirm) {
        this.IgnoreConfirmDialog()
      }

      // 批量确认弹窗
      if (this.showBatchConfirm) {
        this.BatchConfirmDialog()
      }

      // 加载遮罩
      if (this.isLoading || this.batchProcessing) {
        this.LoadingOverlay()
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
      Button('返回')
        .fontSize(16)
        .backgroundColor(Color.Transparent)
        .fontColor('#007DFF')
        .onClick(() => {
          if (this.viewMode !== ViewMode.LIST) {
            this.viewMode = ViewMode.LIST;
            this.selectedGroup = null;
            this.mergePreview = null;
          } else {
            router.back();
          }
        })

      Text(this.getTitleText())
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Center)
        .layoutWeight(1)

      // 右侧操作按钮
      if (this.viewMode === ViewMode.LIST) {
        Button('设置')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => {
            this.viewMode = ViewMode.SETTINGS;
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
      case ViewMode.DETAIL:
        return '重复详情';
      case ViewMode.SETTINGS:
        return '去重设置';
      default:
        return '联系人去重';
    }
  }

  /**
   * 列表视图
   */
  @Builder
  ListView() {
    Column() {
      // 统计信息卡片
      if (this.statistics) {
        this.StatisticsCard()
      }

      // 扫描按钮区域
      this.ScanSection()

      // 结果列表
      if (this.scanState === ScanState.COMPLETED && this.duplicateGroups.length > 0) {
        this.DuplicateGroupList()
      } else if (this.scanState === ScanState.IDLE) {
        this.IdleStateView()
      } else if (this.scanState === ScanState.ERROR) {
        this.ErrorStateView()
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
      Text('扫描结果')
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .width('100%')
        .margin({ bottom: 12 })

      Row() {
        this.StatItem('总联系人', this.statistics!.totalContacts.toString())
        this.StatItem('重复组', this.statistics!.duplicateGroups.toString())
        this.StatItem('重复数', this.statistics!.duplicateContacts.toString())
        this.StatItem('可节省', this.statistics!.potentialSavings.toString())
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
   * 扫描区域
   */
  @Builder
  ScanSection() {
    Column() {
      Row() {
        Button(this.scanState === ScanState.SCANNING ? '扫描中...' : '开始扫描')
          .width('45%')
          .height(44)
          .fontSize(16)
          .backgroundColor('#007DFF')
          .enabled(this.scanState !== ScanState.SCANNING)
          .onClick(() => this.startScan())

        if (this.duplicateGroups.length > 0) {
          Button('一键合并全部')
            .width('45%')
            .height(44)
            .fontSize(16)
            .backgroundColor('#FF6D00')
            .onClick(() => {
              this.showBatchConfirm = true;
            })
        }
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceAround)

      // 扫描进度
      if (this.scanState === ScanState.SCANNING) {
        Column() {
          LoadingProgress()
            .width(40)
            .height(40)
          Text('正在分析联系人...')
            .fontSize(14)
            .fontColor('#666')
            .margin({ top: 8 })
        }
        .width('100%')
        .padding(20)
      }
    }
    .width('100%')
    .padding(16)
  }

  /**
   * 重复组列表
   */
  @Builder
  DuplicateGroupList() {
    Column() {
      Text(`发现 ${this.duplicateGroups.length} 组重复联系人`)
        .fontSize(14)
        .fontColor('#666')
        .width('100%')
        .padding({ left: 16, bottom: 8 })

      List({ space: 12 }) {
        ForEach(this.duplicateGroups, (group: DuplicateGroupModel) => {
          ListItem() {
            this.DuplicateGroupCard(group)
          }
        }, (group: DuplicateGroupModel) => group.groupId)
      }
      .width('100%')
      .layoutWeight(1)
      .padding({ left: 16, right: 16, bottom: 80 })
    }
    .width('100%')
    .layoutWeight(1)
  }

  /**
   * 重复组卡片
   */
  @Builder
  DuplicateGroupCard(group: DuplicateGroupModel) {
    Column() {
      Row() {
        // 联系人头像堆叠
        Stack() {
          ForEach(group.contacts.slice(0, 3), (contactItem: contact.Contact, index: number) => {
            Text(contactItem.name?.fullName?.charAt(0) || '?')
              .fontSize(14)
              .fontColor(Color.White)
              .textAlign(TextAlign.Center)
              .width(36)
              .height(36)
              .borderRadius(18)
              .backgroundColor(this.getAvatarColor(index))
              .border({ width: 2, color: Color.White })
              .position({ x: index * 20, y: 0 })
          }, (item: contact.Contact, index: number) => item.key || index.toString())
        }
        .width(80)
        .height(40)

        Column() {
          // 显示组内联系人名称
          Text(this.getGroupDisplayName(group))
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
            .width('100%')

          Row() {
            Text(`${group.getContactCount()} 个重复`)
              .fontSize(12)
              .fontColor('#666')

            Text(`相似度: ${(group.averageSimilarity * 100).toFixed(0)}%`)
              .fontSize(12)
              .fontColor('#007DFF')
              .margin({ left: 12 })
          }
          .margin({ top: 4 })
        }
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)
        .margin({ left: 12 })

        // 查看详情
        Text('查看')
          .fontSize(14)
          .fontColor('#007DFF')
          .onClick(() => this.viewGroupDetail(group))
      }
      .width('100%')
      .padding(16)

      // 快捷操作 - 高度调整避免误触
      Row() {
        Button('合并')
          .fontSize(12)
          .height(36)
          .layoutWeight(1)
          .backgroundColor('#007DFF')
          .onClick(() => {
            this.selectedGroup = group;
            this.mergePreview = this.duplicateService.getDetector().generateMergePreview(group);
            this.showMergeConfirm = true;
          })

        Button('忽略')
          .fontSize(12)
          .height(36)
          .layoutWeight(1)
          .backgroundColor('#999')
          .margin({ left: 12 })
          .onClick(() => {
            this.selectedGroup = group;
            this.showIgnoreConfirm = true;
          })
      }
      .width('100%')
      .padding({
        left: 16,
        right: 16,
        bottom: 16,
        top: 8
      })
    }
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({ radius: 2, color: '#10000000', offsetY: 1 })
  }

  /**
   * 获取组显示名称
   */
  getGroupDisplayName(group: DuplicateGroupModel): string {
    return group.contacts
      .map(c => c.name?.fullName || '未知')
      .slice(0, 3)
      .join('、') + (group.contacts.length > 3 ? ' 等' : '');
  }

  /**
   * 获取头像颜色
   */
  getAvatarColor(index: number): string {
    const colors = ['#007DFF', '#00C853', '#FF6D00', '#D500F9', '#FF1744'];
    return colors[index % colors.length];
  }

  /**
   * 空状态视图
   */
  @Builder
  IdleStateView() {
    Column() {
      Text('🔍')
        .fontSize(60)
        .margin({ bottom: 16 })

      Text('点击"开始扫描"检测重复联系人')
        .fontSize(16)
        .fontColor('#666')

      Text(`当前共有 ${this.allContacts.length} 个联系人`)
        .fontSize(14)
        .fontColor('#999')
        .margin({ top: 8 })
    }
    .width('100%')
    .layoutWeight(1)
    .justifyContent(FlexAlign.Center)
  }

  /**
   * 错误状态视图
   */
  @Builder
  ErrorStateView() {
    Column() {
      Text('❌')
        .fontSize(60)
        .margin({ bottom: 16 })

      Text(this.errorMessage)
        .fontSize(16)
        .fontColor('#ff3b30')

      Button('重试')
        .margin({ top: 16 })
        .onClick(() => this.checkPermissionsAndLoad())
    }
    .width('100%')
    .layoutWeight(1)
    .justifyContent(FlexAlign.Center)
  }

  /**
   * 详情视图
   */
  @Builder
  DetailView() {
    if (!this.selectedGroup) {
      Text('请选择一个重复组')
        .fontSize(16)
        .fontColor('#666')
    } else {
      Scroll() {
        Column() {
          // 组内联系人列表
          Text('重复的联系人')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .padding({ left: 16, top: 16, bottom: 8 })

          ForEach(this.selectedGroup.contacts, (contactItem: contact.Contact) => {
            this.ContactDetailCard(contactItem,
              contactItem.key === this.selectedGroup?.primaryContact?.key)
          }, (item: contact.Contact) => item.key || '')

          // 合并预览
          if (this.mergePreview) {
            this.MergePreviewSection()
          }

          // 操作按钮 - 添加底部边距避免与Home键重合
          Row() {
            Button('合并联系人')
              .layoutWeight(1)
              .height(48)
              .backgroundColor('#007DFF')
              .onClick(() => {
                this.showMergeConfirm = true;
              })

            Button('忽略此组')
              .layoutWeight(1)
              .height(48)
              .backgroundColor('#999')
              .margin({ left: 16 })
              .onClick(() => {
                this.showIgnoreConfirm = true;
              })
          }
          .width('100%')
          .padding(16)
          .margin({ bottom: 80 })
        }
      }
      .layoutWeight(1)
    }
  }

  /**
   * 联系人详情卡片
   */
  @Builder
  ContactDetailCard(contactItem: contact.Contact, isPrimary: boolean) {
    Column() {
      Row() {
        // 头像
        Text(contactItem.name?.fullName?.charAt(0) || '?')
          .fontSize(18)
          .fontColor(Color.White)
          .textAlign(TextAlign.Center)
          .width(44)
          .height(44)
          .borderRadius(22)
          .backgroundColor(isPrimary ? '#007DFF' : '#999')

        Column() {
          Row() {
            Text(contactItem.name?.fullName || '未知')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)

            if (isPrimary) {
              Text('推荐保留')
                .fontSize(10)
                .fontColor(Color.White)
                .backgroundColor('#00C853')
                .padding({
                  left: 6,
                  right: 6,
                  top: 2,
                  bottom: 2
                })
                .borderRadius(4)
                .margin({ left: 8 })
            }
          }

          if (contactItem.phoneNumbers && contactItem.phoneNumbers.length > 0) {
            Text(contactItem.phoneNumbers[0].phoneNumber || '')
              .fontSize(14)
              .fontColor('#666')
              .margin({ top: 4 })
          }

          if (contactItem.emails && contactItem.emails.length > 0) {
            Text(contactItem.emails[0].email || '')
              .fontSize(12)
              .fontColor('#999')
              .margin({ top: 2 })
          }
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)
        .margin({ left: 12 })
      }
      .width('100%')
    }
    .width('100%')
    .padding(16)
    .margin({ left: 16, right: 16, bottom: 8 })
    .backgroundColor(Color.White)
    .borderRadius(8)
    .border({ width: isPrimary ? 2 : 0, color: '#007DFF' })
  }

  /**
   * 合并预览区域
   */
  @Builder
  MergePreviewSection() {
    Column() {
      Text('合并后的联系人')
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .width('100%')
        .margin({ bottom: 12 })

      // 合并后的信息
      Column() {
        this.PreviewRow('姓名', this.mergePreview!.mergedContact.name?.fullName || '')

        ForEach(this.mergePreview!.mergedContact.phoneNumbers || [],
          (phone: contact.PhoneNumber) => {
            this.PreviewRow('电话', phone.phoneNumber || '')
          }, (phone: contact.PhoneNumber, index: number) => index.toString())

        ForEach(this.mergePreview!.mergedContact.emails || [],
          (email: contact.Email) => {
            this.PreviewRow('邮箱', email.email || '')
          }, (email: contact.Email, index: number) => index.toString())
      }
      .width('100%')
      .padding(12)
      .backgroundColor('#f8f8f8')
      .borderRadius(8)

      // 警告信息
      if (this.mergePreview!.warnings.length > 0) {
        Column() {
          ForEach(this.mergePreview!.warnings, (warning: string) => {
            Text(`⚠️ ${warning}`)
              .fontSize(12)
              .fontColor('#FF6D00')
              .width('100%')
              .margin({ top: 4 })
          }, (warning: string, index: number) => index.toString())
        }
        .margin({ top: 8 })
      }
    }
    .width('100%')
    .padding(16)
    .margin({ left: 16, right: 16, top: 8 })
    .backgroundColor(Color.White)
    .borderRadius(8)
  }

  /**
   * 预览行
   */
  @Builder
  PreviewRow(label: string, value: string) {
    Row() {
      Text(label)
        .fontSize(14)
        .fontColor('#666')
        .width(60)

      Text(value)
        .fontSize(14)
        .fontWeight(FontWeight.Medium)
        .layoutWeight(1)
    }
    .width('100%')
    .padding({ top: 6, bottom: 6 })
  }

  /**
   * 设置视图
   */
  @Builder
  SettingsView() {
    Scroll() {
      Column() {
        // 预设配置
        Text('快捷配置')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .width('100%')
          .padding({ left: 16, top: 16, bottom: 8 })

        Row() {
          this.PresetButton('普通', 'normal')
          this.PresetButton('严格', 'strict')
          this.PresetButton('宽松', 'loose')
          this.PresetButton('仅电话', 'phoneOnly')
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceAround)
        .padding({ left: 16, right: 16 })

        // 策略类型
        Column() {
          Text('检测策略')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .margin({ bottom: 12 })

          ForEach(Array.from(STRATEGY_TYPE_DESCRIPTIONS.entries()),
            (entry: [DuplicateStrategyType, string]) => {
              this.StrategyOption(entry[0], entry[1])
            }, (entry: [DuplicateStrategyType, string]) => entry[0])
        }
        .width('100%')
        .padding(16)
        .margin({ left: 16, right: 16, top: 16 })
        .backgroundColor(Color.White)
        .borderRadius(12)

        // 严格程度
        Column() {
          Text('检测严格度')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .margin({ bottom: 12 })

          ForEach(Array.from(STRICT_LEVEL_DESCRIPTIONS.entries()),
            (entry: [DuplicateStrictLevel, string]) => {
              this.StrictLevelOption(entry[0], entry[1])
            }, (entry: [DuplicateStrictLevel, string]) => entry[0])
        }
        .width('100%')
        .padding(16)
        .margin({ left: 16, right: 16, top: 16 })
        .backgroundColor(Color.White)
        .borderRadius(12)

        // 相似度阈值
        Column() {
          Text('相似度阈值')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .margin({ bottom: 12 })

          this.ThresholdSlider('综合相似度', this.currentConfig.overallSimThreshold,
            (value: number) => {
              this.currentConfig.overallSimThreshold = value;
            })

          this.ThresholdSlider('电话相似度', this.currentConfig.phoneSimThreshold,
            (value: number) => {
              this.currentConfig.phoneSimThreshold = value;
            })

          this.ThresholdSlider('姓名相似度', this.currentConfig.nameSimThreshold,
            (value: number) => {
              this.currentConfig.nameSimThreshold = value;
            })
        }
        .width('100%')
        .padding(16)
        .margin({
          left: 16,
          right: 16,
          top: 16,
          bottom: 16
        })
        .backgroundColor(Color.White)
        .borderRadius(12)
      }
    }
    .layoutWeight(1)
  }

  /**
   * 预设按钮
   */
  @Builder
  PresetButton(label: string, preset: 'strict' | 'normal' | 'loose' | 'phoneOnly') {
    Button(label)
      .fontSize(14)
      .height(36)
      .width(70)
      .backgroundColor('#007DFF')
      .onClick(() => this.applyPreset(preset))
  }

  /**
   * 策略选项
   */
  @Builder
  StrategyOption(type: DuplicateStrategyType, description: string) {
    Row() {
      Radio({ value: type, group: 'strategy' })
        .checked(this.selectedStrategyType === type)
        .onChange((isChecked: boolean) => {
          if (isChecked) {
            this.selectedStrategyType = type;
            this.currentConfig.strategyType = type;
          }
        })

      Text(description)
        .fontSize(14)
        .margin({ left: 8 })
        .layoutWeight(1)
    }
    .width('100%')
    .padding({ top: 8, bottom: 8 })
  }

  /**
   * 严格程度选项
   */
  @Builder
  StrictLevelOption(level: DuplicateStrictLevel, description: string) {
    Row() {
      Radio({ value: level, group: 'strictLevel' })
        .checked(this.selectedStrictLevel === level)
        .onChange((isChecked: boolean) => {
          if (isChecked) {
            this.selectedStrictLevel = level;
            this.currentConfig.strictLevel = level;
          }
        })

      Text(description)
        .fontSize(14)
        .margin({ left: 8 })
        .layoutWeight(1)
    }
    .width('100%')
    .padding({ top: 8, bottom: 8 })
  }

  /**
   * 阈值滑块
   */
  @Builder
  ThresholdSlider(label: string, value: number, onChange: (value: number) => void) {
    Column() {
      Row() {
        Text(label)
          .fontSize(14)

        Text(`${(value * 100).toFixed(0)}%`)
          .fontSize(14)
          .fontColor('#007DFF')
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)

      Slider({
        value: value * 100,
        min: 0,
        max: 100,
        step: 5
      })
        .width('100%')
        .onChange((newValue: number) => {
          onChange(newValue / 100);
        })
    }
    .width('100%')
    .margin({ top: 12 })
  }

  /**
   * 合并确认弹窗
   */
  @Builder
  MergeConfirmDialog() {
    Column() {
      // 上部空白区域，点击可关闭
      Blank()
        .width('100%')
        .layoutWeight(1)
        .backgroundColor('#000000')
        .opacity(0.5)
        .onClick(() => {
          this.showMergeConfirm = false;
        })

      // 弹窗内容 - 底部距离屏幕底部100vp
      Column() {
        Text('确认合并')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 12 })

        Text(`将合并 ${this.selectedGroup?.getContactCount() || 0} 个联系人为一个`)
          .fontSize(14)
          .textAlign(TextAlign.Center)

        Text('此操作会删除重复的联系人，保留信息最完整的版本')
          .fontSize(12)
          .fontColor('#666')
          .textAlign(TextAlign.Center)
          .margin({ top: 8 })

        Row() {
          Button('取消')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#f0f0f0')
            .fontColor('#333')
            .onClick(() => {
              this.showMergeConfirm = false;
            })

          Button('确认合并')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#007DFF')
            .margin({ left: 12 })
            .onClick(() => this.mergeSelectedGroup())
        }
        .width('100%')
        .margin({ top: 20 })
      }
      .width('85%')
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(16)
      .margin({ bottom: 120 })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.End)
    .alignItems(HorizontalAlign.Center)
    .position({ x: 0, y: 0 })
  }

  /**
   * 忽略确认弹窗
   */
  @Builder
  IgnoreConfirmDialog() {
    Column() {
      // 上部空白区域，点击可关闭
      Blank()
        .width('100%')
        .layoutWeight(1)
        .backgroundColor('#000000')
        .opacity(0.5)
        .onClick(() => {
          this.showIgnoreConfirm = false;
        })

      // 弹窗内容 - 底部距离屏幕底部120vp
      Column() {
        Text('确认忽略')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 12 })

        Text('忽略后，这组联系人将不再显示为重复')
          .fontSize(14)
          .textAlign(TextAlign.Center)

        Row() {
          Button('取消')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#f0f0f0')
            .fontColor('#333')
            .onClick(() => {
              this.showIgnoreConfirm = false;
            })

          Button('确认忽略')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#999')
            .margin({ left: 12 })
            .onClick(() => this.ignoreSelectedGroup())
        }
        .width('100%')
        .margin({ top: 20 })
      }
      .width('85%')
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(16)
      .margin({ bottom: 120 })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.End)
    .alignItems(HorizontalAlign.Center)
    .position({ x: 0, y: 0 })
  }

  /**
   * 批量确认弹窗
   */
  @Builder
  BatchConfirmDialog() {
    Column() {
      // 上部空白区域，点击可关闭
      Blank()
        .width('100%')
        .layoutWeight(1)
        .backgroundColor('#000000')
        .opacity(0.5)
        .onClick(() => {
          this.showBatchConfirm = false;
        })

      // 弹窗内容 - 底部距离屏幕底部120vp
      Column() {
        Text('⚠️ 批量合并')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 12 })

        Text(`即将合并 ${this.duplicateGroups.length} 组重复联系人`)
          .fontSize(14)
          .textAlign(TextAlign.Center)

        Text('此操作不可撤销，建议先备份联系人')
          .fontSize(12)
          .fontColor('#ff3b30')
          .textAlign(TextAlign.Center)
          .margin({ top: 8 })

        Row() {
          Button('取消')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#f0f0f0')
            .fontColor('#333')
            .onClick(() => {
              this.showBatchConfirm = false;
            })

          Button('确认全部合并')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#FF6D00')
            .margin({ left: 12 })
            .onClick(() => this.batchMergeAll())
        }
        .width('100%')
        .margin({ top: 20 })
      }
      .width('85%')
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(16)
      .margin({ bottom: 120 })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.End)
    .alignItems(HorizontalAlign.Center)
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

      Text(this.batchProcessing ? '正在处理...' : '加载中...')
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
}
