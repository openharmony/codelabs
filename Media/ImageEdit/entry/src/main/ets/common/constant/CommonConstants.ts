/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

export class CommonConstants {
  /**
   * Title row width.
   */
  static readonly TITLE_ROW_WEIGHT: string = '50%';

  /**
   * Layout full screen.
   */
  static readonly LAYOUT_FULL_SCREEN: string = '100%';

  /**
   * Edit page height.
   */
  static readonly EDIT_PAGE_HEIGHT: string = '26%';

  /**
   * Image show height.
   */
  static readonly IMAGE_SHOW_HEIGHT: string = '68%';

  /**
   * Slider width.
   */
  static readonly SLIDER_WIDTH: string = '80%';

  /**
   * Loading width and height.
   */
  static readonly LOADING_WH: string = '30%';

  /**
   * Clock wise.
   */
  static readonly CLOCK_WISE: number = 90;

  /**
   * Anti clock.
   */
  static readonly ANTI_CLOCK: number = -90;

  /**
   * Tab menu width.
   */
  static readonly TAB_MENU_WIDTH: number = 18;

  /**
   * Navigation height.
   */
  static readonly NAVIGATION_HEIGHT: number = 56;

  /**
   * Adjust slider value.
   */
  static readonly ADJUST_SLIDER_VALUE: number[] = [100, 100, 100];

  /**
   * Slider min.
   */
  static readonly SLIDER_MIN: number = 1;

  /**
   * Slider step.
   */
  static readonly SLIDER_MAX: number = 100;

  /**
   * Slider step.
   */
  static readonly SLIDER_STEP: number = 1;

  /**
   * Pixel step.
   */
  static readonly PIXEL_STEP: number = 4;

  /**
   * Decimal two.
   */
  static readonly DECIMAL_TWO: number = 2;

  /**
   * Color level max.
   */
  static readonly COLOR_LEVEL_MAX: number = 255;

  /**
   * Convert int.
   */
  static readonly CONVERT_INT: number = 100;

  /**
   * Angle 60.
   */
  static readonly ANGLE_60: number = 60;

  /**
   * Angle 120.
   */
  static readonly ANGLE_120: number = 120;

  /**
   * Angle 240.
   */
  static readonly ANGLE_240: number = 240;

  /**
   * Angle 300.
   */
  static readonly ANGLE_360: number = 360;

  /**
   * Angle 360.
   */
  static readonly MOD_2: number = 2;

  /**
   * Average height and width.
   */
  static readonly AVERAGE_WEIGHT_WIDTH: number = 2;

  /**
   * Crop rate 4:3.
   */
  static readonly CROP_RATE_4_3: number = 0.75;

  /**
   * Crop rate 16:9.
   */
  static readonly CROP_RATE_9_16: number = 9 / 16;

  /**
   * Encode quality.
   */
  static readonly ENCODE_QUALITY: number = 100;

  /**
   * Title space.
   */
  static readonly TITLE_SPACE: number = 0;

  /**
   * Slider mode click.
   */
  static readonly SLIDER_CLICK_MODE: number = 3;

  /**
   * Encode format.
   */
  static readonly ENCODE_FORMAT: string = 'image/jpeg';

  /**
   * Encode file permission.
   */
  static readonly ENCODE_FILE_PERMISSION: string = 'rw';

  /**
   * Brightness worker file.
   */
  static readonly BRIGHTNESS_WORKER_FILE = 'entry/ets/workers/AdjustBrightnessWork.ts';

  /**
   * Brightness worker file.
   */
  static readonly SATURATION_WORKER_FILE = 'entry/ets/workers/AdjustSaturationWork.ts';

  /**
   * Image name.
   */
  static readonly IMAGE_PREFIX = 'image';

  /**
   * Image format.
   */
  static readonly IMAGE_FORMAT = '.jpg';

  /**
   * Rawfile name.
   */
  static readonly RAW_FILE_NAME = 'low.jpg';

  /**
   * Cache dir file name.
   */
  static readonly RAW_FILE_NAME_TEST = 'low_test.jpg';

  /**
   * Free ratio.
   */
  static readonly DEFAULT_RATIO: number = -1;

  /**
   * Ratio 1:1.
   */
  static readonly RATIO_1_1: number = 1;

