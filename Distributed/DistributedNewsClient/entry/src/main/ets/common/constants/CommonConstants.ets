/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

export default class CommonConstants {
  /**
   * One.
   */
  static readonly ONE: number = 1;

  /**
   * The max lines of news.
   */
  static readonly NEWS_MAX_LINES: number = 2;

  /**
   * Full percent of component.
   */
  static readonly FULL_COMPONENT: string = '100%';

  /**
   * The weight of font.
   */
  static readonly FONT_WEIGHT_500: number = 500;

  /**
   * Ability name.
   */
  static readonly ABILITY_NAME: string = 'EntryAbility';

  /**
   * News detail page.
   */
  static readonly NEWS_DETAIL_PAGE: string = 'pages/NewsDetail';

  /**
   * Zero.
   */
  static readonly ZERO: string = '0';

  /**
   * All tabs title name.
   */
  static readonly ALL_TITLE: string[] = ['全部', '健康', '金融', '科技', '体育', '网络', '游戏'];

  /**
   * Zero column width.
   */
  static readonly ZERO_COLUMN: number = 0;

  /**
   * Four column width.
   */
  static readonly TWO_COLUMN: number = 2;

  /**
   * Four column width.
   */
  static readonly FOUR_COLUMN: number = 4;

  /**
   * Six column width.
   */
  static readonly SIX_COLUMN: number = 6;

  /**
   * Eight column width.
   */
  static readonly EIGHT_COLUMN: number = 8;

  /**
   * Twelve column width.
   */
  static readonly TWELVE_COLUMN: number = 12;

  /**
   * The width of small devices.
   */
  static readonly SMALL_DEVICE_TYPE: string = '320vp';

  /**
   * The width of middle devices.
   */
  static readonly MIDDLE_DEVICE_TYPE: string = '600vp';

  /**
   * The width of large devices.
   */
  static readonly LARGE_DEVICE_TYPE: string = '840vp';

  /**
   * The opacity of list divider.
   */
  static readonly DIVIDER_OPACITY: number = 0.05;

  /**
   * Breakpoints that represent small device types.
   */
  static readonly BREAKPOINT_SM: string = 'sm';

  /**
   * Trusted device list.
   */
  static readonly TRUSTED_DEVICE_LIST: string = 'trustedDeviceList';

  /**
   * Discover device list.
   */
  static readonly DISCOVER_DEVICE_LIST: string = 'discoverDeviceList';

  /**
   * The mode of start device discovery info.
   */
  static readonly INFO_MODE: number = 0xAA;

  /**
   * The frequency of start device discovery info.
   */
  static readonly INFO_FREQ: number = 2;

  /**
   * News id.
   */
  static readonly NEWS_ID: string[] = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15',
    '16', '17', '18', '19'];

  /**
   * News reads.
   */
  static readonly NEWS_READS: string[] = ['54', '100', '74', '44', '73', '100', '123', '754', '631', '53', '1500',
    '7451', '445', '734', '1010', '1243', '7574', '6311', '6341'];

  /**
   * News likes.
   */
  static readonly NEWS_LIKES: string[] = ['81', '354', '91', '82', '888', '354', '911', '149', '714', '824', '3542',
    '9511', '872', '8788', '3534', '9141', '1439', '7114', '7164'];

  /**
   * Number of titles.
   */
  static readonly TITLES_NUMBER: number = 6;

  /**
   * Number of images.
   */
  static readonly IMAGES_NUMBER: number = 4;
}