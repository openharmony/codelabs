export interface AudioRecord {
  id: string;
  name: string;
  path: string;
  durationMs: number;
  sizeBytes: number;
  createdAt: number;
  format: string;
  sampleRate: number;
  channels: number;
}
