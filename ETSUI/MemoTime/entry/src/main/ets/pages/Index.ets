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
 * MemoTime 主页面 - 日历视图入口
 */
import { CalendarViewType, AppConstants, ReminderTiming } from '../common/constants/Constants'
import { formatDate, formatDateChinese, addMonths, addYears, getMonthName } from '../common/utils/DateUtils'
import { Logger } from '../common/utils/Logger'
import { Schedule, ScheduleInit } from '../model/Schedule'
import { Reminder, ReminderInit } from '../model/Reminder'
import { DataService, dataService } from '../service/DataService'
import { notificationService } from '../service/NotificationService'
import { MonthView } from '../components/calendar/MonthView'
import { WeekView } from '../components/calendar/WeekView'
import { DayView } from '../components/calendar/DayView'
import { YearView } from '../components/calendar/YearView'
import { ScheduleList } from '../components/schedule/ScheduleList'
import { FloatingButton } from '../components/common/FloatingButton'
import { router } from '@kit.ArkUI'

const TAG = 'IndexPage'

/**
 * 路由参数接口
 */
interface ScheduleEditParams {
  date?: number
  scheduleId?: string
}

interface ScheduleDetailParams {
  scheduleId: string
}

/**
 * 视图切换项
 */
interface ViewTypeItem {
  type: CalendarViewType
  label: string
}

@Entry
@Component
struct Index {
  @State currentViewType: CalendarViewType = CalendarViewType.MONTH
  @State selectedDate: Date = new Date()
  @State currentYear: number = new Date().getFullYear()
  @State currentMonth: number = new Date().getMonth()
  @State schedules: Schedule[] = []
  @State selectedDateSchedules: Schedule[] = []
  @State isLoading: boolean = false

  // 响应式布局相关
  @State windowWidth: number = 360
  @State isLargeScreen: boolean = false

  // 视图类型列表
  private viewTypes: ViewTypeItem[] = [
    { type: CalendarViewType.DAY, label: '日' },
    { type: CalendarViewType.WEEK, label: '周' },
    { type: CalendarViewType.MONTH, label: '月' },
    { type: CalendarViewType.YEAR, label: '年' }
  ]

  private context = getContext(this)

  aboutToAppear(): void {
    this.initializeData()
  }

  aboutToDisappear(): void {
    // 应用退出时停止轮询
    notificationService.stopPolling()
  }

  onPageShow(): void {
    // 每次页面显示时重新加载日程数据（处理删除、编辑后返回的情况）
    this.loadSchedules()
  }

  /**
   * 初始化数据
   */
  async initializeData(): Promise<void> {
    this.isLoading = true
    try {
      await dataService.initialize(this.context)
      await notificationService.initialize()
      await this.loadSchedules()

      // 加载示例数据（开发阶段）
      await this.loadSampleData()
      // 检查待触发的提醒
      await notificationService.checkAndSendPendingReminders()

      // 启动轮询（每分钟检查一次提醒）
      notificationService.startPolling()
    } catch (error) {
      Logger.error(TAG, `Failed to initialize: ${JSON.stringify(error)}`)
    }
    this.isLoading = false
  }

