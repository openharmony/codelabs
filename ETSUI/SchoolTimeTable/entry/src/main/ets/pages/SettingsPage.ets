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
import { TimeSlot, AppSettings } from '../model/CourseModel';
import { CourseDataService } from '../service/CourseDataService';
import common from '@ohos.app.ability.common';
import { router } from '@kit.ArkUI';
import promptAction from '@ohos.promptAction';

interface SelectOption {
  value: string;
}

interface ValidationResult {
  valid: boolean;
  message: string;
}

@CustomDialog
struct DeleteConfirmDialog {
  controller?: CustomDialogController;
  section: number = 1;
  onConfirm: () => void = () => {
  };

  build() {
    Column() {
      Text('确认删除')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ bottom: 16 })

      Text(`确定要删除第${this.section}节吗？`)
        .fontSize(16)
        .fontColor('#666666')
        .margin({ bottom: 24 })

      Row() {
        Button('取消')
          .fontSize(16)
          .fontColor('#666666')
          .backgroundColor('#F5F5F5')
          .borderRadius(20)
          .layoutWeight(1)
          .margin({ right: 8 })
          .onClick(() => {
            if (this.controller) {
              this.controller.close();
            }
          })

        Button('删除')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .backgroundColor('#FF6B6B')
          .borderRadius(20)
          .layoutWeight(1)
          .margin({ left: 8 })
          .onClick(() => {
            this.onConfirm();
            if (this.controller) {
              this.controller.close();
            }
          })
      }
      .width('100%')
    }
    .width(280)
    .padding(24)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
  }
}

@CustomDialog
struct TimeSlotDialog {
  controller?: CustomDialogController;
  timeSlot: TimeSlot = { section: 1, startTime: '08:00', endTime: '08:45' };
  onSave: (timeSlot: TimeSlot) => void = (timeSlot: TimeSlot) => {
  };
  existingTimeSlots: TimeSlot[] = [];
  isNew: boolean = false;
  @State selectedSection: number = 1;
  @State selectedStartHour: number = 8;
  @State selectedStartMinute: number = 0;
  @State selectedEndHour: number = 8;
  @State selectedEndMinute: number = 45;
  @State errorMessage: string = '';
  @State successMessage: string = '';
  private hours: SelectOption[] = [];
  private minutes: SelectOption[] = [];
  private sections: SelectOption[] = [];

  aboutToAppear(): void {
    this.errorMessage = '';
    this.successMessage = '';

    this.hours = Array.from({ length: 24 }, (_: Object, i: number): SelectOption => {
      return { value: i.toString().padStart(2, '0') };
    });
    // Changed to 0-59 for continuous time setting
    this.minutes = Array.from({ length: 60 }, (_: Object, i: number): SelectOption => {
      return { value: i.toString().padStart(2, '0') };
    });

    const maxSection = this.existingTimeSlots.length > 0
      ? Math.max(...this.existingTimeSlots.map(t => t.section))
      : 0;

    this.sections = Array.from({ length: maxSection + 1 }, (_: Object, i: number): SelectOption => {
      return { value: i.toString() };
    });

    if (this.isNew) {
      this.selectedSection = 1;
    } else {
      this.selectedSection = this.timeSlot.section;
    }

    const startParts = this.timeSlot.startTime.split(':');
    this.selectedStartHour = parseInt(startParts[0]);
    this.selectedStartMinute = parseInt(startParts[1]);

    const endParts = this.timeSlot.endTime.split(':');
    this.selectedEndHour = parseInt(endParts[0]);
    this.selectedEndMinute = parseInt(endParts[1]);
  }

  private formatTime(hour: number, minute: number): string {
    return `${hour.toString().padStart(2, '0')}:${minute.toString().padStart(2, '0')}`;
  }

