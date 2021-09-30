/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import app from '@system.app';

import featureAbility from '@ohos.ability.featureAbility';
import RemoteDeviceModel from '../../../model/RemoteDeviceModel.js';

let DEVICE_LIST_LOCALHOST;

export default {
  data: {
    title: '',
    deviceList: [],
    remoteDeviceModel: new RemoteDeviceModel()
  },
  onInit() {
    console.info('onInit begin');
    DEVICE_LIST_LOCALHOST = {
      name: this.$t('strings.localhost'),
      id: 'localhost'
    };
    this.deviceList = [DEVICE_LIST_LOCALHOST];
    console.info('onInit end');
  },

  onContinueAbilityClick() {
    console.info('onContinueAbilityClick begin');
    const self = this;
    this.remoteDeviceModel.registerDeviceListCallback(() => {
      console.info('registerDeviceListCallback, callback entered');
      const list = [];
      list[0] = DEVICE_LIST_LOCALHOST;
      let deviceList;
      if (self.remoteDeviceModel.discoverList.length > 0) {
        deviceList = self.remoteDeviceModel.discoverList;
      } else {
        deviceList = self.remoteDeviceModel.deviceList;
      }
      console.info('on remote device updated, count=' + deviceList.length);
      for (let i = 0; i < deviceList.length; i++) {
        console.info('device ' + i + '/' + deviceList.length + ' deviceId='
                + deviceList[i].deviceId + ' deviceName=' + deviceList[i].deviceName + ' deviceType='
                + deviceList[i].deviceType);
        list[i + 1] = {
          name: deviceList[i].deviceName,
          id: deviceList[i].deviceId
        };
      }
      self.deviceList = list;
    });
    this.$element('continueAbilityDialog').show();
    console.info('onContinueAbilityClick end');
  },
  startAbilityContinuation(deviceId, deviceName) {
    this.$element('continueAbilityDialog').close();
    console.info('featureAbility.startAbility deviceId=' + deviceId
        + ' deviceName=' + deviceName);
    const wantValue = {
      bundleName: 'com.huawei.cookbook',
      abilityName: 'com.huawei.cookbook.MainAbility',
      deviceId: deviceId
    };

    featureAbility.startAbility({
      want: wantValue
    }).then((data) => {
      console.info('featureAbility.startAbility finished, ' + JSON.stringify(data));
    });
    console.info('featureAbility.startAbility want=' + JSON.stringify(wantValue));
    console.info('featureAbility.startAbility end');
  },
  onRadioChange(inputValue, e) {
    console.info('onRadioChange ' + inputValue + ', ' + e.value);
    if (inputValue === e.value) {
      if (e.value === 'localhost') {
        this.$element('continueAbilityDialog').close();
        return;
      }
      if (this.remoteDeviceModel.discoverList.length > 0) {
        console.info('continue to unauthed device');
        let name = null;
        for (let i = 0; i < this.remoteDeviceModel.discoverList.length; i++) {
          if (this.remoteDeviceModel.discoverList[i].deviceId === e.value) {
            name = this.remoteDeviceModel.discoverList[i].deviceName;
            break;
          }
        }
        if (name === null) {
          console.error('onRadioChange failed, can not get name from discoverList');
          return;
        }
        console.info('onRadioChange name=' + name);

        const self = this;
        this.remoteDeviceModel.authDevice(e.value, () => {
          console.info('auth and online finished');
          for (let i = 0; i < self.remoteDeviceModel.deviceList.length; i++) {
            if (self.remoteDeviceModel.deviceList[i].deviceName === name) {
              this.startAbilityContinuation(self.remoteDeviceModel.deviceList[i].deviceId, self.remoteDeviceModel.deviceList[i].deviceName);
            }
          }
        });
      } else {
        console.info('continue to authed device');
        for (let i = 0; i < this.remoteDeviceModel.deviceList.length; i++) {
          if (this.remoteDeviceModel.deviceList[i].deviceId === e.value) {
            this.startAbilityContinuation(this.remoteDeviceModel.deviceList[i].deviceId, this.remoteDeviceModel.deviceList[i].deviceName);
          }
        }
      }
    }
  },
  onDestroy() {
    console.info('onDestroy begin');
    this.remoteDeviceModel.unregisterDeviceListCallback();
    console.info('onDestroy end');
  },
  cancelDialog() {
    this.remoteDeviceModel.unregisterDeviceListCallback();
  },
  onDismissDialogClicked() {
    this.dismissDialog();
  },
  dismissDialog() {
    this.$element('continueAbilityDialog').close();
    this.remoteDeviceModel.unregisterDeviceListCallback();
  },
  touchMove(e) {
    if (e.direction === 'right') {
      this.appExit();
    }
  },
  appExit() {
    app.terminate();
  }
};
