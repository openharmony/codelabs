/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import type { RecorderConfig } from '../model/RecorderConfig';
import { DEFAULT_RECORDER_CONFIG } from '../constants/Defaults';

export type SettingsListener = (config: RecorderConfig) => void;

class SettingsStore {
  private config: RecorderConfig = {
    sampleRate: DEFAULT_RECORDER_CONFIG.sampleRate,
    channels: DEFAULT_RECORDER_CONFIG.channels,
    bitRate: DEFAULT_RECORDER_CONFIG.bitRate,
    format: DEFAULT_RECORDER_CONFIG.format
  };
  private listeners: Set<SettingsListener> = new Set();

  getConfig(): RecorderConfig {
    return this.config;
  }

  updateConfig(partial: Partial<RecorderConfig>): void {
    if (partial.sampleRate !== undefined) {
      this.config.sampleRate = partial.sampleRate;
    }
    if (partial.channels !== undefined) {
      this.config.channels = partial.channels;
    }
    if (partial.bitRate !== undefined) {
      this.config.bitRate = partial.bitRate;
    }
    if (partial.format !== undefined) {
      this.config.format = partial.format;
    }
    this.notify();
  }

  updateSampleRate(value: number): void {
    this.config.sampleRate = value;
    this.notify();
  }

  updateChannels(value: number): void {
    this.config.channels = value;
    this.notify();
  }

  updateBitRate(value: number): void {
    this.config.bitRate = value;
    this.notify();
  }

  updateFormat(value: string): void {
    this.config.format = value;
    this.notify();
  }

  subscribe(listener: SettingsListener): () => void {
    this.listeners.add(listener);
    listener(this.config);
    return () => this.listeners.delete(listener);
  }

  private notify(): void {
    this.listeners.forEach(listener => listener(this.config));
  }
}

export const settingsStore = new SettingsStore();
