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
import window from '@ohos.window';
import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';
import bundleManager from '@ohos.bundle.bundleManager';
import RemoteDeviceModel from '../model/RemoteDeviceModel';
import logger from '../common/utils/Logger';

export default class EntryAbility extends UIAbility {
  onNewWant(want, launchParams) {
    globalThis.newWant = want;
    logger.info(` onNewWant launchParam: ${JSON.stringify(launchParams) ?? ''}`);
    this.onWindowStageCreate(globalThis.windowStage);
  }

  onCreate(want) {
    globalThis.newWant = want;
    globalThis.context = this.context;
    logger.info('Ability onCreate');
    RemoteDeviceModel.createDeviceManager(this.context);
  }

  onDestroy() {
    logger.info('Ability onDestroy');
    RemoteDeviceModel.unregisterDeviceListCallback();
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    globalThis.windowStage = windowStage;
    // Main window is created, set main page for this ability.
    logger.info('Ability onWindowStageCreate');

    if (globalThis.newWant?.parameters?.url) {
      windowStage.loadContent(globalThis.newWant.parameters.url, (err, data) => {
        if (err) {
          logger.error(`Failed to load the content. Code is ${err.code}, message is ${err.message}`);
          return;
        }
        logger.info(`Succeeded in loading the content. Data: ${JSON.stringify(data) ?? ''}`);
      });
      return;
    }
    windowStage.loadContent('pages/Index', (err, data) => {
      if (err) {
        logger.error(`Failed to load the content. Cause: Code is ${err.code}, message is ${err.message}`);
        return;
      }
      logger.info(`Succeeded in loading the content. Data: ${JSON.stringify(data) ?? ''}`);
    });
    checkPermissions();
  }

  onWindowStageDestroy() {
    // Main window is destroyed, release UI related resources.
    logger.info('Ability onWindowStageDestroy');
  }

  onForeground() {
    // Ability has brought to foreground.
    logger.info('Ability onForeground');
  }

  onBackground() {
    // Ability has back to background.
    logger.info('Ability onBackground');
  }
}

async function checkPermissions(): Promise<void> {
  let grantStatus: abilityAccessCtrl.GrantStatus = await checkAccessToken();

  if (grantStatus !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
    // Applying for the distributed data synchronization permission.
    let atManager = abilityAccessCtrl.createAtManager();
    let permissions: Array<Permissions> = ['ohos.permission.DISTRIBUTED_DATASYNC'];
    try {
      atManager.requestPermissionsFromUser(globalThis.context, permissions);
    } catch (err) {
      logger.error(`Failed to request permissions from user. Code is ${err.code}, message is ${err.message}`);
    }
  }
}

async function checkAccessToken() {
  let atManager = abilityAccessCtrl.createAtManager();
  let grantStatus: abilityAccessCtrl.GrantStatus;
  let tokenId: number;
  try {
    let bundleInfo: bundleManager.BundleInfo = await bundleManager
      .getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
    let appInfo: bundleManager.ApplicationInfo = bundleInfo.appInfo;
    tokenId = appInfo.accessTokenId;
  } catch (err) {
    logger.error(`Failed to get bundle info for self. Code is ${err.code}, message is ${err.message}`);
  }

  try {
    grantStatus = await atManager.checkAccessToken(tokenId, 'ohos.permission.DISTRIBUTED_DATASYNC');
  } catch (err) {
    logger.error(`Failed to check access token. Code is ${err.code}, message is ${err.message}`);
  }
  return grantStatus;
}