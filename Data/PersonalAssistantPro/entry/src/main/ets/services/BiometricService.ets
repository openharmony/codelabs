/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import userAuth from '@ohos.userIAM.userAuth';
import common from '@ohos.app.ability.common'; // ✅ 引入 common 以使用 Context 类型
import { Logger } from '../common/utils/Logger';

export class BiometricService {
  private static instance: BiometricService;
  private logger = new Logger('BiometricService');

  private constructor() {
  }

  public static getInstance(): BiometricService {
    if (!BiometricService.instance) {
      BiometricService.instance = new BiometricService();
    }
    return BiometricService.instance;
  }

  /**
   * 检查当前设备是否支持生物认证
   */
  checkAvailability(): boolean {
    try {
      userAuth.getAvailableStatus(userAuth.UserAuthType.FINGERPRINT, userAuth.AuthTrustLevel.ATL1);
      this.logger.info('Biometric check passed (Supported).');
      return true;
    } catch (error) {
      this.logger.error(`Biometric check failed: ${JSON.stringify(error)}`);
      return false;
    }
  }

  /**
   * 发起认证
   * @param context UIAbilityContext，用于解析资源字符串
   * @returns Promise<boolean> 认证成功返回 true
   */
  async authenticate(context: common.Context): Promise<boolean> { // ✅ 新增 context 参数
    return new Promise((resolve) => {
      try {
        const challenge = new Uint8Array([1, 2, 3, 4, 5, 6, 7, 8]);

        const authParam: userAuth.AuthParam = {
          challenge: challenge,
          authType: [userAuth.UserAuthType.FINGERPRINT],
          authTrustLevel: userAuth.AuthTrustLevel.ATL1,
        };

        // ✅ 核心修改：通过 context.resourceManager 将 $r 转为 string
        const titleStr = context.resourceManager.getStringSync($r('app.string.auth_title'));
        const cancelStr = context.resourceManager.getStringSync($r('app.string.common_cancel'));

        const widgetParam: userAuth.WidgetParam = {
          title: titleStr,
          navigationButtonText: cancelStr,
        };

        const userAuthInstance = userAuth.getUserAuthInstance(authParam, widgetParam);

        userAuthInstance.on('result', {
          onResult: (result) => {
            this.logger.info(`Auth Result Info: ${JSON.stringify(result)}`);
            if (result.result === userAuth.UserAuthResultCode.SUCCESS) {
              this.logger.info('Biometric auth success');
              resolve(true);
            } else {
              this.logger.warn(`Biometric auth failed/canceled. Code: ${result.result}`);
              resolve(false);
            }
            userAuthInstance.off('result');
          }
        });

        this.logger.info('Starting user authentication...');
        userAuthInstance.start();

      } catch (err) {
        this.logger.error('Authentication process exception', err);
        resolve(false);
      }
    });
  }
}