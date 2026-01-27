// src/main/ets/pages/SchedulePage.ets
import router from '@ohos.router';
import common from '@ohos.app.ability.common';
import promptAction from '@ohos.promptAction';
import {
  ScheduleManager,
  CourseModel,
  ScheduleConfig,
  CourseColorUtil
} from '../common/ScheduleData';

/**
 * 课程表主页面
 * 展示一周七天的课程安排，支持点击编辑
 */
@Entry
@Component
struct SchedulePage {
  // --------------------------------------------------------------------------
  // 状态变量定义
  // --------------------------------------------------------------------------

  // 核心数据源
  @State courseList: CourseModel[] = [];

  // UI 交互控制
  @State isLoading: boolean = true;
  @State showEditDialog: boolean = false;

  // 当前编辑的课程坐标
  @State currentDayIndex: number = 0;
  @State currentPeriodIndex: number = 0;

  // 编辑表单临时状态（用于双向绑定）
  @State tempCourseName: string = '';
  @State tempClassroom: string = '';
  @State tempTeacher: string = '';
  @State tempTimeRange: string = '';

  // 页面标题配置
  private readonly pageTitle: string = '我的课程表';
  private readonly subTitle: string = '2024-2025学年 第1学期';

  // --------------------------------------------------------------------------
  // 生命周期方法
  // --------------------------------------------------------------------------

  /**
   * 页面即将显示时触发
   * 初始化管理器并加载数据
   */
  aboutToAppear() {
    this.initializeData();
  }

  /**
   * 初始化数据逻辑
   */
  private async initializeData() {
    this.isLoading = true;
    try {
      // 获取上下文
      const context = getContext(this) as common.UIAbilityContext;
      // 初始化管理器
      ScheduleManager.init(context);
      // 加载数据
      await this.refreshScheduleData();
    } catch (error) {
      console.error('SchedulePage: Initialization failed', error);
      promptAction.showToast({ message: '数据加载异常' });
    } finally {
      // 模拟一点加载延迟，提升体验
      setTimeout(() => {
        this.isLoading = false;
      }, 300);
    }
  }

  /**
   * 刷新课程数据
   */
  private async refreshScheduleData() {
    this.courseList = await ScheduleManager.getAllCourses();
  }

  // --------------------------------------------------------------------------
  // 业务逻辑方法
  // --------------------------------------------------------------------------

  /**
   * 根据坐标获取当前格子的课程数据
   */
  private getCourseByPosition(day: number, period: number): CourseModel | undefined {
    return this.courseList.find(c => c.dayIndex === day && c.periodIndex === period);
  }

  /**
   * 打开编辑弹窗
   * 预填充数据或重置表单
   */
  private openEditDialog(day: number, period: number) {
    this.currentDayIndex = day;
    this.currentPeriodIndex = period;

    const existCourse = this.getCourseByPosition(day, period);

    if (existCourse) {
      // 编辑模式：填充现有数据
      this.tempCourseName = existCourse.courseName;
      this.tempClassroom = existCourse.classroom;
      this.tempTeacher = existCourse.teacherName;
      this.tempTimeRange = existCourse.timeRange;
    } else {
      // 新增模式：清空表单，填入默认时间
      this.tempCourseName = '';
      this.tempClassroom = '';
      this.tempTeacher = '';
      this.tempTimeRange = ScheduleConfig.DEFAULT_TIMES[period] || '';
    }

    this.showEditDialog = true;
  }

  /**
   * 执行保存操作
   */
  private async handleSave() {
    // 1. 表单校验
    if (!this.tempCourseName || this.tempCourseName.trim().length === 0) {
      promptAction.showToast({ message: '请输入课程名称' });
      return;
    }

    // 2. 构建数据对象
    const courseToSave: CourseModel = {
      id: '', // Manager 会自动处理 ID
      dayIndex: this.currentDayIndex,
      periodIndex: this.currentPeriodIndex,
      courseName: this.tempCourseName.trim(),
      classroom: this.tempClassroom.trim() || '未知教室',
      teacherName: this.tempTeacher.trim() || '未知教师',
      timeRange: this.tempTimeRange.trim(),
      color: CourseColorUtil.getColorForText(this.tempCourseName.trim()),
      createTime: 0,
      updateTime: 0
    };

    // 3. 调用管理器保存
    const success = await ScheduleManager.saveCourse(courseToSave);

    // 4. 处理结果
    if (success) {
      await this.refreshScheduleData();
      this.showEditDialog = false;
      promptAction.showToast({ message: '课程保存成功' });
    } else {
      promptAction.showToast({ message: '保存失败，请重试' });
    }
  }

  /**
   * 执行删除操作
   */
  private async handleDelete() {
    // 调用管理器删除
    const success = await ScheduleManager.deleteCourse(this.currentDayIndex, this.currentPeriodIndex);

    if (success) {
      await this.refreshScheduleData();
      this.showEditDialog = false;
      promptAction.showToast({ message: '课程已移除' });
    } else {
      promptAction.showToast({ message: '删除失败，请重试' });
    }
  }

