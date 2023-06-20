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
import StyleConstants from '../common/constants/StyleConstants';
import { notesItemArr } from '../viewmodel/NotesInfoViewModel';
import { NotesInfoBean } from '../common/bean/NoteInfoBean';

@Component
export default struct BottomBar {
  @Prop collectImgSrc: string;
  @State noteItem: NotesInfoBean = notesItemArr[0];
  clickCollect: () => void;
  clickDelete: () => void;
  clickAddPicture: () => void;

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
        this.clickAddPicture();
      })
    }
    .height($r('app.float.top_bar_container_height'))
    .width(StyleConstants.FULL_WIDTH)
    .justifyContent(FlexAlign.SpaceAround)
    .alignItems(VerticalAlign.Center)
    .backgroundColor($r('app.color.white_color'))
  }
}