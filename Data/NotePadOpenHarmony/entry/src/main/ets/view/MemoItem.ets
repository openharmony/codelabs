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

import StyleConstants from '../common/constants/StyleConstants';
import CommonConstants from '../common/constants/CommonConstants';
import { notesItemArr } from '../viewmodel/NotesInfoViewModel';
import { FolderType, NotesInfoBean } from '../common/bean/NoteInfoBean';

@Component
export default struct MemoItem {
  @State noteItem: NotesInfoBean = notesItemArr[0];

  build() {
    Navigator({ target: 'pages/NotesDetail', type: NavigationType.Replace }) {
      Column() {
        Row() {
          Column() {
            Text(this.noteItem.title)
              .fontSize($r('app.float.font_size_normal'))
              .textOverflow({ overflow: TextOverflow.Ellipsis })
              .maxLines(1)
              .fontColor(this.noteItem.folder === FolderType.Personal ?
                $r('app.color.font_color_light_dark') : $r('app.color.font_color_white'))
            Row() {
              if (this.noteItem.isFavorite) {
                Image(this.noteItem.folder === FolderType.Personal ?
                  $r('app.media.ic_star_yellow') : $r('app.media.ic_star_white'))
                  .height($r('app.float.list_collect_icon_size'))
                  .aspectRatio(1)
                  .margin({ right: $r('app.float.list_content_margin') })
              }
              Text(this.noteItem.content)
                .fontSize($r('app.float.font_size_smaller'))
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .maxLines(1)
                .fontColor(this.noteItem.folder === FolderType.Personal ?
                  $r('app.color.font_color_light_dark') : $r('app.color.font_color_white'))
            }
          }
          .alignItems(HorizontalAlign.Start)
          .width($r('app.float.text_title_width'))
          Image($rawfile(this.noteItem.imageArr[0]))
            .width(36)
            .aspectRatio(1)
        }
        .height($r('app.float.list_item_height'))
        .backgroundColor(this.noteItem.folder === FolderType.Work ?
          $r('app.color.list_item_highlight_color') : $r('app.color.list_item_normal_color'))
        .width(StyleConstants.CONTENT_WIDTH)
        .justifyContent(FlexAlign.SpaceBetween)
        .borderRadius($r('app.float.list_item_border_radius'))
        .padding({ left: $r('app.float.list_item_padding'), right: $r('app.float.list_item_padding') })
      }
      .alignItems(HorizontalAlign.Center)
      .width(StyleConstants.FULL_WIDTH)
    }
    .params({
      notesInfo: this.noteItem,
      operationType: CommonConstants.MODIFY_NOTE,
    })
  }
}