  /**
   * 加载日程数据
   */
  async loadSchedules(): Promise<void> {
    try {
      this.schedules = await dataService.getAllSchedules()
      this.updateSelectedDateSchedules()
      Logger.info(TAG, `Loaded ${this.schedules.length} schedules`)
    } catch (error) {
      Logger.error(TAG, `Failed to load schedules: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 加载示例数据（开发阶段使用）
   */
  async loadSampleData(): Promise<void> {
    if (this.schedules.length > 0) {
      return // 已有数据，不需要加载示例
    }

    const today = new Date()

    const init1: ScheduleInit = {
      title: '团队周会',
      description: '讨论本周工作进展和下周计划',
      startTime: new Date(today.getFullYear(), today.getMonth(), today.getDate(), 10, 0).getTime(),
      endTime: new Date(today.getFullYear(), today.getMonth(), today.getDate(), 11, 30).getTime(),
      location: '3楼会议室A',
      color: '#4ECDC4',
      tagIds: ['tag-meeting']
    }

    const init2: ScheduleInit = {
      title: '项目评审',
      description: 'MemoTime 项目第一阶段评审',
      startTime: new Date(today.getFullYear(), today.getMonth(), today.getDate(), 14, 0).getTime(),
      endTime: new Date(today.getFullYear(), today.getMonth(), today.getDate(), 16, 0).getTime(),
      location: '线上腾讯会议',
      color: '#FF6B6B',
      tagIds: ['tag-work'],
      externalAppLink: 'tencentmeeting://'
    }

    const init3: ScheduleInit = {
      title: '健身运动',
      description: '有氧运动30分钟 + 力量训练30分钟',
      startTime: new Date(today.getFullYear(), today.getMonth(), today.getDate(), 18, 30).getTime(),
      endTime: new Date(today.getFullYear(), today.getMonth(), today.getDate(), 19, 30).getTime(),
      location: '公司健身房',
      color: '#96CEB4',
      tagIds: ['tag-sport']
    }

    const init4: ScheduleInit = {
      title: '生日派对',
      description: '准备蛋糕和礼物',
      startTime: new Date(today.getFullYear(), today.getMonth(), today.getDate() + 3, 19, 0).getTime(),
      endTime: new Date(today.getFullYear(), today.getMonth(), today.getDate() + 3, 21, 0).getTime(),
      location: '家',
      color: '#FFEAA7',
      tagIds: ['tag-birthday', 'tag-life']
    }

    const init5: ScheduleInit = {
      title: '年度体检',
      description: '记得空腹',
      startTime: new Date(today.getFullYear(), today.getMonth(), today.getDate() + 7, 8, 0).getTime(),
      endTime: new Date(today.getFullYear(), today.getMonth(), today.getDate() + 7, 12, 0).getTime(),
      location: '市第一人民医院',
      color: '#DDA0DD',
      tagIds: ['tag-life']
    }

    const sampleSchedules: Schedule[] = [
      new Schedule(init1),
      new Schedule(init2),
      new Schedule(init3),
      new Schedule(init4),
      new Schedule(init5)
    ]

    for (const schedule of sampleSchedules) {
      await dataService.insertSchedule(schedule)
      
      // 为每个日程创建提醒（提前15分钟）
      const reminderInit: ReminderInit = {
        scheduleId: schedule.id,
        timing: ReminderTiming.MINUTES_15
      }
      const reminder = new Reminder(reminderInit)
      reminder.calculateTriggerTime(schedule.startTime)
      await dataService.insertReminder(reminder)
    }

    await this.loadSchedules()
    Logger.info(TAG, 'Sample data loaded with reminders')
  }

  /**
   * 更新选中日期的日程列表
   */
  updateSelectedDateSchedules(): void {
    this.selectedDateSchedules = this.schedules.filter((s: Schedule) => s.isOnDate(this.selectedDate))
  }

  /**
   * 处理日期选择
   */
  handleDateSelect(date: Date): void {
    this.selectedDate = date
    this.currentYear = date.getFullYear()
    this.currentMonth = date.getMonth()
    this.updateSelectedDateSchedules()
  }

  /**
   * 处理月份点击（从年视图）
   */
  handleMonthClick(year: number, month: number): void {
    this.currentYear = year
    this.currentMonth = month
    this.currentViewType = CalendarViewType.MONTH
  }

  /**
   * 切换到上一个周期
   */
  goToPrevious(): void {
    switch (this.currentViewType) {
      case CalendarViewType.DAY:
        this.selectedDate = new Date(this.selectedDate.getTime() - 24 * 60 * 60 * 1000)
        this.currentYear = this.selectedDate.getFullYear()
        this.currentMonth = this.selectedDate.getMonth()
        break
      case CalendarViewType.WEEK:
        this.selectedDate = new Date(this.selectedDate.getTime() - 7 * 24 * 60 * 60 * 1000)
        this.currentYear = this.selectedDate.getFullYear()
        this.currentMonth = this.selectedDate.getMonth()
        break
      case CalendarViewType.MONTH:
        const prevMonth = addMonths(new Date(this.currentYear, this.currentMonth, 1), -1)
        this.currentYear = prevMonth.getFullYear()
        this.currentMonth = prevMonth.getMonth()
        break
      case CalendarViewType.YEAR:
        this.currentYear -= 1
        break
    }
    this.updateSelectedDateSchedules()
  }

  /**
   * 切换到下一个周期
   */
  goToNext(): void {
    switch (this.currentViewType) {
      case CalendarViewType.DAY:
        this.selectedDate = new Date(this.selectedDate.getTime() + 24 * 60 * 60 * 1000)
        this.currentYear = this.selectedDate.getFullYear()
        this.currentMonth = this.selectedDate.getMonth()
        break
      case CalendarViewType.WEEK:
        this.selectedDate = new Date(this.selectedDate.getTime() + 7 * 24 * 60 * 60 * 1000)
        this.currentYear = this.selectedDate.getFullYear()
        this.currentMonth = this.selectedDate.getMonth()
        break
      case CalendarViewType.MONTH:
        const nextMonth = addMonths(new Date(this.currentYear, this.currentMonth, 1), 1)
        this.currentYear = nextMonth.getFullYear()
        this.currentMonth = nextMonth.getMonth()
        break
      case CalendarViewType.YEAR:
        this.currentYear += 1
        break
    }
    this.updateSelectedDateSchedules()
  }

  /**
   * 回到今天
   */
  goToToday(): void {
    const today = new Date()
    this.selectedDate = today
    this.currentYear = today.getFullYear()
    this.currentMonth = today.getMonth()
    this.updateSelectedDateSchedules()
  }

  /**
   * 获取当前标题
   */
  getCurrentTitle(): string {
    switch (this.currentViewType) {
      case CalendarViewType.DAY:
        return formatDateChinese(this.selectedDate)
      case CalendarViewType.WEEK:
        return `${this.currentYear}年${this.currentMonth + 1}月`
      case CalendarViewType.MONTH:
        return `${this.currentYear}年${this.currentMonth + 1}月`
      case CalendarViewType.YEAR:
        return `${this.currentYear}年`
      default:
        return ''
    }
  }

  /**
   * 跳转到新建日程页面
   */
  navigateToAddSchedule(): void {
    const params: ScheduleEditParams = {
      date: this.selectedDate.getTime()
    }
    router.pushUrl({
      url: AppConstants.PAGE_SCHEDULE_EDIT,
      params: params
    }).catch((error: Error) => {
      Logger.error(TAG, `Navigation failed: ${error.message}`)
    })
  }

  /**
   * 跳转到日程详情页面
   */
  navigateToScheduleDetail(schedule: Schedule): void {
    const params: ScheduleDetailParams = {
      scheduleId: schedule.id
    }
    router.pushUrl({
      url: AppConstants.PAGE_SCHEDULE_DETAIL,
      params: params
    }).catch((error: Error) => {
      Logger.error(TAG, `Navigation failed: ${error.message}`)
    })
  }

  @Builder
  TopBar() {
    Row() {
      // 导航按钮
      Image($r('sys.media.ohos_ic_public_arrow_left'))
        .width(24)
        .height(24)
        .fillColor('#333333')
        .onClick(() => this.goToPrevious())

      // 标题
      Text(this.getCurrentTitle())
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ left: 16, right: 16 })
        .onClick(() => {
          // 点击标题切换视图
          if (this.currentViewType === CalendarViewType.MONTH) {
            this.currentViewType = CalendarViewType.YEAR
          } else if (this.currentViewType === CalendarViewType.YEAR) {
            this.currentViewType = CalendarViewType.MONTH
          }
        })

      Image($r('sys.media.ohos_ic_public_arrow_right'))
        .width(24)
        .height(24)
        .fillColor('#333333')
        .onClick(() => this.goToNext())

      Blank()

      // 今天按钮
      Text('今天')
        .fontSize(14)
        .fontColor('#4ECDC4')
        .fontWeight(FontWeight.Medium)
        .padding({ left: 12, right: 12, top: 6, bottom: 6 })
        .backgroundColor('#E8F8F5')
        .borderRadius(16)
        .onClick(() => this.goToToday())

      // 设置按钮
      Image($r('sys.media.ohos_ic_public_more'))
        .width(24)
        .height(24)
        .fillColor('#666666')
        .margin({ left: 16 })
        .onClick(() => {
          router.pushUrl({ url: AppConstants.PAGE_SETTINGS })
        })
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .backgroundColor('#FFFFFF')
  }

  @Builder
  ViewTypeTabs() {
    Row({ space: 8 }) {
      ForEach(this.viewTypes, (item: ViewTypeItem) => {
        Text(item.label)
          .fontSize(14)
          .fontColor(this.currentViewType === item.type ? '#FFFFFF' : '#666666')
          .fontWeight(FontWeight.Medium)
          .padding({ left: 16, right: 16, top: 8, bottom: 8 })
          .backgroundColor(this.currentViewType === item.type ? '#4ECDC4' : '#F5F5F5')
          .borderRadius(20)
          .onClick(() => {
            this.currentViewType = item.type
          })
      }, (item: ViewTypeItem) => item.type)
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 8, bottom: 8 })
    .justifyContent(FlexAlign.Center)
    .backgroundColor('#FFFFFF')
  }

  @Builder
  CalendarContent() {
    if (this.currentViewType === CalendarViewType.MONTH) {
      Column() {
        MonthView({
          year: this.currentYear,
          month: this.currentMonth,
          selectedDate: this.selectedDate,
          schedules: this.schedules,
          onDateSelect: (date: Date) => this.handleDateSelect(date),
          onScheduleClick: (schedule: Schedule) => this.navigateToScheduleDetail(schedule)
        })
          .margin({ left: 16, right: 16, top: 8 })

        // 选中日期的日程列表
        Column() {
          Row() {
            Text(formatDateChinese(this.selectedDate))
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .fontColor('#333333')

            Blank()

            Text(`${this.selectedDateSchedules.length} 项日程`)
              .fontSize(14)
              .fontColor('#999999')
          }
          .width('100%')
          .padding({ left: 16, right: 16, top: 16, bottom: 8 })

          ScheduleList({
            schedules: this.selectedDateSchedules,
            groupByDate: false,
            emptyText: '暂无日程安排',
            onScheduleClick: (schedule: Schedule) => this.navigateToScheduleDetail(schedule)
          })
        }
        .layoutWeight(1)
      }
      .width('100%')
      .height('100%')
    } else if (this.currentViewType === CalendarViewType.WEEK) {
      WeekView({
        currentDate: this.selectedDate,
        selectedDate: this.selectedDate,
        schedules: this.schedules,
        onDateSelect: (date: Date) => this.handleDateSelect(date),
        onScheduleClick: (schedule: Schedule) => this.navigateToScheduleDetail(schedule),
        onTimeSlotClick: (date: Date, hour: number) => {
          // 点击时间槽创建新日程
          this.selectedDate = new Date(date.getFullYear(), date.getMonth(), date.getDate(), hour, 0)
          this.navigateToAddSchedule()
        }
      })
        .padding({ left: 16, right: 16, top: 8 })
    } else if (this.currentViewType === CalendarViewType.DAY) {
      DayView({
        currentDate: this.selectedDate,
        schedules: this.schedules,
        onScheduleClick: (schedule: Schedule) => this.navigateToScheduleDetail(schedule),
        onTimeSlotClick: (date: Date, hour: number) => {
          this.selectedDate = new Date(date.getFullYear(), date.getMonth(), date.getDate(), hour, 0)
          this.navigateToAddSchedule()
        }
      })
        .padding({ left: 16, right: 16, top: 8 })
    } else if (this.currentViewType === CalendarViewType.YEAR) {
      YearView({
        year: this.currentYear,
        schedules: this.schedules,
        onMonthClick: (year: number, month: number) => this.handleMonthClick(year, month)
      })
    }
  }

  build() {
    Stack({ alignContent: Alignment.BottomEnd }) {
      Column() {
        this.TopBar()
        this.ViewTypeTabs()

        // 日历内容区
        Column() {
          this.CalendarContent()
        }
        .layoutWeight(1)
        .backgroundColor('#F8F9FA')
      }
      .width('100%')
      .height('100%')

      // 悬浮添加按钮
      FloatingButton({
        iconRes: $r('sys.media.ohos_ic_public_add'),
        onBtnClick: () => this.navigateToAddSchedule()
      })
        .margin({ right: 24, bottom: 24 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
  }
}