  private validateTime(): ValidationResult {
    const startTime = this.formatTime(this.selectedStartHour, this.selectedStartMinute);
    const endTime = this.formatTime(this.selectedEndHour, this.selectedEndMinute);

    if (this.selectedStartHour >= this.selectedEndHour) {
      if (this.selectedStartHour === this.selectedEndHour && this.selectedStartMinute < this.selectedEndMinute) {
      } else {
        const result: ValidationResult = { valid: false, message: '结束时间必须晚于开始时间' };
        return result;
      }
    }

    const startMinutes = this.selectedStartHour * 60 + this.selectedStartMinute;
    const endMinutes = this.selectedEndHour * 60 + this.selectedEndMinute;

    for (const slot of this.existingTimeSlots) {
      if (slot.section === this.timeSlot.section) {
        continue;
      }

      const slotStartParts = slot.startTime.split(':').map(Number);
      const slotEndParts = slot.endTime.split(':').map(Number);
      const slotStartMinutes = slotStartParts[0] * 60 + slotStartParts[1];
      const slotEndMinutes = slotEndParts[0] * 60 + slotEndParts[1];

      if (!(endMinutes <= slotStartMinutes || startMinutes >= slotEndMinutes)) {
        const result: ValidationResult = { valid: false, message: `与第${slot.section}节时间冲突` };
        return result;
      }
    }

    const result: ValidationResult = { valid: true, message: '' };
    return result;
  }

  build() {
    Column() {
      Text(this.isNew ? '添加节次' : `第${this.timeSlot.section}节时间设置`)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ bottom: 20 })

      if (this.errorMessage) {
        Text(this.errorMessage)
          .fontSize(14)
          .fontColor('#FF6B6B')
          .width('100%')
          .textAlign(TextAlign.Center)
          .padding(8)
          .backgroundColor('#FFF5F5')
          .borderRadius(8)
          .margin({ bottom: 16 })
      }

      if (this.successMessage) {
        Text(this.successMessage)
          .fontSize(14)
          .fontColor('#52C41A')
          .width('100%')
          .textAlign(TextAlign.Center)
          .padding(8)
          .backgroundColor('#F6FFED')
          .borderRadius(8)
          .margin({ bottom: 16 })
      }

      Column({ space: 24 }) {
        // 开始时间选择
        Column({ space: 8 }) {
          Text('开始时间')
            .fontSize(14)
            .fontColor('#666666')

          Row({ space: 12 }) {
            // 小时选择
            Row() {
              Select(this.hours)
                .selected(this.selectedStartHour)
                .value(this.selectedStartHour.toString())
                .font({ size: 16 })
                .selectedOptionFont({ size: 16 })
                .optionFont({ size: 16 })
                .onSelect((index: number) => {
                  this.selectedStartHour = index;
                })
                .layoutWeight(1)
                .backgroundColor('#F5F5F5')
                .borderRadius(8)

              Text('时')
                .fontSize(16)
                .fontColor('#333333')
                .margin({ left: 8 })
            }
            .layoutWeight(1)
            .alignItems(VerticalAlign.Center)

            // 分钟选择
            Row() {
              Select(this.minutes)
                .selected(this.selectedStartMinute)
                .value(this.selectedStartMinute.toString().padStart(2, '0'))
                .font({ size: 16 })
                .selectedOptionFont({ size: 16 })
                .optionFont({ size: 16 })
                .onSelect((index: number) => {
                  this.selectedStartMinute = index;
                })
                .layoutWeight(1)
                .backgroundColor('#F5F5F5')
                .borderRadius(8)

              Text('分')
                .fontSize(16)
                .fontColor('#333333')
                .margin({ left: 8 })
            }
            .layoutWeight(1)
            .alignItems(VerticalAlign.Center)
          }
        }
        .width('100%')
        .alignItems(HorizontalAlign.Start)

        // 结束时间选择
        Column({ space: 8 }) {
          Text('结束时间')
            .fontSize(14)
            .fontColor('#666666')

          Row({ space: 12 }) {
            // 小时选择
            Row() {
              Select(this.hours)
                .selected(this.selectedEndHour)
                .value(this.selectedEndHour.toString())
                .font({ size: 16 })
                .selectedOptionFont({ size: 16 })
                .optionFont({ size: 16 })
                .onSelect((index: number, value: string) => {
                  this.selectedEndHour = parseInt(value);
                })
                .layoutWeight(1)
                .backgroundColor('#F5F5F5')
                .borderRadius(8)

              Text('时')
                .fontSize(16)
                .fontColor('#333333')
                .margin({ left: 8 })
            }
            .layoutWeight(1)
            .alignItems(VerticalAlign.Center)

            // 分钟选择
            Row() {
              Select(this.minutes)
                .selected(this.selectedEndMinute)
                .value(this.selectedEndMinute.toString().padStart(2, '0'))
                .font({ size: 16 })
                .selectedOptionFont({ size: 16 })
                .optionFont({ size: 16 })
                .onSelect((index: number, value: string) => {
                  this.selectedEndMinute = parseInt(value);
                })
                .layoutWeight(1)
                .backgroundColor('#F5F5F5')
                .borderRadius(8)

              Text('分')
                .fontSize(16)
                .fontColor('#333333')
                .margin({ left: 8 })
            }
            .layoutWeight(1)
            .alignItems(VerticalAlign.Center)
          }
        }
        .width('100%')
        .alignItems(HorizontalAlign.Start)
      }
      .width('100%')

