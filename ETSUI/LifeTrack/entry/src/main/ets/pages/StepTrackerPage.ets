// StepTrackerPage.ets - 步数记录页面
import { StepManager } from './StepManager';
import { StepRecord, StepTrend } from '../model/StepModel';
import common from '@ohos.app.ability.common';
import preferences from '@ohos.data.preferences';
import { StepCounterService } from '../services/StepCounterService';

@Builder
export function StepTrackerBuilder() {
  StepTracker();
}

@ComponentV2
struct StepTracker {
  pathStack: NavPathStack = new NavPathStack();

  // 步数相关状态变量
  @Local todaySteps: number = 0;
  @Local todayCalories: number = 0;
  @Local todayDistance: number = 0;
  @Local stepGoal: number = 8000;
  @Local goalMet: boolean = false;
  @Local stepHistory: StepRecord[] = [];
  @Local stepTrends: StepTrend[] = [];
  @Local hasRecordedToday: boolean = false;

  // 输入相关状态
  @Local inputSteps: string = '0';
  @Local inputCalories: string = '0';
  @Local inputDistance: string = '0';
  
  // 目标设置相关状态
  @Local showGoalSetting: boolean = false;
  @Local newGoal: string = '8000';
  
  private context: common.UIAbilityContext =
    this.getUIContext().getHostContext() as common.UIAbilityContext;
  private stepManager: StepManager = StepManager.getInstance(this.context);
  private stepCounterService: StepCounterService = StepCounterService.getInstance(this.context);

  aboutToAppear() {
    // 添加步数变化监听器
    this.stepCounterService.addStepChangeListener(this.onStepsChanged.bind(this));
    this.loadStepGoal();
    this.loadData();
  }

  // 步数变化回调
  private onStepsChanged = (steps: number) => {
    // 更新本地状态以反映实时步数变化
    this.todaySteps = steps;
    this.todayCalories = this.calculateCalories(steps);
    this.todayDistance = this.calculateDistance(steps);
    this.goalMet = steps >= this.stepGoal;
  }

  // 初始化步数监听（如果尚未开始）
  async initStepCounter() {
    try {
      // 启动后台步数监听（如果尚未启动）
      await this.stepCounterService.startBackgroundStepTracking();
    } catch (error) {
      console.error('初始化步数监听失败:', error);
    }
  }

  // 加载数据
  async loadData() {
    try {
      const today = this.getCurrentDate();
      
      // 检查今天是否已有记录
      this.hasRecordedToday = await this.stepManager.hasTodayRecord();
      
      // 获取今天的步数记录
      const result = await this.stepManager.getStepRecordByDate(today);
      if (result.success && result.data) {
        const record = result.data as StepRecord;
        this.todaySteps = record.steps;
        this.todayCalories = record.calories;
        this.todayDistance = record.distance;
        this.goalMet = record.steps >= this.stepGoal;
      } else {
        // 如果数据库中没有今天的记录，使用服务中的当前步数
        this.todaySteps = this.stepCounterService.getCurrentSteps();
        this.todayCalories = this.calculateCalories(this.todaySteps);
        this.todayDistance = this.calculateDistance(this.todaySteps);
        this.goalMet = this.todaySteps >= this.stepGoal;
      }
      
      // 获取最近7天的趋势数据
      this.stepTrends = await this.stepManager.getStepTrends(7);
      
      // 获取最近的步数记录
      const historyResult = await this.stepManager.getRecentStepRecords(10);
      if (historyResult.success && historyResult.data) {
        this.stepHistory = historyResult.data as StepRecord[];
      }
    } catch (error) {
      console.error('加载步数数据失败:', error);
      this.getUIContext().getPromptAction().showToast({
        message: '加载数据失败',
        duration: 2000
      });
    }
  }

