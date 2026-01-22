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
      });
  }
  
  /**
   * 启动系统设置应用
   */
  static launchSettings(context: common.UIAbilityContext): void {
    const want = {
      bundleName: 'com.huawei.hmos.settings',
      abilityName: 'com.huawei.hmos.settings.MainAbility',
      uri: 'main_settings' // 主设置页面
    };
    
    context.startAbility(want)
      .then(() => {
        console.log('设置应用启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('设置应用启动失败:', err.code, err.message);
      });
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
      });
  }
  
  /**
   * 启动音乐应用
   */
  static launchMusic(context: common.UIAbilityContext): void {
    const want = {
      bundleName: 'com.ohos.music',
      abilityName: 'com.ohos.music.MainAbility'
    };
    
    context.startAbility(want)
      .then(() => {
        console.log('音乐应用启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('音乐应用启动失败:', err.code, err.message);
      });
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
      });
  }
  
  /**
   * 启动图库应用
   */
  static launchGallery(context: common.UIAbilityContext): void {
    const want = {
      bundleName: 'com.huawei.hmos.photos',
      abilityName: 'com.huawei.hmos.photos.MainAbility',
      flags: 0x10000000 // FLAG_ABILITY_NEW_MISSION
    };
    
    context.startAbility(want)
      .then(() => {
        console.log('图库应用启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('图库应用启动失败:', err.code, err.message);
        // 备用方案：尝试使用隐式启动
        this.launchGalleryFallback(context);
      });
  }
  
  /**
   * 图库启动备用方案 - 隐式启动
   */
  private static launchGalleryFallback(context: common.UIAbilityContext): void {
    const want = {
      action: 'ohos.want.action.viewData',
      uri: 'file://',
      type: 'image/*'
    };
    
    context.startAbility(want)
      .then(() => {
        console.log('图库应用（隐式启动）启动成功');
      })
      .catch((err: BusinessError) => {
        console.error('图库应用启动完全失败:', err.code, err.message);
      });
  }
}