      Row() {
        Button('取消')
          .fontSize(16)
          .fontColor('#666666')
          .backgroundColor('#F5F5F5')
          .borderRadius(20)
          .layoutWeight(1)
          .margin({ right: 8 })
          .onClick(() => {
            if (this.controller) {
              this.controller.close();
            }
          })

        Button('保存')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .backgroundColor('#007DFF')
          .borderRadius(20)
          .layoutWeight(1)
          .margin({ left: 8 })
          .onClick(() => {
            const validation = this.validateTime();
            if (!validation.valid) {
              this.errorMessage = validation.message;
              this.successMessage = '';
              return;
            }

            this.errorMessage = '';
            this.successMessage = '保存成功';
            const updatedTimeSlot: TimeSlot = {
              section: this.isNew ? this.selectedSection : this.timeSlot.section,
              startTime: this.formatTime(this.selectedStartHour, this.selectedStartMinute),
              endTime: this.formatTime(this.selectedEndHour, this.selectedEndMinute)
            };
            this.onSave(updatedTimeSlot);

            setTimeout(() => {
              if (this.controller) {
                this.controller.close();
              }
            }, 1500);
          })
      }
      .width('100%')
      .margin({ top: 20 })
    }
    .width('100%')
    .padding(24)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
  }
}

@Entry
@Component
struct SettingsPage {
  @State timeSlots: TimeSlot[] = [];
  @State refreshFlag: number = 0;
  private courseService: CourseDataService = CourseDataService.getInstance();
  private settings: AppSettings = {
    theme: {
      name: '默认',
      primaryColor: '#007DFF',
      backgroundColor: '#F5F5F5',
      cardBackgroundColor: '#FFFFFF',
      textColor: '#333333',
      secondaryTextColor: '#666666'
    },
    currentSemester: '',
    customTimeSlots: []
  };
  private timeSlotDialogController: CustomDialogController | null = null;
  private deleteConfirmDialogController: CustomDialogController | null = null;
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;

  aboutToAppear(): void {
    this.timeSlots = [...this.courseService.getTimeSlots()];
    this.settings = this.courseService.getSettings();
  }

  private showTimeSlotDialog(timeSlot: TimeSlot): void {
    this.timeSlotDialogController = new CustomDialogController({
      builder: TimeSlotDialog({
        timeSlot: timeSlot,
        existingTimeSlots: this.timeSlots,
        onSave: (updatedTimeSlot: TimeSlot) => {
          this.updateTimeSlot(updatedTimeSlot);
        }
      }),
      customStyle: true,
      alignment: DialogAlignment.Center
    });
    this.timeSlotDialogController.open();
  }

