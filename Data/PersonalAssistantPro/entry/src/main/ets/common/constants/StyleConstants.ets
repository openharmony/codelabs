// entry/src/main/ets/common/constants/StyleConstants.ets

/**
 * 全局样式常量
 * 统一管理应用中的尺寸、颜色、字体、阴影和常用组件样式
 */
export class StyleConstants {

  // === 间距 Space ===
  static readonly SPACE_XS = 4;
  static readonly SPACE_SM = 8;
  static readonly SPACE_MD = 12;
  static readonly SPACE_LG = 16;
  static readonly SPACE_XL = 24;
  static readonly SPACE_XXL = 32;

  // === 字体大小 Font Size ===
  static readonly FONT_XS = 10;
  static readonly FONT_SM = 12;
  static readonly FONT_MD = 14; // 正文默认
  static readonly FONT_LG = 16; // 标题
  static readonly FONT_XL = 20; // 大标题
  static readonly FONT_XXL = 30; // 特大数字

  // === 字体粗细 Font Weight ===
  static readonly WEIGHT_NORMAL = FontWeight.Normal;
  static readonly WEIGHT_MEDIUM = FontWeight.Medium;
  static readonly WEIGHT_BOLD = FontWeight.Bold;

  // === 颜色 Colors (基础色板) ===
  static readonly COLOR_PRIMARY = '#0A59F7'; // 主色蓝
  static readonly COLOR_PRIMARY_LIGHT = '#E6F0FF'; // 浅蓝背景
  static readonly COLOR_TEXT_PRIMARY = '#182431'; // 主要文字
  static readonly COLOR_TEXT_SECONDARY = '#999999'; // 次要文字
  static readonly COLOR_TEXT_DISABLED = '#C6C6C6'; // 禁用文字
  static readonly COLOR_BACKGROUND = '#F1F3F5'; // 页面背景灰
  static readonly COLOR_WHITE = '#FFFFFF';
  static readonly COLOR_BORDER = '#E3E3E3'; // 边框颜色

  static readonly COLOR_RED = '#E84026';
  static readonly COLOR_GREEN = '#64BB5C';
  static readonly COLOR_ORANGE = '#ED6F21';

  // ✅ [关键修复] 补全语义化颜色别名，解决 EventDetailPage 报错
  static readonly COLOR_DANGER = '#E84026'; // 对应红色
  static readonly COLOR_SUCCESS = '#64BB5C'; // 对应绿色
  static readonly COLOR_WARNING = '#ED6F21'; // 对应橙色

  // === 圆角 Radius ===
  static readonly RADIUS_SM = 4;
  static readonly RADIUS_MD = 8;
  static readonly RADIUS_LG = 12;
  static readonly RADIUS_XL = 16;
  static readonly RADIUS_ROUND = 50;

  // === 阴影 Shadow ===
  static readonly SHADOW_COLOR = 'rgba(0,0,0,0.1)';
  static readonly SHADOW_OFFSET_X = 2;
  static readonly SHADOW_OFFSET_Y = 4;
  static readonly SHADOW_RADIUS = 10;

  // === 布局相关 ===
  static readonly FULL_WIDTH = '100%';
  static readonly FULL_HEIGHT = '100%';

  // === 常用组件尺寸 ===
  static readonly BUTTON_HEIGHT_SM = 28;
  static readonly BUTTON_HEIGHT_MD = 40;
  static readonly BUTTON_HEIGHT_LG = 48;

  static readonly INPUT_HEIGHT = 44;

  static readonly ICON_SIZE_SM = 16;
  static readonly ICON_SIZE_MD = 24;
  static readonly ICON_SIZE_LG = 32;

  // 动画时长
  static readonly ANIMATION_DURATION = 300;
}