  // 保存步数记录
  async saveStepRecord() {
    try {
      const steps = parseInt(this.inputSteps) || 0;
      const calories = parseFloat(this.inputCalories) || 0;
      const distance = parseFloat(this.inputDistance) || 0;
      
      if (steps < 0) {
        this.getUIContext().getPromptAction().showToast({
          message: '步数不能为负数',
          duration: 2000
        });
        return;
      }
      
      const today = this.getCurrentDate();
      const stepData: Partial<StepRecord> = {
        date: today,
        steps: steps,


        calories: calories || this.calculateCalories(steps),
        distance: distance || this.calculateDistance(steps)
      };
      
      const result = await this.stepManager.saveStepRecord(stepData);
      
      if (result.success) {
        this.getUIContext().getPromptAction().showToast({
          message: '步数记录保存成功',
          duration: 2000
        });
        
        // 重新加载数据
        this.loadData();
        
        // 重置输入框
        this.inputSteps = '0';
        this.inputCalories = '';
        this.inputDistance = '';
      } else {
        this.getUIContext().getPromptAction().showToast({
          message: `保存失败: ${result.error}`,
          duration: 3000
        });
      }
    } catch (error) {
      console.error('保存步数记录失败:', error);
      this.getUIContext().getPromptAction().showToast({
        message: `保存失败: ${error.message || '未知错误'}`,
        duration: 3000
      });
    }
  }

