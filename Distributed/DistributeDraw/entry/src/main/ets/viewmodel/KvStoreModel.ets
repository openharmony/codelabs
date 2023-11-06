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

import distributedKVStore from '@ohos.data.distributedKVStore';
import common from '@ohos.app.ability.common';
import CommonConstants from '../common/constants//CommonConstants';
import Logger from '../common/utils/Logger';

export default class KvStoreModel {
  kvManager?: distributedKVStore.KVManager;
  kvStore?: distributedKVStore.SingleKVStore;

  /**
   * Create a distributed key-value database.
   *
   * @param context Ability context.
   * @param callback Callback.
   */
  createKvStore(
    context: common.UIAbilityContext,
    callback: (data: distributedKVStore.ChangeNotification) => void
  ): void {
    if (this.kvStore !== undefined) {
      Logger.info('KvStoreModel', 'createKvStore KVManager is exist');
      return;
    }

    let config: distributedKVStore.KVManagerConfig = {
      bundleName: context.abilityInfo.bundleName,
      context: context
    };
    try {
      this.kvManager = distributedKVStore.createKVManager(config);
    } catch (error) {
      Logger.error('KvStoreModel',
        `createKvStore createKVManager failed, err=${JSON.stringify(error)}`);
      return;
    }

    let options: distributedKVStore.Options = {
      createIfMissing: true,
      encrypt: false,
      backup: false,
      autoSync: true,
      kvStoreType: distributedKVStore.KVStoreType.SINGLE_VERSION,
      securityLevel: distributedKVStore.SecurityLevel.S1
    };

    this.kvManager.getKVStore(CommonConstants.KVSTORE_ID, options).then((store: distributedKVStore.SingleKVStore) => {
      if (store === null) {
        Logger.error('KvStoreModel', `createKvStore getKVStore store is null`);
        return;
      }
      this.kvStore = store;
      this.kvStore.enableSync(true).then(() => {
        Logger.info('KvStoreModel', 'createKvStore enableSync success');
      }).catch((error: Error) => {
        Logger.error('KvStoreModel',
          `createKvStore enableSync fail, error=${JSON.stringify(error)}`);
      });
      this.setDataChangeListener(callback);
    }).catch((error: Error) => {
      Logger.error('getKVStore',
        `createKvStore getKVStore failed, error=${JSON.stringify(error)}`);
    })
  }

  /**
   * Add data to the distributed key-value database.
   *
   * @param key Store key name.
   * @param value Store value.
   */
  put(key: string, value: string): void {
    if (this.kvStore === undefined) {
      return;
    }

    this.kvStore.put(key, value).then(() => {
      Logger.info('KvStoreModel', `kvStore.put key=${key} finished}`);
    }).catch((error: Error) => {
      Logger.error('KvStoreModel',
        `kvStore.put key=${key} failed, error=${JSON.stringify(error)}`);
    });
  }

  /**
   * Set the data change listening function.
   *
   * @param callback Callback.
   */
  setDataChangeListener(callback: (data: distributedKVStore.ChangeNotification) => void): void {
    if (this.kvStore === undefined) {
      Logger.error('KvStoreModel', 'setDataChangeListener kvStore is null');
      return;
    }

    try {
      this.kvStore.on('dataChange', distributedKVStore.SubscribeType.SUBSCRIBE_TYPE_ALL,
        (data: distributedKVStore.ChangeNotification) => {
          if ((data.updateEntries.length > 0) || (data.insertEntries.length > 0)) {
            callback(data);
          }
        });
    } catch (error) {
      Logger.error('KvStoreModel',
        `setDataChangeListener on('dataChange') failed, err=${JSON.stringify(error)}`);
    }
  }

  /**
   * Remove the data change listener.
   */
  removeDataChangeListener(): void {
    if (this.kvStore === undefined) {
      return;
    }

    try {
      this.kvStore.off('dataChange');
    } catch (error) {
      Logger.error('KvStoreModel',
        `removeDataChangeListener off('dataChange') failed, err=${JSON.stringify(error)}`);
    }
  }
}