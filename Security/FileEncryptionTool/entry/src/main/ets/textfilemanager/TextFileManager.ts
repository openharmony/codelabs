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

import fs from '@ohos.file.fs';
import Logger from '../util/Logger';
import { util } from '@kit.ArkTS';
import { CryptoOperation } from '../cryptoframework/CryptoOperation'; // 导入加密操作类

const TAG: string = '[Crypto_Framework]';
const FILE_SLEEP_TIME: number = 100;
const TEXT_MAX_READ_LEN: number = 8192;
const BINARY_BUFFER_SIZE: number = 65536; // 64KB buffer for binary operations
const LARGE_FILE_THRESHOLD: number = 10 * 1024 * 1024; // 10MB threshold for large files

// filePicker和文件管理，在OH 4.0.8.2镜像版本，读取uri后，无法直接read、write文件内容，需要sleep几十ms，这里sleep100毫秒
function sleep(time: number): Promise<number> {
  return new Promise((resolve) => setTimeout(resolve, time));
}

// 扩展文件类型枚举，增加文档类型
enum FileType {
  TEXT = 'text',
  DOCUMENT = 'document', // Word, PDF, PPT, Excel等
  IMAGE = 'image',
  AUDIO = 'audio',
  VIDEO = 'video',
  ARCHIVE = 'archive', // 压缩文件
  ENCRYPTED = 'encrypted', // 加密文件类型
  SIGNATURE = 'signature', // 签名文件类型
  UNKNOWN = 'unknown'
}

// 文件操作结果接口
interface FileOperationResult {
  success: boolean;
  message: string;
  data?: any;
  bytesProcessed?: number;
  originalExtension?: string;
}

// 文件信息接口
interface FileInfo {
  name: string;
  path: string;
  size: number;
  type: FileType;
  extension: string;
  createdTime?: number;
  modifiedTime?: number;
}

// 元数据头接口
interface FileHeader {
  magic: string;          // 魔数标识 "OPFE"
  version: number;        // 格式版本
  originalExtension: string; // 原始文件扩展名
  originalSize: number;   // 原始文件大小
  timestamp: number;      // 加密时间戳
  headerSize: number;     // 头大小
}

// 加密操作结果接口
interface EncryptionResult {
  success: boolean;
  message: string;
  encryptedData?: ArrayBuffer;
  header?: FileHeader;
  bytesProcessed?: number;
}

class EnhancedFileManager {
  private static readString: string = '';
  private static readBuffer: ArrayBuffer = new ArrayBuffer(0);
  private static readResult: number = 0;
  private static writeResult: number = 0;
  private cryptoOp: CryptoOperation = new CryptoOperation();

  // 元数据头相关常量
  private readonly FILE_HEADER_MAGIC = 'OPFE'; // OpenHarmony Protected File Encryption
  private readonly FILE_HEADER_VERSION = 0x01;
  private readonly FILE_HEADER_SIZE = 128; // 固定头大小
  private readonly MAX_EXTENSION_LENGTH = 32; // 扩展名最大长度

  // 扩展文件类型分类
  private readonly textExtensions = ['txt', 'md', 'json', 'xml', 'html', 'css', 'js', 'ts', 'csv'];
  private readonly documentExtensions = ['doc', 'docx', 'pdf', 'ppt', 'pptx', 'xls', 'xlsx', 'rtf', 'odt', 'ods', 'odp'];
  private readonly imageExtensions = ['jpg', 'jpeg', 'png', 'gif', 'bmp', 'webp', 'svg', 'ico', 'tiff'];
  private readonly audioExtensions = ['mp3', 'wav', 'ogg', 'aac', 'flac', 'm4a', 'wma'];
  private readonly videoExtensions = ['mp4', 'avi', 'mov', 'mkv', 'webm', 'flv', 'wmv', 'mpeg'];
  private readonly archiveExtensions = ['zip', 'rar', '7z', 'tar', 'gz', 'bz2'];
  private readonly encryptedExtensions = ['encrypted', 'crypto', 'aes', 'enc'];
  private readonly signatureExtensions = ['signature', 'sig', 'p7s', 'detached'];

