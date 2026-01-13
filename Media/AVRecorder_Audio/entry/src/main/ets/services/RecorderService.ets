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

import media from '@ohos.multimedia.media';
import fs from '@ohos.file.fs';
import type { RecorderConfig } from '../model/RecorderConfig';
import { RecordStatus } from '../model/Enums';
import { Logger } from '../utils/Logger';

export type RecorderListener = (status: RecordStatus) => void;

export class RecorderService {
  private recorder?: media.AVRecorder;
  private status: RecordStatus = RecordStatus.Idle;
  private listeners: Set<RecorderListener> = new Set();
  private recordingFile: fs.File | null = null;

  getStatus(): RecordStatus {
    return this.status;
  }

  subscribe(listener: RecorderListener): () => void {
    this.listeners.add(listener);
    return () => this.listeners.delete(listener);
  }

  private setStatus(status: RecordStatus): void {
    this.status = status;
    this.listeners.forEach(listener => listener(status));
  }

  private formatError(err: Object | string | number | boolean | undefined | null): string {
    if (err === null || err === undefined) {
      return 'unknown error';
    }
    if (typeof err === 'string' || typeof err === 'number' || typeof err === 'boolean') {
      return String(err);
    }
    const value = err as Record<string, string | number | boolean>;
    const code = value.code;
    const message = value.message;
    if (code !== undefined || message !== undefined) {
      return `code=${String(code)} message=${String(message)}`;
    }
    try {
      return JSON.stringify(err);
    } catch (_ignore) {
      return String(err);
    }
  }

  private getErrorCode(err: Object | string | number | boolean | undefined | null): number {
    if (err === null || err === undefined) {
      return -1;
    }
    if (typeof err === 'number') {
      return err;
    }
    if (typeof err === 'object') {
      const value = err as Record<string, number | string>;
      const code = value.code;
      if (typeof code === 'number') {
        return code;
      }
      if (typeof code === 'string') {
        const parsed = Number(code);
        return Number.isNaN(parsed) ? -1 : parsed;
      }
    }
    const text = String(err);
    const index = text.indexOf('code=');
    if (index >= 0) {
      let digits = '';
      for (let i = index + 5; i < text.length; i += 1) {
        const ch = text.charAt(i);
        if (ch < '0' || ch > '9') {
          break;
        }
        digits += ch;
      }
      if (digits.length > 0) {
        const parsed = Number(digits);
        return Number.isNaN(parsed) ? -1 : parsed;
      }
    }
    return -1;
  }

  private ensureDirPath(dirPath: string): void {
    try {
      fs.mkdirSync(dirPath, true);
    } catch (err) {
      const detail = this.formatError(err as Object);
      const code = this.getErrorCode(err as Object);
      if (code === 13900015 || detail.indexOf('File exists') >= 0) {
        return;
      }
      throw new Error(detail);
    }
  }

  async start(filePath: string, config: RecorderConfig): Promise<void> {
    if (this.status === RecordStatus.Recording) {
      return;
    }
    try {
      const dirIndex = filePath.lastIndexOf('/');
      if (dirIndex > 0) {
        const dirPath = filePath.slice(0, dirIndex);
        try {
          this.ensureDirPath(dirPath);
        } catch (err) {
          const detail = this.formatError(err as Object);
          Logger.error('Create record dir failed', detail);
          throw new Error(detail);
        }
      }
      this.recorder = await media.createAVRecorder();
      this.recorder.on('error', (err: Error) => {
        Logger.error('Recorder error', this.formatError(err));
        this.setStatus(RecordStatus.Idle);
      });
      const profile: media.AVRecorderProfile = {
        audioCodec: media.CodecMimeType.AUDIO_AAC,
        audioSampleRate: config.sampleRate,
        audioChannels: config.channels,
        audioBitrate: config.bitRate,
        fileFormat: media.ContainerFormatType.CFT_MPEG_4A
      };

      const pathConfig: media.AVRecorderConfig = {
        audioSourceType: media.AudioSourceType.AUDIO_SOURCE_TYPE_MIC,
        profile: profile,
        url: filePath
      };
      try {
        Logger.info('Recorder url', filePath);
        await this.recorder.prepare(pathConfig);
        await this.recorder.start();
        this.setStatus(RecordStatus.Recording);
        return;
      } catch (err) {
        const detail = this.formatError(err as Object);
        Logger.warn('Recorder path prepare failed', detail);
      }

      let fileExists = false;
      try {
        fs.statSync(filePath);
        fileExists = true;
      } catch (err) {
        fileExists = false;
      }
      if (fileExists) {
        try {
          fs.unlinkSync(filePath);
          fileExists = false;
        } catch (err) {
          Logger.warn('Remove existing file failed', this.formatError(err as Object));
        }
      }
      const openMode = fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE | fs.OpenMode.TRUNC;
      Logger.info('Open recording file', `exists=${String(fileExists)} mode=${String(openMode)}`);
      const file = fs.openSync(filePath, openMode);
      this.recordingFile = file;
      Logger.info('Recording fd', String(file.fd));
      const fdConfig: media.AVRecorderConfig = {
        audioSourceType: media.AudioSourceType.AUDIO_SOURCE_TYPE_MIC,
        profile: profile,
        url: `fd://${file.fd}`
      };

      try {
        await this.recorder.prepare(fdConfig);
        await this.recorder.start();
        this.setStatus(RecordStatus.Recording);
      } catch (err) {
        const detail = this.formatError(err as Object);
        Logger.warn('Recorder fd prepare failed', detail);
        throw new Error(detail);
      }
    } catch (err) {
      const detail = this.formatError(err as Object);
      Logger.error('Recorder start failed', detail);
      await this.safeRelease();
      throw new Error(detail);
    }
  }

  async pause(): Promise<void> {
    if (!this.recorder || this.status !== RecordStatus.Recording) {
      return;
    }
    await this.recorder.pause();
    this.setStatus(RecordStatus.Paused);
  }

  async resume(): Promise<void> {
    if (!this.recorder || this.status !== RecordStatus.Paused) {
      return;
    }
    await this.recorder.resume();
    this.setStatus(RecordStatus.Recording);
  }

  async stop(): Promise<void> {
    if (!this.recorder || this.status === RecordStatus.Idle) {
      return;
    }
    await this.recorder.stop();
    await this.recorder.reset();
    await this.recorder.release();
    this.recorder = undefined;
    this.setStatus(RecordStatus.Idle);
    await this.safeRelease();
  }

  private async safeRelease(): Promise<void> {
    if (this.recordingFile) {
      try {
        fs.closeSync(this.recordingFile);
      } catch (err) {
        Logger.warn('Close recording file failed', this.formatError(err as Object));
      }
      this.recordingFile = null;
    }
    if (this.recorder) {
      try {
        await this.recorder.release();
      } catch (err) {
        Logger.warn('Recorder release failed', this.formatError(err as Object));
      }
      this.recorder = undefined;
    }
  }
}

export const recorderService = new RecorderService();
