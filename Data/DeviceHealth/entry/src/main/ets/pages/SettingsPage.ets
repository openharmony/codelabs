/**
 * 设置页面
 * 展示所有配置项并支持实时修改和持久化
 */

import { SettingGroup, SettingDivider } from '../components/SettingsComponents';
import { PreferenceManager } from '../manager/PreferenceManager';
import { SettingsKeys, DEFAULT_SETTINGS } from '../model/UserSettings';

@Entry
@Component
struct SettingsPage {
  /**
   * @StorageLink 装饰器
   * 核心特性：
   * 1. 双向绑定 AppStorage 中的数据
   * 2. 数据变化时自动刷新 UI
   * 3. UI 修改时自动更新 AppStorage
   *
   * 注意：这里只是同步内存状态，还需要手动调用 PreferenceManager 持久化
   */
  @StorageLink(SettingsKeys.BATTERY_THRESHOLD) batteryThreshold: number = 20;
  @StorageLink(SettingsKeys.STORAGE_THRESHOLD) storageThreshold: number = 15;
  @StorageLink(SettingsKeys.NETWORK_MONITOR) isNetworkMonitorOn: boolean = true;
  @StorageLink(SettingsKeys.LOG_ENCRYPTION) isLogEncryptionOn: boolean = false;
  @StorageLink(SettingsKeys.NOTIFICATIONS) enableNotifications: boolean = true;

  // 获取管理器实例
  private preferenceManager = PreferenceManager.getInstance();