  /**
   * 创建元数据头
   */
  private createFileHeader(originalExtension: string, originalSize: number): ArrayBuffer {
    const headerBuffer = new ArrayBuffer(this.FILE_HEADER_SIZE);
    const dataView = new DataView(headerBuffer);

    let offset = 0;

    // 写入魔数 (4字节)
    const magicBytes = new util.TextEncoder().encode(this.FILE_HEADER_MAGIC);
    for (let i = 0; i < magicBytes.length; i++) {
      dataView.setUint8(offset + i, magicBytes[i]);
    }
    offset += 4;

    // 写入版本号 (1字节)
    dataView.setUint8(offset, this.FILE_HEADER_VERSION);
    offset += 1;

    // 写入原始文件扩展名长度 (1字节) 和扩展名
    const extBytes = new util.TextEncoder().encode(originalExtension.substring(0, this.MAX_EXTENSION_LENGTH));
    dataView.setUint8(offset, extBytes.length);
    offset += 1;

    // 写入扩展名 (最大32字节)
    for (let i = 0; i < this.MAX_EXTENSION_LENGTH; i++) {
      if (i < extBytes.length) {
        dataView.setUint8(offset + i, extBytes[i]);
      } else {
        dataView.setUint8(offset + i, 0); // 填充0
      }
    }
    offset += this.MAX_EXTENSION_LENGTH;

    // 写入原始文件大小 (8字节)
    dataView.setBigUint64(offset, BigInt(originalSize), true);
    offset += 8;

    // 写入时间戳 (8字节)
    dataView.setBigUint64(offset, BigInt(Date.now()), true);
    offset += 8;

    // 写入头大小 (4字节)
    dataView.setUint32(offset, this.FILE_HEADER_SIZE, true);
    offset += 4;

    // 剩余空间用0填充
    while (offset < this.FILE_HEADER_SIZE) {
      dataView.setUint8(offset, 0);
      offset++;
    }

    return headerBuffer;
  }

  /**
   * 解析元数据头
   */
  private parseFileHeader(headerBuffer: ArrayBuffer): FileHeader | null {
    if (headerBuffer.byteLength < this.FILE_HEADER_SIZE) {
      Logger.error(TAG, 'Header buffer too small');
      return null;
    }

    const dataView = new DataView(headerBuffer);
    let offset = 0;

    try {
      // 读取并验证魔数
      const magicBytes = new Uint8Array(headerBuffer, 0, 4);
      const magic = new util.TextDecoder().decode(magicBytes);
      if (magic !== this.FILE_HEADER_MAGIC) {
        Logger.error(TAG, 'Invalid file header magic');
        return null;
      }
      offset += 4;

      // 读取版本号
      const version = dataView.getUint8(offset);
      offset += 1;

      if (version !== this.FILE_HEADER_VERSION) {
        Logger.error(TAG, `Unsupported header version: ${version}`);
        return null;
      }

      // 读取扩展名长度和扩展名
      const extLength = dataView.getUint8(offset);
      offset += 1;

      const extBytes = new Uint8Array(headerBuffer, offset, extLength);
      const originalExtension = new util.TextDecoder().decode(extBytes);
      offset += this.MAX_EXTENSION_LENGTH;

      // 读取原始文件大小
      const originalSize = Number(dataView.getBigUint64(offset, true));
      offset += 8;

      // 读取时间戳
      const timestamp = Number(dataView.getBigUint64(offset, true));
      offset += 8;

      // 读取头大小
      const headerSize = dataView.getUint32(offset, true);
      offset += 4;

      return {
        magic,
        version,
        originalExtension,
        originalSize,
        timestamp,
        headerSize
      };
    } catch (error) {
      Logger.error(TAG, `Parse file header failed: ${error.message}`);
      return null;
    }
  }

