/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { SportType } from '../model/SportModel';

@Component
export struct SportTypeSelector {
  @Prop selectedType: SportType;
  onSelect: (type: SportType) => void = () => {
  };

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
    .padding({
      top: 16,
      bottom: 16,
      left: 24,
      right: 24
    })
    .backgroundColor(this.selectedType === type ? '#4ECDC420' : '#FFFFFF')
    .borderRadius(12)
    .border(this.selectedType === type ? { width: 2, color: '#4ECDC4' } : { width: 1, color: '#E0E0E0' })
    .onClick(() => {
      this.onSelect(type);
    })
  }
}
