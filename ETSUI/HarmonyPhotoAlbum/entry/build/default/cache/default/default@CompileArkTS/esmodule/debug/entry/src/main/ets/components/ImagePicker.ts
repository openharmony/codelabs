if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface ImagePicker_Params {
    context?: common.UIAbilityContext;
    // 2. 必须定义的控制器
    controller?: CustomDialogController;
    // 3. 回调函数：当图片保存成功后，通过这个函数把路径传出去
    onImageSelected?: (savedPath: string) => void;
}
import picker from "@ohos:file.picker";
import cameraPicker from "@ohos:multimedia.cameraPicker";
import camera from "@ohos:multimedia.camera";
import type common from "@ohos:app.ability.common";
import type { BusinessError } from "@ohos:base";
import { FileService } from "@normalized:N&&&entry/src/main/ets/service/FileService&";
import Logger from "@normalized:N&&&entry/src/main/ets/common/utils/Logger&";
export class ImagePicker extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.context = getContext(this) as common.UIAbilityContext;
        this.controller = undefined;
        this.onImageSelected = (path: string) => { };
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: ImagePicker_Params) {
        if (params.context !== undefined) {
            this.context = params.context;
        }
        if (params.controller !== undefined) {
            this.controller = params.controller;
        }
        if (params.onImageSelected !== undefined) {
            this.onImageSelected = params.onImageSelected;
        }
    }
    updateStateVars(params: ImagePicker_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
    }
    aboutToBeDeleted() {
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    // 1. 获取当前上下文 (用于调用 FileService 和 Picker)
    private context: common.UIAbilityContext;
    // 2. 必须定义的控制器
    private controller: CustomDialogController;
    setController(ctr: CustomDialogController) {
        this.
        // 2. 必须定义的控制器
        controller = ctr;
    }
    // 3. 回调函数：当图片保存成功后，通过这个函数把路径传出去
    private onImageSelected: (savedPath: string) => void;
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
        }
        catch (err) {
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
            const pickerResult = await cameraPicker.pick(this.context, [cameraPicker.PickerMediaType.PHOTO], { cameraPosition: camera.CameraPosition.CAMERA_POSITION_BACK });
            if (pickerResult.resultUri) {
                console.info(`[ImagePicker] 拍照结果URI: ${pickerResult.resultUri}`);
                // 同样调用 FileService 保存
                const savedPath = await FileService.saveFileToSandbox(this.context, pickerResult.resultUri);
                this.onImageSelected(savedPath);
                this.controller.close();
            }
        }
        catch (error) {
            let err = error as BusinessError;
            console.error(`[ImagePicker] 拍照失败 code: ${err.code}, message: ${err.message}`);
            Logger.showError('拍照失败，请检查相机权限或稍后重试');
        }
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create({ space: 15 });
            Column.width('90%');
            Column.padding(20);
            Column.backgroundColor(Color.White);
            Column.borderRadius(20);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create('选择图片来源');
            Text.fontSize(20);
            Text.fontWeight(FontWeight.Bold);
            Text.margin({ top: 10, bottom: 10 });
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 按钮 1：从相册选择
            Button.createWithLabel('从相册选择', { type: ButtonType.Normal, stateEffect: true });
            // 按钮 1：从相册选择
            Button.width('80%');
            // 按钮 1：从相册选择
            Button.height(50);
            // 按钮 1：从相册选择
            Button.borderRadius(10);
            // 按钮 1：从相册选择
            Button.backgroundColor('#007DFF');
            // 按钮 1：从相册选择
            Button.onClick(() => {
                this.selectFromAlbum();
            });
        }, Button);
        // 按钮 1：从相册选择
        Button.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 按钮 2：拍照
            Button.createWithLabel('拍照', { type: ButtonType.Normal, stateEffect: true });
            // 按钮 2：拍照
            Button.width('80%');
            // 按钮 2：拍照
            Button.height(50);
            // 按钮 2：拍照
            Button.borderRadius(10);
            // 按钮 2：拍照
            Button.backgroundColor('#FF9800');
            // 按钮 2：拍照
            Button.onClick(() => {
                this.takePhoto();
            });
        }, Button);
        // 按钮 2：拍照
        Button.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 按钮 3：取消
            Button.createWithLabel('取消', { type: ButtonType.Normal, stateEffect: true });
            // 按钮 3：取消
            Button.width('80%');
            // 按钮 3：取消
            Button.height(50);
            // 按钮 3：取消
            Button.borderRadius(10);
            // 按钮 3：取消
            Button.backgroundColor('#F1F3F5');
            // 按钮 3：取消
            Button.fontColor('#000000');
            // 按钮 3：取消
            Button.margin({ bottom: 10 });
            // 按钮 3：取消
            Button.onClick(() => {
                this.controller.close();
            });
        }, Button);
        // 按钮 3：取消
        Button.pop();
        Column.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
}
