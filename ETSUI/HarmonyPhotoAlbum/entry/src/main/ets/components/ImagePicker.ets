import picker from '@ohos.file.picker';
import cameraPicker from '@ohos.multimedia.cameraPicker';
import camera from '@ohos.multimedia.camera';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import { FileService } from '../service/FileService'; // 引入刚才写的服务
import Logger from '../common/utils/Logger';

@CustomDialog
export struct ImagePicker {
  // 1. 获取当前上下文 (用于调用 FileService 和 Picker)
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;

  // 2. 必须定义的控制器
  controller: CustomDialogController;


  // 3. 回调函数：当图片保存成功后，通过这个函数把路径传出去
  onImageSelected: (savedPath: string) => void = (path: string) => {};
  /**
   * 从相册选择图片
   */
  async selectFromAlbum() {
    try {
      const photoSelectOptions = new picker.PhotoSelectOptions();
      photoSelectOptions.MIMEType = picker.PhotoViewMIMETypes.IMAGE_TYPE; // 只选图片
      photoSelectOptions.maxSelectNumber = 1; // 单选

      const photoViewPicker = new picker.PhotoViewPicker();
      const result = await photoViewPicker.select(photoSelectOptions);

      if (result.photoUris.length > 0) {
        // 拿到临时 URI
        const uri = result.photoUris[0];
        console.info(`[ImagePicker] 选中图片URI: ${uri}`);

        // 调用 FileService 保存到沙箱
        const savedPath = await FileService.saveFileToSandbox(this.context, uri);

        // 回调给父组件
        this.onImageSelected(savedPath);

        // 关闭弹窗
        this.controller.close();
      }
    } catch (err) {
      console.error(`[ImagePicker] 相册选择失败: ${JSON.stringify(err)}`);
      Logger.showError('从相册选择图片失败，请稍后重试');
    }
  }

  /**
   * 使用相机拍照
   */
  async takePhoto() {
    try {
      // 拉起系统相机采集成图片
      const pickerResult = await cameraPicker.pick(
        this.context,
        [cameraPicker.PickerMediaType.PHOTO],
        { cameraPosition: camera.CameraPosition.CAMERA_POSITION_BACK }
      );

      if (pickerResult.resultUri) {
        console.info(`[ImagePicker] 拍照结果URI: ${pickerResult.resultUri}`);

        // 同样调用 FileService 保存
        const savedPath = await FileService.saveFileToSandbox(this.context, pickerResult.resultUri);

        this.onImageSelected(savedPath);
        this.controller.close();
      }
    } catch (error) {
      let err = error as BusinessError;
      console.error(`[ImagePicker] 拍照失败 code: ${err.code}, message: ${err.message}`);
      Logger.showError('拍照失败，请检查相机权限或稍后重试');
    }
  }

  build() {
    Column({ space: 15 }) {
      Text('选择图片来源')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 10, bottom: 10 })

      // 按钮 1：从相册选择
      Button('从相册选择', { type: ButtonType.Normal, stateEffect: true })
        .width('80%')
        .height(50)
        .borderRadius(10)
        .backgroundColor('#007DFF')
        .onClick(() => {
          this.selectFromAlbum();
        })

      // 按钮 2：拍照
      Button('拍照', { type: ButtonType.Normal, stateEffect: true })
        .width('80%')
        .height(50)
        .borderRadius(10)
        .backgroundColor('#FF9800') // 橙色区分
        .onClick(() => {
          this.takePhoto();
        })

      // 按钮 3：取消
      Button('取消', { type: ButtonType.Normal, stateEffect: true })
        .width('80%')
        .height(50)
        .borderRadius(10)
        .backgroundColor('#F1F3F5')
        .fontColor('#000000')
        .margin({ bottom: 10 })
        .onClick(() => {
          this.controller.close();
        })
    }
    .width('90%')
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(20)
  }
}
