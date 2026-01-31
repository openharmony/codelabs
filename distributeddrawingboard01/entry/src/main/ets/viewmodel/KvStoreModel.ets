/*
 * Copyright (c) 2025 OpenHarmony Foundation.
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
//分布式数据库
import distributedKVStore from '@ohos.data.distributedKVStore';
import common from '@ohos.app.ability.common';
import CommonConstants from '../common/constants//CommonConstants';
import Logger from '../common/utils/Logger';


export default class kvStoreModel {
  kvManager?: distributedKVStore.KVManager;
  kvStore?: distributedKVStore.SingleKVStore;//SingleKVStore单版本分布式键值数据库，不对数据所属设备进行区分，提供ing数据查询方法

  //创建分布式数据库
  createKvStore(
    context: common.UIAbilityContext,
    callback: (data: distributedKVStore.ChangeNotification) => void
  ): void {
    if (this.kvStore !== undefined) {
      Logger.info('KvStoreModel', 'createKvStore KVManager is exist');
      return;
    }

    let  config: distributedKVStore.KVManagerConfig = {
      bundleName: context.abilityInfo.bundleName,
      context:context
    };
    try {
      //创建一个kvmanager对象实例，用于管理数据库对象
      this.kvManager = distributedKVStore.createKVManager(config);
    } catch (error) {
      Logger.error('KvStoreModel',
        `createKvStore createKVManager failed, err=${JSON.stringify(error)}`);
      return;
    }
    //创建数据库配置信息
    let  options: distributedKVStore.Options = {
      //当前数据库文件不存在时是否创建数据库，默认为true
      createIfMissing: true,
      //设置数据库文件是否加密，默认FALSE
      encrypt: false,
      //是否备份
      backup: false,
      //是否自动同步
      autoSync: true,
      //设置数据库类型      DEVICE_COLLABORATION多设备协同
      // SingleKVStore单版本分布式键值数据库，不对数据所属设备进行区分，提供ing数据查询方法     SINGLE_VERSION
      kvStoreType: distributedKVStore.KVStoreType.SINGLE_VERSION,
      //数据库安全级别：s1低级别
      securityLevel: distributedKVStore.SecurityLevel.S1
    };

    //获取分布式简直数据库 storeId：数据库唯一标识      options配置信息
    this.kvManager.getKVStore(CommonConstants.KVSTORE_ID,options).then((store: distributedKVStore.SingleKVStore) => {
      if (store === null) {
        Logger.error('KvStoreModel', `createKvStore getKVStore store is null`);
        return;
      }
      this.kvStore = store;
      //开启同步
      this.kvStore.enableSync(true).then(() => {
        Logger.info('KvStoreModel', 'createKvStore enableSync success');
      }).catch((error: Error) => {
        Logger.error('KvStoreModel',
          `createKvStore enableSync fail, error=${JSON.stringify(error)}`);
      });
      //调用setData
      this.setDataChangeListener(callback);
    }).catch((error: Error) => {
      Logger.error('getKVStore',
        `createKvStore getKVStore failed, error=${JSON.stringify(error)}`);
    })
  }





  //添加数据
  put(key: string, value: string): void {
    if (this.kvStore === undefined) {
      return;
    }
    this.kvStore.put(key,value).then(() => {
      Logger.info('KvStoreModel', `kvStore.put key=${key} finished}`);
    }).catch((error: Error) => {
      Logger.error('KvStoreModel',
        `kvStore.put key=${key} failed, error=${JSON.stringify(error)}`);
    });
  }



  //数据变更通知
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
        })
    } catch (error) {
      Logger.error('KvStoreModel',
        `setDataChangeListener on('dataChange') failed, err=${JSON.stringify(error)}`);
    }
  }
  //应用退出时，分布式键值数据库取消数据改变监听。
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