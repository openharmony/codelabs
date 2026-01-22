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
// src/main/ets/view/component/CustomDialog.ets
import { StyleConstants } from '../../common/constants/StyleConstants';

/**
 * 通用自定义弹窗内容组件
 * * 用于配合 CustomDialogController 使用
 * 提供了统一的 App 风格弹窗样式：
 * - 标题栏
 * - 内容区域
 * - 底部按钮栏 (取消/确认)
 */
@CustomDialog
export struct CommonDialog {
  // === 控制器 (必须) ===
  controller: CustomDialogController;
  // === 属性 ===
  /** 弹窗标题 */
  // 📝 [修改] 使用 app.string.common_tip 替代 '提示'
  title: string | Resource = $r('app.string.common_tip');
  /** 弹窗内容 */
  message: string | Resource = '';
  /** 取消按钮文本 */
  // 📝 [修改] 使用 app.string.common_cancel 替代 '取消'
  cancelText: string | Resource = $r('app.string.common_cancel');
  /** 确认按钮文本 */
  // 📝 [修改] 使用 app.string.common_confirm 替代 '确定'
  confirmText: string | Resource = $r('app.string.common_confirm');
  // === 回调函数 ===
  /** 点击取消的回调 */
  onCancel?: () => void;
  /** 点击确认的回调 */
  onConfirm?: () => void;

  build() {
    Column() {
      // 1. 标题区域
      Text(this.title)
        .fontSize(StyleConstants.ICON_SIZE_LG)
        .fontWeight(FontWeight.Bold)
        .fontColor(StyleConstants.COLOR_TEXT_PRIMARY)
        .margin({ top: StyleConstants.SPACE_XL, bottom: StyleConstants.SPACE_MD })
        .textAlign(TextAlign.Center)
        .width('100%')

      // 2. 内容区域
      Text(this.message)
        .fontSize(StyleConstants.ICON_SIZE_MD)
        .fontColor(StyleConstants.COLOR_TEXT_SECONDARY)
        .margin({
          bottom: StyleConstants.SPACE_XL,
          left: StyleConstants.SPACE_LG,
          right: StyleConstants.SPACE_LG
        })
        .textAlign(TextAlign.Center)
        .lineHeight(22) // 增加行高，提升可读性

      // 3. 分割线
      Divider()
        .color(StyleConstants.COLOR_DIVIDER)
        .width('100%')
        .strokeWidth(1)

      // 4. 底部按钮区域
      Row() {
        // --- 取消按钮 ---
        Button() {
          Text(this.cancelText)
            .fontSize(StyleConstants.ICON_SIZE_LG)
            .fontColor(StyleConstants.COLOR_TEXT_SECONDARY)
        }
        .type(ButtonType.Normal)
        .backgroundColor(Color.Transparent)
        .layoutWeight(1)
        .height(50)
        .onClick(() => {
          if (this.controller) {
            this.controller.close();
          }
          if (this.onCancel) {
            this.onCancel();
          }
        })

        // 按钮中间的竖线
        Divider()
          .vertical(true)
          .height(30)
          .color(StyleConstants.COLOR_DIVIDER)

        // --- 确认按钮 ---
        Button() {
          Text(this.confirmText)
            .fontSize(StyleConstants.ICON_SIZE_LG)
            .fontWeight(FontWeight.Medium)
            .fontColor(StyleConstants.COLOR_PRIMARY)
        }
        .type(ButtonType.Normal)
        .backgroundColor(Color.Transparent)
        .layoutWeight(1)
        .height(50)
        .onClick(() => {
          if (this.controller) {
            this.controller.close();
          }
          if (this.onConfirm) {
            this.onConfirm();
          }
        })
      }
      .width('100%')
      .height(50)
    }
    .width('80%') // 弹窗宽度
    .backgroundColor(Color.White)
    .borderRadius(StyleConstants.RADIUS_LG)
    // 添加一点阴影效果
    .shadow({
      radius: 10,
      color: '#40000000',
      offsetY: 4
    })
  }
}