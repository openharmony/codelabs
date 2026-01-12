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
import type common from '@ohos.app.ability.common';
import { hilog } from '@kit.PerformanceAnalysisKit';

import { BatteryEventRepository, type RepositoryListener } from '../repository/BatteryEventRepository';
import type { BatteryEvent, BatterySnapshot } from '../model/BatteryEvent';
import { BatteryFormat } from '../utils/BatteryFormat';
import { TimeUtil } from '../utils/TimeUtil';
import { StatCard } from '../components/StatCard';
import { TrendChart } from '../components/TrendChart';

const DOMAIN: number = 0x0000;
const TAG: string = 'BatteryMonitor';

class IndexListener implements RepositoryListener {
  private owner: Index;

  constructor(owner: Index) {
    this.owner = owner;
  }

  onSnapshotUpdated(s: BatterySnapshot): void {
    this.owner.snapshot = s;
  }

  onHistoryUpdated(events: BatteryEvent[]): void {
    this.owner.history = events;
  }
}

@Entry
@Component
export struct Index {
  @State snapshot: BatterySnapshot | null = null;
  @State history: BatteryEvent[] = [];

  private repo: BatteryEventRepository = BatteryEventRepository.inst;
  private listener: RepositoryListener | null = null;

  aboutToAppear() {
    const ctx = getContext(this) as common.UIAbilityContext;
    this.repo.ensureInit(ctx).then(() => {
      this.snapshot = this.repo.getLastSnapshot();
      this.history = this.repo.getHistory();
    }).catch(() => {
      hilog.error(DOMAIN, TAG, 'ensureInit failed');
    });

    this.listener = new IndexListener(this);
    this.repo.addListener(this.listener);
  }

  aboutToDisappear() {
    if (this.listener) {
      this.repo.removeListener(this.listener);
    }
    this.listener = null;
  }

  private gotoHistory(): void {
    try {
      router.pushUrl({ url: 'pages/History' });
    } catch {
      hilog.error(DOMAIN, TAG, 'gotoHistory failed');
    }
  }

  private gotoMemory(): void {
    try {
      router.pushUrl({ url: 'pages/Memory' });
    } catch {
      hilog.error(DOMAIN, TAG, 'gotoMemory failed');
    }
  }

  private gotoSettings(): void {
    try {
      router.pushUrl({ url: 'pages/Settings' });
    } catch {
      hilog.error(DOMAIN, TAG, 'gotoSettings failed');
    }
  }

  private gotoAbout(): void {
    try {
      router.pushUrl({ url: 'pages/About' });
    } catch {
      hilog.error(DOMAIN, TAG, 'gotoAbout failed');
    }
  }

  private gotoDetail(eventId: string): void {
    try {
      router.pushUrl({ url: 'pages/Detail', params: { eventId: eventId } });
    } catch {
      hilog.error(DOMAIN, TAG, 'gotoDetail failed');
    }
  }

  private buildSocValues(): number[] {
    const out: number[] = [];
    const len = this.history.length;
    const cap = len < 50 ? len : 50;
    // Take last 'cap' items in chronological order for chart
    for (let i = cap - 1; i >= 0; i--) {
      out.push(this.history[i].snapshot.soc);
    }
    return out;
  }

  build() {
    Column() {
      Row() {

        Blank().width(44)

        Text('电池监测�?)
          .layoutWeight(1)
          .textAlign(TextAlign.Center)
          .fontSize(20)
          .fontWeight(FontWeight.Medium)

        Button() {
          Text('�?)
            .fontSize(24)
            .fontWeight(FontWeight.Medium)
        }
        .width(44)
        .height(44)
        .backgroundColor(Color.Transparent)
        .bindMenu([
          { value: 'History', action: () => this.gotoHistory() },
          { value: 'Memory', action: () => this.gotoMemory() },
          { value: 'Settings', action: () => this.gotoSettings() },
          { value: 'About', action: () => this.gotoAbout() },
        ], { placement: Placement.Bottom })
      }
      .padding({ left: 16, right: 16, top: 16, bottom: 10 })


      if (this.snapshot) {
        Column({ space: 10 }) {
          Row({ space: 10 }) {
            StatCard({ title: 'SOC', value: BatteryFormat.socText(this.snapshot!.soc) });
            StatCard({ title: 'Charge', value: BatteryFormat.chargeTextBySnapshot(this.snapshot!) });
            StatCard({ title: 'Plug', value: BatteryFormat.pluggedText(this.snapshot!.pluggedType) });
          }
          .padding({ left: 16, right: 16 });

          Row({ space: 10 }) {
            StatCard({ title: 'Temp', value: BatteryFormat.tempText(this.snapshot!.temperatureC) });
            StatCard({ title: 'Volt', value: BatteryFormat.voltText(this.snapshot!.voltageV) });
            StatCard({ title: 'Health', value: BatteryFormat.healthText(this.snapshot!.healthState) });
          }
          .padding({ left: 16, right: 16 });

          TrendChart({ values: this.buildSocValues(), label: 'SOC', heightPx: 140 })
            .padding({ left: 16, right: 16, top: 12 });

          Text(`Last updated: ${TimeUtil.formatTime(this.snapshot!.timeMs)}`)
            .fontSize(12)
            .opacity(0.65)
            .padding({ left: 16, right: 16, top: 8 });
        }
      } else {
        Column() {
          Text('Waiting for battery events...')
            .opacity(0.7)
            .padding(16);
        }
        .padding({ left: 16, right: 16 });
      }

      // Recent records (top 5)
      Column({ space: 10 }) {
        Text('Recent')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .padding({ left: 16, right: 16, top: 14 });

        List() {
          ForEach(this.history.slice(0, 5), (e: BatteryEvent) => {
            ListItem() {
              Column({ space: 4 }) {
                Text(TimeUtil.formatTime(e.timeMs))
                  .fontSize(12)
                  .opacity(0.75);

                Text(BatteryFormat.snapshotSummary(e.snapshot))
                  .fontSize(14);
              }
              .padding(12)
              .backgroundColor(0x0D000000)
              .borderRadius(12)
              .onClick(() => this.gotoDetail(e.id));
            }
            .padding({ left: 16, right: 16, bottom: 10 });
          }, (e: BatteryEvent) => e.id);
        }
        .layoutWeight(1);
      }
      .layoutWeight(1);
    }
    .height('100%');
  }
}
