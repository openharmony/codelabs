import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import UIAbility from '@ohos.app.ability.UIAbility';
import Want from '@ohos.app.ability.Want';
import window from '@ohos.window';
import bundleManager from '@ohos.bundle.bundleManager';
import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';
import common from '@ohos.app.ability.common';
import Logger from '../common/utils/Logger';
import remoteDeviceModel from '../common/utils/RemoteDeviceUtil';
import Position from '../viewmodel/Position';
const permissions: Array<Permissions> = ['ohos.permission.DISTRIBUTED_DATASYNC'];

export default class EntryAbility extends UIAbility {
  storage: LocalStorage = new LocalStorage();
  want?: Want;

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    this.want = want;
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

  onDestroy(): void {
    Logger.info('EntryAbility', 'onDestroy execute');
    remoteDeviceModel.unregisterDeviceListCallback();
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    Logger.info('EntryAbility', 'onWindowStageCreate start');

    if (this?.want?.parameters?.positionList) {
      let positionList: Position[] = JSON.parse((this.want.parameters.positionList) as string);
      this.storage.setOrCreate('positionList', positionList);
    }

    checkPermissions(this.context);

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
      Logger.error('EntryAbility', `onWindowStageCreate failed. err=${JSON.stringify(error)}`);
    }
  }
}

/**
 * 检查权限状态.
 */
async function checkPermissions(context: common.UIAbilityContext) {
  let grantStatus: abilityAccessCtrl.GrantStatus = await checkAccessToken();
  if (grantStatus !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
    // 申请分布式数据库同步权限
    let atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    atManager.requestPermissionsFromUser(context, permissions).then((data) => {
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
    }).catch((err: Error) => {
      Logger.error('EntryAbility',
        `checkPermissions request permissions failed, err=${JSON.stringify(err)}`);
    })
  }
}

/**
 * 获取授权状态
 *状态标记
 * 返回一个授予状态
 */
async function checkAccessToken() {
  let atManager = abilityAccessCtrl.createAtManager();
  let tokenId: number = 0;
  try {
    let bundleInfo: bundleManager.BundleInfo = await bundleManager
      .getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
    let appInfo: bundleManager.ApplicationInfo = bundleInfo.appInfo;
    tokenId = appInfo.accessTokenId;
  } catch (err) {
    Logger.error('EntryAbility',
      `checkAccessToken Failed to get bundle info for self. err=${JSON.stringify(err)}`);
  }

  let grantStatus: abilityAccessCtrl.GrantStatus = abilityAccessCtrl.GrantStatus.PERMISSION_DENIED;
  try {
    grantStatus = await atManager.checkAccessToken(tokenId, 'ohos.permission.DISTRIBUTED_DATASYNC');
  } catch (err) {
    Logger.error('EntryAbility',
      `checkAccessToken Failed to check access token. err=${JSON.stringify(err)}`);
  }

  return grantStatus;
}
