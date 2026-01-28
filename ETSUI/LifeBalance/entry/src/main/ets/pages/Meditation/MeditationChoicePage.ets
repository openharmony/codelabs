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
import router from '@ohos.router'

@Component
export struct MeditationChoicePage {
  @State mindfulnessTime: number = 10
  @State meditationTime: number = 10
  @StorageProp('appFontScale') fontScale: number = 1.0

  // 记录当前正在设置哪一种冥想（'正念' 或 '沉思'）
  @State currentType: string = ''

  // 自定义弹窗控制器
  dialogController: CustomDialogController = new CustomDialogController({
    builder: TimeSelectionDialog({
      fontScale: this.fontScale,
      // 【修改点 1】显式添加 : void 返回类型，解决标红报错
      onSelect: (val: number): void => this.handleTimeSelect(val)
    }),
    alignment: DialogAlignment.Bottom,
    offset: {
      dx: 0,
      dy: -20
    },
    customStyle: false,
    autoCancel: true
  })

  build() {
    Stack() {
      Column({ space: 20 }) {
        this.item('正念')
        this.item('沉思')
      }
      .padding(20)
      .height('100%')
      .justifyContent(FlexAlign.Start)

      // 右下角悬浮按钮
      Button('视频跟学')
        .width(140)
        .height(60)
        .fontSize(18 * this.fontScale)
        .borderRadius(30)
        .backgroundColor('#007DFF')
        .fontColor('#FFF')
        .position({
          bottom: 20,
          right: 20
        })
        .onClick(() => {
          router.pushUrl({ url: 'pages/Meditation/MeditationVideoPage' })
        })
    }
  }

  @Builder
  item(title: string) {
    Row() {
      Column({ space: 6 }) {
        Text(title)
          .fontSize(22 * this.fontScale)
          .fontWeight(FontWeight.Medium)

        Text('专注呼吸 · 放松身心')
          .fontSize(14 * this.fontScale)
          .fontColor('#888')
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
      .onClick(() => {
        router.pushUrl({
          url: title === '正念'
            ? 'pages/Meditation/MindfulnessPage'
            : 'pages/Meditation/MeditationPage',
          params: { duration: title == '正念' ? this.mindfulnessTime : this.meditationTime }
        })
      })

      // 右侧的时长选择区
      Row() {
        Text(`${title == '正念' ? this.mindfulnessTime : this.meditationTime} 分钟`)
          .fontSize(16 * this.fontScale)
        Text(' ▾')
          .fontSize(16 * this.fontScale)
          .margin({ left: 5 })
      }
      .onClick(() => this.showTimeDialog(title))
    }
    .padding(16)
    .backgroundColor('#F5F5F5')
    .borderRadius(16)
  }

  // 【修改点 2】显式添加 : void
  showTimeDialog(type: string): void {
    this.currentType = type;
    this.dialogController.open();
  }

  // 【修改点 3】显式添加 : void
  handleTimeSelect(val: number): void {
    if (this.currentType === '正念') {
      this.mindfulnessTime = val;
    } else {
      this.meditationTime = val;
    }
  }
}

// =========================================
//  自定义时间选择弹窗
// =========================================
@CustomDialog
struct TimeSelectionDialog {
  controller: CustomDialogController
  @Prop fontScale: number = 1.0
  // 【修改点 4】默认值的箭头函数也加上 : void，防止潜在报错
  onSelect: (value: number) => void = (value: number): void => {}

  build() {
    Column() {
      // 标题
      Text('选择冥想时长')
        .fontSize(18 * this.fontScale)
        .fontWeight(FontWeight.Bold)
        .padding({
          top: 20,
          bottom: 20
        })

      Column() {
        this.SelectionItem(
          5,
          '5 分钟'
        )
        Divider().color('#F0F0F0')
        this.SelectionItem(
          10,
          '10 分钟（推荐）'
        )
        Divider().color('#F0F0F0')
        this.SelectionItem(
          15,
          '15 分钟'
        )
      }
      .width('100%')

      // 取消按钮
      Button('取消')
        .type(ButtonType.Normal)
        .fontColor('#666')
        .fontSize(16 * this.fontScale)
        .backgroundColor(Color.White)
        .width('100%')
        .height(55)
        .onClick(() => this.controller.close())
    }
    .backgroundColor(Color.White)
    .borderRadius(24)
    .width('95%')
    .padding({ bottom: 10 })
  }

  @Builder
  SelectionItem(
    val: number,
    label: string
  ) {
    Button(label)
      .type(ButtonType.Normal)
      .fontColor('#007DFF')
      .fontSize(18 * this.fontScale)
      .backgroundColor(Color.White)
      .width('100%')
      .height(60)
      .onClick(() => {
        this.onSelect(val)
        this.controller.close()
      })
  }
}