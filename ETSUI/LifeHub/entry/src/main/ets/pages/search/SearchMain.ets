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
 * 统一搜索页面
 */
import { LifeHubData, HealthRecord, MoodRecord, Goal, Skill, TravelPlan, Diary } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

interface SearchResult {
  type: string;
  title: string;
  subtitle: string;
  icon: string;
  data: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary;
}

@Entry
@Component
export struct SearchMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  @State searchText: string = '';
  @State searchResults: SearchResult[] = [];
  @State isSearching: boolean = false;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
  }

  build() {
    Column() {
      // 搜索栏
      Row() {
        TextInput({ placeholder: '搜索健康记录、目标、心情、技能、旅行计划、日记...', text: this.searchText })
          .layoutWeight(1)
          .height(40)
          .fontSize(16)
          .padding({ left: 15, right: 15 })
          .backgroundColor('#f0f0f0')
          .borderRadius(20)
          .onChange((value: string) => {
            this.searchText = value;
            if (value.length > 0) {
              setTimeout(() => {
                this.performSearch(value);
              }, 300);
            } else {
              this.searchResults = [];
              this.isSearching = false;
            }
          })

        if (this.searchText.length > 0) {
          Button('清除')
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .fontColor('#007DFF')
            .margin({ left: 10 })
            .onClick(() => {
              this.searchText = '';
              this.searchResults = [];
            })
        }
      }
      .width('100%')
      .padding(15)
      .backgroundColor(Color.White)

      // 搜索结果
      if (this.isSearching) {
        Column() {
          LoadingProgress()
            .width(40)
            .height(40)
            .margin({ top: 50 })
          Text('搜索中...')
            .fontSize(14)
            .fontColor('#666')
            .margin({ top: 10 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else if (this.searchResults.length === 0 && this.searchText.length > 0) {
        Column() {
          Text('🔍')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('未找到相关结果')
            .fontSize(18)
            .fontColor('#666')
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else if (this.searchResults.length === 0) {
        Column() {
          Text('🔍')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('输入关键词开始搜索')
            .fontSize(18)
            .fontColor('#666')
          Text('支持搜索所有模块的数据')
            .fontSize(14)
            .fontColor('#999')
            .margin({ top: 10 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List() {
          ForEach(this.searchResults, (result: SearchResult, index: number) => {
            ListItem() {
              Row() {
                Text(result.icon)
                  .fontSize(32)
                  .width(50)
                  .textAlign(TextAlign.Center)

                Column() {
                  Text(result.title)
                    .fontSize(16)
                    .fontWeight(FontWeight.Medium)
                    .textAlign(TextAlign.Start)
                    .width('100%')
                    .maxLines(1)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })

                  Text(result.subtitle)
                    .fontSize(14)
                    .fontColor('#666')
                    .textAlign(TextAlign.Start)
                    .width('100%')
                    .margin({ top: 5 })
                    .maxLines(2)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })

                  Text(result.type)
                    .fontSize(12)
                    .fontColor('#999')
                    .textAlign(TextAlign.Start)
                    .width('100%')
                    .margin({ top: 5 })
                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start)
                .padding({ left: 15 })
              }
              .width('100%')
              .padding(15)
              .backgroundColor(Color.White)
              .borderRadius(10)
              .margin({ left: 15, right: 15, bottom: 10 })
              .onClick(() => {
                this.navigateToDetail(result);
              })
            }
          }, (result: SearchResult, index: number) => `${result.type}-${index}`)
        }
        .width('100%')
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async performSearch(keyword: string) {
    this.isSearching = true;
    this.searchResults = [];

    try {
      // 搜索健康记录
      const healthRecords = await this.lifeHubData.getHealthRecords(undefined, 100);
      for (let i = 0; i < healthRecords.length; i++) {
        const record = healthRecords[i];
        if (this.matches(record.type, keyword) ||
        this.matches(record.note || '', keyword) ||
        this.matches(record.date, keyword)) {
          const result: SearchResult = {
            type: '健康记录',
            title: this.getHealthTypeName(record.type),
            subtitle: `${record.value} ${record.unit} - ${record.date}`,
            icon: '💚',
            data: record
          };
          this.searchResults.push(result);
        }
      }

      // 搜索目标
      const goals = await this.lifeHubData.getAllGoals();
      for (let i = 0; i < goals.length; i++) {
        const goal = goals[i];
        if (this.matches(goal.title, keyword) ||
        this.matches(goal.description || '', keyword) ||
        this.matches(goal.category, keyword)) {
          const result: SearchResult = {
            type: '目标',
            title: goal.title,
            subtitle: goal.description || '',
            icon: '🎯',
            data: goal
          };
          this.searchResults.push(result);
        }
      }

      // 搜索心情记录
      const moodRecords = await this.lifeHubData.getMoodRecords(100);
      for (let i = 0; i < moodRecords.length; i++) {
        const record = moodRecords[i];
        if (this.matches(record.mood, keyword) ||
        this.matches(record.note || '', keyword) ||
        this.matches(record.date, keyword)) {
          const result: SearchResult = {
            type: '心情记录',
            title: `${record.mood} 心情`,
            subtitle: `评分: ${record.score}/10 - ${record.date}`,
            icon: '😊',
            data: record
          };
          this.searchResults.push(result);
        }
      }

      // 搜索技能
      const skills = await this.lifeHubData.getAllSkills();
      for (let i = 0; i < skills.length; i++) {
        const skill = skills[i];
        if (this.matches(skill.name, keyword) ||
        this.matches(skill.description || '', keyword) ||
        this.matches(skill.category, keyword)) {
          const result: SearchResult = {
            type: '技能',
            title: skill.name,
            subtitle: `${skill.category} - 等级 ${skill.level}/${skill.targetLevel}`,
            icon: '📚',
            data: skill
          };
          this.searchResults.push(result);
        }
      }

      // 搜索旅行计划
      const travelPlans = await this.lifeHubData.getAllTravelPlans();
      for (let i = 0; i < travelPlans.length; i++) {
        const plan = travelPlans[i];
        if (this.matches(plan.title, keyword) ||
        this.matches(plan.destination, keyword) ||
        this.matches(plan.description || '', keyword)) {
          const result: SearchResult = {
            type: '旅行计划',
            title: plan.title,
            subtitle: `${plan.destination} - ${plan.startDate} 至 ${plan.endDate}`,
            icon: '✈️',
            data: plan
          };
          this.searchResults.push(result);
        }
      }

      // 搜索日记
      const diaries = await this.lifeHubData.getAllDiaries();
      for (let i = 0; i < diaries.length; i++) {
        const diary = diaries[i];
        if (this.matches(diary.title || '', keyword) ||
        this.matches(diary.content, keyword) ||
        this.matches(diary.date, keyword)) {
          const result: SearchResult = {
            type: '日记',
            title: diary.title || diary.date,
            subtitle: diary.content.substring(0, 50) + (diary.content.length > 50 ? '...' : ''),
            icon: '📝',
            data: diary
          };
          this.searchResults.push(result);
        }
      }

    } catch (error) {
      console.error('SearchMain: 搜索失败:', error);
    } finally {
      this.isSearching = false;
    }
  }

  matches(text: string, keyword: string): boolean {
    return text.toLowerCase().includes(keyword.toLowerCase());
  }

  getHealthTypeName(type: string): string {
    const names: Record<string, string> = {
      'weight': '体重',
      'blood_pressure': '血压',
      'blood_sugar': '血糖',
      'exercise': '运动',
      'sleep': '睡眠'
    };
    return names[type] || type;
  }

  navigateToDetail(result: SearchResult) {
    if (result.type === '目标') {
      router.pushUrl({
        url: 'pages/goals/GoalDetail',
        params: { goalId: result.data.id }
      });
    } else if (result.type === '技能') {
      router.pushUrl({
        url: 'pages/learning/SkillDetail',
        params: { skillId: result.data.id }
      });
    } else if (result.type === '旅行计划') {
      router.pushUrl({
        url: 'pages/travel/TravelPlan',
        params: { planId: result.data.id }
      });
    } else if (result.type === '日记') {
      router.pushUrl({
        url: 'pages/diary/DiaryEdit',
        params: { diaryId: result.data.id }
      });
    }
    // 其他类型暂时不跳转
  }
}
