/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { DatabaseManager } from '../models/DatabaseManager';
import { Pet, PetRecord } from '../models/PetModel';
import reminderAgentManager from '@ohos.reminderAgentManager';
import notificationManager from '@ohos.notificationManager';
import promptAction from '@ohos.promptAction';

/**
 * 接口定义：宠物行为词条项的严格类型约束
 * 增加代码严谨性
 */
interface PetActionItem {
  label: string;
  icon: string;
  color: string;
}

/**
 * 系统常量：宠物 9 大核心行为词条配置
 * 集中管理以便于后期维护和扩展
 */
const PET_ACTIONS: PetActionItem[] = [
  { label: '喂食', icon: '🍖', color: '#FF9800' },
  { label: '遛弯', icon: '🦮', color: '#4CAF50' },
  { label: '排便', icon: '💩', color: '#795548' },
  { label: '疫苗', icon: '💉', color: '#F44336' },
  { label: '驱虫', icon: '💊', color: '#E91E63' },
  { label: '喂药', icon: '🏥', color: '#9C27B0' },
  { label: '洗澡', icon: '🛁', color: '#2196F3' },
  { label: '剪指甲', icon: '✂️', color: '#00BCD4' },
  { label: '称重', icon: '⚖️', color: '#607D8B' }
];

/**
 * 子组件：高级日程提醒弹窗
 * 采用独立结构体定义，增加项目代码层级感
 */
@CustomDialog
struct AdvancedReminderDialog {
  controller: CustomDialogController = new CustomDialogController({ builder: '' })
  @State reminderContent: string = ''
  @State selectedDate: Date = new Date()
  @State selectedTime: Date = new Date()
  /**
   * 确认操作的回调接口
   */
  confirm: (content: string, date: Date, time: Date) => void = () => {
  }

  build() {
    Column() {
      Text('设置计划提醒 ⏰')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 15, bottom: 15 })

      // 提醒内容输入区域
      TextInput({ placeholder: '在此输入提醒备注...', text: this.reminderContent })
        .onChange((value: string) => {
          this.reminderContent = value;
        })
        .width('90%')
        .height(45)
        .margin({ bottom: 12 })

      Text('第一步：选择日期')
        .fontSize(14)
        .fontColor('#666666')
        .width('90%')
        .margin({ bottom: 5 })

      DatePicker({ start: new Date(), end: new Date(2026, 11, 31), selected: this.selectedDate })
        .onChange((value: DatePickerResult) => {
          if (value.year !== undefined && value.month !== undefined && value.day !== undefined) {
            this.selectedDate.setFullYear(value.year, value.month, value.day);
          }
        })
        .height(100)
        .width('100%')

      Text('第二步：选择时间')
        .fontSize(14)
        .fontColor('#666666')
        .width('90%')
        .margin({ top: 10, bottom: 5 })

      TimePicker({ selected: this.selectedTime })
        .useMilitaryTime(true)
        .onChange((value: TimePickerResult) => {
          if (value.hour !== undefined && value.minute !== undefined) {
            this.selectedTime.setHours(value.hour, value.minute);
          }
        })
        .height(100)
        .width('100%')

      // 操作按钮布局
      Row({ space: 20 }) {
        Button('取消')
          .onClick(() => {
            this.controller.close();
          })
          .backgroundColor('#F5F5F5')
          .fontColor('#666666')
          .layoutWeight(1)

        Button('确认提醒')
          .onClick(() => {
            this.confirm(this.reminderContent, this.selectedDate, this.selectedTime);
            this.controller.close();
          })
          .backgroundColor(Color.Orange)
          .layoutWeight(1)
      }
      .width('90%')
      .margin({ top: 20, bottom: 15 })
    }
    .backgroundColor(Color.White)
    .padding(10)
    .borderRadius(20)
  }
}

/**
 * 主页面组件：日历与日常记录系统
 */
@Component
export struct CalendarPage {
  /**
   * 状态管理：当前日历选中的日期字符串 (格式: YYYY-MM-DD)
   */
  @State selectedDate: string = '';
  /**
   * 状态管理：从数据库加载的行为记录数据集
   */
  @State dayRecords: PetRecord[] = [];
  /**
   * 状态管理：当前用户选中的行为类型（如：喂食）
   */
  @State activeType: string = '喂食';
  /**
   * 状态管理：下方的历史记录筛选条件
   */
  @State filterType: string = '全部';
  /**
   * 状态管理：用户填写的实时备注文本
   */
  @State userRemark: string = '';
  /**
   * 状态管理：是否开启按日期筛选模式
   */
  @State isFilterByDay: boolean = false;
  // --- 【新增：多宠物管理相关状态】 ---
  @State allPets: Pet[] = []; // 存储所有宠物档案
  @State currentPetId: number = -1; // 当前记录选中的宠物ID
  @State currentPetName: string = '选择宠物'; // 当前显示的宠物名
  // --- 【新增：全局信号雷达】 ---
  @StorageLink('petDataChangeSignal') @Watch('onDataUpdate') dataSignal: number = 0;
  /**
   * 弹窗控制器：关联高级日程提醒设置弹窗
   */
  advancedReminderDialog: CustomDialogController = new CustomDialogController({
    builder: AdvancedReminderDialog({
      confirm: (content: string, date: Date, time: Date) => {
        this.publishCalendarReminder(content, date, time);
      }
    }),
    autoCancel: true,
    alignment: DialogAlignment.Center
  });

