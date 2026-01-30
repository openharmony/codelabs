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

// EventEditPage.ets
import { DataManager } from './DataManager';
import { CalendarEvent } from '../model/EventModel'
import { ReminderOption, RepeatOption } from '../model/EventTypes';
import common from '@ohos.app.ability.common'
import util from '@ohos.util';
import router from '@ohos.router';

// interface ReminderOption {
//   label: string,
//   minutes: number
// }

@Builder
export function EventEditBuilder() {
  EventEdit();
}

@ComponentV2
struct EventEdit {
  //导航栈
  pathStack: NavPathStack = new NavPathStack();

  // 状态变量
  @Local title: string = ''
  @Local description: string = ''
  @Local startTime: Date = new Date()
  @Local endTime: Date = new Date()
  @Local allDay: boolean = false
  @Local location: string = ''
  @Local calendarId: string = 'default' // 默认日历ID
  @Local selectedColor: string = '#FF3B30' // 默认红色
  @Local completed: boolean = false
  @Local showTimePicker: boolean = false
  @Local editEventId: string = '0'  // 0表示新增，大于0表示编辑现有事件
  @Local createdTime: number = 0;

  // 提醒设置
  @Local reminderIndex: number = 0
  @Local showReminderPicker: boolean = false
  // 重复设置
  @Local repeatIndex: number = 0
  @Local showRepeatPicker: boolean = false

  @Local para: CalendarEvent = {
    id: '0',
    title: '',
    description: '',
    startTime: 0,
    endTime: 0,
    allDay: false,
    location: '',
    calendarId: '',
    color: '',
    reminder: 0,
    repeatRule: '',
    completed: false,
    created: 0,
    updated: 0
  }

  // context获取
  private context: common.UIAbilityContext =
    this.getUIContext().getHostContext() as common.UIAbilityContext;
  private dataManager: DataManager = DataManager.getInstance(this.context);

  // 留着备用
  aboutToAppear() {
    // 如果是编辑模式，检查是否有传入的事件ID
    const params = router.getParams() as Record<string, string>;
    if (params && params['eventId'] && params['eventId'] !== '0') {
      this.editEventId = params['eventId'];
      this.loadEventData(this.editEventId);
    } else {
      // 新建模式，设置默认时间
      this.setDefaultTimes();
    }
  }

  // 设置默认时间（当前时间开始，持续1小时）
  private setDefaultTimes() {
    const now = new Date();
    this.startTime = now;
    const endTime = new Date(now.getTime() + 60 * 60 * 1000); // 1小时后
    this.endTime = endTime;
  }

  // 加载已有事件数据
  private async loadEventData(id: string) {
    try {
      const result = await this.dataManager.getEventById(id);
      if (result.success && result.data) {
        const event = result.data;
        this.editEventId = event.id;
        this.title = event.title;
        this.description = event.description;
        this.startTime = new Date(event.startTime);
        this.endTime = new Date(event.endTime);
        this.allDay = event.allDay;
        this.location = event.location;
        this.calendarId = event.calendarId;
        this.selectedColor = event.color;
        this.completed = event.completed;
        this.createdTime = event.created;

        // 设置提醒选项索引
        this.reminderIndex = this.dataManager.getReminderIndexByMinutes(event.reminder);

        // 设置重复选项索引
        this.repeatIndex = this.dataManager.getRepeatIndexByRule(event.repeatRule);

        this.getUIContext().getPromptAction().showToast({
          message: '事件加载成功',
          duration: 2000
        });
      } else if (result.error) {
        this.getUIContext().getPromptAction().showToast({
          message: `加载失败: ${result.error}`,
          duration: 3000
        });
      }
    } catch (error) {
      console.error('加载事件数据失败:', error);
      this.getUIContext().getPromptAction().showToast({
        message: '加载事件失败',
        duration: 3000
      });
    }
  }

  private colorOptions: string[] = ['#FF3B30', '#FFCC00', '#007AFF', '#34C759']
  private colorNames: string[] = ['红', '黄', '蓝', '绿']

