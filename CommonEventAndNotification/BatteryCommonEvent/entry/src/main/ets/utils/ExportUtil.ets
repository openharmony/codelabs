import fs from '@ohos.file.fs';
import type { BatteryEvent } from '../model/BatteryEvent';
import { JsonUtil } from './JsonUtil';
import { EXPORT_DIR } from '../common/Const';
import { Logger } from '../common/Logger';
import { TimeUtil } from './TimeUtil';

export interface FilesDirContext {
  filesDir: string;
}

export class ExportUtil {
  static async ensureDir(context: FilesDirContext): Promise<string> {
    const dir = context.filesDir + '/' + EXPORT_DIR;
    try {
      const stat = await fs.stat(dir);
      if (stat && stat.isDirectory()) {return dir};
    } catch (e) {}
    await fs.mkdir(dir);
    return dir;
  }

  private static writeTextFile(path: string, text: string): void {
    const file = fs.openSync(path, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE | fs.OpenMode.TRUNC);
    try {
      fs.writeSync(file.fd, text);
    } finally {
      fs.closeSync(file);
    }
  }

  static async exportJson(context: FilesDirContext, events: BatteryEvent[]): Promise<string | null> {
    try {
      const dir = await ExportUtil.ensureDir(context);
      const filename = `battery_${TimeUtil.formatForFile(Date.now())}.json`;
      const path = dir + '/' + filename;
      const jsonList = events.map(e => e.toJson());
      ExportUtil.writeTextFile(path, JsonUtil.safeStringify(jsonList, 2));
      return path;
    } catch (e) {
      Logger.w('exportJson failed', e as Object);
      return null;
    }
  }

  static async exportCsv(context: FilesDirContext, events: BatteryEvent[]): Promise<string | null> {
    try {
      const dir = await ExportUtil.ensureDir(context);
      const filename = `battery_${TimeUtil.formatForFile(Date.now())}.csv`;
      const path = dir + '/' + filename;

      const header = 'time,soc,chargeState,pluggedType,tempC,voltV\n';
      let body = '';
      for (let i = 0; i < events.length; i++) {
        const s = events[i].snapshot;
        body += `${events[i].timeMs},${s.soc},${s.chargeState},${s.pluggedType},${s.temperatureC},${s.voltageV}\n`;
      }
      ExportUtil.writeTextFile(path, header + body);
      return path;
    } catch (e) {
      Logger.w('exportCsv failed', e as Object);
      return null;
    }
  }
}

function safeCsv(s: string): string {
  if (s.indexOf(',') >= 0 || s.indexOf('"') >= 0) {
    return `"${s.replace(/"/g, '""')}"`;
  }
  return s;
}
