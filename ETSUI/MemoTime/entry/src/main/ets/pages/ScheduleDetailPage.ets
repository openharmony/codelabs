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
 * 日程详情页面
 */
import { router } from '@kit.ArkUI'
import { common, Want } from '@kit.AbilityKit'
import { AppConstants } from '../common/constants/Constants'
import { formatDateChinese, formatTime, getWeekDayName } from '../common/utils/DateUtils'
import { Logger } from '../common/utils/Logger'
import { Schedule } from '../model/Schedule'
import { Reminder } from '../model/Reminder'
import { Recording } from '../model/Recording'
import { Tag } from '../model/Tag'
import { dataService } from '../service/DataService'
import { RecordingList } from '../components/recording/RecordingList'

const TAG = 'ScheduleDetailPage'

interface RouterParams {
  scheduleId: string
}

interface EditParams {
  scheduleId: string
}

@Entry
@Component
struct ScheduleDetailPage {
  @State scheduleId: string = ''
  @State scheduleTitle: string = ''
  @State scheduleDescription: string = ''
  @State scheduleStartTime: number = 0
  @State scheduleEndTime: number = 0
  @State scheduleLocation: string = ''
  @State scheduleColor: string = '#4ECDC4'
  @State scheduleIsAllDay: boolean = false
  @State scheduleExternalAppLink: string = ''
  @State scheduleTagIds: string[] = []
  @State reminders: Reminder[] = []
  @State recordings: Recording[] = []
  @State tags: Tag[] = []
  @State isLoading: boolean = true
  @State showDeleteDialog: boolean = false
  @State hasSchedule: boolean = false

  aboutToAppear(): void {
    const params = router.getParams() as RouterParams
    if (params?.scheduleId) {
      this.loadScheduleDetails(params.scheduleId)
    }
  }

  /**
   * 加载日程详情
   */
  async loadScheduleDetails(scheduleId: string): Promise<void> {
    this.isLoading = true
    try {
      const schedule = await dataService.getScheduleById(scheduleId)
      if (schedule) {
        this.hasSchedule = true
        this.scheduleId = schedule.id
        this.scheduleTitle = schedule.title
        this.scheduleDescription = schedule.description
        this.scheduleStartTime = schedule.startTime
        this.scheduleEndTime = schedule.endTime
        this.scheduleLocation = schedule.location
        this.scheduleColor = schedule.color
        this.scheduleIsAllDay = schedule.isAllDay
        this.scheduleExternalAppLink = schedule.externalAppLink
        this.scheduleTagIds = schedule.tagIds

        // 加载关联的提醒
        this.reminders = await dataService.getRemindersByScheduleId(scheduleId)
        // 加载关联的录音
        this.recordings = await dataService.getRecordingsByScheduleId(scheduleId)
        // 加载标签
        this.tags = await dataService.getAllTags()
      }
    } catch (error) {
      Logger.error(TAG, `Failed to load schedule details: ${JSON.stringify(error)}`)
    }
    this.isLoading = false
  }

  /**
   * 获取标签名称
   */
  getTagName(tagId: string): string {
    const tag = this.tags.find((t: Tag) => t.id === tagId)
    return tag ? tag.name : ''
  }

  /**
   * 获取标签颜色
   */
  getTagColor(tagId: string): string {
    const tag = this.tags.find((t: Tag) => t.id === tagId)
    return tag ? tag.color : '#999999'
  }

  /**
   * 编辑日程
   */
  editSchedule(): void {
    if (this.hasSchedule) {
      const params: EditParams = {
        scheduleId: this.scheduleId
      }
      router.pushUrl({
        url: AppConstants.PAGE_SCHEDULE_EDIT,
        params: params
      })
    }
  }

  /**
   * 删除日程
   */
  async deleteSchedule(): Promise<void> {
    if (this.hasSchedule) {
      try {
        await dataService.deleteSchedule(this.scheduleId)
        Logger.info(TAG, `Schedule deleted: ${this.scheduleId}`)
        router.back()
      } catch (error) {
        Logger.error(TAG, `Failed to delete schedule: ${JSON.stringify(error)}`)
      }
    }
  }

