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
// utils is a utility js that contains some common methods to handle data.
import utils from '../../../../../../ailife-view/utils/utils.js';
// observed is used to handle data interactions between the JS and Java and used for event notification.
import observed from '../../../../../../ailife-view/observed/observed.js';

const ABILITY_TYPE_INTERNAL = 1;
const ACTION_MESSAGE_CODE_DATACHANGED = 1004;

export default {
  data: {
    controlData: {},
    imageSrc: '',
    logoSrc: '',
    isLightOn: true,
    deviceName: ''
  },
  onInit() {
    utils.setActionParam('com.huawei.smartlamp.hmservice',
      'com.huawei.smartlamp.DataHandlerAbility', ABILITY_TYPE_INTERNAL);
  },
  async onShow() {
    this.deviceName = '智能灯控';
    this.imageSrc = '/common/images/LED.png';
    this.logoSrc = '/common/images/logo.png';
    await observed.subscribeAbility();
  },
  async reverseIconClick() {
    const data = {};
    if (this.isLightOn) {
      data['switch/on'] = 0;
      this.isLightOn = false;
    } else {
      data['switch/on'] = 1;
      this.isLightOn = true;
    }
    const action = utils.makeAction(ACTION_MESSAGE_CODE_DATACHANGED, data);
    await FeatureAbility.callAbility(action);
  }
};
