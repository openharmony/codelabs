// common/Config.ets
import router from '@ohos.router';
import preferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';

export enum PassType {
  NUMBER = 'number',
  GRAPH = 'graph',
  FINGERPRINT = 'fingerprint',
  FACE = 'face'
}

export interface UserData {
  username: string;
  avatarUrl?: string;
  lastLoginTime: number;
  lastLoginMethod: PassType;
  enabledAuthMethods: PassType[];
  settings: UserSettings;
}

export interface UserSettings {
  autoLogin: boolean;
  rememberMe: boolean;
  biometricsEnabled: boolean;
  securityLevel: 'low' | 'medium' | 'high';
}

export class UserManager {
  private static readonly PREF_NAME: string = 'user_prefs';
  private static pref: preferences.Preferences | null = null;

  private static async getPref(context: common.Context): Promise<preferences.Preferences> {
    if (!UserManager.pref) {
      UserManager.pref = await preferences.getPreferences(context, UserManager.PREF_NAME);
    }
    return UserManager.pref;
  }

  static async getCurrentUser(context: common.Context): Promise<UserData | null> {
    try {
      const p: preferences.Preferences = await UserManager.getPref(context);
      const dataStr: string = await p.get('user_data', '') as string;
      if (dataStr === '') {
        const defaultUser: UserData = {
          username: '鸿小蒙',
          lastLoginTime: Date.now(),
          lastLoginMethod: PassType.NUMBER,
          enabledAuthMethods: [PassType.NUMBER, PassType.GRAPH, PassType.FINGERPRINT, PassType.FACE],
          settings: { autoLogin: false, rememberMe: true, biometricsEnabled: true, securityLevel: 'medium' }
        };
        return defaultUser;
      }
      return JSON.parse(dataStr) as UserData;
    } catch (error) {
      return null;
    }
  }
}

export class PasswordManager {
  private static readonly PREF_NAME: string = 'auth_s torage';
  private static prefInstance: preferences.Preferences | null = null;
  private static fingerprintEnabled: boolean = true

  static isFingerprintEnabled(): boolean {
    return PasswordManager.fingerprintEnabled
  }

  static setFingerprintEnabled(enabled: boolean): void {
    PasswordManager.fingerprintEnabled = enabled
  }

  private static async getPref(context: common.Context): Promise<preferences.Preferences> {
    if (!PasswordManager.prefInstance) {
      PasswordManager.prefInstance = await preferences.getPreferences(context, PasswordManager.PREF_NAME);
    }
    return PasswordManager.prefInstance;
  }

  // 使用函数重载兼容旧代码调用
  static async verifyPassword(type: PassType, input: string): Promise<boolean>;
  static async verifyPassword(context: common.Context, type: PassType, input: string): Promise<boolean>;
  static async verifyPassword(arg1: common.Context | PassType, arg2: PassType | string, arg3?: string): Promise<boolean> {
    if (arg3 === undefined) {
      // 兼容旧调用：无 Context 模式
      const mock: Map<string, string> = new Map([
        [PassType.NUMBER.toString(), '123456'],
        [PassType.GRAPH.toString(), '1478']
      ]);
      return mock.get(arg1.toString()) === (arg2 as string);
    }
    // 持久化验证模式
    const p: preferences.Preferences = await PasswordManager.getPref(arg1 as common.Context);
    const defaultVal: string = (arg2 === PassType.NUMBER) ? '123456' : '1478';
    const stored: string = await p.get(arg2.toString(), defaultVal) as string;
    return stored === arg3;
  }

  static async updatePassword(context: common.Context, type: PassType, oldPass: string, newPass: string): Promise<boolean> {
    const isValid: boolean = await PasswordManager.verifyPassword(context, type, oldPass);
    if (!isValid) return false;
    const p: preferences.Preferences = await PasswordManager.getPref(context);
    await p.put(type.toString(), newPass);
    await p.flush();
    return true;
  }

  // 重载实现 setLastUsedAuth
  static async setLastUsedAuth(type: PassType): Promise<void>;
  static async setLastUsedAuth(context: common.Context, type: PassType): Promise<void>;
  static async setLastUsedAuth(arg1: common.Context | PassType, arg2?: PassType): Promise<void> {
    if (arg2 === undefined || typeof arg1 === 'string') {
      return; // 旧代码调用不执行持久化
    }
    try {
      const p: preferences.Preferences = await PasswordManager.getPref(arg1 as common.Context);
      await p.put('last_auth', arg2.toString());
      await p.flush();
    } catch (err) {
      const error: BusinessError = err as BusinessError;
      console.error(`Persistent Save Error: ${error.code}`);
    }
  }

  static async resetPassword(
    context: common.Context,
    type: PassType,
    newPassword: string
  ): Promise<boolean> {
    try {
      const p: preferences.Preferences = await PasswordManager.getPref(context)
      await p.put(type.toString(), newPassword)
      await p.flush()
      return true
    } catch (err) {
      const error: BusinessError = err as BusinessError
      console.error(`Reset Password Error: ${error.code}`)
      return false
    }
  }
}

// 路由工具显式定义返回类型
export class RouterUtil {
  static goToHome(): void {
    router.pushUrl({ url: 'pages/HomePage' });
  }
  static goBack(): void {
    router.back();
  }
}

// 定义 BusinessError 接口以符合 ArkTS 错误处理规范
interface BusinessError {
  code: string;
  message: string;
}