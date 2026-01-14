/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import { BatteryEventRepository } from '../repository/BatteryEventRepository';
import { Settings, RecordMode } from '../model/Settings';
import { Logger } from '../common/Logger';

@Entry
@Component
export struct SettingsPage {
  @State s: Settings = BatteryEventRepository.inst.getSettings();
  @State toast: string = '';

  private async save(): Promise<void> {
    await BatteryEventRepository.inst.updateSettings(this.s);
    this.toast = 'Saved';
  }

  private back(): void {
    router.back();
  }

  build() {
    Column() {
      Row() {
        Button('← 退后').onClick(() => this.back());
        Text('Settings')
          .layoutWeight(1)
          .textAlign(TextAlign.Center)
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
        Button('保存').onClick(() => this.save());
      }
      .padding({
        left: 16,
        right: 16,
        top: 16,
        bottom: 10
      })
      .width('100%');

      Column({ space: 12 }) {
        Text('History size (1-200)').fontSize(12).opacity(0.75);
        Slider({
          value: this.s.maxHistory,
          min: 1,
          max: 200,
          step: 1
        })
          .onChange((v: number) => {
            this.s.maxHistory = Math.floor(v);
          });

        Row({ space: 10 }) {
          Toggle({ type: ToggleType.Switch, isOn: this.s.autoSubscribe })
            .onChange((on: boolean) => {
              this.s.autoSubscribe = on;
            });
          Text('自动订阅').fontSize(14);
        }

        Row({ space: 10 }) {
          Toggle({ type: ToggleType.Switch, isOn: this.s.keepInBackground })
            .onChange((on: boolean) => {
              this.s.keepInBackground = on;
            });
          Text('后台运行').fontSize(14);
        }

        Text('记录模式').fontSize(12).opacity(0.75);

        Column({ space: 6 }) {
          Row({ space: 10 }) {
            Radio({ value: 'all', group: 'mode' })
              .checked(this.s.recordMode === RecordMode.ALL)
              .onChange((checked: boolean) => {
                if (checked) {
                  this.s.recordMode = RecordMode.ALL;
                }
              });
            Text('全记录Record All').fontSize(14);
          }

          Row({ space: 10 }) {
            Radio({ value: 'soc', group: 'mode' })
              .checked(this.s.recordMode === RecordMode.SOC_ONLY)
              .onChange((checked: boolean) => {
                if (checked) {
                  this.s.recordMode = RecordMode.SOC_ONLY;
                }
              });
            Text('仅电量变换记录SOC only').fontSize(14);
          }

          Row({ space: 10 }) {
            Radio({ value: 'plug', group: 'mode' })
              .checked(this.s.recordMode === RecordMode.PLUG_ONLY)
              .onChange((checked: boolean) => {
                if (checked) {
                  this.s.recordMode = RecordMode.PLUG_ONLY;
                }
              });
            Text('插拔变化时记录Plug only').fontSize(14);
          }
        }
      }
      .padding(16)
      .backgroundColor(0x08000000)
      .borderRadius(12)
      .margin({ left: 16, right: 16 });

      if (this.toast) {
        Text(this.toast)
          .fontSize(12)
          .opacity(0.75)
          .padding({ left: 16, right: 16 });
      }
    }
    .height('100%');
  }
}