  /**
   * 检测文件是否包含元数据头（加密文件）
   */
  async isEncryptedFile(fileUri: string): Promise<{isEncrypted: boolean, originalExtension?: string}> {
    try {
      const fileInfo = await this.getFileInfo(fileUri);
      if (fileInfo.size < this.FILE_HEADER_SIZE) {
        return { isEncrypted: false };
      }

      // 读取文件头部分进行检查
      const headerResult = await this.readBinaryFilePartial(fileUri, 0, this.FILE_HEADER_SIZE);
      if (!headerResult.success) {
        return { isEncrypted: false };
      }

      const headerBuffer = headerResult.data as ArrayBuffer;
      const fileHeader = this.parseFileHeader(headerBuffer);

      if (fileHeader) {
        return {
          isEncrypted: true,
          originalExtension: fileHeader.originalExtension
        };
      } else {
        return { isEncrypted: false };
      }
    } catch (error) {
      Logger.error(TAG, `Check encrypted file failed: ${error.message}`);
      return { isEncrypted: false };
    }
  }

  /**
   * 部分读取文件（用于检测文件头）
   */
  private async readBinaryFilePartial(fileUri: string, offset: number, length: number): Promise<FileOperationResult> {
    await sleep(FILE_SLEEP_TIME);
    try {
      const file = fs.openSync(fileUri, fs.OpenMode.READ_ONLY);
      try {
        const buffer = new ArrayBuffer(length);
        let fd: number = file.fd;
        const readLen: number = fs.readSync(fd, buffer, {
          offset: offset,
          length: length
        });

        if (readLen === length) {
          return {
            success: true,
            message: '文件部分读取成功',
            data: buffer
          };
        } else {
          return {
            success: false,
            message: `文件读取不完整，期望${length}字节，实际${readLen}字节`
          };
        }
      } finally {
        fs.closeSync(file);
      }
    } catch (error) {
      Logger.error(TAG, `文件部分读取失败: ${error.message}`);
      return {
        success: false,
        message: `文件部分读取失败: ${error.message}`
      };
    }
  }

  /**
   * 增强的文件类型检测
   */
  private detectFileType(fileUri: string): FileType {
    const extension = fileUri.split('.').pop()?.toLowerCase() || '';

    if (this.textExtensions.includes(extension)) return FileType.TEXT;
    if (this.documentExtensions.includes(extension)) return FileType.DOCUMENT;
    if (this.imageExtensions.includes(extension)) return FileType.IMAGE;
    if (this.audioExtensions.includes(extension)) return FileType.AUDIO;
    if (this.videoExtensions.includes(extension)) return FileType.VIDEO;
    if (this.archiveExtensions.includes(extension)) return FileType.ARCHIVE;
    if (this.encryptedExtensions.includes(extension)) return FileType.ENCRYPTED;
    if (this.signatureExtensions.includes(extension)) return FileType.SIGNATURE;

    return FileType.UNKNOWN;
  }

  /**
   * 获取详细的文件信息
   */
  async getFileInfo(fileUri: string): Promise<FileInfo> {
    let file: fs.File | null = null;
    try {
      // 直接使用URI打开文件
      file = fs.openSync(fileUri, fs.OpenMode.READ_ONLY);
      // 使用文件描述符获取文件信息
      const fileStats = await fs.stat(file.fd);
      const fileName = fileUri.split('/').pop() || '未知文件';
      const extension = fileName.split('.').pop()?.toLowerCase() || '';
      const fileType = this.detectFileType(fileUri);

      return {
        name: fileName,
        path: fileUri, // 保持原始URI
        size: fileStats.size,
        type: fileType,
        extension: extension,
        createdTime: fileStats.ctime,
        modifiedTime: fileStats.mtime
      };
    } catch (error) {
      Logger.error(TAG, `获取文件信息失败, ${error.code}, ${error.message}`);
      throw new Error(`无法获取文件信息: ${error.message}`);
    } finally {
      // 确保文件被关闭
      if (file != null) {
        fs.closeSync(file);
      }
    }
  }

