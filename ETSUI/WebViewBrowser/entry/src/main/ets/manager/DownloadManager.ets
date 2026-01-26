/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

import { request } from '@kit.BasicServicesKit';
import { preferences } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { fileIo as fs } from '@kit.CoreFileKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

import { FileUtil } from '../utils/FileUtil';

const DOMAIN = 0x0001;
const TAG = 'DownloadManager';

// 下载状态枚举
export enum DownloadState {
  IDLE = 0,
  QUEUED = 1,
  DOWNLOADING = 2,
  PAUSED = 3,
  COMPLETED = 4,
  FAILED = 5,
  CANCELED = 6
}

// 下载项接口定义
export interface DownloadItem {
  id: string;              // 唯一标识 (通常是时间戳+随机数)
  url: string;             // 下载地址
  fileName: string;        // 保存的文件名
  filePath: string;        // 保存的完整路径
  targetUri?: string;      // 用户选择的保存目标URI
  isSavedToTarget?: boolean; // 是否已成功保存到目标位置
  state: DownloadState;    // 当前状态
  progress: number;        // 进度 0-100
  receivedSize: number;    // 已接收字节
  totalSize: number;       // 总字节
  speed: number;           // 下载速度 (bytes/s)
  createTime: number;      // 创建时间
  finishTime?: number;     // 完成时间
  mimeType?: string;       // MIME类型
  errorMessage?: string;   // 错误信息
}

// 监听器接口
export interface DownloadListener {
  onProgress: (item: DownloadItem) => void;
  onStatusChange: (item: DownloadItem) => void;
  onError: (item: DownloadItem, error: string) => void;
}

/**
 * 下载管理器 - 单例模式
 * 负责管理所有下载任务，持久化下载记录，并提供UI回调
 */
export class DownloadManager {
  private static instance: DownloadManager;
  private context: common.UIAbilityContext | null = null;
  private preferences: preferences.Preferences | null = null;
  private downloadList: DownloadItem[] = [];
  private listeners: Set<DownloadListener> = new Set();

  // 内存中维护的request任务映射
  private taskMap: Map<string, request.DownloadTask> = new Map();

  private constructor() {}

  public static getInstance(): DownloadManager {
    if (!DownloadManager.instance) {
      DownloadManager.instance = new DownloadManager();
    }
    return DownloadManager.instance;
  }

  /**
   * 初始化管理器
   * @param context 应用上下文
   */
  public async init(context: common.UIAbilityContext): Promise<void> {
    if (this.context) {
      return;
    }
    this.context = context;
    hilog.info(DOMAIN, TAG, 'DownloadManager initializing...');

    try {
      this.preferences = await preferences.getPreferences(context, 'browser_downloads');
      await this.loadDownloadHistory();
      hilog.info(DOMAIN, TAG, 'DownloadManager initialized successfully');
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Failed to init DownloadManager: %{public}s', JSON.stringify(err));
    }
  }

  /**
   * 开始新的下载任务
   * @param url 下载链接
   * @param suggestedName 建议的文件名
   * @param mimeType 文件类型
   * @param contentLength 文件大小
   */
  public async startDownload(url: string, suggestedName?: string, mimeType?: string, contentLength?: number, targetUri?: string): Promise<boolean> {
    if (!this.context) {
      hilog.error(DOMAIN, TAG, 'Context is null, cannot start download');
      return false;
    }

    try {
      // 1. 确定文件名
      // 优先使用建议名称，否则从URL解析
      let rawFileName = suggestedName || this.getFileNameFromUrl(url);

      // 如果文件名没有后缀，或后缀是服务器脚本（如php），且有MimeType，尝试修正后缀
      if (mimeType) {
        rawFileName = this.ensureExtension(rawFileName, mimeType);
      }

      const id = this.generateId();
      const fileName = this.sanitizeFileName(rawFileName);

      // 使用应用沙箱路径 cache 目录作为下载目录
      const downloadDir = this.context.cacheDir;
      const filePath = `${downloadDir}/${fileName}`;

      hilog.info(DOMAIN, TAG, `Starting download: ${fileName}, MimeType: ${mimeType}`);

      // 2. 创建下载项记录
      const newItem: DownloadItem = {
        id: id,
        url: url,
        fileName: fileName,
        filePath: filePath,
        targetUri: targetUri,
        state: DownloadState.QUEUED,
        progress: 0,
        receivedSize: 0,
        totalSize: contentLength || 0,
        speed: 0,
        createTime: Date.now(),
        mimeType: mimeType
      };

      this.downloadList.unshift(newItem);
      this.notifyStatusChange(newItem);
      await this.saveDownloadHistory();

      // 3. 启动系统下载任务
      try {
        const config: request.DownloadConfig = {
          url: url,
          header: {},
          enableMetered: true,
          enableRoaming: true,
          filePath: filePath,
          title: fileName
        };

        request.downloadFile(this.context, config).then((task: request.DownloadTask) => {
          newItem.state = DownloadState.DOWNLOADING;
          this.taskMap.set(newItem.id, task);

          this.attachTaskListeners(task, newItem);
          this.notifyStatusChange(newItem);
          hilog.info(DOMAIN, TAG, 'Download task started: %{public}s', fileName);
        }).catch((err: BusinessError) => {
          this.handleTaskError(newItem, `Start failed: ${err.message}`);
        });

      } catch (reqErr) {
        this.handleTaskError(newItem, `Request creation failed: ${(reqErr as BusinessError).message}`);
        return false;
      }

      return true;
    } catch (error) {
      hilog.error(DOMAIN, TAG, 'Unexpected error in startDownload: %{public}s', JSON.stringify(error));
      return false;
    }
  }

