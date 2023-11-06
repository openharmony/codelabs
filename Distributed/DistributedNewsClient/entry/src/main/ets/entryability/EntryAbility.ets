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
import Want from '@ohos.app.ability.Want';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import RemoteDeviceModel from '../model/RemoteDeviceModel';
import logger from '../common/utils/Logger';
import common from '@ohos.app.ability.common';

const permissions: Array<Permissions> = ['ohos.permission.DISTRIBUTED_DATASYNC'];

export default class EntryAbility extends UIAbility {
  private want: Want | undefined = undefined;
  private windowStage?: window.WindowStage;

  onNewWant(want: Want, launchParams: AbilityConstant.LaunchParam) {
    this.want = want;
    AppStorage.setOrCreate<string>('wantNewsId', (want.parameters as Record<string, string>)['newsId']);
    logger.info(`onNewWant launchParam: ${JSON.stringify(launchParams) ?? ''}`);
    if (this.windowStage === undefined) {
      return;
    }
    this.onWindowStageCreate(this.windowStage);
  }

  onCreate(want: Want) {
    this.want = want;
    AppStorage.setOrCreate<string>('wantNewsId', (want.parameters as Record<string, string>)['newsId']);
    logger.info('Ability onCreate');
    RemoteDeviceModel.createDeviceManager(this.context);
  }

  onDestroy() {
    logger.info('Ability onDestroy');
    RemoteDeviceModel.unregisterDeviceListCallback();
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    // Main window is created, set main page for this ability.
    logger.info('Ability onWindowStageCreate');
    this.windowStage = windowStage;

    if (this.want === undefined) {
      return;
    }
    if (this.want.parameters?.newsId) {
      windowStage.loadContent('pages/NewsDetail', (err: Error) => {
        if (err) {
          logger.error(`Failed to load the content. Cause: + ${JSON.stringify(err)}`);
          return;
        }
        console.info('Succeeded in loading the content.');
      });
      return;
    }
    windowStage.loadContent('pages/Index', (err: Error) => {
      if (err) {
        logger.error(`Failed to load the content. Cause: ${JSON.stringify(err)}`);
        return;
      }
    });
    checkPermissions(this.context);
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

async function checkPermissions(context: common.UIAbilityContext): Promise<void> {
  let grantStatus: abilityAccessCtrl.GrantStatus = await checkAccessToken();

  if (grantStatus !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
    // Applying for the distributed data synchronization permission.
    let atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    atManager.requestPermissionsFromUser(context, permissions).then((data) => {
      let grantStatus: Array<number> = data.authResults;
      let length: number = grantStatus.length;
      for (let i: number = 0; i < length; i++) {
        if (grantStatus[i] === 0) {
          logger.info('EntryAbility', `checkPermissions request permission ${permissions[i]} success`);
        } else {
          logger.error('EntryAbility',
            `checkPermissions request fail permission=${permissions[i]}, grantStatus=${grantStatus[i]}`);
          return;
        }
      }
    }).catch((err: Error) => {
      logger.error('EntryAbility',
        `checkPermissions request permissions failed, err=${JSON.stringify(err)}`);
    })
  }
}

async function checkAccessToken() {
  let atManager = abilityAccessCtrl.createAtManager();
  let tokenId: number = 0;
  try {
    let bundleInfo: bundleManager.BundleInfo = await bundleManager
      .getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
    let appInfo: bundleManager.ApplicationInfo = bundleInfo.appInfo;
    tokenId = appInfo.accessTokenId;
  } catch (err) {
    logger.error('EntryAbility',
      `checkAccessToken Failed to get bundle info for self. err=${JSON.stringify(err)}`);
  }

  let grantStatus: abilityAccessCtrl.GrantStatus = abilityAccessCtrl.GrantStatus.PERMISSION_DENIED;
  try {
    grantStatus = await atManager.checkAccessToken(tokenId, 'ohos.permission.DISTRIBUTED_DATASYNC');
  } catch (err) {
    logger.error('EntryAbility',
      `checkAccessToken Failed to check access token. err=${JSON.stringify(err)}`);
  }

  return grantStatus;
}