  /**
   * 【新增逻辑】：响应档案页的数据变更
   * 当收到信号弹时，主动下井抓取最新的宠物列表
   */
  onDataUpdate(): void {
    console.info('PetDiary: 记录页感应到数据变化，开始同步...');
    const db = DatabaseManager.getInstance();

    // 异步拉取最新宠物，并触发页面列表刷新
    db.getAllPets().then((latestPets: Pet[]) => {
      this.allPets = latestPets;
      this.refreshLogs(); // 刷新下方的历史词条
    });
  }

  /**
   * 生命周期钩子：组件即将出现时执行初始化加载
   */
  async aboutToAppear() {
    this.selectedDate = this.getTodayDate();

    // 【修改逻辑：初始化时加载宠物列表】
    const db = DatabaseManager.getInstance();
    this.allPets = await db.getAllPets();

    // 默认选中第一个宠物
    if (this.allPets.length > 0) {
      this.currentPetId = this.allPets[0].id;
      this.currentPetName = this.allPets[0].name;
    }

    await this.refreshLogs();
  }

  /**
   * 核心逻辑：从本地数据库刷新记录列表
   * 修改：增加按当前选中宠物过滤的逻辑
   */
  async refreshLogs() {
    try {
      let rawData: PetRecord[] = [];
      if (this.isFilterByDay) {
        rawData = await DatabaseManager.getInstance().getRecordsByDate(this.selectedDate);
      } else {
        rawData = await DatabaseManager.getInstance().getAllRecords();
      }

      // 【关键逻辑升级】：只筛选出属于当前选定宠物的记录
      if (this.currentPetId !== -1) {
        this.dayRecords = rawData.filter(item => item.petId === this.currentPetId);
      } else {
        this.dayRecords = rawData;
      }

      console.info(`PetDiary: 已加载宠物[${this.currentPetName}]的记录共 ${this.dayRecords.length} 条`);
    } catch (exception) {
      console.error("CalendarPage: Refresh Database Error -> " + JSON.stringify(exception));
    }
  }

  /**
   * UI构建模块：日历选择器部分
   */
  @Builder
  CalendarSection() {
    CalendarPicker({ selected: new Date() })
      .edgeAlign(CalendarAlign.CENTER)
      .margin({
        top: 15,
        bottom: 10,
        left: 15,
        right: 15
      })
      .padding(10)
      .backgroundColor(Color.White)
      .borderRadius(16)
      .onChange((value: Date) => {
        const y = value.getFullYear();
        const m = (value.getMonth() + 1).toString().padStart(2, '0');
        const d = value.getDate().toString().padStart(2, '0');

        this.selectedDate = `${y}-${m}-${d}`;
        this.isFilterByDay = true;
        this.refreshLogs();
      })
  }

  /**
   * UI构建模块：历史足迹标题与筛选控制区
   */
  @Builder
  ListHeaderSection() {
    Row() {
      Text(this.isFilterByDay ? `足迹 (${this.selectedDate})` : '历史足迹')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')

      if (this.isFilterByDay) {
        Text('查看全部')
          .fontSize(14)
          .fontColor('#007DFF')
          .margin({ left: 10 })
          .onClick(() => {
            this.isFilterByDay = false;
            this.refreshLogs();
          })
      }

      Blank()

      Select(this.getFilterOptions())
        .selected(0)
        .value(this.filterType)
        .font({ size: 14 })
        .onSelect((index: number, value: string) => {
          this.filterType = value;
        })
    }
    .width('90%')
    .margin({ top: 5, bottom: 10 })
  }

