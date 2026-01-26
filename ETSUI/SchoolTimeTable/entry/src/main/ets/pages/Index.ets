import { WeekDay, Course, TimeSlot, AppSettings } from '../model/CourseModel';
import { CourseDataService, COURSE_LIST_KEY } from '../service/CourseDataService';
import { DateUtils } from '../utils/DateUtils';
import { router } from '@kit.ArkUI';
import { AddCourseDialog } from '../components/AddCourseDialog';
import { CourseDetailDialog } from '../components/CourseDetailDialog';
import { ProfileView } from '../views/ProfileView';


@Entry
@Component
struct Index {
  @State selectedDay: WeekDay = WeekDay.MONDAY;
  @State showAddDialog: boolean = false;
  @State showCourseDetail: boolean = false;
  @State selectedCourseId: string = '';
  @StorageLink(COURSE_LIST_KEY) @Watch('onCoursesChange') allCourses: Course[] = [];
  @State searchKeyword: string = '';
  @State currentWeek: number = 1;
  @State timeSlots: TimeSlot[] = [];
  @State timeSlotsRefreshFlag: number = 0;
  @State coursesRefreshFlag: number = 0;

  onCoursesChange() {
    this.coursesRefreshFlag++;
  }
  @State courses: Course[] = [];
  @State addCourseDay: WeekDay = WeekDay.MONDAY;
  @State addCourseSection: number = 1;
  @State settings: AppSettings = {
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
  @State selectedTab: number = 0;
  @State isWeekView: boolean = false;
  private courseService: CourseDataService = CourseDataService.getInstance();
  private weekDays: WeekDay[] = [WeekDay.MONDAY, WeekDay.TUESDAY, WeekDay.WEDNESDAY,
    WeekDay.THURSDAY, WeekDay.FRIDAY, WeekDay.SATURDAY, WeekDay.SUNDAY];
  private readonly SLOT_HEIGHT: number = 58; // 单节课高度

  async aboutToAppear(): Promise<void> {
    try {
      const context = getContext(this);
      if (context) {
        await this.courseService.init(context);
      } else {
        console.warn('Context is null, using default data for previewer');
      }
    } catch (error) {
      console.error('Failed to initialize course service:', error);
    }
    this.updateCurrentWeek();
    this.loadSettings();
    this.loadTimeSlots();
    this.loadCourses();
  }

  onPageShow(): void {
    this.loadSettings();
    this.loadTimeSlots();
    this.loadCourses();
  }

  aboutToDisappear(): void {
    console.info('Index page about to disappear');
  }

  private loadSettings(): void {
    this.settings = this.courseService.getSettings();
  }

  private loadTimeSlots(): void {
    const loadedTimeSlots = this.courseService.getTimeSlots();
    console.info('Loaded ' + loadedTimeSlots.length + ' time slots');
    loadedTimeSlots.forEach(slot => {
      console.info('Section ' + slot.section + ': ' + slot.startTime + ' - ' + slot.endTime);
    });
    this.timeSlots = [...loadedTimeSlots];
    this.timeSlotsRefreshFlag++;
  }

  private loadCourses(): void {
    this.courses = [...this.courseService.getCourses()];
    this.coursesRefreshFlag++;
    console.info('Loaded ' + this.courses.length + ' courses');
  }

  onRefreshFlagChange(): void {
    console.info('Refresh flag changed, reloading data');
    this.loadCourses();
  }

  private updateCurrentWeek(): void {
    const currentSemester = this.courseService.getCurrentSemester();
    if (currentSemester) {
      this.currentWeek = DateUtils.getCurrentWeekNumber(currentSemester.startDate);
    }
  }

  private getDayDateString(day: WeekDay): string {
    const currentSemester = this.courseService.getCurrentSemester();
    if (!currentSemester) {
      return '';
    }
    const startDate = new Date(currentSemester.startDate);
    const dayOffset = (this.currentWeek - 1) * 7 + (day - 1);
    const targetDate = new Date(startDate.getTime() + dayOffset * 24 * 60 * 60 * 1000);
    return `${targetDate.getMonth() + 1}/${targetDate.getDate()}`;
  }

  build() {
    Stack() {
      Column() {
        if (this.selectedTab === 0) {
          this.Header()

          if (this.isWeekView) {
            this.WeekScheduleView()
          } else {
            Tabs({ barPosition: BarPosition.Start }) {
              ForEach(this.weekDays, (day: WeekDay) => {
                TabContent() {
                  this.ScheduleView(day)
                }
                .tabBar(this.TabBarItem(day))
              })
            }
            .barMode(BarMode.Fixed)
            .layoutWeight(1)
            .animationDuration(200)
            .onChange((index: number) => {
              this.selectedDay = this.weekDays[index];
            })
          }
        } else {
          ProfileView({ refreshFlag: $coursesRefreshFlag })
            .layoutWeight(1)
        }

        this.BottomNavigationBar()
      }
      .width('100%')
      .height('100%')
      .backgroundColor(this.settings.theme.backgroundColor)

      if (this.showAddDialog) {
        Column()
          .width('100%')
          .height('100%')
          .backgroundColor('#80000000')
          .onClick(() => {
            this.showAddDialog = false;
          })
          .zIndex(998)

        AddCourseDialog({
          show: $showAddDialog,
          initialDay: this.addCourseDay,
          initialSection: this.addCourseSection
        })
        .zIndex(999)
      }

      if (this.showCourseDetail) {
        Column() {
          CourseDetailDialog({
            courseId: this.selectedCourseId,
            show: $showCourseDetail
          })
        }
        .width('100%')
        .height('100%')
        .backgroundColor('rgba(0, 0, 0, 0.5)')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .padding(20)

        CourseDetailDialog({
          courseId: this.selectedCourseId,
          show: $showCourseDetail
        })
      }
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  Header() {
    Column() {
      Row() {
        Text('课程表')
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .fontColor(this.settings.theme.textColor)
          .margin({ right: 20 })

        Blank()

        Row({ space: 8 }) {
          Button() {
            Text(this.isWeekView ? '📅' : '🗓️')
              .fontSize(20)
          }
          .width(40)
          .height(40)
          .borderRadius(10)
          .backgroundColor('#F0F0F0')
          .onClick(() => {
            this.isWeekView = !this.isWeekView;
          })

          Button() {
            Text('⚙️')
              .fontSize(20)
          }
          .width(40)
          .height(40)
          .borderRadius(10)
          .backgroundColor('#F0F0F0')
          .onClick(() => {
            router.pushUrl({ url: 'pages/SettingsPage' }).catch((err: Error) => {
              console.error('Failed to push url:', err);
            });
          })

          Button() {
            Text('📊')
              .fontSize(20)
          }
          .width(40)
          .height(40)
          .borderRadius(10)
          .backgroundColor('#F0F0F0')
          .onClick(() => {
            router.pushUrl({ url: 'pages/StatisticsPage' }).catch((err: Error) => {
              console.error('Failed to push url:', err);
            });
          })

          Button() {
            Text('🔍')
              .fontSize(20)
          }
          .width(40)
          .height(40)
          .borderRadius(10)
          .backgroundColor('#F0F0F0')
          .onClick(() => {
            router.pushUrl({ url: 'pages/SearchPage' }).catch((err: Error) => {
              console.error('Failed to push url:', err);
            });
          })

          Button() {
            Text('➕')
              .fontSize(20)
          }
          .width(40)
          .height(40)
          .borderRadius(10)
          .backgroundColor('#F0F0F0')
          .onClick(() => {
            this.addCourseDay = WeekDay.MONDAY;
            this.addCourseSection = 1;
            this.showAddDialog = true;
          })
        }
      }
      .width('100%')
      .padding({
        left: 20,
        right: 20,
        top: 16,
        bottom: 16
      })
    }
    .width('100%')
    .backgroundColor(this.settings.theme.cardBackgroundColor)
    .borderRadius({ bottomLeft: 20, bottomRight: 20 })
    .shadow({
      radius: 12,
      color: '#1A000000',
      offsetX: 0,
      offsetY: 4
    })
  }

  @Builder
  TabBarItem(day: WeekDay) {
    Column() {
      Text(this.courseService.getWeekDayName(day))
        .fontSize(this.selectedDay === day ? 16 : 14)
        .fontColor(this.selectedDay === day ? this.settings.theme.textColor : this.settings.theme.secondaryTextColor)
        .fontWeight(this.selectedDay === day ? FontWeight.Bold : FontWeight.Normal)
        .animation({ duration: 200 })

      Text(this.getDayDateString(day))
        .fontSize(10)
        .fontColor(this.selectedDay === day ? this.settings.theme.textColor : this.settings.theme.secondaryTextColor)
        .opacity(0.8)
        .margin({ top: 4 })
        .visibility(this.getDayDateString(day) ? Visibility.Visible : Visibility.None)
    }
    .width('100%')
    .height(56)
    .justifyContent(FlexAlign.Center)
    .backgroundColor(this.selectedDay === day ? '#F5F5F5' : 'transparent')
    .borderRadius(16)
    .margin(2)
  }

  @Builder
  ScheduleView(day: WeekDay) {
    Column() {
      this.TimeSlotsHeader()

      List() {
        ForEach(this.timeSlots, (timeSlot: TimeSlot, index: number) => {
          ListItem() {
            this.CourseSlot(day, timeSlot)
          }
        }, (timeSlot: TimeSlot, index: number) => `${timeSlot.section}-${this.timeSlotsRefreshFlag}`)
      }
      .width('100%')
      .layoutWeight(1)
      .divider({
        strokeWidth: 1,
        color: '#E0E0E0',
        startMargin: 80,
        endMargin: 0
      })
    }
    .width('100%')
    .height('100%')
    .padding({
      left: 0,
      right: 0,
      top: 0,
      bottom: 0
    })
  }

  @Builder
  TimeSlotsHeader() {
    Row() {
      Text('时间')
        .fontSize(12)
        .fontColor(this.settings.theme.secondaryTextColor)
        .width(80)
        .textAlign(TextAlign.Center)

      Text('课程')
        .fontSize(12)
        .fontColor(this.settings.theme.secondaryTextColor)
        .layoutWeight(1)
        .textAlign(TextAlign.Center)
    }
    .width('100%')
    .padding({
      left: 8,
      right: 8,
      top: 8,
      bottom: 8
    })
    .backgroundColor('#FAFAFA')
    .border({ width: { bottom: 1 }, color: '#E0E0E0' })
  }

  @Builder
  CourseSlot(day: WeekDay, timeSlot: TimeSlot) {
    Row() {
      Column() {
        Text(`${timeSlot.section}`)
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor(this.settings.theme.textColor)

        Text(`${timeSlot.startTime}`)
          .fontSize(12)
          .fontColor(this.settings.theme.secondaryTextColor)
          .margin({ top: 2 })

        Text(`${timeSlot.endTime}`)
          .fontSize(12)
          .fontColor(this.settings.theme.secondaryTextColor)
          .margin({ top: 2 })
      }
      .width(80)
      .alignItems(HorizontalAlign.Center)
      .padding({ left: 8, right: 8 })

      this.CourseCard(day, timeSlot.section)
    }
    .width('100%')
    .height(90)
    .alignItems(VerticalAlign.Center)
    .backgroundColor('#FFFFFF')
    .padding({ left: 0, right: 0 })
  }

  @Builder
  CourseCard(day: WeekDay, section: number) {
    Column() {
      ForEach(this.allCourses
        .filter(c => c.day === day && c.startSection <= section && c.endSection >= section)
        .filter(c => !this.searchKeyword || c.name.includes(this.searchKeyword)),
        (course: Course, _index: number) => {
          Column() {
            Text(course.name)
              .fontSize(15)
              .fontWeight(FontWeight.Bold)
              .fontColor('#FFFFFF')
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.Ellipsis })
              .margin({ bottom: 4 })
            Row() {
              Text(course.location)
                .fontSize(12)
                .fontColor('#FFFFFF')
                .opacity(0.95)
                .maxLines(1)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .margin({ left: 4 })
            }
            .margin({ bottom: 2 })
            Row() {
              Text(course.teacher)
                .fontSize(12)
                .fontColor('#FFFFFF')
                .opacity(0.9)
                .maxLines(1)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .margin({ left: 4 })
            }
          }
          .width('100%')
          .height('100%')
          .padding(14)
          .backgroundColor(course.color)
          .borderRadius(10)
          .shadow({
            radius: 6,
            color: `${course.color}40`,
            offsetX: 0,
            offsetY: 2
          })
          .onClick(() => {
            this.selectedCourseId = course.id;
            this.showCourseDetail = true;
          })
        },
        (course: Course, index: number) => `${course.id}-${this.coursesRefreshFlag}`)

      Text('点击添加课程')
        .fontSize(13)
        .fontColor(this.settings.theme.secondaryTextColor)
        .layoutWeight(1)
        .textAlign(TextAlign.Center)
        .width('100%')
        .padding(12)
        .backgroundColor('#F8F8F8')
        .borderRadius(10)
        .border({ width: 1, color: '#E0E0E0', style: BorderStyle.Dashed })
        .visibility(this.allCourses
          .filter(c => c.day === day)
          .filter(c => c.startSection <= section && c.endSection >= section).length > 0 ? Visibility.None :
          Visibility.Visible)
        .onClick(() => {
          this.addCourseDay = day;
          this.addCourseSection = section;
          this.showAddDialog = true;
        })
    }
    .layoutWeight(1)
    .height('100%')
    .padding({ left: 12, right: 0 })
  }

  @Builder
  BottomNavigationBar() {
    Row() {
      Column() {
        Text('课程表')
          .fontSize(16)
          .fontColor(this.selectedTab === 0 ? '#FFFFFF' : '#666666')
          .fontWeight(this.selectedTab === 0 ? FontWeight.Bold : FontWeight.Normal)
      }
      .layoutWeight(1)
      .height('100%')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)
      .backgroundColor(this.selectedTab === 0 ? this.settings.theme.primaryColor : '#FFFFFF')
      .onClick(() => {
        this.selectedTab = 0;
      })

      Column() {
        Text('个人中心')
          .fontSize(16)
          .fontColor(this.selectedTab === 1 ? '#FFFFFF' : '#666666')
          .fontWeight(this.selectedTab === 1 ? FontWeight.Bold : FontWeight.Normal)
      }
      .layoutWeight(1)
      .height('100%')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)
      .backgroundColor(this.selectedTab === 1 ? this.settings.theme.primaryColor : '#FFFFFF')
      .onClick(() => {
        this.selectedTab = 1;
      })
    }
    .width('100%')
    .height(56)
    .backgroundColor('#FFFFFF')
    .border({ width: { top: 1 }, color: '#E0E0E0' })
  }

  @Builder
  WeekScheduleView() {
    Column() {
      // 1. Header (Days)
      Row() {
        // Empty top-left corner
        Column()
          .width(40)
          .height('100%')
          .backgroundColor('#FAFAFA')
          .border({ width: { bottom: 1, right: 1 }, color: '#E0E0E0' })

        // Day Headers
        ForEach(this.weekDays, (day: WeekDay) => {
          Column() {
            Text(this.courseService.getWeekDayName(day))
              .fontSize(14)
              .fontColor(this.settings.theme.textColor)
              .fontWeight(FontWeight.Bold)
            Text(this.getDayDateString(day))
              .fontSize(10)
              .fontColor(this.settings.theme.secondaryTextColor)
              .opacity(0.8)
              .margin({ top: 2 })
              .visibility(this.getDayDateString(day) ? Visibility.Visible : Visibility.None)
          }
          .layoutWeight(1)
          .height('100%')
          .justifyContent(FlexAlign.Center)
          .backgroundColor('#FAFAFA')
          .border({ width: { bottom: 1 }, color: '#E0E0E0' }) // Grid lines
          .onClick(() => {
            this.selectedDay = day;
          })
        })
      }
      .height(50)
      .width('100%')
      .backgroundColor('#FAFAFA')

      // 2. Body (Scrollable)
      Scroll() {
        Row() {
          // 2.1 Time Column
          Column() {
            ForEach(this.timeSlots, (slot: TimeSlot) => {
              Column() {
                Text(`${slot.section}`)
                  .fontSize(12)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(this.settings.theme.secondaryTextColor)
                Text(slot.startTime)
                  .fontSize(9)
                  .fontColor('#999999')
                  .margin({ top: 1 })
                Text(slot.endTime)
                  .fontSize(9)
                  .fontColor('#999999')
              }
              .width(40)
              .height(this.SLOT_HEIGHT)
              .justifyContent(FlexAlign.Center)
              .border({ width: { bottom: 1, right: 1 }, color: '#E0E0E0' })
              .backgroundColor('#FAFAFA')
            })
          }
          .width(40)

          // 2.2 Course Grid
          Row() {
             ForEach(this.weekDays, (day: WeekDay) => {
                // Each Day Column
                Stack({ alignContent: Alignment.Top }) {
                   // Background Lines for slots
                   Column() {
                      ForEach(this.timeSlots, () => {
                         Column()
                           .height(this.SLOT_HEIGHT)
                           .width('100%')
                           .border({ width: { bottom: 1 }, color: '#F5F5F5' })
                      })
                   }
                   .width('100%')

                   // Courses for this day
                   ForEach(this.allCourses
                     .filter(c => c.day === day)
                     .filter(c => !this.searchKeyword || c.name.includes(this.searchKeyword)),
                     (course: Course) => {
                      this.WeekCourseCard(course)
                   })

                   // Click handler for empty space (optional, to add course)
                   // Since Stack layers, we can put a transparent layer or just handle clicks on the day column?
                   // But finding the section is tricky without coordinates.
                   // For now, let's just show the courses.
                }
                .layoutWeight(1)
                .height(this.timeSlots.length * this.SLOT_HEIGHT)
                .border({ width: { right: 1 }, color: '#F0F0F0' })
             })
          }
          .layoutWeight(1)
        }
      }
      .layoutWeight(1)
      .scrollBar(BarState.Off)
      .align(Alignment.TopStart)
    }
    .width('100%')
    // 使用 layoutWeight(1) 代替 height('100%')，确保不会挤出底部导航栏
    .layoutWeight(1)
    .backgroundColor('#FFFFFF')
  }

  @Builder
  WeekCourseCard(course: Course) {
    Column() {
      Text(course.name)
        .fontSize(10)
        .fontColor('#FFFFFF')
        .fontWeight(FontWeight.Bold)
        .maxLines(3)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        .textAlign(TextAlign.Center)
        .lineHeight(12)

      if (course.endSection - course.startSection + 1 > 1) {
        Text(course.location)
          .fontSize(9)
          .fontColor('#FFFFFF')
          .opacity(0.9)
          .maxLines(1)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .margin({ top: 2 })
          .textAlign(TextAlign.Center)
      }
    }
    .width('94%')
    .height((course.endSection - course.startSection + 1) * this.SLOT_HEIGHT - 2)
    .position({ x: '3%', y: (course.startSection - 1) * this.SLOT_HEIGHT + 1 })
    .backgroundColor(course.color)
    .borderRadius(4)
    .justifyContent(FlexAlign.Center)
    .padding(2)
    .zIndex(10)
    .shadow({ radius: 2, color: '#40000000', offsetY: 1 })
    .onClick(() => {
      this.selectedCourseId = course.id;
      this.showCourseDetail = true;
    })
  }
}
