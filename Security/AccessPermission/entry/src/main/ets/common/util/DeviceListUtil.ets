/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import deviceManager from '@ohos.distributedHardware.deviceManager';
import { CommonConstants, SubscribeInfoInterface } from '../constants/CommonConstants';
import Logger from './Logger';

/**
 * Log tag.
 */
const TAG = '[DeviceListUtil]';

export class DeviceListUtil {
  private dmInstance?: deviceManager.DeviceManager;
  private subscribeId: number = 0;
  private dealDeviceInfo?: Function;

  /**
   * Initializing Device Management Objects.
   */
  initDmInstance(dealDeviceInfo: Function) {
    this.dealDeviceInfo = dealDeviceInfo;
    try {
      deviceManager.createDeviceManager(getContext(this).applicationInfo.name, (err, data) => {
        if (err) {
          Logger.error(TAG, 'createDeviceManager errCode:' + err.code + ',errMessage:' + err.message);
          return;
        }
        this.dmInstance = data;
        this.deviceFoundOn();
        this.startDeviceDiscovery();
      });
    } catch (err) {
      Logger.error(TAG, 'createDeviceManager err=' + JSON.stringify(err));
    }
  }

  /**
   * Discovering Devices.
   */
  startDeviceDiscovery() {
    this.subscribeId = Math.floor(Math.random() * CommonConstants.RANDOM_ONE + CommonConstants.RANDOM_TWO);
    let subscribeInfo: SubscribeInfoInterface = {
      subscribeId: this.subscribeId,
      mode: CommonConstants.MODE,
      medium: 0,
      freq: CommonConstants.FREQ,
      isSameAccount: false,
      isWakeRemote: true,
      capability: 1
    };
    try {
      if (this.dmInstance !== undefined) {
        this.dmInstance.startDeviceDiscovery(subscribeInfo);
      }
    } catch (err) {
      Logger.error(TAG, 'startDeviceDiscovery err:' + JSON.stringify(err));
    }
  }

  /**
   * Registering and Discovering a Device Callback.
   */
  deviceFoundOn() {
    try {
      if (this.dmInstance !== undefined) {
        this.dmInstance.on('deviceFound', (data) => {
          if (this.dealDeviceInfo !== undefined) {
            this.dealDeviceInfo(data);
          }
        });
      }
    } catch (err) {
      Logger.error(TAG, 'deviceFoundOn err:' + JSON.stringify(err));
    }
  }

  /**
   * Stop querying devices.
   */
  stopDeviceDiscovery() {
    try {
      if (this.dmInstance !== undefined) {
        this.dmInstance.stopDeviceDiscovery(this.subscribeId);
      }
    } catch (err) {
      Logger.error(TAG, 'stopDeviceDiscovery err:' + JSON.stringify(err));
    }
  }

  /**
   * Indicates the ID of the processing device.
   *
   * @param deviceId Device ID.
   * @returns Indicates the ID of the device after processing.
   */
  dealDeviceId(deviceId: string) {
    return (deviceId.substring(0, CommonConstants.DEVICE_ID_LENGTH) + CommonConstants.DEVICE_ID_ELLIPSIS);
  }
}