  /**
   * 绑定任务监听器
   */
  private attachTaskListeners(task: request.DownloadTask, item: DownloadItem) {
    try {
      // 进度监听
      task.on('progress', (receivedSize: number, totalSize: number) => {
        // 计算速度 (简单模拟)
        const diff = receivedSize - item.receivedSize;
        if (diff > 0) {
          item.speed = diff;
        }

        item.receivedSize = receivedSize;
        if (totalSize > 0) {
          item.totalSize = totalSize;
          item.progress = Math.floor((receivedSize / totalSize) * 100);
        }
        item.state = DownloadState.DOWNLOADING;
        this.notifyProgress(item);
      });

      // 完成监听
      task.on('complete', async () => {
        hilog.info(DOMAIN, TAG, 'Task complete: %{public}s', item.fileName);
        item.state = DownloadState.COMPLETED;
        item.progress = 100;
        item.finishTime = Date.now();
        item.receivedSize = item.totalSize;

        if (item.targetUri && this.context) {
          try {
            const success = await FileUtil.copyToUri(this.context, item.filePath, item.targetUri);
            item.isSavedToTarget = success;
            if (success) {
              hilog.info(DOMAIN, TAG, 'Auto saved to user target: %{public}s', item.targetUri);
            }
          } catch (copyErr) {
            hilog.error(DOMAIN, TAG, 'Failed to save to target: %{public}s', JSON.stringify(copyErr));
          }
        }

        // 移除任务引用
        this.taskMap.delete(item.id);
        this.saveDownloadHistory();
        this.notifyStatusChange(item);
      });

      // 失败监听
      task.on('fail', (err: number) => {
        hilog.error(DOMAIN, TAG, 'Task failed: %{public}s, code: %{public}d', item.fileName, err);
        item.state = DownloadState.FAILED;
        item.errorMessage = `Error code: ${err}`;
        this.taskMap.delete(item.id);
        this.saveDownloadHistory();
        this.notifyStatusChange(item);
        this.notifyError(item, `Download failed with code ${err}`);
      });

    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Error attaching listeners: %{public}s', JSON.stringify(err));
    }
  }

  /**
   * 打开下载文件
   * @param item 下载项
   */
  public async openDownloadItem(item: DownloadItem): Promise<boolean> {
    if (!this.context || item.state !== DownloadState.COMPLETED) {
      return false;
    }
    // 优先打开沙箱内的缓存文件
    return await FileUtil.openFile(this.context, item.filePath, item.mimeType);
  }

  /**
   * 暂停/恢复/取消操作
   */
  public async operateTask(id: string, action: 'pause' | 'resume' | 'cancel'): Promise<void> {
    const item = this.downloadList.find(d => d.id === id);
    if (!item) return;

    const task = this.taskMap.get(id);

    if (action === 'cancel') {
      if (task) {
        try {
          await task.delete(); // 删除任务
        } catch (e) {}
        this.taskMap.delete(id);
      }
      // 如果文件存在，尝试删除
      try {
        if (fs.accessSync(item.filePath)) {
          fs.unlinkSync(item.filePath);
        }
      } catch (e) {
        hilog.warn(DOMAIN, TAG, 'Failed to delete file: %{public}s', item.filePath);
      }

      // 从列表中移除
      const index = this.downloadList.findIndex(d => d.id === id);
      if (index !== -1) {
        this.downloadList.splice(index, 1);
      }
      await this.saveDownloadHistory();
      this.notifyStatusChange(item);
      return;
    }

    if (!task) {
      hilog.warn(DOMAIN, TAG, 'Task not found in memory map');
      return;
    }

    try {
      if (action === 'pause') {
        await task.suspend();
        item.state = DownloadState.PAUSED;
      } else if (action === 'resume') {
        await task.restore();
        item.state = DownloadState.DOWNLOADING;
      }
      this.notifyStatusChange(item);
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Operate task failed: %{public}s', JSON.stringify(err));
      this.notifyError(item, `Operation ${action} failed`);
    }
  }

  /**
   * 清除所有历史记录 (保留文件)
   */
  public async clearHistory(deleteFiles: boolean = false): Promise<void> {
    if (deleteFiles) {
      // 遍历删除文件
      for (const item of this.downloadList) {
        try {
          if (fs.accessSync(item.filePath)) {
            fs.unlinkSync(item.filePath);
          }
        } catch (e) {}
      }
    }
    this.downloadList = [];
    this.taskMap.clear(); // 清理引用
    await this.saveDownloadHistory();
  }


