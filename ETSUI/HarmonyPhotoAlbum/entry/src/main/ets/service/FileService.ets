import fs from '@ohos.file.fs';
import common from '@ohos.app.ability.common';

/**
 * 文件操作服务类
 * 负责处理文件的复制、保存和路径生成
 */
export class FileService {

  /**
   * 将选中的图片 URI 复制到应用的沙箱目录
   * @param context UIAbilityContext (用于获取沙箱路径)
   * @param sourceUri 选择器返回的原始 URI
   * @returns Promise<string> 返回保存后的沙箱文件路径
   */
  public static async saveFileToSandbox(context: common.UIAbilityContext, sourceUri: string): Promise<string> {
    let fileFd = -1;
    try {
      // 1. 使用 fs.openSync 打开源文件 (以只读模式)
      const srcFile = fs.openSync(sourceUri, fs.OpenMode.READ_ONLY);
      fileFd = srcFile.fd;

      // 2. 生成一个新的唯一文件名
      const newFileName = `img_${new Date().getTime()}.jpg`;

      // 3. 确定目标路径 (应用沙箱的 files 目录)
      const targetPath = `${context.filesDir}/${newFileName}`;

      // 4. 执行复制操作
      fs.copyFileSync(fileFd, targetPath);

      console.info(`[FileService] 图片已成功保存到: ${targetPath}`);

      // 5. 关闭源文件流
      fs.closeSync(fileFd);

      return targetPath;

    } catch (error) {
      console.error(`[FileService] 保存文件失败: ${JSON.stringify(error)}`);

      if (fileFd !== -1) {
        fs.closeSync(fileFd);
      }

      // --- 修改点开始 ---
      // ArkTS 不允许直接 throw unknown 类型的 error
      // 我们创建一个新的 Error 对象抛出
      let errMsg = '';
      if (error instanceof Error) {
        errMsg = error.message;
      } else {
        errMsg = JSON.stringify(error);
      }
      throw new Error(errMsg);
      // --- 修改点结束 ---
    }
  }
  public static getFileInfo(filePath: string) {
    return fs.statSync(filePath);
  }
}