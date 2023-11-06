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

import deviceManager from '@ohos.distributedHardware.deviceManager';
import { Permissions } from '@ohos.abilityAccessCtrl';

export interface DeviceInfoInterface {
  subscribeId: number,
  device: deviceManager.DeviceInfo
}

/**
 * Common constants for all features.
 */
export class CommonConstants {
  /**
   * Full percent.
   */
  static readonly FULL_PERCENT: string = '100%';

  /**
   * X-axis coordinate of the dialog box.
   */
  static readonly DIALOG_OFFSET_X: number = 0;

  /**
   * Y-axis coordinate of the dialog box.
   */
  static readonly DIALOG_OFFSET_Y: number = -20;

  /**
   * Image height on the main page.
   */
  static readonly HOME_IMAGE_HEIGHT: string = '34.5%';

  /**
   * Main Page Button Width.
   */
  static readonly HOME_BUTTON_WIDTH: string = '86.7%';

  /**
   * Width of the text on the device list page.
   */
  static readonly DEVICE_TEXT_WIDTH: string = '80%';

  /**
   * Width of the text on the device list page.
   */
  static readonly DEVICE_IMAGE_WIDTH: string = '6.7%';

  /**
   * Radio Button Name.
   */
  static readonly RADIO_NAME: string = 'radio';

  /**
   * Radio Button Group Name.
   */
  static readonly RADIO_BUTTON_GROUP_NAME: string = 'radioGroup';

  /**
   * Left margin of the button.
   */
  static readonly RADIO_LEFT: string = '5.3%';

  /**
   * Row width.
   */
  static readonly COLUMN_WIDTH: string = '80%';

  /**
   * Radio Column width.
   */
  static readonly RADIO_COLUMN_WIDTH: string = '20%';

  /**
   * Text Width.
   */
  static readonly PERMISSION_TEXT_WIDTH: string = '80%';


  /**
   * Random parameter 1.
   */
  static readonly RANDOM_ONE: number = 10000;

  /**
   * Random parameter 2.
   */
  static readonly RANDOM_TWO: number = 1000;

  /**
   * High frequency.
   */
  static readonly FREQ: number = 2;

  /**
   * Active mode.
   */
  static readonly MODE: number = 0xAA;

  /**
   * Permissions.
   */
  static readonly PERMISSION: Permissions = 'ohos.permission.DISTRIBUTED_DATASYNC';

  /**
   * Device discovery flag.
   */
  static readonly DEVICE_FOUND: string = 'deviceFound';

  /**
   * Device ID length.
   */
  static readonly DEVICE_ID_LENGTH: number = 16;

  /**
   * Device ID ellipsis.
   */
  static readonly DEVICE_ID_ELLIPSIS: string = '...';

  /**
   * Minimum height of the device list.
   */
  static readonly LIST_MIN_HEIGHT: string = '10%';

  /**
   * Maximum height of the device list.
   */
  static readonly LIST_MAX_HEIGHT: string = '50%';
}

export interface SubscribeInfoInterface {
  subscribeId: number,
  mode: number,
  medium: number,
  freq: number,
  isSameAccount: boolean,
  isWakeRemote: boolean,
  capability: number
}
