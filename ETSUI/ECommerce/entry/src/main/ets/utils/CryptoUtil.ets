import { cryptoFramework } from '@kit.CryptoArchitectureKit';
import { util } from '@kit.ArkTS';

// 加密工具类：提供AES加解密
export class CryptoUtil {
  private static readonly AES_KEY_ALIAS = 'fresh_market_aes_key';
  private static readonly DEMO_KEY_HEX = '000102030405060708090a0b0c0d0e0f';

  // AES加密方法
  static async encryptAES(plainText: string): Promise<string> {
    try {
      let symKeyGenerator = cryptoFramework.createSymKeyGenerator('AES128');
      let keyBlob: cryptoFramework.DataBlob = { data: new Uint8Array(CryptoUtil.hexToBytes(CryptoUtil.DEMO_KEY_HEX)) };
      let symKey = await symKeyGenerator.convertKey(keyBlob);

      let cipher = cryptoFramework.createCipher('AES128|ECB|PKCS7');
      await cipher.init(cryptoFramework.CryptoMode.ENCRYPT_MODE, symKey, null);
      
      let input: cryptoFramework.DataBlob = { data: new util.TextEncoder().encodeInto(plainText) };
      let result = await cipher.doFinal(input);
      
      return CryptoUtil.bytesToHex(result.data);
    } catch (err) {
      console.error('Encrypt failed:', err);
      return plainText;
    }
  }

  // AES解密方法
  static async decryptAES(cipherHex: string): Promise<string> {
    try {
      let symKeyGenerator = cryptoFramework.createSymKeyGenerator('AES128');
      let keyBlob: cryptoFramework.DataBlob = { data: new Uint8Array(CryptoUtil.hexToBytes(CryptoUtil.DEMO_KEY_HEX)) };
      let symKey = await symKeyGenerator.convertKey(keyBlob);

      let cipher = cryptoFramework.createCipher('AES128|ECB|PKCS7');
      await cipher.init(cryptoFramework.CryptoMode.DECRYPT_MODE, symKey, null);

      let input: cryptoFramework.DataBlob = { data: new Uint8Array(CryptoUtil.hexToBytes(cipherHex)) };
      let result = await cipher.doFinal(input);

      return new util.TextDecoder().decodeWithStream(result.data);
    } catch (err) {
      console.error('Decrypt failed:', err);
      return '';
    }
  }

  private static hexToBytes(hex: string): number[] {
    let bytes: number[] = [];
    for (let c = 0; c < hex.length; c += 2) {
      bytes.push(parseInt(hex.substr(c, 2), 16));
    }
    return bytes;
  }

  private static bytesToHex(bytes: Uint8Array): string {
    let hex: string[] = [];
    for (let i = 0; i < bytes.length; i++) {
      let current = bytes[i] < 0 ? bytes[i] + 256 : bytes[i];
      hex.push((current >>> 4).toString(16));
      hex.push((current & 0xF).toString(16));
    }
    return hex.join("");
  }
}