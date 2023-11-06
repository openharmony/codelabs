/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
import { MusicItem } from '../viewmodel/MusicItem';
import { PlayMode, PlayerState, StateEvent, CommonConstants } from '../common/constants/CommonConstants';
import { BackgroundTaskUtil } from '../common/utils/BackgroundTaskUtil';
import { AvSessionUtil } from '../common/utils/AvSessionUtil';
import Logger from '../common/utils/Logger';
import { GlobalContext } from '../common/utils/GlobalContext';
import { PlayBarModel } from '../common/model/PlayBarModel';

const TAG = '[AudioPlayerController]';

export class AudioPlayerController {
  public playBarModel: PlayBarModel;
  avPlayer?: media.AVPlayer;
  musicList: Array<MusicItem> = [];
  currentTimeMs: number = 0; // Current playback position.
  timeId = CommonConstants.DEFAULT_TIME_ID;
  avPlayerState = PlayerState.IDLE;

  constructor(playBarModel: PlayBarModel) {
    this.playBarModel = playBarModel;
    this.initAudioPlayer();
  }

  setMusicList(musicList: Array<MusicItem>) {
    this.musicList = musicList;
  }

  /**
   * Initializes the AVPlayer instance.
   */
  initAudioPlayer() {
    media.createAVPlayer((error, video) => {
      if (video === undefined) {
        this.avPlayer = video;
        Logger.error(TAG, `createAVPlayer fail, error: ${error}`);
      } else {
        this.avPlayer = video;
        Logger.info(TAG, 'createAVPlayer success');
      }
    });
  }

  /**
   * Registering the avplayer callback function.
   *
   * @param context Context.
   */
  setEventCallBack() {
    if (this.avPlayer === undefined) {
      return;
    }
    // Callback function for state machine changes.
    this.avPlayer.on('stateChange', async (state) => {
      if (this.avPlayer === undefined) {
        return;
      }
      switch (state) {
        case StateEvent.IDLE: // This state machine is triggered after the reset interface is successfully invoked.
          Logger.info(TAG, 'state idle called');
          break;
        case StateEvent.INITIALIZED: // This status is reported after the playback source is set.
          Logger.info(TAG, 'state initialized called');
          this.avPlayerState = PlayerState.INITIALIZED;
          this.avPlayer.prepare().then(() => {
            Logger.info(TAG, 'prepare success');
          }, (err) => {
            Logger.error(TAG, `prepare failed,error message is: ${err.message}`);
          })
          break;
        case StateEvent.PREPARED:
          Logger.info(TAG, 'state prepared called');
          this.avPlayer.play();
          break;
        case StateEvent.PLAYING:
          Logger.info(TAG, `state playing called: ${this.currentTimeMs}`);
          if (this.currentTimeMs > 0) {
            this.avPlayer.seek(this.currentTimeMs);
          }
          this.progressChange();
          break;
        case StateEvent.COMPLETED:
          Logger.info(TAG, 'state completed called');
          BackgroundTaskUtil.stopContinuousTask(getContext(this))
          this.loopPlay();
          break;
        default:
          Logger.error('unknown state: ' + state);
          break;
      }
    })
  }

  /**
   * Play Audio.
   *
   * @param src Resource path.
   * @param seekTo Playing position.
   */
  async play(src: media.AVFileDescriptor, seekTo: number) {
    Logger.info(TAG, 'audioPlayer play');
    if (this.timeId === CommonConstants.DEFAULT_TIME_ID) {
      clearInterval(this.timeId);
    }
    // Enable a long-term task to allow long-term playback in the background.
    BackgroundTaskUtil.startContinuousTask(getContext(this));
    this.currentTimeMs = Math.max(0, seekTo);
    if (this.avPlayer === undefined) {
      return;
    }
    if (this.avPlayerState === PlayerState.INITIALIZED) {
      await this.avPlayer.reset();
      Logger.info(TAG, 'play reset success');
    }
    this.avPlayer.fdSrc = src;
  }

  async pause() {
    Logger.info(TAG, 'audioPlayer play');
    if (this.timeId === CommonConstants.DEFAULT_TIME_ID) {
      clearInterval(this.timeId);
    }
    if (this.avPlayer === undefined) {
      return;
    }
    BackgroundTaskUtil.stopContinuousTask(getContext(this));
    await this.avPlayer.pause();
  }

  async release() {
    Logger.info(TAG, 'audioPlayer release');
    if (typeof (this.avPlayer) !== 'undefined') {
      if (this.timeId === CommonConstants.DEFAULT_TIME_ID) {
        clearInterval(this.timeId);
      }
      await this.avPlayer.release();
      this.avPlayer = undefined;
    }
  }

  progressChange() {
    this.playBarModel.playValue = this.currentTimeMs;
    if (this.avPlayer === undefined) {
      return;
    }
    this.playBarModel.totalValue = this.avPlayer.duration;
    this.timeId = setInterval(() => {
      if (this.avPlayer === undefined) {
        return;
      }
      this.playBarModel.playValue = this.avPlayer.currentTime;
    }, CommonConstants.DELAY_TIME)
  }

  loopPlay() {
    this.currentTimeMs = 0;
    // Play the next song in loop mode.
    switch (this.playBarModel.playMode) {
      case PlayMode.SINGLE_LOOP:
        this.play(this.playBarModel!.musicItem!.rawFileDescriptor, 0);
        break;
      case PlayMode.LIST_LOOP:
        let nextIndex = this.musicList.length - 1;
        this.musicList.forEach((item, index) => {
          if (item.id === this.playBarModel!.musicItem!.id) {
            nextIndex = index + 1 > this.musicList.length - 1 ? 0 : index + 1;
            return;
          }
        })
        this.playBarModel.musicItem = this.musicList[nextIndex];
        this.play(this.playBarModel.musicItem.rawFileDescriptor, 0);
      // Sets the current music metadata.
        AvSessionUtil.setAVMetadata(this.playBarModel);
        break;
      case PlayMode.RANDOM_PLAY:
      // Obtain the previous song in the random list.
        let randomList = GlobalContext.getContext().getObject('randomList') as number[];
        let nextIndexRandom = randomList.length - 1;
        randomList.forEach((item, index) => {
          if (item === this.playBarModel!.musicItem!.id) {
            nextIndexRandom = index + 1 > randomList.length - 1 ? 0 : index + 1;
            return;
          }
        })
        this.playBarModel.musicItem = this.musicList[randomList[nextIndexRandom]];
        this.play(this.playBarModel.musicItem.rawFileDescriptor, 0);
      // Sets the current music metadata.
        AvSessionUtil.setAVMetadata(this.playBarModel);
        break;
      default:
        Logger.error('unknown mode: ' + this.playBarModel.playMode);
        break;
    }
  }
}