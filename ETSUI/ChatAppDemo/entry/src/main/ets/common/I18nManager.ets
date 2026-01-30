import i18n from '@ohos.i18n';
import { BusinessError } from '@ohos.base';

export class I18nManager {
  /**
   * 切换语言逻辑
   * @param languageCode 'zh-Hans' 或 'en-US'
   */
  static switchLanguage(languageCode: string) {
    try {
      // 设置系统语言（这会影响 $r 的指向）
      i18n.System.setAppPreferredLanguage(languageCode);

      // 将当前语言持久化，以便下次启动时读取
      AppStorage.setOrCreate('currentLanguage', languageCode);

      console.info(`成功切换语言至: ${languageCode}`);
    } catch (error) {
      let err: BusinessError = error as BusinessError;
      console.error(`切换语言失败: ${err.code} ${err.message}`);
    }
  }

  /**
   * 初始化语言（在 EntryAbility 的 onCreate 中调用）
   */
  static initLanguage() {
    let lang = AppStorage.get<string>('currentLanguage') || 'zh-Hans';
    i18n.System.setAppPreferredLanguage(lang);
  }
}