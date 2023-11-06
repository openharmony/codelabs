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

import router from '@ohos.router';
import common from '@ohos.app.ability.common';
import dataStorage from '@ohos.data.preferences';
import StyleConstants from '../common/constants/StyleConstants';
import CommonConstants from '../common/constants/CommonConstants';
import { NoteIdBean, NotesInfoBean } from '../viewmodel/NotesInfoViewModel';

@CustomDialog
export default struct ConfirmDialog {
  @Link noteIdArray: NoteIdBean[];
  @Link notesInfo: NotesInfoBean;
  @Link operationType: string;
  controller?: CustomDialogController;
  cancel?: () => void;
  confirm?: () => void;
  type: string = '';
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;

  build() {
    Column() {
      Text(this.type === CommonConstants.SAVE_DIALOG ? $r('app.string.save_update'):$r('app.string.delete_update'))
        .fontSize($r('app.float.font_size_normal'))
        .height($r('app.float.dialog_title_height'))
        .margin({ top: $r('app.float.dialog_title_margin_top'), bottom: $r('app.float.dialog_title_margin_bottom') })
      Row() {
        Text($r('app.string.cancel_txt'))
          .onClick(() => {
            if (this.controller === undefined) {
              return;
            }
            this.controller.close();
            this.cancel = () => {
              router.replaceUrl({
                url: 'pages/NoteHomePage'
              });
            }
            this.cancel();
          })
          .width($r('app.float.dialog_button_width'))
          .fontColor($r('app.color.cancel_confirm_font_color'))
          .fontSize($r('app.float.font_size_normal'))
          .textAlign(TextAlign.Center)
        Divider().vertical(true).height(24)
        Text(this.type === CommonConstants.SAVE_DIALOG ?
          $r('app.string.confirm_txt') : $r('app.string.delete_txt'))
          .width($r('app.float.dialog_button_width'))
          .onClick(async () => {
            if (this.controller === undefined) {
              return;
            }
            this.controller.close();
            if (this.type === CommonConstants.SAVE_DIALOG) {
              this.confirm = async () => {
                let preferences = await dataStorage.getPreferences(this.context, CommonConstants.PREFERENCE_INSTANCE_NAME);
                if (this.operationType === CommonConstants.ADD_NOTE) {
                  this.noteIdArray.push(new NoteIdBean(this.notesInfo.noteId));
                  preferences.put(CommonConstants.PREFERENCE_NOTE_KEY, JSON.stringify(this.noteIdArray));
                  preferences.flush();
                }
                let newNotes = this.notesInfo;
                await preferences.put(this.notesInfo.noteId, JSON.stringify(newNotes));
                await preferences.flush();
                router.replaceUrl({
                  url: 'pages/NoteHomePage'
                });
              }
            } else {
              this.confirm = async () => {
                let preferences = await dataStorage.getPreferences(this.context, CommonConstants.PREFERENCE_INSTANCE_NAME);
                await preferences.delete(this.notesInfo.noteId);
                await preferences.flush();
                router.replaceUrl({
                  url: 'pages/NoteHomePage'
                });
              }
            }
            this.confirm();
          })
          .textAlign(TextAlign.Center)
          .fontColor(this.type === CommonConstants.SAVE_DIALOG ?
            $r('app.color.cancel_confirm_font_color'): $r('app.color.delete_font_color'))
          .fontSize($r('app.float.font_size_normal'))
      }
      .justifyContent(FlexAlign.SpaceAround)
    }
    .width(StyleConstants.DIALOG_CONTENT_WIDTH)
    .height($r('app.float.dialog_height'))
  }

  async deleteNoteIdArray(): Promise<void> {
    let preferences = await dataStorage.getPreferences(this.context, CommonConstants.PREFERENCE_INSTANCE_NAME);
    await preferences.delete(this.notesInfo.noteId);
    await preferences.flush();
    router.replaceUrl({
      url: 'pages/NoteHomePage'
    });
  }
}