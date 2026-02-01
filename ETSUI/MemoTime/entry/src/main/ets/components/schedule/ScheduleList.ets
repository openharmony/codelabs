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
 * 日程列表组件
 */
import { Schedule } from '../../model/Schedule'
import { ScheduleCard } from './ScheduleCard'
import { formatDateChinese } from '../../common/utils/DateUtils'

/**
 * 分组日程数据
 */
interface ScheduleGroup {
  date: Date
  dateStr: string
  schedules: Schedule[]
}

@Component
export struct ScheduleList {
  @Prop @Watch('onSchedulesChange') schedules: Schedule[] = []
  @Prop groupByDate: boolean = true
  @Prop emptyText: string = '暂无日程'
  onScheduleClick: (schedule: Schedule) => void = () => {
  }
  onScheduleEdit: (schedule: Schedule) => void = () => {
  }
  onScheduleDelete: (schedule: Schedule) => void = () => {
  }
  @State groupedSchedules: ScheduleGroup[] = []
  @State refreshKey: number = 0

  aboutToAppear(): void {
    this.buildGroupedSchedules()
  }

  onSchedulesChange(): void {
    this.buildGroupedSchedules()
    this.refreshKey++
  }

  /**
   * 按日期分组日程
   */
  private buildGroupedSchedules(): void {
    if (!this.groupByDate) {
      if (this.schedules.length > 0) {
        const group: ScheduleGroup = {
          date: new Date(),
          dateStr: 'nogroup',
          schedules: this.schedules
        }
        this.groupedSchedules = [group]
      } else {
        this.groupedSchedules = []
      }
      return
    }

    const groups: Map<string, ScheduleGroup> = new Map()

    for (const schedule of this.schedules) {
      const startDate = new Date(schedule.startTime)
      const dateStr = formatDateChinese(startDate)

      if (!groups.has(dateStr)) {
        const newGroup: ScheduleGroup = {
          date: startDate,
          dateStr: dateStr,
          schedules: []
        }
        groups.set(dateStr, newGroup)
      }
      const group = groups.get(dateStr)
      if (group) {
        group.schedules.push(schedule)
      }
    }

    // 按日期排序
    const result: ScheduleGroup[] = []
    groups.forEach((value: ScheduleGroup) => {
      result.push(value)
    })
    result.sort((a: ScheduleGroup, b: ScheduleGroup) => a.date.getTime() - b.date.getTime())
    this.groupedSchedules = result
  }

  @Builder
  EmptyView() {
    Column() {
      Text('📅')
        .fontSize(48)
        .margin({ bottom: 16 })

      Text(this.emptyText)
        .fontSize(16)
        .fontColor('#999999')
    }
    .width('100%')
    .height(200)
    .justifyContent(FlexAlign.Center)
  }

  @Builder
  DateHeader(dateStr: string) {
    Row() {
      Text(dateStr)
        .fontSize(14)
        .fontWeight(FontWeight.Medium)
        .fontColor('#666666')

      Blank()
    }
    .width('100%')
    .padding({
      left: 4,
      right: 4,
      top: 12,
      bottom: 8
    })
  }

  @Builder
  ScheduleGroupView(group: ScheduleGroup) {
    Column() {
      if (this.groupByDate && group.dateStr && group.dateStr !== 'nogroup') {
        this.DateHeader(group.dateStr)
      }

      Column({ space: 12 }) {
        ForEach(group.schedules, (schedule: Schedule) => {
          ScheduleCard({
            scheduleData: schedule,
            showDate: !this.groupByDate,
            onCardClick: this.onScheduleClick,
            onEditClick: this.onScheduleEdit,
            onDeleteClick: this.onScheduleDelete
          })
        }, (schedule: Schedule) => `${schedule.id}-${this.refreshKey}`)
      }
    }
    .width('100%')
  }

  build() {
    if (this.schedules.length === 0) {
      this.EmptyView()
    } else {
      Scroll() {
        Column({ space: 8 }) {
          ForEach(this.groupedSchedules, (group: ScheduleGroup) => {
            this.ScheduleGroupView(group)
          }, (group: ScheduleGroup) => `${group.dateStr}-${this.refreshKey}`)
        }
        .padding({
          left: 16,
          right: 16,
          top: 8,
          bottom: 16
        })
      }
      .scrollable(ScrollDirection.Vertical)
      .edgeEffect(EdgeEffect.Spring)
      .width('100%')
      .height('100%')
    }
  }
}
