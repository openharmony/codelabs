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
 */

import Constants from '../common/constants/Constants';

/**
 * 分类筛选组件
 * 支持多分类选择和全部选项
 */
@Component
export struct CategoryFilter {
  @Link selectedCategory: string; // 当前选中的分类（双向绑定）
  @State categories: string[] = []; // 分类列表
  private onCategoryChange?: (category: string) => void; // 分类切换回调

  aboutToAppear() {
    // 初始化分类列表：全部 + 预定义分类
    this.categories = [Constants.ALL_CATEGORY, ...Constants.CATEGORIES];
  }

  build() {
    Column() {
      // 标题
      Text('分类')
        .fontSize(14)
        .fontColor('#999999')
        .margin({ left: 16, top: 8, bottom: 8 })
        .alignSelf(ItemAlign.Start)

      // 分类列表（横向滚动）
      Scroll() {
        Row() {
          ForEach(this.categories, (category: string) => {
            this.CategoryItem(category);
          })
        }
        .padding({ left: 12, right: 12 })
      }
      .scrollable(ScrollDirection.Horizontal)
      .scrollBar(BarState.Off)
      .width('100%')
    }
    .width('100%')
    .alignItems(HorizontalAlign.Start)
  }

  /**
   * 分类选项
   */
  @Builder
  CategoryItem(category: string) {
    Text(category)
      .fontSize(14)
      .fontColor(this.selectedCategory === category ? Color.White : '#333333')
      .padding({ left: 16, right: 16, top: 8, bottom: 8 })
      .margin({ left: 4, right: 4 })
      .backgroundColor(this.selectedCategory === category ? '#007DFF' : '#E8E8E8')
      .borderRadius(16)
      .onClick(() => {
        this.selectedCategory = category;
        this.onCategoryChange?.(category);
      })
      .animation({
        duration: 200,
        curve: Curve.EaseInOut
      })
  }
}