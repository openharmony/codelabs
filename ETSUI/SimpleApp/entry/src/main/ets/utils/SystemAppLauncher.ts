/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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


/**
 * 系统应用启动工具类
 * 用于启动系统应用如通讯录、短信、相机、设置等
 */
import { common } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { promptAction } from '@kit.ArkUI';

export class SystemAppLauncher {
  /**
   * 启动系统通讯录
   */
  static launchContacts(context: common.UIAbilityContext): void {
    const want = {
      bundleName: 'com.ohos.contacts',
      abilityName: 'com.ohos.contacts.MainAbility',
      parameters: {
        // 可以传递联系人信息，如果不需要预填信息，可以不传
      }
    };

    context.startAbility(want)
      .then(() => {
        console.log('通讯录启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('通讯录启动失败:', err.code, err.message);
        this.showErrorToast('通讯录启动失败，请检查应用是否安装');
      });
  }

  /**
   * 启动系统短信应用
   */
  static launchMessages(context: common.UIAbilityContext): void {
    const want = {
      bundleName: 'com.ohos.mms',
      abilityName: 'com.ohos.mms.MainAbility',
      parameters: {
        // 可以预填收件人和内容
        // 'contactObjects': JSON.stringify([new Contact("张三", "13800138000")]),
        // 'content': '预填短信内容'
      }
    };

    context.startAbility(want)
      .then(() => {
        console.log('短信应用启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('短信应用启动失败:', err.code, err.message);
        this.showErrorToast('短信应用启动失败，请检查应用是否安装');
      });
  }

  /**
   * 启动系统相机应用
   */
  static launchCamera(context: common.UIAbilityContext): void {
    const want = {
      bundleName: 'com.ohos.camera',
      abilityName: 'com.ohos.camera.MainAbility',
      parameters: {
        'mode': 'photo', // 拍照模式，可选 'video' 为录像模式
        'resolution': '1920x1080'
      }
    };

    context.startAbility(want)
      .then(() => {
        console.log('相机应用启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('相机应用启动失败:', err.code, err.message);
        this.showErrorToast('相机应用启动失败，请检查应用是否安装');
      });
  }

  /**
   * 启动系统设置应用
   */
  static launchSettings(context: common.UIAbilityContext): void {
    console.log('开始启动设置应用...');
    this.launchSettingsExplicit(context);
  }


  /**
   * 设置应用显式启动备用方案
   */
  private static launchSettingsExplicit(context: common.UIAbilityContext): void {
    // 使用标准HarmonyOS设置包名（正确的包名）
    const possiblePackages = [
      { bundleName: 'com.huawei.hmos.settings', abilityName: 'com.huawei.hmos.settings.MainAbility' }, // HMS设置
      { bundleName: 'com.hmos.settings', abilityName: 'com.hmos.settings.MainAbility' }, // HMS设置
      { bundleName: 'com.ohos.settings', abilityName: 'com.ohos.settings.MainAbility' }, // 标准HarmonyOS设置
      { bundleName: 'com.android.settings', abilityName: 'com.android.settings.MainActivity' }// Android兼容包名
    ];

    const tryLaunch = (index: number) => {
      if (index >= possiblePackages.length) {
        console.error('所有设置包名尝试均失败');
        this.showErrorToast('未找到设置应用，请检查设备是否安装设置');
        return;
      }

      const packageInfo = possiblePackages[index];
      const want = {
        bundleName: packageInfo.bundleName,
        abilityName: packageInfo.abilityName
      };

      console.log(`尝试设置包名: ${packageInfo.bundleName}, Ability: ${packageInfo.abilityName}`);

      context.startAbility(want)
        .then(() => {
          console.log(`设置应用（${packageInfo.bundleName}）启动成功`);
        })
        .catch((err: BusinessError) => {
          console.error(`设置启动失败（${packageInfo.bundleName}）:`, err.code, err.message);
          // 尝试下一个包名
          tryLaunch(index + 1);
        });
    };

    tryLaunch(0);
  }

  /**
   * 启动拨号界面
   */
  static launchDialer(context: common.UIAbilityContext): void {
    const want = {
      bundleName: 'com.ohos.telephony',
      abilityName: 'com.ohos.telephony.MainAbility',
      parameters: {
        'phoneNumber': '' // 可以预填电话号码
      }
    };

    context.startAbility(want)
      .then(() => {
        console.log('拨号界面启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('拨号界面启动失败:', err.code, err.message);
        this.showErrorToast('拨号界面启动失败，请检查应用是否安装');
      });
  }

  /**
   * 启动备忘录应用 - 改进版本
   * 使用更可靠的包名和启动策略
   */
  static launchMemo(context: common.UIAbilityContext): void {
    // 方案1：使用隐式Want启动（最可靠的方式）
    const want = {
      action: 'ohos.want.action.EDIT_DATA',
      entities: ['entity.system.notepad'],
      flags: 0x00000001, // FLAG_ABILITY_NEW_MISSION
      parameters: {
        'ohos.ability.params.showDefaultPicker': true
      }
    };

    context.startAbility(want)
      .then(() => {
        console.log('备忘录应用启动成功（隐式启动）');
      })
      .catch((err: BusinessError) => {
        console.error('备忘录隐式启动失败:', err.code, err.message);
        // 备用方案2：尝试使用标准HarmonyOS备忘录包名
        this.launchMemoExplicit(context);
      });
  }

  /**
   * 备忘录显式启动方案
   */
  private static launchMemoExplicit(context: common.UIAbilityContext): void {
    // 使用标准HarmonyOS备忘录包名
    const possiblePackages = [
      { bundleName: 'com.ohos.note', abilityName: 'MainAbility' }, // 标准HarmonyOS备忘录
      { bundleName: 'com.huawei.hmos.notes', abilityName: 'MainAbility' }, // HMS备忘录
      { bundleName: 'com.huawei.notes', abilityName: 'MainAbility' }, // 华为备忘录
      { bundleName: 'com.android.notes', abilityName: 'MainActivity' }// Android兼容包名
    ];

    const tryLaunch = (index: number) => {
      if (index >= possiblePackages.length) {
        console.error('所有备忘录包名尝试均失败');
        this.showErrorToast('未找到备忘录应用，请检查设备是否安装备忘录');
        return;
      }

      const packageInfo = possiblePackages[index];
      const want = {
        bundleName: packageInfo.bundleName,
        abilityName: packageInfo.abilityName
      };

      context.startAbility(want)
        .then(() => {
          console.log(`备忘录应用（${packageInfo.bundleName}）启动成功`);
        })
        .catch((err: BusinessError) => {
          console.error(`备忘录启动失败（${packageInfo.bundleName}）:`, err.code, err.message);
          // 尝试下一个包名
          tryLaunch(index + 1);
        });
    };

    tryLaunch(0);
  }


  /**
   * 显示成功提示Toast
   */
  private static showSuccessToast(message: string): void {
    try {
      promptAction.showToast({
        message: message,
        duration: 3000,
        bottom: '50%'
      });
    } catch (error) {
      console.error('显示成功Toast失败:', error);
    }
  }

  /**
   * 启动图库应用 - 修复版本
   */
  static launchGallery(context: common.UIAbilityContext): void {
    console.log('开始启动图库应用...');
    this.launchGalleryWithView(context);
  }

  /**
   * 图库VIEW action备用方案
   */
  private static launchGalleryWithView(context: common.UIAbilityContext): void {
    this.launchGalleryExplicit(context);
  }

  /**
   * 图库显式启动备用方案 - 优化版本
   */
  private static launchGalleryExplicit(context: common.UIAbilityContext): void {
    // 使用标准HarmonyOS图库包名（正确的包名）
    const possiblePackages = [
      // 优先尝试系统图库应用
      { bundleName: 'com.huawei.photos', abilityName: 'com.huawei.photos.MainAbility' }, // 华为照片
      { bundleName: 'com.huawei.hmos.gallery', abilityName: 'com.huawei.hmos.gallery.MainAbility' }, // HMS图库
      { bundleName: 'com.ohos.photos', abilityName: 'com.ohos.photos.MainAbility' }, // 标准HarmonyOS图库
      { bundleName: 'com.huawei.hmos.photos', abilityName: 'com.huawei.hmos.photos.MainAbility' }, // HMS照片
      { bundleName: 'com.huawei.gallery', abilityName: 'com.huawei.gallery.MainAbility' }, // 华为图库
      { bundleName: 'com.android.gallery3d', abilityName: 'com.android.gallery3d.MainActivity' }// Android兼容包名
    ];

    const tryLaunch = (index: number) => {
      if (index >= possiblePackages.length) {
        console.error('所有图库包名尝试均失败，尝试系统分享功能');
        // 最终备用方案：使用系统分享功能
        this.launchGalleryViaShare(context);
        return;
      }

      const packageInfo = possiblePackages[index];
      const want = {
        bundleName: packageInfo.bundleName,
        abilityName: packageInfo.abilityName
      };

      console.log(`尝试图库包名: ${packageInfo.bundleName}, Ability: ${packageInfo.abilityName}`);

      context.startAbility(want)
        .then(() => {
          console.log(`图库应用（${packageInfo.bundleName}）启动成功`);
        })
        .catch((err: BusinessError) => {
          console.error(`图库启动失败（${packageInfo.bundleName}）:`, err.code, err.message);
          // 尝试下一个包名
          tryLaunch(index + 1);
        });
    };

    tryLaunch(0);
  }

  /**
   * 图库系统分享备用方案
   */
  private static launchGalleryViaShare(context: common.UIAbilityContext): void {
    // 使用系统分享功能作为最终备用方案
    const want = {
      action: 'ohos.want.action.sendData',
      entities: ['entity.system.image'],
      type: 'image/*',
      parameters: {
        'ohos.ability.params.showDefaultPicker': true
      }
    };

    console.log('图库系统分享Want配置:', JSON.stringify(want));

    context.startAbility(want)
      .then(() => {
        console.log('图库应用启动成功（系统分享）');
      })
      .catch((err: BusinessError) => {
        console.error('图库系统分享启动失败:', err.code, err.message);
        this.showErrorToast('无法打开图库应用，请检查设备是否安装图库应用');
      });
  }

  /**
   * 显示错误提示Toast
   */
  private static showErrorToast(message: string): void {
    try {
      promptAction.showToast({
        message: message,
        duration: 3000,
        bottom: '50%'
      });
    } catch (error) {
      console.error('显示Toast失败:', error);
    }
  }
}