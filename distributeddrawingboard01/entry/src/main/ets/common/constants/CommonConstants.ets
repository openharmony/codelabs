/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
// 公共常量类
import deviceManager from '@ohos.distributedDeviceManager';

export interface DeviceInfoInterface {
  subscribeId: number,
  device: deviceManager.DeviceBasicInfo
}

export interface AuthExtraInfoInterface {
  targetPkgName: string,
  appName: string,
  appDescription: string,
  business: string
}

/**
 * Common constants for all features.
 */
export default class CommonConstants {
  /**
   * KvStore key.
   */
  static readonly CHANGE_POSITION: string = 'change_position';
  /**
   * KvStore id.
   */
  static readonly KVSTORE_ID: string = 'draw_board_kvstore';
  /**
   * One hundred percent.
   */
  static readonly FULL_PERCENT: string = '100%';
  /**
   * Icon margin left.
   */
  static readonly ICON_MARGIN_LEFT: string = '6.7%';
  /**
   * Title height.
   */
  static readonly TITLE_HEIGHT: string = '10%';
  /**
   * Number one.
   */
  static readonly NUMBER_ONE: number = 1;
  /**
   * Canvas line width.
   */
  static readonly CANVAS_LINE_WIDTH: number = 4;
  /**
   * Canvas line join.
   */
  static readonly CANVAS_LINE_JOIN: 'bevel' | 'miter' | 'round' = 'miter';
  /**
   * Localhost name.
   */
  static readonly LOCALHOST_NAME: string = '本机';
  /**
   * Font weight.
   */
  static readonly FONT_WEIGHT_500: number = 500;
  /**
   * Invalid Index.
   */
  static readonly INVALID_INDEX: number = -1;
  /**
   * 设备名称宽度.
   */
  static readonly DEVICE_NAME_WIDTH: string = '78%';
  /**
   * 选择图表的宽度.
   */
  static readonly SELECT_ICON_WIDTH: string = '8%';
  /**
   * 订阅ID范围.
   */
  static readonly SUBSCRIBE_ID_RANGE: number = 65536;
  /**
   * 能力名称.
   */
  static readonly ABILITY_NAME: string = 'EntryAbility';
  /**
   * 订阅模式.
   */
  static readonly SUBSCRIBE_MODE: number = 0xAA;
  /**
   * .
   */
  static readonly SUBSCRIBE_MEDIUM: number = 0;
  /**
   * 订阅频率.
   */
  static readonly SUBSCRIBE_FREQ: number = 2;
  /**
   * 订阅
   */
  static readonly SUBSCRIBE_CAPABILITY: number = 0;
  /**
   * 验证类型.
   */
  static readonly AUTH_TYPE: number = 1;
  /**
   * 应用描述.
   */
  static readonly APP_DESCRIPTION: string = 'Distribute Draw Ability';
  /**
   * 类型.
   */
  static readonly BUSINESS_TYPE: string = '0';
  /**
   * 应用名称
   */
  static readonly BUNDLE_NAME: string = 'com.example.distributeDraw';
}