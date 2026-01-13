import type { RecorderConfig } from '../model/RecorderConfig';

export function validateRecorderConfig(config: RecorderConfig): boolean {
  if (config.sampleRate <= 0) {
    return false;
  }
  if (config.channels <= 0 || config.channels > 2) {
    return false;
  }
  if (config.bitRate <= 0) {
    return false;
  }
  if (!config.format) {
    return false;
  }
  return true;
}
