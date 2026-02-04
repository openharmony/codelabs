import common from '@ohos.app.ability.common';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import fs from '@ohos.file.fs';
import { BusinessError } from '@ohos.base';
import Want from '@ohos.app.ability.Want'; // 必须引入 Want

@CustomDialog
export struct ImagePicker {
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;
  controller?: CustomDialogController;
  onImageSelected: (savedPath: string) => void = (path: string): void => {};

  async requestCameraPermission(): Promise<boolean> {
    const atManager = abilityAccessCtrl.createAtManager();
    try {
      const grantStatus = await atManager.requestPermissionsFromUser(this.context, ['ohos.permission.CAMERA']);
      return grantStatus.authResults[0] === 0;
    } catch (err) {
      return false;
    }
  }

  // --- 核心修改：使用 startAbilityForResult ---
  async takePhoto() {
    const hasPermission = await this.requestCameraPermission();
    if (!hasPermission) {
      this.showDialog('权限不足', '请授予相机权限');
      return;
    }

    // 构造意图：动作是“拍照”
    let want: Want = {
      "action": "ohos.want.action.imageCapture",
      "parameters": {
        "callBundleName": this.context.abilityInfo.bundleName // 告诉相机是谁在叫它
      }
    };

    try {
      // 拉起相机
      const result = await this.context.startAbilityForResult(want);

      // 处理返回结果
      if (result.resultCode === 0 && result.want) {
        // 不同版本的系统，返回的 URI 位置可能不同，做个兼容处理
        let uri = result.want.uri;

        // 有些系统会放在 parameters 里的 resourceUri
        if (!uri && result.want.parameters && result.want.parameters.resourceUri) {
          uri = result.want.parameters.resourceUri as string;
        }

        if (uri) {
          console.info(`[ImagePicker] 拍照返回 URI: ${uri}`);
          this.saveCapturedImage(uri);
        } else {
          console.warn('[ImagePicker] 拍照完成了，但是没有返回 URI');
          this.showDialog('提示', '未能获取照片路径，请检查系统兼容性');
        }
      } else {
        console.info('[ImagePicker] 用户取消或拍照失败');
      }
    } catch (err) {
      let error = err as BusinessError;
      console.error(`[ImagePicker] 拉起相机失败: ${error.message}`);
      this.showDialog('错误', `无法启动相机: ${error.message}`);
    }
  }

  // Buffer 保存逻辑（保持不变，这部分是稳的）
  saveCapturedImage(srcUri: string) {
    let srcFile: fs.File | null = null;
    let destFile: fs.File | null = null;

    try {
      srcFile = fs.openSync(srcUri, fs.OpenMode.READ_ONLY);
      const fileStat = fs.statSync(srcFile.fd);
      const buffer = new ArrayBuffer(fileStat.size);
      fs.readSync(srcFile.fd, buffer);

      const fileName = `CAM_${new Date().getTime()}.jpg`;
      const destPath = `${this.context.filesDir}/${fileName}`;

      destFile = fs.openSync(destPath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE | fs.OpenMode.TRUNC);
      fs.writeSync(destFile.fd, buffer);

      console.info(`[ImagePicker] 保存成功: ${destPath}`);
      this.onImageSelected(destPath);
      this.controller?.close();

    } catch (err) {
      let error = err as BusinessError;
      console.error(`[ImagePicker] 保存失败: ${error.message}`);
      this.showDialog('保存失败', error.message);
    } finally {
      if (srcFile) fs.closeSync(srcFile);
      if (destFile) fs.closeSync(destFile);
    }
  }

  showDialog(title: string, msg: string) {
    AlertDialog.show({
      title: title,
      message: msg,
      confirm: { value: '关闭', action: () => {} }
    });
  }

  build() {
    Column({ space: 20 }) {
      Text('拍照 (Want模式)').fontSize(22).fontWeight(FontWeight.Bold).margin({ top: 15, bottom: 5 })

      Button('打开相机')
        .width('85%')
        .height(50)
        .backgroundColor('#007DFF')
        .onClick(() => this.takePhoto())

      Button('取消')
        .width('85%')
        .height(50)
        .backgroundColor('#F1F3F5')
        .fontColor('#182431')
        .onClick(() => this.controller?.close())
    }
    .width('90%')
    .padding(25)
    .backgroundColor(Color.White)
    .borderRadius(24)
  }
}