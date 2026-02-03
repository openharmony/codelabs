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

// @Component
// export struct SettingGroup {
//   ...
// }

/**
 * 设置中心主页面
 * 功能：作为应用入口，聚合展示各项监控指标（电池、网络、存储等）的摘要信息。
 */

import { BusinessError } from '@kit.BasicServicesKit';
import BatteryMini from '../components/Event/BatteryMini';
import { DefaultBatteryProvider } from '../providers/BatteryProvider';

@Entry
@Component
struct MainPage {
  @State globalBattery: number = 30;

  // 硬件能力提供者：负责直接与系统底层 API 通信获取数据
  private batteryProvider: DefaultBatteryProvider = new DefaultBatteryProvider();

  /**
   * 组件生命周期回调：即将显示时触发
   */
  async aboutToAppear() {
    // 步骤1：获取初始电量快照
    const currentSoc = await this.batteryProvider.getSOC();
    if (currentSoc !== null) {
      this.globalBattery = currentSoc;
    }

    // 步骤2：注册持续监听回调
    await this.batteryProvider.subscribe((newSoc: number) => {
      this.globalBattery = newSoc;
    });
  }

  build() {
    Column() {
      // ========== 页面大标题 ==========
      Text('设备健康检测')
        .fontSize(30)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .padding({ left: 24, top: 40, bottom: 20 })

      // ========== 可滚动卡片列表区域 ==========
      Scroll() {
        Column({ space: 12 }) {
          /**
           * 设置项卡片渲染
           * 传入 title 用于图标与文案匹配，url 用于路由跳转
           */
          this.SettingsCard('电池', 'pages/eventpages/BatteryPage')
          this.SettingsCard('网络', 'pages/eventpages/NetworkPage')
          this.SettingsCard('存储', 'pages/eventpages/StoragePage')
          this.SettingsCard('屏幕', 'pages/eventpages/ScreenPage')
          this.SettingsCard('设置', 'pages/SettingsPage')
        }
        .padding({ left: 16, right: 16 })
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5') // 采用系统标准浅灰色背景
  }

  /**
   * @Builder 自定义构建函数：抽离卡片通用 UI 结构
   * @param title - 模块名称（用于匹配图标与颜色）
   * @param url - 跳转的目标页面路径
   */
  @Builder SettingsCard(title: string, url: string) {
    Row() {
      // --- 左侧：图标视觉区域 ---
      Stack() {
        // 图标背景光晕
        Circle({ width: 40, height: 40 })
          .fill(this.getIconBgColor(title))
          .opacity(0.1)

        // 核心图标显示
        Image(this.getIconResource(title))
          .width(22)
          .height(22)
          .objectFit(ImageFit.Contain)
      }
      .margin({ right: 16 })

      // --- 中间：标题文本区域 ---
      Text(title)
        .fontSize(16)
        .fontColor('#182431')
        .fontWeight(FontWeight.Medium)

      // 弹性占位，将右侧组件推至末端
      Blank()

      // --- 右侧：交互与状态反馈 ---
      if (title == "电池") {
        /**
         * 电池特化组件：展示微缩动态电池图标
         * 关联 @State globalBattery 实现父子组件联动
         */
        BatteryMini({ batteryLevel: this.globalBattery })
      } else {
        // 统一导航箭头，增强用户点击指引
        SymbolGlyph($r('sys.symbol.chevron_right'))
          .fontSize(18)
      }
    }
    .width('100%')
    .height(72)
    .padding({ left: 16, right: 16 })
    .backgroundColor(Color.White)
    .borderRadius(20)
    .shadow({ radius: 4, color: '#0A000000', offsetY: 2 }) // 添加轻量级投影增强层级感
    .onClick(() => {
      /**
       * 路由跳转逻辑
       */
      this.getUIContext().getRouter().pushUrl({ url: url }).catch((err: BusinessError) => {
        console.error(`[MainPage] Jump to ${url} failed: ${err.message}`)
      })
    })
  }

  /**
   * 图标资源映射
   * 维护建议：将新增图标存入 resources/base/media 后在此处添加对应的 case
   */
  getIconResource(title: string): Resource {
    switch (title) {
      case '电池':
        return $r('app.media.ic_battery');
      case '网络':
        return $r('app.media.ic_network');
      case '存储':
        return $r('app.media.ic_storage');
      case '屏幕':
        return $r('app.media.ic_screen');
      default:
        return $r('app.media.startIcon');
    }
  }

  /**
   * 视觉主题颜色映射
   * 对应不同功能的语义化颜色
   */
  getIconBgColor(title: string): string {
    const colors: Record<string, string> = {
      '电池': '#32CD32',
      '网络': '#007DFF',
      '存储': '#F7B500',
      '屏幕': '#FF5B5B'
    }
    return colors[title] || '#007DFF'
  }
}