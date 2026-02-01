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
 *
 */

/**
 * 日程编辑页面
 */
import { router } from '@kit.ArkUI'
import { ReminderTiming, TAG_COLORS } from '../common/constants/Constants'
import { formatDateChinese, formatTime } from '../common/utils/DateUtils'
import { Logger } from '../common/utils/Logger'
import { Schedule, ScheduleInit } from '../model/Schedule'
import { Reminder, ReminderInit } from '../model/Reminder'
import { Recording } from '../model/Recording'
import { DEFAULT_TAGS, Tag } from '../model/Tag'
import { dataService } from '../service/DataService'
import { audioService } from '../service/AudioService'
import { RecordingPanel } from '../components/recording/RecordingPanel'
import { RecordingList } from '../components/recording/RecordingList'

const TAG = 'ScheduleEditPage'

interface RouterParams {
  scheduleId?: string
  date?: number
}

@Entry
@Component
struct ScheduleEditPage {
  @State isEditMode: boolean = false
  @State scheduleId: string = ''
  @State scheduleTitle: string = ''
  @State scheduleDescription: string = ''
  @State scheduleStartTime: number = 0
  @State scheduleEndTime: number = 0
  @State scheduleLocation: string = ''
  @State scheduleColor: string = '#4ECDC4'
  @State scheduleIsAllDay: boolean = false
  @State scheduleExternalAppLink: string = ''
  @State selectedTags: string[] = []
  @State availableTags: Tag[] = DEFAULT_TAGS
  @State selectedReminders: ReminderTiming[] = [ReminderTiming.MINUTES_15]
  @State showStartDatePicker: boolean = false
  @State showEndDatePicker: boolean = false
  @State showStartTimePicker: boolean = false
  @State showEndTimePicker: boolean = false
  @State isSaving: boolean = false
  // 录音相关状态
  @State recordings: Recording[] = []
  @State newRecordings: Recording[] = [] // 新添加的录音（尚未保存）
  @State deletedRecordingIds: string[] = [] // 待删除的录音ID
  // 时间选择器临时值
  @State tempHour: number = 0
  @State tempMinute: number = 0
  @State tempYear: number = 0
  @State tempMonth: number = 0
  @State tempDay: number = 0
  private scrollerForPage: Scroller = new Scroller()

  aboutToAppear(): void {
    const params = router.getParams() as RouterParams
    if (params?.scheduleId) {
      this.isEditMode = true
      this.loadSchedule(params.scheduleId)
    } else {
      this.initNewSchedule(params?.date)
    }
    this.loadTags()
  }

