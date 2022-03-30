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
import utils from '../utils/utils.js';

const ACTION_MESSAGE_CODE_SUBSCRIBE = 1001;
const ACTION_MESSAGE_CODE_INIT_DEVICE_DATA = 1005;

export default {
  data: {
    observers: {},
    timer: null
  },
  async subscribeAbility() {
    let action = utils.makeAction(ACTION_MESSAGE_CODE_SUBSCRIBE, {});
    const that = this;
    await FeatureAbility.subscribeAbilityEvent(action, async function(callbackData) {
      let callbackJson;
      if (typeof JSON.parse(callbackData) === 'object' && 'data' in JSON.parse(callbackData)) {
        callbackJson = JSON.parse(callbackData)['data'];
      } else {
        callbackJson = JSON.parse(callbackData);
      }
      for (const key in callbackJson) {
        that.notifyObservers(key, callbackJson[key]);
      }
    });
    action = utils.makeAction(ACTION_MESSAGE_CODE_INIT_DEVICE_DATA, {});
    await FeatureAbility.callAbility(action);
  },
  notifyObservers(key, value) {
    if (Object.prototype.hasOwnProperty.call(this.data.observers, key)) {
      for (let i = 0; i < this.data.observers[key].length; i++) {
        this.data.observers[key][i].callback(key, value);
      }
    }
  }
};
