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

// 数据管理类 - 管理书签
export class DataManager {
  private static instance: DataManager;
  private context: Context | null = null;
  private bookmarkPreferences: preferences.Preferences | null = null;
  
  // 内存缓存
  private bookmarkList: BookmarkItem[] = [];
  
  private constructor() {}
  
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
      
      // 加载书签
      const bookmarkData = this.bookmarkPreferences.getSync('bookmarkList', '[]') as string;
      this.bookmarkList = JSON.parse(bookmarkData) as BookmarkItem[];
      
      this.isInitialized = true;
      console.info(`DataManager初始化完成: 书签${this.bookmarkList.length}个`);
    } catch (error) {
      console.error('DataManager初始化失败:', error);
    }
  }
  
  // 同步初始化（用于确保数据可用）
  public initSync(context: Context): void {
    if (this.isInitialized) {
      return;
    }
    this.context = context;
    try {
      this.bookmarkPreferences = preferences.getPreferencesSync(context, { name: 'browser_bookmarks' });
      
      // 加载书签
      const bookmarkData = this.bookmarkPreferences.getSync('bookmarkList', '[]') as string;
      this.bookmarkList = JSON.parse(bookmarkData) as BookmarkItem[];
      
      this.isInitialized = true;
      console.info(`DataManager同步初始化完成: 书签${this.bookmarkList.length}个`);
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
}

// 导出单例实例
export const dataManager = DataManager.getInstance();
