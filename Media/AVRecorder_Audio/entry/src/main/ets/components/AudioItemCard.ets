/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import type { AudioRecord } from '../model/AudioRecord';
import { formatDuration, formatDate } from '../utils/TimeUtils';
import { formatFileSize } from '../utils/FormatUtils';
import { recordStore } from '../store/RecordStore';
import { appState } from '../store/AppState';

@ComponentV2
export struct AudioItemCard {
  @Require @Param record!: AudioRecord;
  @Param editable: boolean = false;
  @Event onPlay?: (record: AudioRecord) => void;
  @Event onDelete?: (record: AudioRecord) => void;
  @Local isEditing: boolean = false;
  @Local draftName: string = '';

  private startEditing(): void {
    if (!this.editable) {
      return;
    }
    this.draftName = this.record.name;
    this.isEditing = true;
  }

  private commitEditing(): void {
    if (!this.isEditing) {
      return;
    }
    const trimmed = this.draftName.trim();
    if (!trimmed) {
      appState.showToast('Name cannot be empty');
      return;
    }
    const lowerName = trimmed.toLowerCase();
    const exists = recordStore.getRecords().some((item) =>
      item.id !== this.record.id && item.name.toLowerCase() === lowerName
    );
    if (exists) {
      appState.showToast('Name already exists');
      return;
    }
    if (trimmed !== this.record.name) {
      recordStore.updateName(this.record.id, trimmed);
      this.record.name = trimmed;
    }
    this.draftName = trimmed;
    this.isEditing = false;
  }

  build() {
    Column() {
      Row() {
        Column() {
          if (this.isEditing) {
            TextInput({ text: this.draftName, placeholder: 'Recording name' })
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .backgroundColor('#ffffff')
              .borderRadius(8)
              .padding({ left: 8, right: 8, top: 6, bottom: 6 })
              .width('100%')
              .onChange((value: string) => {
                this.draftName = value;
              })
              .onSubmit(() => this.commitEditing())
              .onBlur(() => this.commitEditing());
          } else if (this.editable) {
            Text(this.record.name)
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .padding({ left: 8, right: 8, top: 4, bottom: 4 })
              .border({ width: 1, color: '#d6cbbd' })
              .borderRadius(6)
              .onClick(() => this.startEditing());
          } else {
            Text(this.record.name)
              .fontSize(16)
              .fontWeight(FontWeight.Medium);
          }
          Text(
            `${formatDuration(this.record.durationMs)}  •  ` +
            `${formatFileSize(this.record.sizeBytes)}  •  ` +
            `${formatDate(this.record.createdAt)}`
          )
            .fontSize(12)
            .fontColor('#666666');
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1);
        Row({ space: 8 }) {
          Button() {
            Image($r('app.media.play'))
              .width(18)
              .height(18)
              .fillColor('#ffffff');
          }
          .width(36)
          .height(36)
          .backgroundColor('#1f6f6d')
          .borderRadius(18)
          .onClick(() => this.onPlay && this.onPlay(this.record));

          if (this.onDelete) {
            Button() {
              Image($r('app.media.delete'))
                .width(18)
                .height(18)
                .fillColor('#ffffff');
            }
            .width(36)
            .height(36)
            .backgroundColor('#d94b3d')
            .borderRadius(18)
            .onClick(() => this.onDelete && this.onDelete(this.record));
          }
        }
      }
      .padding({ top: 8, bottom: 8 })
    }
    .padding({ left: 16, right: 16 })
    .backgroundColor('#fff8f0')
    .border({ width: 1, color: '#e7dccb' })
    .borderRadius(16)
    .shadow({ radius: 8, color: '#1a000000', offsetX: 0, offsetY: 4 })
    .margin({ bottom: 12 })
  }
}
