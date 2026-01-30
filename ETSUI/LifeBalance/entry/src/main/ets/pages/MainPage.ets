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
import { DailyData } from '../model/information';
import promptAction from '@ohos.promptAction';
import { HealthDao, HealthRecord } from '../common/database/HealthDao';
import { RdbManager } from '../common/database/RdbManager';
import { WaterDao } from '../common/database/WaterDao';
import relationalStore from '@ohos.data.relationalStore';
import router from '@ohos.router';

@Component
export struct Main {
  // 引入全局字体缩放比例
  @StorageProp('appFontScale') fontScale: number = 1.0
  // --- 本地状态 (带 Watch 监听) ---
  @State @Watch('calculateAll') userHeight: number = 0
  @State @Watch('calculateAll') userWeight: number = 0
  @State @Watch('calculateAll') userAge: number = 0
  @State @Watch('calculateAll') isMale: boolean = true
  @State @Watch('calculateAll') age: number = 25
  @State isDataReady: boolean = false
  // --- 页面计算用的临时状态 ---
  @State bmi: number = 0
  // --- 全局链接 ---
  @StorageLink('targetCalories') targetCalories: number = 0
  @StorageLink('targetWater') targetWater: number = 0
  @StorageLink('targetExercise') targetExercise: number = 0 // 单位为15min
  @StorageLink('healthHistory') historyData: Record<string, DailyData> = {}
  private healthDao = new HealthDao();
  private waterDao = new WaterDao();
  private today: string = this.getTodayStr();

  getTodayStr(): string {
    const now = new Date();
    const y = now.getFullYear();
    const m = (now.getMonth() + 1).toString().padStart(2, '0');
    const d = now.getDate().toString().padStart(2, '0');
    return `${y}-${m}-${d}`;
  }

  async aboutToAppear() {
    console.info('[Main] aboutToAppear started');
    this.today = this.getTodayStr();
    const tryLoadData = async (retryCount: number = 0) => {
      const store: relationalStore.RdbStore | null = RdbManager.getInstance().getRdbStore();

      if (!store && retryCount < 5) {
        console.info(`[Main] Store 尚未就绪，准备第 ${retryCount + 1} 次重试...`);
        setTimeout((): void => {
          tryLoadData(retryCount + 1);
        }, 100);
        return;
      }

      try {
        const lastRecord = await this.healthDao.getLatestRecord();
        if (lastRecord) {
          // ... 原有的赋值逻辑 ...
          this.userHeight = lastRecord.height;
          this.userWeight = lastRecord.weight;
          this.userAge = lastRecord.age;
          this.calculateAll();

          AppStorage.SetOrCreate('targetCalories', this.targetCalories);
          AppStorage.SetOrCreate('targetWater', this.targetWater);
          AppStorage.SetOrCreate('targetExercise', this.targetExercise);
          console.info(`[Main] 异步重试加载成功: H=${this.userHeight}`);
        } else {
          console.warn('[Main] 尝试获取记录完成，但结果确实为空');
        }
      } catch (err) {
        console.error(`[Main] 查询报错: ${JSON.stringify(err)}`);
      }
      this.isDataReady = true;
    };

    // 开始执行
    tryLoadData();
  }

