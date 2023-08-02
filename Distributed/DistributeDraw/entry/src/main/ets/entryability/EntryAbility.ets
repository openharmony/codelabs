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

import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import UIAbility from '@ohos.app.ability.UIAbility';
import Want from '@ohos.app.ability.Want';
import window from '@ohos.window';
import bundleManager from '@ohos.bundle.bundleManager';
import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';
import Logger from '../common/utils/Logger';
import remoteDeviceModel from '../common/utils/RemoteDeviceUtil';
import Position from '../common/bean/Position';

const permissions: Array<Permissions> = ['ohos.permission.DISTRIBUTED_DATASYNC'];

export default class EntryAbility extends UIAbility {
  storage: LocalStorage = new LocalStorage();
  want: Want;

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    this.want = want;
    globalThis.context = this.context;
    Logger.info('EntryAbility',
      `onCreate want=${JSON.stringify(this.want)}, launchParam=${JSON.stringify(launchParam)}`);
    remoteDeviceModel.createDeviceManager();
  }

  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam) {
    this.want = want;
    Logger.info('EntryAbility',
      `onNewWant want=${JSON.stringify(this.want)}, launchParam=${JSON.stringify(launchParam)}`);

    if (this.want?.parameters?.positionList) {
      let positionList: Position[] = JSON.parse((this.want.parameters.positionList) as string);
      this.storage.setOrCreate('positionList', positionList);
      this.storage.setOrCreate('updateCanvas', true);
    }
  }

  onDestroy() {
    Logger.info('EntryAbility', 'onDestroy execute');
    remoteDeviceModel.unregisterDeviceListCallback();
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    Logger.info('EntryAbility', 'onWindowStageCreate start');

    if (this.want?.parameters?.positionList) {
      let positionList: Position[] = JSON.parse((this.want.parameters.positionList) as string);
      this.storage.setOrCreate('positionList', positionList);
    }

    checkPermissions();

    try {
      windowStage.loadContent('pages/Index', this.storage, (err) => {
        if (err) {
          Logger.error('EntryAbility',
            `onWindowStageCreate Failed to load the content. code=${err.code}, msg=${JSON.stringify(err.message)}`);
          return;
        }
        Logger.info('EntryAbility', 'onWindowStageCreate Succeeded in loading the content');
      });
    } catch (error) {
      Logger.error('EntryAbility',
        `onWindowStageCreate failed. code=${error.code}, msg=${JSON.stringify(error.message)}`);
    }
  }
}

/**
 * Check the permission status.
 */
async function checkPermissions() {
  let grantStatus: abilityAccessCtrl.GrantStatus = await checkAccessToken();
  if (grantStatus !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
    // Applying for the distributed data synchronization permission.
    let atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    atManager.requestPermissionsFromUser(globalThis.context, permissions).then((data) => {
      let grantStatus: Array<number> = data.authResults;
      let length: number = grantStatus.length;
      for (let i: number = 0; i < length; i++) {
        if (grantStatus[i] === 0) {
          Logger.info('EntryAbility', `checkPermissions request permission ${permissions[i]} success`);
        } else {
          Logger.error('EntryAbility',
            `checkPermissions request fail permission=${permissions[i]}, grantStatus=${grantStatus[i]}`);
          return;
        }
      }
    }).catch((err) => {
      Logger.error('EntryAbility',
        `checkPermissions request permissions failed, code=${err.code}, msg=${JSON.stringify(err.message)}`);
    })
  }
}

/**
 * Obtains the authorization status.
 *
 * @returns Grant status.
 */
async function checkAccessToken() {
  let atManager = abilityAccessCtrl.createAtManager();
  let tokenId: number;
  try {
    let bundleInfo: bundleManager.BundleInfo = await bundleManager
      .getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
    let appInfo: bundleManager.ApplicationInfo = bundleInfo.appInfo;
    tokenId = appInfo.accessTokenId;
  } catch (err) {
    Logger.error('EntryAbility',
      `checkAccessToken Failed to get bundle info for self. code is ${err.code}, message is ${err.message}`);
  }

  let grantStatus: abilityAccessCtrl.GrantStatus = abilityAccessCtrl.GrantStatus.PERMISSION_DENIED;
  try {
    grantStatus = await atManager.checkAccessToken(tokenId, 'ohos.permission.DISTRIBUTED_DATASYNC');
  } catch (err) {
    Logger.error('EntryAbility',
      `checkAccessToken Failed to check access token. code is ${err.code}, message is ${err.message}`);
  }

  return grantStatus;
}