/**
 * 事件管理中心
 * 功能：
 * 1. 监控 AppStorage 中电量与存储空间的变化
 * 2. 内部硬编码预警阈值，并执行自动弹窗逻辑
 */
import common from '@ohos.app.ability.common';
import DialogUtils from '../components/Notification/DialogUtils';

export class EventHubManager {
  private static instance: EventHubManager;
  private context: common.UIAbilityContext | null = null;

  // 定义统一的事件名常量
  public static readonly EVENT_LOW_BATTERY = 'EVENT_LOW_BATTERY';
  public static readonly EVENT_LOW_STORAGE = 'EVENT_LOW_STORAGE';

  private constructor() {}

  public static getInstance(): EventHubManager {
    if (!EventHubManager.instance) {
      EventHubManager.instance = new EventHubManager();
    }
    return EventHubManager.instance;
  }

  // 初始化，在主页面的 aboutToAppear 调用
  public init(context: common.Context): void {
    this.context = context as common.UIAbilityContext;
  }

  // 开启全局监听：电量不足信号
  public subscribeBatteryWarning(): void {
    if (!this.context) return;

    this.context.eventHub.on(EventHubManager.EVENT_LOW_BATTERY, (soc: number) => {
      // 接收到信号后，直接调用之前封装好的弹窗
      if (soc < 20){
        DialogUtils.showNotice(
          '电量不足警告',
          `检测到系统电量已降至 ${soc}%，请及时充电。`,
          () => { console.info('用户点击确定，准备进入省电模式') }
        );
      }
    });
  }

  // 发送电量不足信号
  public emitBatteryWarning(soc: number): void {
    if (this.context) {
      this.context.eventHub.emit(EventHubManager.EVENT_LOW_BATTERY, soc);
    }
  }

  // 开启全局监听：存储空间不足信号
  public subscribeStorageWarning(): void {
    if (!this.context) return;

    this.context.eventHub.on(EventHubManager.EVENT_LOW_STORAGE, (freePercentage: number) => {
      // 接收到信号后，显示存储空间不足警告

      if (freePercentage < 20) {
        DialogUtils.showNotice(
          '存储空间不足警告',
          '剩余存储空间不足20%，请注意清理',
          () => {
            console.info('用户确认存储空间警告，准备跳转到清理页面');
            // 这里可以添加跳转到清理页面的逻辑
          }
        );
      }
    });
  }
  // 发送存储空间警告信号（简化版）
  public emitStorageWarning(freePercentage: number): void {
    if (this.context) {
      this.context.eventHub.emit(EventHubManager.EVENT_LOW_STORAGE, freePercentage);
    }
  }


  // 开启所有监听
  public subscribeAll(): void {
    this.subscribeBatteryWarning();
    this.subscribeStorageWarning();
  }
  // 取消监听，防止内存泄漏
  public unSubscribeAll(): void {
    if (this.context) {
      this.context.eventHub.off(EventHubManager.EVENT_LOW_BATTERY);
      this.context.eventHub.off(EventHubManager.EVENT_LOW_STORAGE);
    }
  }
}