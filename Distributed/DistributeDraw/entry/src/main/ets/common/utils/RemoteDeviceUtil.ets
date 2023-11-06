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
import common from '@ohos.app.ability.common';
import Want from '@ohos.app.ability.Want';
import CommonConstants from '../constants/CommonConstants';
import { DeviceInfoInterface, AuthExtraInfoInterface } from '../constants/CommonConstants';
import Position from '../../viewmodel/Position';
import Logger from './Logger';

interface RejectError {
  code: number;
  message: string;
}

class RemoteDeviceUtil {
  private static dmInstance: RemoteDeviceUtil | undefined = undefined;
  private myDeviceManager?: deviceManager.DeviceManager;
  private localDevice?: deviceManager.DeviceInfo;
  private deviceList: deviceManager.DeviceInfo[] = [];
  private trustedDeviceList: deviceManager.DeviceInfo[] = [];
  private discoverList: deviceManager.DeviceInfo[] = [];
  private subscribeId: number = Math.floor(CommonConstants.SUBSCRIBE_ID_RANGE * Math.random());

  /**
   * Obtaining a class instance.
   *
   * @returns Class instance.
   */
  static getInstance(): RemoteDeviceUtil {
    if (RemoteDeviceUtil.dmInstance === undefined) {
      RemoteDeviceUtil.dmInstance = new RemoteDeviceUtil();
    }
    return RemoteDeviceUtil.dmInstance;
  }

  /**
   * Create a device manager.
   */
  async createDeviceManager() {
    if (this.myDeviceManager !== undefined) {
      Logger.info('RemoteDeviceModel', 'createDeviceManager myDeviceManager exist');
      return;
    }

    await new Promise((resolve: (value: Object | PromiseLike<Object>) => void, reject: ((reason?: RejectError) => void)) => {
      try {
        deviceManager.createDeviceManager(CommonConstants.BUNDLE_NAME,
          (error, value: deviceManager.DeviceManager) => {
            if (error) {
              reject(error);
              Logger.error('RemoteDeviceModel',
                `createDeviceManager failed, error=${JSON.stringify(error)}`);
              return;
            }
            this.myDeviceManager = value;
            // Registered trusted device online and offline listening.
            this.registerDeviceStateListener();
            this.getLocalDeviceInfo();
            this.getTrustedDeviceList();
            this.initDeviceList();
            resolve(value);
          });
      } catch (error) {
        Logger.error('RemoteDeviceModel',
          `createDeviceManager failed, error=${JSON.stringify(error)}`);
      }
    });
  }

