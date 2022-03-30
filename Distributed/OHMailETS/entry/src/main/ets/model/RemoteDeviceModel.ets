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


export default class RemoteDeviceModel {
  deviceList = [];
  discoverList = [];
  localDeviceId = '';
  callback;
  authCallback = null;
  #deviceManager;

  constructor() {
  }

  registerDeviceListCallback(callback) {
    if (typeof (this.#deviceManager) === 'undefined') {
      console.log('[pictureGame] deviceManager.createDeviceManager begin');
      let self = this;
      deviceManager.createDeviceManager('com.pictureGame.pictureGame', (error, value) => {
        if (error) {
          console.error('createDeviceManager failed.');
          return;
        }
        self.#deviceManager = value;
        self.registerDeviceListCallback_(callback);
        console.log('[pictureGame] createDeviceManager callback returned, error=' + error + ' value=' + value);
      });
      console.log('[pictureGame] deviceManager.createDeviceManager end');

    } else {
      this.registerDeviceListCallback_(callback);
    }
  }

  registerDeviceListCallback_(callback) {
    console.info('[pictureGame] registerDeviceListCallback');
    this.callback = callback;
    if (this.#deviceManager == undefined) {
      console.error('[pictureGame] deviceManager has not initialized');
      this.callback();
      return;
    }

    console.info('[pictureGame] getTrustedDeviceListSync begin');
    var list = this.#deviceManager.getTrustedDeviceListSync();
    // 获取本地设备id，现阶段不可用,需等API版本可用以及系统支持后放开即可
    //this.localDeviceId = this.#deviceManager.getLocalDeviceInfoSync().deviceId
    console.info('[pictureGame] getLocalDeviceInfoSync deviceId is :' + this.localDeviceId);
    console.info('[pictureGame] getTrustedDeviceListSync end, deviceList=' + JSON.stringify(list));
    if (typeof (list) != 'undefined' && typeof (list.length) != 'undefined') {
      this.deviceList = list;
    }
    this.callback();
    console.info('[pictureGame] callback finished');

    let self = this;
    this.#deviceManager.on('deviceStateChange', (data) => {
      console.info('[pictureGame] deviceStateChange data=' + JSON.stringify(data));
      switch (data.action) {
        case 0:
          self.deviceList[self.deviceList.length] = data.device;
          console.info('[pictureGame] online, updated device list=' + JSON.stringify(self.deviceList));
          self.callback();
          if (self.authCallback != null) {
            self.authCallback();
            self.authCallback = null;
          }
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
          console.info('[pictureGame] change, updated device list=' + JSON.stringify(self.deviceList));
          self.callback();
          break;
        case 1:
          if (self.deviceList.length > 0) {
            var list = [];
            for (var j = 0; j < self.deviceList.length; j++) {
              if (self.deviceList[j].deviceId != data.device.deviceId) {
                list[j] = data.device;
              }
            }
            self.deviceList = list;
          }
          console.info('[pictureGame] offline, updated device list=' + JSON.stringify(data.device));
          self.callback();
          break;
        default:
          break;
      }
    });
    this.#deviceManager.on('deviceFound', (data) => {
      console.info('[pictureGame] deviceFound data=' + JSON.stringify(data));
      console.info('[pictureGame] deviceFound self.deviceList=' + self.deviceList);
      console.info('[pictureGame] deviceFound self.deviceList.length=' + self.deviceList.length);
      for (var i = 0; i < self.discoverList.length; i++) {
        if (self.discoverList[i].deviceId === data.device.deviceId) {
          console.info('[pictureGame] device founded, ignored');
          return;
        }
      }
      self.discoverList[self.discoverList.length] = data.device;
      self.callback();
    });
    this.#deviceManager.on('discoverFail', (data) => {
      console.info('[pictureGame] discoverFail data=' + JSON.stringify(data));
    });
    this.#deviceManager.on('serviceDie', () => {
      console.error('[pictureGame] serviceDie');
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
    console.info('[pictureGame] startDeviceDiscovery ' + SUBSCRIBE_ID);
    this.#deviceManager.startDeviceDiscovery(info);
  }

  authDevice(deviceId, callback) {
    console.info('[pictureGame] authDevice ' + deviceId);
    for (var i = 0; i < this.discoverList.length; i++) {
      if (this.discoverList[i].deviceId === deviceId) {
        console.info('[pictureGame] device founded, ignored');
        let extraInfo = {
          "targetPkgName": 'ohos.samples.etsDemo',
          "appName": 'DMS',
          "appDescription": 'Ability and Service',
          "business": '0'
        };
        let authParam = {
          "authType": 1,
          "appIcon": '',
          "appThumbnail": '',
          "extraInfo": extraInfo
        };
        console.info('[pictureGame] authenticateDevice ' + JSON.stringify(this.discoverList[i]));
        let self = this;
        this.#deviceManager.authenticateDevice(this.discoverList[i], authParam, (err, data) => {
          if (err) {
            console.info('[pictureGame] authenticateDevice failed, err=' + JSON.stringify(err));
            self.authCallback = null;
          } else {
            console.info('[pictureGame] authenticateDevice succeed, data=' + JSON.stringify(data));
            self.authCallback = callback;
          }
        });
      }
    }
  }
}