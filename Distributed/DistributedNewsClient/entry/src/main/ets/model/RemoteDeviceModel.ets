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
import prompt from '@ohos.promptAction';
import common from '@ohos.app.ability.common';
import CommonConstants from '../common/constants/CommonConstants';
import logger from '../common/utils/Logger';

// the value is in scope [0, 65535]
const SUBSCRIBE_ID = 100;

interface RejectError {
  code: number;
  message: string;
}

export interface DeviceInfoInterface {
  subscribeId: number,
  device: deviceManager.DeviceInfo
}

export interface AuthExtraInfoInterface {
  targetPkgName: string,
  appName: string,
  appDescription: string,
  business: string
}

class RemoteDeviceModel {
  private static dmInstance: RemoteDeviceModel | undefined = undefined;
  private trustedDeviceList: deviceManager.DeviceInfo[] = [];
  private discoverList: deviceManager.DeviceInfo[] = [];
  private deviceManager?: deviceManager.DeviceManager;

  static getInstance(): RemoteDeviceModel {
    if (RemoteDeviceModel.dmInstance === undefined) {
      RemoteDeviceModel.dmInstance = new RemoteDeviceModel();
    }
    return RemoteDeviceModel.dmInstance;
  }

  showToast(message: Resource): void {
    prompt.showToast({
      message: message
    });
  }

  async createDeviceManager(context: common.UIAbilityContext): Promise<void> {
    if (this.deviceManager !== undefined) {
      return;
    }
    await new Promise((resolve: (value: Object | PromiseLike<Object>) => void, reject:
      ((reason?: RejectError) => void)) => {
      deviceManager.createDeviceManager(context.abilityInfo.bundleName, (err, value) => {
        if (err) {
          reject(err);
          logger.error('createDeviceManager failed.');
          return;
        }
        this.deviceManager = value;
        this.registerDeviceStateListener();
        this.getTrustedDeviceList();
        resolve(value);
      })
    })
  }

  getTrustedDeviceList(): void {
    if (this.deviceManager === undefined) {
      logger.error('deviceManager has not initialized');
      this.showToast($r('app.string.no_device_manager'));
      return;
    }

    try {
      this.trustedDeviceList = this.deviceManager.getTrustedDeviceListSync();
      AppStorage.setOrCreate(CommonConstants.TRUSTED_DEVICE_LIST, this.trustedDeviceList);
    } catch (err) {
      logger.error(`getTrustedDeviceList failed error = ${JSON.stringify(err)}`);
      this.showToast($r('app.string.get_trusted_device_list_failed'));
    }
  }

  deviceStateChangeActionOnline(device: deviceManager.DeviceInfo): void {
    this.trustedDeviceList[this.trustedDeviceList.length] = device;
    for (let j: number = 0; j < this.trustedDeviceList.length; j++) {
      if (this.trustedDeviceList[j].deviceId === device.deviceId) {
        this.trustedDeviceList[j] = device;
        break;
      }
    }
    AppStorage.setOrCreate(CommonConstants.TRUSTED_DEVICE_LIST, this.trustedDeviceList);
  }

  deviceStateChangeActionOffline(device: deviceManager.DeviceInfo): void {
    let list: deviceManager.DeviceInfo[] = [];
    for (let i: number = 0; i < this.trustedDeviceList.length; i++) {
      if (this.trustedDeviceList[i].deviceId !== device.deviceId) {
        list.push(this.trustedDeviceList[i]);
      }
    }
    this.trustedDeviceList = list;
    AppStorage.setOrCreate(CommonConstants.TRUSTED_DEVICE_LIST, this.trustedDeviceList);
  }

  registerDeviceStateListener(): void {
    if (this.deviceManager === undefined) {
      logger.error('deviceManager has not initialized');
      this.showToast($r('app.string.no_device_manager'));
      return;
    }

    this.deviceManager.on('deviceStateChange', (data) => {
      if (data === null) {
        return;
      }
      switch (data.action) {
        case deviceManager.DeviceStateChangeAction.ONLINE:
          this.deviceStateChangeActionOnline(data.device);
          break;
        case deviceManager.DeviceStateChangeAction.OFFLINE:
          this.deviceStateChangeActionOffline(data.device);
          break;
        default:
          break;
      }
    })
  }

