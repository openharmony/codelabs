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

import router from '@ohos.router';
import type { AudioRecord } from '../model/AudioRecord';
import { RecordStatus } from '../model/Enums';
import { RecorderControls } from '../components/RecorderControls';
import { AudioItemCard } from '../components/AudioItemCard';
import { ConfirmDialog } from '../components/ConfirmDialog';
import { TopBar } from '../components/TopBar';
import { Toast } from '../components/Toast';
import { recordStore } from '../store/RecordStore';
import { settingsStore } from '../store/SettingsStore';
import { appState } from '../store/AppState';
import { permissionService } from '../services/PermissionService';
import { recorderService } from '../services/RecorderService';
import { playerService } from '../services/PlayerService';
import { buildRecordPath, deleteFile, getFileSize } from '../utils/FileUtils';
import { createId } from '../utils/IdUtils';
import { formatDate, formatDuration } from '../utils/TimeUtils';
import { Logger } from '../utils/Logger';
import { AppContextHolder } from '../utils/AppContext';
import { RECORDING_EXTENSION } from '../constants/Paths';

interface ThemeColors {
  background: string;
  backgroundAlt: string;
  card: string;
  ink: string;
  muted: string;
  accent: string;
  accentSoft: string;
  accentWarm: string;
  line: string;
}

const COLORS: ThemeColors = {
  background: '#f4efe6',
  backgroundAlt: '#dfeeea',
  card: '#fff8f0',
  ink: '#1f1b16',
  muted: '#5f5a52',
  accent: '#1f6f6d',
  accentSoft: '#cfe8e4',
  accentWarm: '#ee6c4d',
  line: '#e7dccb'
};

const STRIPES: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];

@Entry
@Component
struct RecordPage {
  @State status: RecordStatus = RecordStatus.Idle;
  @State durationMs: number = 0;
  @State records: AudioRecord[] = [];
  @State toastMessage: string = '';
  @State toastVisible: boolean = false;
  @State confirmVisible: boolean = false;
  @State pendingDelete: AudioRecord | null = null;
  private startAt: number = 0;
  private accumulatedMs: number = 0;
  private timerId: number = -1;
  private activePath: string = '';
  private configName: string = '';
  private unsubscribeRecordStore?: () => void;

  @Builder
  private buildBackground() {
    Stack() {
      Column() {
        Blank()
          .height('32%')
          .backgroundColor(COLORS.backgroundAlt);
        Blank()
          .layoutWeight(1)
          .backgroundColor(COLORS.background);
      }
      .width('100%')
      .height('100%');

      Column() {
        ForEach(STRIPES, (value: number) => {
          Blank()
            .height(6)
            .width('100%')
            .backgroundColor(value % 2 === 0 ? '#ffffff' : '#f0e6d6')
            .opacity(0.15);
        }, (value: number) => `${value}`);
      }
      .width('100%')
      .height('100%');

      Column() {
      }
      .width(220)
      .height(220)
      .backgroundColor(COLORS.accentSoft)
      .borderRadius(120)
      .opacity(0.6)
      .align(Alignment.TopEnd)
      .offset({ x: 40, y: -60 });

      Column() {
      }
      .width(160)
      .height(160)
      .backgroundColor('#f7d9c4')
      .borderRadius(100)
      .opacity(0.5)
      .align(Alignment.BottomStart)
      .offset({ x: -20, y: 60 });
    }
    .width('100%')
    .height('100%');
  }

  @Builder
  private buildTopBarTrailing() {
    Row({ space: 12 }) {
      Button('Settings').onClick(() => router.pushUrl({ url: 'pages/SettingsPage' }));
    }
  }

  @Builder
  private buildRecordListHeader() {
    Row() {
      Text('All Recordings')
        .fontSize(18)
        .fontWeight(FontWeight.Medium)
        .fontColor(COLORS.ink);
      Blank();
    }
    .margin({ top: 16, bottom: 12 })
  }

  private refreshRecords(): void {
    this.records = recordStore.getRecords().map((item: AudioRecord) => this.cloneRecord(item));
  }

  private subscribeRecordStore(): void {
    if (this.unsubscribeRecordStore) {
      this.unsubscribeRecordStore();
    }
    this.unsubscribeRecordStore = recordStore.subscribe((records) => {
      this.records = records.map((item: AudioRecord) => this.cloneRecord(item));
    });
  }