  /**
   * 智能文件读取 - 根据文件类型自动选择最佳读取方式
   */
  async readTextFile(fileUri: string): Promise<FileOperationResult> {
    await sleep(FILE_SLEEP_TIME);

    try {
      const fileInfo = await this.getFileInfo(fileUri);
      let result: ArrayBuffer;
      // 根据文件类型选择读取策略
      switch (fileInfo.type) {
        case FileType.TEXT:
          // 小文本或任意文本文件，统一返回二进制（UTF-8 编码）
          result = await this.readTextFileInternal(fileUri);
          break;
          break;

        case FileType.DOCUMENT:
        case FileType.IMAGE:
        case FileType.AUDIO:
        case FileType.VIDEO:
        case FileType.ARCHIVE:
        case FileType.ENCRYPTED:
        case FileType.SIGNATURE:
        case FileType.UNKNOWN:
          // 所有二进制文件使用二进制读取
          result = await this.readBinaryFileInternal(fileUri);
          break;

        default:
          result = await this.readBinaryFileInternal(fileUri);
      }

      return {
        success: true,
        message: '文件读取成功',
        data: result,
        bytesProcessed: fileInfo.size
      };

    } catch (error) {
      Logger.error(TAG, `文件读取失败, ${error.code}, ${error.message}`);
      return {
        success: false,
        message: `文件读取失败: ${error.message}`
      };
    }
  }

  /**
   * 内部文本文件读取实现
   */
  private async readTextFileInternal(fileUri: string): Promise<ArrayBuffer> {
    let file: number | null = null;
    try {
      let file: fs.File = fs.openSync(fileUri, fs.OpenMode.READ_ONLY);
      let fd: number = file.fd;
      Logger.info(TAG, `读取文本文件(二进制返回), fd: ${file}`);

      const buf = new ArrayBuffer(TEXT_MAX_READ_LEN);
      const readLen: number = fs.readSync(fd, buf, { offset: 0 });

      if (readLen > 0) {
        EnhancedFileManager.readBuffer = buf.slice(0, readLen);
        EnhancedFileManager.readResult = readLen;
        try {
          const decoder = new util.TextDecoder('utf-8');
          EnhancedFileManager.readString = decoder.decode(new Uint8Array(EnhancedFileManager.readBuffer).slice(0, 1000));
        } catch (e) {
          EnhancedFileManager.readString = '';
        }
        return EnhancedFileManager.readBuffer;
      } else {
        return new ArrayBuffer(0);
      }
    } finally {
      if (file !== null) {
        fs.closeSync(file);
      }
    }
  }

  /**
   * 内部二进制文件读取实现（支持大文件分块读取）
   */
  private async readBinaryFileInternal(fileUri: string): Promise<ArrayBuffer> {
    const fileInfo = await this.getFileInfo(fileUri);
    let file: number | null = null;

    try {
      let file: fs.File = fs.openSync(fileUri, fs.OpenMode.READ_ONLY);
      let fd: number = file.fd;
      Logger.info(TAG, `读取二进制文件, fd: ${file}, 大小: ${fileInfo.size} bytes`);

      // 小文件一次性读取
      if (fileInfo.size <= BINARY_BUFFER_SIZE) {
        const buffer = new ArrayBuffer(fileInfo.size);
        const readLen: number = fs.readSync(fd, buffer, { offset: 0 });
        EnhancedFileManager.readBuffer = buffer.slice(0, readLen);
        EnhancedFileManager.readResult = readLen;
        return EnhancedFileManager.readBuffer;
      }

      // 大文件分块读取
      const buffer = new ArrayBuffer(fileInfo.size);
      let bytesRead = 0;

      while (bytesRead < fileInfo.size) {
        const chunkSize = Math.min(BINARY_BUFFER_SIZE, fileInfo.size - bytesRead);
        const chunkBuffer = new ArrayBuffer(chunkSize);
        const readLen: number = fs.readSync(fd, chunkBuffer, {
          offset: bytesRead,
          length: chunkSize
        });

        if (readLen === 0) break;

        // 复制数据到主缓冲区
        const mainView = new Uint8Array(buffer, bytesRead, readLen);
        const chunkView = new Uint8Array(chunkBuffer, 0, readLen);
        mainView.set(chunkView);

        bytesRead += readLen;
      }

      EnhancedFileManager.readBuffer = buffer.slice(0, bytesRead);
      EnhancedFileManager.readResult = bytesRead;
      Logger.info(TAG, `二进制文件读取完成, 总字节数: ${bytesRead}`);

      return EnhancedFileManager.readBuffer;

    } finally {
      if (file !== null) {
        fs.closeSync(file);
      }
    }
  }

