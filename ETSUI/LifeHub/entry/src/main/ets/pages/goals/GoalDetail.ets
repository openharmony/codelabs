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
 * 目标详情/创建页面
 */
import { LifeHubData, Goal } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Entry
@Component
export struct GoalDetail {
  private lifeHubData: LifeHubData = new LifeHubData();
  @State title: string = '';
  @State description: string = '';
  @State category: string = '生活';
  @State priority: 'high' | 'medium' | 'low' = 'medium';
  @State progress: number = 0;
  @State isSaving: boolean = false;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
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

        Text('创建目标')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        Button('保存')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor(this.title ? '#007DFF' : '#999')
          .enabled(!!this.title && !this.isSaving)
          .onClick(() => {
            this.saveGoal();
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          TextInput({ placeholder: '目标标题', text: this.title })
            .width('100%')
            .height(50)
            .fontSize(18)
            .padding({ left: 15, right: 15 })
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.title = value;
            })
            .margin({ top: 20, left: 15, right: 15 })

          TextArea({ placeholder: '目标描述（可选）', text: this.description })
            .width('100%')
            .height(100)
            .padding(15)
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.description = value;
            })
            .margin({ top: 15, left: 15, right: 15 })
        }
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async saveGoal() {
    if (!this.title) {
      return;
    }

    this.isSaving = true;
    try {
      const goal: Goal = {
        title: this.title,
        description: this.description,
        category: this.category,
        priority: this.priority,
        progress: this.progress,
        status: 'not_started',
        createTime: Date.now(),
        updateTime: Date.now()
      };

      await this.lifeHubData.addGoal(goal);
      router.back();
    } catch (error) {
      console.error('GoalDetail: 保存失败:', error);
    } finally {
      this.isSaving = false;
    }
  }
}
