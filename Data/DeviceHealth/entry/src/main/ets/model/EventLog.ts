export enum EventType {
  BATTERY = 'BATTERY',
  NETWORK = 'NETWORK',
  STORAGE = 'STORAGE',
  SCREEN = 'SCREEN',
}

export enum EventLevel {
  INFO = 'INFO',
  WARN = 'WARN',
  ERROR = 'ERROR',
}

export interface EventLog {
  id?: number;
  type: EventType;
  level: EventLevel;
  timestamp: number; // ms
  message: string;
  payload?: Record<string, any>;
  source?: string;
}

export function nowMs(): number {
  return Date.now();
}

export function stablePayloadString(payload?: Record<string, any>): string {
  if (!payload) return '';
  const keys = Object.keys(payload).sort();
  const normalized: Record<string, any> = {};
  for (const k of keys) normalized[k] = payload[k];
  try {
    return JSON.stringify(normalized);
  } catch (_) {
    return '';
  }
}