  /**
   * Ratio 16:9.
   */
  static readonly RATIO_16_9: number[] = [16, 9];

  /**
   * Ratio 9:16.
   */
  static readonly RATIO_9_16: number[] = [9, 16];

  /**
   * Ratio 4:3.
   */
  static readonly RATIO_4_3: number[] = [4, 3];

  /**
   * Ratio 3:4.
   */
  static readonly RATIO_3_4: number[] = [3, 4];

  /**
   * Ratio 3:2.
   */
  static readonly RATIO_3_2: number[] = [3, 2];

  /**
   * Ratio 2:3.
   */
  static readonly RATIO_2_3: number[] = [2, 3];

  /**
   * Screen display margin.
   */
  static readonly SCREEN_DISPLAY_MARGIN: number = 15;

  /**
   * Double.
   */
  static readonly DOUBLE: number = 2;

  /**
   * Edit screen can use scope.
   */
  static readonly EDIT_SCREEN_USAGE: number = 0.68;

  /**
   * Title height.
   */
  static readonly TITLE_HEIGHT: number = 56;

  /**
   * Screen manager key.
   */
  static readonly APP_KEY_SCREEN_MANAGER: string = 'app_key_screen_manager';

  /**
   * Whether full screen.
   */
  static readonly IS_FULL_SCREEN_KEY: string = 'isFullScreen';

  /**
   * Device type.
   */
  static readonly DEFAULT_DEVICE_TYPE: string = 'phone';

  /**
   * Status bar color.
   */
  static readonly STATUS_BAR_BACKGROUND_COLOR: string = '#F1F3F5';

  /**
   * Status bar text color.
   */
  static readonly STATUS_BAR_CONTENT_COLOR: string = '#000000';

  /**
   * Navigation height.
   */
  static readonly TOP_BAR_SIZE: number = 56;

  /**
   * Tool bar size.
   */
  static readonly TOOL_BAR_SIZE: number = 72;

  /**
   * Time out.
   */
  static readonly TIMEOUT: number = 50;

  /**
   * Rect line width inner.
   */
  static readonly DEFAULT_LINE_WIDTH: number = 0.4;

  /**
   * Rect line width outer.
   */
  static readonly DEFAULT_LINE_RECT_WIDTH: number = 0.8;

  /**
   * Rect Button line width.
   */
  static readonly DEFAULT_BUTTON_WIDTH: number = 2.3;
  /**
   * Rect Button line padding.
   */
  static readonly DEFAULT_BUTTON_PADDING: number = 1;

  /**
   * Rect Button line length.
   */
  static readonly DEFAULT_BUTTON_LENGTH: number = 20;

  /**
   * Rect line color.
   */
  static readonly DEFAULT_LINE_COLOR: string = '#80FFFFFF';

  /**
   * Rect Button line color outer.
   */
  static readonly DEFAULT_RECT_LINE_COLOR: string = '#FFFFFFFF';

  /**
   * Rect Button line color.
   */
  static readonly DEFAULT_BUTTON_COLOR: string = 'white';

  /**
   * Mask style.
   */
  static readonly DEFAULT_MASK_STYLE: string = 'rgba(0, 0, 0, 0.3)';

  /**
   * Equality threshold.
   */
  static readonly EQUALITY_THRESHOLD = 0.0001;

  /**
   * Min side length.
   */
  static readonly DEFAULT_MIN_SIDE_LENGTH: number = 90;

  /**
   * Touch move identification range.
   */
  static readonly DEFAULT_TOUCH_BOUND: number = 20;

  /**
   * Base scale value.
   */
  static readonly BASE_SCALE_VALUE: number = 1.0;

  /**
   * Default image ratio.
   */
  static readonly DEFAULT_IMAGE_RATIO: number = 1.0;

  /**
   * Rect min side length.
   */
  static readonly DEFAULT_MIN_SIDE_LENGTH_EDIT: number = 32;

  /**
   * Default margin length.
   */
  static readonly DEFAULT_MARGIN_LENGTH: number = 20;

  /**
   * Time out.
   */
  static readonly DEFAULT_TIMEOUT_MILLISECOND_1000: number = 1000;

  /**
   * Default split fraction.
   */
  static readonly DEFAULT_SPLIT_FRACTION: number = 3;
}