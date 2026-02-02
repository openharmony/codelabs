/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import router from '@ohos.router';
import Habit from '../common/bean/Habit';
import { CommonConstants } from '../common/constants/CommonConstants';
import { DateUtil } from '../common/utils/DateUtil';
import RdbHelper from '../model/database/RdbHelper';
import HabitRepository from '../model/repository/HabitRepository';
import { HabitItem } from '../view/component/HabitItem';
import { ProgressCircle } from '../view/component/ProgressCircle';
import RecordRepository from '../model/repository/RecordRepository';
import Category from '../common/bean/Category';
import CategoryRepository from '../model/repository/CategoryRepository';
// 🆕 引入打卡输入弹窗
import { CheckInDialog } from '../view/dialog/CheckInDialog';

@Entry
@Component
struct Index {
  // === 页面状态 ===
  @State habits: Array<Habit> = []; // 当前显示的习惯列表
  @State todayProgress: number = 0; // 今日进度百分比
  @State isLoading: boolean = true; // 加载状态
  @State categories: Category[] = []; // 分类列表
  @State currentCategoryId: number = 1; // 当前选中的分类ID，默认1(全部)
  @State allHabitsCache: Habit[] = []; // 本地缓存所有习惯
  // 🆕 记录当前正在操作的习惯 (为了传给弹窗)
  @State currentOperatingHabit: Habit | null = null;
  // 🆕 定义打卡弹窗控制器
  checkInDialogController: CustomDialogController = new CustomDialogController({
    builder: CheckInDialog({
      onConfirm: (mood: number, note: string) => {
        // 弹窗确认后的回调
        if (this.currentOperatingHabit) {
          this.executeCheckIn(this.currentOperatingHabit, mood, note);
        }
      }
    }),
    alignment: DialogAlignment.Center,
    customStyle: true
  });

  // 页面显示时触发 (包括从编辑页返回)
  onPageShow() {
    this.initSystem();
  }

  // 初始化系统
  async initSystem() {
    const context = getContext(this);
    // 1. 初始化数据库
    await RdbHelper.initRdb(context);

    // 2. 加载分类数据
    this.categories = await CategoryRepository.queryAll();

    // 3. 刷新习惯列表数据
    this.refreshData();
  }

  // 刷新数据的核心方法
  refreshData() {
    this.isLoading = true;
    // 并行查询：所有习惯 + 今日打卡记录ID + 今日打卡次数Map
    Promise.all([
      HabitRepository.queryAll(),
      RecordRepository.getTodayCompletedIds(),
      RecordRepository.getTodayCompletedNums()
    ]).then((results: Object[]) => {

      // 强制类型转换，解决编译报错
      let allHabits = results[0]! as Habit[];
      let completedNums = results[2]! as Map<number, number>;

      // 1. 标记进度和完成状态
      allHabits.forEach(habit => {
        // 从 Map 中获取当前打卡次数，没有则为 0
        habit.currentCount = completedNums.get(habit.id) || 0;
        // 判断标准：当前次数 >= 目标次数
        habit.isChecked = habit.currentCount >= habit.targetCount;
      });

      // 2. 存入缓存池
      this.allHabitsCache = allHabits;

      // 3. 触发筛选和进度计算
      this.filterHabits();

      this.isLoading = false;
    }).catch((err: Error) => {
      console.error('[Index] 刷新失败: ' + JSON.stringify(err));
      this.isLoading = false;
    });
  }

  // 🔍 筛选逻辑
  filterHabits() {
    if (this.currentCategoryId === 1) {
      this.habits = this.allHabitsCache;
    } else {
      this.habits = this.allHabitsCache.filter(h => h.categoryId === this.currentCategoryId);
    }
    this.calculateProgress();
  }

  // 📊 计算进度
  calculateProgress() {
    let validFinishedCount = this.habits.filter(h => h.isChecked).length;
    let totalCount = this.habits.length;

    if (totalCount > 0) {
      this.todayProgress = (validFinishedCount / totalCount) * 100;
    } else {
      this.todayProgress = 0;
    }
  }

  // ✅ 打卡逻辑 (支持多次打卡)
  // 修改：点击不再直接打卡，而是打开弹窗
  createRecord(habit: Habit) {
    if (habit.isChecked) {
      // 如果已经达标，通常不再打卡，或者你可以允许继续打
      return;
    }
    // 🆕 设置当前习惯并打开弹窗
    this.currentOperatingHabit = habit;
    this.checkInDialogController.open();
  }

  // 🆕 真正执行写入数据库的方法 (由弹窗回调调用)
  executeCheckIn(habit: Habit, mood: number, note: string) {
    // 1. 插入记录 (带心情和心得)
    RecordRepository.createRecord(habit.id, mood, note).then(() => {
      // 2. 刷新页面
      this.refreshData();
    }).catch((err: Error) => {
      console.error('打卡失败:' + JSON.stringify(err));
    });
  }

  // 🗑️ 删除习惯逻辑
  deleteHabit(id: number) {
    RecordRepository.deleteByHabitId(id).then(() => {
      return HabitRepository.deleteById(id);
    }).then(() => {
      this.refreshData();
    }).catch((err: Error) => {
      console.error('删除失败: ' + JSON.stringify(err));
    });
  }

  // ↩️ 撤销打卡逻辑 (反悔功能)
  backoutRecord(id: number) {
    RecordRepository.deleteLatestRecord(id).then(() => {
      this.refreshData();
    }).catch((err: Error) => {
      console.error('撤销失败: ' + JSON.stringify(err));
    });
  }

