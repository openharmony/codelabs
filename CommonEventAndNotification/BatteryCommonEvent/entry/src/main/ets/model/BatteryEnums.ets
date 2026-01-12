
export enum ChargeState {
  UNKNOWN = 0,
  CHARGING = 1,
  DISCHARGING = 2,
  FULL = 3
}

export enum HealthState {
  UNKNOWN = 0,
  GOOD = 1,
  OVERHEAT = 2,
  DEAD = 3,
  OVERVOLTAGE = 4,
  UNSPECIFIED_FAILURE = 5,
  COLD = 6
}

export enum PluggedType {
  NONE = 0,
  AC = 1,
  USB = 2,
  WIRELESS = 3
}

export function chargeStateText(v: number): string {
  switch (v) {
    case ChargeState.DISCHARGING: return 'Discharging';
    case ChargeState.CHARGING: return 'Charging';
    case ChargeState.FULL: return 'Full';
    default: return 'Unknown';
  }
}

export function healthStateText(v: number): string {
  switch (v) {
    case HealthState.GOOD: return 'Good';
    case HealthState.OVERHEAT: return 'Overheat';
    case HealthState.DEAD: return 'Dead';
    case HealthState.OVERVOLTAGE: return 'Overvoltage';
    case HealthState.UNSPECIFIED_FAILURE: return 'Failure';
    case HealthState.COLD: return 'Cold';
    default: return 'Unknown';
  }
}

export function pluggedTypeText(v: number): string {
  switch (v) {
    case PluggedType.AC: return 'AC';
    case PluggedType.USB: return 'USB';
    case PluggedType.WIRELESS: return 'Wireless';
    default: return 'None';
  }
}
