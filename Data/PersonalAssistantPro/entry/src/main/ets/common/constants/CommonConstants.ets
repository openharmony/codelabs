// src/main/ets/common/constants/CommonConstants.ets
/**
 * 全局通用常量定义
 * 包含数据库配置、路由名称、日志标签等
 */
export class CommonConstants {
  // === 数据库配置 ===
  /** 数据库文件名 */
  static readonly RDB_NAME = 'PersonalAssistant.db';
  /** 数据库安全等级 */
  static readonly RDB_SECURITY_LEVEL = 1;
  /** 联系人表名 */
  static readonly TABLE_CONTACT = 'contact';
  /** 日程表名 */
  static readonly TABLE_EVENT = 'event';

  // === 路由名称 (Router Names) ===
  static readonly PAGE_MAIN = 'MainPage';
  static readonly PAGE_CONTACT_LIST = 'ContactListPage';
  static readonly PAGE_CONTACT_DETAIL = 'ContactDetailPage';
  static readonly PAGE_EVENT_LIST = 'EventListPage';
  static readonly PAGE_EVENT_EDIT = 'EventEditPage';
  static readonly PAGE_MINE = 'MinePage';
  static readonly PAGE_SETTINGS = 'SettingsPage';

  // === 首选项 Key (Preferences Keys) ===
  static readonly PREF_NAME = 'app_preferences';
  static readonly KEY_IS_BIOMETRIC_ENABLED = 'is_biometric_enabled';
  static readonly KEY_USER_NAME = 'key_user_name';
  static readonly KEY_THEME_MODE = 'key_theme_mode';

  // === 日志配置 ===
  static readonly LOG_DOMAIN = 0x0000;
  static readonly LOG_TAG_PREFIX = 'PA_Pro_';

  // === 业务常量 ===
  /** 默认分页大小 */
  static readonly DEFAULT_PAGE_SIZE = 20;
  /** 最大重试次数 */
  static readonly MAX_RETRY_COUNT = 3;
  /** 搜索防抖时间 (ms) */
  static readonly SEARCH_DEBOUNCE_TIME = 300;
}