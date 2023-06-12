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

import dataPreferences from '@ohos.data.preferences';
import featureAbility from '@ohos.ability.featureAbility';
import promptAction from '@ohos.promptAction';
import CommonConstants from '../constants/commonConstants';
import { logger } from '../utils/logger';

let preference = null;

class Preference {

  /**
   * Obtains the Preferences instance.
   */
  async initPreferences() {
    try {
      let context = featureAbility.getContext();
      preference = await dataPreferences.getPreferences(context, CommonConstants.PREFERENCES_NAME);
    } catch (err) {
      logger.error(`Failed to get preferences. code: ${err.code},message: ${err.message}`);
    }
  }

  /**
   * Write data to Preferences instance.
   *
   * @param key{String} Key properties.
   * @param value{String} Value properties.
   */
  async addData(key, value) {
    if (preference === null) {
      await this.initPreferences();
    }
    try {
      await preference.put(key, value);
      await preference.flush();
    } catch (err) {
      logger.error(`Fail to add data. code: ${err.code},message: ${err.message}`);
    }
  }

  /**
   * Deletes the storage key-value pair named the given Key from the Preferences instance.
   *
   * @param key{String} The given Key.
   */
  async deleteData(key) {
    if (preference === null) {
      await this.initPreferences();
    }
    try {
      await preference.delete(key);
      await preference.flush();
    } catch (err) {
      logger.error(`Failed to delete. code: ${err.code},message: ${err.message}`);
    }
  }

  /**
   * Clearing data.
   */
  async clearData() {
    if (preference === null) {
      await this.initPreferences();
    }
    try {
      await preference.clear();
      await preference.flush();
    } catch (err) {
      logger.error(`Failed to clear. code: ${err.code},message: ${err.message}`);
    }
  }

  /**
   * Querying All Data.
   *
   * @returns allData{Array} All key-value data.
   */
  async getAllData() {
    let allData = [];
    if (preference === null) {
      await this.initPreferences();
    }
    try {
      let value = await preference.getAll();
      let allKeys = Object.keys(value);
      let keyNum = allKeys.length;
      for (let i = 0; i < keyNum; i++) {
        let dataObj = await this.getDataByKey(allKeys[i]);
        allData.push(dataObj);
      }
    } catch (err) {
      logger.error(`Failed to query all data. code: ${err.code},message: ${err.message}`)
    }
    return allData;
  }

  /**
   * Obtains the value of the given key.
   *
   * @param key{String} The given key.
   * @returns dataObject{Object} The value.
   */
  async getDataByKey(key) {
    let dataObj = {};
    dataObj.key = key;
    if (preference === null) {
      await this.initPreferences();
    }
    try {
      dataObj.value = await preference.get(key, 'default');
    } catch (err) {
      logger.error(`Failed to get data by key. code: ${err.code},message: ${err.message}`);
    }
    return dataObj;
  }

  /**
   * Checks if the Preferences instance contains a storage key-value pair named the given Key.
   *
   * @param key{String} The given key.
   * @returns hasKey{Boolean} The check results.
   */
  async hasDataKey(key) {
    let hasKey = false;
    if (preference === null) {
      await this.initPreferences();
    }
    try {
      hasKey = await preference.has(key);
    } catch (err) {
      logger.error(`Failed to put value. code: ${err.code},message: ${err.message}`);
    }
    return hasKey;
  }

  /**
   * Message pop-up window.
   *
   * @param message{String} The message.
   */
  showToastMessage(message) {
    promptAction.showToast({
      message: message,
      duration: CommonConstants.DURATION
    });
  };
}

export default new Preference();