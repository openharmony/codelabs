/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import prompt from '@system.prompt';
import media from '@ohos.multimedia.media';
import Song from '../media/been/SongInfo'

let AudioPlayerState = {
  IDLE: -1,
  LOAD: 0,
  PLAY: 1,
  PAUSE: 2,
  STOP: 3,
  ERROR: 4,
  FINISH: 5,
  TIME_UPDATE: 6,
  VOLUME_CHANGE: 7
}

export { AudioPlayerState }

export class AudioPlayer {
  playlist = new Array<Song>();
  index = 0;
  #player;
  #statusChangedListener;
  #intervalID;
  #isAutoPlay = true;
  #currentTimeMs = 0;
  #currentState = AudioPlayerState.IDLE;
  constructor() {
    this.initAudioPlayer();
  }

  initAudioPlayer() {
    this.#player = media.createAudioPlayer();
    this.setOperateListener();
    this.setStatusListener();
  }

  setOperateListener() {
    this.#player.on('finish', () => {
      this.#currentState = AudioPlayerState.FINISH;
      if (typeof (this.#statusChangedListener) != 'undefined') {
        this.#statusChangedListener(AudioPlayerState.FINISH);
      }
    });
    this.#player.on('timeUpdate', (seekTime, action) => {
      this.#currentTimeMs = parseInt(seekTime);
      if (typeof (this.#statusChangedListener) != 'undefined') {
        this.#statusChangedListener(AudioPlayerState.TIME_UPDATE, JSON.stringify(action));
      }
    });
    this.#player.on('volumeChange', () => {
      if (typeof (this.#statusChangedListener) != 'undefined') {
        this.#statusChangedListener(AudioPlayerState.VOLUME_CHANGE);
      }
    });
  }

  setStatusListener() {
    this.#player.on('dataLoad', () => {
      this.#currentState = AudioPlayerState.LOAD;
      if (this.#isAutoPlay) {
        this.#player.play();
      }
      if (typeof (this.#statusChangedListener) != 'undefined') {
        this.#statusChangedListener(AudioPlayerState.LOAD);
      }
    });
    this.#player.on('play', () => {
      this.#currentState = AudioPlayerState.PLAY;
      if (this.#currentTimeMs > 0) {
        this.seek(this.#currentTimeMs);
      }
      if (typeof (this.#statusChangedListener) != 'undefined') {
        this.#statusChangedListener(AudioPlayerState.PLAY);
      }
    });
    this.#player.on('pause', () => {
      this.#currentState = AudioPlayerState.PAUSE;
      if (typeof (this.#statusChangedListener) != 'undefined') {
        this.#statusChangedListener(AudioPlayerState.PAUSE);
      }
    });
    this.#player.on('stop', () => {
      this.#currentState = AudioPlayerState.STOP;
      if (typeof (this.#statusChangedListener) != 'undefined') {
        this.#statusChangedListener(AudioPlayerState.STOP);
      }
    });
    this.#player.on('error', (err) => {
      this.#currentState = AudioPlayerState.ERROR;
      if (typeof (this.#statusChangedListener) != 'undefined') {
        this.#statusChangedListener(AudioPlayerState.ERROR, err);
      }
    });
  }

  release() {
    if (typeof (this.#player) != 'undefined') {
      this.#player.release();
      this.#player = undefined;
    }
  }

  addPlaySrc(name, src) {
    let self = this;
    self.playlist.push(new Song(name, src, 0))
  }

  setOnStatusChangedListener(callback) {
    this.#statusChangedListener = callback;
  }

  playList(index, isAutoPlay) {
    if (index < 0 || index >= this.playlist.length) {
      return 0;
    }
    this.index = index;
    var source = this.playlist[index].getFileUri();
    if (typeof (source) === 'undefined') {
      return;
    }
    this.play(source, -1, isAutoPlay);
  }

  getDuration() {
    if (this.playlist[this.index].getDuration() > 0) {
      return this.playlist[this.index].getDuration();
    }
    if (this.#player.AudioPlayerState === AudioPlayerState.IDLE) {
      return 0;
    }
    this.playlist[this.index].setDuration(Math.min(this.#player.duration, 97615));
    return this.playlist[this.index].getDuration();
  }

  play(src, seekTo, autoPlay) {
    this.#player.reset();
    this.#isAutoPlay = autoPlay;
    this.#currentTimeMs = Math.max(0, seekTo);
    this.#player.src = src;
  }

  resume() {
    if (this.#currentState === AudioPlayerState.PAUSE) {
      this.#player.play();
    }
  }

  pause() {
    if (this.#currentState === AudioPlayerState.PLAY) {
      this.#player.pause();
    }
  }

  seek(ms) {
    this.#currentTimeMs = parseInt(ms);
    if (this.#currentState === AudioPlayerState.PLAY) {
      this.#player.seek(this.#currentTimeMs);
    }
  }

  stop() {
    if (this.#currentState === AudioPlayerState.PLAY) {
      this.#player.stop();
    }
  }
}