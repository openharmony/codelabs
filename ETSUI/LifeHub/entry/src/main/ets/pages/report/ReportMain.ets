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
 * 周报/月报生成页面
 */
import { LifeHubData, DashboardStats, HealthRecord, MoodRecord, Goal, Skill, TravelPlan, Diary } from '../../model/LifeHubData';

interface PeriodRecords {
  health: HealthRecord[];
  mood: MoodRecord[];
}

@Entry
@Component
export struct ReportMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State reportType: 'week' | 'month' = 'week';
  @State reportData: string = '';
  @State isLoading: boolean = false;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.generateReport();
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('生活报告')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
        
        Row() {
          Button('周报')
            .fontSize(14)
            .height(32)
            .backgroundColor(this.reportType === 'week' ? '#007DFF' : '#f0f0f0')
            .fontColor(this.reportType === 'week' ? Color.White : '#666')
            .onClick(() => {
              this.reportType = 'week';
              this.generateReport();
            })
            .margin({ right: 10 })

          Button('月报')
            .fontSize(14)
            .height(32)
            .backgroundColor(this.reportType === 'month' ? '#007DFF' : '#f0f0f0')
            .fontColor(this.reportType === 'month' ? Color.White : '#666')
            .onClick(() => {
              this.reportType = 'month';
              this.generateReport();
            })
        }
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      if (this.isLoading) {
        Column() {
          LoadingProgress()
            .width(50)
            .height(50)
            .margin({ top: 100 })
          Text('生成报告中...')
            .fontSize(16)
            .fontColor('#666')
            .margin({ top: 10 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        Scroll() {
          Column() {
            Text(this.reportData)
              .fontSize(16)
              .fontColor('#333')
              .lineHeight(28)
              .textAlign(TextAlign.Start)
              .width('100%')
          }
          .width('100%')
          .padding(20)
        }
        .layoutWeight(1)
        .margin({ top: 10 })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async generateReport() {
    this.isLoading = true;
    this.reportData = '';

    try {
      const stats = await this.lifeHubData.getDashboardStats();
      const healthRecords = await this.lifeHubData.getHealthRecords(undefined, this.reportType === 'week' ? 7 : 30);
      const goals = await this.lifeHubData.getAllGoals();
      const moodRecords = await this.lifeHubData.getMoodRecords(this.reportType === 'week' ? 7 : 30);
      const skills = await this.lifeHubData.getAllSkills();
      const travelPlans = await this.lifeHubData.getAllTravelPlans();
      const diaries = await this.lifeHubData.getAllDiaries();

      const period = this.reportType === 'week' ? '本周' : '本月';
      const periodRecords = this.reportType === 'week' ? 
        this.filterThisWeek(healthRecords, moodRecords) :
        this.filterThisMonth(healthRecords, moodRecords);

      let report = `📊 ${period}生活报告\n`;
      report += `生成时间: ${new Date().toLocaleString()}\n`;
      report += `${'='.repeat(30)}\n\n`;

      // 健康模块
      report += `💚 健康记录\n`;
      report += `  • ${period}记录数: ${periodRecords.health.length}条\n`;
      if (periodRecords.health.length > 0) {
        const avgWeight = this.getAverageValue(periodRecords.health, 'weight');
        const avgExercise = this.getAverageValue(periodRecords.health, 'exercise');
        const avgSleep = this.getAverageValue(periodRecords.health, 'sleep');
        if (avgWeight > 0) report += `  • 平均体重: ${avgWeight.toFixed(1)}kg\n`;
        if (avgExercise > 0) report += `  • 平均运动: ${avgExercise.toFixed(0)}分钟\n`;
        if (avgSleep > 0) report += `  • 平均睡眠: ${avgSleep.toFixed(1)}小时\n`;
      }
      report += `  • 总记录数: ${healthRecords.length}条\n\n`;

      // 目标模块
      report += `🎯 目标管理\n`;
      const activeGoals: Goal[] = [];
      const completedGoals: Goal[] = [];
      for (let i = 0; i < goals.length; i++) {
        if (goals[i].status === 'in_progress') {
          activeGoals.push(goals[i]);
        } else if (goals[i].status === 'completed') {
          completedGoals.push(goals[i]);
        }
      }
      report += `  • 进行中: ${activeGoals.length}个\n`;
      report += `  • 已完成: ${completedGoals.length}个\n`;
      report += `  • 总目标数: ${goals.length}个\n`;
      if (goals.length > 0) {
        let sum = 0;
        for (let i = 0; i < goals.length; i++) {
          sum += goals[i].progress;
        }
        const avgProgress = sum / goals.length;
        report += `  • 平均进度: ${avgProgress.toFixed(1)}%\n`;
      }
      report += `\n`;

      // 心情模块
      report += `😊 心情记录\n`;
      report += `  • ${period}记录数: ${periodRecords.mood.length}条\n`;
      if (periodRecords.mood.length > 0) {
        let sum = 0;
        for (let i = 0; i < periodRecords.mood.length; i++) {
          sum += periodRecords.mood[i].score;
        }
        const avgScore = sum / periodRecords.mood.length;
        report += `  • 平均心情评分: ${avgScore.toFixed(1)}/10\n`;
        const moodDistribution = this.getMoodDistribution(periodRecords.mood);
        report += `  • 心情分布: ${moodDistribution}\n`;
      }
      report += `  • 总记录数: ${moodRecords.length}条\n\n`;

      // 学习模块
      report += `📚 技能学习\n`;
      report += `  • 技能总数: ${skills.length}个\n`;
      if (skills.length > 0) {
        let sum = 0;
        for (let i = 0; i < skills.length; i++) {
          sum += skills[i].level;
        }
        const avgLevel = sum / skills.length;
        report += `  • 平均等级: ${avgLevel.toFixed(1)}/10\n`;
        const categories = this.getSkillCategories(skills);
        let categoryStr = '';
        for (let i = 0; i < categories.length; i++) {
          if (i > 0) categoryStr += ', ';
          categoryStr += categories[i];
        }
        report += `  • 技能分类: ${categoryStr}\n`;
      }
      report += `\n`;

      // 旅行模块
      report += `✈️ 旅行计划\n`;
      const activePlans: TravelPlan[] = [];
      for (let i = 0; i < travelPlans.length; i++) {
        if (travelPlans[i].status === 'ongoing') {
          activePlans.push(travelPlans[i]);
        }
      }
      report += `  • 进行中: ${activePlans.length}个\n`;
      report += `  • 总计划数: ${travelPlans.length}个\n\n`;

      // 日记模块
      report += `📝 日记记录\n`;
      const periodDiaries = this.reportType === 'week' ? 
        this.filterThisWeekDiaries(diaries) : 
        this.filterThisMonthDiaries(diaries);
      report += `  • ${period}日记数: ${periodDiaries.length}篇\n`;
      report += `  • 总日记数: ${diaries.length}篇\n\n`;

      // 总结
      report += `${'='.repeat(30)}\n`;
      report += `📈 总结\n`;
      const totalRecords = periodRecords.health.length + periodRecords.mood.length + 
                          periodDiaries.length;
      report += `  • ${period}总活动: ${totalRecords}次\n`;
      
      if (this.reportType === 'week') {
        const dailyAvg = (totalRecords / 7).toFixed(1);
        report += `  • 日均活动: ${dailyAvg}次\n`;
      } else {
        const dailyAvg = (totalRecords / 30).toFixed(1);
        report += `  • 日均活动: ${dailyAvg}次\n`;
      }

      this.reportData = report;
    } catch (error) {
      console.error('ReportMain: 生成报告失败:', error);
      this.reportData = '生成报告时出错，请稍后重试。';
    } finally {
      this.isLoading = false;
    }
  }

  filterThisWeek(health: HealthRecord[], mood: MoodRecord[]): PeriodRecords {
    const today = new Date();
    const weekStart = new Date(today);
    weekStart.setDate(today.getDate() - today.getDay());
    weekStart.setHours(0, 0, 0, 0);

    const filteredHealth: HealthRecord[] = [];
    const filteredMood: MoodRecord[] = [];
    
    for (let i = 0; i < health.length; i++) {
      const recordDate = new Date(health[i].date);
      if (recordDate >= weekStart) {
        filteredHealth.push(health[i]);
      }
    }
    
    for (let i = 0; i < mood.length; i++) {
      const recordDate = new Date(mood[i].date);
      if (recordDate >= weekStart) {
        filteredMood.push(mood[i]);
      }
    }
    
    return {
      health: filteredHealth,
      mood: filteredMood
    };
  }

  filterThisMonth(health: HealthRecord[], mood: MoodRecord[]): PeriodRecords {
    const today = new Date();
    const monthStart = new Date(today.getFullYear(), today.getMonth(), 1);

    const filteredHealth: HealthRecord[] = [];
    const filteredMood: MoodRecord[] = [];
    
    for (let i = 0; i < health.length; i++) {
      const recordDate = new Date(health[i].date);
      if (recordDate >= monthStart) {
        filteredHealth.push(health[i]);
      }
    }
    
    for (let i = 0; i < mood.length; i++) {
      const recordDate = new Date(mood[i].date);
      if (recordDate >= monthStart) {
        filteredMood.push(mood[i]);
      }
    }
    
    return {
      health: filteredHealth,
      mood: filteredMood
    };
  }

  filterThisWeekDiaries(diaries: Diary[]): Diary[] {
    const today = new Date();
    const weekStart = new Date(today);
    weekStart.setDate(today.getDate() - today.getDay());
    weekStart.setHours(0, 0, 0, 0);

    const filtered: Diary[] = [];
    for (let i = 0; i < diaries.length; i++) {
      const diaryDate = new Date(diaries[i].date);
      if (diaryDate >= weekStart) {
        filtered.push(diaries[i]);
      }
    }
    return filtered;
  }

  filterThisMonthDiaries(diaries: Diary[]): Diary[] {
    const today = new Date();
    const monthStart = new Date(today.getFullYear(), today.getMonth(), 1);

    const filtered: Diary[] = [];
    for (let i = 0; i < diaries.length; i++) {
      const diaryDate = new Date(diaries[i].date);
      if (diaryDate >= monthStart) {
        filtered.push(diaries[i]);
      }
    }
    return filtered;
  }

  getAverageValue(records: HealthRecord[], type: string): number {
    const filtered: HealthRecord[] = [];
    for (let i = 0; i < records.length; i++) {
      if (records[i].type === type) {
        filtered.push(records[i]);
      }
    }
    if (filtered.length === 0) return 0;
    let sum = 0;
    for (let i = 0; i < filtered.length; i++) {
      sum += filtered[i].value;
    }
    return sum / filtered.length;
  }

  getMoodDistribution(records: MoodRecord[]): string {
    const counts: Record<string, number> = {};
    for (let i = 0; i < records.length; i++) {
      const mood = records[i].mood;
      counts[mood] = (counts[mood] || 0) + 1;
    }
    const entries = Object.entries(counts);
    const parts: string[] = [];
    for (let i = 0; i < entries.length; i++) {
      parts.push(`${entries[i][0]}(${entries[i][1]})`);
    }
    return parts.join(' ');
  }

  getSkillCategories(skills: Skill[]): string[] {
    const categories = new Set<string>();
    for (let i = 0; i < skills.length; i++) {
      categories.add(skills[i].category);
    }
    const result: string[] = [];
    categories.forEach((cat: string) => {
      result.push(cat);
    });
    return result;
  }
}
