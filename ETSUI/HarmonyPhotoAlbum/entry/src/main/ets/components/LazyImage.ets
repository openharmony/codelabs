/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
import imageCache, { ImageSourceType } from '../common/utils/ImageCache';
import Logger from '../common/utils/Logger';

/**
 * LazyImage - 懒加载 + 简单缓存 + 占位/错误兜底
 *
 * 使用方式：
 *   LazyImage({ src: 'file:///xxx.jpg', width: '100%', height: 100 })
 */

@Component
export struct LazyImage {
  // 对外属性
  src: ImageSourceType = '';
  widthValue: Length | LayoutPolicy = '100%';
  heightValue: Length | LayoutPolicy = '100%';
  objectFit: ImageFit = ImageFit.Cover;
  cornerRadius: number | Resource | Length = 0;
  placeholder?: Resource; // 加载中占位图
  errorHolder?: Resource; // 加载失败占位图
  enableCache: boolean = true;
  // 基类要求的函数实现
  width(value: Length): LazyImage {
    this.widthValue = value;
    return this;
  }

  height(value: Length): LazyImage {
    this.heightValue = value;
    return this;
  }

  // 内部状态
  @State private realSrc: ImageSourceType | null = null;
  @State private isLoading: boolean = false;
  @State private hasError: boolean = false;
  @State private isAppeared: boolean = false;

  aboutToAppear() {
    this.isAppeared = true;
    this.tryLoad();
  }

  aboutToDisappear() {
    this.isAppeared = false;
  }

  private tryLoad() {
    if (!this.isAppeared || this.isLoading || this.realSrc) {
      return;
    }
    this.isLoading = true;
    this.hasError = false;

    // 简化：本地 file:// 或资源图，本质上不需要异步解码，这里只做缓存封装
    try {
      if (this.enableCache && imageCache.has(this.src)) {
        const cached = imageCache.get(this.src);
        if (cached) {
          this.realSrc = cached;
          this.isLoading = false;
          return;
        }
      }

      // 未命中缓存，直接使用原 src，并写入缓存
      this.realSrc = this.src;
      if (this.enableCache) {
        imageCache.set(this.src);
      }
      this.isLoading = false;
    } catch (err) {
      Logger.error('[LazyImage] load error', JSON.stringify(err));
      this.hasError = true;
      this.isLoading = false;
    }
  }

  build() {
    Column() {
      // 加载失败 -> 错误占位
      if (this.hasError && this.errorHolder) {
        Image(this.errorHolder)
          .width(this.widthValue)
          .height(this.heightValue)
          .objectFit(this.objectFit)
          .borderRadius(this.cornerRadius);
      } else if (!this.realSrc) {
        // 加载中或尚未开始 -> 占位
        if (this.placeholder) {
        Image(this.placeholder)
          .width(this.widthValue)
          .height(this.heightValue)
          .objectFit(ImageFit.Contain)
          .borderRadius(this.cornerRadius)
          .opacity(0.5);
        } else {
          // 简单的灰色占位
          // 这里用一个透明矩形模拟占位，避免使用 Color.fromARGB（在当前版本中无该 API）
          // 改为使用背景色和透明度控制
          Blank()
            .width(this.widthValue)
            .height(this.heightValue);
        }
      } else {
        // 正常加载完成
        Image(this.realSrc as ImageSourceType)
          .width(this.widthValue)
          .height(this.heightValue)
          .objectFit(this.objectFit)
          .borderRadius(this.cornerRadius);
      }
    }
  }
}
