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

@Entry
@Component
struct SearchPage {
  @State searchKeyword: string = '';
  @State filteredCourses: Course[] = [];
  @State showCourseDetail: boolean = false;
  @State selectedCourse: Course | null = null;
  private courseService: CourseDataService = CourseDataService.getInstance();
  // 2. 修改这里：使用 @StorageLink 直接链接全局数据
  // 这样 allCourses 会自动获取 AppStorage 中 'GlobalCourseList' 的值
  @StorageLink(COURSE_LIST_KEY) @Watch('onDataChange') allCourses: Course[] = [];

  aboutToAppear(): void {
    // 3. 修改这里：不再调用 loadCourses() 去 service 取数
    // 而是直接利用已经链接好的 allCourses 初始化过滤列表
    this.onDataChange();
    console.info('SearchPage initialized with ' + this.allCourses.length + ' courses');
  }

  private onDataChange(): void {
    if (this.searchKeyword.trim() === '') {
      this.filteredCourses = [...this.allCourses];
    } else {
      this.onSearchChange(this.searchKeyword);
    }
  }

  // private loadCourses(): void {
  //   console.info('Loading courses...');
  //   this.allCourses = this.courseService.getCourses();
  //   console.info('Loaded ' + this.allCourses.length + ' courses');
  //   this.filteredCourses = [...this.allCourses];
  //   console.info('filteredCourses initialized with ' + this.filteredCourses.length + ' courses');
  // }

  private onSearchChange(value: string): void {
    console.info('onSearchChange called with: ' + value);
    // this.allCourses 现在已经是响应式的，且有数据
    console.info('allCourses length: ' + this.allCourses.length);
    
    if (value.trim() === '') {
      this.filteredCourses = [...this.allCourses];
      console.info('filteredCourses reset to all: ' + this.filteredCourses.length);
    } else {
      const keyword = value.toLowerCase().trim();
      console.info('Searching for keyword: ' + keyword);
      
      this.filteredCourses = this.allCourses.filter(course => {
        const name = course.name?.toLowerCase() || '';
        const teacher = course.teacher?.toLowerCase() || '';
        const location = course.location?.toLowerCase() || '';
        const remark = course.remark?.toLowerCase() || '';
        
        const matches = name.includes(keyword) ||
               teacher.includes(keyword) ||
               location.includes(keyword) ||
               remark.includes(keyword);
               
        if (matches) {
          console.info('Match found: ' + course.name);
        }
        
        return matches;
      });
      
      console.info('filteredCourses after search: ' + this.filteredCourses.length);
    }
  }

  private onCourseClick(course: Course): void {
    this.selectedCourse = course;
    this.showCourseDetail = true;
  }

  private getWeekDayName(day: WeekDay): string {
    switch (day) {
      case WeekDay.MONDAY:
        return '周一';
      case WeekDay.TUESDAY:
        return '周二';
      case WeekDay.WEDNESDAY:
        return '周三';
      case WeekDay.THURSDAY:
        return '周四';
      case WeekDay.FRIDAY:
        return '周五';
      case WeekDay.SATURDAY:
        return '周六';
      case WeekDay.SUNDAY:
        return '周日';
      default:
        return '';
    }
  }

  // private getWeekTypeName(type: WeekType): string {
  //   switch (type) {
  //     case WeekType.ALL:
  //       return '全周';
  //     case WeekType.SINGLE:
  //       return '单周';
  //     case WeekType.DOUBLE:
  //       return '双周';
  //     default:
  //       return '';
  //   }
  // }

  build() {
    Stack() {
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
          
          Text('搜索课程')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .layoutWeight(1)
        }
        .width('100%')
        .padding({ left: 16, right: 16, top: 12, bottom: 12 })
        .backgroundColor('#FFFFFF')

        Row() {
          Text('🔍')
            .fontSize(18)
            .margin({ right: 8 })
          
          TextInput({ placeholder: '搜索课程名称、教师、地点或备注' })
            .layoutWeight(1)
            .fontSize(16)
            .fontColor('#333333')
            .backgroundColor(Color.Transparent)
            .border({ width: 0 })
            .placeholderColor('#999999')
            .caretColor('#007DFF')
            .onChange((value: string) => {
              console.info('Search input changed: ' + value);
              this.searchKeyword = value;
              this.onSearchChange(value);
            })
          
          if (this.searchKeyword) {
            Button() {
              Text('×')
                .fontSize(20)
                .fontColor('#999999')
            }
            .width(32)
            .height(32)
            .backgroundColor('#F0F0F0')
            .borderRadius(16)
            .margin({ left: 8 })
            .onClick(() => {
              this.searchKeyword = '';
              this.filteredCourses = [...this.allCourses];
            })
          }
        }
        .width('100%')
        .height(52)
        .padding({ left: 16, right: 12, top: 4, bottom: 4 })
        .backgroundColor('#FFFFFF')
        .borderRadius(26)
        .border({ width: 1.5, color: '#E0E0E0' })
        .shadow({
          radius: 8,
          color: '#1A000000',
          offsetX: 0,
          offsetY: 2
        })
        .margin({ left: 16, right: 16, top: 12, bottom: 12 })

        Column() {
          if (this.filteredCourses.length === 0) {
            Column() {
              Text('📚')
                .fontSize(60)
                .margin({ bottom: 16 })
              
              Text(this.searchKeyword ? '未找到匹配的课程' : '暂无课程')
                .fontSize(16)
                .fontColor('#666666')
            }
            .width('100%')
            .layoutWeight(1)
            .justifyContent(FlexAlign.Center)
          } else {
            List() {
              ForEach(this.filteredCourses, (course: Course) => {
                ListItem() {
                  Row() {
                    Column() {
                      Text(course.name)
                        .fontSize(18)
                        .fontWeight(FontWeight.Bold)
                        .fontColor('#333333')
                        .maxLines(1)
                        .textOverflow({ overflow: TextOverflow.Ellipsis })
                      
                      Row({ space: 8 }) {
                        Text(this.getWeekDayName(course.day))
                          .fontSize(14)
                          .fontColor('#666666')
                        
                        Text(`第${course.startSection}-${course.endSection}节`)
                          .fontSize(14)
                          .fontColor('#666666')
                        
                        // Text(this.getWeekTypeName(course.weekType))
                        //   .fontSize(14)
                        //   .fontColor('#666666')
                      }
                      .margin({ top: 4 })
                      
                      Text(`${course.teacher} · ${course.location}`)
                        .fontSize(14)
                        .fontColor('#999999')
                        .margin({ top: 4 })
                        .maxLines(1)
                        .textOverflow({ overflow: TextOverflow.Ellipsis })
                    }
                    .alignItems(HorizontalAlign.Start)
                    .layoutWeight(1)
                    
                    Column() {
                      Row() {
                        Text('详情')
                          .fontSize(14)
                          .fontColor('#007DFF')
                      }
                      .padding({ left: 12, right: 12, top: 6, bottom: 6 })
                      .borderRadius(12)
                      .backgroundColor('#F0F7FF')
                    }
                  }
                  .width('100%')
                  .padding(16)
                  .backgroundColor('#FFFFFF')
                  .borderRadius(12)
                  .onClick(() => {
                    this.onCourseClick(course);
                  })
                }
                .margin({ bottom: 8 })
              }, (course: Course) => course.id)
            }
            .width('100%')
            .layoutWeight(1)
            .padding({ left: 16, right: 16, bottom: 16 })
          }
        }
        .width('100%')
        .layoutWeight(1)
        .backgroundColor('#F5F5F5')
      }
      .width('100%')
      .height('100%')
      