  /**
   * 主页面渲染构建函数
   */
  build() {
    Column() {
      // 1. 顶部日历
      this.CalendarSection()

      // --- 【新增：宠物切换选择器】 ---
      Row({ space: 10 }) {
        Text('正在记录:').fontSize(14).fontColor('#888')
        if (this.allPets.length > 0) {
          Select(this.allPets.map(p => ({ value: p.name } as SelectOption)))
            .value(this.currentPetName)
            .selected(this.allPets.findIndex(p => p.id === this.currentPetId))
            .font({ size: 14, weight: FontWeight.Medium })
            .onSelect((idx, val) => {
              this.currentPetId = this.allPets[idx].id;
              this.currentPetName = val;
              this.refreshLogs();
            })
        } else {
          Text('暂无宠物')
            .fontSize(14)
            .fontColor(Color.Red)
            .onClick(() => {
              promptAction.showToast({ message: '请先去宠物档案页添加宠物' })
            })
        }
      }
      .width('90%')
      .justifyContent(FlexAlign.End)
      .margin({ bottom: 5 })

      // 2. 快捷记录面板
      Column() {
        Text(`准备在 [${this.selectedDate}] 记一笔...`)
          .fontSize(14)
          .fontColor('#666666')
          .width('100%')
          .margin({ bottom: 12 })

        Flex({ wrap: FlexWrap.Wrap, justifyContent: FlexAlign.SpaceBetween }) {
          ForEach(PET_ACTIONS, (item: PetActionItem) => {
            Column() {
              Text(item.icon).fontSize(24)
              Text(item.label).fontSize(12).margin({ top: 4 })
            }
            .width('21%')
            .margin({ bottom: 10 })
            .padding(8)
            .borderRadius(12)
            .backgroundColor(this.activeType === item.label ? item.color : '#F7F7F7')
            .shadow({
              radius: this.activeType === item.label ? 10 : 0,
              color: this.activeType === item.label ? item.color + '44' : '#00000000'
            })
            .onClick(() => {
              this.activeType = item.label;
            })
          })
        }

        TextInput({ placeholder: `给 [${this.activeType}] 增加备注（可选）...`, text: this.userRemark })
          .onChange((value: string) => {
            this.userRemark = value;
          })
          .width('100%')
          .height(42)
          .backgroundColor('#F1F3F5')
          .borderRadius(8)
          .margin({ top: 8, bottom: 12 })

        Row({ space: 12 }) {
          Button(`保存到足迹`)
            .layoutWeight(1)
            .backgroundColor('#4CAF50')
            .height(45)
            .onClick(() => {
              this.handleSaveRecord();
            })

          Button('设为提醒')
            .layoutWeight(1)
            .backgroundColor(Color.Orange)
            .height(45)
            .onClick(() => {
              this.advancedReminderDialog.open();
            })
        }
        .width('100%')
      }
      .padding(15)
      .backgroundColor(Color.White)
      .borderRadius(16)
      .margin({ left: 15, right: 15, bottom: 15 })

      // 3. 历史足迹标题
      this.ListHeaderSection()

      // 4. 数据展示列表
      List({ space: 12 }) {
        ForEach(this.dayRecords, (item: PetRecord) => {
          if (this.filterType === '全部' || item.type === this.filterType) {
            ListItem() {
              Row() {
                Circle({ width: 12, height: 12 })
                  .fill(this.getIconColor(item.type))

                Column() {
                  Text(item.type).fontSize(16).fontWeight(FontWeight.Bold)
                  Text(item.content).fontSize(13).fontColor('#666666').margin({ top: 4 })
                }
                .alignItems(HorizontalAlign.Start)
                .margin({ left: 12 })
                .layoutWeight(1)

                Text(item.date)
                  .fontSize(12)
                  .fontColor('#999999')
                  .padding({
                    left: 8,
                    right: 8,
                    top: 4,
                    bottom: 4
                  })
                  .backgroundColor('#F1F3F5')
                  .borderRadius(6)
              }
              .width('95%')
              .padding(15)
              .backgroundColor(Color.White)
              .borderRadius(12)
              .gesture(
                LongPressGesture({ repeat: false })
                  .onAction(() => {
                    this.confirmDelete(item.id, item.type);
                  })
              )
            }
          }
        }, (item: PetRecord) => item.id.toString())
      }
      .width('100%')
      .layoutWeight(1)
      .alignListItem(ListItemAlign.Center)
      .padding({ bottom: 20 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
    .onAppear(async () => {
      console.info('PetDiary: 记录页激活，正在执行全量同步...');
      const db = DatabaseManager.getInstance();

      // 1. 无论如何，先拿最新的宠物名单
      const latestList = await db.getAllPets();
      this.allPets = latestList;

      // 2. 核心逻辑：如果当前选中的宠物不在新列表里，或者还没选过，才强制选第一个
      if (this.allPets.length > 0) {
        const isCurrentValid = this.allPets.find(p => p.id === this.currentPetId);
        if (!isCurrentValid || this.currentPetId === -1) {
          this.currentPetId = this.allPets[0].id;
          this.currentPetName = this.allPets[0].name;
        }
      }

      // 3. 【最重要】：无论列表变没变，都重新刷一遍足迹，确保数据是最新的
      await this.refreshLogs();
      console.info('PetDiary: 记录页数据同步完成');
    })
  }

  /**
   * 辅助方法：生成标准格式的今日日期
   * 增加补零逻辑以确保数据库排序正常
   */
  private getTodayDate(): string {
    const date = new Date();
    const year = date.getFullYear();
    const month = (date.getMonth() + 1).toString().padStart(2, '0');
    const day = date.getDate().toString().padStart(2, '0');
    return `${year}-${month}-${day}`;
  }

  /**
   * 逻辑方法：动态生成下拉筛选菜单的选项列表
   */
  private getFilterOptions(): SelectOption[] {
    let options: SelectOption[] = [{ value: '全部' }];
    for (let i = 0; i < PET_ACTIONS.length; i++) {
      options.push({ value: PET_ACTIONS[i].label });
    }
    return options;
  }

  /**
   * 辅助方法：根据行为类型获取对应的主题颜色
   */
  private getIconColor(type: string): string {
    for (let i = 0; i < PET_ACTIONS.length; i++) {
      if (PET_ACTIONS[i].label === type) {
        return PET_ACTIONS[i].color;
      }
    }
    return '#CCCCCC';
  }

  /**
   * 业务逻辑：处理保存操作
   */
  private async handleSaveRecord() {
    // 校验：必须先选中宠物
    if (this.currentPetId === -1) {
      promptAction.showToast({ message: '请先在档案页添加宠物' });
      return;
    }

    try {
      let finalContent: string = this.userRemark.trim();
      if (finalContent === '') {
        finalContent = `${this.activeType}完成`;
      }

      // 【逻辑升级】：使用 this.currentPetId 存储，实现多宠独立记录
      await DatabaseManager.getInstance()
        .insertRecord(this.currentPetId, this.activeType, finalContent, this.selectedDate);

      this.userRemark = '';
      promptAction.showToast({ message: `已为 ${this.currentPetName} 记录成功！` });

      await this.refreshLogs();
    } catch (err) {
      console.error("Save Record Logic Error");
    }
  }

  /**
   * 逻辑方法：删除行为记录的二次确认弹窗
   */
  private confirmDelete(id: number, type: string) {
    AlertDialog.show({
      title: '删除记录',
      message: `确定要永久删除这条 [${type}] 的足迹吗？`,
      primaryButton: {
        value: '取消',
        action: () => {
        }
      },
      secondaryButton: {
        value: '确认删除',
        fontColor: Color.Red,
        action: async () => {
          try {
            await DatabaseManager.getInstance().deleteRecord(id);
            await this.refreshLogs();
            promptAction.showToast({ message: '已删除' });
          } catch (e) {
            console.error("Delete Record Failed");
          }
        }
      }
    })
  }

  /**
   * 核心逻辑：发布系统级代理提醒 (日历模式)
   */
  private publishCalendarReminder(content: string, date: Date, time: Date) {
    // 逻辑升级：提醒内容自动加上宠物名字，让提醒更精准
    let finalContent = content.trim();
    if (finalContent === '') {
      finalContent = `提醒：该给 ${this.currentPetName} 处理 [${this.activeType}] 啦！`;
    } else {
      finalContent = `【${this.currentPetName}】${finalContent}`;
    }

    let calendarReminder: reminderAgentManager.ReminderRequestCalendar = {
      reminderType: reminderAgentManager.ReminderType.REMINDER_TYPE_CALENDAR,
      dateTime: {
        year: date.getFullYear(),
        month: date.getMonth() + 1,
        day: date.getDate(),
        hour: time.getHours(),
        minute: time.getMinutes(),
        second: 0
      },
      repeatMonths: [],
      repeatDays: [],
      title: `🐾 ${this.currentPetName} 的日程提醒`,
      content: finalContent,
      slotType: notificationManager.SlotType.SOCIAL_COMMUNICATION,
      ringDuration: 60,
      wantAgent: {
        pkgName: "com.group.petrecord",
        abilityName: "EntryAbility"
      }
    }

    try {
      reminderAgentManager.publishReminder(calendarReminder, (err, reminderId) => {
        if (!err) {
          AlertDialog.show({
            title: '提醒设定成功',
            message: `提醒时间：${date.getFullYear()}-${date.getMonth() +
              1}-${date.getDate()} ${time.getHours()}:${time.getMinutes()} \n对象：${this.currentPetName}`,
            confirm: {
              value: '知道了', action: () => {
              }
            }
          });
        }
      });
    } catch (e) {
      console.error('PetDiary: 系统组件调用异常');
    }
  }
}