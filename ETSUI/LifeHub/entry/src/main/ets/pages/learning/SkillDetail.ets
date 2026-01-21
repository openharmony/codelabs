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
 * 技能详情/创建页面
 */
import { LifeHubData, Skill } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Entry
@Component
export struct SkillDetail {
  private lifeHubData: LifeHubData = new LifeHubData();
  @State name: string = '';
  @State category: string = '技能';
  @State level: number = 1;
  @State targetLevel: number = 5;
  @State description: string = '';
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

        Text('添加技能')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        Button('保存')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor(this.name ? '#007DFF' : '#999')
          .enabled(!!this.name && !this.isSaving)
          .onClick(() => {
            this.saveSkill();
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          TextInput({ placeholder: '技能名称', text: this.name })
            .width('100%')
            .height(50)
            .fontSize(18)
            .padding({ left: 15, right: 15 })
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.name = value;
            })
            .margin({ top: 20, left: 15, right: 15 })

          TextArea({ placeholder: '技能描述（可选）', text: this.description })
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

  async saveSkill() {
    if (!this.name) {
      return;
    }

    this.isSaving = true;
    try {
      const skill: Skill = {
        name: this.name,
        category: this.category,
        level: this.level,
        targetLevel: this.targetLevel,
        description: this.description,
        createTime: Date.now(),
        updateTime: Date.now()
      };

      await this.lifeHubData.addSkill(skill);
      router.back();
    } catch (error) {
      console.error('SkillDetail: 保存失败:', error);
    } finally {
      this.isSaving = false;
    }
  }
}