  // ================= 辅助方法 =================

  private handleTaskError(item: DownloadItem, msg: string) {
    item.state = DownloadState.FAILED;
    item.errorMessage = msg;
    this.notifyStatusChange(item);
    this.notifyError(item, msg);
    this.saveDownloadHistory();
  }

  private generateId(): string {
    return `${Date.now()}_${Math.floor(Math.random() * 1000)}`;
  }

  private getFileNameFromUrl(url: string): string {
    try {
      const parts = url.split('/');
      let name = parts[parts.length - 1];
      if (name.includes('?')) {
        name = name.split('?')[0];
      }
      return name || `download_${Date.now()}`;
    } catch (e) {
      return `download_${Date.now()}`;
    }
  }

  // 根据MimeType确保文件名有正确的后缀
  private ensureExtension(fileName: string, mimeType: string): string {
    const suggestedExt = this.getExtensionFromMimeType(mimeType);

    // 如果没有映射到具体的后缀，直接返回原文件名
    if (!suggestedExt) {
      return fileName;
    }

    const dotIndex = fileName.lastIndexOf('.');

    // 情况1: 文件名没有后缀 -> 补全后缀
    if (dotIndex === -1) {
      return `${fileName}.${suggestedExt}`;
    }

    // 情况2: 文件名有后缀，但后缀是服务器脚本（如 .php, .asp） -> 强制替换后缀
    const currentExt = fileName.substring(dotIndex + 1).toLowerCase();
    const scriptExtensions = ['php', 'asp', 'aspx', 'jsp', 'cgi', 'do', 'action'];

    if (scriptExtensions.includes(currentExt)) {
      // 保留前面的文件名，替换后缀。
      return `${fileName.substring(0, dotIndex)}.${suggestedExt}`;
    }

    return fileName;
  }

  // MimeType 到 文件后缀的映射
  private getExtensionFromMimeType(mimeType: string): string {
    if (!mimeType) return '';

    const lowerMime = mimeType.toLowerCase();

    // 图片
    if (lowerMime.includes('jpeg') || lowerMime.includes('jpg')) return 'jpg';
    if (lowerMime.includes('png')) return 'png';
    if (lowerMime.includes('gif')) return 'gif';
    if (lowerMime.includes('webp')) return 'webp';
    if (lowerMime.includes('svg')) return 'svg';
    if (lowerMime.includes('bmp')) return 'bmp';

    // 文档
    if (lowerMime.includes('pdf')) return 'pdf';
    if (lowerMime.includes('text/plain')) return 'txt';
    if (lowerMime.includes('text/html')) return 'html';

    // 压缩包
    if (lowerMime.includes('zip')) return 'zip';
    if (lowerMime.includes('rar')) return 'rar';
    if (lowerMime.includes('7z')) return '7z';

    // 应用
    if (lowerMime.includes('android.package-archive')) return 'hap';

    // 音视频
    if (lowerMime.includes('mp4')) return 'mp4';
    if (lowerMime.includes('mp3')) return 'mp3';

    return '';
  }

  // 清理文件名中的非法字符
  private sanitizeFileName(name: string): string {
    return name.replace(/[\\/:*?"<>|]/g, '_');
  }

  // ================= 持久化 =================

  private async loadDownloadHistory() {
    if (!this.preferences) return;
    try {
      const jsonStr = await this.preferences.get('download_list', '[]') as string;
      const list = JSON.parse(jsonStr) as DownloadItem[];
      this.downloadList = list.map(item => {
        if (item.state === DownloadState.DOWNLOADING || item.state === DownloadState.QUEUED) {
          item.state = DownloadState.PAUSED;
        }
        return item;
      });
      hilog.info(DOMAIN, TAG, 'Loaded %{public}d download items', this.downloadList.length);
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Failed to load history: %{public}s', JSON.stringify(err));
    }
  }

  private async saveDownloadHistory() {
    if (!this.preferences) return;
    try {
      await this.preferences.put('download_list', JSON.stringify(this.downloadList));
      await this.preferences.flush();
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Failed to save history: %{public}s', JSON.stringify(err));
    }
  }

  // ================= 监听器管理 =================

  public getDownloadList(): DownloadItem[] {
    return [...this.downloadList];
  }

  public addListener(listener: DownloadListener) {
    this.listeners.add(listener);
  }

  public removeListener(listener: DownloadListener) {
    this.listeners.delete(listener);
  }

  private notifyProgress(item: DownloadItem) {
    this.listeners.forEach(l => l.onProgress(item));
  }

  private notifyStatusChange(item: DownloadItem) {
    this.listeners.forEach(l => l.onStatusChange(item));
  }

  private notifyError(item: DownloadItem, msg: string) {
    this.listeners.forEach(l => l.onError(item, msg));
  }

  // 格式化文件大小工具方法
  public static formatFileSize(bytes: number): string {
    if (bytes === 0) return '0 B';
    const k = 1024;
    const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
  }
}

// 导出全局单例
export const downloadManager = DownloadManager.getInstance();