  /**
   * 打开外部应用
   * 支持的链接格式：
   * - contacts:// 联系人
   * - tel:电话号码 拨打电话
   * - sms:电话号码 发送短信
   * - mailto:邮箱 发送邮件
   * - https://网址 打开浏览器
   * - geo:经度,纬度 打开地图
   */
  async openExternalApp(): Promise<void> {
    if (!this.scheduleExternalAppLink) {
      return
    }

    try {
      const context = getContext(this) as common.UIAbilityContext
      const want: Want = {
        uri: this.scheduleExternalAppLink
      }

      // 对于特定的 scheme，可以添加额外的处理
      const link = this.scheduleExternalAppLink.toLowerCase()
      if (link.startsWith('tel:')) {
        want.action = 'ohos.want.action.call'
      } else if (link.startsWith('sms:')) {
        want.action = 'ohos.want.action.sendSms'
      } else if (link.startsWith('mailto:')) {
        want.action = 'ohos.want.action.sendEmail'
      } else if (link.startsWith('http://') || link.startsWith('https://')) {
        want.action = 'ohos.want.action.viewData'
      }

      await context.startAbility(want)
      Logger.info(TAG, `Opened external app: ${this.scheduleExternalAppLink}`)
    } catch (error) {
      Logger.error(TAG, `Failed to open external app: ${this.scheduleExternalAppLink} ${JSON.stringify(error)}`)
      // 可以在这里添加用户提示
    }
  }

  @Builder
  TopBar() {
    Row() {
      Image($r('sys.media.ohos_ic_public_arrow_left'))
        .width(24)
        .height(24)
        .fillColor('#333333')
        .onClick(() => router.back())

      Blank()

      // 编辑按钮
      Image($r('sys.media.ohos_ic_public_edit'))
        .width(24)
        .height(24)
        .fillColor('#666666')
        .margin({ right: 16 })
        .onClick(() => this.editSchedule())

      // 删除按钮
      Text('删除')
        .fontSize(14)
        .fontColor('#FF6B6B')
        .onClick(() => {
          this.showDeleteDialog = true
        })
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .backgroundColor('#FFFFFF')
  }

  @Builder
  HeaderSection() {
    Column() {
      // 颜色条
      Row()
        .width('100%')
        .height(6)
        .backgroundColor(this.scheduleColor)
        .borderRadius({ topLeft: 12, topRight: 12 })

      Column() {
        // 标题
        Text(this.scheduleTitle)
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .width('100%')

        // 时间信息
        Row() {
          Text('🕐')
            .fontSize(14)

          if (this.scheduleIsAllDay) {
            Text(`${formatDateChinese(new Date(this.scheduleStartTime))} 全天`)
              .fontSize(14)
              .fontColor('#666666')
              .margin({ left: 8 })
          } else {
            Text(`${formatDateChinese(new Date(this.scheduleStartTime))} ${formatTime(new Date(this.scheduleStartTime))} - ${formatTime(new Date(this.scheduleEndTime))}`)
              .fontSize(14)
              .fontColor('#666666')
              .margin({ left: 8 })
          }
        }
        .margin({ top: 12 })

        // 星期几
        Text(`星期${getWeekDayName(new Date(this.scheduleStartTime).getDay())}`)
          .fontSize(13)
          .fontColor('#999999')
          .margin({ top: 4, left: 24 })
      }
      .width('100%')
      .padding(16)
    }
    .width('100%')
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
  }

  @Builder
  InfoSection() {
    Column({ space: 16 }) {
      // 地点
      if (this.scheduleLocation) {
        Row() {
          Text('📍')
            .fontSize(16)

          Text(this.scheduleLocation)
            .fontSize(15)
            .fontColor('#333333')
            .margin({ left: 12 })
            .layoutWeight(1)
        }
        .width('100%')
        .padding(16)
        .backgroundColor('#FFFFFF')
        .borderRadius(12)
      }

      // 描述
      if (this.scheduleDescription) {
        Column() {
          Row() {
            Text('📝')
              .fontSize(16)

            Text('描述')
              .fontSize(15)
              .fontWeight(FontWeight.Medium)
              .fontColor('#333333')
              .margin({ left: 12 })
          }

          Text(this.scheduleDescription)
            .fontSize(14)
            .fontColor('#666666')
            .margin({ top: 12 })
            .width('100%')
        }
        .width('100%')
        .padding(16)
        .backgroundColor('#FFFFFF')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)
      }

      // 标签
      if (this.scheduleTagIds.length > 0) {
        Column() {
          Row() {
            Text('🏷️')
              .fontSize(16)

            Text('标签')
              .fontSize(15)
              .fontWeight(FontWeight.Medium)
              .fontColor('#333333')
              .margin({ left: 12 })
          }

          Row({ space: 8 }) {
            ForEach(this.scheduleTagIds, (tagId: string) => {
              Row() {
                Circle()
                  .width(10)
                  .height(10)
                  .fill(this.getTagColor(tagId))

                Text(this.getTagName(tagId))
                  .fontSize(13)
                  .fontColor('#666666')
                  .margin({ left: 6 })
              }
              .padding({ left: 10, right: 10, top: 6, bottom: 6 })
              .backgroundColor('#F5F5F5')
              .borderRadius(16)
            }, (tagId: string) => tagId)
          }
          .width('100%')
          .margin({ top: 12 })
        }
        .width('100%')
        .padding(16)
        .backgroundColor('#FFFFFF')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)
      }

      // 提醒
      if (this.reminders.length > 0) {
        Column() {
          Row() {
            Text('🔔')
              .fontSize(16)

            Text('提醒')
              .fontSize(15)
              .fontWeight(FontWeight.Medium)
              .fontColor('#333333')
              .margin({ left: 12 })
          }

          Column({ space: 8 }) {
            ForEach(this.reminders, (reminder: Reminder) => {
              Text(reminder.getTimingText())
                .fontSize(14)
                .fontColor('#666666')
            }, (reminder: Reminder) => reminder.id)
          }
          .margin({ top: 12 })
          .alignItems(HorizontalAlign.Start)
        }
        .width('100%')
        .padding(16)
        .backgroundColor('#FFFFFF')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)
      }

