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
 * 学习模块主页面
 */
import { LifeHubData, Skill } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Component
export struct LearningMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State skills: Skill[] = [];
  @State isLoading: boolean = true;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadSkills();
  }

  async loadSkills() {
    this.isLoading = true;
    try {
      this.skills = await this.lifeHubData.getAllSkills();
    } catch (error) {
      console.error('LearningMain: 加载数据失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  build() {
    Column() {
      Row() {
        Text('技能学习')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
        
        Button('+')
          .fontSize(20)
          .width(40)
          .height(40)
          .borderRadius(20)
          .backgroundColor('#9C27B0')
          .fontColor(Color.White)
          .onClick(() => {
            router.pushUrl({ url: 'pages/learning/SkillDetail' });
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
      } else if (this.skills.length === 0) {
        Column() {
          Text('📚')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('还没有技能')
            .fontSize(18)
            .fontColor('#666')
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List() {
          ForEach(this.skills, (skill: Skill) => {
            ListItem() {
              Column() {
                Row() {
                  Text(skill.name)
                    .fontSize(18)
                    .fontWeight(FontWeight.Medium)
                    .layoutWeight(1)

                  Text(`Lv.${skill.level}/${skill.targetLevel}`)
                    .fontSize(16)
                    .fontColor('#9C27B0')
                    .fontWeight(FontWeight.Bold)
                }
                .width('100%')

                // 等级进度条
                Row() {
                  Blank()
                    .layoutWeight(1 - skill.level / skill.targetLevel)
                    .backgroundColor('#e0e0e0')
                    .height(6)
                    .borderRadius(3)
                  
                  Blank()
                    .layoutWeight(skill.level / skill.targetLevel)
                    .backgroundColor('#9C27B0')
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
                  url: 'pages/learning/SkillDetail',
                  params: { skillId: skill.id }
                });
              })
            }
          }, (skill: Skill) => skill.id?.toString() || '')
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
