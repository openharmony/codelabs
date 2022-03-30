/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
import app from '@system.app';
import {getApp} from '../../common.js';

export default {
  data: {
    progress: '',
    desc: '',
    progressStatus: [],
    descStatus: [],
    failDescStatus: [],
    deviceImg: ''
  },
  onInit() {
    this.deviceImg = '/common/images/LED.png';
    this.progressStatus = [
      this.$t('strings.progress-20'),
      this.$t('strings.progress-40'),
      this.$t('strings.progress-60'),
      this.$t('strings.progress-80'),
      this.$t('strings.progress-100')
    ];
    this.descStatus = [
      this.$t('strings.desc-20'),
      this.$t('strings.desc-40'),
      this.$t('strings.desc-60'),
      this.$t('strings.desc-80'),
      this.$t('strings.desc-100')
    ];
    this.failDescStatus = [
      this.$t('strings.fail-desc-0'),
      this.$t('strings.fail-desc-20'),
      this.$t('strings.fail-desc-40'),
      this.$t('strings.fail-desc-60'),
      this.$t('strings.fail-desc-80'),
      this.$t('strings.fail-desc-100')
    ];
    this.progress = this.$t('strings.progress-0');
    this.desc = this.descStatus[0];
    this.discoverDevice();
  },
  discoverDevice() {
    const scanInfo = {
      duration: 30,
      lockTime: 60,
      sessionId: getApp(this).ConfigParams.sessionId
    };
    // Step1 discover the device through the NaN broadcast service.
    getApp(this).NetConfig.discoveryByNAN(scanInfo, (result) => {
      if (result.code == 0) {
        this.progress = this.progressStatus[1];
        this.desc = this.descStatus[1];
        getApp(this).ConfigParams.deviceInfo = result.data;
        this.registerDisconnectCallback(getApp(this).ConfigParams.deviceInfo.sessionId);
        this.connectDevice();
      } else {
        this.progress = this.progressStatus[4];
        this.desc = this.failDescStatus[1];
        this.disconnectDevice();
      }
    });
  },
  connectDevice() {
    const connectInfo = {
      targetDeviceId: getApp(this).ConfigParams.deviceInfo.productId,
      type: 0,
      pin: '0123456789012345',
      password: getApp(this).ConfigParams.deviceInfo.sn,
      sessionId: getApp(this).ConfigParams.deviceInfo.sessionId
    };
    // Step2 connect the device.
    getApp(this).NetConfig.connectDevice(connectInfo, (result) => {
      if (result.code === 0) {
        this.progress = this.progressStatus[2];
        this.desc = this.descStatus[2];
        this.getWifiInfo();
      } else {
        this.progress = this.progressStatus[4];
        this.desc = this.failDescStatus[2];
        this.disconnectDevice();
      }
    });
  },
  getWifiInfo() {
    // Step3 get the WiFi information of the current phone connection.
    getApp(this).NetConfig.getWifiList((result) => {
      if (result.code == 0 && result.data && result.data.length > 0) {
        this.progress = this.progressStatus[3];
        this.desc = this.descStatus[3];
        this.configDevice(result.data[0]);
      } else {
        this.progress = this.progressStatus[4];
        this.desc = this.descStatus[3];
        this.disconnectDevice();
      }
    });
  },
  async configDevice(wifiApInfo) {
    const netConfigInfo = {
      ssid: wifiApInfo.ssid,
      ssidPassword: '',
      isDefaultPassword: true,
      channel: wifiApInfo.channel,
      sessionId: getApp(this).ConfigParams.deviceInfo.sessionId,
      type: 0,
      wifiApId: wifiApInfo.wifiApId,
      vendorData: '',
      timeout: 30,
      paramValid: true
    };
    // Step4 config the device net.
    getApp(this).NetConfig.configDeviceNet('deviceInfo', 'accountInfo', netConfigInfo, (result) => {
      if (result.code == 0) {
        this.progress = this.progressStatus[4];
        this.desc = this.descStatus[4];
        this.registerMsgReceive();
        // Step5 config the device net success, go to the control.
        this.goToControl();
      } else {
        this.progress = this.progressStatus[4];
        this.desc = this.failDescStatus[4];
        this.disconnectDevice();
      }
    });
  },
  registerDisconnectCallback(sessionId) {
    const commonInfo = {
      sessionId: sessionId
    };
    getApp(this).NetConfig.registerDisconnectCallback(commonInfo, (result) => {
      if (result.code == 0) {
        this.desc = this.failDescStatus[5];
      }
    });
  },
  registerMsgReceive() {
    const commonInfo = {
      sessionId: getApp(this).ConfigParams.deviceInfo.sessionId
    };
    getApp(this).NetConfig.registerMsgReceive(commonInfo, () => {
    });
  },
  disconnectDevice() {
    const commonInfo = {
      sessionId: getApp(this).ConfigParams.deviceInfo.sessionId
    };
    getApp(this).NetConfig.disconnectDevice(commonInfo, (result) => {
    });
  },
  goToControl() {
    const target = {
      bundleName: 'com.huawei.smartlamp.hmservice',
      abilityName: 'com.huawei.smartlamp.ControlMainAbility',
      deviceType: 1, // 从本地设备中拉起原子化服务
      data: {
        session_id: getApp(this).ConfigParams.deviceInfo.sessionId,
        product_id: getApp(this).Product.productId,
        product_name: getApp(this).Product.productName
      }
    };
    FeatureAbility.startAbility(target);
    app.terminate();
  },
  cancel() {
    app.terminate();
  }
};