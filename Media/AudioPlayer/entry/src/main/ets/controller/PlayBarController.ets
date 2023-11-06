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

import avSession from '@ohos.multimedia.avsession';
import { CommonUtil } from '../common/utils/CommonUtil';
import { AvSessionUtil } from '../common/utils/AvSessionUtil';
import { PlayState, PlayMode, CommonConstants } from '../common/constants/CommonConstants';
import Logger from '../common/utils/Logger';
import { GlobalContext } from '../common/utils/GlobalContext';
import { AudioPlayerController } from './AudioPlayerControllerl';
import { MusicItem } from '../viewmodel/MusicItem';
import { PlayBarModel } from '../common/model/PlayBarModel';

const TAG = '[PlayBarController]';

export class PlayBarController {
  public playBarModel: PlayBarModel;

  constructor(playBarModel: PlayBarModel) {
    this.playBarModel = playBarModel;
  }

  /**
   * Toggle playback mode.
   */
  switchPlayMode() {
    this.playBarModel.playMode = (this.playBarModel.playMode + 1) % CommonConstants.MODE_QUANTITY;
    if (this.playBarModel.playMode === PlayMode.LIST_LOOP) {
      this.playBarModel.playModeIcon = $r('app.media.ic_list_mode');
    } else if (this.playBarModel.playMode === PlayMode.SINGLE_LOOP) {
      this.playBarModel.playModeIcon = $r('app.media.ic_single_mode');
    } else {
      this.playBarModel.playModeIcon = $r('app.media.ic_random_mode');
      // Random mode, generating random play list.
      let randomList = CommonUtil.getRandomNumber(CommonConstants.MUSICS_QUANTITY);
      GlobalContext.getContext().setObject('randomList', randomList);
    }
  }

  /**
   * Switching the Playback Status.
   *
   * @param playState
   */
  switchPlayState(playState: number) {
    let playController = GlobalContext.getContext().getObject('audioPlayerController') as AudioPlayerController;
    if (playState === PlayState.PLAY) {
      this.playBarModel.playState = PlayState.PAUSE;
      this.playBarModel.playStateIcon = $r('app.media.ic_pause');
      playController.pause();
      AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PAUSE);
    } else {
      this.playBarModel.playState = PlayState.PLAY;
      this.playBarModel.playStateIcon = $r('app.media.ic_play');
      Logger.info(TAG, `rawFileDescriptor: ${this.playBarModel!.musicItem!.rawFileDescriptor},
        playValue ${this.playBarModel!.playValue}`);
      playController.play(this.playBarModel.musicItem!.rawFileDescriptor, this.playBarModel.playValue);
      AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PLAY);
    }
  }

  /**
   * Previous song.
   *
   * @param musicList
   * @param playState
   */
  getPreviousMusic(musicList: Array<MusicItem>, playState: number) {
    if (this.playBarModel.playMode === PlayMode.RANDOM_PLAY) {
      // Obtain the previous song in the random list.
      let preIndex = 0;
      let randomList = GlobalContext.getContext().getObject('randomList') as number[];
      randomList.forEach((item: number, index: number) => {
        if (item === this.playBarModel!.musicItem!.id) {
          preIndex = index - 1 < 0 ? randomList.length - 1 : index - 1;
          return;
        }
      })
      this.playBarModel.musicItem = musicList[randomList[preIndex]];
    } else {
      let preIndex = 0;
      musicList.forEach((item: MusicItem, index: number) => {
        if (item.id === this.playBarModel.musicItem!.id) {
          preIndex = index - 1 < 0 ? musicList.length - 1 : index - 1;
          return;
        }
      })
      this.playBarModel.musicItem = musicList[preIndex];
    }
    Logger.info(TAG, 'state pre called' + this.playBarModel.playValue);
    let playController = GlobalContext.getContext().getObject('audioPlayerController') as AudioPlayerController;
    playController.play(this.playBarModel.musicItem.rawFileDescriptor, 0);
    this.playBarModel.playState = PlayState.PLAY;
    this.playBarModel.playStateIcon = $r('app.media.ic_play');
    // Sets the current music metadata.
    AvSessionUtil.setAVMetadata(this.playBarModel);
    AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PLAY);
  }

  /**
   * Get next song.
   *
   * @param musicList
   * @param playState
   */
  getNextMusic(musicList: Array<MusicItem>) {
    // Random mode, which is used to obtain the next song randomly.
    if (this.playBarModel.playMode === PlayMode.RANDOM_PLAY) {
      // Obtain the previous song in the random list.
      let randomList = GlobalContext.getContext().getObject('randomList') as number[];
      let nextIndex = randomList.length - 1;
      randomList.forEach((item: number, index: number) => {
        if (item === this.playBarModel!.musicItem!.id) {
          nextIndex = index + 1 > randomList.length - 1 ? 0 : index + 1;
          return;
        }
      })
      this.playBarModel.musicItem = musicList[randomList[nextIndex]]
    } else {
      let nextIndex = musicList.length - 1;
      musicList.forEach((item: MusicItem, index: number) => {
        if (item.id === this.playBarModel!.musicItem!.id) {
          nextIndex = index + 1 > musicList.length - 1 ? 0 : index + 1;
          return;
        }
      })
      this.playBarModel.musicItem = musicList[nextIndex];
    }
    let playController = GlobalContext.getContext().getObject('audioPlayerController') as AudioPlayerController;
    playController.play(this.playBarModel.musicItem.rawFileDescriptor, 0);
    this.playBarModel.playState = PlayState.PLAY;
    this.playBarModel.playStateIcon = $r('app.media.ic_play');
    // Sets the current music metadata.
    AvSessionUtil.setAVMetadata(this.playBarModel);
    AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PLAY);
  }
}