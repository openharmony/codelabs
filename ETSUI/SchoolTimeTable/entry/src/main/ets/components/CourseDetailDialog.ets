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
import promptAction from '@ohos.promptAction';

interface SelectOption {
  value: string;
}

@Component
export struct CourseDetailDialog {
  @Link show: boolean;
  courseId: string = '';
  private courseService: CourseDataService = CourseDataService.getInstance();
  
  @State course: Course | null = null;
  @State isEditing: boolean = false;
  
  @State courseName: string = '';
  @State courseLocation: string = '';
  @State courseTeacher: string = '';
  @State selectedDay: WeekDay = WeekDay.MONDAY;
  @State startSection: number = 1;
  @State endSection: number = 1;
  @State selectedColor: string = '#007DFF';
  @State courseRemark: string = '';
  @State startWeek: number = 1;
  @State endWeek: number = 18;
  @State courseCredit: number = 0;
  
  private colors: string[] = ColorConstants.MACARON_COLORS;

  aboutToAppear(): void {
    this.loadCourse();
  }

  private loadCourse(): void {
    const foundCourse = this.courseService.getCourseById(this.courseId);
    this.course = foundCourse ? foundCourse : null;
    if (this.course) {
      this.courseName = this.course.name;
      this.courseLocation = this.course.location;
      this.courseTeacher = this.course.teacher;
      this.selectedDay = this.course.day;
      this.startSection = this.course.startSection;
      this.endSection = this.course.endSection;
      this.selectedColor = this.course.color;
      this.courseRemark = this.course.remark;
      this.startWeek = this.course.startWeek;
      this.endWeek = this.course.endWeek;
      this.courseCredit = this.course.credit;
    }
  }

  build() {
    Column() {
      Row() {
        Text(this.isEditing ? '编辑课程' : '课程详情')
          .fontSize(22)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
        
        Blank()
        
        if (!this.isEditing) {
          Button() {
            Row() {
              Text('✎')
                .fontSize(16)
              Text('编辑')
                .fontSize(14)
                .fontColor('#007DFF')
                .margin({ left: 4 })
            }
          }
          .backgroundColor('#F0F7FF')
          .borderRadius(20)
          .padding({ left: 12, right: 12, top: 6, bottom: 6 })
          .onClick(() => {
            this.isEditing = true;
          })
        }
      }
      .width('100%')
      .margin({ bottom: 24 })
      
      Scroll() {
        Column({ space: 16 }) {
          if (this.isEditing) {
            this.EditForm()
          } else {
            this.DetailView()
          }
        }
        .width('100%')
      }
      .layoutWeight(1)
      
      Row() {
        Button(this.isEditing ? '取消' : '关闭')
          .fontSize(16)
          .fontColor('#666666')
          .backgroundColor('#F0F0F0')
          .layoutWeight(1)
          .height(48)
          .borderRadius(24)
          .onClick(() => {
            if (this.isEditing) {
              this.isEditing = false;
              this.loadCourse();
            } else {
              this.show = false;
            }
          })
        
        if (this.isEditing) {
          Button('保存')
            .fontSize(16)
            .fontColor('#FFFFFF')
            .backgroundColor('#007DFF')
            .layoutWeight(1)
            .height(48)
            .borderRadius(24)
            .margin({ left: 12 })
            .onClick(() => {
              this.saveCourse();
            })
        } else {
          Button() {
            Row() {
              Text('🗑')
                .fontSize(16)
              Text('删除')
                .fontSize(16)
                .fontColor('#FFFFFF')
                .margin({ left: 4 })
            }
          }
          .backgroundColor('#FF6B6B')
          .layoutWeight(1)
          .height(48)
          .borderRadius(24)
          .margin({ left: 12 })
          .onClick(() => {
            this.deleteCourse();
          })
        }
      }
      .width('100%')
      .margin({ top: 24 })
    }
    .width('90%')
    .height(600)
    .padding(28)
    .backgroundColor('#FFFFFF')
    .borderRadius(20)
    .shadow({
      radius: 24,
      color: '#1A000000',
      offsetX: 0,
      offsetY: 8
    })
  }

