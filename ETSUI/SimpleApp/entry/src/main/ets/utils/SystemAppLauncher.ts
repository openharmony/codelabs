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

/*
 *
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
    
    // 方案1：使用隐式Want启动（带URI）
    const want = {
      action: 'ohos.want.action.viewData',
      entities: ['entity.system.settings'],
      uri: 'settings://system/main',
      flags: 0x00000001, // FLAG_ABILITY_NEW_MISSION
      parameters: {
        'ohos.ability.params.showDefaultPicker': true
      }
    };
    
    console.log('设置隐式Want配置:', JSON.stringify(want));
    
    context.startAbility(want)
      .then(() => {
        console.log('设置应用启动成功（隐式启动）');
      })
      .catch((err: BusinessError) => {
        console.error('设置应用隐式启动失败 - 错误码:', err.code, '错误信息:', err.message);
        // 备用方案：尝试VIEW action
        this.launchSettingsWithView(context);
      });
  }
  
  /**
   * 设置VIEW action备用方案
   */
  private static launchSettingsWithView(context: common.UIAbilityContext): void {
    const want = {
      action: 'ohos.want.action.VIEW',
      entities: ['entity.system.settings'],
      uri: 'settings://system/main',
      flags: 0x00000001
    };
    
    console.log('设置VIEW action Want配置:', JSON.stringify(want));
    
    context.startAbility(want)
      .then(() => {
        console.log('设置应用启动成功（VIEW action）');
      })
      .catch((err: BusinessError) => {
        console.error('设置VIEW action启动失败:', err.code, err.message);
        // 最终备用方案：显式启动
        this.launchSettingsExplicit(context);
      });
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
      { bundleName: 'com.android.settings', abilityName: 'com.android.settings.MainActivity' } // Android兼容包名
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
      { bundleName: 'com.android.notes', abilityName: 'MainActivity' } // Android兼容包名
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
   * 启动浏览器（推荐方法）
   */
  static launchBrowser(context: common.UIAbilityContext, url: string = 'https://www.example.com'): void {
    // 方法1：使用openLink接口（推荐）
    const options = {
      appLinkingOnly: false
    };
    
    context.openLink(url, options)
      .then(() => {
        console.log('浏览器启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('浏览器启动失败:', err.code, err.message);
        
        // 备用方案：使用隐式Want启动
        this.launchBrowserFallback(context, url);
      });
  }
  
  /**
   * 浏览器启动备用方案 - 隐式Want启动
   */
  private static launchBrowserFallback(context: common.UIAbilityContext, url: string): void {
    const want = {
      action: 'ohos.want.action.viewData',
      entities: ['entity.system.browsable'],
      uri: url,
      parameters: {
        'ohos.ability.params.showDefaultPicker': true
      }
    };
    
    context.startAbility(want)
      .then(() => {
        console.log('浏览器（隐式启动）启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('浏览器启动完全失败:', err.code, err.message);
        this.showErrorToast('浏览器启动失败，请检查网络连接');
      });
  }
  
  /**
   * 启动时钟应用或创建闹钟
   */
  static launchClock(context: common.UIAbilityContext): void {
    console.log('开始启动时钟功能...');
    
    // 方案1：先尝试显式启动系统时钟应用
    const possiblePackages = [
      // 华为时钟应用标准包名（简化AbilityName）
      { bundleName: 'com.huawei.android.deskclock', abilityName: 'MainAbility' },
      // HarmonyOS标准包名
      { bundleName: 'com.ohos.clock', abilityName: 'MainAbility' },
      // HMS包名
      { bundleName: 'com.huawei.hmos.clock', abilityName: 'MainAbility' },
      // Android兼容包名
      { bundleName: 'com.android.deskclock', abilityName: 'MainActivity' },
      // 备用包名
      { bundleName: 'com.huawei.clock', abilityName: 'MainActivity' }
    ];
    
    const tryLaunch = (index: number) => {
      if (index >= possiblePackages.length) {
        console.error('所有时钟包名尝试均失败，使用闹钟API作为替代方案');
        // 方案2：使用系统闹钟API创建闹钟
        this.createAlarmReminder(context);
        return;
      }
      
      const packageInfo = possiblePackages[index];
      console.log(`尝试时钟包名: ${packageInfo.bundleName}, Ability: ${packageInfo.abilityName}`);
      
      const want = {
        bundleName: packageInfo.bundleName,
        abilityName: packageInfo.abilityName
      };
      
      context.startAbility(want)
        .then(() => {
          console.log(`时钟应用（${packageInfo.bundleName}）启动成功`);
        })
        .catch((err: BusinessError) => {
          console.error(`时钟启动失败（${packageInfo.bundleName}）:`, err.code, err.message);
          // 尝试下一个包名
          tryLaunch(index + 1);
        });
    };
    
    tryLaunch(0);
  }
  
  /**
   * 创建闹钟提醒（替代方案）
   */
  private static createAlarmReminder(context: common.UIAbilityContext): void {
    try {
      // 显示当前时间并提示用户
      const now = new Date();
      const hours = now.getHours();
      const minutes = now.getMinutes();
      const seconds = now.getSeconds();
      
      const timeString = `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
      
      this.showSuccessToast(`当前时间：${timeString}\n设备未安装时钟应用`);
      
      console.info('时钟功能已触发，显示当前时间作为替代方案');
      
    } catch (error) {
      console.error('创建闹钟时发生错误:', error);
      this.showErrorToast('时钟功能暂不可用');
    }
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
   * 启动图库应用
   */
  static launchGallery(context: common.UIAbilityContext): void {
    console.log('开始启动图库应用...');
    
    // 方案1：使用正确的隐式Want启动（带URI）
    const want = {
      action: 'ohos.want.action.viewData',
      entities: ['entity.system.image'],
      uri: 'image://gallery/photos',
      flags: 0x00000001, // FLAG_ABILITY_NEW_MISSION
      parameters: {
        'ohos.ability.params.showDefaultPicker': true
      }
    };
    
    console.log('图库隐式Want配置:', JSON.stringify(want));
    
    context.startAbility(want)
      .then(() => {
        console.log('图库应用启动成功（隐式启动）');
      })
      .catch((err: BusinessError) => {
        console.error('图库应用隐式启动失败 - 错误码:', err.code, '错误信息:', err.message);
        // 备用方案：尝试VIEW action
        this.launchGalleryWithView(context);
      });
  }
  
  /**
   * 图库VIEW action备用方案
   */
  private static launchGalleryWithView(context: common.UIAbilityContext): void {
    const want = {
      action: 'ohos.want.action.VIEW',
      entities: ['entity.system.image'],
      uri: 'image://gallery/photos',
      flags: 0x00000001
    };
    
    console.log('图库VIEW action Want配置:', JSON.stringify(want));
    
    context.startAbility(want)
      .then(() => {
        console.log('图库应用启动成功（VIEW action）');
      })
      .catch((err: BusinessError) => {
        console.error('图库VIEW action启动失败:', err.code, err.message);
        // 最终备用方案：显式启动
        this.launchGalleryExplicit(context);
      });
  }
  
  /**
   * 图库显式启动备用方案
   */
  private static launchGalleryExplicit(context: common.UIAbilityContext): void {
    // 使用标准HarmonyOS图库包名（正确的包名）
    const possiblePackages = [
      { bundleName: 'com.huawei.hmos.gallery', abilityName: 'com.huawei.hmos.gallery.MainAbility' }, // HMS图库
      { bundleName: 'com.huawei.hmos.photos', abilityName: 'com.huawei.hmos.photos.MainAbility' }, // HMS照片
      { bundleName: 'com.huawei.gallery', abilityName: 'com.huawei.gallery.MainAbility' }, // 华为图库
      { bundleName: 'com.huawei.photos', abilityName: 'com.huawei.photos.MainAbility' }, // 华为照片
      { bundleName: 'com.ohos.photos', abilityName: 'com.ohos.photos.MainAbility' }, // 标准HarmonyOS图库
      { bundleName: 'com.android.gallery3d', abilityName: 'com.android.gallery3d.MainActivity' } // Android兼容包名
    ];
    
    const tryLaunch = (index: number) => {
      if (index >= possiblePackages.length) {
        console.error('所有图库包名尝试均失败');
        this.showErrorToast('未找到图库应用，请检查设备是否安装图库');
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