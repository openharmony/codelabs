/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import promptAction from '@ohos.promptAction';
import common from '@ohos.app.ability.common';
import { Event, EventModel } from '../model/EventModel';
import { NotificationService } from '../services/NotificationService';

@Component
export struct EventEditPage {
  // 接收全局路由栈
  @Consume('pageStack') pageStack: NavPathStack;

  // 接收参数
  @State eventItem: Event | undefined = undefined;

  // 表单状态
  @State eventId: number = 0;
  @State title: string = '';
  @State description: string = '';
  @State isAllDay: boolean = false;
  @State startTime: Date = new Date();
  @State endTime: Date = new Date(new Date().getTime() + 60 * 60 * 1000);

  // 提醒相关状态
  @State isReminderOn: boolean = false;
  private oldReminderId: number = -1;

  aboutToAppear() {
    if (this.eventItem && this.eventItem.id) {
      this.eventId = this.eventItem.id;
      this.title = this.eventItem.title;
      this.description = this.eventItem.description;
      this.isAllDay = this.eventItem.isAllDay;
      this.startTime = new Date(this.eventItem.startTime);
      this.endTime = new Date(this.eventItem.endTime);

      // 初始化提醒状态
      this.oldReminderId = this.eventItem.reminderId;
      this.isReminderOn = this.oldReminderId > -1;
    }
  }

  private formatDate(date: Date): string {
    const y = date.getFullYear();
    const m = date.getMonth() + 1;
    const d = date.getDate();
    return `${y}/${m}/${d}`;
  }

  private formatTime(date: Date): string {
    const h = date.getHours().toString().padStart(2, '0');
    const m = date.getMinutes().toString().padStart(2, '0');
    return `${h}:${m}`;
  }

  private pickDate(isStart: boolean) {
    DatePickerDialog.show({
      start: new Date("2020-1-1"),
      end: new Date("2030-12-31"),
      selected: isStart ? this.startTime : this.endTime,
      onAccept: (value: DatePickerResult) => {
        const targetDate = isStart ? this.startTime : this.endTime;
        const newDate = new Date(targetDate.getTime());
        newDate.setFullYear(value.year, value.month, value.day);
        if (isStart) this.startTime = new Date(newDate.getTime());
        else this.endTime = new Date(newDate.getTime());
      }
    })
  }

  private pickTime(isStart: boolean) {
    TimePickerDialog.show({
      selected: isStart ? this.startTime : this.endTime,
      useMilitaryTime: true,
      onAccept: (value: TimePickerResult) => {
        const targetDate = isStart ? this.startTime : this.endTime;
        const newDate = new Date(targetDate.getTime());
        newDate.setHours(value.hour, value.minute);
        if (isStart) this.startTime = new Date(newDate.getTime());
        else this.endTime = new Date(newDate.getTime());
      }
    })
  }

  async saveEvent() {
    if (this.title.trim() === '') {
      promptAction.showToast({ message: $r('app.string.toast_input_error') });
      return;
    }

    // 1. 如果之前有旧提醒，先取消（防止重复或僵尸提醒）
    if (this.oldReminderId > -1) {
      await NotificationService.cancelReminder(this.oldReminderId);
    }

    let newReminderId = -1;

    // 2. 如果开启了新提醒
    if (this.isReminderOn) {
      const now = new Date().getTime();
      if (this.startTime.getTime() > now) {
        // 获取 Context 并检查权限
        const context = getContext(this) as common.UIAbilityContext;
        const hasPermission = await NotificationService.checkAndRequestPermission(context);

        if (hasPermission) {
          // ✅ 修改：传入 context
          newReminderId = await NotificationService.publishCalendarReminder(
            context,
            this.title,
            this.description || "您有一个日程待办",
            this.startTime.getTime()
          );
        } else {
          promptAction.showToast({ message: '通知权限被拒绝，将仅保存日程' });
        }
      } else {
        promptAction.showToast({ message: '开始时间已过，无法设置提醒' });
        // 继续保存日程，但不带提醒
      }
    }

    // 3. 构建对象
    const event = new Event(
      this.title,
      this.startTime.getTime(),
      this.endTime.getTime(),
      this.description,
      this.isAllDay,
      newReminderId // 存入 ID
    );
    event.id = this.eventId;

    // 4. 存入数据库
    let result = 0;
    if (this.eventId > 0) {
      result = await EventModel.update(event);
    } else {
      result = await EventModel.insert(event);
    }

    if (result >= 0) {
      promptAction.showToast({ message: $r('app.string.toast_save_success') });
      // 通知列表刷新
      AppStorage.setOrCreate('refreshEventList', Date.now());
      // 返回上一页
      this.pageStack.pop();
    }
  }

