import type { BatteryEventRawParams } from '../model/BatteryEvent';
import { BatterySnapshot } from '../model/BatteryEvent';
import { ChargeState, HealthState, PluggedType } from '../model/BatteryEnums';

export class BatteryEventParser {
  parse(raw: BatteryEventRawParams): BatterySnapshot {
    const s = new BatterySnapshot();
    s.timeMs = Date.now();

    const soc = raw.soc;
    if (typeof soc === 'number') {
      s.soc = clampInt(soc, 0, 100);
    }

    const cs = raw.chargeState;
    if (typeof cs === 'number') {
      s.chargeState = clampEnum(cs, ChargeState.UNKNOWN, ChargeState.FULL) as ChargeState;
    }

    const hs = raw.healthState;
    if (typeof hs === 'number') {
      s.healthState = clampEnum(hs, HealthState.UNKNOWN, HealthState.COLD) as HealthState;
    }

    const pt = raw.pluggedType;
    if (typeof pt === 'number') {
      s.pluggedType = clampEnum(pt, PluggedType.NONE, PluggedType.WIRELESS) as PluggedType;
    }

    const present = raw.present;
    if (typeof present === 'boolean') {
      s.present = present;
    }

    const tech = raw.technology;
    if (typeof tech === 'string') {
      s.technology = tech;
    }

    const temp = raw.temperature;
    if (typeof temp === 'number') {
      s.temperatureC = temp > 100 ? temp / 10.0 : temp;
    }

    const volt = raw.voltage;
    if (typeof volt === 'number') {
      s.voltageV = volt > 1000 ? volt / 1_000_000.0 : volt;
    }

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

function clampEnum(v: number, lo: number, hi: number): number {
  const x = Math.floor(v);
  if (x < lo) {
    return lo;
  }
  if (x > hi) {
    return hi;
  }
  return x;
}
