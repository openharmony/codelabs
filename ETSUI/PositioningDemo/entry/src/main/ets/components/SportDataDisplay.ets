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
// SPDX-License-Identifier: MIT
import { SportData, SportType } from '../model/SportModel';

@Component
export struct SportDataDisplay {
  @Prop sportData: SportData | null;

  build() {
    if (this.sportData) {
      Column() {
        Row() {
          this.DataItem('距离', this.formatDistance(this.sportData.distance), '#FF6B6B')
          this.DataItem('时间', this.formatDuration(this.sportData.duration), '#4ECDC4')
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceEvenly)
        .margin({ bottom: 20 })

        Row() {
          this.DataItem('速度', this.formatSpeed(this.sportData.currentSpeed), '#45B7D1')
          this.DataItem('卡路里', this.formatCalories(this.sportData.calories), '#FFA502')
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceEvenly)
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#FFFFFF')
      .borderRadius(16)
      .shadow({ radius: 8, color: '#1A000000', offsetX: 0, offsetY: 2 })
    }
  }

  @Builder
  DataItem(label: string, value: string, color: string) {
    Column() {
      Text(value)
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor(color)
        .margin({ bottom: 4 })
      Text(label)
        .fontSize(12)
        .fontColor('#999999')
    }
    .alignItems(HorizontalAlign.Center)
  }

  private formatDistance(distance: number): string {
    if (distance >= 1000) {
      return `${(distance / 1000).toFixed(2)} km`;
    }
    return `${distance.toFixed(0)} m`;
  }

  private formatDuration(duration: number): string {
    const hours = Math.floor(duration / 3600000);
    const minutes = Math.floor((duration % 3600000) / 60000);
    const seconds = Math.floor((duration % 60000) / 1000);

    if (hours > 0) {
      return `${hours}:${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
    }
    return `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
  }

  private formatSpeed(speed: number): string {
    return `${speed.toFixed(1)} m/s`;
  }

  private formatCalories(calories: number): string {
    return `${calories.toFixed(0)} kcal`;
  }
}
