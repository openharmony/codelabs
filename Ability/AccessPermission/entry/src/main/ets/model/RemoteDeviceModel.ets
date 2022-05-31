/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

var SUBSCRIBE_ID = 100;

// 远程设备模型类
export default class RemoteDeviceModel {
  // 设备集合：deviceManager.getTrustedDeviceListSync
  deviceList = [];
  // 设备集合：deviceManager.on
  discoverList = [];
  // 回调index.ets中的callback方法
  callback;
  // 设备管理器
  deviceManager;
  // 构造函数
  constructor() {
  }

  // 发起请求配对时调用此方法（index.ets调用）
  registerDeviceListCallback(callback) {
    if (typeof (this.deviceManager) === 'undefined') {
      console.log('AccessPermission[RemoteDeviceModel] deviceManager.createDeviceManager begin');
      let self = this;
      // 创建设备管理器
      deviceManager.createDeviceManager('com.example.helloworld0218', (error, value) => {
        if (error) {
          console.error('createDeviceManager failed.');
          return;
        }
        // 获取设备管理器并保存
        self.deviceManager = value;
        // 注册设备列表
        self.registerDeviceListCallback_(callback);
        console.log('AccessPermission[RemoteDeviceModel] createDeviceManager callback returned, error=' + error + ' value=' + value);
      });
      console.log('AccessPermission[RemoteDeviceModel] deviceManager.createDeviceManager end');
    } else {
      this.registerDeviceListCallback_(callback);
    }
  }

  // 注册设备列表
  registerDeviceListCallback_(callback) {
    console.info('AccessPermission[RemoteDeviceModel] registerDeviceListCallback');
    this.callback = callback;
    if (this.deviceManager == undefined) {
      console.error('AccessPermission[RemoteDeviceModel] deviceManager has not initialized');
      this.callback();
      return;
    }

    console.info('AccessPermission[RemoteDeviceModel] getTrustedDeviceListSync begin');
    // 获取设备集合
    var list = this.deviceManager.getTrustedDeviceListSync();
    console.info('AccessPermission[RemoteDeviceModel] getTrustedDeviceListSync end, deviceList=' + JSON.stringify(list));
    if (typeof (list) != 'undefined' && typeof (list.length) != 'undefined') {
      this.deviceList = list;
    }

    this.callback();
    console.info('AccessPermission[RemoteDeviceModel] callback finished');

    let self = this;
    // 监听设备状态
    this.deviceManager.on('deviceStateChange', (data) => {
      console.info('AccessPermission[RemoteDeviceModel] deviceStateChange data=' + JSON.stringify(data));
      switch (data.action) {
        case 0:
          self.deviceList[self.deviceList.length] = data.device;
          console.info('AccessPermission[RemoteDeviceModel] online, updated device list=' + JSON.stringify(self.deviceList));
          break;
        case 2:
          if (self.deviceList.length > 0) {
            for (var i = 0; i < self.deviceList.length; i++) {
              if (self.deviceList[i].deviceId === data.device.deviceId) {
                self.deviceList[i] = data.device;
                break;
              }
            }
          }
          console.info('AccessPermission[RemoteDeviceModel] change, updated device list=' + JSON.stringify(self.deviceList));
          self.callback();
          break;
        case 1:
          if (self.deviceList.length > 0) {
            var list = [];
            for (var i = 0; i < self.deviceList.length; i++) {
              if (self.deviceList[i].deviceId != data.device.deviceId) {
                list[i] = data.device;
              }
            }
            self.deviceList = list;
          }
          console.info('AccessPermission[RemoteDeviceModel] offline, updated device list=' + JSON.stringify(data.device));
          self.callback();
          break;
        default:
          break;
      }
    });
    this.deviceManager.on('deviceFound', (data) => {
      console.info('AccessPermission[RemoteDeviceModel] deviceFound data=' + JSON.stringify(data));
      console.info('AccessPermission[RemoteDeviceModel] deviceFound self.deviceList=' + self.deviceList);
      console.info('AccessPermission[RemoteDeviceModel] deviceFound self.deviceList.length=' + self.deviceList.length);
      for (var i = 0; i < self.discoverList.length; i++) {
        if (self.discoverList[i].deviceId === data.device.deviceId) {
          console.info('AccessPermission[RemoteDeviceModel] device founded, ignored');
          return;
        }
      }
      self.discoverList[self.discoverList.length] = data.device;
      self.callback();
    });
    this.deviceManager.on('discoverFail', (data) => {
      console.info('AccessPermission[RemoteDeviceModel] discoverFail data=' + JSON.stringify(data));
    });
    this.deviceManager.on('serviceDie', () => {
      console.error('AccessPermission[RemoteDeviceModel] serviceDie');
    });

    SUBSCRIBE_ID = Math.floor(65536 * Math.random());
    var info = {
      subscribeId: SUBSCRIBE_ID,
      mode: 0xAA,
      medium: 2,
      freq: 2,
      isSameAccount: false,
      isWakeRemote: true,
      capability: 0
    };
    console.info('AccessPermission[RemoteDeviceModel] startDeviceDiscovery ' + SUBSCRIBE_ID);
    this.deviceManager.startDeviceDiscovery(info);
  }

  // 取消设备注册
  unregisterDeviceListCallback() {
    console.info('AccessPermission[RemoteDeviceModel] stopDeviceDiscovery ' + SUBSCRIBE_ID);
    this.deviceManager.stopDeviceDiscovery(SUBSCRIBE_ID);
    this.deviceManager.off('deviceStateChange');
    this.deviceManager.off('deviceFound');
    this.deviceManager.off('discoverFail');
    this.deviceManager.off('serviceDie');
    this.deviceList = [];
  }
}