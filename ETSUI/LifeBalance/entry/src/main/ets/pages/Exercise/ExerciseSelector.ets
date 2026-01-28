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
import promptAction from '@ohos.promptAction';
import { ExerciseCategory, ExerciseItem, EXERCISE_DATABASE, getExercisesByCategory } from '../../model/ExerciseModel';

@CustomDialog
export struct ExerciseSelectorDialog {
  controller: CustomDialogController;
  onConfirm: (item: ExerciseItem, count: number) => void = () => {};

  @State currentCategoryIndex: number = 0;
  @State expandedId: number = -1;
  @State count: number = 1;

  // 1. 引入全局字体缩放
  @StorageProp('appFontScale') fontScale: number = 1.0;

  @State keyword: string = '';
  @State searchResults: ExerciseItem[] = [];

  @State customName: string = ''
  @State customCalories: string = ''

  private categories: ExerciseCategory[] = [
    ExerciseCategory.AEROBIC, ExerciseCategory.STRENGTH_TRAINING,
    ExerciseCategory.YOGA_FLEXIBILITY, ExerciseCategory.BALL_SPORTS,
    ExerciseCategory.OUTDOOR_RECREATION, ExerciseCategory.DAILY_ACTIVITIES,
    ExerciseCategory.CUSTOM
  ];

  handleSearch(value: string) {
    this.keyword = value;
    if (value.trim() === '') {
      this.searchResults = [];
    } else {
      this.searchResults = EXERCISE_DATABASE.filter(item =>
      item.name.includes(value)
      );
    }
    this.expandedId = -1;
  }

  build() {
    Column() {
      // 1. 标题栏
      Row() {
        Text('添加运动')
          .fontSize(20 * this.fontScale)
          .fontWeight(FontWeight.Bold)
        Blank()
        Text('关闭')
          .fontSize(14 * this.fontScale)
          .fontColor('#999')
          .onClick(() => this.controller.close())
      }.width('100%').padding(16)

      // 2. 【核心修复】搜索栏适配
      Search({
        placeholder: '搜索运动名称...',
        value: this.keyword
      })
        .width('90%')
        .height(40 * this.fontScale)
        .margin({ bottom: 10 })
        .textFont({ size: 14 * this.fontScale })
        .placeholderFont({ size: 14 * this.fontScale })
        .onChange((value) => {
          this.handleSearch(value);
        })

      Divider()

      // 3. 内容区域
      if (this.keyword.length > 0) {
        if (this.searchResults.length === 0) {
          Column() {
            Image($r('app.media.icon'))
              .width(60)
              .opacity(0.2)
              .grayscale(1)
              .margin({top: 50})
            Text('未找到相关运动')
              .fontSize(14 * this.fontScale)
              .fontColor('#999')
              .margin({ top: 10 })
          }
          .height(450)
          .width('100%')
        } else {
          this.ExerciseListBuilder(this.searchResults)
        }
      } else {
        Tabs({ index: this.currentCategoryIndex }) {
          ForEach(
            this.categories,
            (cat: ExerciseCategory, index: number) => {
            TabContent() {
              if (cat === ExerciseCategory.CUSTOM) {
                this.CustomInputForm()
              } else {
                this.ExerciseListBuilder(getExercisesByCategory(cat))
              }
            }
            .tabBar(this.TabItem(cat, index))
          })
        }
        .barMode(BarMode.Scrollable)
        .height(450)
        .onChange((index) => {
          this.currentCategoryIndex = index
          this.expandedId = -1;
        })
      }
    }
    .backgroundColor(Color.White).borderRadius(24)
  }

  @Builder
  TabItem(
    name: string,
    index: number
  ) {
    Text(name)
      .fontSize((this.currentCategoryIndex === index ? 16 : 14) * this.fontScale)
      .fontColor(this.currentCategoryIndex === index ? '#007DFF' : '#666')
      .fontWeight(this.currentCategoryIndex === index ? FontWeight.Bold : FontWeight.Normal)
      .padding(10)
  }

