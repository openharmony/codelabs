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
 * 创建/编辑旅行计划页面
 */
import { LifeHubData, TravelPlan as TravelPlanModel } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Entry
@Component
export struct TravelPlan {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State title: string = '';
  @State destination: string = '';
  @State startDate: string = '';
  @State endDate: string = '';
  @State budget: string = '';
  @State description: string = '';
  @State isSaving: boolean = false;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    // 设置默认日期为今天和一周后
    const today = new Date();
    const nextWeek = new Date(today);
    nextWeek.setDate(nextWeek.getDate() + 7);
    this.startDate = this.formatDate(today);
    this.endDate = this.formatDate(nextWeek);
  }

  formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }

  build() {
    Column() {
      Row() {
        Button('取消')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => {
            router.back();
          })

        Text('创建旅行计划')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        Button('保存')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor(this.title && this.destination ? '#007DFF' : '#999')
          .enabled(!!this.title && !!this.destination && !this.isSaving)
          .onClick(() => {
            this.savePlan();
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          this.buildInputField('行程标题', this.title, (value: string) => {
            this.title = value;
          }, true)

          this.buildInputField('目的地', this.destination, (value: string) => {
            this.destination = value;
          }, true)

          this.buildInputField('开始日期', this.startDate, (value: string) => {
            this.startDate = value;
          }, true)

          this.buildInputField('结束日期', this.endDate, (value: string) => {
            this.endDate = value;
          }, true)

          this.buildInputField('预算', this.budget, (value: string) => {
            this.budget = value;
          }, false)

          Column() {
            Text('描述（可选）')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ bottom: 10 })

            TextArea({ placeholder: '添加行程描述...', text: this.description })
              .width('100%')
              .height(100)
              .onChange((value: string) => {
                this.description = value;
              })
          }
          .width('100%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ top: 15, left: 15, right: 15, bottom: 20 })
        }
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  @Builder
  buildInputField(label: string, value: string, onChange: (value: string) => void, required: boolean = false) {
    Column() {
      Row() {
        Text(label)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
        if (required) {
          Text('*')
            .fontSize(16)
            .fontColor('#ff3b30')
        }
      }
      .width('100%')
      .margin({ bottom: 10 })

      TextInput({ placeholder: `请输入${label}`, text: value })
        .width('100%')
        .height(50)
        .padding({ left: 15, right: 15 })
        .backgroundColor('#f8f8f8')
        .borderRadius(8)
        .onChange(onChange)
    }
    .width('100%')
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .margin({ top: 15, left: 15, right: 15 })
  }

  async savePlan() {
    if (!this.title || !this.destination) return;

    this.isSaving = true;
    try {
      const plan: TravelPlanModel = {
        title: this.title,
        destination: this.destination,
        startDate: this.startDate,
        endDate: this.endDate,
        budget: parseFloat(this.budget) || 0,
        actualCost: 0,
        status: 'planning',
        createTime: Date.now(),
        description: this.description
      };

      await this.lifeHubData.addTravelPlan(plan);
      router.back();
    } catch (error) {
      console.error('TravelPlan: 保存失败:', error);
    } finally {
      this.isSaving = false;
    }
  }
}
