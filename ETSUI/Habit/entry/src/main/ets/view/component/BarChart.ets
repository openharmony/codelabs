/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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
@Component
export struct BarChart {
  @Prop data: number[];
  @Prop labels: string[];

  build() {
    Column() {
      Text("近 7 天趋势")
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .margin({ bottom: 20 })

      // 图表主体区域
      Row({ space: 10 }) {
        // 找出最大值，确保比例正确
        ForEach(this.data, (count: number, index: number) => {
          this.BarItem(count, this.labels[index], this.getMaxValue())
        })
      }
      .width('100%')
      // 🔴 删除固定的 .height(150)，让内容自己撑开
      .alignItems(VerticalAlign.Bottom)
      .justifyContent(FlexAlign.SpaceBetween)
    }
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(16)
    .width('100%')
  }

  getMaxValue(): number {
    let max = 0;
    this.data.forEach(v => {
      if (v > max) {
        max = v;
      }
    });
    return max < 5 ? 5 : max;
  }

  @Builder
  BarItem(count: number, label: string, max: number) {
    Column() {
      // 1. 柱子容器 (固定高度安全区)
      // 柱子最高只能在这个 100vp 的盒子里生长，绝对不会挤到下面的字
      Column() {
        Column()
          .width(12)
          // 计算高度百分比
          .height(`${(count / max) * 100}%`)
          .backgroundColor(count > 0 ? '#007DFF' : '#F0F0F0')
          .borderRadius({ topLeft: 4, topRight: 4 })
          .animation({ duration: 500 })
      }
      .height(100) // 👈 关键修复：给柱子划定高度上限
      .width('100%')
      .justifyContent(FlexAlign.End) // 让柱子沉底对齐
      .alignItems(HorizontalAlign.Center)

      // 2. 数值 (放在柱子上面一点点，或者这里简单处理放在下面)
      // 为了布局整齐，我们把数值暂时隐藏，或者你也可以放出来
      // Text(count.toString()).fontSize(10)...

      // 3. 日期标签 (安全地待在下面)
      Text(label)
        .fontSize(10)
        .fontColor('#666')
        .margin({ top: 8 }) // 👈 增加一点间距，更美观
        .maxLines(1) // 防止换行
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    // 让每列平分宽度，防止挤在一起
    .layoutWeight(1)
  }
}