      if (this.showCourseDetail && this.selectedCourse) {
        this.CourseDetailDialog()
      }
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  CourseDetailDialog() {
    if (!this.selectedCourse) {
      Column() {}
    } else {
      Column() {
        Row() {
          Text('课程详情')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .layoutWeight(1)
          
          Button() {
            Text('×')
              .fontSize(24)
              .fontColor('#666666')
          }
          .width(32)
          .height(32)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.showCourseDetail = false;
            this.selectedCourse = null;
          })
        }
        .width('100%')
        .padding({ bottom: 20 })

        Column({ space: 16 }) {
          Column({ space: 8 }) {
            Text('课程名称')
              .fontSize(14)
              .fontColor('#666666')
            
            Text(this.selectedCourse.name)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333333')
          }
          .width('100%')
          .alignItems(HorizontalAlign.Start)

          Row({ space: 16 }) {
            Column({ space: 8 }) {
              Text('授课教师')
                .fontSize(14)
                .fontColor('#666666')
              
              Text(this.selectedCourse.teacher)
                .fontSize(16)
                .fontColor('#333333')
            }
            .layoutWeight(1)
            .alignItems(HorizontalAlign.Start)
            
            Column({ space: 8 }) {
              Text('上课地点')
                .fontSize(14)
                .fontColor('#666666')
              
              Text(this.selectedCourse.location)
                .fontSize(16)
                .fontColor('#333333')
            }
            .layoutWeight(1)
            .alignItems(HorizontalAlign.Start)
          }
          .width('100%')

          Row({ space: 16 }) {
            Column({ space: 8 }) {
              Text('上课时间')
                .fontSize(14)
                .fontColor('#666666')
              
              Text(this.getWeekDayName(this.selectedCourse.day))
                .fontSize(16)
                .fontColor('#333333')
            }
            .layoutWeight(1)
            .alignItems(HorizontalAlign.Start)
            
            Column({ space: 8 }) {
              Text('节次')
                .fontSize(14)
                .fontColor('#666666')
              
              Text(`第${this.selectedCourse.startSection}-${this.selectedCourse.endSection}节`)
                .fontSize(16)
                .fontColor('#333333')
            }
            .layoutWeight(1)
            .alignItems(HorizontalAlign.Start)
          }
          .width('100%')

          Column({ space: 8 }) {
            Text('备注')
              .fontSize(14)
              .fontColor('#666666')
            
            Text(this.selectedCourse.remark || '无')
              .fontSize(16)
              .fontColor('#333333')
          }
          .width('100%')
          .alignItems(HorizontalAlign.Start)

          Column({ space: 8 }) {
            Text('课程颜色')
              .fontSize(14)
              .fontColor('#666666')
            
            Row() {
              Text(this.selectedCourse.color)
                .fontSize(16)
                .fontColor('#333333')
              
              Row()
                .width(30)
                .height(30)
                .backgroundColor(this.selectedCourse.color)
                .borderRadius(6)
                .margin({ left: 12 })
            }
          }
          .width('100%')
          .alignItems(HorizontalAlign.Start)
        }
        .width('100%')
        .padding({ left: 20, right: 20 })

        Button('关闭')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .backgroundColor('#007DFF')
          .borderRadius(20)
          .width(120)
          .height(40)
          .margin({ top: 20 })
          .onClick(() => {
            this.showCourseDetail = false;
            this.selectedCourse = null;
          })
      }
      .width('90%')
      .padding(24)
      .backgroundColor('#FFFFFF')
      .borderRadius(16)
      .shadow({
        radius: 20,
        color: '#1A000000',
        offsetX: 0,
        offsetY: 4
      })
    }
  }
}
