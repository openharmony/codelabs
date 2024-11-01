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

import UIAbility from '@ohos.app.ability.UIAbility';
import Window from '@ohos.window';
import avSession from '@ohos.multimedia.avsession';
import { AudioPlayerController } from '../controller/AudioPlayerControllerl';
import Logger from '../common/utils/Logger';
import { GlobalContext } from '../common/utils/GlobalContext';
import { PlayBarModel } from '../common/model/PlayBarModel';

export default class EntryAbility extends UIAbility {
  onDestroy() {
    Logger.info('Ability onDestroy');
    // Release resources.
    let audioPlayerController = GlobalContext.getContext().getObject('audioPlayerController') as AudioPlayerController;
    audioPlayerController.release();

    let currentSession = GlobalContext.getContext().getObject('currentSession') as avSession.AVSession;
    currentSession.off('play');
    currentSession.off('pause');
    currentSession.deactivate().then(() => {
      currentSession.destroy();
    });
  }

  onWindowStageCreate(windowStage: Window.WindowStage) {
    // Main window is created, set main page for this ability.
    let context = this.context;
    let resourceManager = context.resourceManager;
    GlobalContext.getContext().setObject('resourceManager', resourceManager);

    // Create an audioPlayer instance.
    let audioPlayerController = new AudioPlayerController(new PlayBarModel());
    GlobalContext.getContext().setObject('audioPlayerController', audioPlayerController);

    // Creating an Audio Session.
    avSession.createAVSession(context, 'AudioAppSample', 'audio').then((session) => {
      session.activate();
      GlobalContext.getContext().setObject('currentSession', session);
    }).catch((err: Error) => {
      Logger.error(`createAVSession fail, Cause: ${err}`);
    });

    windowStage.loadContent('pages/AudioStartUp', (err, data) => {
      if (err.code) {
        Logger.error('testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      Logger.info('testTag', 'Succeeded in loading the content. Data: %{public}s', JSON.stringify(data) ?? '');
    });
  }
}