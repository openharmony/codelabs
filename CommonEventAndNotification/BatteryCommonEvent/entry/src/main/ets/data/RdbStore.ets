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
import relationalStore from '@ohos.data.relationalStore';
import type common from '@ohos.app.ability.common';
import type { BatteryEvent } from '../model/BatteryEvent';
import { Logger } from '../common/Logger';
import { DEFAULT_RETENTION_DAYS } from '../common/Const';

const DB_NAME = 'battery_monitor.db';
const TABLE = 'battery_event';

const CREATE_TABLE_SQL = `
CREATE TABLE IF NOT EXISTS ${TABLE} (
  id TEXT PRIMARY KEY,
  timeMs INTEGER,
  soc INTEGER,
  chargeState INTEGER,
  healthState INTEGER,
  pluggedType INTEGER,
  temperatureC REAL,
  voltageV REAL,
  technology TEXT,
  present INTEGER,
  raw TEXT
);`;

interface ValuesRow {}

export class RdbStore {
  private static _inst: RdbStore | null = null;

  static get inst(): RdbStore {
    if (!RdbStore._inst) {
      RdbStore._inst = new RdbStore();
    }
    return RdbStore._inst;
  }

  private store?: relationalStore.RdbStore;

  async init(context: common.Context): Promise<void> {
    if (this.store) {
      return;
    }
    try {
      const cfg: relationalStore.StoreConfig = {
        name: DB_NAME,
        securityLevel: relationalStore.SecurityLevel.S1
      };
      const store = await relationalStore.getRdbStore(context, cfg);
      await store.executeSql(CREATE_TABLE_SQL);
      this.store = store;
      Logger.i('Rdb init ok');
    } catch (e) {
      Logger.w('Rdb init failed', e);
    }
  }

  async insertEvent(ev: BatteryEvent): Promise<void> {
    if (!this.store) {
      return;
    }
    try {
      const row: relationalStore.ValuesBucket = {} as ValuesRow as relationalStore.ValuesBucket;

      row['id'] = ev.id;
      row['timeMs'] = ev.timeMs;
      row['soc'] = ev.snapshot.soc;
      row['chargeState'] = ev.snapshot.chargeState;
      row['healthState'] = ev.snapshot.healthState;
      row['pluggedType'] = ev.snapshot.pluggedType;
      row['temperatureC'] = ev.snapshot.temperatureC;
      row['voltageV'] = ev.snapshot.voltageV;
      row['technology'] = ev.snapshot.technology;
      row['present'] = ev.snapshot.present ? 1 : 0;
      row['raw'] = JSON.stringify(ev.raw);
      await this.store.insert(TABLE, row);
    } catch (e) {
      Logger.w('insertEvent failed', e);
    }
  }

  async cleanup(retentionDays: number = DEFAULT_RETENTION_DAYS): Promise<void> {
    if (!this.store) {
      return;
    }
    const cutoff = Date.now() - retentionDays * 24 * 3600 * 1000;
    const predicates = new relationalStore.RdbPredicates(TABLE);
    predicates.lessThan('timeMs', cutoff);
    try {
      await this.store.delete(predicates);
      Logger.i('cleanup ok, cutoff=%{public}d', cutoff);
    } catch (e) {
      Logger.w('cleanup failed', e);
    }
  }
}
