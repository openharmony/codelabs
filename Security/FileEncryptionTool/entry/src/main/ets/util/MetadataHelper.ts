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

import { util } from '@kit.ArkTS';

/**
 * 文件元数据接口
 */
export interface FileMetadata {
  originalExtension: string; // 原始文件扩展名
  timestamp: string; // ISO格式时间戳
  version: string; // 元数据版本号
  algorithm?: string; // 算法标识：'AES-GCM' 或 'RSA-PKCS1-SHA256'
  fileSize?: number; // 文件大小（字节）
  tag?: string; // GCM Tag
  hash?: string; // 文件哈希（Base64）
}

/**
 * 元数据解析结果
 */
export interface MetadataParseResult {
  success: boolean;
  metadata?: FileMetadata;
  data?: ArrayBuffer;
  tag?: ArrayBuffer;
  message?: string;
}

/**
 * 元数据助手类 - 统一处理元数据头的创建和解析
 */
export class MetadataHelper {
  // 魔数常量：0x4D455441 = "META"
  private static readonly MAGIC_NUMBER = 0x4D455441;

  // 当前元数据版本
  private static readonly CURRENT_VERSION = '1.0';

  // 最小有效数据大小（魔数4 + 头长度4）
  private static readonly MIN_DATA_SIZE = 8;

  /**
   * 创建带元数据头的二进制数据
   * @param data 原始数据
   * @param metadata 元数据
   * @returns 包含元数据头的二进制数据
   */
  static createDataWithMetadata(data: ArrayBuffer, metadata: FileMetadata): ArrayBuffer {
    // 确保元数据有版本信息
    const finalMetadata: FileMetadata = {
      ...metadata,
      version: metadata.version || this.CURRENT_VERSION
    };

    // 将元数据转换为JSON字符串
    const metadataStr = JSON.stringify(finalMetadata);
    const encoder = new util.TextEncoder();
    const headerData = encoder.encode(metadataStr);

    // 计算总大小：魔数4 + 头长度4 + 元数据JSON + 原始数据
    const headerSize = 8 + headerData.length;
    const totalSize = headerSize + data.byteLength;

    // 创建合并的数据缓冲区
    const combinedData = new Uint8Array(totalSize);
    const dataView = new DataView(combinedData.buffer);

    // 写入魔数（大端序）
    dataView.setUint32(0, this.MAGIC_NUMBER, false);

    // 写入元数据长度（大端序）
    dataView.setUint32(4, headerData.length, false);

    // 写入元数据JSON
    combinedData.set(headerData, 8);

    // 写入原始数据
    combinedData.set(new Uint8Array(data), headerSize);

    return combinedData.buffer;
  }

  /**
   * 解析带元数据头的二进制数据
   * @param combinedData 包含元数据头的二进制数据
   * @returns 解析结果
   */
  static parseDataWithMetadata(combinedData: ArrayBuffer): MetadataParseResult {
    try {
      // 检查数据大小是否足够
      if (combinedData.byteLength < this.MIN_DATA_SIZE) {
        return {
          success: false,
          message: '数据太小，无法包含有效的元数据头'
        };
      }

      const dataView = new DataView(combinedData);

      // 验证魔数
      const magicNumber = dataView.getUint32(0, false);
      if (magicNumber !== this.MAGIC_NUMBER) {
        return {
          success: false,
          message: '无效的文件格式：魔数不匹配'
        };
      }

      // 读取元数据长度
      const metadataLength = dataView.getUint32(4, false);

      // 验证元数据长度是否合理
      if (metadataLength === 0 || metadataLength > 1024 * 1024) { // 限制为1MB
        return {
          success: false,
          message: `无效的元数据长度: ${metadataLength}`
        };
      }

      // 检查是否有足够的空间容纳元数据和至少1字节的原始数据
      const headerSize = 8 + metadataLength;
      if (headerSize >= combinedData.byteLength) {
        return {
          success: false,
          message: '元数据头损坏：没有足够的空间容纳原始数据'
        };
      }

      // 提取元数据JSON
      const metadataBytes = new Uint8Array(combinedData, 8, metadataLength);
      const decoder = new util.TextDecoder('utf-8');
      const metadataStr = decoder.decode(metadataBytes);

      // 解析元数据JSON
      const metadata = JSON.parse(metadataStr) as FileMetadata;

      // 验证元数据版本
      if (!metadata.version) {
        return {
          success: false,
          message: '元数据缺少版本信息'
        };
      }

      // 提取原始数据
      const data = combinedData.slice(headerSize);

      // 创建返回结果
      const result: MetadataParseResult = {
        success: true,
        metadata,
        data,
        message: '元数据头解析成功'
      };

      // 从元数据中提取 Tag（如果存在）
      if (metadata.tag) {
        try {
          result.tag = this.base64ToArrayBuffer(metadata.tag);
        } catch (error) {
          // Tag 解码失败，但不影响整体解析
          console.warn('Tag 解码失败:', error);
        }
      }

      return result;

    } catch (error) {
      return {
        success: false,
        message: `元数据解析失败: ${error.message}`
      };
    }
  }

