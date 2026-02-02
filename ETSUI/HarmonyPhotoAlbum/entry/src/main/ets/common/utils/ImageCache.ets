/**
 * ImageCache - 简单的内存图片缓存工具
 * 目前只做内存级缓存（适合本地 file:// 与资源图的重复展示场景）
 */
/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
import Constants from '../constants/Constants';

export type ImageSourceType = string | Resource;

interface CacheEntry {
  src: ImageSourceType;
  time: number; // 缓存写入时间，用于简单淘汰策略
}

class ImageCache {
  // 简单的 Map + 上限控制，实现类似 LRU/FIFO 的效果
  private cache: Map<string, CacheEntry> = new Map();

  // 最大缓存条目数，避免内存无限增长
  private maxSize: number = Constants.IMAGE_CACHE_MAX_SIZE;

  /**
   * 设置最大缓存数量（可被外部调整）
   */
  setMaxSize(size: number) {
    if (size > 0) {
      this.maxSize = size;
      this.shrinkIfNeeded();
    }
  }

  /**
   * 生成缓存 key，目前直接使用字符串形式
   */
  private getKey(src: ImageSourceType): string {
    return typeof src === 'string' ? src : JSON.stringify(src);
  }

  /**
   * 从缓存中获取
   */
  get(src: ImageSourceType): ImageSourceType | null {
    const key = this.getKey(src);
    const entry = this.cache.get(key);
    if (!entry) {
      return null;
    }
    // 更新访问时间，模拟 LRU
    entry.time = Date.now();
    this.cache.set(key, entry);
    return entry.src;
  }

  /**
   * 写入缓存
   */
  set(src: ImageSourceType): void {
    const key = this.getKey(src);
    const now = Date.now();
    this.cache.set(key, { src, time: now });
    this.shrinkIfNeeded();
  }

  has(src: ImageSourceType): boolean {
    const key = this.getKey(src);
    return this.cache.has(key);
  }

  delete(src: ImageSourceType): void {
    const key = this.getKey(src);
    this.cache.delete(key);
  }

  clear(): void {
    this.cache.clear();
  }

  /**
   * 当缓存超出上限时，按时间从旧到新淘汰
   */
  private shrinkIfNeeded() {
    if (this.cache.size <= this.maxSize) {
      return;
    }
    // 将 Map 转为数组，按 time 排序，删除最旧的若干条
    const entries: Array<[string, CacheEntry]> = Array.from(this.cache.entries());
    entries.sort((a: [string, CacheEntry], b: [string, CacheEntry]) => a[1].time - b[1].time);
    const removeCount: number = this.cache.size - this.maxSize;
    for (let i = 0; i < removeCount; i++) {
      const entry: [string, CacheEntry] = entries[i];
      const key: string = entry[0];
      this.cache.delete(key);
    }
  }
}

// 导出单例，方便直接使用
const imageCache = new ImageCache();
export default imageCache;
