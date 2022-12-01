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

import router from '@system.router';
import deviceManager from '@ohos.distributedHardware.deviceManager';
import featureAbility from '@ohos.ability.featureAbility';

export default {
  data: {
    title: '',
    type: '',
    imgUrl: '',
    reads: '',
    likes: '',
    content: '',
    deviceList: [],
    deviceMag: {}
  },

  onInit() {
    if (router.getParams()) {
      console.info('router.getParams():' + JSON.stringify(router.getParams()));
    } else {
      // 从远端获取数据
      featureAbility.getWant()
        .then((want) => {
          this.title = want.parameters.title;
          this.imgUrl = want.parameters.imgUrl;
          this.reads = want.parameters.reads;
          this.likes = want.parameters.likes;
          this.content = want.parameters.content;
          console.info('featureAbility.getWant' + JSON.stringify(want));
        }).catch((error) => {
        });
    }
  },

  toShare() {
    // 创建设备管理实例
    deviceManager.createDeviceManager('com.huawei.newsdemooh', (err, data) => {
      if (err) {
        return;
      }
      this.deviceMag = data;
      // 获取所有可信设备的列表
      this.deviceList = this.deviceMag.getTrustedDeviceListSync();
    });
    // 循环遍历设备列表,获取设备名称和设备Id
    for (let i = 0; i < this.deviceList.length; i++) {
      this.deviceList[i] = {
        deviceName: this.deviceList[i].deviceName,
        deviceId: this.deviceList[i].deviceId,
        checked: false
      };
    }
    this.$element('showDialog').show();
  },

  chooseCancel() {
    this.$element('showDialog').close();
  },

  chooseComform() {
    this.$element('showDialog').close();
    for (let i = 0; i < this.deviceList.length; i++) {
      // 判断设备是否被选中
      if (this.deviceList[i].checked) {
        const params = {
          url: 'pages/detail/detail',
          title: this.title,
          type: this.type,
          imgUrl: this.imgUrl,
          reads: this.reads,
          likes: this.likes,
          content: this.content
        };

        const wantValue = {
          bundleName: 'com.huawei.newsdemooh',
          abilityName: 'com.huawei.newsdemooh.MainAbility',
          deviceId: this.deviceList[i].deviceId,
          parameters: params
        };

        featureAbility.startAbility({
          want: wantValue
        }).then((data) => {
          console.info('featureAbility.startAbility finished, ' + JSON.stringify(data));
        });
        console.info('featureAbility.startAbility want=' + JSON.stringify(wantValue));
        console.info('featureAbility.startAbility end');
      }
    }
  },

  selectDevice(index, e) {
    this.deviceList[index].checked = e.checked;
  },

  onDestroy() {
    if (this.deviceMag !== null && JSON.stringify(this.deviceMag) !== '{}') {
      this.deviceMag.release();
    }
  }
};
