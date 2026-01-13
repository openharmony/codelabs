import type { RecorderConfig } from '../model/RecorderConfig';

export const DEFAULT_RECORDER_CONFIG: RecorderConfig = {
  sampleRate: 48000,
  channels: 2,
  bitRate: 128000,
  format: 'm4a'
};
