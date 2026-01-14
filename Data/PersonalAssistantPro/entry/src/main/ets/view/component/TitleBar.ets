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
// src/main/ets/view/component/TitleBar.ets
import { StyleConstants } from '../../common/constants/StyleConstants';

/**
 * 通用标题栏组件
 * 支持返回按钮、标题显示、右侧菜单
 */
@Component
export struct TitleBar {
  /** 标题文本 */
  @Prop title: string | Resource = '';
  /** 是否显示返回按钮 */
  @Prop showBack: boolean = true;
  /** 点击返回的回调 */
  onBackClick?: () => void;
  /** 右侧自定义构建器 */
  @BuilderParam rightContent?: CustomBuilder;

  build() {
    Row() {
      // 1. 返回按钮区域
      if (this.showBack) {
        Row() {
          Image($r('app.media.ic_back'))
            .width(StyleConstants.ICON_SIZE_MD)
            .height(StyleConstants.ICON_SIZE_MD)
            .fillColor(StyleConstants.COLOR_TEXT_PRIMARY)
        }
        .width(48)
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .onClick(() => {
          if (this.onBackClick) {
            this.onBackClick();
          }
        })
      } else {
        // 占位，保持间距
        Blank().width(StyleConstants.SPACE_LG)
      }

      // 2. 标题区域
      Text(this.title)
        .fontSize(StyleConstants.FONT_SIZE_XL)
        .fontWeight(FontWeight.Bold)
        .fontColor(StyleConstants.COLOR_TEXT_PRIMARY)
        .layoutWeight(1) // 占据中间剩余空间
        .maxLines(1)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        .textAlign(this.showBack ? TextAlign.Start : TextAlign.Start)

      // 3. 右侧扩展区域
      if (this.rightContent) {
        Row() {
          this.rightContent()
        }
        .height('100%')
        .padding({ right: StyleConstants.SPACE_LG })
      }
    }
    .width('100%')
    .height(StyleConstants.HEIGHT_TITLE_BAR)
    .backgroundColor(StyleConstants.COLOR_CARD)
    .shadow({ radius: 2, color: '#1A000000', offsetY: 1 })
    .zIndex(100) // 确保在最上层
  }
}