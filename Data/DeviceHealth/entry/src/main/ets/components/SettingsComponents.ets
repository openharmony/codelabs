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

/**
 * 设置页面通用 UI 组件
 * 提供可复用的卡片式布局组件
 */

/**
 * 设置页面通用 UI 组件
 * 提供可复用的卡片式布局组件
 */

/**
 * 设置分组组件
 * 用于将相关配置项组织在一起，提供卡片式背景
 */
@Component
export struct SettingGroup {
  /**
   * 分组标题
   * 必须提供默认值（即使是空字符串）
   */
  title: string = '';

  /**
   * @BuilderParam 装饰器
   * 允许父组件传入自定义的 UI 构建函数
   * 类似于 React 的 children 或 Vue 的 slot
   *
   * 注意：@BuilderParam 不需要默认值
   */
  @BuilderParam content: () => void = this.defaultContent;

  /**
   * 默认内容构建器
   * 当父组件未传入 content 时使用
   */
  @Builder
  defaultContent() {
    // 空实现
  }

  build() {
    Column() {
      // 分组标题
      Text(this.title)
        .fontSize(14)
        .fontColor('#999999')
        .margin({ left: 16, bottom: 8 })

      // 卡片容器
      Column() {
        // 渲染传入的内容
        this.content()
      }
      .backgroundColor(Color.White)
      .borderRadius(12)
      .padding(12)
    }
    .alignItems(HorizontalAlign.Start)
    .width('100%')
    .margin({ bottom: 16 })
  }
}

/**
 * 设置项组件
 * 标准的配置项布局：左侧文字 + 右侧控件
 */
@Component
export struct SettingItem {
  /**
   * 主标题
   * 必须提供默认值
   */
  title: string = '';

  /**
   * 副标题（灰色小字）
   * 必须提供默认值
   */
  subTitle: string = '';

  /**
   * 右侧插槽
   * 用于放置 Toggle、Slider 等交互控件
   */
  @BuilderParam endSlot: () => void = this.defaultEndSlot;

  /**
   * 默认右侧内容构建器
   * 当父组件未传入 endSlot 时使用
   */
  @Builder
  defaultEndSlot() {
    // 空实现
  }

  build() {
    Row() {
      // 左侧文字区域
      Column() {
        // 主标题
        Text(this.title)
          .fontSize(16)
          .fontColor('#333333')
          .fontWeight(FontWeight.Medium)

        // 副标题（如果存在）
        if (this.subTitle) {
          Text(this.subTitle)
            .fontSize(12)
            .fontColor('#999999')
            .margin({ top: 4 })
        }
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1) // 占据剩余空间

      // 右侧控件区域
      Row() {
        this.endSlot()
      }
    }
    .width('100%')
    .padding({ left: 12, right: 12, top: 16, bottom: 16 })
    .justifyContent(FlexAlign.SpaceBetween)
  }
}

/**
 * 分割线组件
 * 用于分隔不同的设置项
 */
@Component
export struct SettingDivider {
  build() {
    Divider()
      .color('#F0F0F0')
      .strokeWidth(1)
      .margin({ left: 12, right: 12 })
  }
}