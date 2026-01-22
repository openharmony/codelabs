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
// src/main/ets/view/form/DatePickerItem.ets
import { StyleConstants } from '../../common/constants/StyleConstants';
import { DateUtils } from '../../common/utils/DateUtils';

/**
 * 封装的日期/时间选择组件
 * * 功能特点：
 * 1. 支持日期选择 (YYYY-MM-DD)
 * 2. 支持时间选择 (HH:mm)
 * 3. 自定义标题和占位符
 * 4. 底部弹窗交互
 */
@Component
export struct DatePickerItem {
  // === 属性定义 ===

  /** 标题文本 */
  @Prop title: string | Resource = '';
  /** 当前选中的时间戳 */
  @Link timestamp: number;
  /** 选择模式: true=日期, false=时间 */
  @Prop isDateMode: boolean = true;
  /** 占位提示文案 */
  // 📝 [修改] 类型增加 Resource，默认值改为资源引用
  @Prop placeholder: string | Resource = $r('app.string.common_select_placeholder');
  // === 内部状态 ===
  @State private isPressed: boolean = false;

  /**
   * 构建 UI
   */
  build() {
    Column() {
      // 1. 标题区域
      Text(this.title)
        .fontSize(StyleConstants.ICON_SIZE_MD)
        .fontColor(StyleConstants.COLOR_TEXT_SECONDARY)
        .margin({ bottom: StyleConstants.SPACE_XS })
        .width('100%')

      // 2. 交互区域 (模拟输入框样式)
      Row() {
        // 显示选中的时间，或者占位符
        Text(this.getDisplayText())
          .fontSize(StyleConstants.ICON_SIZE_LG)
          .fontColor(this.timestamp > 0 ? StyleConstants.COLOR_TEXT_PRIMARY : StyleConstants.COLOR_TEXT_SECONDARY)
          .layoutWeight(1)

        // 右侧箭头图标
        Image($r('app.media.ic_back')) // 复用返回图标，旋转180度
          .width(16)
          .height(16)
          .fillColor(StyleConstants.COLOR_TEXT_SECONDARY)
          .rotate({ angle: 180 })
          .opacity(0.5)
      }
      .width('100%')
      .height(StyleConstants.HEIGHT_INPUT)
      .backgroundColor(this.isPressed ? '#F5F5F5' : Color.Transparent) // 按压态
      .border({
        width: { bottom: 1 },
        color: StyleConstants.COLOR_DIVIDER
      })
      // 3. 点击交互事件
      .onClick(() => {
        this.showPickerDialog();
      })
      .onTouch((event) => {
        if (event.type === TouchType.Down) {
          this.isPressed = true;
        } else if (event.type === TouchType.Up || event.type === TouchType.Cancel) {
          this.isPressed = false;
        }
      })
    }
    .width('100%')
    .padding({
      top: StyleConstants.SPACE_MD,
      bottom: StyleConstants.SPACE_MD
    })
  }

  /**
   * 获取显示的文本内容
   */
  // 📝 [修改] 返回类型兼容 Resource
  private getDisplayText(): string | Resource {
    if (!this.timestamp || this.timestamp <= 0) {
      return this.placeholder;
    }
    if (this.isDateMode) {
      return DateUtils.formatDate(this.timestamp);
    } else {
      return DateUtils.formatTime(this.timestamp);
    }
  }

  /**
   * 弹出系统选择器
   */
  private showPickerDialog() {
    // 默认选中时间，如果没有则用当前时间
    const current = this.timestamp > 0 ? new Date(this.timestamp) : new Date();

    if (this.isDateMode) {
      // --- 日期选择模式 ---
      DatePickerDialog.show({
        start: new Date("2000-1-1"),
        end: new Date("2100-12-31"),
        selected: current,
        onAccept: (value: DatePickerResult) => {
          // 通过 value.year, value.month, value.day 重新构造时间戳
          const newDate = new Date();
          newDate.setFullYear(value.year!, value.month!, value.day!);
          // 保留原来的时分秒信息 (可选逻辑)
          if (this.timestamp > 0) {
            const oldDate = new Date(this.timestamp);
            newDate.setHours(oldDate.getHours(), oldDate.getMinutes());
          } else {
            newDate.setHours(9, 0, 0); // 默认上午9点
          }
          this.timestamp = newDate.getTime();
        }
      });
    } else {
      // --- 时间选择模式 ---
      TimePickerDialog.show({
        selected: current,
        useMilitaryTime: true, // 24小时制
        onAccept: (value: TimePickerResult) => {
          const newDate = this.timestamp > 0 ? new Date(this.timestamp) : new Date();
          newDate.setHours(value.hour!, value.minute!);
          this.timestamp = newDate.getTime();
        }
      });
    }
  }
}