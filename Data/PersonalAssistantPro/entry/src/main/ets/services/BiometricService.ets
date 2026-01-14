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
import { Logger } from '../common/utils/Logger';

export class BiometricService {
  private static instance: BiometricService;
  private logger = new Logger('BiometricService');

  private constructor() {}

  public static getInstance(): BiometricService {
    if (!BiometricService.instance) {
      BiometricService.instance = new BiometricService();
    }
    return BiometricService.instance;
  }

  /**
   * 检查当前设备是否支持生物认证 (指纹)
   * API 9 逻辑：成功则无返回值，失败则抛出异常
   */
  checkAvailability(): boolean {
    try {
      // 检查指纹认证能力 (AuthType.FINGERPRINT = 4)
      // AuthTrustLevel.ATL1 = 10000

      // ⚠️ 修正点：直接调用，不接收返回值
      userAuth.getAvailableStatus(userAuth.UserAuthType.FINGERPRINT, userAuth.AuthTrustLevel.ATL1);

      // 如果代码能走到这里，说明没有抛出异常，即支持指纹
      this.logger.info('Biometric check passed (Supported).');
      return true;

    } catch (error) {
      // 如果不支持或未录入指纹，会抛出异常
      // 常见错误码: 12501 (General Error), 12502 (Not Supported), 12504 (Not Enrolled)
      this.logger.error(`Biometric check failed: ${JSON.stringify(error)}`);
      return false;
    }
  }

  /**
   * 发起认证
   * @returns Promise<boolean> 认证成功返回 true
   */
  async authenticate(): Promise<boolean> {
    return new Promise((resolve) => {
      try {
        const challenge = new Uint8Array([1, 2, 3, 4, 5, 6, 7, 8]);

        const authParam: userAuth.AuthParam = {
          challenge: challenge,
          authType: [userAuth.UserAuthType.FINGERPRINT],
          authTrustLevel: userAuth.AuthTrustLevel.ATL1,
        };

        const widgetParam: userAuth.WidgetParam = {
          title: '身份验证',
          navigationButtonText: '取消',
        };

        const userAuthInstance = userAuth.getUserAuthInstance(authParam, widgetParam);

        userAuthInstance.on('result', {
          onResult: (result) => {
            this.logger.info(`Auth Result Info: ${JSON.stringify(result)}`);
            // 结果码 12500 代表成功
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