  deviceFound(data: DeviceInfoInterface): void {
    for (let i: number = 0; i < this.discoverList.length; i++) {
      if (this.discoverList[i].deviceId === data.device.deviceId) {
        return;
      }
    }
    this.discoverList[this.discoverList.length] = data.device;
    AppStorage.setOrCreate(CommonConstants.DISCOVER_DEVICE_LIST, this.discoverList);
  }

  startDeviceDiscovery(): void {
    if (this.deviceManager === undefined) {
      logger.error('deviceManager has not initialized');
      this.showToast($r('app.string.no_device_manager'));
      return;
    }
    this.deviceManager.on('deviceFound', (data) => {
      if (data === null) {
        return;
      }
      this.deviceFound(data);
    })
    this.deviceManager.on('discoverFail', (data) => {
      logger.error(`discoverFail data = ${JSON.stringify(data)}`);
    })
    this.deviceManager.on('serviceDie', () => {
      logger.error('serviceDie');
    })

    let info: deviceManager.SubscribeInfo = {
      subscribeId: SUBSCRIBE_ID,
      mode: CommonConstants.INFO_MODE,
      medium: 0,
      freq: CommonConstants.INFO_FREQ,
      isSameAccount: false,
      isWakeRemote: true,
      capability: 0
    };
    this.discoverList = [];
    AppStorage.setOrCreate(CommonConstants.DISCOVER_DEVICE_LIST, this.discoverList);

    try {
      this.deviceManager.startDeviceDiscovery(info);
    } catch (err) {
      logger.error(`startDeviceDiscovery failed error = ${JSON.stringify(err)}`);
    }
  }

  stopDeviceDiscovery(): void {
    if (this.deviceManager === undefined) {
      logger.error('deviceManager has not initialized');
      this.showToast($r('app.string.no_device_manager'));
      return;
    }
    try {
      this.deviceManager.stopDeviceDiscovery(SUBSCRIBE_ID);
      this.deviceManager.off('deviceFound');
      this.deviceManager.off('discoverFail');
    } catch (err) {
      logger.error(`stopDeviceDiscovery failed error = ${JSON.stringify(err)}`);
    }
  }

  unregisterDeviceListCallback(): void {
    if (this.deviceManager === undefined) {
      logger.error('deviceManager has not initialized');
      this.showToast($r('app.string.no_device_manager'));
      return;
    }
    try {
      this.deviceManager.off('deviceStateChange');
      this.deviceManager.off('serviceDie');
      this.deviceManager.release();
      this.trustedDeviceList = [];
      this.discoverList = [];
    } catch (err)  {
      logger.error(`stopDeviceDiscovery failed error = ${JSON.stringify(err)}` );
    }
  }

  authenticateDevice(device: deviceManager.DeviceInfo, context: common.UIAbilityContext): void {
    if (this.deviceManager === undefined) {
      logger.error('deviceManager has not initialized');
      this.showToast($r('app.string.no_device_manager'));
      return;
    }

    for (let i: number = 0; i < this.discoverList.length; i++) {
      if (this.discoverList[i].deviceId !== device.deviceId) {
        continue;
      }
      let extraInfo: AuthExtraInfoInterface = {
        targetPkgName: context.abilityInfo.bundleName,
        appName: context.applicationInfo.name,
        appDescription: context.applicationInfo.description,
        business: CommonConstants.ZERO
      };
      let authParam: deviceManager.AuthParam = {
        'authType': CommonConstants.ONE,
        'extraInfo': extraInfo
      };
      try {
        this.deviceManager.authenticateDevice(device, authParam, (err) => {
          if (err) {
            logger.error(`authenticateDevice error. Code is ${err.code}, message is ${err.message}`);
            return;
          }
        })
      } catch (err) {
        logger.error(`authenticateDevice failed error = ${JSON.stringify(err)}`);
      }
    }
  }
}

export default RemoteDeviceModel.getInstance();