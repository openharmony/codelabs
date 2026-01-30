/**
 * 屏幕状态监控详情页
 * 功能：实时监听并展示设备的屏幕状态变化（亮屏、熄灭、锁定、解锁）。
 */

import BackContainer from '../../components/BackContainer';
import { DefaultScreenProvider, ScreenEvent } from '../../providers/ScreenProvider';
import { EventDB } from '../../database/EventDB';
import { EventLog, EventType } from '../../model/EventLog';
import { ScreenPayload } from '../../services/monitors/ScreenMonitor';

@Entry
@Component
struct ScreenPage {
  /**
   * @State 状态变量
   * currentStatus: 用于顶部卡片的实时文字反馈
   * historyLogs: 存储从数据库加载的历史事件列表，驱动 List 组件渲染
   */
  @State currentStatus: string = "检测中...";
  @State historyLogs: EventLog[] = [];
  private screenProvider: DefaultScreenProvider = new DefaultScreenProvider();

  /**
   * 组件生命周期：即将显示
   * 1. 初始化数据库连接。
   * 2. 注册屏幕事件回调：当系统发出屏幕相关广播时，触发 UI 刷新与历史记录重载。
   * 3. 初始加载：获取当前已有的历史数据。
   */
  async aboutToAppear() {
    // 步骤1：确保数据库处于 Ready 状态
    await EventDB.init(getContext(this));

    // 步骤2：订阅实时事件流
    await this.screenProvider.subscribe((event: ScreenEvent) => {
      /**
       * 响应式更新策略：
       * 1. 更新当前状态文字（映射逻辑见 updateStatusText）。
       * 2. 重新拉取数据库记录，确保列表同步显示最新捕获的事件。
       */
      this.updateStatusText(event);
      this.refreshHistory();
    });

    // 步骤3：预加载历史数据
    this.refreshHistory();
  }

  /**
   * 组件生命周期：即将销毁
   * 核心操作：取消订阅，防止内存泄漏及后台无效计算。
   */
  async aboutToDisappear() {
    await this.screenProvider.unsubscribe();
  }

  /**
   * 状态映射函数
   * 将底层的 ScreenEvent 枚举值转换为用户友好的中文描述。
   * @param event - 原始屏幕事件类型
   */
  updateStatusText(event: ScreenEvent) {
    const map: Record<string, string> = {
      'SCREEN_ON': '屏幕已点亮',
      'SCREEN_OFF': '屏幕已熄灭',
      'LOCKED': '设备已锁定',
      'UNLOCKED': '设备已解锁'
    };
    this.currentStatus = map[event] || event;
  }

  /**
   * 数据库交互逻辑
   * 从 EventDB 分页查询最近 20 条屏幕相关的记录。
   */
  async refreshHistory() {
    if (EventDB.isReady()) {
      const logs = await EventDB.query({
        type: EventType.SCREEN,
        limit: 20
      });
      this.historyLogs = logs;
    }
  }

  build() {
    Column() {
      // 通用返回标题栏
      BackContainer({ header: "屏幕监控" })

      // ========== 顶部：动态屏幕状态卡片 (可视化区域) ==========
      Column({ space: 15 }) {
        Stack() {
          /**
           * 拟物化外框设计
           * 背景色随屏幕状态切换：熄灭时为深黑，点亮时为浅蓝。
           */
          Rect({ width: 140, height: 240 })
            .fill(this.currentStatus.includes('熄灭') ? '#1A1A1A' : '#F0F9FF')
            .stroke('#333')
            .strokeWidth(5)
            .radius(20)

          // 状态内容展示层
          Column({ space: 10 }) {
            // 根据关键词动态展示系统图标
            if (this.currentStatus.includes('锁定')) {
              SymbolGlyph($r('sys.symbol.lock_fill'))
                .fontSize(40)
            } else if (this.currentStatus.includes('点亮') || this.currentStatus.includes('解锁')) {
              SymbolGlyph($r('sys.symbol.sun_max_fill'))
                .fontSize(40)
            }

            Text(this.currentStatus)
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .fontColor(this.currentStatus.includes('熄灭') ? '#666' : '#333')
          }
        }
        .margin({ top: 20 })
      }
      .width('100%')
      .padding(20)

      // ========== 底部：事件记录时间线 (历史日志区域) ==========
      Column() {
        Row() {
          Text("活动日志")
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
          Blank()
          Text("实时捕获").fontSize(12).fontColor('#999')
        }
        .width('100%')
        .padding(20)

        // 历史事件列表
        List() {
          ForEach(this.historyLogs, (log: EventLog) => {
            ListItem() {
              Row() {
                // 左侧时间线视觉装饰：圆点 + 垂直线
                Column() {
                  Circle({ width: 12, height: 12 })
                    .fill(this.getEventColor(log.message))
                  Line().width(2).height(40).backgroundColor('#F0F0F0')
                }.width(30)

                // 中间事件详情
                Column() {
                  Text(log.message)
                    .fontSize(15)
                    .fontWeight(FontWeight.Medium)

                  /**
                   * Payload 解析逻辑
                   * 1. 通过 'as' 关键字进行类型断言，安全访问 ScreenPayload 接口。
                   * 2. 展示底层的原始事件名称，方便技术人员调试。
                   */
                  if (log.payload) {
                    Text(`事件类型: ${(log.payload as ScreenPayload).event}`)
                      .fontSize(11)
                      .fontColor('#BBB')
                      .margin({ top: 2 })
                  }
                }
                .alignItems(HorizontalAlign.Start)
                .layoutWeight(1)

                // 右侧时间标记（精确到秒）
                Text(new Date(log.timestamp).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' }))
                  .fontSize(12)
                  .fontColor('#999')
              }
              .width('100%')
              .padding({ left: 20, right: 20 })
            }
          })
        }
        .layoutWeight(1)
      }
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius({ topLeft: 30, topRight: 30 }) // 顶部圆角营造容器感
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }

  /**
   * 语义化颜色映射
   * 根据消息内容返回对应的主题色，增强日志的可读性。
   */
  getEventColor(msg: string): string {
    if (msg.includes('锁定')) return '#FF9500'; // 橙色表示安全锁定
    if (msg.includes('熄灭')) return '#666666'; // 灰色表示进入休眠
    return '#007DFF'; // 蓝色表示常规操作
  }
}