  build() {
    Stack({ alignContent: Alignment.BottomEnd }) {

      // === 0. 全局背景 (清新蓝白渐变) ===
      Column()
        .width('100%')
        .height('100%')
        .linearGradient({
          direction: GradientDirection.Bottom,
          colors: [
            ['#FFFFFF', 0.0], // 顶部白
            ['#F0F4FF', 0.5], // 中间淡蓝
            ['#E6F2FF', 1.0]// 底部蓝
          ]
        })

      Column() {
        // === 1. 顶部标题栏 ===
        Row() {
          // 左侧：设置按钮
          Button({ type: ButtonType.Circle }) {
            Text('⚙️').fontSize(24)
          }
          .width(48)
          .height(48)
          .backgroundColor('#FFFFFF')
          .shadow({ radius: 8, color: '#1A000000', offsetY: 2 })
          .margin({ right: 12 })
          .onClick(() => {
            router.pushUrl({ url: 'pages/SettingPage' });
          })

          // 中间：标题
          Column() {
            Text(CommonConstants.HOME_TITLE)
              .fontSize(28)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333')

            Text(DateUtil.formatDate(DateUtil.now()))
              .fontSize(14)
              .fontColor('#888')
              .margin({ top: 4 })
          }
          .alignItems(HorizontalAlign.Start)

          Blank()

          // 右侧：统计按钮
          Button({ type: ButtonType.Circle }) {
            Text('📊').fontSize(22)
          }
          .width(48)
          .height(48)
          .backgroundColor('#FFFFFF')
          .shadow({ radius: 8, color: '#1A000000', offsetY: 2 })
          .onClick(() => {
            router.pushUrl({ url: 'pages/StatsPage' });
          })
        }
        .width('100%')
        .padding({
          left: 24,
          right: 24,
          top: 20,
          bottom: 10
        })
        .alignItems(VerticalAlign.Center)

        // === 2. 分类 Tab 栏 (胶囊样式) ===
        List({ space: 12 }) {
          ForEach(this.categories, (item: Category) => {
            ListItem() {
              Text(item.name)
                .fontSize(14)
                .fontWeight(this.currentCategoryId === item.id ? FontWeight.Bold : FontWeight.Medium)
                .fontColor(this.currentCategoryId === item.id ? Color.White : '#666')
                .backgroundColor(this.currentCategoryId === item.id ? '#333333' : '#FFFFFF')
                .padding({
                  left: 20,
                  right: 20,
                  top: 8,
                  bottom: 8
                })
                .borderRadius(20)
                .shadow({ radius: this.currentCategoryId === item.id ? 4 : 0, color: '#33000000', offsetY: 2 })
                .onClick(() => {
                  this.currentCategoryId = item.id;
                  this.filterHabits();
                })
                .animation({ duration: 300 })
            }
          })
        }
        .listDirection(Axis.Horizontal)
        .width('100%')
        .height(60)
        .padding({ left: 24, right: 24 })
        .scrollBar(BarState.Off)

        // === 3. 仪表盘区域 ===
        Row() {
          ProgressCircle({
            percent: this.todayProgress,
            color: '#007DFF'
          })
        }
        .width('100%')
        .justifyContent(FlexAlign.Center)
        .padding({ top: 10, bottom: 20 })

        // === 4. 习惯列表区域 ===
        if (this.isLoading) {
          Column() {
            LoadingProgress().width(40).height(40).color('#007DFF')
            Text('加载中...').fontSize(14).fontColor('#999').margin({ top: 10 })
          }
          .width('100%').margin({ top: 50 })
        } else if (this.habits.length === 0) {
          Column() {
            Text('🍃').fontSize(60).opacity(0.3)
            Text('还没有习惯，快去创建一个吧！')
              .fontSize(14).fontColor('#999').margin({ top: 10 })
          }
          .width('100%').margin({ top: 50 })
        } else {
          List({ space: 16 }) {
            ForEach(this.habits, (item: Habit) => {
              ListItem() {
                HabitItem({
                  habit: item,
                  // 🔴 适配 HabitItem 组件的回调
                  clickDelete: (id: number) => {
                    this.deleteHabit(id);
                  },

                  // 打卡回调 (点击右侧圆圈)
                  onCheckIn: () => {
                    this.createRecord(item);
                  },

                  // 🆕 适配新页面：点击左侧跳转详情页
                  clickToEdit: () => {
                    router.pushUrl({
                      url: 'pages/HabitDetailPage',
                      params: { habitId: item.id }
                    });
                  },

                  // ↩️ 撤销回调 (长按按钮)
                  clickBackout: (id: number) => {
                    this.backoutRecord(id);
                  }
                })
              }
            }, (item: Habit) => JSON.stringify(item))
          }
          .width('100%')
          .layoutWeight(1)
          .padding({ left: 24, right: 24, bottom: 100 }) // 底部留白给按钮
          .scrollBar(BarState.Off)
        }
      }
      .width('100%')
      .height('100%')

      // === 5. 悬浮添加按钮 (渐变色) ===
      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('+')
          .fontSize(36)
          .fontColor(Color.White)
          .fontWeight(FontWeight.Medium)
          .margin({ bottom: 2 })
      }
      .width(64)
      .height(64)
      .linearGradient({
        angle: 135,
        colors: [['#007DFF', 0.0], ['#005BBB', 1.0]]
      })
      .shadow({ radius: 10, color: '#4D007DFF', offsetY: 5 })
      .margin({ right: 24, bottom: 36 })
      .onClick(() => {
        try {
          router.pushUrl({ url: 'pages/HabitEditPage' });
        } catch (err) {
          console.error("路由跳转错误: " + JSON.stringify(err));
        }
      })

    }
    .width('100%')
    .height('100%')
  }
}