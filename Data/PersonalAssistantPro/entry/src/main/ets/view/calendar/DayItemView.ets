/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
// entry/src/main/ets/view/calendar/DayItemView.ets
import { CalendarModel } from '../../viewmodel/CalendarViewModel';

@Component
export struct DayItemView {
  @ObjectLink dayModel: CalendarModel;
  @Prop selectedTimestamp: number;
  @Prop stripType: string;
  @Prop hasPoint: boolean;

  // 常量定义
  private readonly ITEM_SIZE = 40;
  private readonly CIRCLE_SIZE = 36;
  private readonly DOT_SIZE = 4;

  build() {
    Stack({ alignContent: Alignment.Center }) { // 父容器依然居中，服务于大部分元素

      // ===========================================
      // 1. [居中层] 跨天背景条
      // ===========================================
      if (this.stripType && this.stripType !== '') {
        Row()
          .width('100%')
          .height(this.CIRCLE_SIZE)
          .backgroundColor('#E5F1FF')
          .borderRadius(this.getStripBorderRadius())
          .margin({
            left: this.stripType === 'head' ? 4 : 0,
            right: this.stripType === 'tail' ? 4 : 0
          })
      }

      // ===========================================
      // 2. [居中层] 选中圆圈
      // ===========================================
      if (this.isSelected()) {
        Circle({ width: this.CIRCLE_SIZE, height: this.CIRCLE_SIZE })
          .fill('#007DFF')
      }

      // ===========================================
      // 3. [居中层] 日期文字
      // ===========================================
      Text(this.dayModel.day.toString())
        .fontSize(16)
        .fontWeight(this.dayModel.isToday ? FontWeight.Bold : FontWeight.Medium)
        .fontColor(this.getTextColor())
        .hitTestBehavior(HitTestMode.None)

      // ===========================================
      // 4. [底部层] 单天小圆点 (独立容器)
      //  修复方案：用一个铺满的 Column 专门负责把点“挤”到底部
      // ===========================================
      if (this.hasPoint) {
        Column() {
          Circle({ width: this.DOT_SIZE, height: this.DOT_SIZE })
            .fill(this.isSelected() ? Color.White : '#007DFF')
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.End) // 垂直到底
        .alignItems(HorizontalAlign.Center) // 水平居中
        .padding({ bottom: 4 }) // 距离底部的间距
        .hitTestBehavior(HitTestMode.None) // 避免挡住点击
      }

    }
    .width('100%')
    .height(this.ITEM_SIZE)
    .opacity(this.dayModel.isCurrentMonth ? 1 : 0.4)
  }

  private getStripBorderRadius(): BorderRadiuses | number {
    const r = 18;
    if (this.stripType === 'head') return { topLeft: r, bottomLeft: r, topRight: 0, bottomRight: 0 };
    if (this.stripType === 'tail') return { topLeft: 0, bottomLeft: 0, topRight: r, bottomRight: r };
    if (this.stripType === 'body') return 0;
    return 0;
  }

  private getTextColor(): string {
    if (this.isSelected()) return '#FFFFFF';
    if (this.dayModel.isToday) return '#007DFF';
    return '#182431';
  }

  private isSelected(): boolean {
    return this.dayModel.timestamp === this.selectedTimestamp;
  }
}