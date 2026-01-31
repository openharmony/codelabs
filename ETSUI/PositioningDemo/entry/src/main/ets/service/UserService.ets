// SPDX-License-Identifier: MIT
import { UserInfo, UserSettings } from '../model/UserModel';
import preferences from '@ohos.data.preferences';
import { common } from '@kit.AbilityKit';

export class UserService {
  private static readonly USER_INFO_KEY = 'user_info';
  private static readonly USER_SETTINGS_KEY = 'user_settings';
  private store?: preferences.Preferences;

  // 初始化用户服务
  async init(context: common.UIAbilityContext): Promise<void> {
    try {
      this.store = await preferences.getPreferences(context, 'user_data');
      console.log('用户服务初始化成功');
    } catch (error) {
      console.error('初始化用户服务失败:', error);
    }
  }

  // 获取用户信息
  async getUserInfo(): Promise<UserInfo> {
    if (!this.store) {
      console.warn('用户服务未初始化，返回默认用户信息');
      return new UserInfo();
    }

    try {
      const data = await this.store.get(UserService.USER_INFO_KEY, '');
      if (data && typeof data === 'string' && data.length > 0) {
        const userInfo = JSON.parse(data) as UserInfo;
        console.log('获取用户信息成功:', userInfo);
        return userInfo;
      }
    } catch (error) {
      console.error('获取用户信息失败:', error);
    }

    // 返回默认用户信息
    return new UserInfo();
  }

  // 保存用户信息
  async saveUserInfo(userInfo: UserInfo): Promise<boolean> {
    if (!this.store) {
      console.error('用户服务未初始化，无法保存用户信息');
      return false;
    }

    try {
      await this.store.put(UserService.USER_INFO_KEY, JSON.stringify(userInfo));
      await this.store.flush();
      console.log('保存用户信息成功:', userInfo);
      return true;
    } catch (error) {
      console.error('保存用户信息失败:', error);
      return false;
    }
  }

  // 获取用户设置
  async getUserSettings(): Promise<UserSettings> {
    if (!this.store) {
      console.warn('用户服务未初始化，返回默认设置');
      return new UserSettings();
    }

    try {
      const data = await this.store.get(UserService.USER_SETTINGS_KEY, '');
      if (data && typeof data === 'string' && data.length > 0) {
        const settings = JSON.parse(data) as UserSettings;
        console.log('获取用户设置成功:', settings);
        return settings;
      }
    } catch (error) {
      console.error('获取用户设置失败:', error);
    }

    return new UserSettings();
  }

  // 保存用户设置
  async saveUserSettings(settings: UserSettings): Promise<boolean> {
    if (!this.store) {
      console.error('用户服务未初始化，无法保存设置');
      return false;
    }

    try {
      await this.store.put(UserService.USER_SETTINGS_KEY, JSON.stringify(settings));
      await this.store.flush();
      console.log('保存用户设置成功:', settings);
      return true;
    } catch (error) {
      console.error('保存用户设置失败:', error);
      return false;
    }
  }
}
