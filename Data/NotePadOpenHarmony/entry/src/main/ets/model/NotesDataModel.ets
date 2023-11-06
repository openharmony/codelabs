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

import common from '@ohos.app.ability.common';
import dataStorage from '@ohos.data.preferences';
import { FolderType, noteIdArray, NotesInfoBean, notesItemArr } from '../viewmodel/NotesInfoViewModel';
import Logger from '../common/utils/Logger';
import CommonConstants from '../common/constants/CommonConstants';

class NotesDataModel {
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;

  /**
   * Put the data in preferences.
   */
  async saveDefaultData() {
    try {
      let preferences = await dataStorage.getPreferences(this.context, CommonConstants.PREFERENCE_INSTANCE_NAME);
      let isExist = await preferences.has(CommonConstants.PREFERENCE_NOTE_KEY);
      if (!isExist) {
        preferences.put(CommonConstants.PREFERENCE_NOTE_KEY, JSON.stringify(noteIdArray));
        preferences.flush();
        notesItemArr.forEach((item: NotesInfoBean) => {
          let notes: NotesInfoBean = item;
          let res = preferences.put(item.noteId, JSON.stringify(notes));
          preferences.flush();
          res.then(() => {
            Logger.info('Put the value successfully.' + item.noteId);
          }).catch((err: Error) => {
            Logger.error(`Put the value failed with err: ${err}`);
          })
        })
      }
    } catch (err) {
      Logger.error(`Failed to get preferences. Error = ${err}`);
    }
  }

  /**
   * Get the notes by folder type.
   *
   * @param flag the folder type.
   * @param allNotes all of notes.
   * @returns subNotes.
   */
  getSelectNotes(flag: FolderType, allNotes: Array<NotesInfoBean>): Array<NotesInfoBean> {
    return allNotes.filter((item: NotesInfoBean) => item.folder === flag);
  }
}

const NotesData = new NotesDataModel();

export default NotesData;