export enum RecordMode {
  ALL = 0,
  SOC_ONLY = 1,
  PLUG_ONLY = 2
}

export interface ThresholdRule {
  enabled: boolean;
  minSocDelta: number;
  maxTempC: number;
  minVoltV: number;
}

export interface SettingsJson {
  maxHistory: number;
  autoSubscribe: boolean;
  keepInBackground: boolean;
  recordMode: number;
  thresholds: ThresholdRule;
}

export class Settings {
  public maxHistory: number = 80;
  public autoSubscribe: boolean = true;
  public keepInBackground: boolean = false;
  public recordMode: RecordMode = RecordMode.ALL;
  public thresholds: ThresholdRule = {
    enabled: false,
    minSocDelta: 3,
    maxTempC: 45,
    minVoltV: 3.2
  };

  normalize(): void {
    this.maxHistory = clampInt(this.maxHistory, 1, 200);
    this.thresholds.minSocDelta = clampInt(this.thresholds.minSocDelta, 1, 20);
    this.thresholds.maxTempC = clampInt(this.thresholds.maxTempC, 0, 80);
    this.thresholds.minVoltV = clampFloat(this.thresholds.minVoltV, 2.0, 5.5);
  }

  toJson(): SettingsJson {
    return {
      maxHistory: this.maxHistory,
      autoSubscribe: this.autoSubscribe,
      keepInBackground: this.keepInBackground,
      recordMode: this.recordMode,
      thresholds: {
        enabled: this.thresholds.enabled,
        minSocDelta: this.thresholds.minSocDelta,
        maxTempC: this.thresholds.maxTempC,
        minVoltV: this.thresholds.minVoltV
      }
    };
  }

  static fromJson(obj: SettingsJson | null): Settings {
    const s = new Settings();
    if (!obj) {
      return s;
    }

    if (typeof obj.maxHistory === 'number') {
      s.maxHistory = obj.maxHistory;
    }
    if (typeof obj.autoSubscribe === 'boolean') {
      s.autoSubscribe = obj.autoSubscribe;
    }
    if (typeof obj.keepInBackground === 'boolean') {
      s.keepInBackground = obj.keepInBackground;
    }
    if (typeof obj.recordMode === 'number') {
      s.recordMode = obj.recordMode as RecordMode;
    }

    if (obj.thresholds) {
      const t = obj.thresholds;
      if (typeof t.enabled === 'boolean') {
        s.thresholds.enabled = t.enabled;
      }
      if (typeof t.minSocDelta === 'number') {
        s.thresholds.minSocDelta = t.minSocDelta;
      }
      if (typeof t.maxTempC === 'number') {
        s.thresholds.maxTempC = t.maxTempC;
      }
      if (typeof t.minVoltV === 'number') {
        s.thresholds.minVoltV = t.minVoltV;
      }
    }

    s.normalize();
    return s;
  }
}

function clampInt(v: number, lo: number, hi: number): number {
  const x = Math.floor(v);
  if (x < lo) {
    return lo;
  }
  if (x > hi) {
    return hi;
  }
  return x;
}

function clampFloat(v: number, lo: number, hi: number): number {
  if (v < lo) {
    return lo;
  }
  if (v > hi) {
    return hi;
  }
  return v;
}
