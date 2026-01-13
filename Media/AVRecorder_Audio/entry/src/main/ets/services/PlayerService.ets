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
import { PlayerStatus } from '../model/Enums';
import { Logger } from '../utils/Logger';

export type PlayerListener = (status: PlayerStatus, positionMs: number, durationMs: number) => void;

export class PlayerService {
  private player?: media.AVPlayer;
  private status: PlayerStatus = PlayerStatus.Idle;
  private durationMs: number = 0;
  private listeners: Set<PlayerListener> = new Set();
  private positionMs: number = 0;
  private playingFile: fs.File | null = null;
  private stateWaitTimer: number = -1;
  subscribe(listener: PlayerListener): () => void {
    this.listeners.add(listener);
    return () => this.listeners.delete(listener);
  }

  private setStatus(status: PlayerStatus): void {
    this.status = status;
    this.listeners.forEach(listener => listener(status, this.positionMs, this.durationMs));
  }

  private setPosition(positionMs: number): void {
    this.positionMs = positionMs;
    this.listeners.forEach(listener => listener(this.status, positionMs, this.durationMs));
  }

  private setDuration(durationMs: number): void {
    if (durationMs <= 0) {
      return;
    }
    this.durationMs = durationMs;
    this.listeners.forEach(listener => listener(this.status, this.positionMs, this.durationMs));
  }

  private waitForState(states: string[], timeoutMs: number): Promise<void> {
    if (!this.player) {
      return Promise.reject(new Error('player not created'));
    }
    if (states.indexOf(this.player.state) >= 0) {
      return Promise.resolve();
    }
    return new Promise((resolve, reject) => {
      const handler = (state: string) => {
        if (states.indexOf(state) >= 0) {
          this.player?.off('stateChange', handler);
          if (this.stateWaitTimer !== -1) {
            clearTimeout(this.stateWaitTimer);
            this.stateWaitTimer = -1;
          }
          resolve();
        }
      };
      this.player?.on('stateChange', handler);
      const timerId: number = setTimeout(() => {
        this.player?.off('stateChange', handler);
        this.stateWaitTimer = -1;
        reject(new Error('wait state timeout'));
      }, timeoutMs);
      this.stateWaitTimer = timerId;
    });
  }

  async play(path: string): Promise<void> {
    await this.release();
    try {
      const file = fs.openSync(path, fs.OpenMode.READ_ONLY);
      this.playingFile = file;
      Logger.info('Play fd', String(file.fd));
      this.player = await media.createAVPlayer();
      this.player.on('error', (err: Error) => {
        Logger.error('Player error', String(err));
        this.setStatus(PlayerStatus.Idle);
        this.safeClose();
      });
      this.player.on('timeUpdate', (time: number) => {
        this.setPosition(time);
      });
      this.player.on('stateChange', (state: string) => {
        Logger.info('Player state', state);
        if (state === 'completed') {
          this.setStatus(PlayerStatus.Completed);
          this.safeClose();
        }
      });
      if (this.player.state !== 'idle' && this.player.state !== 'initialized') {
        Logger.warn('Player reset before prepare', String(this.player.state));
        await this.player.reset();
      }
      this.player.url = `fd://${file.fd}`;
      Logger.info('Player url', this.player.url);
      await this.waitForState(['initialized'], 1000);
      await this.player.prepare();
      this.setDuration(this.player.duration);
      await this.player.play();
      this.setStatus(PlayerStatus.Playing);
    } catch (err) {
      Logger.error('Player start failed', String(err));
      this.safeClose();
      this.setStatus(PlayerStatus.Idle);
      throw new Error(String(err));
    }
  }

  async pause(): Promise<void> {
    if (!this.player || this.status !== PlayerStatus.Playing) {
      return;
    }
    await this.player.pause();
    this.setStatus(PlayerStatus.Paused);
  }

  async resume(): Promise<void> {
    if (!this.player || this.status !== PlayerStatus.Paused) {
      return;
    }
    await this.player.play();
    this.setStatus(PlayerStatus.Playing);
  }

  async seek(positionMs: number): Promise<void> {
    if (!this.player) {
      return;
    }
    try {
      await this.player.seek(positionMs);
      this.setPosition(positionMs);
      this.setDuration(this.player.duration);
    } catch (err) {
      Logger.warn('Player seek failed', String(err));
    }
  }

  async stop(): Promise<void> {
    if (!this.player) {
      return;
    }
    await this.player.stop();
    this.setStatus(PlayerStatus.Idle);
    this.safeClose();
  }

  async release(): Promise<void> {
    if (!this.player) {
      this.safeClose();
      return;
    }
    try {
      await this.player.release();
    } catch (err) {
      Logger.warn('Player release failed', String(err));
    }
    this.player = undefined;
    this.setStatus(PlayerStatus.Idle);
    this.safeClose();
  }

  private safeClose(): void {
    if (this.playingFile) {
      try {
        fs.closeSync(this.playingFile);
      } catch (err) {
        Logger.warn('Close play file failed', String(err));
      }
      this.playingFile = null;
    }
    this.durationMs = 0;
  }
}

export const playerService = new PlayerService();