  /**
   * Obtains local device information.
   */
  getLocalDeviceInfo(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'getLocalDeviceInfo deviceManager has not initialized');
      return;
    }

    try {
      this.localDevice = this.myDeviceManager.getLocalDeviceInfoSync();
      this.localDevice.deviceName = CommonConstants.LOCALHOST_NAME;
    } catch (error) {
      Logger.error('RemoteDeviceModel',
        `getLocalDeviceInfo failed, error=${JSON.stringify(error)}`);
    }
  }

  /**
   * Obtain the list of trusted devices.
   */
  getTrustedDeviceList(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'getTrustedDeviceList deviceManager has not initialized');
      return;
    }

    try {
      this.trustedDeviceList = this.myDeviceManager.getTrustedDeviceListSync();
    } catch (error) {
      Logger.error('RemoteDeviceModel',
        `getTrustedDeviceList failed error=${JSON.stringify(error)}`);
    }
  }

  /**
   * Initialize the device list.
   */
  initDeviceList(): void {
    this.deviceList = [];
    if (this.localDevice !== undefined) {
      this.addToDeviceList(this.localDevice)
    }
    this.trustedDeviceList.forEach((item: deviceManager.DeviceInfo) => {
      this.addToDeviceList(item);
    })
  }

  /**
   * Register the device status listener.
   */
  registerDeviceStateListener(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'registerDeviceStateListener deviceManager has not initialized');
      return;
    }

    try {
      // Registering a device status change listener.
      this.myDeviceManager.on('deviceStateChange', (data) => {
        if (data === null) {
          return;
        }
        switch (data.action) {
          case deviceManager.DeviceStateChangeAction.ONLINE: {
            this.deviceStateChangeActionOnline(data.device);
            break;
          }
          case deviceManager.DeviceStateChangeAction.OFFLINE: {
            this.deviceStateChangeActionOffline(data.device);
            break;
          }
          default: {
            break;
          }
        }
      });
    } catch (error) {
      Logger.error('RemoteDeviceModel',
        `registerDeviceStateListener on('deviceStateChange') failed, error=${JSON.stringify(error)}`);
    }
  }

  /**
   * The device goes online and is added to the trust list.
   *
   * @param device Information about online devices.
   */
  deviceStateChangeActionOnline(device: deviceManager.DeviceInfo): void {
    this.trustedDeviceList[this.trustedDeviceList.length] = device;
    this.addToDeviceList(device);
  }

  /**
   * Device offline.
   *
   * @param device Information about offline devices.
   */
  deviceStateChangeActionOffline(device: deviceManager.DeviceInfo): void {
    let list: deviceManager.DeviceInfo[] = [];
    for (let i: number = 0; i < this.trustedDeviceList.length; i++) {
      if (this.trustedDeviceList[i].networkId !== device.networkId) {
        list.push(this.trustedDeviceList[i]);
        continue;
      }
    }
    this.deleteFromDeviceList(device);
    this.trustedDeviceList = list;
  }

  /**
   * Disabling the listening event.
   */
  unregisterDeviceListCallback(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'unregisterDeviceListCallback deviceManager has not initialized');
      return;
    }

    try {
      this.myDeviceManager.off('deviceStateChange');
      this.myDeviceManager.release();
    } catch (err) {
      Logger.error('RemoteDeviceModel',
        `unregisterDeviceListCallback stopDeviceDiscovery failed, error=${JSON.stringify(err)}`);
    }
  }

  /**
   * Device connection verification.
   *
   * @param context Ability context.
   * @param device Device information.
   * @param positionList Position List.
   */
  authenticateDevice(
    context: common.UIAbilityContext,
    device: deviceManager.DeviceInfo,
    positionList: Position[]
  ): void {
    let tmpList = this.trustedDeviceList.filter((item: deviceManager.DeviceInfo) => device.deviceId === item.deviceId);
    if (tmpList.length > 0) {
      this.startAbility(context, device, positionList);
      return;
    }

    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'authenticateDevice deviceManager has not initialized');
      return;
    }

    let extraInfo: AuthExtraInfoInterface = {
      targetPkgName: context.abilityInfo.bundleName,
      appName: context.abilityInfo.applicationInfo.name,
      appDescription: CommonConstants.APP_DESCRIPTION,
      business: CommonConstants.BUSINESS_TYPE
    };
    let authParam: deviceManager.AuthParam = {
      'authType': CommonConstants.AUTH_TYPE,
      'extraInfo': extraInfo
    };
    try {
      this.myDeviceManager.authenticateDevice(device, authParam, (err) => {
        if (err) {
          Logger.error('RemoteDeviceModel',
            `authenticateDevice error code=${err.code}, msg=${JSON.stringify(err.message)}`);
          return;
        }
      })
    } catch (error) {
      Logger.error('RemoteDeviceModel',
        `authenticateDevice failed error=${JSON.stringify(error)}`);
    }
  }

  /**
   * Start Ability.
   *
   * @param context Ability context.
   * @param device Device information.
   * @param positionList Position list.
   */
  startAbility(context: common.UIAbilityContext, device: deviceManager.DeviceInfo, positionList: Position[]): void {
    let wantValue: Want = {
      bundleName: context.abilityInfo.bundleName,
      abilityName: CommonConstants.ABILITY_NAME,
      deviceId: device.networkId,
      parameters: {
        positionList: JSON.stringify(positionList)
      }
    };

    context.startAbility(wantValue).then(() => {
      Logger.info('RemoteDeviceModel', `startAbility finished wantValue=${JSON.stringify(wantValue)}`);
    }).catch((error: Error) => {
      Logger.error('RemoteDeviceModel', `startAbility failed, error=${JSON.stringify(error)}`);
    })
  }

  /**
   * Discover the device.
   *
   * @param data Device information.
   */
  deviceFound(data: DeviceInfoInterface): void {
    for (let i: number = 0; i < this.discoverList.length; i++) {
      if (this.discoverList[i].deviceId === data.device.deviceId) {
        Logger.info('RemoteDeviceModel', `deviceFound device exist=${JSON.stringify(data)}`);
        return;
      }
    }
    this.discoverList[this.discoverList.length] = data.device;
    this.addToDeviceList(data.device);
  }

  /**
   * Start discovering devices.
   */
  startDeviceDiscovery(): void {
    this.discoverList = [];
    this.initDeviceList();
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'startDeviceDiscovery deviceManager has not initialized');
      return;
    }

    try {
      this.myDeviceManager.on('deviceFound', (data) => {
        if (data === null) {
          return;
        }
        Logger.info('RemoteDeviceModel', `startDeviceDiscovery deviceFound data=${JSON.stringify(data)}`);
        this.deviceFound(data);
      });
      this.myDeviceManager.on('discoverFail', (data) => {
        if (data === null) {
          return;
        }
        Logger.info('RemoteDeviceModel', `startDeviceDiscovery discoverFail data=${JSON.stringify(data)}`);
      });

      let info: deviceManager.SubscribeInfo = {
        subscribeId: this.subscribeId,
        mode: CommonConstants.SUBSCRIBE_MODE,
        medium: CommonConstants.SUBSCRIBE_MEDIUM,
        freq: CommonConstants.SUBSCRIBE_FREQ,
        isSameAccount: false,
        isWakeRemote: true,
        capability: CommonConstants.SUBSCRIBE_CAPABILITY
      };

      this.myDeviceManager.startDeviceDiscovery(info);
    } catch (error) {
      Logger.error('RemoteDeviceModel',
        `startDeviceDiscovery failed error=${JSON.stringify(error)}`);
    }
  }

  /**
   * Stop discovering devices.
   */
  stopDeviceDiscovery(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'stopDeviceDiscovery deviceManager has not initialized');
      return;
    }

    try {
      this.myDeviceManager.stopDeviceDiscovery(this.subscribeId);
      this.myDeviceManager.off('deviceFound');
      this.myDeviceManager.off('discoverFail');
    } catch (error) {
      Logger.error('RemoteDeviceModel',
        `stopDeviceDiscovery failed error=${JSON.stringify(error)}`);
    }
  }

  /**
   * Add a device to the device list.
   *
   * @param device Device information.
   */
  addToDeviceList(device: deviceManager.DeviceInfo): void {
    let isExist: boolean = false;
    for (let i: number = 0; i < this.deviceList.length; i++) {
      if (device.deviceId === this.deviceList[i].deviceId) {
        this.deviceList[i] = device;
        isExist = true;
        break;
      }
    }
    if (!isExist) {
      this.deviceList.push(device);
    }

    AppStorage.setOrCreate('deviceList', this.deviceList);
  }

  /**
   * Deletes a device from the device list.
   *
   * @param device Device information.
   */
  deleteFromDeviceList(device: deviceManager.DeviceInfo): void {
    for (let i: number = 0; i < this.deviceList.length; i++) {
      if (device.deviceId === this.deviceList[i].deviceId) {
        this.deviceList.splice(i, 1);
        break;
      }
    }
    AppStorage.setOrCreate('deviceList', this.deviceList);
  }
}

export default RemoteDeviceUtil.getInstance();