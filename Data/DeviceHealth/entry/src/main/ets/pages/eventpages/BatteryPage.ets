
/**
 * 电池状态监控详情页
 * 功能：展示当前设备的精确电量百分比，并列出由 BatteryMonitor 捕获的电量变化事件记录。
 */

import BackContainer from '../../components/BackContainer';
import { DefaultBatteryProvider } from '../../providers/BatteryProvider';
import { EventDB } from '../../database/EventDB';
import { EventLog, EventType, EventLevel } from '../../model/EventLog';
import { BatteryPayload } from '../../services/monitors/BatteryMonitor';

@Entry
@Component
struct BatteryPage {
  /**
   * @State 状态变量
   * batteryLevel: 实时电量值，驱动页面顶部的数字和图标显示。
   * historyLogs: 电量事件日志列表，展示电量波动及低电量预警历史。
   */
  @State batteryLevel: number = 0;
  @State historyLogs: EventLog[] = [];

  // 电池能力提供者：直接对接系统 batteryInfo 接口
  private batteryProvider = new DefaultBatteryProvider();

  /**
   * 组件生命周期：即将显示
   * 1. 确保数据库单例已初始化。
   * 2. 执行首屏电量数据拉取。
   * 3. 开启订阅模式：监听电量变化并同步刷新历史日志列表。
   */
  async aboutToAppear() {
    // 步骤1：初始化数据库
    await EventDB.init(getContext(this));

    // 步骤2：获取即时电量，避免初始化时的“0%”闪烁
    const currentSoc = await this.batteryProvider.getSOC();
    if (currentSoc !== null) {
      this.batteryLevel = currentSoc;
    }

    // 步骤3：注册持续监听回调
    await this.batteryProvider.subscribe((newSoc: number) => {
      this.batteryLevel = newSoc;
      /**
       * 自动同步：
       * 每当电量发生变化，由后台 Monitor 存入数据库后，
       * 此处触发 refreshHistory 以确保用户看到最新的变化日志。
       */
      this.refreshHistory();
    });

    // 初始加载历史日志
    this.refreshHistory();
  }

  /**
   * 组件生命周期：即将销毁
   * 必须调用 unsubscribe 以清除 setInterval 定时器，防止后台内存溢出。
   */
  async aboutToDisappear() {
    await this.batteryProvider.unsubscribe();
  }

  /**
   * 历史记录查询
   * 从 EventDB 调取最近 20 条电量相关日志（包含 INFO 和 WARN 级别）。
   */
  async refreshHistory() {
    if (EventDB.isReady()) {
      const logs = await EventDB.query({
        type: EventType.BATTERY,
        limit: 20
      });
      this.historyLogs = logs;
    }
  }

  build() {
    Column() {
      // 通用返回标题栏
      BackContainer({ header: "电池管理" })

      // ========== 顶部：实时电量可视化卡片 ==========
      Column({ space: 10 }) {
        // 动态电池图标展示区
        Stack() {
          // 电池轮廓
          Rect({ width: 100, height: 50 })
            .fill(Color.Transparent)
            .stroke('#333')
            .strokeWidth(3)
            .radius(8)

          // 内部动态电量填充
          Rect({ width: (this.batteryLevel * 0.92), height: 42 }) // 根据百分比计算宽度
            .fill(this.batteryLevel <= 20 ? '#FF4500' : '#32CD32') // 低电量变色预警
            .radius(4)
            .margin({ left: 4 })
            .animation({ duration: 500, curve: Curve.EaseInOut })
        }
        .margin({ top: 30 })

        // 电量百分比文字
        Text(`${Math.floor(this.batteryLevel)}%`)
          .fontSize(40)
          .fontWeight(FontWeight.Bold)
          .fontColor('#182431')

        Text(this.batteryLevel <= 20 ? '电池电量偏低' : '电池状态良好')
          .fontSize(14)
          .fontColor(this.batteryLevel <= 20 ? '#FF4500' : '#999')
          .margin({ bottom: 20 })
      }
      .width('100%')
      .backgroundColor(Color.White)
      .margin({ bottom: 10 })

      // ========== 底部：电量监控日志 (历史区域) ==========
      Column() {
        Row() {
          Text("监控日志")
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
          Blank()
          Text("实时捕获").fontSize(12).fontColor('#999')
        }
        .width('100%')
        .padding(20)

        List() {
          ForEach(this.historyLogs, (log: EventLog) => {
            ListItem() {
              Row() {
                // 状态指示器：WARN 级别显示红色，INFO 显示绿色
                Circle({ width: 10, height: 10 })
                  .fill(log.level === EventLevel.WARN ? '#FF4500' : '#32CD32')
                  .margin({ right: 15 })

                Column() {
                  Text(log.message)
                    .fontSize(15)
                    .fontWeight(FontWeight.Medium)

                  // 解析 Payload 详情：展示触发记录时的具体 SOC
                  if (log.payload) {
                    Text(`记录值: ${(log.payload as BatteryPayload).soc}%`)
                      .fontSize(12)
                      .fontColor('#BBB')
                      .margin({ top: 2 })
                  }
                }
                .alignItems(HorizontalAlign.Start)
                .layoutWeight(1)

                // 格式化时间戳
                Text(new Date(log.timestamp).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }))
                  .fontSize(12)
                  .fontColor('#999')
              }
              .width('100%')
              .padding({ left: 20, right: 20, top: 12, bottom: 12 })
            }
          })
        }
        .divider({ strokeWidth: 0.5, color: '#F0F0F0', startMargin: 45, endMargin: 20 })
        .layoutWeight(1)
      }
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius({ topLeft: 30, topRight: 30 })
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}