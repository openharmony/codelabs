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
import HabitRepository from '../model/repository/HabitRepository';
import RecordRepository from '../model/repository/RecordRepository'; // 🆕 引入 RecordDetail 接口
import AchievementRepository, { Achievement } from '../model/repository/AchievementRepository';
import { CalendarView } from '../view/component/CalendarView';
import { TimeAnalysisCard } from '../view/component/TimeAnalysisCard';
// 🆕 引入详情弹窗
import { RecordDetailDialog } from '../view/dialog/RecordDetailDialog';

@Entry
@Component
struct HabitDetailPage {
  @State habit: Habit | null = null;
  @State achievements: Achievement[] = [];
  @State recordDateStrings: string[] = [];
  // 存储详细的时间戳 (用于时段分析)
  @State recordTimestamps: number[] = [];
  // 统计数据
  @State currentStreak: number = 0;
  @State maxStreak: number = 0;
  @State totalCount: number = 0;
  @State selectedAchievementName: string = '';
  // 🆕 详情弹窗所需数据
  @State detailDate: string = '';
  @State detailMood: number = 1;
  @State detailNote: string = '';
  // 🆕 详情弹窗控制器
  detailDialogController: CustomDialogController = new CustomDialogController({
    builder: RecordDetailDialog({
      dateStr: this.detailDate,
      mood: this.detailMood,
      note: this.detailNote
    }),
    alignment: DialogAlignment.Center,
    customStyle: true
  });

  async onPageShow() {
    const params = router.getParams() as Record<string, Object>;
    if (params && params.habitId) {
      const habitId = params.habitId as number;

      // 1. 加载基础数据 (只加载一次)
      this.habit = await HabitRepository.queryById(habitId);

      // 2. 加载动态数据 (提取为独立方法，方便打卡后刷新)
      await this.loadData();
    }
  }

  /**
   * 🆕 [核心修复] 数据加载方法
   * 打卡后调用此方法，强制刷新所有组件的数据（包括时段分析卡片）
   */
  async loadData() {
    if (!this.habit) {
      return;
    }
    const habitId = this.habit.id;

    // A. 刷新统计数据
    const stats = await RecordRepository.getStreakStats(habitId);
    this.currentStreak = stats.currentStreak;
    this.maxStreak = stats.maxStreak;
    this.totalCount = stats.totalCount;

    // B. 刷新日历热力图
    this.recordDateStrings = await RecordRepository.getRecordDateStrings(habitId);

    // C. ❗刷新详细时间戳 (这里更新后，TimeAnalysisCard 会自动重绘)
    this.recordTimestamps = await RecordRepository.getRecordTimestamps(habitId);

    // D. 刷新勋章状态
    let allAchievements = await AchievementRepository.queryAll();
    this.achievements = this.calculateAchievementState(allAchievements);
  }

  // 🆕 打开详情弹窗的逻辑
  async openRecordDetail(dateStr: string) {
    if (!this.habit) {
      return;
    }

    // 查询那天的记录
    const detail = await RecordRepository.getRecordByDate(this.habit.id, dateStr);

    if (detail) {
      this.detailDate = dateStr;
      this.detailMood = detail.mood || 1; // 默认值保护
      this.detailNote = detail.note || '';

      this.detailDialogController.open();
    } else {
      // 没查到记录（可能是单纯的补卡或者数据问题）
      // 可以在这里加一个 Prompt.showToast 提示，或者什么都不做
    }
  }

  calculateAchievementState(list: Achievement[]): Achievement[] {
    return list.map(item => {
      let isUnlockedNow = false;
      switch (item.unlockCondition) {
        case 'total_3':
          if (this.totalCount >= 3) {
            isUnlockedNow = true;
          }
          break;
        case 'total_7':
          if (this.totalCount >= 7) {
            isUnlockedNow = true;
          }
          break;
        case 'total_15':
          if (this.totalCount >= 15) {
            isUnlockedNow = true;
          }
          break;
        case 'total_30':
          if (this.totalCount >= 30) {
            isUnlockedNow = true;
          }
          break;
        case 'total_50':
          if (this.totalCount >= 50) {
            isUnlockedNow = true;
          }
          break;
        case 'total_100':
          if (this.totalCount >= 100) {
            isUnlockedNow = true;
          }
          break;
        case 'total_365':
          if (this.totalCount >= 365) {
            isUnlockedNow = true;
          }
          break;

        case 'streak_3':
          if (this.maxStreak >= 3) {
            isUnlockedNow = true;
          }
          break;
        case 'streak_7':
          if (this.maxStreak >= 7) {
            isUnlockedNow = true;
          }
          break;
        case 'streak_15':
          if (this.maxStreak >= 15) {
            isUnlockedNow = true;
          }
          break;
        case 'streak_30':
          if (this.maxStreak >= 30) {
            isUnlockedNow = true;
          }
          break;
        case 'streak_50':
          if (this.maxStreak >= 50) {
            isUnlockedNow = true;
          }
          break;
        case 'streak_100':
          if (this.maxStreak >= 100) {
            isUnlockedNow = true;
          }
          break;
        case 'streak_365':
          if (this.maxStreak >= 365) {
            isUnlockedNow = true;
          }
          break;
      }
      item.isUnlocked = isUnlockedNow;
      return item;
    });
  }

  @Builder
  AchievementPopupBuilder(desc: string) {
    Text(desc)
      .fontSize(12)
      .fontColor(Color.White)
      .padding({
        left: 10,
        right: 10,
        top: 8,
        bottom: 8
      })
      .backgroundColor('rgba(0,0,0,0.8)')
      .borderRadius(8)
  }