  // --- 核心算法 ---
  calculateAll() {
    // 1. 增加前置校验：如果身高或体重未输入（为0），则所有目标归零
    if (this.userHeight <= 0 || this.userWeight <= 0) {
      this.bmi = 0;
      this.targetCalories = 0;
      this.targetWater = 0;
      this.targetExercise = 0;
      return;
    }
    // 1. BMI 计算
    const h_meter = this.userHeight / 100
    if (h_meter > 0) {
      this.bmi = parseFloat((this.userWeight / (h_meter * h_meter)).toFixed(1))
    } else {
      this.bmi = 0
    }

    // 2. BMR & 饮食目标
    let bmr = 0
    bmr = 10 * this.userWeight + 6.25 * this.userHeight - 5 * this.userAge + 5

    // 更新全局变量
    this.targetCalories = Math.round(bmr * 1.2)
    this.targetWater = Math.round(this.userWeight * 35)

    // 【新增】根据身体状况自动计算建议运动时长
    // ---------------------------------------------------------
    let exercisePortion = 2; // 默认 2份 (30分钟)
    // 规则A: 如果 BMI 超重 (>24)，建议增加到 45分钟
    if (this.bmi >= 24) {
      exercisePortion = 3;
    }
    // 规则B: 如果 BMI 肥胖 (>28)，建议增加到 60分钟
    if (this.bmi >= 28) {
      exercisePortion = 4;
    }

    // 规则C: 如果年龄过大 (>65岁) 或 过小 (<12岁)，为了安全保持适量 (30分钟)
    if (this.age > 65 || this.age < 12) {
      exercisePortion = 2;
    }
    this.targetExercise = exercisePortion;
  }

  async saveDailyRecord() {
    try {
      this.calculateAll();
      this.today = this.getTodayStr();
      const healthRec: HealthRecord = {
        date: this.today,
        height: this.userHeight,
        weight: this.userWeight,
        age: this.userAge,
        bmi: this.bmi,
        targetCalories: this.targetCalories,
        targetWater: this.targetWater,
        targetExercise: this.targetExercise
      };

      console.info(`[Main] 准备保存记录: ${JSON.stringify(healthRec)}`);
      await this.healthDao.saveHealthRecord(healthRec);
      console.info(`[Main] HealthDao 保存成功`);

      // 饮水部分也加上日志
      await this.waterDao.saveWaterRecord({
        date: this.today,
        actual: 0,
        goal: this.targetWater
      });

      promptAction.showToast({ message: '同步成功' });
      AppStorage.SetOrCreate('globalDataUpdate', Date.now());
    } catch (err) {
      console.error(`[Main] 保存失败: ${JSON.stringify(err)}`);
      promptAction.showToast({ message: '保存失败，请检查数据库' });
    }
  }

  // --- 辅助显示 ---
  getBMIStatus(): string {
    if (this.bmi < 18.5) {
      return '偏瘦'
    }
    if (this.bmi < 24) {
      return '正常'
    }
    if (this.bmi < 28) {
      return '超重'
    }
    return '肥胖'
  }

  getBMIColor(): string {
    if (this.bmi < 18.5) {
      return '#3F51B5'
    }
    if (this.bmi < 24) {
      return '#00C853'
    }
    if (this.bmi < 28) {
      return '#FFAB00'
    }
    return '#FF5252'
  }

