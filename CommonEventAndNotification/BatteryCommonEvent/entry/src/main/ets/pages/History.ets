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
import { BatteryEventRepository, type RepositoryListener } from '../repository/BatteryEventRepository';
import type { BatteryEvent, BatterySnapshot } from '../model/BatteryEvent';
import { BatteryFormat } from '../utils/BatteryFormat';
import { TimeUtil } from '../utils/TimeUtil';
import { EmptyState } from '../components/EmptyState';

class HistoryListener implements RepositoryListener {
  private owner: History;

  constructor(owner: History) {
    this.owner = owner;
  }

  onSnapshotUpdated(_: BatterySnapshot): void {
  }

  onHistoryUpdated(events: BatteryEvent[]): void {
    this.owner.history = events;
  }
}

@Entry
@Component
export struct History {
  @State history: BatteryEvent[] = [];
  @State pageSize: number = 40;
  private repo: BatteryEventRepository = BatteryEventRepository.inst;
  private listener: RepositoryListener | null = null;

  aboutToAppear() {
    const ctx = getContext(this) as common.UIAbilityContext;
    this.repo.ensureInit(ctx).then(() => {
      this.history = this.repo.getHistory();
    }).catch(() => {
      this.history = this.repo.getHistory();
    });
    this.listener = new HistoryListener(this);
    this.repo.addListener(this.listener);
  }

  aboutToDisappear() {
    if (this.listener) {
      this.repo.removeListener(this.listener)
    }
    ;
    this.listener = null;
  }

  private back(): void {
    router.back();
  }

  build() {
    Column() {
      Row() {
        Button('Back').onClick(() => this.back());
        Text('History')
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
          .margin({ left: 12 });
        Blank();
      }
      .padding({
        left: 16,
        right: 16,
        top: 16,
        bottom: 10
      })
      .width('100%');

      if (!this.history || this.history.length === 0) {
        EmptyState({ title: 'No records', desc: 'Wait for battery events or enable auto subscribe.' })
          .margin({ top: 30 });
      } else {
        List() {
          ForEach(this.history.slice(0, this.pageSize), (e: BatteryEvent) => {
            ListItem() {
              Row() {
                Column() {
                  Text(TimeUtil.formatTime(e.timeMs))
                    .fontSize(12)
                    .opacity(0.75);
                  Text(`SOC ${e.snapshot.soc}% · ${BatteryFormat.chargeText(e.snapshot.chargeState)}`)
                    .fontSize(14)
                    .margin({ top: 2 });
                }.layoutWeight(1);

                Text('›').fontSize(20).opacity(0.5);
              }
              .padding(12)
              .backgroundColor(0x0D000000)
              .borderRadius(12)
              .onClick(() => {
                router.pushUrl({ url: 'pages/Detail', params: { eventId: e.id } });
              });
            }.padding({ left: 16, right: 16, bottom: 10 });
          }, (e: BatteryEvent) => e.id);
        }.layoutWeight(1);
      }
    }.height('100%');
  }
}