  @Builder
  ExerciseListBuilder(items: ExerciseItem[]) {
    List() {
      ForEach(items, (item: ExerciseItem) => {
        ListItem() {
          Column() {
            // 常规行
            Row() {
              Column({ space: 4 }) {
                Text(item.name)
                  .fontSize(16 * this.fontScale)
                  .fontColor('#333')

                if (this.keyword.length > 0) {
                  Text(`${item.category}`)
                    .fontSize(10 * this.fontScale)
                    .fontColor('#999')
                }
              }.alignItems(HorizontalAlign.Start)
              Blank()
              Text(`${item.calories} kcal`)
                .fontSize(16 * this.fontScale)
                .fontWeight(this.expandedId === item.id ? FontWeight.Bold : FontWeight.Normal)
                .fontColor(this.expandedId === item.id ? '#007DFF' : '#333')
            }
            .width('100%')
            .padding({ left: 16, right: 16, top: 12, bottom: 12 })
            .backgroundColor(this.expandedId === item.id ? '#F0F7FF' : Color.White)
            .onClick(() => {
              animateTo({ duration: 300, curve: Curve.FastOutSlowIn }, () => {
                if (this.expandedId === item.id) {
                  this.expandedId = -1;
                } else {
                  this.expandedId = item.id;
                  this.count = 1;
                }
              })
            })

            // 展开内容
            if (this.expandedId === item.id) {
              Column() {
                Divider().color('#E6F7FF')
                Row() {
                  Column() {
                    Text(`${Math.round(item.calories * this.count)}`)
                      .fontSize(24 * this.fontScale)
                      .fontWeight(FontWeight.Bold)
                      .fontColor('#007DFF')
                    Text('kcal')
                      .fontSize(12 * this.fontScale)
                      .fontColor('#999')
                  }
                }
                .justifyContent(FlexAlign.End)
                .width('100%')
                .padding(16)

                Row({ space: 10 }) {
                  Text('时长')
                    .fontSize(14 * this.fontScale)
                    .fontColor('#666')
                  Counter() {
                    Text(`${this.count * 15} 分钟`)
                      .fontSize(14 * this.fontScale)
                      .fontWeight(FontWeight.Bold)
                  }
                  .onInc(() => { if (this.count < 12 * 4) this.count += 1 })
                  .onDec(() => { if (this.count > 1) this.count -= 1 })
                  .height(30)
                  .width(140)

                  Blank()

                  Button('添加')
                    .height(32)
                    .fontSize(14 * this.fontScale)
                    .backgroundColor('#007DFF')
                    .onClick(() => {
                      this.onConfirm(item, this.count);
                      this.controller.close();
                    })
                }
                .width('100%')
                .padding({
                  left: 16,
                  right: 16,
                  bottom: 16
                })
              }
              .backgroundColor('#F0F7FF')
            }
          }
        }
      })
    }
    .width('100%').height('100%')
  }

  @Builder
  CustomInputForm() {
    Column({ space: 15 }) {
      Text('手动输入运动信息')
        .fontSize(14 * this.fontScale)
        .fontColor('#999')
        .alignSelf(ItemAlign.Start)
      TextInput({ placeholder: '运动名称', text: this.customName })
        .onChange(v => this.customName = v)
        .height(45)
      TextInput({ placeholder: '热量 (kcal) / 15分钟', text: this.customCalories })
        .type(InputType.Number)
        .onChange(v => this.customCalories = v)
        .height(45)
      Row() {
        Text('时长')
          .fontColor('#333')
          .fontSize(14 * this.fontScale)
        Blank()
        Counter() {
          Text(`${this.count * 15} 分钟`)
            .fontSize(14 * this.fontScale)
            .fontWeight(FontWeight.Bold)
        }
        .onInc(() => { if (this.count < 12 * 4) this.count += 1 })
        .onDec(() => { if (this.count > 1) this.count -= 1 })
        .height(30)
        .width(140)
      }
      .width('100%')
      Button('确认添加')
        .width('100%')
        .margin({ top: 10 })
        .onClick(() => this.handleCustomAdd())
    }
    .padding(20)
    .height('100%')
    .justifyContent(FlexAlign.Start)
  }

  handleCustomAdd() {
    if (!this.customName || !this.customCalories) {
      promptAction.showToast({ message: '请输入名称和热量' });
      return;
    }
    const customItem: ExerciseItem = {
      id: new Date().getTime(),
      name: this.customName,
      calories: Number(this.customCalories),
      category: ExerciseCategory.CUSTOM
    };
    this.onConfirm(customItem, this.count);
    this.controller.close();
  }
}