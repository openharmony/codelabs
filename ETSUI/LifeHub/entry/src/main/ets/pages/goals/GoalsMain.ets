/**
 * 目标模块主页面
 */
import { LifeHubData, Goal } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Component
export struct GoalsMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State goals: Goal[] = [];
  @State isLoading: boolean = true;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadGoals();
  }

  async loadGoals() {
    this.isLoading = true;
    try {
      this.goals = await this.lifeHubData.getAllGoals();
    } catch (error) {
      console.error('GoalsMain: 加载数据失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  build() {
    Column() {
      Row() {
        Text('我的目标')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
        
        Button('+')
          .fontSize(20)
          .width(40)
          .height(40)
          .borderRadius(20)
          .backgroundColor('#FF9800')
          .fontColor(Color.White)
          .onClick(() => {
            router.pushUrl({ url: 'pages/goals/GoalDetail' });
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      if (this.isLoading) {
        LoadingProgress()
          .width(50)
          .height(50)
          .margin({ top: 100 })
      } else if (this.goals.length === 0) {
        Column() {
          Text('🎯')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('还没有目标')
            .fontSize(18)
            .fontColor('#666')
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List() {
          ForEach(this.goals, (goal: Goal) => {
            ListItem() {
              Column() {
                Row() {
                  Text(goal.title)
                    .fontSize(18)
                    .fontWeight(FontWeight.Medium)
                    .layoutWeight(1)

                  Text(`${goal.progress}%`)
                    .fontSize(16)
                    .fontColor('#FF9800')
                    .fontWeight(FontWeight.Bold)
                }
                .width('100%')

                // 进度条
                Row() {
                  Blank()
                    .layoutWeight(1 - goal.progress / 100)
                    .backgroundColor('#e0e0e0')
                    .height(6)
                    .borderRadius(3)
                  
                  Blank()
                    .layoutWeight(goal.progress / 100)
                    .backgroundColor('#FF9800')
                    .height(6)
                    .borderRadius(3)
                }
                .width('100%')
                .borderRadius(3)
                .margin({ top: 10 })
              }
              .width('100%')
              .padding(15)
              .alignItems(HorizontalAlign.Start)
              .backgroundColor(Color.White)
              .borderRadius(10)
              .margin({ left: 15, right: 15, bottom: 10 })
              .onClick(() => {
                router.pushUrl({
                  url: 'pages/goals/GoalDetail',
                  params: { goalId: goal.id }
                });
              })
            }
          }, (goal: Goal) => goal.id?.toString() || '')
        }
        .width('100%')
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }
}