  @Builder
  DetailView() {
    Column({ space: 16 }) {
      Column() {
        Text(this.course?.name || '')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .width('100%')
        
        Row() {
          Text(this.courseService.getWeekDayName(this.course?.day || WeekDay.MONDAY))
            .fontSize(14)
            .fontColor('#FFFFFF')
            .padding({ left: 12, right: 12, top: 6, bottom: 6 })
            .backgroundColor(this.course?.color || '#007DFF')
            .borderRadius(12)
          
          Text(`第${this.course?.startSection}-${this.course?.endSection}节`)
            .fontSize(14)
            .fontColor('#FFFFFF')
            .padding({ left: 12, right: 12, top: 6, bottom: 6 })
            .backgroundColor(this.course?.color || '#007DFF')
            .borderRadius(12)
            .margin({ left: 8 })
        }
        .margin({ top: 12 })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#F8F9FA')
      .borderRadius(16)
      
      Column({ space: 12 }) {
        this.DetailItem('📍', '上课地点', this.course?.location || '')
        this.DetailItem('👨‍🏫', '授课教师', this.course?.teacher || '')
        if (this.course?.remark) {
          this.DetailItem('📝', '备注', this.course?.remark)
        }
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#FFFFFF')
      .borderRadius(16)
      .border({ width: 1, color: '#E0E0E0' })
    }
    .width('100%')
  }

  @Builder
  DetailItem(icon: string, label: string, value: string) {
    Row() {
      Text(icon)
        .fontSize(20)
        .width(40)
      
      Column({ space: 4 }) {
        Text(label)
          .fontSize(12)
          .fontColor('#999999')
        
        Text(value)
          .fontSize(16)
          .fontColor('#333333')
          .fontWeight(FontWeight.Medium)
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
    }
    .width('100%')
    .padding({ top: 16, bottom: 16 })
    .borderRadius(12)
    .backgroundColor('#F8F9FA')
  }

  @Builder
  EditForm() {
    Column({ space: 16 }) {
      this.InputField('课程名称', this.courseName, (value: string) => {
        this.courseName = value;
      })
      
      this.InputField('上课地点', this.courseLocation, (value: string) => {
        this.courseLocation = value;
      })
      
      this.InputField('授课教师', this.courseTeacher, (value: string) => {
        this.courseTeacher = value;
      })
      
      this.DaySelector()
      
      this.SectionSelector()
      
      this.ColorSelector()
 
      this.RemarkInput()
    }
    .width('100%')
  }

  @Builder
  RemarkInput() {
    Column({ space: 8 }) {
      Text('备注')
        .fontSize(14)
        .fontColor('#666666')
      
      TextArea({ placeholder: '请输入备注', text: this.courseRemark })
        .fontSize(16)
        .fontColor('#333333')
        .backgroundColor('#F5F5F5')
        .borderRadius(8)
        .height(100)
        .padding({ left: 12, right: 12, top: 10, bottom: 10 })
        .onChange((value: string) => {
          this.courseRemark = value;
        })
    }
    .width('100%')
    .alignItems(HorizontalAlign.Start)
  }

  @Builder
  InputField(label: string, value: string, onChange: (value: string) => void) {
    Column({ space: 8 }) {
      Text(label)
        .fontSize(14)
        .fontColor('#666666')
      
      TextInput({ placeholder: `请输入${label}`, text: value })
        .fontSize(16)
        .fontColor('#333333')
        .backgroundColor('#F5F5F5')
        .borderRadius(8)
        .padding({ left: 12, right: 12, top: 10, bottom: 10 })
        .onChange((value: string) => {
          onChange(value);
        })
    }
    .width('100%')
    .alignItems(HorizontalAlign.Start)
  }

  @Builder
  DaySelector() {
    Column({ space: 8 }) {
      Text('星期')
        .fontSize(14)
        .fontColor('#666666')
      
      Flex({ wrap: FlexWrap.Wrap, justifyContent: FlexAlign.SpaceBetween }) {
        ForEach([1, 2, 3, 4, 5, 6, 7], (day: number) => {
          Button(this.courseService.getWeekDayName(day as WeekDay))
            .fontSize(14)
            .fontColor(this.selectedDay === day ? '#FFFFFF' : '#666666')
            .backgroundColor(this.selectedDay === day ? '#007DFF' : '#F0F0F0')
            .borderRadius(8)
            .width('30%')
            .height(40)
            .margin({ bottom: 8 })
            .onClick(() => {
              this.selectedDay = day as WeekDay;
            })
        })
      }
    }
    .width('100%')
    .alignItems(HorizontalAlign.Start)
  }

  @Builder
  SectionSelector() {
    Column({ space: 8 }) {
      Text('节次')
        .fontSize(14)
        .fontColor('#666666')
      
      Row() {
        Column() {
          Text('开始')
            .fontSize(12)
            .fontColor('#999999')
          
          Select(this.getSectionOptions())
            .selected(this.startSection - 1)
            .value(`${this.startSection}节`)
            .font({ size: 16 })
            .selectedOptionFont({ size: 16 })
            .optionFont({ size: 16 })
            .onSelect((index: number) => {
              this.startSection = index + 1;
              if (this.endSection < this.startSection) {
                this.endSection = this.startSection;
              }
            })
        }
        .layoutWeight(1)
        
        Text('至')
          .fontSize(14)
          .fontColor('#666666')
          .margin({ left: 12, right: 12 })
        
        Column() {
          Text('结束')
            .fontSize(12)
            .fontColor('#999999')
          
          Select(this.getEndSectionOptions())
            .selected(this.endSection - this.startSection)
            .value(`${this.endSection}节`)
            .font({ size: 16 })
            .selectedOptionFont({ size: 16 })
            .optionFont({ size: 16 })
            .onSelect((index: number) => {
              this.endSection = this.startSection + index;
            })
        }
        .layoutWeight(1)
      }
      .width('100%')
    }
    .width('100%')
    .alignItems(HorizontalAlign.Start)
  }

  private getSectionOptions(): SelectOption[] {
    const options: SelectOption[] = [];
    const timeSlots = this.courseService.getTimeSlots();
    for (let i = 0; i < timeSlots.length; i++) {
      const option: SelectOption = { value: timeSlots[i].section.toString() };
      options.push(option);
    }
    return options;
  }

  private getEndSectionOptions(): SelectOption[] {
    const options: SelectOption[] = [];
    const timeSlots = this.courseService.getTimeSlots()
      .filter(slot => slot.section >= this.startSection);
    for (let i = 0; i < timeSlots.length; i++) {
      const option: SelectOption = { value: timeSlots[i].section.toString() };
      options.push(option);
    }
    return options;
  }

  @Builder
  ColorSelector() {
    Column({ space: 8 }) {
      Text('课程颜色')
        .fontSize(14)
        .fontColor('#666666')
      
      Flex({ wrap: FlexWrap.Wrap }) {
        ForEach(this.colors, (color: string) => {
          Button()
            .width(36)
            .height(36)
            .borderRadius(18)
            .backgroundColor(color)
            .margin({ right: 12, bottom: 12 })
            .border({
              width: this.selectedColor === color ? 3 : 0,
              color: '#333333'
            })
            .onClick(() => {
              this.selectedColor = color;
            })
        })
      }
    }
    .width('100%')
    .alignItems(HorizontalAlign.Start)
  }



  private saveCourse(): void {
    if (!this.courseName.trim() || !this.course) {
      return;
    }

    // --- 新增：冲突检测 (传入 this.course.id 以排除自身) ---
    const hasConflict = this.courseService.checkConflict(
      this.selectedDay,
      this.startSection,
      this.endSection,
      this.course.id // 关键：告诉检测函数，忽略我自己
    );

    if (hasConflict) {
      promptAction.showDialog({
        title: '时间冲突',
        message: `周${this.courseService.getWeekDayName(this.selectedDay)} 第${this.startSection}-${this.endSection}节 已经有其他课程了。`,
        buttons: [{ text: '我知道了', color: '#007DFF' }]
      });
      return;
    }
    
    const updatedCourse: Course = {
      id: this.course.id,
      name: this.courseName,
      location: this.courseLocation,
      teacher: this.courseTeacher,
      day: this.selectedDay,
      startSection: this.startSection,
      endSection: this.endSection,
      color: this.selectedColor,
      remark: this.courseRemark,
      semester: this.course.semester,
      startWeek: this.startWeek,
      endWeek: this.endWeek,
      credit: this.courseCredit
    };
    
    this.courseService.updateCourse(updatedCourse);
    this.isEditing = false;
    this.show = false;
  }

  private deleteCourse(): void {
    if (this.course) {
      this.courseService.deleteCourse(this.course.id);
      this.show = false;
    }
  }
}
