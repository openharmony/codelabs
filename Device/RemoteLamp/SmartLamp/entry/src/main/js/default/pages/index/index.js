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
import router from '@system.router';
import {getApp} from '../../common.js';

export default {
  data: {
    deviceName: '',
    deviceImg: '',
    productName: 'LED',
    sessionId: ''
  },
  onInit() {
    this.deviceName = this.$t('strings.device-name');
    this.deviceImg = '/common/images/LED.png';
    getApp(this).Product.productName = this.productName;
    getApp(this).Product.productId = this.productId;
    getApp(this).ConfigParams.sessionId = this.sessionId;
  },
  cancel() {
    app.terminate();
  },
  configNet() {
    router.push({
      uri: 'pages/netconfig/netconfig'
    });
  }
};