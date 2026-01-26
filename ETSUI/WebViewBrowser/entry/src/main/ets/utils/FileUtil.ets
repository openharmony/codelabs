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

import { common, Want, wantConstant } from '@kit.AbilityKit';
import { fileIo as fs, fileUri } from '@kit.CoreFileKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const DOMAIN = 0x0002;
const TAG = 'FileUtil';

/**
 * 文件操作工具类
 * 负责文件打开、复制、URI转换等底层操作
 */
export class FileUtil {
  /**
   * 调用系统能力打开文件
   * @param context UIAbilityContext
   * @param filePath 文件沙箱路径
   * @param mimeType 文件MIME类型
   */
  static async openFile(context: common.UIAbilityContext, filePath: string, mimeType?: string): Promise<boolean> {
    try {
      // 检查文件是否存在
      if (!fs.accessSync(filePath)) {
        hilog.error(DOMAIN, TAG, 'File not found: %{public}s', filePath);
        return false;
      }

      // 获取文件的URI
      const uri = fileUri.getUriFromPath(filePath);
      hilog.info(DOMAIN, TAG, 'Opening file uri: %{public}s, type: %{public}s', uri, mimeType);

      // 构建Want
      const want: Want = {
        action: 'ohos.want.action.viewData',
        uri: uri,
        type: mimeType || '*/*', // 默认匹配所有类型
        flags: wantConstant.Flags.FLAG_AUTH_READ_URI_PERMISSION // 授权目标应用读取该URI
      };

      // 启动能力
      await context.startAbility(want);
      return true;
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Failed to open file: %{public}s', JSON.stringify(err));
      return false;
    }
  }

  /**
   * 将源文件复制到目标URI（通常用于保存到用户选择的目录）
   * @param context Context
   * @param srcPath 源文件路径
   * @param destUri 目标文件URI
   */
  static async copyToUri(context: Context, srcPath: string, destUri: string): Promise<boolean> {
    let srcFile: fs.File | null = null;
    let destFile: fs.File | null = null;
    try {
      if (!fs.accessSync(srcPath)) {
        return false;
      }

      // 打开源文件
      srcFile = fs.openSync(srcPath, fs.OpenMode.READ_ONLY);

      // 打开目标URI
      destFile = fs.openSync(destUri, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);

      // 复制内容
      const bufferSize = 4096;
      const buffer = new ArrayBuffer(bufferSize);
      let len = 0;
      let position = 0;

      while ((len = fs.readSync(srcFile.fd, buffer, { offset: position })) > 0) {
        fs.writeSync(destFile.fd, buffer, { length: len });
        position += len;
      }

      hilog.info(DOMAIN, TAG, 'File copied successfully to %{public}s', destUri);
      return true;
    } catch (err) {
      hilog.error(DOMAIN, TAG, 'Failed to copy file: %{public}s', JSON.stringify(err));
      return false;
    } finally {
      // 确保关闭文件描述符
      if (srcFile) fs.closeSync(srcFile);
      if (destFile) fs.closeSync(destFile);
    }
  }
}