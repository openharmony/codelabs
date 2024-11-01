/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
import Logger from '../common/utils/Logger';
import CommonConstants from '../common/constants/CommonConstants';
import Want from '@ohos.app.ability.Want';
import common from '@ohos.app.ability.common';

/**
 * Get the context.
 */
let currentContext = getContext(this);
let textMessage: string = currentContext.resourceManager.getStringSync($r('app.string.text_message'));
let returnMessage: string = currentContext.resourceManager.getStringSync($r('app.string.return_message'));

/**
 * A method class for button click events.
 */
class ButtonClickMethod {
  /**
   * Navigate to the Second Page of EntryAbility.
   */
  toEntryAbilitySecond() {
    router.pushUrl({
      url: 'pages/Second',
      params: {
        src: textMessage,
        count: CommonConstants.NUM_VALUES[0]
      }
    });
  }

  /**
   * Navigate to the Index Page of SecondAbility.
   *
   * @param context Context of EntryAbility.
   */
  toSecondAbilityIndex(context: common.UIAbilityContext) {
    let want: Want = {
      'deviceId': '',
      'bundleName': CommonConstants.BUNDLE_NAME,
      'abilityName': CommonConstants.SECOND_ABILITY_NAME,
      'moduleName': CommonConstants.DEVICE_MODULE_NAME,
      'parameters': {
        src: textMessage,
        count: CommonConstants.NUM_VALUES[1]
      }
    };
    context.startAbility(want).then(() => {
      Logger.info(CommonConstants.TAG, `start second ability index page succeed with ${JSON.stringify(want)}`);
    }).catch((error: Error) => {
      Logger.error(CommonConstants.TAG, `start second ability index page failedwith ${error}`);
    });
  }

  /**
   * Navigate to the Second Page of SecondAbility.
   *
   * @param context Context of EntryAbility.
   * @param callback Execute the result callback function.
   */
  toSecondAbilitySecond(context: common.UIAbilityContext, callback: (abilityResult: common.AbilityResult) => void) {
    let want: Want = {
      'deviceId': '',
      'bundleName': CommonConstants.BUNDLE_NAME,
      'abilityName': CommonConstants.SECOND_ABILITY_NAME,
      'moduleName': CommonConstants.DEVICE_MODULE_NAME,
      'parameters': {
        url: 'pages/Second',
        src: textMessage,
        count: CommonConstants.NUM_VALUES[2]
      }
    };

    /**
     * After the pulled side is destroyed,
     * the AbilityResult passed when the pulled side is destroyed is available
     * in the startAbilityForResult callback.
     */
    context.startAbilityForResult(want).then((result) => {
      callback(result);
      Logger.info(CommonConstants.TAG, `start second ability second page succeed with ${JSON.stringify(want)}`);
    }).catch((error: Error) => {
      Logger.error(CommonConstants.TAG, `start second ability second page failed with ${error}`);
    });
  }

  /**
   * Stop SecondAbility itself.
   *
   * @param context Context of SecondAbility.
   */
  terminateSecondAbility(context: common.UIAbilityContext) {
    context.terminateSelf().then(() => {
      Logger.info(CommonConstants.TAG, 'terminate second ability self succeed');
    }).catch((error: Error) => {
      Logger.error(CommonConstants.TAG, `terminate second ability self failed with ${error}`);
    });
  }

  /**
   * Stop SecondAbility itself and return the result.
   *
   * @param context Context of SecondAbility.
   */
  terminateSecondAbilityForResult(context: common.UIAbilityContext) {
    let abilityResult: common.AbilityResult = {
      resultCode: CommonConstants.RESULT_CODE,
      want: {
        'deviceId': '',
        'bundleName': CommonConstants.BUNDLE_NAME,
        'abilityName': CommonConstants.SECOND_ABILITY_NAME,
        'moduleName': CommonConstants.DEVICE_MODULE_NAME,
        'parameters': {
          src: returnMessage,
          count: CommonConstants.RESULT_NUM_VALUE
        }
      }
    };

    // Stop SecondAbility itself and return abilityResult to the startAbilityForResult interface caller
    context.terminateSelfWithResult(abilityResult).then(() => {
      Logger.info(CommonConstants.TAG, `terminate second ability self succeed with ${JSON.stringify(abilityResult)}`);
    }).catch((error: Error) => {
      Logger.error(CommonConstants.TAG, `terminate second ability self failed with ${error}`);
    });
  }
}

export default new ButtonClickMethod();