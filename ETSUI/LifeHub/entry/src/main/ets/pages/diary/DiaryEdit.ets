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
 * 写日记/编辑日记页面
 */
import { LifeHubData, Diary } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Entry
@Component
export struct DiaryEdit {
  private lifeHubData: LifeHubData = new LifeHubData();
  private diaryId: number = 0;
  @State title: string = '';
  @State content: string = '';
  @State mood: string = '😊';
  @State isSaving: boolean = false;
  private moods = ['😄', '😊', '😐', '😔', '😢'];

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    const params = router.getParams() as Record<string, number>;
    this.diaryId = params?.diaryId || 0;
    if (this.diaryId) {
      this.loadDiary();
    }
  }

  async loadDiary() {
    // TODO: 实现加载日记详情
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

        Text('写日记')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        Button('保存')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor(this.content ? '#007DFF' : '#999')
          .enabled(!!this.content && !this.isSaving)
          .onClick(() => {
            this.saveDiary();
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          // 心情选择
          Row() {
            Text('心情：')
              .fontSize(16)
              .fontColor('#666')

            ForEach(this.moods, (m: string) => {
              Text(m)
                .fontSize(24)
                .opacity(this.mood === m ? 1 : 0.5)
                .onClick(() => {
                  this.mood = m;
                })
                .margin({ left: 10 })
            })
          }
          .width('100%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ top: 20, left: 15, right: 15 })

          // 标题
          TextInput({ placeholder: '标题（可选）', text: this.title })
            .width('100%')
            .height(50)
            .fontSize(18)
            .padding({ left: 15, right: 15 })
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.title = value;
            })
            .margin({ top: 15, left: 15, right: 15 })

          // 内容
          TextArea({ placeholder: '记录今天的生活...', text: this.content })
            .width('100%')
            .height(300)
            .fontSize(16)
            .padding(15)
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.content = value;
            })
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

  async saveDiary() {
    if (!this.content) {
      return;
    }

    this.isSaving = true;
    try {
      const diary: Diary = {
        title: this.title,
        content: this.content,
        mood: this.mood,
        date: this.getTodayString(),
        createTime: Date.now(),
        updateTime: Date.now()
      };

      await this.lifeHubData.addDiary(diary);
      router.back();
    } catch (error) {
      console.error('DiaryEdit: 保存失败:', error);
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