  // 顶部标题栏
  @Builder
  buildHeader() {
    Row({ space: 0 }) {
      // 返回按钮
      Button('取消')
        .fontSize(16)
        .fontColor('#007AFF')
        .backgroundColor(Color.Transparent)
        .onClick(() => {
          // 返回逻辑
          const eventData: CalendarEvent = {
            id: '0',
            title: '',
            description: '',
            startTime: 0,
            endTime: 0,
            allDay: false,
            location: '',
            calendarId: '',
            color: '',
            reminder: 0,
            repeatRule: '',
            completed: false,
            created: 0,
            updated: 0
          }
          this.pathStack.replacePathByName('Calendar', eventData, null)
        })

      Text(this.editEventId === '0' ? '新建事件' : '编辑事件')
        .fontSize(18)
        .fontWeight(FontWeight.Medium)
        .layoutWeight(1)
        .textAlign(TextAlign.Center)

      Button('保存')
        .fontSize(16)
        .fontColor('#007AFF')
        .backgroundColor(Color.Transparent)
        .onClick(() => {
          this.saveEvent()
        })
    }
    .padding({ left: 16, right: 16, top: 10, bottom: 10 })
    .width('100%')
    .backgroundColor(Color.White)
  }

  // 标题输入框
  @Builder
  buildTitleInput() {
    Column({ space: 8 }) {
      Text('标题')
        .fontSize(16)
        .fontColor('#333333')
        .align(Alignment.Start)
        .width('100%')

      TextInput({ text:this.title, placeholder: '添加标题' })
        .onChange((value: string) => {
          this.title = value
        })
        .height(48)
        .width('100%')
        .backgroundColor(Color.White)
        .borderRadius(8)
        .padding(12)
    }
  }

  // 描述输入框
  @Builder
  buildDescriptionInput() {
    Column({ space: 8 }) {
      Text('描述')
        .fontSize(16)
        .fontColor('#333333')
        .align(Alignment.Start)
        .width('100%')

      TextInput({ text:this.description, placeholder: '添加描述' })
        .onChange((value: string) => {
          this.description = value
        })
        .height(80)
        .width('100%')
        .backgroundColor(Color.White)
        .borderRadius(8)
        .padding(12)
    }
  }

  // 时间选择部分
  @Builder
  buildTimeSection() {
    Column({ space: 8 }) {
      Text('时间')
        .fontSize(16)
        .fontColor('#333333')
        .align(Alignment.Start)
        .width('100%')

      // 时间显示和选择按钮
      Row() {
        Column({ space: 4 }) {
          Text(this.formatTime(this.startTime, this.allDay))
            .fontSize(14)
            .fontColor('#333333')
          Text('开始时间')
            .fontSize(12)
            .fontColor('#666666')
        }
        .layoutWeight(1)

        Image($r('app.media.minus'))
          .width(16)
          .height(16)
          .margin({ left: 8, right: 8 })

        Column({ space: 4 }) {
          Text(this.formatTime(this.endTime, this.allDay))
            .fontSize(14)
            .fontColor('#333333')
          Text('结束时间')
            .fontSize(12)
            .fontColor('#666666')
        }
        .layoutWeight(1)
      }
      .padding(16)
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius(8)
      .onClick(() => {
        this.closeAllPickers();  // 先关闭其他弹窗
        this.showTimePicker = true
      })
    }

    // 时间选择器弹窗
    if (this.showTimePicker) {
      // this.buildTimePickerDialog()
    }
  }