  private showInsertSectionDialog(): void {
    if (this.timeSlots.length >= 12) {
      promptAction.showToast({
        message: '最多只能添加12节课',
        duration: 2000
      });
      return;
    }

    this.timeSlotDialogController = new CustomDialogController({
      builder: TimeSlotDialog({
        timeSlot: { section: 1, startTime: '08:00', endTime: '08:45' },
        existingTimeSlots: this.timeSlots,
        isNew: true,
        onSave: (newTimeSlot: TimeSlot) => {
          this.addTimeSlot(newTimeSlot);
        }
      }),
      customStyle: true,
      alignment: DialogAlignment.Center
    });
    this.timeSlotDialogController.open();
  }

  private showDeleteConfirmDialog(section: number): void {
    this.deleteConfirmDialogController = new CustomDialogController({
      builder: DeleteConfirmDialog({
        section: section,
        onConfirm: () => {
          this.deleteTimeSlot(section);
        }
      }),
      customStyle: true,
      alignment: DialogAlignment.Center
    });
    this.deleteConfirmDialogController.open();
  }

  private async updateTimeSlot(timeSlot: TimeSlot): Promise<void> {
    console.info('updateTimeSlot called for section: ' + timeSlot.section);
    console.info('New time: ' + timeSlot.startTime + ' - ' + timeSlot.endTime);

    const validation = this.validateTimeSlot(timeSlot.startTime, timeSlot.endTime, timeSlot.section);
    if (!validation.valid) {
      console.error('Validation failed: ' + validation.message);
      return;
    }

    const index = this.timeSlots.findIndex(t => t.section === timeSlot.section);
    console.info('Found index: ' + index);

    if (index !== -1) {
      const newTimeSlots = [...this.timeSlots];
      newTimeSlots[index] = timeSlot;
      this.timeSlots = this.sortTimeSlots(newTimeSlots);
      this.refreshFlag++;
      console.info('Updated timeSlots array and refreshFlag');

      await this.courseService.setTimeSlots(this.timeSlots);
      console.info('Updated time slot in storage');
    }
  }

  private sortTimeSlots(timeSlots: TimeSlot[]): TimeSlot[] {
    const sorted = [...timeSlots].sort((a, b) => {
      const timeA = a.startTime.split(':').map(Number);
      const timeB = b.startTime.split(':').map(Number);
      if (timeA[0] !== timeB[0]) {
        return timeA[0] - timeB[0];
      }
      return timeA[1] - timeB[1];
    });

    for (let i = 0; i < sorted.length; i++) {
      sorted[i].section = i + 1;
    }

    return sorted;
  }

  private validateTimeSlot(startTime: string, endTime: string, excludeSection?: number): ValidationResult {
    if (!startTime || !endTime) {
      const result: ValidationResult = { valid: false, message: '时间不能为空' };
      return result;
    }

    const startParts = startTime.split(':').map(Number);
    const endParts = endTime.split(':').map(Number);

    if (startParts.length !== 2 || endParts.length !== 2) {
      const result: ValidationResult = { valid: false, message: '时间格式错误' };
      return result;
    }

    if (startParts[0] < 0 || startParts[0] > 23 || startParts[1] < 0 || startParts[1] > 59) {
      const result: ValidationResult = { valid: false, message: '开始时间无效' };
      return result;
    }

    if (endParts[0] < 0 || endParts[0] > 23 || endParts[1] < 0 || endParts[1] > 59) {
      const result: ValidationResult = { valid: false, message: '结束时间无效' };
      return result;
    }

    const startMinutes = startParts[0] * 60 + startParts[1];
    const endMinutes = endParts[0] * 60 + endParts[1];

    if (startMinutes >= endMinutes) {
      const result: ValidationResult = { valid: false, message: '结束时间必须晚于开始时间' };
      return result;
    }

    for (const slot of this.timeSlots) {
      if (excludeSection && slot.section === excludeSection) {
        continue;
      }

      const slotStartParts = slot.startTime.split(':').map(Number);
      const slotEndParts = slot.endTime.split(':').map(Number);
      const slotStartMinutes = slotStartParts[0] * 60 + slotStartParts[1];
      const slotEndMinutes = slotEndParts[0] * 60 + slotEndParts[1];

      if (!(endMinutes <= slotStartMinutes || startMinutes >= slotEndMinutes)) {
        const result: ValidationResult = { valid: false, message: `与第${slot.section}节时间冲突` };
        return result;
      }
    }

    const result: ValidationResult = { valid: true, message: '' };
    return result;
  }

