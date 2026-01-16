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
import { playerService } from '../services/PlayerService';
import { PlayerStatus } from '../model/Enums';
import { TopBar } from '../components/TopBar';
import { formatDuration } from '../utils/TimeUtils';
import { appState } from '../store/AppState';

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

interface PlayerParams {
  name?: string;
  path?: string;
  durationMs?: number;
}

@Entry
@Component
struct PlayerPage {
  @State name: string = '';
  @State path: string = '';
  @State durationMs: number = 0;
  @State positionMs: number = 0;
  @State status: PlayerStatus = PlayerStatus.Idle;
  @State isSeeking: boolean = false;
  @State seekValue: number = 0;

  @Builder
  private buildTrailing() {
    Button('Back').onClick(() => router.back());
  }

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
      .width(200)
      .height(200)
      .backgroundColor(COLORS.accentSoft)
      .borderRadius(120)
      .opacity(0.6)
      .align(Alignment.TopEnd)
      .offset({ x: 36, y: -50 });

      Column() {
      }
      .width(150)
      .height(150)
      .backgroundColor('#f7d9c4')
      .borderRadius(100)
      .opacity(0.5)
      .align(Alignment.BottomStart)
      .offset({ x: -10, y: 50 });
    }
    .width('100%')
    .height('100%');
  }

  aboutToAppear() {
    const params = router.getParams() as PlayerParams;
    this.name = params.name ?? 'Recording';
    this.path = params.path ?? '';
    this.durationMs = params.durationMs ?? 0;

    playerService.subscribe((status, positionMs, durationMs) => {
      // Keep UI in sync when playback finishes
      if (durationMs > 0) {
        this.durationMs = durationMs;
      }
      if (this.durationMs > 0 && positionMs >= this.durationMs) {
        this.status = PlayerStatus.Completed;
        this.positionMs = this.durationMs;
        this.isSeeking = false;
        this.seekValue = 100;
        return;
      }
      this.status = status;
      if (status === PlayerStatus.Completed && this.durationMs > 0) {
        this.positionMs = this.durationMs;
        this.isSeeking = false;
        this.seekValue = 100;
        return;
      }
      this.positionMs = positionMs;
    });
  }

  aboutToDisappear() {
    playerService.release();
  }

  private async playOrResume(): Promise<void> {
    if (!this.path) {
      appState.showToast('No file to play');
      return;
    }
    if (this.status === PlayerStatus.Idle || this.status === PlayerStatus.Completed) {
      try {
        await playerService.play(this.path);
      } catch (err) {
        appState.showToast(`Play failed: ${String(err)}`);
      }
      return;
    }
    if (this.status === PlayerStatus.Paused) {
      await playerService.resume();
    }
  }

  private async togglePlayback(): Promise<void> {
    if (this.status === PlayerStatus.Playing) {
      await playerService.pause();
      return;
    }
    if (this.status === PlayerStatus.Completed) {
      // Restart from beginning when user taps after completion
      this.positionMs = 0;
      this.isSeeking = false;
      this.seekValue = 0;
    }
    await this.playOrResume();
  }

  private async seekToPercent(value: number): Promise<void> {
    if (!this.durationMs || this.durationMs <= 0) {
      return;
    }
    let percent = value;
    if (percent < 0) {
      percent = 0;
    }
    if (percent > 100) {
      percent = 100;
    }
    const target = Math.floor(this.durationMs * percent / 100);
    await playerService.seek(target);
  }

  build() {
    Stack() {
      this.buildBackground();

      Column() {
        TopBar({ title: 'Player', trailing: this.buildTrailing });

        Column() {
          Column({ space: 12 }) {
            Text(this.name)
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .fontColor(COLORS.ink);
            Text(`${formatDuration(this.positionMs)} / ${formatDuration(this.durationMs)}`)
              .fontSize(14)
              .fontColor(COLORS.muted);
            Slider({
              value: this.isSeeking
                ? this.seekValue
                : (this.durationMs > 0 ? this.positionMs / this.durationMs * 100 : 0),
              min: 0,
              max: 100,
              step: 1
            })
              .blockColor(COLORS.accentWarm)
              .trackColor(COLORS.accent)
              .selectedColor(COLORS.accentWarm)
              .showSteps(false)
              .showTips(false)
              .onChange((value: number, mode: SliderChangeMode) => {
                if (mode === SliderChangeMode.Begin) {
                  this.isSeeking = true;
                }
                this.seekValue = value;
                if (mode === SliderChangeMode.End) {
                  this.isSeeking = false;
                  void this.seekToPercent(value);
                }
              });

            Row({ space: 12 }) {
              Button(this.status === PlayerStatus.Playing ? 'Pause' : 'Play')
                .fontColor('#ffffff')
                .backgroundColor(this.status === PlayerStatus.Playing ? COLORS.accentWarm : COLORS.accent)
                .borderRadius(18)
                .padding({
                  left: 20,
                  right: 20,
                  top: 8,
                  bottom: 8
                })
                .onClick(() => this.togglePlayback());
            }
          }
          .alignItems(HorizontalAlign.Center)
          .padding({
            left: 20,
            right: 20,
            top: 18,
            bottom: 16
          })
          .backgroundColor(COLORS.card)
          .borderRadius(24)
          .shadow({
            radius: 12,
            color: '#22000000',
            offsetX: 0,
            offsetY: 6
          })
          .width('100%');
        }
        .padding({ left: 16, right: 16, top: 16 });
      }
      .width('100%')
      .height('100%')
      .backgroundColor('transparent');
    }
    .height('100%')
    .width('100%');
  }
}