      // 录音附件
      if (this.recordings.length > 0) {
        Column() {
          Row() {
            Text('🎤')
              .fontSize(16)

            Text('语音笔记')
              .fontSize(15)
              .fontWeight(FontWeight.Medium)
              .fontColor('#333333')
              .margin({ left: 12 })

            Blank()

            Text(`${this.recordings.length} 条`)
              .fontSize(13)
              .fontColor('#999999')
          }
          .width('100%')
          .margin({ bottom: 12 })

          RecordingList({
            recordings: this.recordings,
            onDelete: () => {}  // 详情页不允许删除，需到编辑页操作
          })
        }
        .width('100%')
        .padding(16)
        .backgroundColor('#FFFFFF')
        .borderRadius(12)
        .alignItems(HorizontalAlign.Start)
      }

      // 外部链接（功能暂时禁用，待真机测试）
      // if (this.scheduleExternalAppLink) {
      //   Row() {
      //     Text('🔗')
      //       .fontSize(16)

      //     Column() {
      //       Text('关联应用')
      //         .fontSize(15)
      //         .fontWeight(FontWeight.Medium)
      //         .fontColor('#333333')

      //       Text(this.scheduleExternalAppLink)
      //         .fontSize(13)
      //         .fontColor('#666666')
      //         .margin({ top: 4 })
      //     }
      //     .margin({ left: 12 })
      //     .alignItems(HorizontalAlign.Start)
      //     .layoutWeight(1)

      //     Image($r('sys.media.ohos_ic_public_arrow_right'))
      //       .width(16)
      //       .height(16)
      //       .fillColor('#CCCCCC')
      //   }
      //   .width('100%')
      //   .padding(16)
      //   .backgroundColor('#FFFFFF')
      //   .borderRadius(12)
      //   .onClick(() => this.openExternalApp())
      // }
    }
  }

  @Builder
  LoadingView() {
    Column() {
      LoadingProgress()
        .width(48)
        .height(48)
        .color('#4ECDC4')

      Text('加载中...')
        .fontSize(14)
        .fontColor('#999999')
        .margin({ top: 16 })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }

  @Builder
  DeleteConfirmDialog() {
    Column() {
      Text('确认删除')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ bottom: 16 })

      Text('确定要删除这个日程吗？此操作不可恢复。')
        .fontSize(14)
        .fontColor('#666666')
        .textAlign(TextAlign.Center)
        .margin({ bottom: 24 })

      Row({ space: 16 }) {
        Button('取消')
          .fontSize(16)
          .fontColor('#666666')
          .backgroundColor('#F5F5F5')
          .layoutWeight(1)
          .height(44)
          .onClick(() => {
            this.showDeleteDialog = false
          })

        Button('删除')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .backgroundColor('#FF6B6B')
          .layoutWeight(1)
          .height(44)
          .onClick(() => {
            this.showDeleteDialog = false
            this.deleteSchedule()
          })
      }
      .width('100%')
    }
    .width('80%')
    .padding(24)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
  }

  build() {
    Stack() {
      Column() {
        this.TopBar()

        if (this.isLoading) {
          this.LoadingView()
        } else if (this.hasSchedule) {
          Scroll() {
            Column({ space: 12 }) {
              this.HeaderSection()
              this.InfoSection()
            }
            .padding({ left: 16, right: 16, top: 16, bottom: 32 })
          }
          .layoutWeight(1)
          .scrollable(ScrollDirection.Vertical)
        } else {
          Column() {
            Text('日程不存在')
              .fontSize(16)
              .fontColor('#999999')
          }
          .layoutWeight(1)
          .justifyContent(FlexAlign.Center)
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F8F9FA')

      // 删除确认弹窗
      if (this.showDeleteDialog) {
        Column() {
          // 空白区域点击关闭
        }
        .width('100%')
        .height('100%')
        .backgroundColor('rgba(0, 0, 0, 0.5)')
        .position({ x: 0, y: 0 })
        .onClick(() => {
          this.showDeleteDialog = false
        })

        Column() {
          this.DeleteConfirmDialog()
        }
        .position({ x: '10%', y: '35%' })
      }
    }
    .width('100%')
    .height('100%')
  }
}
