/**
 * MemoTime 应用常量定义
 */

/**
 * 日历视图类型
 */
export enum CalendarViewType {
  DAY = 'day',
  WEEK = 'week',
  MONTH = 'month',
  YEAR = 'year'
}

/**
 * 提醒时机类型
 */
export enum ReminderTiming {
  ON_TIME = 0,           // 准时
  MINUTES_5 = 5,         // 提前5分钟
  MINUTES_10 = 10,       // 提前10分钟
  MINUTES_15 = 15,       // 提前15分钟
  MINUTES_30 = 30,       // 提前30分钟
  HOURS_1 = 60,          // 提前1小时
  HOURS_2 = 120,         // 提前2小时
  DAYS_1 = 1440,         // 提前1天
  DAYS_2 = 2880,         // 提前2天
  WEEKS_1 = 10080        // 提前1周
}

/**
 * 提醒方式
 */
export enum ReminderType {
  NOTIFICATION = 'notification',  // 系统通知
  POPUP = 'popup',                // App内弹窗
  EMAIL = 'email',                // 邮件
  SMS = 'sms'                     // 短信
}

/**
 * 重复周期类型
 */
export enum RepeatType {
  NONE = 'none',           // 不重复
  DAILY = 'daily',         // 每日
  WEEKLY = 'weekly',       // 每周
  MONTHLY = 'monthly',     // 每月
  YEARLY = 'yearly',       // 每年
  CUSTOM = 'custom'        // 自定义
}

/**
 * 标签预设颜色
 */
export const TAG_COLORS: string[] = [
  '#FF6B6B',  // 红色
  '#4ECDC4',  // 青色
  '#45B7D1',  // 蓝色
  '#96CEB4',  // 绿色
  '#FFEAA7',  // 黄色
  '#DDA0DD',  // 紫色
  '#FF8C42',  // 橙色
  '#95E1D3',  // 薄荷绿
  '#F38181',  // 珊瑚色
  '#AA96DA'   // 淡紫色
]

/**
 * 日程分类预设
 */
export const SCHEDULE_CATEGORIES: string[] = [
  '工作',
  '学习',
  '会议',
  '生日',
  '生活',
  '运动',
  '旅行',
  '其他'
]

/**
 * 星期名称
 */
export const WEEK_DAYS: string[] = ['日', '一', '二', '三', '四', '五', '六']

/**
 * 月份名称
 */
export const MONTH_NAMES: string[] = [
  '一月', '二月', '三月', '四月', '五月', '六月',
  '七月', '八月', '九月', '十月', '十一月', '十二月'
]

/**
 * 应用常量
 */
export class AppConstants {
  // 数据库相关
  static readonly DB_NAME: string = 'MemoTimeDB'
  static readonly DB_VERSION: number = 1

  // 表名
  static readonly TABLE_SCHEDULE: string = 'schedules'
  static readonly TABLE_REMINDER: string = 'reminders'
  static readonly TABLE_RECORDING: string = 'recordings'
  static readonly TABLE_TAG: string = 'tags'

  // 存储键
  static readonly PREF_STORE_NAME: string = 'MemoTimePreferences'
  static readonly KEY_VIEW_TYPE: string = 'calendar_view_type'
  static readonly KEY_FIRST_DAY_OF_WEEK: string = 'first_day_of_week'
  static readonly KEY_THEME_MODE: string = 'theme_mode'
  static readonly KEY_USER_ID: string = 'user_id'

  // 页面路由
  static readonly PAGE_INDEX: string = 'pages/Index'
  static readonly PAGE_SCHEDULE_DETAIL: string = 'pages/ScheduleDetailPage'
  static readonly PAGE_SCHEDULE_EDIT: string = 'pages/ScheduleEditPage'
  static readonly PAGE_RECORDING: string = 'pages/RecordingPage'
  static readonly PAGE_SETTINGS: string = 'pages/SettingsPage'

  // 动画时长
  static readonly ANIMATION_DURATION_SHORT: number = 150
  static readonly ANIMATION_DURATION_NORMAL: number = 300
  static readonly ANIMATION_DURATION_LONG: number = 500

  // 布局断点
  static readonly BREAKPOINT_SM: number = 320   // 手机
  static readonly BREAKPOINT_MD: number = 520   // 折叠屏/小平板
  static readonly BREAKPOINT_LG: number = 840   // 平板/2in1
}

/**
 * 设备类型
 */
export enum DeviceType {
  PHONE = 'phone',
  TABLET = 'tablet',
  TWO_IN_ONE = '2in1'
}

/**
 * 外部应用类型（用于智能跳转）
 */
export enum ExternalAppType {
  EMAIL = 'email',
  MEETING = 'meeting',
  BROWSER = 'browser',
  DOCUMENT = 'document',
  CUSTOM = 'custom'
}