  @Builder
  AchievementGrid(list: Achievement[]) {
    Flex({ wrap: FlexWrap.Wrap }) {
      ForEach(list, (item: Achievement) => {
        Column() {
          Image($r(`app.media.${item.icon}`))
            .width(50)
            .height(50)
            .grayscale(item.isUnlocked ? 0 : 1)
            .opacity(item.isUnlocked ? 1 : 0.4)
            .animation({ duration: 300 })

          Text(item.name)
            .fontSize(12)
            .margin({ top: 6 })
            .fontColor(item.isUnlocked ? '#333' : '#999')
        }
        .width('25%').margin({ bottom: 20 })
        .onClick(() => {
          if (this.selectedAchievementName === item.name) {
            this.selectedAchievementName = '';
          } else {
            this.selectedAchievementName = item.name;
          }
        })
        .bindPopup(this.selectedAchievementName === item.name, {
          builder: this.AchievementPopupBuilder(item.description),
          placement: Placement.Top,
          mask: false,
          onStateChange: (e) => {
            if (!e.isVisible) {
              this.selectedAchievementName = '';
            }
          }
        })
      })
    }
  }

  build() {
    Column() {
      // --- 顶部导航栏 ---
      Row() {
        Text('<')
          .fontSize(30).fontWeight(FontWeight.Bold).fontColor('#333')
          .onClick(() => router.back())
        Text('习惯详情').fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 16 })
        Blank()
        Text('编辑').fontColor('#007DFF')
          .onClick(() => {
            router.pushUrl({
              url: 'pages/HabitEditPage',
              params: { habitId: this.habit?.id }
            })
          })
      }
      .width('100%')
      .padding({
        left: 20,
        right: 20,
        top: 12,
        bottom: 12
      })
      .backgroundColor('#F1F3F5')

      Scroll() {
        Column({ space: 12 }) {
          // 1. 习惯信息头
          if (this.habit) {
            Row() {
              Circle().width(12).height(12).fill(this.habit.color)
              Text(this.habit.name).fontSize(24).fontWeight(FontWeight.Bold).margin({ left: 12 })
            }
            .cardStyle() // 🆕 应用通用卡片样式
          }

          // 2. 统计数据卡片
          Row() {
            Column() {
              Text(this.currentStreak.toString()).statNumStyle() // 🆕 应用数字样式
              Text('当前连续').statLabelStyle() // 🆕 应用标签样式
            }.layoutWeight(1)

            Column() {
              Text(this.maxStreak.toString()).statNumStyle()
              Text('历史最高').statLabelStyle()
            }.layoutWeight(1)

            Column() {
              Text(this.totalCount.toString()).statNumStyle()
              Text('累计打卡').statLabelStyle()
            }.layoutWeight(1)
          }
          .cardStyle() // 🆕 应用通用卡片样式

          // 3. 热力图组件
          Column() {
            CalendarView({
              recordDates: this.recordDateStrings,
              // ✅ 修复点：加上 : string 类型注解
              onDateClick: async (dateStr: string) => {
                await this.loadData();
                await this.openRecordDetail(dateStr);
              }
            })
          }
          .width('100%').backgroundColor(Color.White).borderRadius(16) // 热力图无内边距，保持原样

          // 4. 时段分析卡片
          TimeAnalysisCard({ timeStamps: this.recordTimestamps })

          // 5. 成就勋章墙
          this.AchievementSection()
        }
        .padding({ left: 16, right: 16, bottom: 20 })
      }
      .layoutWeight(1)
    }
    .width('100%').height('100%').backgroundColor('#F1F3F5')
  }

  @Builder
  AchievementSection() {
    Column() {
      Text('成就勋章').sectionTitleStyle() // 🆕 应用标题样式

      if (this.achievements.length === 0) {
        Text('暂无成就数据').fontSize(14).fontColor('#999')
      } else {
        Text('🌟 累计里程碑').milestoneTitleStyle() // 🆕 应用副标题样式

        this.AchievementGrid(this.achievements.filter(item => item.unlockCondition.startsWith('total')))

        Divider().color('#F1F3F5').margin({ top: 10, bottom: 20 })

        Text('🔥 连胜里程碑').milestoneTitleStyle()

        this.AchievementGrid(this.achievements.filter(item => item.unlockCondition.startsWith('streak')))
      }
    }
    .cardStyle() // 🆕 应用通用卡片样式
  }
}

// =================================================================
// 🆕 底部扩展样式定义 (@Extend & @Styles)
// =================================================================

/**
 * 统计卡片中的数字样式
 */
@Extend(Text)
function statNumStyle() {
  .fontSize(18)
  .fontWeight(FontWeight.Bold)
  .fontColor('#007DFF')
  .textAlign(TextAlign.Center)
}

/**
 * 统计卡片中的说明文字样式
 */
@Extend(Text)
function statLabelStyle() {
  .fontSize(12)
  .fontColor('#666')
  .margin({ top: 4 })
  .fontWeight(FontWeight.Regular)
}

/**
 * 板块主标题样式 (如"成就勋章")
 */
@Extend(Text)
function sectionTitleStyle() {
  .fontSize(16)
  .fontWeight(FontWeight.Bold)
  .width('100%')
  .margin({ bottom: 15 })
  .fontColor('#333')
}

/**
 * 里程碑副标题样式 (如"累计里程碑")
 */
@Extend(Text)
function milestoneTitleStyle() {
  .fontSize(14)
  .fontColor('#666')
  .fontWeight(FontWeight.Bold)
  .width('100%')
  .margin({ bottom: 10, top: 5 })
}

/**
 * 通用白色卡片样式
 * 包含：白色背景、圆角、内边距
 */
@Styles
function cardStyle() {
  .width('100%')
  .padding(20)
  .backgroundColor(Color.White)
  .borderRadius(16)
}