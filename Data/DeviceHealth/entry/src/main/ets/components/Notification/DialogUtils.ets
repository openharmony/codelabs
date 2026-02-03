/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// DialogUtils.ets
export default class DialogUtils {
  /**
   * 弹出通用告警对话框
   * @param title 标题
   * @param message 内容
   * @param onConfirm 点击确定的回调（可选）
   */
  static showNotice(title: string | Resource, message: string | Resource, onConfirm?: () => void) {
    AlertDialog.show({
      title: title,
      message: message,
      alignment: DialogAlignment.Bottom,
      offset: { dx: 0, dy: -20 },
      primaryButton: {
        value: '取消',
        action: () => {
          console.info('Dialog: Cancelled');
        }
      },
      secondaryButton: {
        value: '确定',
        fontColor: '#007DFF',
        action: () => {
          if (onConfirm) {
            onConfirm();
          }
        }
      },
      autoCancel: true
    });
  }

  /**
   * 只有一个按钮的简单提示
   */
  static showAlert(title: string | Resource, message: string | Resource) {
    AlertDialog.show({
      title: title,
      message: message,
      alignment: DialogAlignment.Center,
      confirm: {
        value: '我知道了',
        action: () => {}
      }
    });
  }
}