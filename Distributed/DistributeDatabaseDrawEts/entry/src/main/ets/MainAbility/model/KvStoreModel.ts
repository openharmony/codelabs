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

const STORE_ID = 'DrawBoard_kvstore';

export default class KvStoreModel {
  kvManager: any;
  kvStore: any;

  constructor() {
  }

  createKvStore(callback: any) {
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
        var options = {
          createIfMissing: true,
          encrypt: false,
          backup: false,
          autoSync: true,
          kvStoreType: 1,
          schema: '',
          securityLevel: 3,
        };
        console.info('DrawBoard[KvStoreModel] kvManager.getKVStore begin');
        self.kvManager.getKVStore(STORE_ID, options).then((store: any) => {
          console.info('DrawBoard[KvStoreModel] getKVStore success, kvStore=' + store);
          self.kvStore = store;
          callback();
        });
        console.info('DrawBoard[KvStoreModel] kvManager.getKVStore end');
      });
      console.info('DrawBoard[KvStoreModel] createKVManager end');
    } else {
      callback();
    }
  }

  broadcastMessage(msg: any) {
    console.info('DrawBoard[KvStoreModel] broadcastMessage ' + msg);
    var num = Math.random();
    let self = this;
    this.createKvStore(() => {
      self.put(msg, num);
    });
  }

  put(key: any, value: any) {
    if (typeof (this.kvStore) === 'undefined') {
      return;
    }
    console.info('DrawBoard[KvStoreModel] kvStore.put ' + key + '=' + value);
    this.kvStore.put(key, value).then((data: any) => {
      this.kvStore.get(key).then((data:any) => {
        console.info('DrawBoard[KvStoreModel] kvStore.get ' + key + '=' + JSON.stringify(data));
      });
      console.info('DrawBoard[KvStoreModel] kvStore.put ' + key + ' finished, data=' + JSON.stringify(data));
    }).catch((err: JSON) => {
      console.error('DrawBoard[KvStoreModel] kvStore.put ' + key + ' failed, ' + JSON.stringify(err));
    });
  }

   get(key: any,callback: any) {
     this.createKvStore(() => {
       this.kvStore.get(key, function (err: any ,data: any) {
         console.log("get success data: " + data);
         callback(data);
       });
     })
   }

  setOnMessageReceivedListener(callback: any) {
    console.info('DrawBoard[KvStoreModel] setOnMessageReceivedListener ');
    let self = this;
    this.createKvStore(() => {
      console.info('DrawBoard[KvStoreModel] kvStore.on(dataChange) begin');
      self.kvStore.on('dataChange', 1, (data: any) => {
        console.info('DrawBoard[KvStoreModel] dataChange, ' + JSON.stringify(data));
        console.info('DrawBoard[KvStoreModel] dataChange, insert ' + data.insertEntries.length + ' udpate '
        + data.updateEntries.length);
        if (data.insertEntries.length < 1 && data.updateEntries.length < 1) {
          return;
        }

        callback(data);
      });
      console.info('DrawBoard[KvStoreModel] kvStore.on(dataChange) end');
    });
  }
  setDataChangeListener(callback: any) {
    let self = this;
    this.createKvStore(() => {
      self.kvStore.on('dataChange', 1, (data: any) => {
        if (data.updateEntries.length > 0) {
          callback(data);
        }
      });
    });
  }
}