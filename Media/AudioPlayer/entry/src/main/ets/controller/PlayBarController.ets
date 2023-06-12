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

const TAG = '[PlayBarController]';

export class PlayBarController {
  /**
   * Toggle playback mode.
   *
   * @param context Context.
   */
  switchPlayMode(context) {
    if (CommonUtil.isEmpty(context)) {
      Logger.info(TAG, 'switchPlayMode fail,context is empty.');
      return;
    }
    context.playMode = (context.playMode + 1) % CommonConstants.MODE_QUANTITY;
    if (context.playMode === PlayMode.LIST_LOOP) {
      context.playModeIcon = $r('app.media.ic_list_mode');
    } else if (context.playMode === PlayMode.SINGLE_LOOP) {
      context.playModeIcon = $r('app.media.ic_single_mode');
    } else {
      context.playModeIcon = $r('app.media.ic_random_mode');
      // Random mode, generating random play list.
      globalThis.randomMusicList = [];
      globalThis.randomList = CommonUtil.getRandomNumber(CommonConstants.MUSICS_QUANTITY);
    }
  }

  /**
   * Switching the Playback Status.
   *
   * @param context Context.
   */
  switchPlayState(context) {
    if (CommonUtil.isEmpty(context)) {
      Logger.info(TAG, 'switchPlayState fail,context is empty.');
      return;
    }
    if (context.playState === PlayState.PLAY) {
      context.playState = PlayState.PAUSE;
      context.playStateIcon = $r('app.media.ic_pause');
      globalThis.AudioPlayerController.pause();
      AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PAUSE);
    } else {
      context.playState = PlayState.PLAY;
      context.playStateIcon = $r('app.media.ic_play');
      Logger.info(TAG, `rawFileDescriptor: ${context.musicItem.rawFileDescriptor} playValue ${context.playValue}`);
      globalThis.AudioPlayerController.play(context.musicItem.rawFileDescriptor, context.playValue);
      AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PLAY);
    }
  }

  /**
   * Previous song.
   *
   * @param context Context.
   */
  getPreviousMusic(context) {
    if (CommonUtil.isEmpty(context)) {
      Logger.info(TAG, 'getPreviousMusic fail,context is empty.');
      return;
    }
    if (context.playMode === PlayMode.RANDOM_PLAY) {
      // Obtain the previous song in the random list.
      let preIndex = 0;
      globalThis.randomList.forEach((item, index) => {
        if (item === context.musicItem.id) {
          preIndex = index - 1 < 0 ? globalThis.randomList.length - 1 : index - 1;
          return;
        }
      })
      context.musicItem = context.musicList[globalThis.randomList[preIndex]];
    } else {
      let preIndex = 0;
      context.musicList.forEach((item, index) => {
        if (item.id === context.musicItem.id) {
          preIndex = index - 1 < 0 ? context.musicList.length - 1 : index - 1;
          return;
        }
      })
      context.musicItem = context.musicList[preIndex];
    }
    Logger.info(TAG, 'state pre called' + context.playValue);
    globalThis.AudioPlayerController.play(context.musicItem.rawFileDescriptor, 0);
    context.playState = PlayState.PLAY;
    context.playStateIcon = $r('app.media.ic_play');
    // Sets the current music metadata.
    AvSessionUtil.setAVMetadata(context);
    AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PLAY);
  }

  /**
   * Get next song.
   *
   * @param context Context.
   */
  getNextMusic(context) {
    if (CommonUtil.isEmpty(context)) {
      Logger.info(TAG, 'getNextMusic fail,context is empty.');
      return;
    }
    // Random mode, which is used to obtain the next song randomly.
    if (context.playMode === PlayMode.RANDOM_PLAY) {
      // Obtain the previous song in the random list.
      let nextIndex = globalThis.randomList.length - 1;
      globalThis.randomList.forEach((item, index) => {
        if (item === context.musicItem.id) {
          nextIndex = index + 1 > globalThis.randomList.length - 1 ? 0 : index + 1;
          return;
        }
      })
      context.musicItem = context.musicList[globalThis.randomList[nextIndex]]
    } else {
      let nextIndex = context.musicList.length - 1;
      context.musicList.forEach((item, index) => {
        if (item.id === context.musicItem.id) {
          nextIndex = index + 1 > context.musicList.length - 1 ? 0 : index + 1;
          return;
        }
      })
      context.musicItem = context.musicList[nextIndex];
    }
    globalThis.AudioPlayerController.play(context.musicItem.rawFileDescriptor, 0);
    context.playState = PlayState.PLAY;
    context.playStateIcon = $r('app.media.ic_play');
    // Sets the current music metadata.
    AvSessionUtil.setAVMetadata(context);
    AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PLAY);
  }
}