  private async addTimeSlot(newTimeSlot: TimeSlot): Promise<void> {
    const validation = this.validateTimeSlot(newTimeSlot.startTime, newTimeSlot.endTime);
    if (!validation.valid) {
      console.error('Validation failed: ' + validation.message);
      return;
    }

    const newTimeSlots = [...this.timeSlots, newTimeSlot];

    this.timeSlots = this.sortTimeSlots(newTimeSlots);
    this.refreshFlag++;

    await this.courseService.setTimeSlots(this.timeSlots);
    console.info('Added new time slot');
  }

  private async deleteTimeSlot(section: number): Promise<void> {
    const newTimeSlots = this.timeSlots.filter(t => t.section !== section);

    this.timeSlots = this.sortTimeSlots(newTimeSlots);
    this.refreshFlag++;

    await this.courseService.setTimeSlots(this.timeSlots);
    console.info('Deleted time slot: section ' + section);
  }

  build() {
    Column() {
      Row() {
        Button() {
          Text('←')
            .fontSize(20)
            .fontColor('#333333')
        }
        .width(40)
        .height(40)
        .backgroundColor(Color.Transparent)
        .onClick(() => {
          router.back();
        })

        Text('设置')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .layoutWeight(1)

        Blank()
      }
      .width('100%')
      .padding({
        left: 16,
        right: 16,
        top: 12,
        bottom: 12
      })
      .backgroundColor('#FFFFFF')

      this.TimeSlotSettings()
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }

  @Builder
  TimeSlotSettings() {
    Column() {
      Row() {
        Text('节次时间设置')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .layoutWeight(1)

        Button() {
          Row() {
            Text('+')
              .fontSize(16)
            Text('添加节次')
              .fontSize(14)
              .fontColor('#FFFFFF')
              .margin({ left: 4 })
          }
        }
        .backgroundColor('#007DFF')
        .borderRadius(20)
        .padding({
          left: 16,
          right: 16,
          top: 8,
          bottom: 8
        })
        .onClick(() => {
          this.showInsertSectionDialog();
        })
      }
      .width('100%')
      .padding({
        left: 20,
        right: 20,
        top: 20,
        bottom: 20
      })

      List() {
        ForEach(this.timeSlots, (timeSlot: TimeSlot, index: number) => {
          ListItem() {
            Row() {
              Text(`第${timeSlot.section}节`)
                .fontSize(16)
                .fontColor('#333333')
                .width(80)

              Text(`${timeSlot.startTime} - ${timeSlot.endTime}`)
                .fontSize(16)
                .fontColor('#666666')
                .layoutWeight(1)

              Button('编辑')
                .fontSize(14)
                .fontColor('#007DFF')
                .backgroundColor(Color.Transparent)
                .onClick(() => {
                  this.showTimeSlotDialog(timeSlot);
                })

              Button() {
                Text('删除')
                  .fontSize(14)
                  .fontColor('#FF6B6B')
              }
              .backgroundColor(Color.Transparent)
              .onClick(() => {
                this.showDeleteConfirmDialog(timeSlot.section);
              })
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(8)
          }
          .margin({ bottom: 8 })
        }, (timeSlot: TimeSlot, index: number) => `${timeSlot.section}-${this.refreshFlag}`)
      }
      .width('100%')
      .layoutWeight(1)
      .padding({ left: 16, right: 16 })
    }
    .width('100%')
    .height('100%')
    .padding({ bottom: 60 })
  }
}
