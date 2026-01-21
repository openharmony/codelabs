/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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
/**
 * 统计和可视化主页面
 */
import { LifeHubData, HealthRecord, MoodRecord, Goal, Skill } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

interface DayData {
  date: string;
  count: number;
}

interface TypeItem {
  type: string;
  name: string;
  count: number;
  percentage: number;
}

interface MoodItem {
  mood: string;
  label: string;
  count: number;
  percentage: number;
}

interface StatusItem {
  status: string;
  label: string;
  count: number;
  percentage: number;
}

interface CategoryItem {
  category: string;
  count: number;
  percentage: number;
}

@Entry
@Component
export struct StatisticsMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  @State healthRecords: HealthRecord[] = [];
  @State moodRecords: MoodRecord[] = [];
  @State goals: Goal[] = [];
  @State skills: Skill[] = [];
  @State isLoading: boolean = true;
  @State selectedTab: number = 0; // 0-健康, 1-心情, 2-目标, 3-学习
  @State typeItems: TypeItem[] = [];
  @State moodItems: MoodItem[] = [];
  @State statusItems: StatusItem[] = [];
  @State categoryItems: CategoryItem[] = [];
  @State last7Days: DayData[] = [];

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadAllData();
  }

  async loadAllData() {
    this.isLoading = true;
    try {
      this.healthRecords = await this.lifeHubData.getHealthRecords(undefined, 30);
      this.moodRecords = await this.lifeHubData.getMoodRecords(30);
      this.goals = await this.lifeHubData.getAllGoals();
      this.skills = await this.lifeHubData.getAllSkills();

      // 计算统计数据
      this.typeItems = this.getTypeItems();
      this.moodItems = this.getMoodItems();
      this.statusItems = this.getStatusItems();
      this.categoryItems = this.getCategoryItems();
      this.last7Days = this.getLast7DaysData();
    } catch (error) {
      console.error('StatisticsMain: 加载数据失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('数据统计')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)

        Blank()
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      // 标签页
      Row() {
        this.buildTab('健康', 0)
        this.buildTab('心情', 1)
        this.buildTab('目标', 2)
        this.buildTab('学习', 3)
      }
      .width('100%')
      .height(50)
      .backgroundColor(Color.White)
      .justifyContent(FlexAlign.SpaceAround)

      // 内容区域
      if (this.isLoading) {
        Column() {
          LoadingProgress()
            .width(50)
            .height(50)
            .margin({ top: 100 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        Scroll() {
          Column() {
            if (this.selectedTab === 0) {
              this.buildHealthStatistics()
            } else if (this.selectedTab === 1) {
              this.buildMoodStatistics()
            } else if (this.selectedTab === 2) {
              this.buildGoalStatistics()
            } else if (this.selectedTab === 3) {
              this.buildLearningStatistics()
            }
          }
          .width('100%')
          .padding(15)
        }
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  @Builder
  buildTab(title: string, index: number) {
    Column() {
      Text(title)
        .fontSize(16)
        .fontColor(this.selectedTab === index ? '#007DFF' : '#666')
        .fontWeight(this.selectedTab === index ? FontWeight.Bold : FontWeight.Normal)

      if (this.selectedTab === index) {
        Divider()
          .color('#007DFF')
          .strokeWidth(3)
          .width(30)
          .margin({ top: 5 })
      }
    }
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .onClick(() => {
      this.selectedTab = index;
    })
  }

  @Builder
  buildHealthStatistics() {
    Column() {
      // 统计卡片
      Row() {
        this.buildStatCard('总记录数', this.healthRecords.length.toString(), '📊', '#4CAF50')
        this.buildStatCard('本周记录', this.getThisWeekCount(this.healthRecords).toString(), '📅', '#2196F3')
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ bottom: 15 })

      // 类型分布
      Text('类型分布')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .textAlign(TextAlign.Start)
        .margin({ bottom: 10 })

      Column() {
        this.buildTypeDistribution()
      }
      .width('100%')
      .padding(15)
      .backgroundColor(Color.White)
      .borderRadius(12)
      .margin({ bottom: 15 })

      // 最近趋势
      Text('最近趋势')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .textAlign(TextAlign.Start)
        .margin({ bottom: 10 })

      Column() {
        this.buildTrendChart()
      }
      .width('100%')
      .height(200)
      .padding(15)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .width('100%')
  }

  @Builder
  buildMoodStatistics() {
    Column() {
      Row() {
        this.buildStatCard('总记录数', this.moodRecords.length.toString(), '😊', '#E91E63')
        this.buildStatCard('平均评分', this.getAverageMoodScore().toFixed(1), '⭐', '#FF9800')
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ bottom: 15 })

      Text('心情分布')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .textAlign(TextAlign.Start)
        .margin({ bottom: 10 })

      Column() {
        this.buildMoodDistribution()
      }
      .width('100%')
      .padding(15)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .width('100%')
  }

  @Builder
  buildGoalStatistics() {
    Column() {
      Row() {
        this.buildStatCard('总目标数', this.goals.length.toString(), '🎯', '#FF9800')
        this.buildStatCard('完成率', this.getGoalCompletionRate() + '%', '✅', '#4CAF50')
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ bottom: 15 })

      Text('目标状态')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .textAlign(TextAlign.Start)
        .margin({ bottom: 10 })

      Column() {
        this.buildGoalStatus()
      }
      .width('100%')
      .padding(15)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .width('100%')
  }

  @Builder
  buildLearningStatistics() {
    Column() {
      Row() {
        this.buildStatCard('技能总数', this.skills.length.toString(), '📚', '#9C27B0')
        this.buildStatCard('平均等级', this.getAverageSkillLevel().toFixed(1), '📈', '#2196F3')
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ bottom: 15 })

      Text('技能分布')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .textAlign(TextAlign.Start)
        .margin({ bottom: 10 })

      Column() {
        this.buildSkillDistribution()
      }
      .width('100%')
      .padding(15)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .width('100%')
  }

  @Builder
  buildStatCard(title: string, value: string, icon: string, color: string) {
    Column() {
      Text(icon)
        .fontSize(32)
        .margin({ bottom: 8 })

      Text(value)
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor(color)
        .margin({ bottom: 4 })

      Text(title)
        .fontSize(14)
        .fontColor('#666')
    }
    .width('48%')
    .height(120)
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({
      radius: 4,
      color: '#10000000',
      offsetX: 0,
      offsetY: 2
    })
  }

  getTypeItems(): TypeItem[] {
    const typeCounts: Record<string, number> = {};
    for (let i = 0; i < this.healthRecords.length; i++) {
      const r = this.healthRecords[i];
      typeCounts[r.type] = (typeCounts[r.type] || 0) + 1;
    }

    const types: string[] = ['weight', 'blood_pressure', 'blood_sugar', 'exercise', 'sleep'];
    const typeNames: Record<string, string> = {
      'weight': '体重',
      'blood_pressure': '血压',
      'blood_sugar': '血糖',
      'exercise': '运动',
      'sleep': '睡眠'
    };

    const typeItems: TypeItem[] = [];
    const total = this.healthRecords.length;
    for (let i = 0; i < types.length; i++) {
      const type = types[i];
      const count = typeCounts[type] || 0;
      typeItems.push({
        type: type,
        name: typeNames[type] || type,
        count: count,
        percentage: total > 0 ? (count / total * 100) : 0
      });
    }
    return typeItems;
  }

  @Builder
  buildTypeDistribution() {
    ForEach(this.typeItems, (item: TypeItem) => {

      Column() {
        Row() {
          Text(item.name)
            .fontSize(14)
            .fontColor('#333')

          Blank()

          Text(`${item.count} (${item.percentage.toFixed(0)}%)`)
            .fontSize(14)
            .fontColor('#666')
        }
        .width('100%')
        .margin({ bottom: 8 })

        Progress({ value: item.percentage, total: 100, type: ProgressType.Linear })
          .width('100%')
          .height(8)
          .color('#4CAF50')
      }
      .width('100%')
      .margin({ bottom: 15 })
    }, (item: TypeItem) => item.type)
  }

  @Builder
  buildTrendChart() {
    Column() {
      ForEach(this.last7Days, (day: DayData) => {
        Row() {
          Text(day.date)
            .fontSize(12)
            .width(60)
            .fontColor('#666')

          Progress({ value: day.count, total: 10, type: ProgressType.Linear })
            .layoutWeight(1)
            .height(20)
            .color('#4CAF50')
            .margin({ left: 10, right: 10 })

          Text(day.count.toString())
            .fontSize(12)
            .width(30)
            .fontColor('#666')
        }
        .width('100%')
        .margin({ bottom: 10 })
      }, (day: DayData) => day.date)
    }
    .width('100%')
  }

  getMoodItems(): MoodItem[] {
    const moodCounts: Record<string, number> = {};
    for (let i = 0; i < this.moodRecords.length; i++) {
      const r = this.moodRecords[i];
      moodCounts[r.mood] = (moodCounts[r.mood] || 0) + 1;
    }

    const moods: string[] = ['😄', '😊', '😐', '😔', '😢'];
    const moodLabels: Record<string, string> = {
      '😄': '非常开心',
      '😊': '开心',
      '😐': '一般',
      '😔': '低落',
      '😢': '难过'
    };

    const moodItems: MoodItem[] = [];
    const total = this.moodRecords.length;
    for (let i = 0; i < moods.length; i++) {
      const mood = moods[i];
      const count = moodCounts[mood] || 0;
      moodItems.push({
        mood: mood,
        label: moodLabels[mood] || '',
        count: count,
        percentage: total > 0 ? (count / total * 100) : 0
      });
    }
    return moodItems;
  }

  @Builder
  buildMoodDistribution() {
    ForEach(this.moodItems, (item: MoodItem) => {
      Row() {
        Text(item.mood)
          .fontSize(24)
          .width(50)

        Text(item.label)
          .fontSize(14)
          .fontColor('#333')
          .layoutWeight(1)

        Text(`${item.count} (${item.percentage.toFixed(0)}%)`)
          .fontSize(14)
          .fontColor('#666')
      }
      .width('100%')
      .margin({ bottom: 15 })
    }, (item: MoodItem) => item.mood)
  }

  getStatusItems(): StatusItem[] {
    const statusCounts: Record<string, number> = {};
    for (let i = 0; i < this.goals.length; i++) {
      const g = this.goals[i];
      statusCounts[g.status] = (statusCounts[g.status] || 0) + 1;
    }

    const statuses: string[] = ['not_started', 'in_progress', 'completed', 'cancelled'];
    const statusLabels: Record<string, string> = {
      'not_started': '未开始',
      'in_progress': '进行中',
      'completed': '已完成',
      'cancelled': '已取消'
    };

    const statusItems: StatusItem[] = [];
    const total = this.goals.length;
    for (let i = 0; i < statuses.length; i++) {
      const status = statuses[i];
      const count = statusCounts[status] || 0;
      statusItems.push({
        status: status,
        label: statusLabels[status] || status,
        count: count,
        percentage: total > 0 ? (count / total * 100) : 0
      });
    }
    return statusItems;
  }

  @Builder
  buildGoalStatus() {
    ForEach(this.statusItems, (item: StatusItem) => {
      Column() {
        Row() {
          Text(item.label)
            .fontSize(14)
            .fontColor('#333')

          Blank()

          Text(`${item.count} (${item.percentage.toFixed(0)}%)`)
            .fontSize(14)
            .fontColor('#666')
        }
        .width('100%')
        .margin({ bottom: 8 })

        Progress({ value: item.percentage, total: 100, type: ProgressType.Linear })
          .width('100%')
          .height(8)
          .color('#FF9800')
      }
      .width('100%')
      .margin({ bottom: 15 })
    }, (item: StatusItem) => item.status)
  }

  getCategoryItems(): CategoryItem[] {
    const categoryCounts: Record<string, number> = {};
    for (let i = 0; i < this.skills.length; i++) {
      const s = this.skills[i];
      categoryCounts[s.category] = (categoryCounts[s.category] || 0) + 1;
    }

    const categoryKeys = Object.keys(categoryCounts);
    const categoryItems: CategoryItem[] = [];
    const total = this.skills.length;
    for (let i = 0; i < categoryKeys.length; i++) {
      const category = categoryKeys[i];
      const count = categoryCounts[category] || 0;
      categoryItems.push({
        category: category,
        count: count,
        percentage: (count / total * 100)
      });
    }
    return categoryItems;
  }

  @Builder
  buildSkillDistribution() {
    if (this.categoryItems.length === 0) {
      Text('暂无数据')
        .fontSize(14)
        .fontColor('#999')
        .width('100%')
        .textAlign(TextAlign.Center)
        .padding(20)
    } else {
      ForEach(this.categoryItems, (item: CategoryItem) => {
        Column() {
          Row() {
            Text(item.category)
              .fontSize(14)
              .fontColor('#333')

            Blank()

            Text(`${item.count} (${item.percentage.toFixed(0)}%)`)
              .fontSize(14)
              .fontColor('#666')
          }
          .width('100%')
          .margin({ bottom: 8 })

          Progress({ value: item.percentage, total: 100, type: ProgressType.Linear })
            .width('100%')
            .height(8)
            .color('#9C27B0')
        }
        .width('100%')
        .margin({ bottom: 15 })
      }, (item: CategoryItem) => item.category)
    }
  }

  getThisWeekCount(records: HealthRecord[]): number {
    const today = new Date();
    const weekStart = new Date(today);
    weekStart.setDate(today.getDate() - today.getDay());
    weekStart.setHours(0, 0, 0, 0);

    let count = 0;
    for (let i = 0; i < records.length; i++) {
      const recordDate = new Date(records[i].date);
      if (recordDate >= weekStart) {
        count++;
      }
    }
    return count;
  }

  getAverageMoodScore(): number {
    if (this.moodRecords.length === 0) {
      return 0;
    }
    let sum = 0;
    for (let i = 0; i < this.moodRecords.length; i++) {
      sum += this.moodRecords[i].score;
    }
    return sum / this.moodRecords.length;
  }

  getGoalCompletionRate(): number {
    if (this.goals.length === 0) {
      return 0;
    }
    let completed = 0;
    for (let i = 0; i < this.goals.length; i++) {
      if (this.goals[i].status === 'completed') {
        completed++;
      }
    }
    return Math.round((completed / this.goals.length) * 100);
  }

  getAverageSkillLevel(): number {
    if (this.skills.length === 0) {
      return 0;
    }
    let sum = 0;
    for (let i = 0; i < this.skills.length; i++) {
      sum += this.skills[i].level;
    }
    return sum / this.skills.length;
  }

  getLast7DaysData(): DayData[] {
    const result: DayData[] = [];
    const today = new Date();

    for (let i = 6; i >= 0; i--) {
      const date = new Date(today);
      date.setDate(today.getDate() - i);
      const dateStr = this.formatDate(date);
      let count = 0;
      for (let j = 0; j < this.healthRecords.length; j++) {
        if (this.healthRecords[j].date === dateStr) {
          count++;
        }
      }
      result.push({
        date: `${date.getMonth() + 1}/${date.getDate()}`,
        count: count
      });
    }

    return result;
  }

  formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }
}
