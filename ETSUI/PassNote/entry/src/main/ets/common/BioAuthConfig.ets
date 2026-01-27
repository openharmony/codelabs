/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

// 生物识别管理器
export class BioAuthManager {
  // 检查设备是否支持指纹识别
  static async checkFingerprintSupport(): Promise<boolean> {
    try {
      userAuth.getAvailableStatus(
        userAuth.UserAuthType.FINGERPRINT,
        userAuth.AuthTrustLevel.ATL3
      );
      return true;
    } catch (error) {
      console.error('设备不支持指纹认证或权限不足', error);
      return false;
    }
  }

  // 执行指纹认证
  static authenticateFingerprint(): Promise<boolean> {
    return new Promise<boolean>((resolve) => {
      try {
        const authParam: userAuth.AuthParam = {
          authType: [userAuth.UserAuthType.FINGERPRINT],
          authTrustLevel: userAuth.AuthTrustLevel.ATL3,
          challenge: new Uint8Array([1, 2, 3, 4])
        };

        const widgetParam: userAuth.WidgetParam = {
          title: '指纹验证',
          navigationButtonText: '取消'
        };

        const authInstance = userAuth.getUserAuthInstance(authParam, widgetParam);

        authInstance.on('result', {
          onResult(result: userAuth.UserAuthResult) {
            authInstance.off('result');
            resolve(result.result === userAuth.ResultCode.SUCCESS);
          }
        });

        authInstance.start();
      } catch (err) {
        console.error('指纹认证失败', err);
        resolve(false);
      }
    });
  }

  // 检查设备是否支持人脸识别
  static async checkFaceSupport(): Promise<boolean> {
    try {
      userAuth.getAvailableStatus(
        userAuth.UserAuthType.FACE,
        userAuth.AuthTrustLevel.ATL3
      );
      return true;
    } catch (error) {
      console.log('设备不支持人脸认证或权限不足', error);
      return false;
    }
  }

  // 执行人脸认证
  static authenticateFace(): Promise<boolean> {
    return new Promise<boolean>((resolve) => {
      try {
        const authParam: userAuth.AuthParam = {
          authType: [userAuth.UserAuthType.FACE],
          authTrustLevel: userAuth.AuthTrustLevel.ATL3,
          challenge: new Uint8Array([1, 2, 3, 4])
        };

        const widgetParam: userAuth.WidgetParam = {
          title: '人脸认证',
          navigationButtonText: '取消'
        };

        const authInstance = userAuth.getUserAuthInstance(
          authParam,
          widgetParam
        );

        authInstance.on('result', {
          onResult(result: userAuth.UserAuthResult) {
            authInstance.off('result');
            resolve(result.result === userAuth.ResultCode.SUCCESS);
          }
        });

        authInstance.start();
      } catch (err) {
        console.error('人脸认证失败', err);
        resolve(false);
      }
    });
  }

  // 获取人脸识别配置
  static getFaceAuthConfig(): AuthConfig {
    return BioAuthConfig.FACE;
  }

  // 人脸位置检测（模拟）
  static detectFacePosition(
    x: number,
    y: number,
    size: number,
    centerX: number = 150,
    centerY: number = 150,
    baseSize: number = 100
  ): FacePositionResult {
    const offsetX = x - centerX;
    const offsetY = y - centerY;
    const offsetSize = size - baseSize;
    const tolerance = BioAuthConfig.FACE.PERFECT_POSITION_TOLERANCE;

    let position = 'perfect';
    let isPerfect = true;

    if (offsetSize > tolerance) {
      position = 'too_close';
      isPerfect = false;
    } else if (offsetSize < -tolerance) {
      position = 'too_far';
      isPerfect = false;
    } else if (offsetX > tolerance) {
      position = 'too_right';
      isPerfect = false;
    } else if (offsetX < -tolerance) {
      position = 'too_left';
      isPerfect = false;
    } else if (offsetY > tolerance) {
      position = 'too_down';
      isPerfect = false;
    } else if (offsetY < -tolerance) {
      position = 'too_up';
      isPerfect = false;
    }

    return {
      position,
      offsetX,
      offsetY,
      offsetSize,
      isPerfect
    };
  }

}

// 定义配置接口
interface AuthConfig {
  MAX_ATTEMPTS: number;
  LOCK_TIME: number;
  SCAN_DURATION: number;
  MIN_QUALITY: number;
  PERFECT_POSITION_TOLERANCE: number;
}

// 生物识别配置
export class BioAuthConfig {
  // 指纹识别配置
  static readonly FINGERPRINT: AuthConfig = {
    MAX_ATTEMPTS: 5,           // 最大尝试次数
    LOCK_TIME: 30,             // 锁定时间（秒）
    SCAN_DURATION: 1000,       // 扫描持续时间（毫秒）
    MIN_QUALITY: 0,
    PERFECT_POSITION_TOLERANCE: 20,
  };

  // 人脸识别配置
  static readonly FACE: AuthConfig = {
    MAX_ATTEMPTS: 3,
    LOCK_TIME: 60,
    SCAN_DURATION: 2000,
    MIN_QUALITY: 60,           // 最低质量要求
    PERFECT_POSITION_TOLERANCE: 20, // 完美位置容差
  };
}

// 定义人脸认证结果的接口
interface FaceAuthResult {
  success: boolean;
  confidence?: number;
  message?: string;
}

// 定义人脸位置检测结果的接口
interface FacePositionResult {
  position: string;
  offsetX: number;
  offsetY: number;
  offsetSize: number;
  isPerfect: boolean;
}