  build() {
    NavDestination() {
      Column() {
        // --- 自定义头部 ---
        Row() {
          Image($r('app.media.ic_back'))
            .width(24).height(24).margin({ right: 16 })
            .onClick(() => { this.pageStack.pop(); })

          Text(this.eventId > 0 ? "编辑日程" : $r('app.string.event_btn_add'))
            .fontSize(20).fontWeight(FontWeight.Bold).layoutWeight(1)

          Button($r('app.string.btn_save'))
            .height(32).fontSize(14)
            .onClick(() => { this.saveEvent() })
        }
        .width('100%').height(56).padding({ left: 16, right: 16 }).backgroundColor(Color.White)

        // --- 表单内容 ---
        Scroll() {
          GridRow({
            columns: { sm: 4, md: 8, lg: 12 },
            gutter: { x: 12, y: 12 }
          }) {
            GridCol({ span: { sm: 4, md: 6, lg: 8 }, offset: { sm: 0, md: 1, lg: 2 } }) {
              Column({ space: 12 }) {
                // 标题
                TextInput({ text: this.title, placeholder: $r('app.string.label_event_title') })
                  .height(48).backgroundColor(Color.White).borderRadius(12)
                  .onChange((val) => this.title = val)

                // 全天开关
                Row() {
                  Text($r('app.string.label_all_day')).fontSize(16)
                  Toggle({ type: ToggleType.Switch, isOn: this.isAllDay })
                    .onChange((isOn) => this.isAllDay = isOn)
                }.width('100%').justifyContent(FlexAlign.SpaceBetween).padding(12).backgroundColor(Color.White).borderRadius(12)

                // 提醒开关
                Row() {
                  Text("开启提醒").fontSize(16)
                  Toggle({ type: ToggleType.Switch, isOn: this.isReminderOn })
                    .onChange((isOn) => this.isReminderOn = isOn)
                }.width('100%').justifyContent(FlexAlign.SpaceBetween).padding(12).backgroundColor(Color.White).borderRadius(12)

                // 开始时间
                Row() {
                  Text($r('app.string.label_start_time')).fontSize(16)
                  Text(this.formatDate(this.startTime)).fontSize(16).fontColor('#007DFF')
                }.width('100%').justifyContent(FlexAlign.SpaceBetween).padding(16).backgroundColor(Color.White).borderRadius(12)
                .onClick(() => { this.pickDate(true); })

                if (!this.isAllDay) {
                  Row() {
                    Text(" ").fontSize(16)
                    Text(this.formatTime(this.startTime)).fontSize(16).fontColor('#007DFF')
                  }.width('100%').justifyContent(FlexAlign.SpaceBetween).padding(16).backgroundColor(Color.White).borderRadius(12)
                  .onClick(() => { this.pickTime(true); })
                }

                // 结束时间
                Row() {
                  Text($r('app.string.label_end_time')).fontSize(16)
                  Text(this.formatDate(this.endTime)).fontSize(16).fontColor('#007DFF')
                }.width('100%').justifyContent(FlexAlign.SpaceBetween).padding(16).backgroundColor(Color.White).borderRadius(12)
                .onClick(() => { this.pickDate(false); })

                if (!this.isAllDay) {
                  Row() {
                    Text(" ").fontSize(16)
                    Text(this.formatTime(this.endTime)).fontSize(16).fontColor('#007DFF')
                  }.width('100%').justifyContent(FlexAlign.SpaceBetween).padding(16).backgroundColor(Color.White).borderRadius(12)
                  .onClick(() => { this.pickTime(false); })
                }

                // 描述
                TextArea({ text: this.description, placeholder: $r('app.string.label_event_desc') })
                  .height(120).backgroundColor(Color.White).borderRadius(12)
                  .onChange((val) => this.description = val)
              }
              .width('100%').padding({ top: 12, bottom: 24 })
            }
          }
        }
        .layoutWeight(1).width('100%').padding({ left: 16, right: 16 }).backgroundColor('#F1F3F5')
      }
      .width('100%').height('100%').backgroundColor('#F1F3F5')
    }
    .hideTitleBar(true)
  }
}