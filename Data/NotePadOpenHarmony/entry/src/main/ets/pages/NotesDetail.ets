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
import StyleConstants from '../common/constants/StyleConstants';
import CommonConstants from '../common/constants/CommonConstants';
import ConfirmDialog from '../view/ConfirmDialog';
import { FolderType, noteIdArray, NoteIdBean, NotesInfoBean } from '../viewmodel/NotesInfoViewModel';
import formatTime from '../common/utils/Format';
import BottomBar from '../view/BottomBar';

@Entry
@Component
struct NotesDetail {
  @State notesInfo: NotesInfoBean = (router.getParams() as Record<string, NotesInfoBean>)['notesInfo'];
  @State operationType: string = (router.getParams() as Record<string, string>)['operationType'];
  @State imageArr: string[] = this.notesInfo.imageArr;
  @State noteIdArray: NoteIdBean[] = noteIdArray;
  @State @Watch('onChangeCollect') isFavorite: boolean = this.notesInfo.isFavorite;
  @State notesFolder: FolderType = this.notesInfo.folder;
  @State isDataChanged: boolean = false;
  @State isCollectChange: boolean = false;

  @Builder MyMenu() {
    Menu() {
      MenuItem({ content: $r('app.string.notes_personal') })
        .onChange(() => {
          this.notesInfo.folder = FolderType.Personal;
        })
      MenuItem({ content: $r('app.string.notes_work') })
        .onChange(() => {
          this.notesInfo.folder = FolderType.Work;
        })
    }
  }
  scroller: Scroller = new Scroller();
  saveDialogController: CustomDialogController = new CustomDialogController({
    builder: ConfirmDialog({
      notesInfo: $notesInfo,
      operationType: $operationType,
      noteIdArray: $noteIdArray,
      type: CommonConstants.SAVE_DIALOG
    }),
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    offset: { dx: $r('app.float.dialog_offset_x'), dy: $r('app.float.dialog_margin_bottom') }
  });

  build() {
    Column() {
      Row() {
        Image($r('app.media.ic_public_back'))
          .width($r('app.float.back_icon_size'))
          .aspectRatio(1)
          .objectFit(ImageFit.Cover)
          .onClick(() => {
            if (this.isDataChanged || this.notesFolder !== this.notesInfo.folder || this.isCollectChange) {
              this.saveDialogController.open();
            } else {
              router.replaceUrl({
                url: 'pages/NoteHomePage'
              });
            }
          })
        Text($r('app.string.back_text'))
          .fontSize($r('app.float.font_size_normal'))
          .margin({ left: $r('app.float.back_icon_margin') })
      }
      .height($r('app.float.top_bar_container_height'))
      .width(StyleConstants.FULL_WIDTH)
      .padding({ left: $r('app.float.notes_container_padding') })
      Stack({ alignContent: Alignment.Bottom }) {
        Scroll(this.scroller) {
          Column() {
            TextInput({
              text: this.notesInfo.title != '' ? this.notesInfo.title : '',
              placeholder: this.notesInfo.title != '' ? '' : $r('app.string.note_title_placeholder')
            })
              .width(StyleConstants.TEXT_INPUT_WIDTH)
              .height($r('app.float.text_input_height'))
              .fontSize($r('app.float.font_size_larger'))
              .placeholderFont({ size: $r('app.float.font_size_larger') })
              .placeholderColor($r('app.color.placeholder_color'))
              .backgroundColor($r('app.color.white_color'))
              .onChange((value: string) => {
                if (value !== this.notesInfo.title) {
                  this.notesInfo.title = value;
                  this.isDataChanged = true;
                }
              })
            Row() {
              Text(this.notesInfo.time)
                .fontSize($r('app.float.font_size_smaller'))
                .fontColor($r('app.color.font_color_gray'))
              Row() {
                Text(this.notesInfo.folder === FolderType.Personal ?
                $r('app.string.notes_personal') : $r('app.string.notes_work'))
                  .fontSize($r('app.float.font_size_little'))
                  .fontColor($r('app.color.placeholder_color'))
                  .textAlign(TextAlign.Center)
                Image($r('app.media.ic_public_spinner_small'))
                  .width($r('app.float.ic_public_spinner_width'))
                  .height($r('app.float.ic_public_spinner_height'))
                  .objectFit(ImageFit.Contain)
              }
              .bindMenu(this.MyMenu)
              .height($r('app.float.select_bar_height'))
              .width($r('app.float.select_bar_width'))
              .borderRadius($r('app.float.select_bar_border_radius'))
              .justifyContent(FlexAlign.Center)
              .alignItems(VerticalAlign.Center)
              .margin({ left: $r('app.float.select_bar_margin_left') })
              .backgroundColor($r('app.color.select_bar_background_color'))
            }
            .height($r('app.float.select_bar_height'))
            .margin({ bottom: $r('app.float.select_bar_margin_bottom') })
            .width(StyleConstants.CONTENT_WIDTH)
            TextArea({
              text: this.notesInfo.content !== '' ? this.notesInfo.content : '',
              placeholder: this.notesInfo.content !== '' ? '' : $r('app.string.note_content_placeholder')
            })
              .onChange((value: string) => {
                if (value !== this.notesInfo.content) {
                  this.notesInfo.content = value;
                  this.isDataChanged = true;
                }
              })
              .width(StyleConstants.CONTENT_WIDTH)
              .fontSize($r('app.float.font_size_normal'))
              .placeholderFont({ size: $r('app.float.font_size_normal') })
              .placeholderColor($r('app.color.placeholder_color'))
              .backgroundColor($r('app.color.select_bar_background_color'))
              .padding({ bottom: $r('app.float.text_area_padding_bottom') })
            ForEach(this.imageArr, (item: string) => {
              Image($rawfile(item))
                .height($r('app.float.note_picture_height'))
                .width(StyleConstants.CONTENT_WIDTH)
                .objectFit(ImageFit.Fill)
                .margin({ top: $r('app.float.image_Space') })
            }, (item: string) => JSON.stringify(item))
          }
        }
        .scrollable(ScrollDirection.Vertical)
        .scrollBar(BarState.Off)
        .edgeEffect(EdgeEffect.None)
        .align(Alignment.Top)
        .height(StyleConstants.FULL_HEIGHT)
        .margin({ bottom: $r('app.float.top_bar_container_height') })

        BottomBar({
          imageArr: $imageArr,
          notesInfo: $notesInfo,
          operationType: $operationType,
          noteIdArray: $noteIdArray,
          isDataChanged: $isDataChanged,
          isFavorite: $isFavorite
        })
      }
      .width(StyleConstants.FULL_WIDTH)
      .height(StyleConstants.FULL_HEIGHT)
      .padding({ bottom: $r('app.float.top_bar_container_height') })
      .backgroundColor($r('app.color.white_color'))
    }
    .height(StyleConstants.FULL_HEIGHT)
    .backgroundColor($r('app.color.white_color'))
  }

  onPageShow() {
    this.notesInfo.time = formatTime();
  }

  onChangeCollect() {
    this.isCollectChange = !this.isCollectChange;
  }

  onBackPress() {
    if (this.isDataChanged || this.notesFolder !== this.notesInfo.folder || this.isCollectChange) {
      this.saveDialogController.open();
    } else {
      router.replaceUrl({
        url: 'pages/NoteHomePage'
      });
    }
    return true;
  }
}