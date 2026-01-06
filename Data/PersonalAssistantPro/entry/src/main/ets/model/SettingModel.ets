import { PreferenceService } from '../services/PreferenceService';
import { Logger } from '../common/utils/Logger';

export class SettingModel {
  private logger = new Logger('SettingModel');
  private readonly KEY_BIOMETRIC_ENABLE = 'is_biometric_enabled';

  /**
   * 读取状态
   */
  async isBiometricEnabled(): Promise<boolean> {
    // 强制获取服务实例
    const service = PreferenceService.getInstance();

    // 读取值，默认为 false
    const value = await service.getValue<boolean>(this.KEY_BIOMETRIC_ENABLE, false);

    this.logger.info(`🔍 [Model] 读取生物认证状态: ${value}`);
    return value;
  }

  /**
   * 保存状态
   */
  async setBiometricEnabled(isEnabled: boolean): Promise<void> {
    this.logger.info(`💾 [Model] 正在保存状态: ${isEnabled}`);

    // 写入并强制刷新
    await PreferenceService.getInstance().putValue(this.KEY_BIOMETRIC_ENABLE, isEnabled);

    this.logger.info(`✅ [Model] 保存完成`);
  }
}