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

import prompt from '@system.prompt';
import CommonConstants from '../common/constants/CommonConstants';
import StyleConstants from '../common/constants/StyleConstants';
import { NoteIdBean, NotesInfoBean, notesItemArr } from '../viewmodel/NotesInfoViewModel';
import ConfirmDialog from './ConfirmDialog';

@Component
export default struct BottomBar {
  @Link notesInfo: NotesInfoBean;
  @Link operationType: string;
  @Link noteIdArray: NoteIdBean[];
  @Link imageArr: string[];
  @Link isDataChanged: boolean;
  @Link isFavorite: boolean;
  @State isCollectChange: boolean = false;
  @State collectImgSrc: string = this.isFavorite ? 'ic_bottom_star_selected.svg' : 'ic_bottom_star_normal.svg';
  @State imageUrls: string[] = ['cook_picture.png', 'dinner_food.png', 'drink_coffee.png'];
  @State noteItem: NotesInfoBean = notesItemArr[0];
  @State imageSrc: string = '';

  clickCollect?: () => void;
  clickDelete?: () => void;
  clickAddPicture?: () => void;
  deleteDialogController: CustomDialogController = new CustomDialogController({
    builder: ConfirmDialog({
      notesInfo: $notesInfo,
      operationType: $operationType,
      noteIdArray: $noteIdArray,
      type: CommonConstants.DELETE_DIALOG
    }),
    autoCancel: true,
    alignment: DialogAlignment.Bottom,
    offset: { dx: $r('app.float.dialog_offset_x'), dy: $r('app.float.dialog_margin_bottom') }
  });

  build() {
    Row() {
      Column() {
        Image($r('app.media.ic_bottom_share'))
          .width($r('app.float.ic_bottom_picture_size'))
          .aspectRatio(1)
        Text($r('app.string.share_note'))
          .fontSize($r('app.float.font_size_smallest'))
          .margin({ top: $r('app.float.bottom_txt_margin') })
      }
      .onClick(() => {
        prompt.showToast({
          message: '该功能暂未实现'
        });
      })
      Column() {
        Image($rawfile(this.collectImgSrc))
          .width($r('app.float.ic_bottom_picture_size'))
          .aspectRatio(1)
        Text($r('app.string.collect_note'))
          .fontSize($r('app.float.font_size_smallest'))
          .margin({ top: $r('app.float.bottom_txt_margin') })
      }
      .onClick(() => {
        this.clickCollect = () => {
          this.notesInfo.isFavorite = !this.notesInfo.isFavorite;
          this.isFavorite = !this.isFavorite;
          this.collectImgSrc = this.notesInfo.isFavorite ?
            'ic_bottom_star_selected.svg' : 'ic_bottom_star_normal.svg';
        }
        this.clickCollect();
      })
      Column() {
        Image($r('app.media.ic_bottom_delete'))
          .width($r('app.float.ic_bottom_picture_size'))
          .aspectRatio(1)
        Text($r('app.string.delete_note'))
          .fontSize($r('app.float.font_size_smallest'))
          .margin({ top: $r('app.float.bottom_txt_margin') })
      }
      .onClick(() => {
        this.clickDelete = () => {
          if (this.operationType === CommonConstants.MODIFY_NOTE) {
            this.deleteDialogController.open();
          } else {
            prompt.showToast({
              message: 'The addition operation cannot be deleted'
            });
          }
        }
        this.clickDelete();
      })
      Column() {
        Image($r('app.media.ic_bottom_picture'))
          .width($r('app.float.ic_bottom_picture_size'))
          .aspectRatio(1)
        Text($r('app.string.add_note_photo'))
          .fontSize($r('app.float.font_size_smallest'))
          .margin({ top: $r('app.float.bottom_txt_margin') })
      }
      .onClick(() => {
        this.clickAddPicture = () => {
          this.imageSrc = this.chooseImage();
          if (this.imageSrc === '') {
            prompt.showToast({
              message: 'Not anymore pictures'
            });
          } else {
            this.imageArr = this.notesInfo.imageArr;
            this.imageArr.push(this.imageSrc);
            this.isDataChanged = true;
          }
        }
        this.clickAddPicture();
      })
    }
    .height($r('app.float.top_bar_container_height'))
    .width(StyleConstants.FULL_WIDTH)
    .justifyContent(FlexAlign.SpaceAround)
    .alignItems(VerticalAlign.Center)
    .backgroundColor($r('app.color.white_color'))
  }

  chooseImage() {
    for (let i = 0;i < this.imageUrls.length; i++) {
      if (this.imageArr.length === 0) {
        return this.imageUrls[i];
      }
      if (!this.imageArr.includes(this.imageUrls[i])) {
        return this.imageUrls[i];
      }
    }
    return '';
  }
}