  /**
   * 加载已有日程
   */
  async loadSchedule(scheduleId: string): Promise<void> {
    try {
      const schedule = await dataService.getScheduleById(scheduleId)
      if (schedule) {
        this.scheduleId = schedule.id
        this.scheduleTitle = schedule.title
        this.scheduleDescription = schedule.description
        this.scheduleStartTime = schedule.startTime
        this.scheduleEndTime = schedule.endTime
        this.scheduleLocation = schedule.location
        this.scheduleColor = schedule.color
        this.scheduleIsAllDay = schedule.isAllDay
        this.scheduleExternalAppLink = schedule.externalAppLink
        this.selectedTags = schedule.tagIds

        // 加载已有录音
        this.recordings = await dataService.getRecordingsByScheduleId(scheduleId)
      }
    } catch (error) {
      Logger.error(TAG, `Failed to load schedule: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 初始化新日程
   */
  initNewSchedule(timestamp?: number): void {
    const now = timestamp ? new Date(timestamp) : new Date()
    // 设置开始时间为下一个整点
    const startTime = new Date(now)
    startTime.setMinutes(0, 0, 0)
    if (now.getMinutes() > 0) {
      startTime.setHours(startTime.getHours() + 1)
    }

    // 默认时长1小时
    const endTime = new Date(startTime)
    endTime.setHours(endTime.getHours() + 1)

    this.scheduleStartTime = startTime.getTime()
    this.scheduleEndTime = endTime.getTime()
    this.scheduleColor = TAG_COLORS[0]
  }

  /**
   * 加载标签列表
   */
  async loadTags(): Promise<void> {
    try {
      const tags = await dataService.getAllTags()
      if (tags.length > 0) {
        this.availableTags = tags
      }
    } catch (error) {
      Logger.error(TAG, `Failed to load tags: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 保存日程
   */
  async saveSchedule(): Promise<void> {
    if (!this.scheduleTitle.trim()) {
      return
    }

    this.isSaving = true
    try {
      const init: ScheduleInit = {
        id: this.isEditMode ? this.scheduleId : undefined,
        title: this.scheduleTitle,
        description: this.scheduleDescription,
        startTime: this.scheduleStartTime,
        endTime: this.scheduleEndTime,
        location: this.scheduleLocation,
        color: this.scheduleColor,
        isAllDay: this.scheduleIsAllDay,
        externalAppLink: this.scheduleExternalAppLink,
        tagIds: this.selectedTags
      }

      const schedule = new Schedule(init)

      if (this.isEditMode) {
        await dataService.updateSchedule(schedule)

        // 编辑模式下更新提醒：先删除旧提醒，再创建新提醒
        await dataService.deleteRemindersByScheduleId(schedule.id)
        for (const timing of this.selectedReminders) {
          const reminderInit: ReminderInit = {
            scheduleId: schedule.id,
            timing: timing
          }
          const reminder = new Reminder(reminderInit)
          reminder.calculateTriggerTime(schedule.startTime)
          await dataService.insertReminder(reminder)
        }

        // 删除待删除的录音
        for (const recordingId of this.deletedRecordingIds) {
          const recording = this.recordings.find((r: Recording) => r.id === recordingId)
          if (recording) {
            await audioService.deleteRecordingFile(recording.filePath)
          }
          await dataService.deleteRecording(recordingId)
        }
      } else {
        await dataService.insertSchedule(schedule)

        // 保存提醒
        for (const timing of this.selectedReminders) {
          const reminderInit: ReminderInit = {
            scheduleId: schedule.id,
            timing: timing
          }
          const reminder = new Reminder(reminderInit)
          reminder.calculateTriggerTime(schedule.startTime)
          await dataService.insertReminder(reminder)
        }
      }

      // 保存新录音
      for (const recording of this.newRecordings) {
        recording.scheduleId = schedule.id
        await dataService.insertRecording(recording)
      }

      Logger.info(TAG, `Schedule saved: ${schedule.id}`)
      router.back()
    } catch (error) {
      Logger.error(TAG, `Failed to save schedule: ${JSON.stringify(error)}`)
    }
    this.isSaving = false
  }

  /**
   * 切换标签选中状态
   */
  toggleTag(tagId: string): void {
    const index = this.selectedTags.indexOf(tagId)
    if (index >= 0) {
      this.selectedTags.splice(index, 1)
    } else {
      this.selectedTags.push(tagId)
    }
    // 更新日程颜色为第一个选中标签的颜色
    if (this.selectedTags.length > 0) {
      const selectedTag = this.availableTags.find((t: Tag) => t.id === this.selectedTags[0])
      if (selectedTag) {
        this.scheduleColor = selectedTag.color
      }
    }
  }

  /**
   * 切换提醒选中状态
   */
  toggleReminder(timing: ReminderTiming): void {
    const index = this.selectedReminders.indexOf(timing)
    if (index >= 0) {
      this.selectedReminders.splice(index, 1)
    } else {
      this.selectedReminders.push(timing)
    }
  }

  /**
   * 获取提醒时机文本
   */
  getReminderText(timing: ReminderTiming): string {
    switch (timing) {
      case ReminderTiming.ON_TIME:
        return '准时'
      case ReminderTiming.MINUTES_5:
        return '5分钟前'
      case ReminderTiming.MINUTES_10:
        return '10分钟前'
      case ReminderTiming.MINUTES_15:
        return '15分钟前'
      case ReminderTiming.MINUTES_30:
        return '30分钟前'
      case ReminderTiming.HOURS_1:
        return '1小时前'
      case ReminderTiming.HOURS_2:
        return '2小时前'
      case ReminderTiming.DAYS_1:
        return '1天前'
      default:
        return ''
    }
  }

  /**
   * 打开开始日期选择器
   */
  openStartDatePicker(): void {
    const date = new Date(this.scheduleStartTime)
    this.tempYear = date.getFullYear()
    this.tempMonth = date.getMonth() + 1
    this.tempDay = date.getDate()
    this.showStartDatePicker = true
  }

  /**
   * 打开结束日期选择器
   */
  openEndDatePicker(): void {
    const date = new Date(this.scheduleEndTime)
    this.tempYear = date.getFullYear()
    this.tempMonth = date.getMonth() + 1
    this.tempDay = date.getDate()
    this.showEndDatePicker = true
  }

  /**
   * 打开开始时间选择器
   */
  openStartTimePicker(): void {
    const date = new Date(this.scheduleStartTime)
    this.tempHour = date.getHours()
    this.tempMinute = date.getMinutes()
    this.showStartTimePicker = true
  }

  /**
   * 打开结束时间选择器
   */
  openEndTimePicker(): void {
    const date = new Date(this.scheduleEndTime)
    this.tempHour = date.getHours()
    this.tempMinute = date.getMinutes()
    this.showEndTimePicker = true
  }

  /**
   * 确认开始日期
   */
  confirmStartDate(): void {
    const date = new Date(this.scheduleStartTime)
    date.setFullYear(this.tempYear, this.tempMonth - 1, this.tempDay)
    this.scheduleStartTime = date.getTime()
    // 如果结束时间早于开始时间，调整结束时间
    if (this.scheduleEndTime < this.scheduleStartTime) {
      this.scheduleEndTime = this.scheduleStartTime + 60 * 60 * 1000
    }
    this.showStartDatePicker = false
  }

  /**
   * 确认结束日期
   */
  confirmEndDate(): void {
    const date = new Date(this.scheduleEndTime)
    date.setFullYear(this.tempYear, this.tempMonth - 1, this.tempDay)
    this.scheduleEndTime = date.getTime()
    // 如果结束时间早于开始时间，调整开始时间
    if (this.scheduleEndTime < this.scheduleStartTime) {
      this.scheduleStartTime = this.scheduleEndTime - 60 * 60 * 1000
    }
    this.showEndDatePicker = false
  }

  /**
   * 确认开始时间
   */
  confirmStartTime(): void {
    const date = new Date(this.scheduleStartTime)
    date.setHours(this.tempHour, this.tempMinute, 0, 0)
    this.scheduleStartTime = date.getTime()
    // 如果结束时间早于开始时间，调整结束时间
    if (this.scheduleEndTime <= this.scheduleStartTime) {
      this.scheduleEndTime = this.scheduleStartTime + 60 * 60 * 1000
    }
    this.showStartTimePicker = false
  }

  /**
   * 确认结束时间
   */
  confirmEndTime(): void {
    const date = new Date(this.scheduleEndTime)
    date.setHours(this.tempHour, this.tempMinute, 0, 0)
    this.scheduleEndTime = date.getTime()
    this.showEndTimePicker = false
  }

  @Builder
  TopBar() {
    Row() {
      Text('取消')
        .fontSize(16)
        .fontColor('#666666')
        .onClick(() => router.back())

      Blank()

      Text(this.isEditMode ? '编辑日程' : '新建日程')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')

      Blank()

      Text('保存')
        .fontSize(16)
        .fontColor(this.scheduleTitle.trim() ? '#4ECDC4' : '#CCCCCC')
        .fontWeight(FontWeight.Medium)
        .onClick(() => this.saveSchedule())
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .backgroundColor('#FFFFFF')
    .border({ width: { bottom: 1 }, color: '#E0E0E0' })
  }

  @Builder
  InputSection() {
    Column() {
      TextInput({ placeholder: '添加标题', text: this.scheduleTitle })
        .fontSize(20)
        .fontWeight(FontWeight.Medium)
        .backgroundColor(Color.Transparent)
        .padding(0)
        .onChange((value: string) => {
          this.scheduleTitle = value
        })

      Divider()
        .margin({ top: 16, bottom: 16 })
        .color('#E0E0E0')

      TextArea({ placeholder: '添加描述', text: this.scheduleDescription })
        .fontSize(15)
        .backgroundColor(Color.Transparent)
        .padding(0)
        .height(80)
        .onChange((value: string) => {
          this.scheduleDescription = value
        })
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  TimeSection() {
    Column() {
      // 全天开关
      Row() {
        Text('全天')
          .fontSize(16)
          .fontColor('#333333')

        Blank()

        Toggle({ type: ToggleType.Switch, isOn: this.scheduleIsAllDay })
          .selectedColor('#4ECDC4')
          .onChange((isOn: boolean) => {
            this.scheduleIsAllDay = isOn
          })
      }
      .width('100%')
      .height(48)

      Divider().color('#E0E0E0')

      // 开始时间
      Row() {
        Text('开始')
          .fontSize(16)
          .fontColor('#333333')

        Blank()

        Text(formatDateChinese(new Date(this.scheduleStartTime)))
          .fontSize(15)
          .fontColor('#4ECDC4')
          .padding({
            left: 8,
            right: 8,
            top: 4,
            bottom: 4
          })
          .backgroundColor('#E8F8F5')
          .borderRadius(4)
          .onClick(() => this.openStartDatePicker())

        if (!this.scheduleIsAllDay) {
          Text(formatTime(new Date(this.scheduleStartTime)))
            .fontSize(15)
            .fontColor('#4ECDC4')
            .padding({
              left: 8,
              right: 8,
              top: 4,
              bottom: 4
            })
            .backgroundColor('#E8F8F5')
            .borderRadius(4)
            .margin({ left: 8 })
            .onClick(() => this.openStartTimePicker())
        }
      }
      .width('100%')
      .height(56)

      Divider().color('#E0E0E0')

      // 结束时间
      Row() {
        Text('结束')
          .fontSize(16)
          .fontColor('#333333')

        Blank()

        Text(formatDateChinese(new Date(this.scheduleEndTime)))
          .fontSize(15)
          .fontColor('#4ECDC4')
          .padding({
            left: 8,
            right: 8,
            top: 4,
            bottom: 4
          })
          .backgroundColor('#E8F8F5')
          .borderRadius(4)
          .onClick(() => this.openEndDatePicker())

        if (!this.scheduleIsAllDay) {
          Text(formatTime(new Date(this.scheduleEndTime)))
            .fontSize(15)
            .fontColor('#4ECDC4')
            .padding({
              left: 8,
              right: 8,
              top: 4,
              bottom: 4
            })
            .backgroundColor('#E8F8F5')
            .borderRadius(4)
            .margin({ left: 8 })
            .onClick(() => this.openEndTimePicker())
        }
      }
      .width('100%')
      .height(56)
    }
    .width('100%')
    .padding({ left: 16, right: 16 })
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  LocationSection() {
    Row() {
      Text('📍')
        .fontSize(16)

      TextInput({ placeholder: '添加地点', text: this.scheduleLocation })
        .fontSize(15)
        .backgroundColor(Color.Transparent)
        .layoutWeight(1)
        .margin({ left: 12 })
        .onChange((value: string) => {
          this.scheduleLocation = value
        })
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  TagSection() {
    Column() {
      Row() {
        Text('标签')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#333333')
      }
      .width('100%')
      .margin({ bottom: 12 })

      Flex({ wrap: FlexWrap.Wrap }) {
        ForEach(this.availableTags, (tag: Tag) => {
          Row() {
            Circle()
              .width(12)
              .height(12)
              .fill(tag.color)

            Text(tag.name)
              .fontSize(14)
              .fontColor(this.selectedTags.includes(tag.id) ? '#FFFFFF' : '#666666')
              .margin({ left: 6 })
          }
          .padding({
            left: 12,
            right: 12,
            top: 8,
            bottom: 8
          })
          .backgroundColor(this.selectedTags.includes(tag.id) ? tag.color : '#F5F5F5')
          .borderRadius(20)
          .margin({ right: 8, bottom: 8 })
          .onClick(() => this.toggleTag(tag.id))
        }, (tag: Tag) => tag.id)
      }
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  ReminderSection() {
    Column() {
      Row() {
        Text('提醒')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#333333')
      }
      .width('100%')
      .margin({ bottom: 12 })

      Flex({ wrap: FlexWrap.Wrap }) {
        ForEach([
          ReminderTiming.ON_TIME,
          ReminderTiming.MINUTES_5,
          ReminderTiming.MINUTES_15,
          ReminderTiming.MINUTES_30,
          ReminderTiming.HOURS_1,
          ReminderTiming.DAYS_1
        ], (timing: ReminderTiming) => {
          Text(this.getReminderText(timing))
            .fontSize(14)
            .fontColor(this.selectedReminders.includes(timing) ? '#FFFFFF' : '#666666')
            .padding({
              left: 12,
              right: 12,
              top: 8,
              bottom: 8
            })
            .backgroundColor(this.selectedReminders.includes(timing) ? '#4ECDC4' : '#F5F5F5')
            .borderRadius(20)
            .margin({ right: 8, bottom: 8 })
            .onClick(() => this.toggleReminder(timing))
        }, (timing: ReminderTiming) => timing.toString())
      }
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  ExternalLinkSection() {
    Column() {
      Row() {
        Text('关联应用')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#333333')
      }
      .width('100%')
      .margin({ bottom: 12 })

      TextInput({ placeholder: '输入应用链接（如 tencentmeeting://）', text: this.scheduleExternalAppLink })
        .fontSize(14)
        .backgroundColor('#F5F5F5')
        .borderRadius(8)
        .padding({
          left: 12,
          right: 12,
          top: 12,
          bottom: 12
        })
        .onChange((value: string) => {
          this.scheduleExternalAppLink = value
        })

      Text('提醒触发时可选择打开关联的应用')
        .fontSize(12)
        .fontColor('#999999')
        .margin({ top: 8 })
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  /**
   * 录音完成处理
   */
  handleRecordingComplete(recording: Recording): void {
    this.newRecordings.push(recording)
    Logger.info(TAG, `New recording added: ${recording.id}`)
  }

  /**
   * 删除录音
   */
  handleDeleteRecording(recording: Recording): void {
    // 检查是新录音还是已保存的录音
    const newIndex = this.newRecordings.findIndex((r: Recording) => r.id === recording.id)
    if (newIndex >= 0) {
      // 是新录音，直接从列表移除并删除文件
      this.newRecordings.splice(newIndex, 1)
      audioService.deleteRecordingFile(recording.filePath)
    } else {
      // 是已保存的录音，标记为待删除
      this.deletedRecordingIds.push(recording.id)
      const existingIndex = this.recordings.findIndex((r: Recording) => r.id === recording.id)
      if (existingIndex >= 0) {
        this.recordings.splice(existingIndex, 1)
      }
    }
    Logger.info(TAG, `Recording deleted: ${recording.id}`)
  }

  /**
   * 获取所有录音（已保存 + 新添加）
   */
  getAllRecordings(): Recording[] {
    return [...this.recordings, ...this.newRecordings]
  }

  @Builder
  RecordingSection() {
    Column() {
      Row() {
        Text('🎤')
          .fontSize(16)

        Text('语音笔记')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#333333')
          .margin({ left: 8 })
      }
      .width('100%')
      .margin({ bottom: 12 })

      // 已有录音列表
      if (this.getAllRecordings().length > 0) {
        RecordingList({
          recordings: this.getAllRecordings(),
          onDelete: (recording: Recording) => this.handleDeleteRecording(recording)
        })
          .margin({ bottom: 12 })
      }

      // 录音面板
      RecordingPanel({
        onRecordingComplete: (recording: Recording) => this.handleRecordingComplete(recording),
        onCancel: () => {
          Logger.info(TAG, 'Recording cancelled')
        }
      })
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  DatePickerDialog(isStart: boolean) {
    Column() {
      Row() {
        Text('取消')
          .fontSize(16)
          .fontColor('#666666')
          .onClick(() => {
            if (isStart) {
              this.showStartDatePicker = false
            } else {
              this.showEndDatePicker = false
            }
          })

        Blank()

        Text(isStart ? '选择开始日期' : '选择结束日期')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')

        Blank()

        Text('确定')
          .fontSize(16)
          .fontColor('#4ECDC4')
          .fontWeight(FontWeight.Medium)
          .onClick(() => {
            if (isStart) {
              this.confirmStartDate()
            } else {
              this.confirmEndDate()
            }
          })
      }
      .width('100%')
      .padding(16)

      DatePicker({
        start: new Date('2020-01-01'),
        end: new Date('2030-12-31'),
        selected: isStart ? new Date(this.scheduleStartTime) : new Date(this.scheduleEndTime)
      })
        .onDateChange((value: Date) => {
          this.tempYear = value.getFullYear()
          this.tempMonth = value.getMonth() + 1
          this.tempDay = value.getDate()
        })
        .height(200)
    }
    .width('90%')
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
  }

  @Builder
  TimePickerDialog(isStart: boolean) {
    Column() {
      Row() {
        Text('取消')
          .fontSize(16)
          .fontColor('#666666')
          .onClick(() => {
            if (isStart) {
              this.showStartTimePicker = false
            } else {
              this.showEndTimePicker = false
            }
          })

        Blank()

        Text(isStart ? '选择开始时间' : '选择结束时间')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')

        Blank()

        Text('确定')
          .fontSize(16)
          .fontColor('#4ECDC4')
          .fontWeight(FontWeight.Medium)
          .onClick(() => {
            if (isStart) {
              this.confirmStartTime()
            } else {
              this.confirmEndTime()
            }
          })
      }
      .width('100%')
      .padding(16)

      TimePicker({
        selected: isStart ? new Date(this.scheduleStartTime) : new Date(this.scheduleEndTime)
      })
        .useMilitaryTime(true)
        .onChange((value: TimePickerResult) => {
          this.tempHour = value.hour
          this.tempMinute = value.minute
        })
        .height(200)
    }
    .width('90%')
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
  }

  build() {
    Stack() {
      Column() {
        this.TopBar()

        Scroll(this.scrollerForPage) {
          Column({ space: 12 }) {
            this.InputSection()
            this.TimeSection()
            this.LocationSection()
            this.TagSection()
            this.ReminderSection()
            this.RecordingSection()
            // 关联应用功能暂时禁用，待真机测试
            // this.ExternalLinkSection()
          }
          .padding({
            left: 16,
            right: 16,
            top: 16,
            bottom: 32
          })
        }
        .layoutWeight(1)
        .scrollable(ScrollDirection.Vertical)
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F8F9FA')

      // 开始日期选择器弹窗
      if (this.showStartDatePicker) {
        Column() {
          this.DatePickerDialog(true)
        }
        .width('100%')
        .height('100%')
        .backgroundColor('rgba(0, 0, 0, 0.5)')
        .justifyContent(FlexAlign.Center)
        .onClick(() => {
          this.showStartDatePicker = false
        })
      }

      // 结束日期选择器弹窗
      if (this.showEndDatePicker) {
        Column() {
          this.DatePickerDialog(false)
        }
        .width('100%')
        .height('100%')
        .backgroundColor('rgba(0, 0, 0, 0.5)')
        .justifyContent(FlexAlign.Center)
        .onClick(() => {
          this.showEndDatePicker = false
        })
      }

      // 开始时间选择器弹窗
      if (this.showStartTimePicker) {
        Column() {
          this.TimePickerDialog(true)
        }
        .width('100%')
        .height('100%')
        .backgroundColor('rgba(0, 0, 0, 0.5)')
        .justifyContent(FlexAlign.Center)
        .onClick(() => {
          this.showStartTimePicker = false
        })
      }

      // 结束时间选择器弹窗
      if (this.showEndTimePicker) {
        Column() {
          this.TimePickerDialog(false)
        }
        .width('100%')
        .height('100%')
        .backgroundColor('rgba(0, 0, 0, 0.5)')
        .justifyContent(FlexAlign.Center)
        .onClick(() => {
          this.showEndTimePicker = false
        })
      }
    }
    .width('100%')
    .height('100%')
  }
}
