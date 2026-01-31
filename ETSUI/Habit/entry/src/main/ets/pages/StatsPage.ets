import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import { DateUtil } from '../common/utils/DateUtil';
import { HeatMap } from '../view/component/HeatMap';
import { BarChart } from '../view/component/BarChart';
import { PieChart, PieData } from '../view/component/PieChart';
import RecordRepository from '../model/repository/RecordRepository';
import HabitRepository from '../model/repository/HabitRepository';
import Habit from '../common/bean/Habit';

@Entry
@Component
struct StatsPage {
  @State recordDates: string[] = [];
  @State totalCheckIns: number = 0;
  @State continuousDays: number = 0;
  @State barData: number[] = [];
  @State barLabels: string[] = [];
  @State pieData: PieData[] = [];

  aboutToAppear() {
    this.loadData();
  }

  loadData() {
    RecordRepository.getAllRecordDates().then((dates: string[]) => {
      this.recordDates = dates;
      this.totalCheckIns = dates.length;
      this.continuousDays = this.calculateStreak(dates);
      this.calculateBarData(dates);
      this.calculatePieData();
    }).catch((err: Error) => {
      console.error('[StatsPage] 加载失败: ' + JSON.stringify(err));
    });
  }

  // 计算连续天数 (逻辑保留，虽然界面不显示了)
  calculateStreak(dates: string[]): number {
    if (dates.length === 0) return 0;
    let uniqueDates = Array.from(new Set(dates)).sort().reverse();
    let now = new Date();
    let todayStr = DateUtil.formatDate(now.getTime());
    let yesterday = new Date();
    yesterday.setDate(now.getDate() - 1);
    let yesterdayStr = DateUtil.formatDate(yesterday.getTime());

    let streak = 0;
    let checkDate = new Date();

    if (uniqueDates.includes(todayStr)) {
      // 今天有
    } else if (uniqueDates.includes(yesterdayStr)) {
      checkDate.setDate(now.getDate() - 1);
    } else {
      return 0;
    }

    while (true) {
      let dateStr = DateUtil.formatDate(checkDate.getTime());
      if (uniqueDates.includes(dateStr)) {
        streak++;
        checkDate.setDate(checkDate.getDate() - 1);
      } else {
        break;
      }
    }
    return streak;
  }

  calculateBarData(dates: string[]) {
    let resultCounts = [0, 0, 0, 0, 0, 0, 0];
    let resultLabels: string[] = [];
    let now = new Date();
    for (let i = 6; i >= 0; i--) {
      let targetDate = new Date();
      targetDate.setDate(now.getDate() - i);
      let dateStr = DateUtil.formatDate(targetDate.getTime());
      let month = (targetDate.getMonth() + 1).toString().padStart(2, '0');
      let day = targetDate.getDate().toString().padStart(2, '0');
      resultLabels.push(`${month}-${day}`);
      let count = dates.filter(d => d === dateStr).length;
      resultCounts[6 - i] = count;
    }
    this.barData = resultCounts;
    this.barLabels = resultLabels;
  }

  async calculatePieData() {
    try {
      let results: Object[] = await Promise.all([
        HabitRepository.queryAll(),
        RecordRepository.getAllRecordIds()
      ]);
      let allHabits = results[0] as Habit[];
      let recordIds = results[1] as number[];
      let countMap = new Map<number, number>();
      recordIds.forEach((id: number) => {
        let current = countMap.get(id) || 0;
        countMap.set(id, current + 1);
      });
      let result: PieData[] = [];
      allHabits.forEach((h: Habit) => {
        let count = countMap.get(h.id) || 0;
        if (count > 0) {
          result.push(new PieData(h.name, count, h.color));
        }
      });
      this.pieData = result;
    } catch (err) {
      console.error('[StatsPage] 饼图失败: ' + JSON.stringify(err));
    }
  }

  async generateMockData() {
    try {
      let habits = await HabitRepository.queryAll();
      if (habits.length === 0) {
        promptAction.showToast({ message: '请先在首页创建一个习惯！' });
        return;
      }

      promptAction.showToast({ message: '正在生成...' });

      let promises: Promise<number>[] = [];
      const now = new Date();

      for (let i = 1; i < 90; i++) {
        if (Math.random() > 0.3) {
          let targetDate = new Date();
          targetDate.setDate(now.getDate() - i);
          let randomHabit = habits[Math.floor(Math.random() * habits.length)];
          promises.push(RecordRepository.insertMockData(randomHabit.id, targetDate.getTime()));
        }
      }
      promises.push(RecordRepository.insertMockData(habits[0].id, new Date().getTime()));

      await Promise.all(promises);
      promptAction.showToast({ message: '生成完毕，正在刷新...' });

      setTimeout(() => {
        this.loadData();
      }, 500);

    } catch (err) {
      console.error('生成失败: ' + JSON.stringify(err));
    }
  }

  async forceReset() {
    try {
      await RecordRepository.cleanAll();
      promptAction.showToast({ message: '数据已清空' });
      this.loadData();
    } catch (err) {
      console.error('重置失败: ' + JSON.stringify(err));
    }
  }

  build() {
    Column() {
      // 导航栏
      Row() {
        Button({ type: ButtonType.Circle }) {
          Text('<').fontSize(26).fontWeight(FontWeight.Bold)
        }
        .width(40).height(40).backgroundColor('#FFF').onClick(() => router.back())
        Text("数据统计").fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 15 })
      }
      .width('100%').padding(15)

      Scroll() {
        Column() {
          // ✂️✂️✂️ 这里删除了原来的 Row { StatCard... } ✂️✂️✂️

          // 1. 热力图 (现在是第一个)
          HeatMap({ recordDates: this.recordDates })
          // .margin({ top: 15 }) // 不需要顶距了，因为上面空了
            .shadow({ radius: 10, color: '#0F000000', offsetY: 5 })

          // 2. 柱状图
          BarChart({ data: this.barData, labels: this.barLabels })
            .margin({ top: 15 })
            .shadow({ radius: 10, color: '#0F000000', offsetY: 5 })

          // 3. 饼图
          PieChart({ data: this.pieData })
            .margin({ top: 15 })
            .shadow({ radius: 10, color: '#0F000000', offsetY: 5 })

          // 按钮区
          Button('⚡ 生成模拟数据')
            .width('100%').height(50).backgroundColor('#333')
            .margin({ top: 30 })
            .onClick(() => this.generateMockData())

          Button('🗑️ 强制清空旧数据')
            .width('100%').height(40).backgroundColor('#FF4444')
            .margin({ top: 15, bottom: 50 })
            .onClick(() => {
              AlertDialog.show({
                title: '清空数据',
                message: '确定要清空所有打卡记录吗？',
                primaryButton: { value: '取消', action: () => {} },
                secondaryButton: {
                  value: '确认清空',
                  fontColor: '#FF0000',
                  action: () => this.forceReset()
                }
              })
            })
        }
        .padding(15)
      }
    }
    .height('100%').backgroundColor('#F1F3F5')
  }

  // ✂️✂️✂️ 这里删除了 @Builder StatCard() 方法 ✂️✂️✂️
}