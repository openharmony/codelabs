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

@Entry
@Component
struct SettingsPage {
  // 关联全局存储的 appFontScale (缩放系数)
  // 当 UI 修改这个值时，PersistenceStorage 会自动持久化，所有 @StorageProp 的页面都会更新
  @StorageLink('appFontScale') fontScale: number = 1.0;

  // 滑块的进度值 (0, 1, 2, 3)
  // 我们将用这个整数值来映射到具体的缩放倍率 (0.85, 1.0, 1.15, 1.30)
  @State sliderValue: number = 1;

  // 映射关系配置
  private readonly STEP_MAP = [0.85, 1.0, 1.15, 1.30];
  private readonly TEXT_MAP = ['小', '标准', '大', '超大'];

  aboutToAppear() {
    // 初始化时，根据当前的 scale 反推滑块位置
    // 简单的最近匹配算法
    let minDiff = 100;
    let closestIndex = 1;

    for (let i = 0; i < this.STEP_MAP.length; i++) {
      const diff = Math.abs(this.fontScale - this.STEP_MAP[i]);
      if (diff < minDiff) {
        minDiff = diff;
        closestIndex = i;
      }
    }
    this.sliderValue = closestIndex;
  }

  build() {
    Column() {
      // 1. 标题栏
      Row() {
        Image($r('app.media.icon')) // 使用你已有的图标或返回图标
          .width(24)
          .height(24)
          .margin({ right: 16 })
          .onClick(() => {
            router.back();
          })

        Text('字体大小设置')
          .fontSize(20 * this.fontScale)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
      .padding(16)
      .backgroundColor(Color.White)

      // 2. 预览区域 (模拟对话列表，让用户直观看到效果)
      List() {
        ListItem() {
          this.ChatBubble(false, '你好，请问这个字体大小看得清吗？')
        }
        ListItem() {
          this.ChatBubble(true, '这是当前的显示效果，你可以拖动下方的滑块进行调整。')
        }
        ListItem() {
          this.ChatBubble(false, '好的，我觉得这个功能非常实用！')
        }
      }
      .layoutWeight(1)
      .width('100%')
      .padding(16)

      // 3. 底部控制区 (仿照 Codelab 样式)
      Column() {
        Text('调整字体大小')
          .fontSize(14 * this.fontScale)
          .fontColor('#666')
          .margin({ bottom: 20 })

        // 档位文字
        Text(this.TEXT_MAP[this.sliderValue])
          .fontSize(16 * this.fontScale)
          .fontWeight(FontWeight.Bold)
          .fontColor('#007DFF')
          .margin({ bottom: 10 })

        Row() {
          Text('A')
            .fontSize(14) // 这里的 A 用绝对大小，作为参照物
            .fontWeight(FontWeight.Bold)
            .fontColor('#999')

          // 滑块组件
          Slider({
            value: this.sliderValue,
            min: 0,
            max: 3, // 对应数组索引 0-3
            step: 1,
            style: SliderStyle.InSet
          })
            .layoutWeight(1)
            .showSteps(true)
            .margin({ left: 10, right: 10 })
            .onChange((value: number, mode: SliderChangeMode) => {
              this.sliderValue = value;
              // 实时修改全局缩放比例
              this.fontScale = this.STEP_MAP[value];
            })

          Text('A')
            .fontSize(24) // 大号 A 作为参照
            .fontWeight(FontWeight.Bold)
            .fontColor('#333')
        }
        .width('100%')
        .padding({ left: 20, right: 20 })
      }
      .width('94%')
      .backgroundColor(Color.White)
      .borderRadius(24)
      .padding({ top: 20, bottom: 30 })
      .margin({ bottom: 20 })
      .shadow({ radius: 20, color: 'rgba(0,0,0,0.05)' })

    }
    .height('100%')
    .width('100%')
    .backgroundColor('#F1F3F5')
  }

  // 聊天气泡组件 (仅作展示用)
  @Builder ChatBubble(isSelf: boolean, content: string) {
    Row() {
      if (isSelf) {
        Blank()
        Text(content)
          .fontSize(16 * this.fontScale)
          .fontColor(Color.White)
          .backgroundColor('#007DFF')
          .padding(12)
          .borderRadius({ topLeft: 12, topRight: 0, bottomLeft: 12, bottomRight: 12 })
          .constraintSize({ maxWidth: '75%' })

        Image($r('app.media.startIcon')) // 你的头像资源
          .width(40)
          .height(40)
          .borderRadius(20)
          .margin({ left: 10 })
          .backgroundColor('#DDD')
      } else {
        Image($r('app.media.icon')) // 对方头像
          .width(40)
          .height(40)
          .borderRadius(20)
          .margin({ right: 10 })
          .backgroundColor('#DDD')

        Text(content)
          .fontSize(16 * this.fontScale)
          .fontColor('#333')
          .backgroundColor(Color.White)
          .padding(12)
          .borderRadius({ topLeft: 0, topRight: 12, bottomLeft: 12, bottomRight: 12 })
          .constraintSize({ maxWidth: '75%' })
        Blank()
      }
    }
    .width('100%')
    .padding({ bottom: 15 })
  }
}