  private cloneRecord(item: AudioRecord): AudioRecord {
    const clone: AudioRecord = {
      id: item.id,
      name: item.name,
      path: item.path,
      durationMs: item.durationMs,
      sizeBytes: item.sizeBytes,
      createdAt: item.createdAt,
      format: item.format,
      sampleRate: item.sampleRate,
      channels: item.channels
    };
    return clone;
  }

  aboutToAppear() {
    this.refreshRecords();
    this.subscribeRecordStore();

    recorderService.subscribe((status) => {
      this.status = status;
      if (status === RecordStatus.Idle || status === RecordStatus.Paused) {
        this.stopTimers();
      }
    });

    appState.subscribeToast((message) => {
      if (message) {
        this.toastMessage = message;
        this.toastVisible = true;
        setTimeout(() => {
          this.toastVisible = false;
          this.toastMessage = '';
          appState.clearToast();
        }, 2000);
      }
    });
  }

  aboutToDisappear() {
    this.stopTimers();
    if (this.unsubscribeRecordStore) {
      this.unsubscribeRecordStore();
      this.unsubscribeRecordStore = undefined;
    }
    playerService.release();
  }

  onPageShow() {
    this.refreshRecords();
    this.subscribeRecordStore();
  }

  private stopTimers(): void {
    if (this.timerId !== -1) {
      clearInterval(this.timerId);
      this.timerId = -1;
    }
  }

  private computeElapsed(): number {
    if (this.startAt > 0) {
      return this.accumulatedMs + (Date.now() - this.startAt);
    }
    return this.accumulatedMs;
  }

  private startTimers(): void {
    this.stopTimers();
    this.timerId = setInterval(() => {
      this.durationMs = this.computeElapsed();
    }, 200);
  }

  private async startRecording(): Promise<void> {
    const context = AppContextHolder.context;
    if (!context) {
      appState.showToast('Context not ready');
      return;
    }
    const granted = await permissionService.requestRecordingPermissions(context);
    if (!granted) {
      appState.showToast('Permission denied');
      return;
    }
    const config = settingsStore.getConfig();
    const id = createId();
    const name = `Record ${formatDate(Date.now())}`;
    const fileName = `${id}.${RECORDING_EXTENSION}`;
    let path = buildRecordPath(context.filesDir, fileName);
    Logger.info('Recording path', path);

    try {
      try {
        await recorderService.start(path, config);
      } catch (err) {
        const detail = String(err);
        if (detail.indexOf('13900002') >= 0 && context.cacheDir) {
          const fallbackPath = buildRecordPath(context.cacheDir, fileName);
          Logger.warn('Recording fallback path', fallbackPath);
          path = fallbackPath;
          await recorderService.start(path, config);
        } else {
          throw new Error(detail);
        }
      }
      this.startAt = Date.now();
      this.accumulatedMs = 0;
      this.durationMs = 0;
      this.activePath = path;
      this.configName = name;
      this.startTimers();
    } catch (err) {
      const detail = String(err);
      Logger.error('Start recording failed', detail);
      appState.showToast(`Failed to start recorder: ${detail}`);
    }
  }

  private async pauseOrResume(): Promise<void> {
    if (this.status === RecordStatus.Recording) {
      await recorderService.pause();
      if (this.startAt > 0) {
        this.accumulatedMs += Date.now() - this.startAt;
        this.startAt = 0;
      }
      this.durationMs = this.computeElapsed();
      this.stopTimers();
      return;
    }
    if (this.status === RecordStatus.Paused) {
      await recorderService.resume();
      this.startAt = Date.now();
      this.startTimers();
    }
  }

  private async stopRecording(): Promise<void> {
    if (this.status === RecordStatus.Idle) {
      return;
    }
    await recorderService.stop();
    this.stopTimers();

    if (!this.activePath) {
      return;
    }
    const finalDuration = this.computeElapsed();
    const record: AudioRecord = {
      id: createId('record'),
      name: this.configName || `Record ${formatDate(Date.now())}`,
      path: this.activePath,
      durationMs: finalDuration,
      sizeBytes: getFileSize(this.activePath),
      createdAt: this.startAt || Date.now(),
      format: RECORDING_EXTENSION,
      sampleRate: settingsStore.getConfig().sampleRate,
      channels: settingsStore.getConfig().channels
    };
    recordStore.addRecord(record);
    this.activePath = '';
    this.startAt = 0;
    this.accumulatedMs = 0;
    this.durationMs = 0;
    appState.showToast('Saved recording');
  }

