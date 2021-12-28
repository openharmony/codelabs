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

import prompt from '@system.prompt';
import data_storage from '@ohos.data.storage';

export default {
  data: {
    title: '',
    key: '',
    value: '',
    tableData: [
    ],
    defaultResult: 'Not found',
    path: '/data/accounts/account_0/appdata/com.huawei.cookbook/pdb'
  },
  onInit() {
    data_storage.deleteStorageSync(this.path);
  },

  // 文本框内容发生变化
  change(e) {
    const idName = e.target.id;
    if (idName === 'key') {
      this.key = e.value;
    } else if (idName === 'value') {
      this.value = e.value;
    }
  },

  // 提交
  buttonClickAdd() {
    if (this.key !== '' && this.value !== '') {
      const store = data_storage.getStorageSync(this.path);
      const ret = store.getSync(this.key, this.defaultResult);
      const data = store.putSync(this.key, this.value);
      store.flushSync();
      if (ret === this.defaultResult) {
        this.showPrompt('Add Success!');
        this.tableData.push({KEY: this.key, VALUE: this.value});
      } else {
        this.tableData = this.tableData.filter(item => item.KEY !== this.key);
        this.tableData.push({KEY: this.key, VALUE: this.value});
        this.showPrompt('Update!');
      }
    } else {
      this.showPrompt('Key or value is Empty!');
    }
  },

  buttonClickQuery() {
    if (this.key !== '') {
      const store = data_storage.getStorageSync(this.path);
      const ret = store.getSync(this.key, this.defaultResult);
      this.showPrompt(ret);
    } else {
      this.showPrompt('Key is Empty!');
    }
  },

  buttonClickDel() {
    if (this.key !== '') {
      const store = data_storage.getStorageSync(this.path);
      const ret = store.hasSync(this.key);
      if (ret) {
        store.deleteSync(this.key);
        this.tableData = this.tableData.filter(item => item.KEY !== this.key);
        this.showPrompt('Del Sucess');
      } else {
        this.showPrompt('Error, The KEY is not exist');
      }
    } else {
      this.showPrompt('Error, The KEY is Empty!');
    }
  },
  buttonClickDelDatabase() {
    data_storage.deleteStorageSync(this.path);
    this.tableData = [];
    this.showPrompt('database is delete!');
  },
  // 弹框
  showPrompt(msg) {
    prompt.showToast({
      message: msg,
      duration: 3000
    });
  }
};

