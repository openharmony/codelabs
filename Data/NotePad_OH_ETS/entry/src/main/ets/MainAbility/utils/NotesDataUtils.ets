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

import {FolderType, NotesInfo, notesInfoArr, noteIdArray} from '../model/NotesInfo'
import dataStorage from '@ohos.data.storage'
import featureAbility from '@ohos.ability.featureAbility'

let logTag = "[NotesLog NotesDataUtils]"

export async function saveDefaultData() {
  var context = featureAbility.getContext()
  var path = await context.getFilesDir()
  let storage = dataStorage.getStorageSync(path + '/mystore')

  console.info(logTag + "saveDefaultData start ...")

  storage.has('noteIdArr', function (err, isExist) {
    console.info(logTag + "storage has noteIdArr is " + isExist)
    if (!isExist) {
      console.info(logTag + "saveDefaultData donothas ...")
      let promise = storage.put("noteIdArr", JSON.stringify(noteIdArray))
      storage.flush()
      promise.then(() => {
        console.info(logTag + "Put the value successfully. " + JSON.stringify(noteIdArray))
      }).catch((err) => {
        console.info(logTag + "Put the value failed with err: " + err)
      })

      notesInfoArr.forEach(item => {
        console.info(logTag + "----------- putData -----------" + item.noteId)
        let promise = storage.put(item.noteId, JSON.stringify(new NotesInfo(item.noteId, item.title, item.folder, item.time, item.isFavorite, item.imageArr, item.content)))
        storage.flush()
        promise.then(() => {
          console.info(logTag + "Put the value successfully." + item.noteId)
        }).catch((err) => {
          console.info(logTag + "Put the value failed with err: " + err)
        })
      })
    }
  })
}

export function getSelectNotes(flag: string, allNotes: Array<NotesInfo>): Array<NotesInfo> {
  let selectNotes: Array<NotesInfo> = []
  console.info(logTag + "----------- getSelectNotes -----------")

  allNotes.forEach(item => {
    if (item.folder === FolderType.Personal && flag === FolderType.Personal) {
      selectNotes.push(new NotesInfo(item.noteId, item.title, item.folder, item.time, item.isFavorite, item.imageArr, item.content));
    }
    if (item.folder === FolderType.Work && flag === FolderType.Work) {
      selectNotes.push(new NotesInfo(item.noteId, item.title, item.folder, item.time, item.isFavorite, item.imageArr, item.content));
    }
  })
  return selectNotes;
}