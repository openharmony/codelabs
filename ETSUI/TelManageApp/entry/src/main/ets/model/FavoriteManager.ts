// model/FavoriteManager.ts
import { contact } from '@kit.ContactsKit';
import { Context, common } from '@kit.AbilityKit';
import { fileIo } from '@kit.CoreFileKit';

export class FavoriteManager {
  private context: Context;
  private favoriteFileName = 'favorite_contacts.json';

  constructor(context: Context) {
    this.context = context;
  }

  // 获取文件路径
  private getFavoriteFilePath(): string {
    const uiContext = this.context as common.UIAbilityContext;
    return uiContext.filesDir + '/' + this.favoriteFileName;
  }

  // 字符串转Uint8Array
  private stringToUint8Array(str: string): Uint8Array {
    const arr = new Uint8Array(str.length);
    for (let i = 0; i < str.length; i++) {
      arr[i] = str.charCodeAt(i);
    }
    return arr;
  }

  // Uint8Array转字符串
  private uint8ArrayToString(uint8Array: Uint8Array): string {
    let str = '';
    for (let i = 0; i < uint8Array.length; i++) {
      str += String.fromCharCode(uint8Array[i]);
    }
    return str;
  }

  // 获取收藏联系人ID列表
  async getFavoriteContactIds(): Promise<string[]> {
    try {
      const filePath = this.getFavoriteFilePath();

      // 检查文件是否存在
      try {
        await fileIo.access(filePath);
      } catch {
        // 文件不存在，返回空数组
        return [];
      }

      // 打开文件
      const file = await fileIo.open(filePath, fileIo.OpenMode.READ_ONLY);

      try {
        // 获取文件大小
        const stat = await fileIo.stat(filePath);
        if (stat.size === 0) {
          return [];
        }

        // 读取文件内容
        const buffer = new ArrayBuffer(stat.size);
        const readLen = await fileIo.read(file.fd, buffer);

        if (readLen <= 0) {
          return [];
        }

        // 转换为Uint8Array
        const uint8Array = new Uint8Array(buffer, 0, readLen);
        const content = this.uint8ArrayToString(uint8Array);

        // 解析JSON
        if (content.trim()) {
          return JSON.parse(content);
        }
        return [];
      } finally {
        // 确保文件被关闭
        await fileIo.close(file.fd);
      }
    } catch (error) {
      console.error('读取收藏文件失败:', error);
      return [];
    }
  }

  // 保存收藏联系人ID列表
  async saveFavoriteContactIds(contactIds: string[]): Promise<void> {
    try {
      const filePath = this.getFavoriteFilePath();
      const content = JSON.stringify(contactIds);

      // 打开文件（创建如果不存在）
      const file = await fileIo.open(filePath, fileIo.OpenMode.READ_WRITE | fileIo.OpenMode.CREATE);

      try {
        // 清空文件
        await fileIo.truncate(file.fd, 0);

        // 将字符串转换为Uint8Array
        const uint8Array = this.stringToUint8Array(content);

        // 写入文件
        await fileIo.write(file.fd, uint8Array.buffer);

        console.log('收藏联系人已保存到:', filePath);
      } finally {
        // 确保文件被关闭
        await fileIo.close(file.fd);
      }
    } catch (error) {
      console.error('保存收藏文件失败:', error);
      throw error;
    }
  }

  // 添加单个收藏联系人
  async addFavoriteContact(contactId: string): Promise<void> {
    if (!contactId) return;

    const favoriteIds = await this.getFavoriteContactIds();
    if (!favoriteIds.includes(contactId)) {
      favoriteIds.push(contactId);
      await this.saveFavoriteContactIds(favoriteIds);
    }
  }

  // 移除收藏联系人
  async removeFavoriteContact(contactId: string): Promise<void> {
    if (!contactId) return;

    let favoriteIds = await this.getFavoriteContactIds();
    favoriteIds = favoriteIds.filter(id => id !== contactId);
    await this.saveFavoriteContactIds(favoriteIds);
  }

  // 检查是否已收藏
  async isContactFavorite(contactId: string): Promise<boolean> {
    if (!contactId) return false;

    const favoriteIds = await this.getFavoriteContactIds();
    return favoriteIds.includes(contactId);
  }

  // 获取收藏的联系人详情
  async getFavoriteContacts(allContacts: contact.Contact[]): Promise<contact.Contact[]> {
    const favoriteIds = await this.getFavoriteContactIds();
    return allContacts.filter(contactItem => {
      const contactId = contactItem.key || '';
      return contactId && favoriteIds.includes(contactId);
    });
  }

  // 批量设置收藏状态
  async setFavoriteStatus(contactIds: string[], isFavorite: boolean): Promise<void> {
    if (!contactIds || contactIds.length === 0) return;

    let favoriteIds = await this.getFavoriteContactIds();

    if (isFavorite) {
      // 添加收藏
      contactIds.forEach(id => {
        if (id && !favoriteIds.includes(id)) {
          favoriteIds.push(id);
        }
      });
    } else {
      // 移除收藏
      favoriteIds = favoriteIds.filter(id => !contactIds.includes(id));
    }

    await this.saveFavoriteContactIds(favoriteIds);
  }

  // 清除所有收藏
  async clearAllFavorites(): Promise<void> {
    await this.saveFavoriteContactIds([]);
  }

  // 获取收藏数量
  async getFavoriteCount(): Promise<number> {
    const favoriteIds = await this.getFavoriteContactIds();
    return favoriteIds.length;
  }
}