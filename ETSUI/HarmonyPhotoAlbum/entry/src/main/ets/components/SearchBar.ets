/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 *
 */
/**
 * 搜索栏组件
 * 支持实时搜索和防抖优化
 */
@Component
export struct SearchBar {
  @Link searchText: string; // 搜索文本（双向绑定）
  private onSearch?: (text: string) => void; // 搜索回调
  private debounceTimer: number = -1; // 防抖定时器
  private debounceDelay: number = 300; // 防抖延迟（毫秒）

  build() {
    Row() {
      // 搜索图标
      Text('🔍')
        .fontSize(18)
        .margin({ left: 12, right: 8 })

      // 搜索输入框
      TextInput({
        placeholder: '搜索图片名称或标签',
        text: this.searchText
      })
        .layoutWeight(1)
        .height(40)
        .backgroundColor(Color.Transparent)
        .placeholderColor('#999999')
        .fontSize(14)
        .onChange((value: string) => {
          this.searchText = value;
          this.handleSearchWithDebounce(value);
        })

      // 清除按钮
      if (this.searchText.length > 0) {
        Text('✖')
          .fontSize(14)
          .fontColor('#666666')
          .margin({ right: 12 })
          .onClick(() => {
            this.searchText = '';
            this.onSearch?.('');
          })
      }
    }
    .width('100%')
    .height(48)
    .backgroundColor('#F5F5F5')
    .borderRadius(24)
    .margin({ top: 8, bottom: 8 })
  }

  /**
   * 防抖搜索处理
   */
  private handleSearchWithDebounce(text: string) {
    // 清除之前的定时器
    if (this.debounceTimer !== -1) {
      clearTimeout(this.debounceTimer);
    }

    // 设置新的定时器
    this.debounceTimer = setTimeout(() => {
      this.onSearch?.(text);
      this.debounceTimer = -1;
    }, this.debounceDelay);
  }
}