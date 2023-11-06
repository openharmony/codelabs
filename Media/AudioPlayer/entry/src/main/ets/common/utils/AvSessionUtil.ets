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
import { AudioPlayerController } from '../../controller/AudioPlayerControllerl';
import { PlayBarModel } from '../model/PlayBarModel';
import Logger from '../utils/Logger';
import { GlobalContext } from './GlobalContext';

const TAG = '[AvSessionUtil]';

/**
 * Media session tool class.
 */
export class AvSessionUtil {
  /**
   * Initializing the playback control center.
   *
   * @param context Context.
   */
  public static initAvSession(playBarModel: PlayBarModel) {
    return new Promise<string>(async () => {
      try {
        // Sets the current music metadata.
        let metadata = {
          assetId: playBarModel!.musicItem!.id.toString(),
          title: playBarModel!.musicItem!.name,
          artist: playBarModel!.musicItem!.singer
        } as avSession.AVMetadata;
        let currentSession = GlobalContext.getContext().getObject('currentSession') as avSession.AVSession;
        await currentSession.setAVMetadata(metadata);

        // Set the playback status and give an initial status.
        AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PLAY);

        // The following are all callback listeners. Currently, only the playback and pause functions are implemented.
        currentSession.on('play', () => {
          Logger.info(TAG, 'Invoke the playback method of avSession.');
          let playController = GlobalContext.getContext().getObject('audioPlayerController') as AudioPlayerController;
          playController.play(playBarModel!.musicItem!.rawFileDescriptor, playBarModel.playValue);
          playBarModel.playStateIcon = $r('app.media.ic_play');
          AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PLAY);
        });

        // Registering and suspending command listening.
        currentSession.on('pause', () => {
          Logger.info(TAG, 'Invoke the pause method of avSession.');
          let playController = GlobalContext.getContext().getObject('audioPlayerController') as AudioPlayerController;
          playController.pause();
          playBarModel.playStateIcon = $r('app.media.ic_pause');
          AvSessionUtil.setAVPlayState(avSession.PlaybackState.PLAYBACK_STATE_PAUSE);
        });
      }
      catch (err) {
        Logger.info(TAG, `initAvSession ${JSON.stringify(err)}`);
      }
    })
  }

  /**
   * Setting the Playback Status of the Playback Control Center.
   *
   * @param state Status of the Playback Control Center.
   */
  public static setAVPlayState(state: avSession.PlaybackState) {
    if (state === undefined) {
      Logger.info(TAG, 'setAVPlayState fail,state is empty.');
      return;
    }
    let currentSession = GlobalContext.getContext().getObject('currentSession') as avSession.AVSession;
    currentSession.setAVPlaybackState({ state: state })
      .then(() => {
        Logger.info(TAG, 'setAVPlaybackState successfully');
      })
      .catch((err: Error) => {
        Logger.info(TAG, `setAVPlaybackState : ERROR : ${err}`);
      });
  }

  /**
   * Setting the Display Data of the Playback Control Center.
   *
   * @param context Context.
   */
  public static setAVMetadata(playBarModel: PlayBarModel) {
    let metadata = {
      assetId: playBarModel!.musicItem!.id.toString(),
      title: playBarModel!.musicItem!.name,
      artist: playBarModel!.musicItem!.singer
    } as avSession.AVMetadata;
    let currentSession = GlobalContext.getContext().getObject('currentSession') as avSession.AVSession;
    currentSession.setAVMetadata(metadata);
  }
}