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
        id:0,
        name: '李四',
        nameAge: 18,
        gender:'男',
        contactList: [],
        listIndex: -1,
        deleteIndex: -1,
        kvStoreModel: new KvStoreModel(),
    },
    onInit() {
        this.title = this.$t('strings.world');
    },

    addContact() {
        this.dialogTitle = '添加信息';
        this.showDialog();
    },

    queryContact() {
        this.kvStoreModel.queryValue((contactList) => {
            console.info('*****************lianghao number    ' + contactList.length);
            if (contactList.length > 0) {
                this.contactList = contactList;
            }
        });
        //this.showDialog();
    },
    // 展示添加或编辑信息弹窗
    showDialog() {
        this.$element('addDialog').show();
    },
    // 关闭添加或编辑信息弹窗
    cancelDialog() {
        this.name = '';
        this.nameAge = '';
        this.gender = '';
        this.$element('addDialog').close();
    },
    confirmClick() {
        if (this.dialogTitle === '添加信息') {
            const obj = {
                name: this.name,age:this.nameAge,gender:this.gender
            };
            this.contactList.push(obj);
            this.kvStoreModel.insertValue(obj);
        } else {
            this.contactList[this.listIndex].name = this.name;
            this.contactList[this.listIndex].age = this.nameAge;
            this.contactList[this.listIndex].gender = this.gender;
            const obj = {
                name: this.name,age:this.nameAge,gender:this.gender
            };
            this.kvStoreModel.updateValue(obj,this.id);
        }
        this.queryContact();
        this.cancelDialog();
    },
    changeName(e) {
        this.name = e.value;
    },
    changeAge(e) {
        this.nameAge = e.value;
    },
    changeGender(e) {
        this.gender = e.value;
    },
    editContact(obj,index) {
        this.dialogTitle = '修改信息';

        this.listIndex = index;
        this.id = obj.id;
        this.name = obj.name;
        this.nameAge = obj.age;
        this.gender = obj.gender;
        //修改数据
        this.showDialog();
    },
    deleteContact(obj,index) {
        this.showDeleteDialog();
        this.id = obj.id;
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
        this.contactList.splice(this.deleteIndex, 1);
        this.kvStoreModel.deleteValue(this.id);
        this.cancelDeleteDialog();
    }
};
