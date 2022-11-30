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

import distributedData from '@ohos.data.distributedData';

const STORE_ID = 'musicplayer_kvstore';

export default class KvStoreModel {
	  kvManager;
	  kvStore;

    constructor() {
    }

  createKvStore(callback) {
    if (typeof (this.kvStore) === 'undefined') {
      var config = {
        bundleName: 'com.huawei.cookbook',
        userInfo: {
          userId: '0',
          userType: 0
        }
      };
      let self = this;
      console.info('DrawBoard[KvStoreModel] createKVManager begin');
      distributedData.createKVManager(config).then((manager) => {
        console.info('DrawBoard[KvStoreModel] createKVManager success, kvManager=' + JSON.stringify(manager));
        self.kvManager = manager;
        let options = {
          createIfMissing: true,
          encrypt: false,
          backup: false,
          autoSync: true,
          kvStoreType: 0,
          schema: '',
          securityLevel: 1,
        };
        console.info('DrawBoard[KvStoreModel] kvManager.getKVStore begin');
        self.kvManager.getKVStore(STORE_ID, options).then((store) => {
          console.info('DrawBoard[KvStoreModel] getKVStore success, kvStore=' + store);
          self.kvStore = store;
          try {
            self.kvStore.enableSync(true).then((err) => {
              console.log('enableSync success');
            }).catch((err) => {
              console.log('enableSync fail ' + JSON.stringify(err));
            });
          }catch(e) {
            console.log('EnableSync e ' + e);
          }
          callback();
        });
        console.info('DrawBoard[KvStoreModel] kvManager.getKVStore end');
      });
      console.info('DrawBoard[KvStoreModel] createKVManager end');
    } else {
      console.info('DrawBoard[KvStoreModel] KVManager is exist');
      callback();
    }
  }

    broadcastMessage(key,value) {
      console.info('DrawBoard[KvStoreModel] kvStore.put ' + key + '=' + value);
      let self = this;
      this.createKvStore(() => {
        self.put(key,value+";");
        console.info('DrawBoard[KvStoreModel] kvStore.put ' + key + '=' + value);
      });
    }

    deleteData(key) {
      let self = this;
      this.createKvStore(() => {
        self.delete(key);
      });
    }

	put(key, value) {
      console.info('DrawBoard[KvStoreModel] kvStore.put ' + key + '=' + value);
      this.kvStore.put(key, value).then((data) => {
        this.kvStore.get(key).then((data) => {
          console.info('DrawBoard[KvStoreModel] kvStore.get ' + key + '=' + JSON.stringify(data));
        });
        console.info('DrawBoard[KvStoreModel] kvStore.put ' + key + ' finished, data=' + JSON.stringify(data));
      }).catch((err) => {
        console.error('DrawBoard[KvStoreModel] kvStore.put ' + key + ' failed, ' + JSON.stringify(err));
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
            console.info('dataChange:' + JSON.stringify(data));
          for (var i = 0; i < data.insertEntries.length; i++) {
              var str = data.insertEntries[0].value.value.toString();
              var strs = str.split(';');
            callback(data.insertEntries[0].key,strs[0], 0);
            return;
          }
          for (i = 0; i < data.updateEntries.length; i++) {
              var str = data.updateEntries[0].value.value.toString();
              var strs = str.split(';');
            callback(data.updateEntries[0].key,strs[0], 1);
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
