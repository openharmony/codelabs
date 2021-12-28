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

import distributedData from '@ohos.data.distributeddata';

const STORE_ID = 'musicplayer_kvstore';

export default class KvStoreModel {
    kvManager;
    kvStore;

    constructor() {
    }

    createKvStore(callback) {
      if (typeof (this.kvStore) === 'undefined') {
        var config = {
          bundleName: 'com.ohos.distributedmusicplayer',
          userInfo: {
            userId: '0',
            userType: 0
          }
        };
        let self = this;
        distributedData.createKVManager(config).then((manager) => {
          self.kvManager = manager;
          var options = {
            createIfMissing: true,
            encrypt: false,
            backup: false,
            autoSync: true,
            kvStoreType: 1,
            schema: '',
            securityLevel: 3,
          };
          self.kvManager.getKVStore(STORE_ID, options).then((store) => {
            self.kvStore = store;
            callback();
          });
        });
      } else {
        callback();
      }
    }

    broadcastMessage(key, value) {
      let self = this;
      this.createKvStore(() => {
        self.put(key, value);
      });
    }

    deleteData(key) {
      let self = this;
      this.createKvStore(() => {
        self.delete(key);
      });
    }

    put(key, value) {
      this.kvStore.put(key, value).then((data) => {
        this.kvStore.get(key).then((data) => {
        });
      }).catch((err) => {
      });
    }

    delete(key) {
      this.kvStore.delete(key).then((data) => {
      }).catch((err) => {
      });
    }

    setOnMessageReceivedListener(callback) {
      let self = this;
      this.createKvStore(() => {
        self.kvStore.on('dataChange', 1, (data) => {
          for (var i = 0; i < data.insertEntries.length; i++) {
            callback(data.insertEntries[0].key, data.insertEntries[0].value.value, 0);
            return;
          }
          for (i = 0; i < data.updateEntries.length; i++) {
            callback(data.updateEntries[0].key, data.updateEntries[0].value.value, 1);
            return;
          }
          for (i = 0; i < data.deleteEntries.length; i++) {
            callback(data.deleteEntries[0].key, '', 2);
            return;
          }
        });
      });
    }
}