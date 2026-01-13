import type { Context } from '@kit.AbilityKit';

export class AppContextHolder {
  public static context: Context | null = null;
}
