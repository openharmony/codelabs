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

import KvStoreModel from '../../../model/KvStoreModel.js';

export default {
  data: {
    dialogTitle: '',
    name: '',
    phone: '',
    contactList: [],
    listIndex: -1,
    deleteIndex: -1,
    kvStoreModel: new KvStoreModel(),
    disableValue: false
  },
  onInit() {
    this.title = this.$t('strings.world');

    // type表示操作0：插入数据、1：修改数据：2：删除数据
    this.kvStoreModel.setOnMessageReceivedListener((k, y, type) => {
      if (type === 0) {
        const obj = {
          name: y, phone: k
        };
        this.contactList.push(obj);
      } else if (type === 1) {
        this.listIndex = this.getIndex(k);
        this.contactList[this.listIndex].name = y;
      } else if (type === 2) {
        this.deleteIndex = this.getIndex(k);
        this.contactList.splice(this.deleteIndex, 1);
      }
    });
  },

  getIndex(key) {
    for (let i = 0; i < this.contactList.length; i++) {
      if (this.contactList[i].phone === key) {
        return i;
      }
    }
  },

  addContact() {
    this.dialogTitle = '添加信息';
    this.disableValue = false;
    this.showDialog();
  },
  // 展示添加或编辑信息弹窗
  showDialog() {
    this.$element('addDialog').show();
  },
  // 关闭添加或编辑信息弹窗
  cancelDialog() {
    this.name = '';
    this.phone = '';
    this.$element('addDialog').close();
  },
  confirmClick() {
    if (!this.checkInput()) {
      return;
    }
    if (this.dialogTitle === '添加信息') {
      const obj = {
        name: this.name, phone: this.phone
      };
      this.contactList.push(obj);
      this.kvStoreModel.broadcastMessage(this.phone, this.name);
    } else {
      this.contactList[this.listIndex].name = this.name;
    }
    this.cancelDialog();
  },
  // 校验数据
  checkInput() {
    if (this.name === '' || this.name.trim() === '') {
      this.showErrorMessage('name', '姓名不能为空');
      return false;
    } else if (this.phone === '' || this.phone.trim() === '') {
      this.showErrorMessage('phone', '手机号码不能为空');
      return false;
    } else if (!this.name.match('^([\\u4e00-\\u9fa5]{1,20}|[a-zA-Z\\.\\s]{1,20})$')) {
      this.showErrorMessage('name', '姓名填写内容不正确');
      return false;
    } else if (this.dialogTitle === '添加信息' && this.phoneIsExist()) {
      this.showErrorMessage('phone', '手机号码已经存在了');
      return false;
    } else if (!this.phone.match('^\\d{1,11}$')) {
      this.showErrorMessage('phone', '手机号码填写内容不正确');
      return false;
    } else {
      return true;
    }
  },
  // 验证电话号码是否存在
  phoneIsExist() {
    for (let i = 0; i < this.contactList.length; i++) {
      if (this.phone === this.contactList[i].phone) {
        return true;
      }
    }
    return false;
  },
  changeName(e) {
    this.name = e.value;
  },
  changePhone(e) {
    this.phone = e.value;
  },
  editContact(obj, index) {
    this.dialogTitle = '修改信息';
    this.name = obj.name;
    this.phone = obj.phone;
    this.listIndex = index;
    this.disableValue = true;
    this.kvStoreModel.broadcastMessage(this.phone, this.name);
    this.showDialog();
  },
  deleteContact(index) {
    this.showDeleteDialog();
    this.deleteIndex = index;
  },
  // 错误提示信息
  showErrorMessage(id, msg) {
    this.$element(id).showError({
      error: msg
    });
  },
  // 展示确认删除弹窗
  showDeleteDialog() {
    this.$element('deleteDialog').show();
  },
  // 关闭确认删除弹窗
  cancelDeleteDialog() {
    this.$element('deleteDialog').close();
  },
  // 确认删除
  confirmDeleteClick() {
    // 刪除分布式数据库中数据
    this.kvStoreModel.deleteData(this.contactList[this.deleteIndex].phone);
    this.contactList.splice(this.deleteIndex, 1);
    this.cancelDeleteDialog();
  }
};
