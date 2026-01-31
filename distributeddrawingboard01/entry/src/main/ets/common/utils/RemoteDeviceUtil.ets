/*
 * Copyright (c) 2025 OpenHarmony Foundation.
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
//设备管理
import deviceManager from '@ohos.distributedDeviceManager';
import common from '@ohos.app.ability.common';
import Want from '@ohos.app.ability.Want';
import CommonConstants from '../constants/CommonConstants';
import { DeviceInfoInterface, AuthExtraInfoInterface } from '../constants/CommonConstants';
import Position from '../../viewmodel/Position';
import Logger from './Logger';
import { BusinessError } from '@ohos.base';


interface RejectError {
  code: number;
  message: string;
}


class DeviceData {
  device: deviceManager.DeviceBasicInfo = {
    deviceId: "",
    deviceName: "",
    deviceType: "",
    networkId: "",
  }
}


class RemoteDeviceUtil {
  private static dmInstance: RemoteDeviceUtil | undefined = undefined;
  private myDeviceManager?: deviceManager.DeviceManager;
  private localDevice?: deviceManager.DeviceBasicInfo = {
    deviceId: "",
    deviceName: "",
    deviceType: "",
    networkId: "",
  }
  private deviceList: deviceManager.DeviceBasicInfo[] = [];
  private trustDeviceList: deviceManager.DeviceBasicInfo[] = [];
  private discoverList: deviceManager.DeviceBasicInfo[] = [];
  private subscribeId: number = Math.floor(CommonConstants.SUBSCRIBE_ID_RANGE * Math.random());

  //获取实例
  static getInstance(): RemoteDeviceUtil {
    if (RemoteDeviceUtil.dmInstance === undefined) {
      RemoteDeviceUtil.dmInstance = new RemoteDeviceUtil();
    }
    return RemoteDeviceUtil.dmInstance;
  }

  //创建设备管理器
  async createDeviceManager() {
    if (this.myDeviceManager !== undefined) {
      Logger.info('remoteDeviceModel', 'createDeviceManager myDeviceManager exist');
      return;
    }
    await new Promise((resolve: (value: Object | PromiseLike<Object>) => void, reject: ((reason?: RejectError) => void)) => {
      try {
        //创建设备管理器
        this.myDeviceManager = deviceManager.createDeviceManager(CommonConstants.BUNDLE_NAME);
        //上下线监听
        this.registerDeviceStateListener();
        //获取本地设备
        this.getLocalDeviceInfo();
        //获取受信任列表
        this.getTrustedDeviceList();
        //初始化设备列表
        this.initDeviceList();

      } catch (error) {
        Logger.error('RemoteDeviceModel', `createDeviceManager failed, error=${JSON.stringify(error)}`);
      }
    })
  }

  //获取本地设备信息
  getLocalDeviceInfo(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('remoteDeviceModel', 'getLocalDeviceInfo initialized failed');
      return;
    }
    try {
      this.localDevice = {
        deviceId: "",
        deviceName: "",
        deviceType: "",
        networkId: "",
      };
      this.localDevice.deviceId = this.myDeviceManager.getLocalDeviceId();
      this.localDevice.deviceName = this.myDeviceManager.getLocalDeviceName();
      this.localDevice.deviceType = "" + this.myDeviceManager.getLocalDeviceType();
      this.localDevice.networkId = this.myDeviceManager.getLocalDeviceNetworkId();
      this.localDevice.deviceName = CommonConstants.LOCALHOST_NAME;
    } catch (error) {
      Logger.error('RemoteDeviceModel', `getLocalDeviceInfo failed, error=${JSON.stringify(error)}`);
    }
  }

  //获取受信任设备
  getTrustedDeviceList(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'getTrustedDeviceList deviceManager has not initialized')
      return;
    }
    try {
      this.trustDeviceList = this.myDeviceManager.getAvailableDeviceListSync();
    } catch (error) {
      Logger.error('RemoteDeviceModel', `getTrustedDeviceList failed error=${JSON.stringify(error)}`);
    }
  }

  //初始化设备列表
  initDeviceList(): void {
    this.deviceList = [];
    if (this.localDevice !== undefined) {
      this.addToDeviceList(this.localDevice);
    }
    this.trustDeviceList.forEach((item: deviceManager.DeviceBasicInfo) => {
      this.addToDeviceList(item);
    })
  }

  //设备监听
  registerDeviceStateListener(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'registerDeviceStateListener deviceManager has not initialized');
      return;
    }
    try {
      //注册设备状态改变
      this.myDeviceManager.on('deviceStateChange', (data) => {
        if (data === null) {
          return;
        }
        switch (data.action) {
        //上线
          case deviceManager.DeviceStateChange.AVAILABLE: {
            this.deviceStateChangeActionOnlice(data.device);
            break;
          }
        //下线
          case deviceManager.DeviceStateChange.UNAVAILABLE: {
            this.deviceStateChangeActionOffline(data.device);
            break;
          }
          default: {
            break;
          }
        }
      });
    } catch (error) {
      let e: BusinessError = error as BusinessError;
      Logger.error('RemoteDeviceModel', `registerDeviceStateListener on('deviceStateChange') failed, error=${e.code},${e.message}}`);
    }
  }

  //设备上线
  deviceStateChangeActionOnlice(device: deviceManager.DeviceBasicInfo): void {
    this.trustDeviceList[this.trustDeviceList.length] = device;
    this.addToDeviceList(device);
  }

  //设备下线
  deviceStateChangeActionOffline(device: deviceManager.DeviceBasicInfo): void {
    let list: deviceManager.DeviceBasicInfo[] = [];
    for (let i: number = 0; i < this.trustDeviceList.length; i++) {
      if (this.trustDeviceList[i].networkId !== device.networkId) {
        list.push(this.trustDeviceList[i])
        continue;
      }
    }
    this.deleteFromDeviceList(device);
    this.trustDeviceList = list;
  }

  //添加设备
  addToDeviceList(device: deviceManager.DeviceBasicInfo): void {
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

  //删除设备
  deleteFromDeviceList(device: deviceManager.DeviceBasicInfo): void {
    for (let i: number = 0; i < this.deviceList.length; i++) {
      if (device.deviceId === this.deviceList[i].deviceId) {
        this.deviceList.splice(i, 1);
        break;
      }
    }
    AppStorage.setOrCreate('deviceList', this.deviceList);
  }

  //注销监听
  unregisterDeviceListCallback(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'unregisterDeviceListCallback deviceManager has not initialized');
      return;
    }

    try {
      //注销设备监听状态
      this.myDeviceManager.off('deviceStateChange');
      //释放是咧
      deviceManager.releaseDeviceManager(this.myDeviceManager);
    } catch (error) {
      Logger.error('RemoteDeviceModel', `unregisterDeviceListCallback stopDeviceDiscovery failed, error=${JSON.stringify(error)}`);
    }
  }

  //设备认证
  authenticateDevice(
    context: common.UIAbilityContext,
    device: deviceManager.DeviceBasicInfo,
    positionList: Position[]
  ): void {
    let  tempList = this.trustDeviceList.filter((item: deviceManager.DeviceBasicInfo) => device.deviceId === item.deviceId);
    if (tempList.length > 0) {
      this.startAbility(context,device,positionList);
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


    let bindParam: Record<string, string | number> = {
      'bindType': 1, // 认证类型： 1 - 无帐号PIN码认证
      'targetPkgName': context.abilityInfo.bundleName,
      'appName': context.abilityInfo.applicationInfo.name,
      'appOperation': 'xxxx',
      'customDescription': CommonConstants.APP_DESCRIPTION
    };

    try {
      //z执行设备认证
      this.myDeviceManager.bindTarget(device.deviceId,bindParam,(err,data) => {
        if (err) {
          Logger.error('RemoteDeviceModel', `authenticateDevice error code=${err.code}, msg=${JSON.stringify(err.message)}`);
          return;
        }
        Logger.info("bindTarget result:" + JSON.stringify(data));
      })
    }catch (error) {
      Logger.error('RemoteDeviceModel', `authenticateDevice failed error=${JSON.stringify(error)}`);
    }



  }

  //启动设备链接
  startAbility(context: common.UIAbilityContext, device: deviceManager.DeviceBasicInfo, positionList: Position[]): void {
    console.log('console');
    let wantValue: Want = {
      bundleName: context.abilityInfo.bundleName,
      abilityName: CommonConstants.ABILITY_NAME,
      deviceId: device.networkId,
      parameters: {
        positionList: JSON.stringify(positionList)
      }
    };
    //起动应用
    context.startAbility(wantValue).then(() => {
      Logger.info('RemoteDeviceModel', `startAbility finished wantValue=${JSON.stringify(wantValue)}`);
    }).catch((error: Error) => {
      Logger.error('RemoteDeviceModel', `startAbility failed, error=${JSON.stringify(error)}`);
    })
  }

  //处理新发现的设备
  deviceFound(data: DeviceData): void {
    for (let i: number = 0; i < this.deviceList.length; i++) {
      if (this.discoverList[i].deviceId === data.device.deviceId) {
        Logger.info('RemoteDeviceModel', `deviceFound device exist=${JSON.stringify(data)}`);
        return;
      }
    }
    this.deviceList[this.deviceList.length] = data.device;
    this.addToDeviceList(data.device);
  }

  //开始发现
  startDeviceDiscovery(): void {
    this.discoverList = [];
    this.initDeviceList();
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'startDeviceDiscovery deviceManager has not initialized');
      return;
    }

    //注册发现设备
    try {
      this.myDeviceManager.on('discoverSuccess', (data: DeviceData) => {
        if (data === null) {
          return;
        }
        Logger.info('RemoteDeviceModel', `startDeviceDiscovery deviceFound data=${JSON.stringify(data)}`);
        // 处理发现的设备
        this.deviceFound(data);
      });
      this.myDeviceManager.on('discoverFailure', (data) => {
        if (data === null) {
          return;
        }
        Logger.info('RemoteDeviceModel', `startDeviceDiscovery discoverFail data=${JSON.stringify(data)}`);
      });
      let discoverParam: Record<string, number> = {
        'discoverTargetType': 1
      };
      let filterOptions: Record<string, number> = {
        'availableStatus': 0,
      };
      //开始发现
      this.myDeviceManager.startDiscovering(discoverParam);
    } catch (error) {
      Logger.error('RemoteDeviceModel', `startDeviceDiscovery failed error=${JSON.stringify(error)}`);
    }
  }

  //停止发现
  stopDeviceDiscovery(): void {
    if (this.myDeviceManager === undefined) {
      Logger.error('RemoteDeviceModel', 'stopDeviceDiscovery deviceManager has not initialized');
      return;
    }
    try {
      this.myDeviceManager.stopDiscovering();
      this.myDeviceManager.off('discoverSuccess');
    } catch (error) {
      Logger.error('RemoteDeviceModel', `stopDeviceDiscovery failed error=${JSON.stringify(error)}`);
    }
  }
}

export default RemoteDeviceUtil.getInstance();
