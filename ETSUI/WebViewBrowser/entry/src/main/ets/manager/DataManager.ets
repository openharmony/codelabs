/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import { preferences } from '@kit.ArkData';

// 书签项接口
export interface BookmarkItem {
  id: number;
  url: string;
  title: string;
  createTime: number;
  favicon?: string;
  folder?: string;
}

// 历史记录项接口
export interface HistoryItem {
  id: number;
  url: string;
  title: string;
  visitTime: number;
  visitCount: number;
  favicon?: string;
}

// 数据管理类 - 管理书签和历史记录
export class DataManager {
  private static instance: DataManager;
  private context: Context | null = null;
  private bookmarkPreferences: preferences.Preferences | null = null;
  private historyPreferences: preferences.Preferences | null = null;
  // 内存缓存
  private bookmarkList: BookmarkItem[] = [];
  private historyList: HistoryItem[] = [];

  private constructor() {
  }

  // 单例模式
  public static getInstance(): DataManager {
    if (!DataManager.instance) {
      DataManager.instance = new DataManager();
    }
    return DataManager.instance;
  }

  // 是否已初始化
  private isInitialized: boolean = false;

  // 初始化
  public async init(context: Context): Promise<void> {
    if (this.isInitialized) {
      return;
    }
    this.context = context;
    try {
      this.bookmarkPreferences = await preferences.getPreferences(context, 'browser_bookmarks');
      this.historyPreferences = await preferences.getPreferences(context, 'browser_history');

      // 加载书签
      const bookmarkData = this.bookmarkPreferences.getSync('bookmarkList', '[]') as string;
      this.bookmarkList = JSON.parse(bookmarkData) as BookmarkItem[];

      // 加载历史记录
      const historyData = this.historyPreferences.getSync('historyList', '[]') as string;
      this.historyList = JSON.parse(historyData) as HistoryItem[];

      this.isInitialized = true;
      console.info(`DataManager初始化完成: 书签${this.bookmarkList.length}个, 历史${this.historyList.length}条`);
    } catch (error) {
      console.error('DataManager初始化失败:', error);
    }
  }

  // 同步初始化（用于确保数据可用）
  public initSync(context: Context): void {
    console.info('=== DataManager initSync 开始 ===');
    if (this.isInitialized) {
      console.info('DataManager 已初始化，跳过');
      return;
    }
    this.context = context;
    try {
      console.info('获取 bookmarkPreferences...');
      this.bookmarkPreferences = preferences.getPreferencesSync(context, { name: 'browser_bookmarks' });
      console.info('bookmarkPreferences 获取成功');

      console.info('获取 historyPreferences...');
      this.historyPreferences = preferences.getPreferencesSync(context, { name: 'browser_history' });
      console.info('historyPreferences 获取成功');
      console.info(`historyPreferences 是否为null: ${this.historyPreferences === null}`);

      // 加载书签
      const bookmarkData = this.bookmarkPreferences.getSync('bookmarkList', '[]') as string;
      this.bookmarkList = JSON.parse(bookmarkData) as BookmarkItem[];
      console.info(`加载书签: ${this.bookmarkList.length}个`);

      // 加载历史记录
      const historyData = this.historyPreferences.getSync('historyList', '[]') as string;
      console.info(`历史记录原始数据长度: ${historyData.length}`);
      this.historyList = JSON.parse(historyData) as HistoryItem[];
      console.info(`加载历史记录: ${this.historyList.length}条`);

      this.isInitialized = true;
      console.info(`DataManager同步初始化完成: 书签${this.bookmarkList.length}个, 历史${this.historyList.length}条`);
    } catch (error) {
      console.error('DataManager同步初始化失败:', error);
    }
  }


  // ==================== 书签相关方法 ====================

  // 添加书签
  public async addBookmark(url: string, title: string, folder?: string): Promise<boolean> {
    if (!url || url === 'about:blank') {
      return false;
    }

    // 检查是否已存在
    if (this.isBookmarked(url)) {
      return false;
    }

    const newBookmark: BookmarkItem = {
      id: Date.now(),
      url: url,
      title: title || url,
      createTime: Date.now(),
      folder: folder
    };

    this.bookmarkList.unshift(newBookmark);
    await this.saveBookmarks();
    return true;
  }

