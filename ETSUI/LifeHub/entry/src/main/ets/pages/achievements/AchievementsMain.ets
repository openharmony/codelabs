/**
 * 成就系统页面
 */
import { LifeHubData, HealthRecord, Goal, MoodRecord, Skill, TravelPlan, Diary } from '../../model/LifeHubData';

interface Achievement {
  id: string;
  title: string;
  description: string;
  icon: string;
  progress: number;
  maxProgress: number;
  unlocked: boolean;
  category: string;
}

@Entry
@Component
export struct AchievementsMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State achievements: Achievement[] = [];
  @State unlockedCount: number = 0;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadAchievements();
  }

  async loadAchievements() {
    try {
      // 获取统计数据
      const stats = await this.lifeHubData.getDashboardStats();
      const healthRecords = await this.lifeHubData.getHealthRecords(undefined, 1000);
      const goals = await this.lifeHubData.getAllGoals();
      const moodRecords = await this.lifeHubData.getMoodRecords(1000);
      const skills = await this.lifeHubData.getAllSkills();
      const travelPlans = await this.lifeHubData.getAllTravelPlans();
      const diaries = await this.lifeHubData.getAllDiaries();

      // 计算成就
      this.achievements = [
        {
          id: 'first_record',
          title: '初次记录',
          description: '记录第一条健康数据',
          icon: '🎉',
          progress: healthRecords.length > 0 ? 1 : 0,
          maxProgress: 1,
          unlocked: healthRecords.length > 0,
          category: '健康'
        },
        {
          id: 'health_master',
          title: '健康达人',
          description: '记录100条健康数据',
          icon: '💚',
          progress: healthRecords.length,
          maxProgress: 100,
          unlocked: healthRecords.length >= 100,
          category: '健康'
        },
        {
          id: 'goal_setter',
          title: '目标设定者',
          description: '创建第一个目标',
          icon: '🎯',
          progress: goals.length > 0 ? 1 : 0,
          maxProgress: 1,
          unlocked: goals.length > 0,
          category: '目标'
        },
        {
          id: 'goal_master',
          title: '目标大师',
          description: '完成10个目标',
          icon: '🏆',
          progress: this.getCompletedGoalsCount(goals),
          maxProgress: 10,
          unlocked: this.getCompletedGoalsCount(goals) >= 10,
          category: '目标'
        },
        {
          id: 'mood_tracker',
          title: '心情记录者',
          description: '记录30天心情',
          icon: '😊',
          progress: moodRecords.length,
          maxProgress: 30,
          unlocked: moodRecords.length >= 30,
          category: '心情'
        },
        {
          id: 'skill_learner',
          title: '技能学习者',
          description: '添加第一个技能',
          icon: '📚',
          progress: skills.length > 0 ? 1 : 0,
          maxProgress: 1,
          unlocked: skills.length > 0,
          category: '学习'
        },
        {
          id: 'skill_master',
          title: '技能大师',
          description: '拥有10个技能',
          icon: '🌟',
          progress: skills.length,
          maxProgress: 10,
          unlocked: skills.length >= 10,
          category: '学习'
        },
        {
          id: 'traveler',
          title: '旅行者',
          description: '创建第一个旅行计划',
          icon: '✈️',
          progress: travelPlans.length > 0 ? 1 : 0,
          maxProgress: 1,
          unlocked: travelPlans.length > 0,
          category: '旅行'
        },
        {
          id: 'diarist',
          title: '日记作家',
          description: '写10篇日记',
          icon: '📝',
          progress: diaries.length,
          maxProgress: 10,
          unlocked: diaries.length >= 10,
          category: '日记'
        },
        {
          id: 'dedicated',
          title: '持之以恒',
          description: '连续记录7天',
          icon: '🔥',
          progress: this.getConsecutiveDays(moodRecords),
          maxProgress: 7,
          unlocked: this.getConsecutiveDays(moodRecords) >= 7,
          category: '坚持'
        },
        {
          id: 'data_collector',
          title: '数据收集者',
          description: '总记录数达到500条',
          icon: '📊',
          progress: healthRecords.length + goals.length + moodRecords.length + 
                   skills.length + travelPlans.length + diaries.length,
          maxProgress: 500,
          unlocked: (healthRecords.length + goals.length + moodRecords.length + 
                    skills.length + travelPlans.length + diaries.length) >= 500,
          category: '综合'
        },
        {
          id: 'all_rounder',
          title: '全能选手',
          description: '所有模块都有记录',
          icon: '⭐',
          progress: this.getAllModulesProgress(healthRecords, goals, moodRecords, skills, travelPlans, diaries),
          maxProgress: 6,
          unlocked: this.getAllModulesProgress(healthRecords, goals, moodRecords, skills, travelPlans, diaries) === 6,
          category: '综合'
        }
      ];

      this.unlockedCount = this.achievements.filter(a => a.unlocked).length;
    } catch (error) {
      console.error('AchievementsMain: 加载成就失败:', error);
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('成就系统')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
        
        Column() {
          Text(this.unlockedCount.toString())
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#FF9800')
          Text(`/${this.achievements.length}`)
            .fontSize(14)
            .fontColor('#666')
        }
        .alignItems(HorizontalAlign.End)
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          // 成就列表
          ForEach(this.achievements, (achievement: Achievement) => {
            this.buildAchievementCard(achievement)
          })
        }
        .width('100%')
        .padding(15)
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  @Builder
  buildAchievementCard(achievement: Achievement) {
    Column() {
      Row() {
        Text(achievement.icon)
          .fontSize(40)
          .opacity(achievement.unlocked ? 1 : 0.3)
          .width(60)
          .textAlign(TextAlign.Center)

        Column() {
          Text(achievement.title)
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor(achievement.unlocked ? '#333' : '#999')
            .textAlign(TextAlign.Start)
            .width('100%')

          Text(achievement.description)
            .fontSize(14)
            .fontColor('#666')
            .textAlign(TextAlign.Start)
            .width('100%')
            .margin({ top: 5 })

          Row() {
            Text(achievement.category)
              .fontSize(12)
              .fontColor('#999')
              .padding({ left: 8, right: 8, top: 4, bottom: 4 })
              .backgroundColor('#f0f0f0')
              .borderRadius(10)

            Blank()

            if (achievement.unlocked) {
              Text('✓ 已解锁')
                .fontSize(12)
                .fontColor('#4CAF50')
                .fontWeight(FontWeight.Bold)
            } else {
              Text(`${achievement.progress}/${achievement.maxProgress}`)
                .fontSize(12)
                .fontColor('#666')
            }
          }
          .width('100%')
          .margin({ top: 10 })
        }
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 15 })
      }
      .width('100%')

      if (!achievement.unlocked && achievement.maxProgress > 1) {
        Progress({ 
          value: achievement.progress, 
          total: achievement.maxProgress, 
          type: ProgressType.Linear 
        })
          .width('100%')
          .height(6)
          .color('#007DFF')
          .margin({ top: 15 })
      }
    }
    .width('100%')
    .padding(15)
    .backgroundColor(achievement.unlocked ? Color.White : '#fafafa')
    .borderRadius(12)
    .border({
      width: achievement.unlocked ? 2 : 1,
      color: achievement.unlocked ? '#FFD700' : '#e0e0e0'
    })
    .margin({ bottom: 15 })
  }

  getCompletedGoalsCount(goals: Goal[]): number {
    let count = 0;
    for (let i = 0; i < goals.length; i++) {
      if (goals[i].status === 'completed') {
        count++;
      }
    }
    return count;
  }

  getConsecutiveDays(records: MoodRecord[]): number {
    if (records.length === 0) return 0;
    
    const dates: string[] = [];
    for (let i = 0; i < records.length; i++) {
      dates.push(records[i].date);
    }
    dates.sort().reverse();
    let consecutive = 1;
    const today = new Date();
    
    for (let i = 0; i < dates.length - 1; i++) {
      const date1 = new Date(dates[i]);
      const date2 = new Date(dates[i + 1]);
      const diffDays = Math.floor((date1.getTime() - date2.getTime()) / (1000 * 60 * 60 * 24));
      
      if (diffDays === 1) {
        consecutive++;
      } else {
        break;
      }
    }
    
    return consecutive;
  }

  getAllModulesProgress(health: HealthRecord[], goals: Goal[], mood: MoodRecord[], skills: Skill[], travel: TravelPlan[], diaries: Diary[]): number {
    let progress = 0;
    if (health.length > 0) progress++;
    if (goals.length > 0) progress++;
    if (mood.length > 0) progress++;
    if (skills.length > 0) progress++;
    if (travel.length > 0) progress++;
    if (diaries.length > 0) progress++;
    return progress;
  }
}