  // 获取当前日期字符串 (YYYY-MM-DD)
  getCurrentDate(): string {
    const now = new Date();
    const year = now.getFullYear();
    const month = String(now.getMonth() + 1).padStart(2, '0');
    const day = String(now.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }

  // 计算卡路里消耗（估算值，基于平均值）
  calculateCalories(steps: number): number {
    // 每1000步大约消耗40-60卡路里，这里取中间值50
    return Math.round((steps / 1000) * 50 * 100) / 100; // 保留两位小数
  }

  // 计算行走距离（估算值，基于平均步幅）
  calculateDistance(steps: number): number {
    // 平均步幅约为0.7米
    return Math.round(steps * 0.7 * 100) / 100; // 保留两位小数
  }

  // 加载目标步数
  loadStepGoal() {
    try {
      // 从应用首选项加载目标步数
      preferences.getPreferences(this.context, 'settings').then((preferences) => {
        preferences.get('stepGoal', 8000).then((value) => {
          this.stepGoal = value as number;
        }).catch((error: Error) => {
          console.error('读取目标步数失败:', error);
        });
      }).catch((error: Error) => {
        console.error('获取偏好设置失败:', error);
      });
    } catch (error) {
      console.error('加载目标步数异常:', error);
    }
  }
  
  // 保存目标步数
  async saveStepGoal() {
    try {
      const newGoalValue = parseInt(this.newGoal) || 8000;
      if (newGoalValue <= 0) {
        this.getUIContext().getPromptAction().showToast({
          message: '目标步数必须大于0',
          duration: 2000
        });
        return;
      }
      
      // 更新目标步数
      this.stepGoal = newGoalValue;
      
      // 更新目标达成状态
      this.goalMet = this.todaySteps >= this.stepGoal;
      
      // 关闭设置弹窗
      this.showGoalSetting = false;
      
      // 保存到持久化存储
      preferences.getPreferences(this.context, 'settings').then((preferences) => {
        preferences.put('stepGoal', newGoalValue).then(() => {
          console.info('目标步数已保存:', newGoalValue);
        }).catch((error: Error) => {
          console.error('保存目标步数失败:', error);
        });
      }).catch((error: Error) => {
        console.error('获取偏好设置失败:', error);
      });
      
      this.getUIContext().getPromptAction().showToast({
        message: `目标步数已更新为 ${newGoalValue}`,
        duration: 2000
      });
    } catch (error) {
      console.error('保存目标步数失败:', error);
      this.getUIContext().getPromptAction().showToast({
        message: `保存失败: ${error.message || '未知错误'}`,
        duration: 2000
      });
    }
  }

  // 格式化日期显示
  formatDateDisplay(dateStr: string): string {
    const date = new Date(dateStr);
    const today = new Date();
    
    if (date.toDateString() === today.toDateString()) {
      return '今天';
    }
    
    const yesterday = new Date(today);
    yesterday.setDate(yesterday.getDate() - 1);
    
    if (date.toDateString() === yesterday.toDateString()) {
      return '昨天';
    }
    
    return `${date.getMonth() + 1}/${date.getDate()}`;
  }

  // 步数输入组件
  @Builder
  buildStepInput() {
    Column({ space: 24 }) {
      // 步数输入
      Column({ space: 8 }) {
        Text('步数')
          .fontSize(16)
          .fontColor('#333333')
          .align(Alignment.Start)
          .width('100%')

        TextInput({ text: this.inputSteps, placeholder: '输入今日步数' })
          .onChange((value: string) => {
            const numValue = parseInt(value) || 0;
            this.inputSteps = Math.max(0, numValue).toString();
            
            // 自动计算卡路里和距离
            if (value.trim() !== '') {
              this.inputCalories = this.calculateCalories(numValue).toString();
              this.inputDistance = this.calculateDistance(numValue).toString();
            }
          })
          .height(48)
          .width('100%')
          .backgroundColor(Color.White)
          .borderRadius(8)
          .padding(12)
          .fontSize(16)
      }

      // 卡路里输入
      Column({ space: 8 }) {
        Text('消耗卡路里 (可选)')
          .fontSize(16)
          .fontColor('#333333')
          .align(Alignment.Start)
          .width('100%')

        TextInput({ text: this.inputCalories, placeholder: '卡路里消耗' })
          .onChange((value: string) => {
            const numValue = parseFloat(value) || 0;
            this.inputCalories = Math.max(0, numValue).toString();
          })
          .height(48)
          .width('100%')
          .backgroundColor(Color.White)
          .borderRadius(8)
          .padding(12)
          .fontSize(16)
      }

      // 距离输入
      Column({ space: 8 }) {
        Text('行走距离(米) (可选)')
          .fontSize(16)
          .fontColor('#333333')
          .align(Alignment.Start)
          .width('100%')

        TextInput({ text: this.inputDistance, placeholder: '行走距离(米)' })
          .onChange((value: string) => {
            const numValue = parseFloat(value) || 0;
            this.inputDistance = Math.max(0, numValue).toString();
          })
          .height(48)
          .width('100%')
          .backgroundColor(Color.White)
          .borderRadius(8)
          .padding(12)
          .fontSize(16)
      }

      // 保存按钮
      Button('保存步数记录')
        .width('100%')
        .height(48)
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .backgroundColor('#007AFF')
        .fontColor(Color.White)
        .borderRadius(8)
        .onClick(() => {
          this.saveStepRecord();
        })
    }
    .padding(16)
    .width('100%')
  }

  // 今日步数统计组件
  @Builder
  buildTodayStats() {
    Column() {
      Text('今日步数统计')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .align(Alignment.Start)
        .width('100%')
        .margin({ bottom: 16 })

      // 步数环形图
      Stack() {
        // 步数进度显示
        Stack() {
          // 背景圆环（使用圆形容器模拟）
          Circle()
            .width(140)
            .height(140)
            .stroke('#E0E0E0')
            .strokeWidth(12)
            .fill(Color.Transparent)
          
          // 进度圆弧
          if (this.stepGoal > 0) {
            Progress({ value: Math.min(100, (this.todaySteps / this.stepGoal) * 100), total: 100, type: ProgressType.Ring })
              .width(140)
              .height(140)
              .color(this.goalMet ? '#34C759' : '#007AFF')
          }
        }
        .width(160)
        .height(160)

        // 中心文本
        Column() {
          Text(`${this.todaySteps.toLocaleString()}`)
            .fontSize(24)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
          Text(`/ ${this.stepGoal.toLocaleString()} 步`)
            .fontSize(14)
            .fontColor('#999999')
          Text(this.goalMet ? '目标已完成!' : '继续加油!')
            .fontSize(12)
            .fontColor(this.goalMet ? '#34C759' : '#FF9500')
            .margin({ top: 8 })
        }
        .alignItems(HorizontalAlign.Center)
        .justifyContent(FlexAlign.Center)
        .width('100%')
        .height('80%')
      }
      .margin({ bottom: 16 })

      // 详细数据
      Grid() {
        GridItem() {
          Column() {
            Text(`${this.todayCalories}`)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333333')
            Text('卡路里')
              .fontSize(12)
              .fontColor('#999999')
          }
          .alignItems(HorizontalAlign.Center)
        }

        GridItem() {
          Column() {
            Text(`${this.todayDistance}m`)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333333')
            Text('距离')
              .fontSize(12)
              .fontColor('#999999')
          }
          .alignItems(HorizontalAlign.Center)
        }

        GridItem() {
          Column() {
            Text(`${this.stepGoal}`)
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333333')
              .onClick(() => {
                this.newGoal = this.stepGoal.toString();
                this.showGoalSetting = true;
              })
            Text('目标步数')
              .fontSize(12)
              .fontColor('#999999')
          }
          .alignItems(HorizontalAlign.Center)
        }
      }
      .columnsTemplate('1fr 1fr 1fr')
      .width('100%')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FEFEFE')
    .border({ radius: 12 })
    .shadow({
      radius: 6,
      color: '#4D7CFF',
      offsetX: 0,
      offsetY: 2,
    })
  }

  // 步数趋势图表组件
  @Builder
  buildStepTrendChart() {
    Column() {
      Text('近7日步数趋势')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .align(Alignment.Start)
        .width('100%')
        .margin({ bottom: 16 })

      // 图表容器
      Row() {
        // Y轴标签
        Column() {
          Text('高')
            .fontSize(12)
            .fontColor('#999999')
            .layoutWeight(1)
            .align(Alignment.Top)
          
          Text('')
            .fontSize(12)
            .fontColor('#999999')
            .layoutWeight(1)
            .align(Alignment.Center)
          
          Text('低')
            .fontSize(12)
            .fontColor('#999999')
            .layoutWeight(1)
            .align(Alignment.Bottom)
        }
        .height(150)
        .width(15)

        // 图表主体
        Column() {
          Row() {
            ForEach(this.stepTrends.slice(0, 7), (item: StepTrend, index: number) => { // 限制最多显示7天的数据
              Column() {
                // 柱状图
                Row() {
                  Blank()
                    .width(20)
                    .height(Math.max(4, Math.min(140, item.steps / 100))) // 固定比例，每100步对应1px高度，最大140px
                    .backgroundColor(item.goalMet ? '#34C759' : '#007AFF')
                    .borderRadius(2)
                }
                .height(150)
                .alignItems(VerticalAlign.Bottom) // 对齐到底部

                // 日期标签
                Text(this.formatDateDisplay(item.date))
                  .fontSize(10)
                  .fontColor('#666666')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Center)
              .layoutWeight(0.1)
            })
          }
          .height(150)
        }
      }
      .width('100%')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FEFEFE')
    .border({ radius: 12 })
    .shadow({
      radius: 6,
      color: '#4D7CFF',
      offsetX: 0,
      offsetY: 2,
    })
  }

  // 目标设置弹窗
  @Builder
  buildGoalSettingDialog() {
    if (this.showGoalSetting) {
      Column() {
        // 弹窗内容容器
        Column() {
          Text('设置目标步数')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ bottom: 20 })

          TextInput({ text: this.newGoal, placeholder: '输入目标步数' })
            .onChange((value: string) => {
              const numValue = parseInt(value) || 0;
              this.newGoal = Math.max(0, numValue).toString();
            })
            .width('80%')
            .height(40)
            .padding(10)
            .border({ width: 1, color: '#E0E0E0' })
            .borderRadius(8)
            .fontSize(16)

          Row({ space: 20 }) {
            Button('取消')
              .width(100)
              .height(40)
              .backgroundColor('#F0F0F0')
              .fontColor('#333333')
              .borderRadius(6)
              .onClick(() => {
                this.showGoalSetting = false;
              })

            Button('确定')
              .width(100)
              .height(40)
              .backgroundColor('#007AFF')
              .fontColor(Color.White)
              .borderRadius(6)
              .onClick(() => {
                this.saveStepGoal();
              })
          }
          .margin({ top: 20 })
        }
        .width('80%')
        .padding(24)
        .backgroundColor(Color.White)
        .borderRadius(12)
        .shadow({
          radius: 8,
          color: '#1A000000',
          offsetX: 0,
          offsetY: 4,
        })
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#FF333333')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)
      .zIndex(1000)
      .position({ x: 0, y: 0 })
    }
  }

  // 历史记录组件
  @Builder
  buildHistoryRecords() {
    Column() {
      Text('历史记录')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .align(Alignment.Start)
        .width('100%')
        .margin({ bottom: 16 })

      List({ space: 8 }) {
        ForEach(this.stepHistory, (record: StepRecord) => {
          ListItem() {
            Row() {
              Column() {
                Text(`${record.steps.toLocaleString()} 步`)
                  .fontSize(16)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333333')
                
                Text(`${record.calories} 卡路里 | ${record.distance}m`)
                  .fontSize(12)
                  .fontColor('#999999')
              }
              .layoutWeight(1)

              Text(this.formatDateDisplay(record.date))
                .fontSize(14)
                .fontColor('#666666')
            }
            .padding(12)
            .backgroundColor(Color.White)
            .borderRadius(8)
          }
        })
      }
      .cachedCount(5)
      .edgeEffect(EdgeEffect.None)
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FEFEFE')
    .border({ radius: 12 })
    .shadow({
      radius: 6,
      color: '#4D7CFF',
      offsetX: 0,
      offsetY: 2,
    })
  }

  aboutToDisappear() {
    // 移除步数变化监听器
    this.stepCounterService.removeStepChangeListener(this.onStepsChanged.bind(this));
    console.info('已移除步数变化监听器');
  }

  build() {
    NavDestination() {
      Scroll() {
        Column({ space: 16 }) {
          // 今日统计
          if (this.hasRecordedToday) {
            this.buildTodayStats()
          }

          // 输入组件（如果今天还没有记录）
          if (!this.hasRecordedToday) {
            this.buildStepInput()
          }

          // 趋势图表
          this.buildStepTrendChart()

          // 历史记录
          if (this.stepHistory && this.stepHistory.length > 0) {
            this.buildHistoryRecords()
          } else {
            // 如果没有历史记录，也显示一个提示
            Column() {
              Text('历史记录')
                .fontSize(20)
                .fontWeight(FontWeight.Bold)
                .align(Alignment.Start)
                .width('100%')
                .margin({ bottom: 16 })
              
              Column() {
                Text('暂无历史记录')
                  .fontSize(16)
                  .fontColor('#999999')
                  .align(Alignment.Center)
                  .width('100%')
              }
              .width('100%')
              .padding(16)
              .backgroundColor('#FEFEFE')
              .border({ radius: 12 })
              .shadow({
                radius: 6,
                color: '#4D7CFF',
                offsetX: 0,
                offsetY: 2,
              })
            }
            .width('100%')
            .padding(16)
          }

          // 空白填充
          Blank()
            .height(20)
        }
        .padding({ left: 16, right: 16, top: 0, bottom: 16 })
        .width('100%')
        // 移除固定高度，让内容自然撑开
      }
      .scrollBar(BarState.Auto)
      
      // 目标设置弹窗
      this.buildGoalSettingDialog()
    }
    .title('步数记录')
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}