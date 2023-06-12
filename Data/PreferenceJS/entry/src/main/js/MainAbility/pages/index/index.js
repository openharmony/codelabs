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

import Preference from '../../common/database/preference';

export default {
  data: {
    isNoData: true,
    allDataArray: [],
    dataKey: '',
    dataValue: '',
    deleteKey: ''
  },
  async onInit() {
    await Preference.initPreferences();
    this.refreshList();
  },
  removeAll() {
    this.$element('clearDialog').show();
  },
  deleteData(key) {
    this.$element('deleteDialog').show();
    this.deleteKey = key;
  },
  addData() {
    this.dataKey = '';
    this.dataValue = '';
    this.$element('addDialog').show();
  },
  keyChange(event) {
    this.dataKey = event.value;
  },
  dataChange(event) {
    this.dataValue = event.value;
  },
  searchByKey(event) {
    let searchKey = event.text;
    if (searchKey === '') {
      this.refreshList();
    } else {
      Preference.hasDataKey(searchKey).then(hasKey => {
        if (hasKey) {
          Preference.getDataByKey(searchKey).then(dataObj => {
            if (this.allDataArray !== []) {
              this.allDataArray = [];
            }
            this.allDataArray.push(dataObj);
            this.isNoData = false;
          })
        } else {
          this.isNoData = true;
        }
      })
    }
  },
  addCancel() {
    this.$element('addDialog').close();
  },
  deleteCancel() {
    this.$element('deleteDialog').close();
  },
  clearCancel() {
    this.$element('clearDialog').close();
  },
  addConfirm() {
    if (this.dataKey === '' || this.dataValue === '') {
      Preference.showToastMessage(this.$t('strings.input_enter_check'));
      return;
    }
    this.$element('addDialog').close();
    Preference.addData(this.dataKey, this.dataValue).then(() => {
      Preference.showToastMessage(this.$t('strings.add_success'));
    });
    this.refreshList();
  },
  deleteConfirm() {
    this.$element('deleteDialog').close();
    Preference.deleteData(this.deleteKey).then(() => {
      Preference.showToastMessage(this.$t('strings.delete_success'));
    });
    this.refreshList();
  },
  clearConfirm() {
    this.$element('clearDialog').close();
    Preference.clearData().then(() => {
      Preference.showToastMessage(this.$t('strings.clear_success'));
    });
    this.refreshList();
  },
  refreshList() {
    Preference.getAllData().then(allData => {
      if (allData === null || allData.length === 0) {
        this.isNoData = true;
        return;
      }
      this.isNoData = false;
      this.allDataArray = allData;
    });
  }
}