  /**
   * 智能文件写入 - 自动处理文本和二进制数据
   */
  async writeTextFile(fileUri: string, data: string | ArrayBuffer | Uint8Array): Promise<FileOperationResult> {
    await sleep(FILE_SLEEP_TIME);

    try {
      if (typeof data === 'string') {
        return await this.writeTextFileInternal(fileUri, data);
      } else {
        return await this.writeBinaryFileInternal(fileUri, data);
      }
    } catch (error) {
      Logger.error(TAG, `文件写入失败, ${error.code}, ${error.message}`);
      return {
        success: false,
        message: `文件写入失败: ${error.message}`
      };
    }
  }

  /**
   * 内部文本文件写入实现
   */
  private async writeTextFileInternal(fileUri: string, textString: string): Promise<FileOperationResult> {
    let file: number | null = null;
    try {
      let file: fs.File = fs.openSync(fileUri, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
      let fd: number = file.fd;
      const writeLen = fs.writeSync(fd, textString);

      EnhancedFileManager.writeResult = writeLen;
      Logger.info(TAG, `文本文件写入成功, 写入字节数: ${writeLen}`);

      return {
        success: true,
        message: '文本文件写入成功',
        bytesProcessed: writeLen
      };
    } finally {
      if (file !== null) {
        fs.closeSync(file);
      }
    }
  }

  /**
   * 内部二进制文件写入实现（支持大文件分块写入）
   */
  private async writeBinaryFileInternal(fileUri: string, data: ArrayBuffer | Uint8Array): Promise<FileOperationResult> {
    let file: number | null = null;
    try {
      let file: fs.File = fs.openSync(fileUri, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
      let fd: number = file.fd;
      const dataView = data instanceof Uint8Array ? data : new Uint8Array(data);

      let totalWritten = 0;
      const chunkSize = BINARY_BUFFER_SIZE;

      // 分块写入大文件
      while (totalWritten < dataView.length) {
        const chunkEnd = Math.min(totalWritten + chunkSize, dataView.length);
        const chunk = dataView.slice(totalWritten, chunkEnd);
        const writeLen = fs.writeSync(fd, chunk.buffer, {
          offset: totalWritten,
          length: chunk.length
        });

        totalWritten += writeLen;
        if (writeLen === 0) break;
      }

      EnhancedFileManager.writeResult = totalWritten;
      Logger.info(TAG, `二进制文件写入完成, 总字节数: ${totalWritten}`);

      return {
        success: true,
        message: '二进制文件写入成功',
        bytesProcessed: totalWritten
      };
    } finally {
      if (file !== null) {
        fs.closeSync(file);
      }
    }
  }

  /**
   * 增强的文件加密方法 - 包含元数据头，与CryptoOperation结合
   */
  async encryptFileWithHeader(sourceUri: string, targetUri: string, encryptionKey: string): Promise<FileOperationResult> {
    try {
      // 获取源文件信息
      const fileInfo = await this.getFileInfo(sourceUri);

      // 读取源文件内容

      const readResult = await this.readTextFile(sourceUri);
      if (!readResult.success) {
        return readResult;
      }

      const fileData = readResult.data as ArrayBuffer;

      // 创建元数据头
      const fileHeader = this.createFileHeader(fileInfo.extension, fileData.byteLength);

      // 使用CryptoOperation进行加密
      const encryptResult = await this.cryptoOp.aesConvertAndEncryptBinary(encryptionKey, fileData);
      if (!encryptResult.success) {
        return {
          success: false,
          message: `加密失败: ${encryptResult.error}`
        };
      }

      // 组合元数据头和加密数据
      const combinedData = new Uint8Array(fileHeader.byteLength + encryptResult.data.byteLength);
      combinedData.set(new Uint8Array(fileHeader), 0);
      combinedData.set(new Uint8Array(encryptResult.data), fileHeader.byteLength);

      // 写入目标文件
      const writeResult = await this.writeTextFile(targetUri, combinedData.buffer);
      if (!writeResult.success) {
        return writeResult;
      }

      Logger.info(TAG, `文件加密成功，原始格式: ${fileInfo.extension}, 头大小: ${fileHeader.byteLength}字节`);

      return {
        success: true,
        message: '文件加密成功',
        bytesProcessed: fileData.byteLength,
        originalExtension: fileInfo.extension
      };

    } catch (error) {
      Logger.error(TAG, `文件加密失败, ${error.code}, ${error.message}`);
      return {
        success: false,
        message: `文件加密失败: ${error.message}`
      };
    }
  }

  /**
   * 增强的文件解密方法 - 解析元数据头，与CryptoOperation结合
   */
  async decryptFileWithHeader(sourceUri: string, targetUri: string, decryptionKey: string): Promise<FileOperationResult> {
    try {
      // 读取加密文件（包含元数据头）
      const readResult = await this.readTextFile(sourceUri);
      if (!readResult.success) {
        return readResult;
      }

      const encryptedFileData = readResult.data as ArrayBuffer;

      // 检查文件大小是否足够包含元数据头
      if (encryptedFileData.byteLength < this.FILE_HEADER_SIZE) {
        return {
          success: false,
          message: '文件太小，可能不是有效的加密文件'
        };
      }

      // 解析元数据头
      const headerBuffer = encryptedFileData.slice(0, this.FILE_HEADER_SIZE);
      const fileHeader = this.parseFileHeader(headerBuffer);

      if (!fileHeader) {
        return {
          success: false,
          message: '无效的加密文件格式或文件已损坏'
        };
      }

      // 提取加密数据（头之后的内容）
      const cipherText = encryptedFileData.slice(this.FILE_HEADER_SIZE);

      // 使用CryptoOperation进行解密
      // 注意：这里需要从加密结果中获取tag，但当前实现需要调整
      // 暂时使用空tag，实际使用时需要确保tag的正确传递
      const emptyTag = new ArrayBuffer(0);
      const decryptResult = await this.cryptoOp.aesConvertAndDecryptBinary(decryptionKey, cipherText, emptyTag);

      if (!decryptResult.success) {
        return {
          success: false,
          message: `解密失败: ${decryptResult.error}`
        };
      }

      // 验证解密后数据大小与头中记录的是否一致
      if (decryptResult.data.byteLength !== fileHeader.originalSize) {
        Logger.warn(TAG, `解密后文件大小不匹配: 头中记录=${fileHeader.originalSize}, 实际=${decryptResult.data.byteLength}`);
      }

      // 写入解密后的文件，使用原始扩展名
      const originalExtension = fileHeader.originalExtension;
      const finalTargetUri = targetUri + (originalExtension ? '.' + originalExtension : '');
      const writeResult = await this.writeTextFile(finalTargetUri, decryptResult.data);
      if (!writeResult.success) {
        return writeResult;
      }

      Logger.info(TAG, `文件解密成功，原始格式: ${fileHeader.originalExtension}, 解密大小: ${decryptResult.data.byteLength}字节`);

      return {
        success: true,
        message: '文件解密成功',
        bytesProcessed: decryptResult.data.byteLength,
        originalExtension: fileHeader.originalExtension
      };

    } catch (error) {
      Logger.error(TAG, `文件解密失败, ${error.code}, ${error.message}`);
      return {
        success: false,
        message: `文件解密失败: ${error.message}`
      };
    }
  }

  /**
   * 文件哈希计算（用于完整性验证）
   */
  async calculateFileHash(fileUri: string, algorithm: string = 'SHA256'): Promise<FileOperationResult> {
    try {

      const readResult = await this.readTextFile(fileUri);
      if (!readResult.success) {
        return readResult;
      }

      const fileData = readResult.data as ArrayBuffer;

      const hashResult = await this.cryptoOp.calculateFileHash(fileData, algorithm);
      if (!hashResult.success) {
        return {
          success: false,
          message: `哈希计算失败: ${hashResult.error}`
        };
      }

      return {
        success: true,
        message: '文件哈希计算成功',
        data: hashResult.data
      };

    } catch (error) {
      Logger.error(TAG, `文件哈希计算失败, ${error.code}, ${error.message}`);
      return {
        success: false,
        message: `文件哈希计算失败: ${error.message}`
      };
    }
  }

  /**
   * 文件签名 - 使用RSA对文件进行数字签名
   */
  async signFile(fileUri: string, rsaKeyJson: string): Promise<FileOperationResult> {
    try {

      const readResult = await this.readTextFile(fileUri);
      if (!readResult.success) {
        return readResult;
      }

      const fileData = readResult.data as ArrayBuffer;

      const signResult = await this.cryptoOp.rsaConvertAndSignBinary(rsaKeyJson, fileData);
      if (!signResult.success) {
        return {
          success: false,
          message: `文件签名失败: ${signResult.error}`
        };
      }

      return {
        success: true,
        message: '文件签名成功',
        data: signResult.data
      };

    } catch (error) {
      Logger.error(TAG, `文件签名失败, ${error.code}, ${error.message}`);
      return {
        success: false,
        message: `文件签名失败: ${error.message}`
      };
    }
  }

  /**
   * 文件验签 - 验证文件数字签名
   */
  async verifyFileSignature(fileUri: string, signature: ArrayBuffer, rsaKeyJson: string): Promise<{success: boolean, valid?: boolean, message: string}> {
    try {
      const readResult = await this.readTextFile(fileUri);
      if (!readResult.success) {
        return {
          success: false,
          message: `文件读取失败: ${readResult.message}`
        };
      }

      let fileData: ArrayBuffer;
      if (typeof readResult.data === 'string') {
        const encoder = new util.TextEncoder();
        fileData = encoder.encode(readResult.data).buffer;
      } else {
        fileData = readResult.data as ArrayBuffer;
      }

      const verifyResult = await this.cryptoOp.rsaConvertAndVerifyBinary(rsaKeyJson, fileData, signature);
      if (!verifyResult.success) {
        return {
          success: false,
          message: `签名验证失败: ${verifyResult.error}`
        };
      }

      return {
        success: true,
        valid: verifyResult.valid,
        message: verifyResult.valid ? '签名验证成功' : '签名验证失败'
      };

    } catch (error) {
      Logger.error(TAG, `文件验签失败, ${error.code}, ${error.message}`);
      return {
        success: false,
        message: `文件验签失败: ${error.message}`
      };
    }
  }

  /**
   * 文件复制（支持大文件，自动选择最优策略）
   */
  async copyFile(sourceUri: string, targetUri: string): Promise<FileOperationResult> {
    try {
      const sourceInfo = await this.getFileInfo(sourceUri);
      Logger.info(TAG, `复制文件: ${sourceUri} -> ${targetUri}, 大小: ${sourceInfo.size} bytes`);

      // 小文件直接读取写入
      if (sourceInfo.size <= BINARY_BUFFER_SIZE) {
        const readResult = await this.readTextFile(sourceUri);
        if (!readResult.success) {
          return readResult;
        }

        return await this.writeTextFile(targetUri, readResult.data as ArrayBuffer);
      }

      // 大文件使用流式复制
      return await this.streamCopyFile(sourceUri, targetUri);

    } catch (error) {
      Logger.error(TAG, `文件复制失败, ${error.code}, ${error.message}`);
      return {
        success: false,
        message: `文件复制失败: ${error.message}`
      };
    }
  }

  /**
   * 流式文件复制（用于大文件）
   */
  private async streamCopyFile(sourceUri: string, targetUri: string): Promise<FileOperationResult> {
    let sourceFile: number | null = null;
    let targetFile: number | null = null;

    try {
      let sourceFile = fs.openSync(sourceUri, fs.OpenMode.READ_ONLY);
      let targetFile = fs.openSync(targetUri, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
      let s_fd: number = sourceFile.fd;
      let t_fd: number = targetFile.fd;

      const fileInfo = await this.getFileInfo(sourceUri);
      let bytesCopied = 0;
      const buffer = new ArrayBuffer(BINARY_BUFFER_SIZE);

      while (bytesCopied < fileInfo.size) {
        const chunkSize = Math.min(BINARY_BUFFER_SIZE, fileInfo.size - bytesCopied);
        const readLen = fs.readSync(s_fd, buffer, {
          offset: bytesCopied,
          length: chunkSize
        });

        if (readLen === 0) break;

        fs.writeSync(t_fd, buffer, {
          offset: bytesCopied,
          length: readLen
        });

        bytesCopied += readLen;
      }

      Logger.info(TAG, `流式复制完成, 总字节数: ${bytesCopied}`);
      return {
        success: true,
        message: '文件复制成功',
        bytesProcessed: bytesCopied
      };
    } finally {
      if (sourceFile !== null) fs.closeSync(sourceFile);
      if (targetFile !== null) fs.closeSync(targetFile);
    }
  }

  /**
   * 批量文件操作
   */
  async batchProcessFiles(
    fileUris: string[],
    processor: (data: ArrayBuffer) => Promise<ArrayBuffer>
  ): Promise<FileOperationResult[]> {
    const results: FileOperationResult[] = [];

    for (const fileUri of fileUris) {
      try {
        const readResult = await this.readTextFile(fileUri);
        if (!readResult.success) {
          results.push(readResult);
          continue;
        }

        const fileData = readResult.data as ArrayBuffer;

        const processedData = await processor(fileData);
        const tempUri = fileUri + '.processed';
        const writeResult = await this.writeTextFile(tempUri, processedData);

        results.push(writeResult);
      } catch (error) {
        results.push({
          success: false,
          message: `处理文件失败: ${error.message}`
        });
      }
    }

    return results;
  }

  // 保持向后兼容的方法
  getString(): string {
    return EnhancedFileManager.readString;
  }

  getBinaryData(): ArrayBuffer {
    return EnhancedFileManager.readBuffer;
  }

  /**
   * 将二进制数据转换为Base64字符串
   */
  binaryToBase64(): string {
    if (EnhancedFileManager.readBuffer.byteLength === 0) {
      return '';
    }

    try {
      const base64Helper = new util.Base64Helper();
      const uint8Array = new Uint8Array(EnhancedFileManager.readBuffer);
      return base64Helper.encodeToStringSync(uint8Array);
    } catch (error) {
      Logger.error(TAG, `Base64编码失败: ${error.message}`);
      return '';
    }
  }

  /**
   * 将Base64字符串转换为二进制数据
   */
  base64ToBinary(base64: string): ArrayBuffer {
    if (!base64 || base64.trim() === '') {
      return new ArrayBuffer(0);
    }

    try {
      const base64Helper = new util.Base64Helper();
      const uint8Array = base64Helper.decodeSync(base64);
      return uint8Array.buffer;
    } catch (error) {
      Logger.error(TAG, `Base64解码失败: ${error.message}`);
      return new ArrayBuffer(0);
    }
  }

  /**
   * 获取支持的文档格式列表
   */
  getSupportedDocumentFormats(): string[] {
    return [...this.documentExtensions];
  }

  /**
   * 获取所有支持的文件格式
   */
  getAllSupportedFormats(): { [key: string]: string[] } {
    return {
      text: this.textExtensions,
      document: this.documentExtensions,
      image: this.imageExtensions,
      audio: this.audioExtensions,
      video: this.videoExtensions,
      archive: this.archiveExtensions
    };
  }
}

export default new EnhancedFileManager();