  build() {
    Scroll() {
      Column({ space: 15 }) {
        Text('个人健康概览')
          .fontSize(24 * this.fontScale)
          .fontWeight(FontWeight.Bold)
          .width('100%')
          .margin({
            top: 10,
            bottom: 5
          })

        // --- 1. 基础信息设置卡片 ---
        Column() {
          Row() {
            Text('基础信息')
              .fontSize(16 * this.fontScale)
              .fontWeight(FontWeight.Bold)
            Blank()
          }.width('100%').padding({ bottom: 10 })

          Column() {
            this.GenderRow()
            Divider().color('#F5F5F5')
            InputRow({
              label: '年龄',
              unit: '岁',
              value: $userAge,
              fontScale: this.fontScale
            })
            Divider().color('#F5F5F5')
            InputRow({
              label: '身高',
              unit: 'cm',
              value: $userHeight,
              fontScale: this.fontScale
            })
            Divider().color('#F5F5F5')
            InputRow({
              label: '体重',
              unit: 'kg',
              value: $userWeight,
              fontScale: this.fontScale
            })
          }
        }
        .padding(16)
        .backgroundColor(Color.White)
        .borderRadius(16)
        .shadow({
          radius: 10,
          color: 'rgba(0,0,0,0.05)'
        })

        // --- 2. BMI 仪表盘 ---
        Column() {
          Text('身体质量指数 (BMI)')
            .fontSize(14 * this.fontScale)
            .fontColor('#666').width('100%').margin({ bottom: 10 })

          Stack({ alignContent: Alignment.Bottom }) {
            Gauge({
              value: Math.min(this.bmi, 35), // 限制最大值为35，防止指针回弹
              min: 10,
              max: 35
            })
              .startAngle(210)
              .endAngle(150)
              .colors([['#3F51B5', 0.2], ['#00C853', 0.3], ['#FFAB00', 0.2], ['#FF5252', 0.3]])
              .strokeWidth(20)
              .width(200)
              .height(200)

            Column() {
              Text(this.bmi.toFixed(1))
                .fontSize(40 * this.fontScale)
                .fontWeight(FontWeight.Bold).fontColor('#333')
              Text(this.getBMIStatus())
                .fontSize(16 * this.fontScale)
                .fontColor(this.getBMIColor())
                .fontWeight(FontWeight.Medium)
                .margin({ top: 5 })
            }.margin({ bottom: 60 })

            Row() {
              Text('10')
                .fontSize(12 * this.fontScale)
                .fontColor('#999')
              Blank()
              Text('35')
                .fontSize(12 * this.fontScale)
                .fontColor('#999')
            }.width('100%').padding({
              left: 35,
              right: 35,
              bottom: 25
            })
          }
        }
        .padding(16)
        .backgroundColor(Color.White)
        .borderRadius(16)

        // --- 3. 目标展示卡片 (改用 RingCardComponent) ---
        // 【关键修复】这里现在传递的是数值给子组件的 @Prop
        // 只要 targetCalories 变了，RingCardComponent 必定重绘
        Row({ space: 12 }) {
          RingCardComponent({
            title: '建议热量',
            value: this.targetCalories, // 传入 StorageLink 的值
            unit: 'kcal',
            max: 3000,
            color: '#FF9C6E',
            fontScale: this.fontScale
          })

          RingCardComponent({
            title: '建议饮水',
            value: this.targetWater, // 传入 StorageLink 的值
            unit: 'ml',
            max: 4000,
            color: '#5CDBD3',
            fontScale: this.fontScale
          })
        }

        // --- 4. 运动卡片 ---
        Row() {
          // 左侧标题
          Text('建议运动时长')
            .fontSize(16 * this.fontScale)
            .fontColor('#333')
            .fontWeight(FontWeight.Medium)

          Blank()

          // 右侧数值展示
          Row() {
            Text((this.targetExercise * 15).toString()) // 自动计算的值
              .fontSize(20 * this.fontScale)
              .fontWeight(FontWeight.Bold)
              .fontColor('#52C41A') // 使用绿色，代表健康建议

            Text(' 分钟')
              .fontSize(14 * this.fontScale)
              .fontColor('#666')
              .margin({
                left: 4,
                bottom: 2
              })
          }
          .alignItems(VerticalAlign.Bottom)
        }
        .width('100%')
        .padding(16)
        .backgroundColor(Color.White)
        .borderRadius(16)

        // --- 5. 保存按钮 ---
        Button('保存今日健康记录')
          .width('100%')
          .height(50)
          .fontSize(16 * this.fontScale)
          .fontWeight(FontWeight.Medium)
          .backgroundColor('#007DFF')
          .margin({ top: 10 })
          .onClick(() => this.saveDailyRecord())

        Button('查看历史与统计')
          .width('100%')
          .height(50)
          .fontSize(16 * this.fontScale)
          .fontWeight(FontWeight.Medium)
          .fontColor('#007dff')
          .backgroundColor('#00000000')
          .onClick(() => router.push({
            url: 'pages/CalendarPage'
          })) // 跳转到日历
        Button('字体大小设置') // 新增的入口按钮
          .width('100%')
          .height(50)
          .fontSize(16 * this.fontScale) // 按钮文字也适配缩放
          .fontWeight(FontWeight.Medium)
          .fontColor('#333')
          .backgroundColor('#FFFFFF') // 白色背景
          .margin({ top: 10, bottom: 20 }) // 底部留点空隙
          .onClick(() => {
            // 跳转到我们刚刚创建的设置页
            router.pushUrl({
              url: 'pages/Mine/SettingsPage'
            }, (err) => {
              if (err) {
                console.error(`跳转失败: ${err.code}, ${err.message}`);
              }
            });
          })
      }
      .padding(16)
    }
    .backgroundColor('#F1F3F5')
    .height('100%')
  }

