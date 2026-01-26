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
 * 健康记录页面
 */
import { LifeHubData, HealthRecord as HealthRecordModel } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

// 健康类型选项接口
interface HealthTypeOption {
  value: string;
  label: string;
  unit: string;
  icon: string;
}

@Entry
@Component
export struct HealthRecord {
  private lifeHubData: LifeHubData = new LifeHubData();
  @State selectedType: string = 'weight';
  @State value: string = '';
  @State unit: string = 'kg';
  @State note: string = '';
  @State isSaving: boolean = false;
  private types: HealthTypeOption[] = [
    {
      value: 'weight',
      label: '体重',
      unit: 'kg',
      icon: '⚖️'
    },
    {
      value: 'blood_pressure',
      label: '血压',
      unit: 'mmHg',
      icon: '🩺'
    },
    {
      value: 'blood_sugar',
      label: '血糖',
      unit: 'mmol/L',
      icon: '🩸'
    },
    {
      value: 'exercise',
      label: '运动',
      unit: '分钟',
      icon: '🏃'
    },
    {
      value: 'sleep',
      label: '睡眠',
      unit: '小时',
      icon: '😴'
    }
  ];

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    // 根据类型设置默认单位
    this.updateUnit();
  }

  updateUnit() {
    const type = this.types.find(t => t.value === this.selectedType);
    if (type) {
      this.unit = type.unit;
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Button('取消')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => {
            router.back();
          })

        Text('记录健康')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        Button('保存')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor(this.value ? '#007DFF' : '#999')
          .enabled(!!this.value && !this.isSaving)
          .onClick(() => {
            this.saveRecord();
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          // 类型选择
          Column() {
            Text('记录类型')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ bottom: 15 })

            Flex({ wrap: FlexWrap.Wrap, justifyContent: FlexAlign.Start }) {
              ForEach(this.types, (type: HealthTypeOption) => {
                Column() {
                  Text(type.icon)
                    .fontSize(32)
                  Text(type.label)
                    .fontSize(14)
                    .fontColor('#333')
                    .margin({ top: 5 })
                }
                .width(80)
                .height(100)
                .justifyContent(FlexAlign.Center)
                .backgroundColor(this.selectedType === type.value ? '#4CAF5020' : '#f0f0f0')
                .borderRadius(10)
                .onClick(() => {
                  this.selectedType = type.value;
                  this.updateUnit();
                })
                .margin({ right: 10, bottom: 10 })
              })
            }
            .width('100%')
          }
          .width('100%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ top: 20, left: 15, right: 15 })

          // 数值输入
          Column() {
            Text('数值')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ bottom: 15 })

            Row() {
              TextInput({ placeholder: '请输入数值', text: this.value })
                .layoutWeight(1)
                .type(InputType.Number)
                .onChange((value: string) => {
                  this.value = value;
                })

              Text(this.unit)
                .fontSize(16)
                .fontColor('#666')
                .margin({ left: 10 })
            }
            .width('100%')
            .height(50)
            .padding({ left: 15, right: 15 })
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
          }
          .width('100%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ top: 15, left: 15, right: 15 })

          // 备注
          Column() {
            Text('备注（可选）')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ bottom: 15 })

            TextArea({ placeholder: '添加备注...', text: this.note })
              .width('100%')
              .height(100)
              .onChange((value: string) => {
                this.note = value;
              })
          }
          .width('100%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({
            top: 15,
            left: 15,
            right: 15,
            bottom: 20
          })
        }
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async saveRecord() {
    if (!this.value) {
      return;
    }

    this.isSaving = true;
    try {
      const record: HealthRecordModel = {
        type: this.selectedType as 'weight' | 'blood_pressure' | 'blood_sugar' | 'exercise' | 'sleep',
        value: parseFloat(this.value),
        unit: this.unit,
        note: this.note,
        date: this.getTodayString(),
        time: Date.now()
      };

      await this.lifeHubData.addHealthRecord(record);
      router.replaceUrl({
        url: 'pages/Index',
        params: {
          tab: 1
        }
      });
    } catch (error) {
      console.error('HealthRecord: 保存失败:', error);
    } finally {
      this.isSaving = false;
    }
  }

  getTodayString(): string {
    const date = new Date();
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }
}