  /**
   * 检查数据是否包含有效的元数据头
   * @param data 要检查的数据
   * @returns 是否包含有效的元数据头
   */
  static hasValidMetadataHeader(data: ArrayBuffer): boolean {
    try {
      if (data.byteLength < this.MIN_DATA_SIZE) {
        return false;
      }

      const dataView = new DataView(data);
      const magicNumber = dataView.getUint32(0, false);

      return magicNumber === this.MAGIC_NUMBER;
    } catch {
      return false;
    }
  }

  /**
   * 从文件名创建默认元数据
   * @param fileName 文件名
   * @param algorithm 算法类型
   * @param fileSize 文件大小（可选）
   * @returns 默认元数据对象
   */
  static createDefaultMetadata(
    fileName: string,
    algorithm: string,
    fileSize?: number,
    tag?: ArrayBuffer, // 直接传入 Tag 二进制数据
    hash?: ArrayBuffer // 可选的文件哈希二进制
  ): FileMetadata {
    const extension = this.getFileExtension(fileName);

    const metadata: FileMetadata = {
      originalExtension: extension,
      timestamp: new Date().toISOString(),
      version: this.CURRENT_VERSION,
      algorithm: algorithm,
      fileSize: fileSize,
    };

    // 如果有 Tag，转换为 Base64 字符串
    if (tag && tag.byteLength > 0) {
      metadata.tag = this.arrayBufferToBase64(tag);
    }

    // 如果有 hash，转换为 Base64 字符串并存入 metadata
    if (hash && hash.byteLength > 0) {
      metadata.hash = this.arrayBufferToBase64(hash);
    }

    return metadata;
  }

  /**
   * ArrayBuffer 转换为 Base64 字符串
   */
  static arrayBufferToBase64(buffer: ArrayBuffer): string {
    try {
      const base64Helper = new util.Base64Helper();
      const uint8Array = new Uint8Array(buffer);
      return base64Helper.encodeToStringSync(uint8Array);
    } catch (error) {
      console.error('ArrayBuffer 转 Base64 失败:', error);
      return '';
    }
  }

  /**
   * Base64 字符串转换为 ArrayBuffer
   */
  static base64ToArrayBuffer(base64: string): ArrayBuffer {
    try {
      const base64Helper = new util.Base64Helper();
      const uint8Array = base64Helper.decodeSync(base64);
      return uint8Array.buffer;
    } catch (error) {
      console.error('Base64 转 ArrayBuffer 失败:', error);
      return new ArrayBuffer(0);
    }
  }

  /**
   * 从文件名中提取扩展名
   * @param fileName 文件名
   * @returns 文件扩展名（不含点）
   */
  static getFileExtension(fileName: string): string {
    if (!fileName) {
      return '';
    }

    const parts = fileName.split('.');
    return parts.length > 1 ? parts[parts.length - 1] : '';
  }

  /**
   * 从文件名中提取基础名称（去除扩展名）
   * @param fileName 文件名
   * @returns 基础文件名
   */
  static getFileBaseName(fileName: string): string {
    if (!fileName) {
      return '';
    }

    const lastDotIndex = fileName.lastIndexOf('.');
    if (lastDotIndex === -1) {
      return fileName;
    }

    return fileName.substring(0, lastDotIndex);
  }

  /**
   * 生成带元数据头的文件名
   * @param originalFileName 原始文件名
   * @param suffix 后缀（如：encrypted, signature）
   * @param originalExtension 原始扩展名（可选，从元数据中获取）
   * @returns 生成的文件名
   */
  static generateMetadataFileName(originalFileName: string, suffix: string, originalExtension?: string): string {
    const baseName = this.getFileBaseName(originalFileName);

    if (originalExtension) {
      return `${baseName}.${originalExtension}.${suffix}`;
    } else {
      return `${baseName}.${suffix}`;
    }
  }
}

