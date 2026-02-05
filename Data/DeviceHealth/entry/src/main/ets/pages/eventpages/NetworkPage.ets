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
 * 网络监控详情页
 * 功能：实时监测设备的连通性，并展示网络状态切换（如连接、断开）的历史日志。
 */

import BackContainer from '../../components/BackContainer';
import { DefaultNetworkProvider, NetworkStatus } from '../../providers/NetworkProvider';
import { EventDB } from '../../database/EventDB';
import { EventLog, EventType, EventLevel } from '../../model/EventLog';


@Entry
@Component
struct NetworkPage {
  /**
   * @State 状态变量
   * isConnected: 驱动顶部实时状态球的视觉表现（颜色、文字）
   * historyLogs: 驱动列表渲染，展示从 EventDB 获取的历史波动记录
   */
  @State isConnected: boolean = false;
  @State historyLogs: EventLog[] = [];

  // 网络能力提供者：封装了系统连接状态获取与订阅逻辑
  private netProvider: DefaultNetworkProvider = new DefaultNetworkProvider();

  /**
   * 组件生命周期：即将显示
   * 1. 初始化持久化数据库。
   * 2. 执行一次同步获取，确保首屏状态显示不延迟。
   * 3. 订阅 Provider：当网络状态切换时，同步更新 UI 并重载日志列表。
   */
  async aboutToAppear() {
    // 步骤1：数据库环境检查
    await EventDB.init(getContext(this));

    // 步骤2：初始快照获取
    const initialStatus = await this.netProvider.getStatus();
    this.isConnected = initialStatus.connected;

    // 步骤3：注册实时状态监听
    await this.netProvider.subscribe((s: NetworkStatus) => {
      this.isConnected = s.connected;
      /**
       * 联动机制：
       * 由于日志是由后台 Monitor 在感知状态变化后异步写入数据库的，
       * 故此处触发刷新以拉取最新的日志条目。
       */
      this.refreshHistory();
    });

    // 预加载历史记录
    this.refreshHistory();
  }

  /**
   * 组件生命周期：即将销毁
   * 核心操作：取消订阅，释放底层系统监听资源。
   */
  async aboutToDisappear() {
    await this.netProvider.unsubscribe();
  }

  /**
   * 数据库异步查询
   * 逻辑：提取最近 20 条网络相关的 EventLog。
   */
  async refreshHistory() {
    if (EventDB.isReady()) {
      const logs = await EventDB.query({
        type: EventType.NETWORK,
        limit: 20
      });
      this.historyLogs = logs;
    }
  }

  build() {
    Column() {
      // 通用返回容器
      BackContainer({ header: "网络监控" })

      // ========== 上部：实时状态可视化（响应式显示区域） ==========
      Column({ space: 15 }) {
        Stack() {
          // 动态呼吸光晕：根据连接状态切换主题色
          Circle({ width: 110, height: 110 })
            .fill(this.isConnected ? '#32CD32' : '#FF4500')
            .opacity(0.1)

          Circle({ width: 100, height: 100 })
            .fill(this.isConnected ? '#E8F5E9' : '#FFEBEE')

          // 核心语义化提示
          Text(this.isConnected ? "已连接" : "断开")
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.isConnected ? '#4CAF50' : '#F44336')
        }
        .margin({ top: 20 })

        Text(this.isConnected ? "网络环境正常" : "网络连接受阻")
          .fontSize(18)
          .fontWeight(FontWeight.Medium)

        // 诊断信息辅助说明
        Text(`检查周期：10秒 | 状态：${this.isConnected ? '稳定' : '异常'}`)
          .fontSize(12)
          .fontColor('#999999')
      }
      .width('100%')
      .padding(30)
      .backgroundColor(Color.White)
      .margin({ bottom: 10 })

      // ========== 下部：监控日志列表（历史波动区域） ==========
      Column() {
        Row() {
          Text("监控日志")
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
          Blank()
          Text("仅记录状态切换").fontSize(12).fontColor('#999')
        }
        .width('100%')
        .padding({ left: 20, right: 20, top: 15, bottom: 10 })

        List() {
          // 空状态占位处理
          if (this.historyLogs.length === 0) {
            ListItem() {
              Text("暂无网络波动记录")
                .width('100%')
                .textAlign(TextAlign.Center)
                .margin({ top: 50 })
                .fontColor('#CCCCCC')
            }
          }

          ForEach(this.historyLogs, (log: EventLog) => {
            ListItem() {
              Row() {
                // 左侧：多维度时间戳展示
                Column() {
                  Text(new Date(log.timestamp).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }))
                    .fontSize(14)
                    .fontWeight(FontWeight.Medium)
                  Text(new Date(log.timestamp).toLocaleDateString())
                    .fontSize(10)
                    .fontColor('#BBB')
                }
                .alignItems(HorizontalAlign.Start)
                .width('25%')

                // 中间：分级别状态语义化显示
                Row() {
                  Circle({ width: 8, height: 8 })
                    .fill(log.level === EventLevel.WARN ? '#F44336' : '#4CAF50') // WARN 红色警告，INFO 绿色正常
                    .margin({ right: 8 })

                  Text(log.message)
                    .fontSize(15)
                    .fontColor(log.level === EventLevel.WARN ? '#D32F2F' : '#388E3C')
                }
                .layoutWeight(1)

                /**
                 * 核心解析逻辑：
                 * 1. 将泛型 payload 显式断言为 NetworkPayload。
                 * 2. 安全读取 networkType（如 WiFi/Cellular）并以胶囊标签形式展示。
                 */
                if (log.payload) {
                  Text(log.payload.networkType || 'Unknown')
                    .fontSize(12)
                    .fontColor('#999')
                    .backgroundColor('#F0F0F0')
                    .padding({ left: 6, right: 6, top: 2, bottom: 2 })
                    .borderRadius(4)
                }
              }
              .width('100%')
              .padding({ top: 15, bottom: 15, left: 20, right: 20 })
            }
          })
        }
        .divider({ strokeWidth: 0.5, color: '#F0F0F0', startMargin: 20, endMargin: 20 })
        .width('100%')
        .layoutWeight(1)
      }
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius({ topLeft: 24, topRight: 24 })
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}