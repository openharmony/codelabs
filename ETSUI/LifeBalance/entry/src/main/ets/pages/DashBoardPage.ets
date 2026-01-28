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
import router from '@ohos.router';
import promptAction from '@ohos.promptAction';

@Component
export struct DashboardPage {
  @StorageProp('appFontScale') fontScale: number = 1.0;

  // 双向绑定全局存储
  @StorageLink('user_gender') @Watch('calculateAll') gender: number = 1;
  @StorageLink('user_height') @Watch('calculateAll') heightVal: number = 170;
  @StorageLink('user_weight') @Watch('calculateAll') weightVal: number = 65;
  @StorageLink('user_age') @Watch('calculateAll') age: number = 25;

  // 目标数据
  @StorageLink('targetCalories') targetCalories: number = 2000;
  @StorageLink('targetWater') targetWater: number = 2000;
  @StorageLink('targetExercise') targetExercise: number = 2;

  @State bmi: number = 0;

  aboutToAppear() {
    this.calculateAll();
  }

  // --- 核心算法 ---
  calculateAll() {
    const h_meter = this.heightVal / 100;
    if (h_meter > 0) {
      this.bmi = parseFloat((this.weightVal / (h_meter * h_meter)).toFixed(1));
    } else {
      this.bmi = 0;
    }

    let bmr = 0;
    if (this.gender === 1) {
      bmr = (10 * this.weightVal) + (6.25 * this.heightVal) - (5 * this.age) + 5;
    } else {
      bmr = (10 * this.weightVal) + (6.25 * this.heightVal) - (5 * this.age) - 161;
    }
    this.targetCalories = Math.round(bmr * 1.2);
    this.targetWater = Math.round(this.weightVal * 33);
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

  build() {
    Column() {
      // 1. 顶部大标题
      Text('健康档案')
        .fontSize(24 * this.fontScale)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .padding({ left: 20, top: 20, bottom: 10 })

      Scroll() {
        Column({ space: 15 }) {

          // --- 第一部分：基础信息 ---
          Column() {
            // 标题行
            Row() {
              Text('个人信息')
                .fontSize(18 * this.fontScale)
                .fontColor('#333')
                .fontWeight(FontWeight.Bold)

              Blank()

            }
            .width('100%')
            .padding({ left: 5, bottom: 8 })

            // 白色输入卡片
            Column() {
              this.GenderRow()
              Divider().color('#F5F5F5')
              this.InputRow(
                '年龄',
                this.age,
                '岁',
                (v) => this.age = v
              )
              Divider().color('#F5F5F5')
              this.InputRow(
                '身高',
                this.heightVal,
                'cm',
                (v) => this.heightVal = v
              )
              Divider().color('#F5F5F5')
              this.InputRow(
                '体重',
                this.weightVal,
                'kg',
                (v) => this.weightVal = v
              )
            }
            .padding({
              top: 5,
              bottom: 5,
              left: 16,
              right: 16
            })
            .backgroundColor(Color.White)
            .borderRadius(16)
            .shadow({ radius: 10, color: 'rgba(0,0,0,0.03)' })
          }
          .margin({ left: 16, right: 16 })


          // --- 第二部分：BMI 仪表盘 ---
          Column() {
            Text('身体质量指数 (BMI)')
              .fontSize(14 * this.fontScale)
              .fontColor('#666')
              .width('100%')
              .margin({ bottom: 15 })

            Stack({ alignContent: Alignment.Bottom }) {
              Gauge({ value: this.bmi, min: 10, max: 35 })
                .startAngle(210).endAngle(150)
                .colors([[ '#3F51B5', 0.2 ], [ '#00C853', 0.3 ], [ '#FFAB00', 0.2 ], [ '#FF5252', 0.3 ]])
                .strokeWidth(20)
                .width(220).height(220)

              // 中间数值
              Column() {
                Text(this.bmi.toFixed(1))
                  .fontSize(46 * this.fontScale)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#333')

                Text(this.getBMIStatus())
                  .fontSize(18 * this.fontScale)
                  .fontColor(this.getBMIColor())
                  .fontWeight(FontWeight.Medium)
                  .margin({ top: 4 })
              }
              .margin({ bottom: 70 })

              // 底部刻度
              Row() {
                Text('10').fontSize(12 * this.fontScale).fontColor('#999')
                Blank()
                Text('35').fontSize(12 * this.fontScale).fontColor('#999')
              }
              .width('100%')
              .padding({ left: 40, right: 40, bottom: 30 })
            }
          }
          .padding(16)
          .backgroundColor(Color.White)
          .borderRadius(16)
          .margin({ left: 16, right: 16 })


          // --- 第三部分：目标展示卡片 ---
          Row({ space: 12 }) { // space 稍微加大一点
            this.GoalCard(
              '每日热量',
              `${this.targetCalories}`,
              'kcal',
              '#FF9C6E'
            )
            this.GoalCard(
              '每日饮水',
              `${this.targetWater}`,
              'ml',
              '#5CDBD3'
            )
            this.GoalCard(
              '建议运动',
              `${this.targetExercise * 15}`,
              'min',
              '#52C41A'
            )
          }
          .margin({ left: 16, right: 16 })

          // --- 底部：通用设置 ---
          Button('通用设置')
            .width('90%')
            .backgroundColor('#FFFFFF')
            .fontColor('#333')
            .margin({ top: 10 })
            .borderRadius(12)
            .onClick(() => {
              router.pushUrl({ url: 'pages/Mine/SettingsPage' });
            })

          Blank().height(50)
        }
      }
    }
    .height('100%')
    .backgroundColor('#F1F3F5')
  }

  // --- 辅助逻辑 ---
  getBMIStatus(): string {
    if (this.bmi < 18.5) return '偏瘦';
    if (this.bmi < 24) return '正常';
    if (this.bmi < 28) return '超重';
    return '肥胖';
  }

  getBMIColor(): string {
    if (this.bmi < 18.5) return '#3F51B5';
    if (this.bmi < 24) return '#00C853';
    if (this.bmi < 28) return '#FFAB00';
    return '#FF5252';
  }

  // --- UI 构建器 ---

  @Builder GenderRow() {
    Row() {
      Text('性别').fontSize(14 * this.fontScale).fontWeight(FontWeight.Medium)
      Blank()
      Row({ space: 10 }) {
        this.GenderButton(1, '男')
        this.GenderButton(0, '女')
      }
    }
    .width('100%')
    .height(50)
  }

  @Builder GenderButton(val: number, text: string) {
    Text(text)
      .fontSize(14 * this.fontScale)
      .fontColor(this.gender === val ? '#FFF' : '#666')
      .backgroundColor(this.gender === val ? '#007DFF' : '#F0F0F0')
      .padding({ left: 15, right: 15, top: 6, bottom: 6 })
      .borderRadius(15)
      .onClick(() => {
        this.gender = val;
        this.calculateAll();
      })
  }

  @Builder InputRow(label: string, val: number, unit: string, onChange: (v: number) => void) {
    Row() {
      Text(label).fontSize(14 * this.fontScale).fontWeight(FontWeight.Medium)
      Blank()

      TextInput({ text: val ? val.toString() : '' })
        .type(InputType.Number)
        .textAlign(TextAlign.End)
        .backgroundColor(Color.Transparent)
        .width(100)
        .fontSize(16 * this.fontScale)
        .fontColor('#007DFF')
        .onChange((value) => {
          const num = Number(value);
          if (!isNaN(num)) {
            onChange(num);
            this.calculateAll();
          }
        })

      Text(unit).fontSize(12 * this.fontScale).fontColor('#999').margin({ left: 5 })
    }
    .width('100%')
    .height(50)
  }

  // 【修改点】优化了 GoalCard 的样式，增加了 Padding
  @Builder GoalCard(title: string, val: string, unit: string, color: string) {
    Column() {
      // 标题
      Text(title)
        .fontSize(14 * this.fontScale)
        .fontColor('#666')
        .alignSelf(ItemAlign.Start)
        .margin({ bottom: 8, left: 4 }) // 【关键修改】向右移了一点点

      // 数值
      Text(val)
        .fontSize(24 * this.fontScale)
        .fontWeight(FontWeight.Bold)
        .fontColor(color)

      // 单位
      Text(`目标 ${unit}`)
        .fontSize(12 * this.fontScale)
        .fontColor('#999')
    }
    .layoutWeight(1)
    .height(110) // 稍微加高一点，防止内容拥挤
    .backgroundColor(Color.White)
    .borderRadius(16)
    .justifyContent(FlexAlign.Center)
    .shadow({ radius: 5, color: 'rgba(0,0,0,0.05)' })
    .padding(15) // 【关键修改】增加了内边距，内容不再贴边
  }
}