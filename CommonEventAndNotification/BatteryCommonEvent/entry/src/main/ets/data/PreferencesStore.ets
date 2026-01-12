import preferences from '@ohos.data.preferences';
import type common from '@ohos.app.ability.common';
import { Logger } from '../common/Logger';
import { JsonUtil } from '../utils/JsonUtil';
import { Settings, type SettingsJson } from '../model/Settings';
import { BatterySnapshot, type BatterySnapshotJson, BatteryEvent, type BatteryEventJson } from '../model/BatteryEvent';

const PREFS_NAME = 'battery_monitor';
const KEY_SETTINGS = 'settings_json';
const KEY_SNAPSHOT = 'snapshot_json';
const KEY_HISTORY = 'history_json';

export class PreferencesStore {
  private static _inst: PreferencesStore | null = null;
  static get inst(): PreferencesStore {
    if (!PreferencesStore._inst) {
      PreferencesStore._inst = new PreferencesStore();
    }
    return PreferencesStore._inst;
  }

  private pref?: preferences.Preferences;

  async init(context: common.Context): Promise<void> {
    if (this.pref) {
      return;
    }
    try {
      const p = await preferences.getPreferences(context, PREFS_NAME);
      this.pref = p;
      Logger.i('Preferences init ok');
    } catch (e) {
      Logger.w('Preferences init failed', e as Object);
    }
  }

  async loadSettings(): Promise<Settings> {
    if (!this.pref) {
      return new Settings();
    }
    try {
      const txt = this.pref.getSync(KEY_SETTINGS, '') as string;
      if (!txt) {
        return new Settings();
      }
      const obj = JsonUtil.safeParseObject(txt);
      return Settings.fromJson(obj as (SettingsJson | null));
    } catch (e) {
      Logger.w('loadSettings failed', e as Object);
      return new Settings();
    }
  }

  async saveSettings(s: Settings): Promise<void> {
    if (!this.pref) {
      return;
    }
    try {
      const txt = JsonUtil.safeStringify(s.toJson(), 0);
      await this.pref.put(KEY_SETTINGS, txt);
      await this.pref.flush();
    } catch (e) {
      Logger.w('saveSettings failed', e as Object);
    }
  }

  async loadSnapshot(): Promise<BatterySnapshot | null> {
    if (!this.pref) {
      return null;
    }
    try {
      const txt = this.pref.getSync(KEY_SNAPSHOT, '') as string;
      if (!txt) {
        return null;
      }
      const obj = JsonUtil.safeParseObject(txt);
      return BatterySnapshot.fromJson(obj as (BatterySnapshotJson | null));
    } catch (e) {
      Logger.w('loadSnapshot failed', e as Object);
      return null;
    }
  }

  async saveSnapshot(s: BatterySnapshot): Promise<void> {
    if (!this.pref) {
      return;
    }
    try {
      const txt = JsonUtil.safeStringify(s.toJson(), 0);
      await this.pref.put(KEY_SNAPSHOT, txt);
      await this.pref.flush();
    } catch (e) {
      Logger.w('saveSnapshot failed', e as Object);
    }
  }

  async loadHistoryCache(): Promise<BatteryEvent[]> {
    if (!this.pref) {
      return [];
    }
    try {
      const txt = this.pref.getSync(KEY_HISTORY, '') as string;
      if (!txt) {
        return [];
      }
      const parsed = JsonUtil.safeParseObject(txt);
      if (!parsed) {
        return [];
      }

      const arr = parsed as Object as BatteryEventJson[];
      const out: BatteryEvent[] = [];
      for (let i = 0; i < arr.length; i++) {
        const ev = BatteryEvent.fromJson(arr[i]);
        if (ev) {
          out.push(ev);
        }
      }
      return out;
    } catch (e) {
      Logger.w('loadHistoryCache failed', e as Object);
      return [];
    }
  }

  async saveHistoryCache(history: BatteryEvent[]): Promise<void> {
    if (!this.pref) {
      return;
    }
    try {
      const jsonList: BatteryEventJson[] = [];
      for (let i = 0; i < history.length; i++) {
        jsonList.push(history[i].toJson());
      }
      const txt = JsonUtil.safeStringify(jsonList, 0);
      await this.pref.put(KEY_HISTORY, txt);
      await this.pref.flush();
    } catch (e) {
      Logger.w('saveHistoryCache failed', e as Object);
    }
  }
}