  // --- UI Builders (简单的性别选择不需要做成组件) ---
  @Builder
  GenderRow() {
    Row() {
      Text('性别')
        .fontSize(14 * this.fontScale)
        .fontWeight(FontWeight.Medium)
      Blank()
      Row({ space: 10 }) {
        this.GenderButton(true, '男')
        this.GenderButton(false, '女')
      }
    }.width('100%').height(50)
  }

  @Builder
  GenderButton(isMaleBtn: boolean, text: string) {
    Text(text)
      .fontSize(14 * this.fontScale)
      .fontColor(this.isMale === isMaleBtn ? '#FFF' : '#666')
      .backgroundColor(this.isMale === isMaleBtn ? '#007DFF' : '#F0F0F0')
      .padding({
        left: 15,
        right: 15,
        top: 6,
        bottom: 6
      })
      .borderRadius(15)
      .onClick(() => {
        this.isMale = isMaleBtn
      })
  }
}

// =======================================================
//   组件区：独立的 Component 保证数据联动的可靠性
// =======================================================

// 1. 输入行组件
@Component
struct InputRow {
  @Prop label: string
  @Prop unit: string
  @Link value: number
  @Prop fontScale: number = 1.0

  build() {
    Row() {
      // label 标签字体适配
      Text(this.label)
        .fontSize(14 * this.fontScale)
        .fontWeight(FontWeight.Medium)
      Blank()
      TextInput({ text: this.value.toString() })
        .type(InputType.Number)
        .textAlign(TextAlign.End)
        .backgroundColor(Color.Transparent)
        .width(80 * this.fontScale) // 输入框宽度也可以适当伸缩
        .fontSize(16 * this.fontScale)
        .fontColor('#007DFF')
        .onChange((v) => {
          if (v === '') {
            return
          }
          const num = Number(v)
          if (!isNaN(num)) {
            // 修改 Link 会触发父组件 Main 的 @Watch
            this.value = num
          }
        })
      // 单位文本适配
      Text(this.unit)
        .fontSize(12 * this.fontScale)
        .fontColor('#999')
        .margin({ left: 5 })
    }
    .width('100%').height(50)
  }
}

// 2. 【关键修复】环形进度卡片组件
// 之前是 @Builder，现在改为 @Component + @Prop
// 这样可以确保当 Main 中的 targetCalories 变化时，这里一定会刷新
@Component
struct RingCardComponent {
  @Prop title: string
  @Prop value: number // 接收父组件变化的数值
  @Prop unit: string
  @Prop max: number
  @Prop color: string
  @Prop fontScale: number = 1.0 //

  build() {
    Column() {
      // 标题适配
      Text(this.title)
        .fontSize(14 * this.fontScale)
        .fontColor('#666')
        .margin({ bottom: 10 })

      Stack() {
        Progress({
          value: this.value,
          total: this.max,
          type: ProgressType.Ring
        })
          .width(80 * this.fontScale)
          .color(this.color)
          .style({ strokeWidth: 8 * this.fontScale })

        Column() {
          // 核心数值适配
          Text(`${this.value}`)
            .fontSize(18 * this.fontScale)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.color)

          // 单位小字适配
          Text(this.unit)
            .fontSize(10 * this.fontScale)
            .fontColor('#999')
        }
      }
    }
    .layoutWeight(1)
    .constraintSize({ minHeight: 140 }) // <--- 【新增这一行】 设置最小高度，内容多了自动长高
    .backgroundColor(Color.White)
    .borderRadius(16)
    .justifyContent(FlexAlign.Center)
    .padding(15)
    .shadow({
      radius: 5,
      color: 'rgba(0,0,0,0.05)'
    })
  }
}