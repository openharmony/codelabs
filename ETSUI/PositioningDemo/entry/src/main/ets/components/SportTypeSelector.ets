/*
 * Copyright (c) 2023, Your Company Name. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Your Company Name, if you need additional information or
 * have any */
import { SportType } from '../model/SportModel';

@Component
export struct SportTypeSelector {
  @Prop selectedType: SportType;
  onSelect: (type: SportType) => void = () => {};

  build() {
    Row() {
      this.TypeButton(SportType.RUNNING, '跑步', '🏃')
      this.TypeButton(SportType.WALKING, '行走', '🚶')
      this.TypeButton(SportType.CYCLING, '骑行', '🚴')
    }
    .width('100%')
    .justifyContent(FlexAlign.SpaceEvenly)
  }

  @Builder
  TypeButton(type: SportType, label: string, icon: string) {
    Column() {
      Text(icon)
        .fontSize(32)
        .margin({ bottom: 8 })
      Text(label)
        .fontSize(14)
        .fontColor(this.selectedType === type ? '#4ECDC4' : '#999999')
    }
    .padding({ top: 16, bottom: 16, left: 24, right: 24 })
    .backgroundColor(this.selectedType === type ? '#4ECDC420' : '#FFFFFF')
    .borderRadius(12)
    .border(this.selectedType === type ? { width: 2, color: '#4ECDC4' } : { width: 1, color: '#E0E0E0' })
    .onClick(() => {
      this.onSelect(type);
    })
  }
}
