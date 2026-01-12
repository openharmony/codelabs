export class JsonUtil {
  /**
   * ArkTS strict mode dislikes "any/unknown" and indexed signatures.
   * Keep JSON helpers simple: stringify to string and parse to Object.
   */
  static safeStringify(obj: object | object[] | string | number | boolean | null, space: number = 2): string {
    try {
      return JSON.stringify(obj, null, space);
    } catch (_) {
      return '{}';
    }
  }

  static safeParseObject(text: string): Object | null {
    try {
      const v = JSON.parse(text) as Object;
      return v;
    } catch (_) {
      return null;
    }
  }
}
