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
 * 日记模块主页面
 */
import { LifeHubData, Diary } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Component
export struct DiaryMain {
  private lifeHubData: LifeHubData = new LifeHubData();

  @State diaries: Diary[] = [];
  @State isLoading: boolean = true;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadDiaries();
  }

  async loadDiaries() {
    this.isLoading = true;
    try {
      this.diaries = await this.lifeHubData.getAllDiaries();
    } catch (error) {
      console.error('DiaryMain: 加载数据失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  build() {
    Column() {
      Row() {
        Text('我的日记')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)

        Blank()

        Button('+')
          .fontSize(20)
          .width(40)
          .height(40)
          .borderRadius(20)
          .backgroundColor('#00BCD4')
          .fontColor(Color.White)
          .onClick(() => {
            router.pushUrl({ url: 'pages/diary/DiaryEdit' });
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
      } else if (this.diaries.length === 0) {
        Column() {
          Text('📝')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('还没有日记')
            .fontSize(18)
            .fontColor('#666')
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List() {
          ForEach(this.diaries, (diary: Diary) => {
            ListItem() {
              Column() {
                Row() {
                  Text(diary.mood || '📝')
                    .fontSize(24)
                    .width(40)

                  Column() {
                    Text(diary.title || diary.date)
                      .fontSize(18)
                      .fontWeight(FontWeight.Medium)
                      .textAlign(TextAlign.Start)
                      .width('100%')

                    Text(diary.content)
                      .fontSize(14)
                      .fontColor('#666')
                      .textAlign(TextAlign.Start)
                      .width('100%')
                      .margin({ top: 5 })
                      .maxLines(2)
                      .textOverflow({ overflow: TextOverflow.Ellipsis })

                    Text(diary.date)
                      .fontSize(12)
                      .fontColor('#999')
                      .textAlign(TextAlign.Start)
                      .width('100%')
                      .margin({ top: 5 })
                  }
                  .layoutWeight(1)
                  .alignItems(HorizontalAlign.Start)
                  .padding({ left: 15 })
                }
                .width('100%')
              }
              .width('100%')
              .padding(15)
              .alignItems(HorizontalAlign.Start)
              .backgroundColor(Color.White)
              .borderRadius(10)
              .margin({ left: 15, right: 15, bottom: 10 })
              .onClick(() => {
                router.pushUrl({
                  url: 'pages/diary/DiaryEdit',
                  params: { diaryId: diary.id }
                });
              })
            }
          }, (diary: Diary) => diary.id?.toString() || '')
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
