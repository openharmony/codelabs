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

import dataStorage from '@ohos.data.preferences';
import { FolderType, NotesInfoBean } from '../common/bean/NoteInfoBean';
import StyleConstants from '../common/constants/StyleConstants';
import CommonConstants from '../common/constants/CommonConstants';
import MemoItem from '../view/MemoItem';
import NotesDataModel from '../model/NotesDataModel';
import Logger from '../common/utils/Logger';

@Entry
@Component
struct NoteHomePage {
  @State folderType: Resource = $r('app.string.notes_all');
  @State allNotes: Array<NotesInfoBean> = [];
  @State selectNotes: Array<NotesInfoBean> = this.allNotes.sort();
  @Builder MyMenu(){
    Menu() {
      MenuItem({ content: $r('app.string.notes_all') })
        .onChange((selected) => {
          if(selected) {
            this.folderType = $r('app.string.notes_all');
            this.selectNotes = this.allNotes;
          }
        })
      MenuItem({ content: $r('app.string.notes_personal') })
        .onChange((selected) => {
          if(selected){
            this.folderType = $r('app.string.notes_personal');
            this.selectNotes = NotesDataModel.getSelectNotes(FolderType.Personal, this.allNotes);
          }
        })
      MenuItem({ content: $r('app.string.notes_work') })
        .onChange((selected) => {
          if(selected) {
            this.folderType = $r('app.string.notes_work');
            this.selectNotes = NotesDataModel.getSelectNotes(FolderType.Work, this.allNotes);
          }
        })
    }
  }

  build() {
    Column() {
      Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
        Column() {
          Row() {
            Row() {
              Text(this.folderType)
                .fontSize($r('app.float.font_size_larger'))
                .fontWeight(FontWeight.Normal)
                .textAlign(TextAlign.Start)
              Image($r('app.media.ic_arrow_down'))
                .objectFit(ImageFit.Contain)
                .height($r('app.float.arrow_down_height'))
                .width($r('app.float.arrow_down_width'))
                .margin({ left: $r('app.float.arrow_down_title_margin') })
            }
            .bindMenu(this.MyMenu)
            Navigator({ target: 'pages/NotesDetail', type: NavigationType.Replace }) {
              Row() {
                Image($r('app.media.ic_title_add'))
                  .objectFit(ImageFit.Contain)
                  .height($r('app.float.note_add_size'))
                  .aspectRatio(1)
              }
              .margin({ right: $r('app.float.note_add_margin') })
            }
            .params({
              notesInfo: {
                'noteId': new Date().getTime().toString(),
                'title': '',
                'folder': FolderType.Personal,
                'content': '',
                'imageArr': [],
                'time': new Date().toTimeString().split(' ')[0],
                'isFavorite': false
              },
              operationType: CommonConstants.ADD_NOTE
            })
          }
          .width(StyleConstants.CONTENT_WIDTH)
          .justifyContent(FlexAlign.SpaceBetween)
        }
        .width(StyleConstants.FULL_WIDTH)
        .height($r('app.float.top_bar_container_height'))
        .justifyContent(FlexAlign.Center)
      }
      List({ space: StyleConstants.MEMO_LIST_SPACE }) {
        ForEach(this.selectNotes, (item: NotesInfoBean) => {
          ListItem() {
            MemoItem({ noteItem: item })
          }
        }, item => JSON.stringify(item))
      }
      .margin({ top: $r('app.float.list_container_margin') })
      .height(StyleConstants.NOTE_CONTENT_HEIGHT)
      .width(StyleConstants.FULL_WIDTH)
    }
    .backgroundColor($r('app.color.page_background'))
    .height(StyleConstants.FULL_HEIGHT)
  }

  onPageShow() {
    this.getAllNotes();
  }

  async getAllNotes() {
    await NotesDataModel.saveDefaultData();
    try {
      let preferences = await dataStorage.getPreferences(globalThis.context,
        CommonConstants.PREFERENCE_INSTANCE_NAME);
      let noteIds = await preferences.get(CommonConstants.PREFERENCE_NOTE_KEY, '');
      while (this.allNotes.length >= 1) {
        this.allNotes.pop();
      }
      JSON.parse(noteIds.toString()).forEach(async (item: NotesInfoBean) => {
        let note = await preferences.get(item.noteId, '');
        this.allNotes.push(new NotesInfoBean(JSON.parse(note.toString())));
      })
    } catch (err) {
      Logger.error('Get the value of noteIdArr failed with err:', err);
    }
  }
}