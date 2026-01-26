import { Course, WeekDay } from '../model/CourseModel';
import { CourseDataService } from '../service/CourseDataService';

import promptAction from '@ohos.promptAction'; // 引入提示工具

import { ColorConstants } from '../model/ColorConstants';

interface SelectOption {
  value: string;
}

@Component
export struct AddCourseDialog {
  @Link @Watch('onShowChange') show: boolean;
  @Prop initialDay: WeekDay = WeekDay.MONDAY;
  @Prop initialSection: number = 1;
  private courseService: CourseDataService = CourseDataService.getInstance();
  
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

  onShowChange(): void {
    if (this.show) {
      this.initForm();
    }
  }

  aboutToAppear() {
    this.initForm();
  }

  private initForm() {
    this.selectedDay = this.initialDay;
    this.startSection = this.initialSection;
    this.endSection = this.initialSection;
    this.resetForm();
  }

  build() {
    Column() {
      Text('添加课程')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .width('100%')
        .textAlign(TextAlign.Center)
        .margin({ bottom: 20 })
      
      Scroll() {
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
      .layoutWeight(1)
      
      Row() {
        Button('取消')
          .fontSize(16)
          .fontColor('#666666')
          .backgroundColor('#F0F0F0')
          .layoutWeight(1)
          .height(44)
          .borderRadius(22)
          .onClick(() => {
            this.show = false;
            this.resetForm();
          })
        
        Button('保存')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .backgroundColor('#007DFF')
          .layoutWeight(1)
          .height(44)
          .borderRadius(22)
          .margin({ left: 12 })
          .onClick(() => {
            this.saveCourse();
          })
      }
      .width('100%')
      .margin({ top: 20 })
    }
    .width('90%')
    .height(580)
    .padding(24)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
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

  @Builder
  RemarkInput() {
    Column({ space: 8 }) {
      Text('备注')
        .fontSize(14)
        .fontColor('#666666')
      
      TextArea({ placeholder: '请输入课程备注（可选）', text: this.courseRemark })
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

  private saveCourse(): void {
    if (!this.courseName.trim()) {
      promptAction.showToast({ message: '请输入课程名称' });
      return;
    }

    // --- 新增：冲突检测 ---
    const hasConflict = this.courseService.checkConflict(
      this.selectedDay,
      this.startSection,
      this.endSection
    );

    if (hasConflict) {
      // 发现冲突，提示用户并终止
      promptAction.showDialog({
        title: '时间冲突',
        message: `周${this.courseService.getWeekDayName(this.selectedDay)} 第${this.startSection}-${this.endSection}节 已经有课程了，请检查时间或先删除原有课程。`,
        buttons: [{ text: '我知道了', color: '#007DFF' }]
      });
      return;
    }
    
    const currentSemester = this.courseService.getCurrentSemester();
    const newCourse: Course = {
      id: Date.now().toString(),
      name: this.courseName,
      location: this.courseLocation,
      teacher: this.courseTeacher,
      day: this.selectedDay,
      startSection: this.startSection,
      endSection: this.endSection,
      color: this.selectedColor,
      remark: this.courseRemark,
      semester: currentSemester?.id || '',
      startWeek: this.startWeek,
      endWeek: this.endWeek,
      credit: this.courseCredit
    };

    // 调用 Service 添加课程，Service 会负责更新 AppStorage，从而自动刷新 Index 界面
    this.courseService.addCourse(newCourse);

    // 在 saveCourse() 方法内部，this.show = false 之前加入：
    promptAction.showToast({
      message: '课程添加成功',
      duration: 2000
    });

    this.show = false;
    this.resetForm();
  }

  private resetForm(): void {
    this.courseName = '';
    this.courseLocation = '';
    this.courseTeacher = '';
    this.selectedColor = '#007DFF';

    this.courseRemark = '';
    this.startWeek = 1;
    this.endWeek = 18;
    this.courseCredit = 0;
  }
}