  // --------------------------------------------------------------------------
  // UI 构建方法 (Builders)
  // --------------------------------------------------------------------------

  /**
   * 构建页面顶部导航栏
   */
  @Builder
  buildNavigationBar() {
    Row() {
      // 返回按钮
      Button({ type: ButtonType.Normal, stateEffect: true }) {
        // 【修改点】：使用 Text 代替 Image
        Text('<')
          .fontSize(24)
          .fontColor('#007DFF') // 使用主题蓝色，或者 '#333333' 黑色
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
      }
      .width(40)
      .height(40)
      .backgroundColor(Color.Transparent)
      .borderRadius(20)
      .onClick(() => {
        router.back();
      })

      // 标题区域
      Column() {
        Text(this.pageTitle)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')

        Text(this.subTitle)
          .fontSize(12)
          .fontColor('#999999')
          .margin({ top: 2 })
      }
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Center)

      // 占位符，保持标题居中
      Blank().width(40)
    }
    .width('100%')
    .height(60)
    .padding({ left: 16, right: 16 })
    .backgroundColor(Color.White)
    .shadow({ radius: 4, color: '#0000000D', offsetY: 2 })
    .zIndex(10) // 确保在最上层
  }

  /**
   * 构建星期表头
   */
  @Builder
  buildWeekHeader() {
    Row() {
      // 左上角空白块（对应时间轴）
      Text('')
        .width('12%') // 对应左侧时间栏宽度
        .height(44)
        .backgroundColor('#F9F9F9')

      // 星期遍历
      ForEach(ScheduleConfig.WEEK_DAYS, (day: string, index: number) => {
        Column() {
          Text(day)
            .fontSize(14)
            .fontWeight(FontWeight.Medium)
            .fontColor(index === new Date().getDay() - 1 ? '#007DFF' : '#666666') // 高亮当天

          // 装饰点
          if (index === new Date().getDay() - 1) {
            Circle({ width: 4, height: 4 })
              .fill('#007DFF')
              .margin({ top: 2 })
          }
        }
        .width(`${88 / 7}%`) // 剩余宽度均分
        .height(44)
        .justifyContent(FlexAlign.Center)
        .backgroundColor('#F9F9F9')
      })
    }
    .width('100%')
    .height(44)
    .backgroundColor('#F9F9F9')
    .border({ width: { bottom: 1 }, color: '#E0E0E0' })
  }

  /**
   * 构建单个课程单元格
   */
  @Builder
  buildCourseCell(dayIndex: number, periodIndex: number) {
    Stack() {
      // 根据是否有数据渲染不同内容
      if (this.getCourseByPosition(dayIndex, periodIndex)) {
        // --- 有课程的样式 ---
        Column() {
          Text(this.getCourseByPosition(dayIndex, periodIndex)?.courseName)
            .fontSize(11)
            .fontWeight(FontWeight.Bold)
            .fontColor(Color.White)
            .maxLines(3)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
            .textAlign(TextAlign.Center)
            .lineHeight(14)

          Blank().height(4)

          Text('@' + this.getCourseByPosition(dayIndex, periodIndex)?.classroom)
            .fontSize(9)
            .fontColor('rgba(255,255,255,0.9)')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
        }
        .width('96%')
        .height('96%')
        // 动态获取背景色
        .backgroundColor(this.getCourseByPosition(dayIndex, periodIndex)?.color || '#CCCCCC')
        .borderRadius(6)
        .padding(4)
        .shadow({ radius: 2, color: '#0000001A', offsetY: 1 })
        .justifyContent(FlexAlign.Center)

      } else {
        // --- 空格子的样式 ---
        Column() {
          Text('+')
            .fontSize(16)
            .fontColor('#EEEEEE')
            .fontWeight(FontWeight.Bold)
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
      }
    }
    .width('100%')
    .height('100%')
    // 格子边框
    .border({ width: 0.5, color: '#F0F0F0' })
    .onClick(() => {
      this.openEditDialog(dayIndex, periodIndex);
    })
  }

  /**
   * 构建编辑课程的弹窗
   * 使用全屏遮罩 + 中心弹窗卡片的布局
   */
  @Builder
  buildEditDialogOverlay() {
    if (this.showEditDialog) {
      Stack() {
        // 1. 半透明遮罩层
        Column()
          .width('100%')
          .height('100%')
          .backgroundColor('rgba(0,0,0,0.4)')
          .onClick(() => {
            // 点击背景关闭
            this.showEditDialog = false;
          })
          .transition({ type: TransitionType.All, opacity: 0 })

        // 2. 弹窗主体内容
        Column() {
          // 标题栏
          Text('编辑课程信息')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .width('100%')
            .textAlign(TextAlign.Start)
            .margin({ bottom: 20 })

          // 当前时间提示
          Text(`${ScheduleConfig.WEEK_DAYS[this.currentDayIndex]} - ${ScheduleConfig.PERIODS[this.currentPeriodIndex]}`)
            .fontSize(14)
            .fontColor('#007DFF')
            .backgroundColor('#E8F4FF')
            .padding({ left: 10, right: 10, top: 6, bottom: 6 })
            .borderRadius(4)
            .margin({ bottom: 20 })
            .alignSelf(ItemAlign.Start)

          // 表单区域 - 课程名称
          this.buildInputGroup('课程名称', '请输入课程名称（必填）', this.tempCourseName, (val) => this.tempCourseName = val)

          // 表单区域 - 上课时间
          this.buildInputGroup('上课时间', '例如：08:00-09:40', this.tempTimeRange, (val) => this.tempTimeRange = val)

          // 表单区域 - 教室
          this.buildInputGroup('上课教室', '请输入教室', this.tempClassroom, (val) => this.tempClassroom = val)

          // 表单区域 - 教师
          this.buildInputGroup('授课教师', '请输入教师姓名', this.tempTeacher, (val) => this.tempTeacher = val)

          // 按钮区域
          Row({ space: 15 }) {
            // 删除/清空按钮
            Button('清空本节')
              .layoutWeight(1)
              .height(44)
              .backgroundColor('#FFEBEE')
              .fontColor('#D32F2F')
              .fontSize(15)
              .onClick(() => {
                this.handleDelete();
              })

            // 保存按钮
            Button('保存课程')
              .layoutWeight(1)
              .height(44)
              .backgroundColor('#007DFF')
              .fontColor(Color.White)
              .fontSize(15)
              .onClick(() => {
                this.handleSave();
              })
          }
          .width('100%')
          .margin({ top: 10 })

        }
        .width('85%')
        .backgroundColor(Color.White)
        .borderRadius(16)
        .padding(24)
        .shadow({ radius: 20, color: 'rgba(0,0,0,0.2)', offsetY: 10 })
        .transition({ type: TransitionType.Insert, translate: { y: 100 }, opacity: 0 })
        .transition({ type: TransitionType.Delete, opacity: 0 })
      }
      .width('100%')
      .height('100%')
      .zIndex(999) // 保证在最上层
    }
  }

  /**
   * 辅助构建器：通用的输入框组件
   */
  @Builder
  buildInputGroup(label: string, placeholder: string, value: string, onChange: (val: string) => void) {
    Column({ space: 6 }) {
      Text(label)
        .fontSize(14)
        .fontColor('#666666')
        .fontWeight(FontWeight.Medium)

      TextInput({ text: value, placeholder: placeholder })
        .width('100%')
        .height(44)
        .backgroundColor('#F5F7FA') // 浅灰色背景
        .borderRadius(8)
        .fontSize(15)
        .fontColor('#333333')
        .padding({ left: 12, right: 12 })
        .placeholderColor('#CCCCCC')
        .onChange((val) => {
          onChange(val);
        })
    }
    .width('100%')
    .alignItems(HorizontalAlign.Start)
    .margin({ bottom: 16 })
  }

  // --------------------------------------------------------------------------
  // 主布局渲染 (Build)
  // --------------------------------------------------------------------------

  build() {
    Stack() {
      Column() {
        // 1. 顶部导航
        this.buildNavigationBar()

        // 2. 课程表主体内容
        Column() {
          // 2.1 星期表头
          this.buildWeekHeader()

          // 2.2 课程表网格区域
          // 使用 Grid 布局：左侧1列时间 + 右侧7列课程
          Grid() {
            ForEach(ScheduleConfig.PERIODS, (periodName: string, pIndex: number) => {
              // --- 左侧时间段列 ---
              GridItem() {
                Column() {
                  Text((pIndex + 1).toString())
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#333333')

                  Text(periodName.substring(0, 2)) // 取"上午"/"下午"
                    .fontSize(10)
                    .fontColor('#999999')
                    .margin({ top: 2 })
                }
                .width('100%')
                .height('100%')
                .justifyContent(FlexAlign.Center)
                .backgroundColor('#F9F9F9')
                .border({ width: { bottom: 0.5 }, color: '#E0E0E0' })
              }
              .width('100%')
              .height(110) // 设定每一节课的高度

              // --- 右侧 7 天的课程格子 ---
              ForEach(ScheduleConfig.WEEK_DAYS, (day: string, dIndex: number) => {
                GridItem() {
                  this.buildCourseCell(dIndex, pIndex)
                }
                .height(110)
              })
            })
          }
          // 定义列宽比例：时间栏占 12%，其余 7 天均分剩余 88%
          .columnsTemplate('12fr 12.5fr 12.5fr 12.5fr 12.5fr 12.5fr 12.5fr 12.5fr')
          .rowsGap(0)
          .columnsGap(0)
          .layoutWeight(1) // 占满剩余高度
          .backgroundColor(Color.White)
        }
        .width('100%')
        .layoutWeight(1)
        .backgroundColor(Color.White)
      }
      .width('100%')
      .height('100%')

      // 3. 全局弹窗层
      this.buildEditDialogOverlay()

      // 4. 加载中遮罩 (可选)
      if (this.isLoading) {
        Column() {
          LoadingProgress()
            .width(50)
            .height(50)
            .color('#007DFF')
        }
        .width('100%')
        .height('100%')
        .backgroundColor('rgba(255,255,255,0.8)')
        .justifyContent(FlexAlign.Center)
        .zIndex(1000)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }
}