  // 时间选择器弹窗
  @Builder
  buildTimePickerDialog() {
    Column() {
      // 弹窗标题栏
      Row({ space: 0 }) {
        Button('取消')
          .fontSize(16)
          .fontColor('#666666')
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.showTimePicker = false
          })

        Text('选择时间')
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
          .layoutWeight(1)
          .textAlign(TextAlign.Center)

        Button('完成')
          .fontSize(16)
          .fontColor('#007AFF')
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.showTimePicker = false
          })
      }
      .padding({ left: 16, right: 16, top: 10, bottom: 10 })
      .width('100%')

      // 时间选择器
      Row() {
        Column() {
          Text('开始时间')
            .fontSize(14)
            .margin({ bottom: 8 })
          TimePicker({
            selected: this.startTime
          })
            .onChange((time: TimePickerResult)  => {
              // const newStartTime = new Date()
              // newStartTime.setHours(time.hour)
              // newStartTime.setMinutes(time.minute)
              // this.startTime = newStartTime
              // 修改现有Date对象的时分，保留日期
              this.startTime.setHours(time.hour);
              this.startTime.setMinutes(time.minute);
              this.startTime = new Date(this.startTime.getTime());
              // 如果结束时间早于开始时间，自动调整
              if (this.endTime.getTime() < this.startTime.getTime()) {
                let newEndTime = new Date(this.startTime)
                newEndTime.setHours(this.startTime.getHours() + 1)
                this.endTime = newEndTime
              }
            })
        }
        .layoutWeight(1)

        Column() {
          Text('结束时间')
            .fontSize(14)
            .margin({ bottom: 8 })
          TimePicker({
            selected: this.endTime
          })
            .onChange((time: TimePickerResult) => {
              // 使用TimePickerResult类型
              // const newEndTime = new Date()
              // newEndTime.setHours(time.hour)
              // newEndTime.setMinutes(time.minute)
              // this.endTime = newEndTime
              // 修改时分，不改变日期
              this.endTime.setHours(time.hour);
              this.endTime.setMinutes(time.minute);
              // 触发UI更新
              this.endTime = new Date(this.endTime.getTime());
            })
        }
        .layoutWeight(1)
      }
      .padding(16)
    }
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(16)
    .shadow({ radius: 20, color: '#1A000000', offsetX: 0, offsetY: 2 })
    .position({ x: 0, y: '20%' })
    .zIndex(999)
  }

  // 全天事件开关
  @Builder
  buildAllDayToggle() {
    Row() {
      Text('全天')
        .fontSize(16)
        .fontColor('#333333')
        .layoutWeight(1)

      Toggle({ type: ToggleType.Switch, isOn: this.allDay })
        .onChange((isOn: boolean) => {
          this.allDay = isOn
          if (isOn) {
            // 设置为全天：0-24点
            let start = new Date()
            start.setHours(0, 0, 0, 0)
            let end = new Date()
            end.setHours(23, 59, 0, 0)
            this.startTime = start
            this.endTime = end
          }
        })
    }
    .padding(16)
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(8)
  }

  // 地点输入
  @Builder
  buildLocationInput() {
    Column({ space: 8 }) {
      Text('地点')
        .fontSize(16)
        .fontColor('#333333')
        .align(Alignment.Start)
        .width('100%')

      TextInput({ text:this.location, placeholder: '添加地点' })
        .onChange((value: string) => {
          this.location = value
        })
        .height(48)
        .width('100%')
        .backgroundColor(Color.White)
        .borderRadius(8)
        .padding(12)
    }
  }

  // 颜色选择器
  @Builder
  buildColorPicker() {
    Column({ space: 8 }) {
      Text('颜色标记')
        .fontSize(16)
        .fontColor('#333333')
        .align(Alignment.Start)
        .width('100%')

      Row({ space: 16 }) {
        ForEach(this.colorOptions, (color: string, index: number) => {
          Column({ space: 4 }) {
            // 颜色圆圈
            Circle()
              .width(40)
              .height(40)
              .fill(color)
              .border({
                width: this.selectedColor === color ? 3 : 0,
                color: '#ff8000ff',
                style: BorderStyle.Solid
              })
              .onClick(() => {
                this.selectedColor = color
              })
              .borderRadius(20)

            Text(this.colorNames[index])
              .fontSize(12)
              .fontColor('#666666')
          }
        })
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceAround)
      .padding(16)
      .backgroundColor(Color.White)
      .borderRadius(8)
    }
  }

  // 提醒设置
  @Builder
  buildReminderSection() {
    Column({ space: 8 }) {
      Text('提醒')
        .fontSize(16)
        .fontColor('#333333')
        .align(Alignment.Start)
        .width('100%')

      Row() {
        Text(this.dataManager.getReminderOptions()[this.reminderIndex].label)
          .fontSize(14)
          .fontColor('#333333')
          .layoutWeight(1)

        Blank()
          .width(16)
          .height(16)
      }
      .padding(16)
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius(8)
      .onClick(() => {
        this.closeAllPickers();
        this.showReminderPicker = true
      })
    }

    // 提醒选择弹窗
    if (this.showReminderPicker) {
      this.buildReminderPicker()
    }
  }

  // 提醒选择器
  @Builder
  buildReminderPicker() {
    Column() {
      // 弹窗标题
      Text('选择提醒时间')
        .fontSize(18)
        .fontWeight(FontWeight.Medium)
        .padding({ top: 20, bottom: 20 })

      // 选项列表
      Column() {
        ForEach(this.dataManager.getReminderOptions(), (option: ReminderOption, index: number) => {
          Row() {
            Text(option.label)
              .fontSize(16)
              .fontColor('#333333')
              .layoutWeight(1)

            if (index === this.reminderIndex) {
              Image($r('app.media.checkmark'))
                .width(20)
                .height(20)
                .fillColor('#007AFF')
            }
          }
          .padding({ left: 20, right: 20, top: 14, bottom: 14 })
          .width('100%')
          .onClick(() => {
            this.reminderIndex = index
            this.showReminderPicker = false
          })
        })
      }

      // 取消按钮
      Button('取消')
        .fontSize(16)
        .fontColor('#007AFF')
        .backgroundColor(Color.Transparent)
        .width('100%')
        .height(48)
        .onClick(() => {
          this.showReminderPicker = false;
        })
    }
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(16)
    .shadow({ radius: 20, color: '#1A000000', offsetX: 0, offsetY: 2 })
    .position({ x: '0%', y: '40%' })
    .zIndex(999)
  }

  // 重复设置
  @Builder
  buildRepeatSection() {
    Column({ space: 8 }) {
      Text('重复')
        .fontSize(16)
        .fontColor('#333333')
        .align(Alignment.Start)
        .width('100%')

      Row() {
        Text(this.dataManager.getRepeatOptions()[this.repeatIndex].label)
          .fontSize(14)
          .fontColor('#333333')
          .layoutWeight(1)

        Blank()
          .width(16)
          .height(16)
      }
      .padding(16)
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius(8)
      .onClick(() => {
        this.closeAllPickers();
        this.showRepeatPicker = true
      })
    }

    // 重复选择弹窗
    if (this.showRepeatPicker) {
      this.buildRepeatPicker()
    }
  }

  // 重复选择器
  @Builder
  buildRepeatPicker() {
    Column() {
      Text('选择重复')
        .fontSize(18)
        .fontWeight(FontWeight.Medium)
        .padding({ top: 20, bottom: 20 })

      Column() {
        ForEach(this.dataManager.getRepeatOptions(), (option: RepeatOption, index: number) => {
          Row() {
            Text(option.label)
              .fontSize(16)
              .fontColor('#333333')
              .layoutWeight(1)

            if (index === this.repeatIndex) {
              Image($r('app.media.checkmark'))
                .width(20)
                .height(20)
                .fillColor('#007AFF')
            }
          }
          .padding({ left: 20, right: 20, top: 14, bottom: 14 })
          .width('100%')
          .onClick(() => {
            this.repeatIndex = index
            this.showRepeatPicker = false
          })
        })
      }
      // 取消按钮
      Button('取消')
        .fontSize(16)
        .fontColor('#007AFF')
        .backgroundColor(Color.Transparent)
        .width('100%')
        .height(48)
        .onClick(() => {
          this.showRepeatPicker = false;
        })
    }
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(16)
    .shadow({ radius: 20, color: '#1A000000', offsetX: 0, offsetY: 2 })
    .position({ x: '0%', y: '70%' })
    .zIndex(999)
  }

  // 关闭所有复选框
  private closeAllPickers() {
    this.showTimePicker = false;
    this.showReminderPicker = false;
    this.showRepeatPicker = false;
  }

  // 格式化时间显示
  private formatTime(date: Date, isAllDay: boolean): string {
    if (isAllDay) {
      return '全天'
    }
    let hours = date.getHours().toString().padStart(2, '0')
    let minutes = date.getMinutes().toString().padStart(2, '0')
    return `${hours}:${minutes}`
  }

  // 保存事件
  private async saveEvent() {
    // 验证必填字段
    if (!this.title || !this.title.trim()) {
      this.getUIContext().getPromptAction().showToast({
        message: '标题不能为空',
        duration: 3000
      });
      return;
    }

    if (this.startTime.getTime() > this.endTime.getTime()) {
      this.getUIContext().getPromptAction().showToast({
        message: '开始时间不能晚于结束时间',
        duration: 3000
      });
      return;
    }

    // 获取提醒分钟数
    const reminderOptions = this.dataManager.getReminderOptions();
    const reminderMinutes = this.reminderIndex < reminderOptions.length
      ? reminderOptions[this.reminderIndex].minutes
      : -1;

    // 获取重复规则字符串
    const repeatOptions = this.dataManager.getRepeatOptions();
    let repeatRuleString = '';
    if (this.repeatIndex < repeatOptions.length) {
      repeatRuleString = this.dataManager.getRepeatRuleByIndex(this.repeatIndex);
    }

    const now = new Date().getTime()
    let uuidString: string = util.generateRandomUUID(true);
    const eventData: CalendarEvent = {
      id: this.editEventId === '0' ? uuidString : this.editEventId,
      title: this.title,
      description: this.description,
      startTime: this.startTime.getTime(), // 转换为时间戳
      endTime: this.endTime.getTime(),     // 转换为时间戳
      allDay: this.allDay,
      location: this.location,
      calendarId: this.calendarId,
      color: this.selectedColor,
      reminder: reminderMinutes,
      repeatRule: repeatRuleString, // DataManager会处理转换
      completed: this.completed,
      created: this.editEventId === '0' ? now : this.createdTime,
      updated: now
    }

    try {
      this.getUIContext().getPromptAction().showToast({
        message: '正在保存...',
        duration: 1000
      });

      const result = await this.dataManager.saveEvent(eventData);

      if (result.success) {
        console.log('事件保存成功，ID:', result.id);
        this.getUIContext().getPromptAction().showToast({
          message: '保存成功',
          duration: 2000
        });

        // 延迟返回，让用户看到成功提示
        setTimeout(() => {
          this.pathStack.replacePathByName('Calendar', eventData, null)
        }, 2000);
      } else {
        console.error('保存失败:', result.error);
        this.getUIContext().getPromptAction().showToast({
          message: `保存失败: ${result.error}`,
          duration: 3000
        });
      }
    } catch (error) {
      console.error('保存事件时发生错误:', error);
      this.getUIContext().getPromptAction().showToast({
        message: `保存失败: ${error.message || '未知错误'}`,
        duration: 3000
      });
    }
  }

  //构建主界面
  build() {
    NavDestination(){
      Stack(){
        Column() {
          // 顶部标题栏
          this.buildHeader()

          // 主体内容
          Scroll() {
            Column({ space: 24 }) {
              // 1. 标题输入
              this.buildTitleInput()

              // 2. 描述输入
              this.buildDescriptionInput()

              // 3. 时间选择部分
              this.buildTimeSection()

              // 4. 全天事件开关
              this.buildAllDayToggle()

              // 5. 地点输入
              this.buildLocationInput()

              // 6. 颜色标记
              this.buildColorPicker()

              // 7. 提醒设置
              this.buildReminderSection()

              // 8. 重复设置
              this.buildRepeatSection()
            }
            .padding(20)
            .width('100%')
          }
          .scrollBar(BarState.Off)
          .layoutWeight(1)
        }
        .width('100%')
        .height('100%')
        .backgroundColor('#F5F5F5')

        // 半透明遮罩层，点击关闭所有弹窗
        if (this.showTimePicker || this.showReminderPicker || this.showRepeatPicker) {
          Column()
            .width('100%')
            .height('100%')
            .backgroundColor('#000000')
            .opacity(0.3)
            .onClick(() => {
              this.closeAllPickers();
            })
            .zIndex(99)

          Stack() {
            if (this.showTimePicker) {
              this.buildTimePickerDialog()
            }
            if (this.showReminderPicker) {
              this.buildReminderPicker()
            }
            if (this.showRepeatPicker) {
              this.buildRepeatPicker()
            }
          }
          .width('100%')
          .height('100%')
          .position({ x: 0, y: 0 })
          .zIndex(100) // 确保容器在最顶层
        }
      }
    }
    .hideBackButton(true)
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
      this.para = context.pathInfo.param as CalendarEvent
      console.log("收到的信息是:"+`${this.para}`);
      const id:string = this.para.id
      if (id!='0') {
        this.editEventId = id;
        this.loadEventData(id);
      } else {
        // 新增事件，设置默认时间
        this.setDefaultTimes();
      }
    })
  }
}