  // 删除书签
  public async deleteBookmark(id: number): Promise<void> {
    const index = this.bookmarkList.findIndex(item => item.id === id);
    if (index >= 0) {
      this.bookmarkList.splice(index, 1);
      await this.saveBookmarks();
    }
  }

  // 根据URL删除书签
  public async deleteBookmarkByUrl(url: string): Promise<void> {
    const index = this.bookmarkList.findIndex(item => item.url === url);
    if (index >= 0) {
      this.bookmarkList.splice(index, 1);
      await this.saveBookmarks();
    }
  }

  // 更新书签
  public async updateBookmark(id: number, title: string, folder?: string): Promise<void> {
    const bookmark = this.bookmarkList.find(item => item.id === id);
    if (bookmark) {
      bookmark.title = title;
      bookmark.folder = folder;
      await this.saveBookmarks();
    }
  }

  // 获取所有书签
  public getBookmarkList(): BookmarkItem[] {
    return [...this.bookmarkList];
  }

  // 检查URL是否已收藏
  public isBookmarked(url: string): boolean {
    return this.bookmarkList.some(item => item.url === url);
  }

  // 根据URL获取书签
  public getBookmarkByUrl(url: string): BookmarkItem | undefined {
    return this.bookmarkList.find(item => item.url === url);
  }

  // 搜索书签
  public searchBookmarks(keyword: string): BookmarkItem[] {
    const lowerKeyword = keyword.toLowerCase();
    return this.bookmarkList.filter(item =>
    item.url.toLowerCase().includes(lowerKeyword) ||
    item.title.toLowerCase().includes(lowerKeyword)
    );
  }

  // 清空所有书签
  public async clearAllBookmarks(): Promise<void> {
    this.bookmarkList = [];
    await this.saveBookmarks();
  }

  // 保存书签到持久化存储
  private async saveBookmarks(): Promise<void> {
    if (this.bookmarkPreferences) {
      await this.bookmarkPreferences.put('bookmarkList', JSON.stringify(this.bookmarkList));
      await this.bookmarkPreferences.flush();
    }
  }

  // ==================== 历史记录相关方法 ====================

  // 添加历史记录
  public async addHistory(url: string, title: string): Promise<void> {
    console.info('=== addHistory 被调用 ===');
    console.info(`URL: ${url}`);
    console.info(`Title: ${title}`);
    console.info(`当前历史记录数量: ${this.historyList.length}`);

    if (!url || url === 'about:blank') {
      console.info('URL为空或about:blank，跳过记录');
      return;
    }

    // 查找是否已存在该URL的历史记录
    const existingIndex = this.historyList.findIndex(item => item.url === url);
    console.info(`已存在索引: ${existingIndex}`);

    if (existingIndex >= 0) {
      // 如果已存在，更新访问时间和访问次数
      console.info('更新已存在的历史记录');
      const existingItem = this.historyList[existingIndex];
      existingItem.visitTime = Date.now();
      existingItem.visitCount++;
      existingItem.title = title || existingItem.title;

      // 移到列表开头
      this.historyList.splice(existingIndex, 1);
      this.historyList.unshift(existingItem);
      console.info(`访问次数: ${existingItem.visitCount}`);
    } else {
      // 不存在则新建
      console.info('创建新的历史记录');
      const newHistory: HistoryItem = {
        id: Date.now(),
        url: url,
        title: title || url,
        visitTime: Date.now(),
        visitCount: 1
      };

      this.historyList.unshift(newHistory);
      console.info(`新历史记录ID: ${newHistory.id}`);

      // 限制历史记录数量（最多保存1000条）
      if (this.historyList.length > 1000) {
        this.historyList = this.historyList.slice(0, 1000);
      }
    }

    console.info(`保存前历史记录数量: ${this.historyList.length}`);
    await this.saveHistory();
    console.info('addHistory 完成');
  }