  build() {
    Column() {
      // 顶部标题栏
      Row() {
        Text('设置')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
      }
      .width('100%')
      .height(56)
      .padding({ left: 16, right: 16 })
      .backgroundColor(Color.White)

      // 可滚动内容区域
      Scroll() {
        Column() {
          // ========== 监控配置 ==========
          SettingGroup({ title: '监控配置' }) {
            /**
             * 电量阈值设置
             * 解决方案：直接在父组件中完整构建 UI，不使用 SettingItem 组件
             * 这样可以确保所有引用 @StorageLink 的部分都在同一个组件中
             */
            Row() {
              // 左侧文字区域
              Column() {
                // 主标题
                Text('电量预警阈值')
                  .fontSize(16)
                  .fontColor('#333333')
                  .fontWeight(FontWeight.Medium)

                // 副标题 - 会实时更新
                Text(`当前设置: ${this.batteryThreshold}%`)
                  .fontSize(12)
                  .fontColor('#999999')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)

              // 右侧 Slider 控件
              Row() {
                Slider({
                  value: this.batteryThreshold,
                  min: 5,
                  max: 50,
                  step: 5,
                  style: SliderStyle.InSet
                })
                  .width(150)
                  .blockColor('#007DFF')
                  .trackColor('#E0E0E0')
                  .selectedColor('#007DFF')
                  .showSteps(true)
                  /**
                   * onChange 回调
                   * 1. 更新本地状态（通过 @StorageLink 自动同步到 AppStorage）
                   * 2. 调用 PreferenceManager 持久化到磁盘
                   */
                  .onChange((value: number) => {
                    this.batteryThreshold = value;
                    this.preferenceManager.saveValue(SettingsKeys.BATTERY_THRESHOLD, value);
                  })
              }
            }
            .width('100%')
            .padding({ left: 12, right: 12, top: 16, bottom: 16 })
            .justifyContent(FlexAlign.SpaceBetween)

            SettingDivider()

            /**
             * 存储阈值设置
             * 同样直接构建，确保响应式更新
             */
            Row() {
              // 左侧文字区域
              Column() {
                // 主标题
                Text('存储预警阈值')
                  .fontSize(16)
                  .fontColor('#333333')
                  .fontWeight(FontWeight.Medium)

                // 副标题 - 会实时更新
                Text(`当前设置: ${this.storageThreshold}%`)
                  .fontSize(12)
                  .fontColor('#999999')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)

              // 右侧 Slider 控件
              Row() {
                Slider({
                  value: this.storageThreshold,
                  min: 5,
                  max: 50,
                  step: 5,
                  style: SliderStyle.InSet
                })
                  .width(150)
                  .blockColor('#007DFF')
                  .trackColor('#E0E0E0')
                  .selectedColor('#007DFF')
                  .showSteps(true)
                  .onChange((value: number) => {
                    this.storageThreshold = value;
                    this.preferenceManager.saveValue(SettingsKeys.STORAGE_THRESHOLD, value);
                  })
              }
            }
            .width('100%')
            .padding({ left: 12, right: 12, top: 16, bottom: 16 })
            .justifyContent(FlexAlign.SpaceBetween)

            SettingDivider()

            // 网络监控开关（静态文本，可以继续使用组件）
            Row() {
              Column() {
                Text('网络监控')
                  .fontSize(16)
                  .fontColor('#333333')
                  .fontWeight(FontWeight.Medium)

                Text('实时监控网络连接状态')
                  .fontSize(12)
                  .fontColor('#999999')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)

              Row() {
                Toggle({ type: ToggleType.Switch, isOn: this.isNetworkMonitorOn })
                  .selectedColor('#007DFF')
                  .onChange((isOn: boolean) => {
                    this.isNetworkMonitorOn = isOn;
                    this.preferenceManager.saveValue(SettingsKeys.NETWORK_MONITOR, isOn);
                  })
              }
            }
            .width('100%')
            .padding({ left: 12, right: 12, top: 16, bottom: 16 })
            .justifyContent(FlexAlign.SpaceBetween)
          }

          // ========== 安全设置 ==========
          SettingGroup({ title: '安全设置' }) {
            Row() {
              Column() {
                Text('日志加密')
                  .fontSize(16)
                  .fontColor('#333333')
                  .fontWeight(FontWeight.Medium)

                Text('加密存储敏感日志信息')
                  .fontSize(12)
                  .fontColor('#999999')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)

              Row() {
                Toggle({ type: ToggleType.Switch, isOn: this.isLogEncryptionOn })
                  .selectedColor('#007DFF')
                  .onChange((isOn: boolean) => {
                    this.isLogEncryptionOn = isOn;
                    this.preferenceManager.saveValue(SettingsKeys.LOG_ENCRYPTION, isOn);
                  })
              }
            }
            .width('100%')
            .padding({ left: 12, right: 12, top: 16, bottom: 16 })
            .justifyContent(FlexAlign.SpaceBetween)
          }

          // ========== 系统设置 ==========
          SettingGroup({ title: '系统设置' }) {
            Row() {
              Column() {
                Text('通知提醒')
                  .fontSize(16)
                  .fontColor('#333333')
                  .fontWeight(FontWeight.Medium)

                Text('接收设备健康状态通知')
                  .fontSize(12)
                  .fontColor('#999999')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)

              Row() {
                Toggle({ type: ToggleType.Switch, isOn: this.enableNotifications })
                  .selectedColor('#007DFF')
                  .onChange((isOn: boolean) => {
                    this.enableNotifications = isOn;
                    this.preferenceManager.saveValue(SettingsKeys.NOTIFICATIONS, isOn);
                  })
              }
            }
            .width('100%')
            .padding({ left: 12, right: 12, top: 16, bottom: 16 })
            .justifyContent(FlexAlign.SpaceBetween)
          }

          // ========== 重置按钮 ==========
          Button('恢复默认设置')
            .width('90%')
            .height(44)
            .fontSize(16)
            .fontColor(Color.White)
            .backgroundColor('#FF6B6B')
            .borderRadius(8)
            .margin({ top: 32, bottom: 32 })
            .onClick(() => {
              /**
               * 重置操作（调试版）
               * 添加详细日志来定位问题
               */
              console.info('[SettingsPage] 按钮被点击');

              // 直接更新 AppStorage，不依赖 PreferenceManager
              AppStorage.set(SettingsKeys.BATTERY_THRESHOLD, DEFAULT_SETTINGS.batteryThreshold);
              AppStorage.set(SettingsKeys.STORAGE_THRESHOLD, DEFAULT_SETTINGS.storageThreshold);
              AppStorage.set(SettingsKeys.NETWORK_MONITOR, DEFAULT_SETTINGS.isNetworkMonitorOn);
              AppStorage.set(SettingsKeys.LOG_ENCRYPTION, DEFAULT_SETTINGS.isLogEncryptionOn);
              AppStorage.set(SettingsKeys.NOTIFICATIONS, DEFAULT_SETTINGS.enableNotifications);

              console.info('[SettingsPage] AppStorage 已更新');

              // 异步持久化到 Preferences
              this.preferenceManager.resetToDefaults()
                .then(() => {
                  console.info('[SettingsPage] 持久化完成');
                })
                .catch((err: Error) => {
                  console.error('[SettingsPage] 持久化失败:', err.message);
                });
            })
        }
        .padding({ left: 16, right: 16, top: 16, bottom: 16 })
      }
      .layoutWeight(1)
      .backgroundColor('#F5F5F5')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}