  private playRecord(record: AudioRecord): void {
    router.pushUrl({
      url: 'pages/PlayerPage',
      params: {
        id: record.id,
        name: record.name,
        path: record.path,
        durationMs: record.durationMs
      }
    });
  }

  private deleteRecord(record: AudioRecord): void {
    const removed = deleteFile(record.path);
    if (removed) {
      recordStore.removeRecord(record.id);
      appState.showToast('Deleted recording');
    } else {
      appState.showToast('Delete failed');
    }
  }

  private requestDelete(record: AudioRecord): void {
    this.pendingDelete = record;
    this.confirmVisible = true;
  }

  private cancelDelete(): void {
    this.confirmVisible = false;
    this.pendingDelete = null;
  }

  private confirmDelete(): void {
    if (this.pendingDelete) {
      this.deleteRecord(this.pendingDelete);
    }
    this.pendingDelete = null;
    this.confirmVisible = false;
  }

  build() {
    Stack() {
      this.buildBackground();

      Column() {
        TopBar({ title: 'Field Recorder', trailing: this.buildTopBarTrailing });

        Column({ space: 12 }) {
          Row() {
            Blank();
            Text(this.status === RecordStatus.Recording ? 'REC' : 'READY')
              .fontSize(12)
              .fontColor('#ffffff')
              .backgroundColor(this.status === RecordStatus.Recording ? COLORS.accentWarm : COLORS.accent)
              .padding({
                left: 10,
                right: 10,
                top: 4,
                bottom: 4
              })
              .borderRadius(12)
              .letterSpacing(1);
          }

          Text(formatDuration(this.durationMs))
            .fontSize(36)
            .fontWeight(FontWeight.Bold)
            .fontFamily('HarmonyOS Sans')
            .fontColor(COLORS.ink);
          RecorderControls({
            status: this.status,
            onRecord: () => this.startRecording(),
            onPause: () => this.pauseOrResume(),
            onStop: () => this.stopRecording()
          });
        }
        .padding({
          left: 20,
          right: 20,
          top: 18,
          bottom: 16
        })
        .backgroundColor(COLORS.card)
        .borderRadius(24)
        .margin({ left: 16, right: 16, top: 16 })
        .shadow({
          radius: 12,
          color: '#22000000',
          offsetX: 0,
          offsetY: 6
        });

        Column() {
          this.buildRecordListHeader();
          if (this.records.length === 0) {
            Text('No recordings yet')
              .fontColor(COLORS.muted)
              .padding({ top: 12, bottom: 12 });
          } else {
            List() {
              ForEach(this.records, (record: AudioRecord) => {
                ListItem() {
                  AudioItemCard({
                    record,
                    editable: true,
                    onPlay: (item) => this.playRecord(item),
                    onDelete: (item) => this.requestDelete(item)
                  });
                }
              }, (record: AudioRecord) => record.id)
            }
            .layoutWeight(1)
            .width('100%')
            .padding({ bottom: 8 });
          }
        }
        .padding({ left: 16, right: 16, bottom: 16 })
        .layoutWeight(1);
      }
      .width('100%')
      .height('100%')
      .backgroundColor('transparent');

      Toast({ message: this.toastMessage, visible: this.toastVisible });
      if (this.confirmVisible && this.pendingDelete) {
        Stack() {
          Column() {
            Blank()
              .layoutWeight(1)
              .width('100%')
              .backgroundColor('#88000000');
          }
          .width('100%')
          .height('100%');

          Column() {
            ConfirmDialog({
              title: 'Confirm delete',
              message: `Delete "${this.pendingDelete.name}"?`,
              onConfirm: () => this.confirmDelete(),
              onCancel: () => this.cancelDelete()
            });
          }
          .width('80%')
          .backgroundColor('#ffffff')
          .borderRadius(12)
          .padding(8)
          .align(Alignment.Center);
        }
        .width('100%')
        .height('100%');
      }
    }
    .width('100%')
    .height('100%');
  }
}