  // 删除历史记录
  public async deleteHistory(id: number): Promise<void> {
    const index = this.historyList.findIndex(item => item.id === id);
    if (index >= 0) {
      this.historyList.splice(index, 1);
      await this.saveHistory();
    }
  }

  // 根据URL删除历史记录
  public async deleteHistoryByUrl(url: string): Promise<void> {
    const index = this.historyList.findIndex(item => item.url === url);
    if (index >= 0) {
      this.historyList.splice(index, 1);
      await this.saveHistory();
    }
  }

  // 批量删除历史记录
  public async deleteHistoryBatch(ids: number[]): Promise<void> {
    this.historyList = this.historyList.filter(item => !ids.includes(item.id));
    await this.saveHistory();
  }

  // 获取所有历史记录
  public getHistoryList(): HistoryItem[] {
    console.info(`=== getHistoryList 被调用，返回 ${this.historyList.length} 条记录 ===`);
    return [...this.historyList];
  }

  // 按日期分组获取历史记录
  public getHistoryByDate(): Map<string, HistoryItem[]> {
    const grouped = new Map<string, HistoryItem[]>();
    const now = new Date();
    const today = new Date(now.getFullYear(), now.getMonth(), now.getDate()).getTime();
    const yesterday = today - 24 * 60 * 60 * 1000;
    const weekAgo = today - 7 * 24 * 60 * 60 * 1000;

    this.historyList.forEach(item => {
      const itemTime = new Date(item.visitTime);
      const itemDate = new Date(itemTime.getFullYear(), itemTime.getMonth(), itemTime.getDate()).getTime();

      let dateLabel: string;
      if (itemDate === today) {
        dateLabel = '今天';
      } else if (itemDate === yesterday) {
        dateLabel = '昨天';
      } else if (itemDate >= weekAgo) {
        dateLabel = '最近7天';
      } else {
        dateLabel = '更早';
      }

      if (!grouped.has(dateLabel)) {
        grouped.set(dateLabel, []);
      }
      grouped.get(dateLabel)?.push(item);
    });

    return grouped;
  }

  // 搜索历史记录
  public searchHistory(keyword: string): HistoryItem[] {
    const lowerKeyword = keyword.toLowerCase();
    return this.historyList.filter(item =>
    item.url.toLowerCase().includes(lowerKeyword) ||
    item.title.toLowerCase().includes(lowerKeyword)
    );
  }

  // 清空所有历史记录
  public async clearAllHistory(): Promise<void> {
    this.historyList = [];
    await this.saveHistory();
  }

  // 清空指定日期范围的历史记录
  public async clearHistoryByDateRange(startTime: number, endTime: number): Promise<void> {
    this.historyList = this.historyList.filter(item =>
    item.visitTime < startTime || item.visitTime > endTime
    );
    await this.saveHistory();
  }

  // 保存历史记录到持久化存储
  private async saveHistory(): Promise<void> {
    console.info('=== saveHistory 被调用 ===');
    console.info(`historyPreferences 是否存在: ${this.historyPreferences !== null}`);

    if (this.historyPreferences) {
      const historyJson = JSON.stringify(this.historyList);
      console.info(`准备保存的历史记录数量: ${this.historyList.length}`);
      console.info(`JSON长度: ${historyJson.length}`);

      try {
        await this.historyPreferences.put('historyList', historyJson);
        console.info('historyList 已写入 preferences');

        await this.historyPreferences.flush();
        console.info('preferences 已flush到磁盘');

        // 验证保存
        const savedData = this.historyPreferences.getSync('historyList', '[]') as string;
        const savedList = JSON.parse(savedData) as HistoryItem[];
        console.info(`验证：实际保存的历史记录数量: ${savedList.length}`);
      } catch (error) {
        console.error('保存历史记录失败:', error);
      }
    } else {
      console.error('historyPreferences 为 null，无法保存历史记录');
    }
  }
}

// 导出单例实例
export const dataManager = DataManager.getInstance();
