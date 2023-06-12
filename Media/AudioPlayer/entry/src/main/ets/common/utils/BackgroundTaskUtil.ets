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

import wantAgent from '@ohos.app.ability.wantAgent';
import backgroundTaskManager from '@ohos.resourceschedule.backgroundTaskManager';
import { CommonConstants } from '../constants/CommonConstants';
import Logger from '../utils/Logger';
import { CommonUtil } from './CommonUtil';

const TAG = '[BackgroundTaskUtil]';

/**
 * Background task tool class.
 */
export class BackgroundTaskUtil {

  /**
   * Start a long-time task in the background.
   *
   * @param context Context.
   */
  public static startContinuousTask(context) {
    if (CommonUtil.isEmpty(context)) {
      Logger.info(TAG, 'startContinuousTask fail,context is empty.');
      return;
    }
    let wantAgentInfo = {
      // Action to be performed after a notification is clicked.
      wants: [
        {
          bundleName: CommonConstants.BUNDLE_NAME,
          abilityName: CommonConstants.ABILITY_NAME
        }
      ],
      // Action type after a notification is clicked.
      operationType: wantAgent.OperationType.START_ABILITY,
      // A private value defined by the user.
      requestCode: CommonConstants.BACKGROUND_REQUEST_CODE
    };

    // Obtain the WantAgent object by using the method of the wantAgent module.
    wantAgent.getWantAgent(wantAgentInfo).then((wantAgentObj) => {
      try {
        backgroundTaskManager.startBackgroundRunning(context, backgroundTaskManager.BackgroundMode.AUDIO_PLAYBACK,
          wantAgentObj).then(() => {
          Logger.info(TAG, 'startBackgroundRunning succeeded');
        }).catch((err) => {
          Logger.error(TAG, 'startBackgroundRunning failed Cause: ' + JSON.stringify(err));
        });
      } catch (error) {
        Logger.error(TAG, `startBackgroundRunning failed. code is ${error.code} message is ${error.message}`);
      }
    });
  }

  /**
   * Stopping a Long-Time Task in the Background.
   *
   * @param context context.
   */
  public static stopContinuousTask(context) {
    if (CommonUtil.isEmpty(context)) {
      Logger.info(TAG, 'stopContinuousTask fail,context is empty.');
      return;
    }
    try {
      backgroundTaskManager.stopBackgroundRunning(context).then(() => {
        Logger.info(TAG, 'stopBackgroundRunning succeeded');
      }).catch((err) => {
        Logger.error(TAG, 'stopBackgroundRunning failed Cause: ' + JSON.stringify(err));
      });
    } catch (error) {
      Logger.error(TAG, `stopBackgroundRunning failed. code is ${error.code} message is ${error.message}`);
    }
  }
}