import {
  chargeStateText, healthStateText, pluggedTypeText,
  ChargeState, PluggedType
} from '../model/BatteryEnums';

import type { BatterySnapshot } from '../model/BatteryEvent';


export class BatteryFormat {
  static socText(soc: number): string {
    if (soc < 0 || soc > 100) {
      return '--';
    }
    return `${soc}%`;
  }

  static tempText(c: number): string {
    if (!isFinite(c)) {
      return '--';
    }
    return `${c.toFixed(1)}°C`;
  }

  static voltText(v: number): string {
    if (!isFinite(v)) {
      return '--';
    }
    return `${v.toFixed(3)} V`;
  }

  static chargeText(v: number): string {
    return chargeStateText(v);
  }
  static chargeTextBySnapshot(s: BatterySnapshot): string {
    if (s.soc >= 100 || s.chargeState === ChargeState.FULL) {
      return 'Full';
    }
    if (s.pluggedType !== PluggedType.NONE) {
      return 'Charging';
    }
    return 'Not charging'; // 或者你想写 'Discharging'
  }

  static healthText(v: number): string {
    return healthStateText(v);
  }

  static pluggedText(v: number): string {
    return pluggedTypeText(v);
  }

  static snapshotSummary(s: BatterySnapshot): string {
    const parts: string[] = [];
    parts.push(`SOC ${BatteryFormat.socText(s.soc)}`);
    parts.push(BatteryFormat.chargeText(s.chargeState));
    parts.push(BatteryFormat.pluggedText(s.pluggedType));
    if (isFinite(s.temperatureC)) {
      parts.push(`Temp ${BatteryFormat.tempText(s.temperatureC)}`);
    }
    if (isFinite(s.voltageV)) {
      parts.push(`Volt ${BatteryFormat.voltText(s.voltageV)}`);
    }
    return parts.join(' · ');
  }
}
