if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface IndexPage_Params {
    swiperController?: SwiperController;
    currentIndex?: number;
    tempSelectedPath?: string;
    dbPhotoList?: Array<PhotoModel>;
    selectedIndex?: number;
    currentTabIndex?: number;
    infoDialogController?: CustomDialogController;
    pickerDialogController?: CustomDialogController;
}
interface PhotoInfoDialog_Params {
    controller?: CustomDialogController;
    imagePath?: string;
    category?: string;
    tags?: string;
    fileName?: string;
    onSave?: (data: PhotoData) => void;
}
import router from "@ohos:router";
import type common from "@ohos:app.ability.common";
import Constants from "@normalized:N&&&entry/src/main/ets/common/constants/Constants&";
import { ImagePicker } from "@normalized:N&&&entry/src/main/ets/components/ImagePicker&";
import { PhotoModel } from "@normalized:N&&&entry/src/main/ets/model/PhotoModel&";
import PhotoService from "@normalized:N&&&entry/src/main/ets/service/PhotoService&";
import Logger from "@normalized:N&&&entry/src/main/ets/common/utils/Logger&";
// 定义数据接口
interface PhotoData {
    filePath: string;
    fileName: string;
    category: string;
    tags: string;
    createTime: number;
}
class PhotoInfoDialog extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.controller = undefined;
        this.imagePath = '';
        this.__category = new ObservedPropertySimplePU('生活', this, "category");
        this.__tags = new ObservedPropertySimplePU('', this, "tags");
        this.__fileName = new ObservedPropertySimplePU('', this, "fileName");
        this.onSave = (data: PhotoData) => { };
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: PhotoInfoDialog_Params) {
        if (params.controller !== undefined) {
            this.controller = params.controller;
        }
        if (params.imagePath !== undefined) {
            this.imagePath = params.imagePath;
        }
        if (params.category !== undefined) {
            this.category = params.category;
        }
        if (params.tags !== undefined) {
            this.tags = params.tags;
        }
        if (params.fileName !== undefined) {
            this.fileName = params.fileName;
        }
        if (params.onSave !== undefined) {
            this.onSave = params.onSave;
        }
    }
    updateStateVars(params: PhotoInfoDialog_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__category.purgeDependencyOnElmtId(rmElmtId);
        this.__tags.purgeDependencyOnElmtId(rmElmtId);
        this.__fileName.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__category.aboutToBeDeleted();
        this.__tags.aboutToBeDeleted();
        this.__fileName.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    private controller: CustomDialogController;
    setController(ctr: CustomDialogController) {
        this.controller = ctr;
    }
    private imagePath: string;
    private __category: ObservedPropertySimplePU<string>;
    get category() {
        return this.__category.get();
    }
    set category(newValue: string) {
        this.__category.set(newValue);
    }
    private __tags: ObservedPropertySimplePU<string>;
    get tags() {
        return this.__tags.get();
    }
    set tags(newValue: string) {
        this.__tags.set(newValue);
    }
    private __fileName: ObservedPropertySimplePU<string>;
    get fileName() {
        return this.__fileName.get();
    }
    set fileName(newValue: string) {
        this.__fileName.set(newValue);
    }
    private onSave: (data: PhotoData) => void;
    aboutToAppear() {
        this.fileName = 'IMG_' + new Date().getTime();
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create({ space: 15 });
            Column.padding(20);
            Column.backgroundColor(Color.White);
            Column.borderRadius(16);
            Column.width('90%');
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create('完善图片信息');
            Text.fontSize(20);
            Text.fontWeight(FontWeight.Bold);
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Image.create('file://' + this.imagePath);
            Image.height(150);
            Image.width('100%');
            Image.objectFit(ImageFit.Contain);
            Image.borderRadius(8);
            Image.backgroundColor('#f0f0f0');
        }, Image);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            TextInput.create({ placeholder: '图片名称', text: this.fileName });
            TextInput.onChange((value) => { this.fileName = value; });
        }, TextInput);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.width('100%');
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create('分类:');
            Text.fontSize(16);
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            TextInput.create({ placeholder: '例如: 风景', text: this.category });
            TextInput.layoutWeight(1);
            TextInput.onChange((value) => { this.category = value; });
        }, TextInput);
        Row.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.width('100%');
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create('标签:');
            Text.fontSize(16);
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            TextInput.create({ placeholder: '例如: #旅行', text: this.tags });
            TextInput.layoutWeight(1);
            TextInput.onChange((value) => { this.tags = value; });
        }, TextInput);
        Row.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create({ space: 20 });
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Button.createWithLabel('取消');
            Button.backgroundColor('#E5E5E5');
            Button.fontColor('#000');
            Button.onClick(() => { this.controller.close(); });
        }, Button);
        Button.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Button.createWithLabel('保存');
            Button.onClick(() => {
                const finalData: PhotoData = {
                    filePath: this.imagePath,
                    fileName: this.fileName,
                    category: this.category,
                    tags: this.tags,
                    createTime: new Date().getTime()
                };
                this.onSave(finalData);
                this.controller.close();
            });
        }, Button);
        Button.pop();
        Row.pop();
        Column.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
}
class IndexPage extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.swiperController = new SwiperController();
        this.__currentIndex = new ObservedPropertySimplePU(0, this, "currentIndex");
        this.__tempSelectedPath = new ObservedPropertySimplePU('', this, "tempSelectedPath");
        this.__dbPhotoList = new ObservedPropertyObjectPU([], this, "dbPhotoList");
        this.__selectedIndex = this.createStorageLink('selectedIndex', 0, "selectedIndex");
        this.__currentTabIndex = new ObservedPropertySimplePU(0, this, "currentTabIndex");
        this.infoDialogController = new CustomDialogController({
            builder: () => {
                let jsDialog = new PhotoInfoDialog(this, {
                    imagePath: this.tempSelectedPath,
                    onSave: (data: PhotoData) => {
                        this.handleSaveData(data);
                    }
                }, undefined, -1, () => { }, { page: "entry/src/main/ets/pages/IndexPage.ets", line: 131, col: 14 });
                jsDialog.setController(this.infoDialogController);
                ViewPU.create(jsDialog);
                let paramsLambda = () => {
                    return {
                        imagePath: this.tempSelectedPath,
                        onSave: (data: PhotoData) => {
                            this.handleSaveData(data);
                        }
                    };
                };
                jsDialog.paramsGenerator_ = paramsLambda;
            },
            autoCancel: false,
            customStyle: true
        }, this);
        this.pickerDialogController = new CustomDialogController({
            builder: () => {
                let jsDialog = new ImagePicker(this, {
                    onImageSelected: (path: string) => {
                        this.tempSelectedPath = path;
                        setTimeout(() => {
                            this.infoDialogController.open();
                        }, 100);
                    }
                }, undefined, -1, () => { }, { page: "entry/src/main/ets/pages/IndexPage.ets", line: 141, col: 14 });
                jsDialog.setController(this.pickerDialogController);
                ViewPU.create(jsDialog);
                let paramsLambda = () => {
                    return {
                        onImageSelected: (path: string) => {
                            this.tempSelectedPath = path;
                            setTimeout(() => {
                                this.infoDialogController.open();
                            }, 100);
                        }
                    };
                };
                jsDialog.paramsGenerator_ = paramsLambda;
            },
            alignment: DialogAlignment.Bottom,
            customStyle: true
        }, this);
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: IndexPage_Params) {
        if (params.swiperController !== undefined) {
            this.swiperController = params.swiperController;
        }
        if (params.currentIndex !== undefined) {
            this.currentIndex = params.currentIndex;
        }
        if (params.tempSelectedPath !== undefined) {
            this.tempSelectedPath = params.tempSelectedPath;
        }
        if (params.dbPhotoList !== undefined) {
            this.dbPhotoList = params.dbPhotoList;
        }
        if (params.currentTabIndex !== undefined) {
            this.currentTabIndex = params.currentTabIndex;
        }
        if (params.infoDialogController !== undefined) {
            this.infoDialogController = params.infoDialogController;
        }
        if (params.pickerDialogController !== undefined) {
            this.pickerDialogController = params.pickerDialogController;
        }
    }
    updateStateVars(params: IndexPage_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__currentIndex.purgeDependencyOnElmtId(rmElmtId);
        this.__tempSelectedPath.purgeDependencyOnElmtId(rmElmtId);
        this.__dbPhotoList.purgeDependencyOnElmtId(rmElmtId);
        this.__selectedIndex.purgeDependencyOnElmtId(rmElmtId);
        this.__currentTabIndex.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__currentIndex.aboutToBeDeleted();
        this.__tempSelectedPath.aboutToBeDeleted();
        this.__dbPhotoList.aboutToBeDeleted();
        this.__selectedIndex.aboutToBeDeleted();
        this.__currentTabIndex.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    private swiperController: SwiperController;
    private __currentIndex: ObservedPropertySimplePU<number>;
    get currentIndex() {
        return this.__currentIndex.get();
    }
    set currentIndex(newValue: number) {
        this.__currentIndex.set(newValue);
    }
    private __tempSelectedPath: ObservedPropertySimplePU<string>;
    get tempSelectedPath() {
        return this.__tempSelectedPath.get();
    }
    set tempSelectedPath(newValue: string) {
        this.__tempSelectedPath.set(newValue);
    }
    private __dbPhotoList: ObservedPropertyObjectPU<Array<PhotoModel>>;
    get dbPhotoList() {
        return this.__dbPhotoList.get();
    }
    set dbPhotoList(newValue: Array<PhotoModel>) {
        this.__dbPhotoList.set(newValue);
    }
    private __selectedIndex: ObservedPropertyAbstractPU<number>;
    get selectedIndex() {
        return this.__selectedIndex.get();
    }
    set selectedIndex(newValue: number) {
        this.__selectedIndex.set(newValue);
    }
    // ==========================================
    // 🆕 Tab导航相关状态
    // ==========================================
    private __currentTabIndex: ObservedPropertySimplePU<number>;
    get currentTabIndex() {
        return this.__currentTabIndex.get();
    }
    set currentTabIndex(newValue: number) {
        this.__currentTabIndex.set(newValue);
    }
    aboutToAppear() {
        console.info('[IndexPage] 页面加载,正在初始化数据库...');
        PhotoService.initDB(getContext(this) as common.UIAbilityContext)
            .then(() => {
            console.info('[IndexPage] 数据库初始化完成,开始加载数据');
            this.refreshData();
        })
            .catch((err: Error) => {
            console.error('[IndexPage] 数据库启动失败: ' + JSON.stringify(err));
            Logger.showError('相册初始化失败，请稍后重试');
        });
    }
    refreshData() {
        PhotoService.queryAll().then((photos) => {
            this.dbPhotoList = photos;
            console.info(`[IndexPage] 列表已刷新,当前共有 ${this.dbPhotoList.length} 张照片`);
        });
    }
    private infoDialogController: CustomDialogController;
    private pickerDialogController: CustomDialogController;
    handleSaveData(data: PhotoData) {
        const newPhoto = new PhotoModel(0, data.fileName, data.filePath, data.category, data.createTime, data.tags);
        PhotoService.insert(newPhoto).then((rowId) => {
            console.info(`[IndexPage] 保存成功,新记录 ID: ${rowId}`);
            this.refreshData();
            AlertDialog.show({
                title: '保存成功',
                message: '图片已安全存入数据库!',
                confirm: {
                    value: '棒', action: () => {
                    }
                }
            });
        }).catch((err: Error) => {
            console.error('[IndexPage] 保存失败: ' + JSON.stringify(err));
            Logger.showError('保存失败，数据库写入错误');
        });
    }
    // ==========================================
    // 🆕 首页内容构建器
    // ==========================================
    HomeContent(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Stack.create({ alignContent: Alignment.BottomEnd });
            Stack.width(Constants.FULL_PERCENT);
            Stack.height(Constants.FULL_PERCENT);
        }, Stack);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width(Constants.FULL_PERCENT);
            Column.height(Constants.FULL_PERCENT);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 1. 标题栏
            Row.create();
            // 1. 标题栏
            Row.height({ "id": 16777229, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
            // 1. 标题栏
            Row.width(Constants.FULL_PERCENT);
            // 1. 标题栏
            Row.padding({ left: { "id": 16777231, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" } });
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create({ "id": 16777219, "type": 10003, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
            Text.fontSize({ "id": 16777230, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
            Text.fontWeight(Constants.TITLE_FONT_WEIGHT);
        }, Text);
        Text.pop();
        // 1. 标题栏
        Row.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 2. 轮播图
            Swiper.create(this.swiperController);
            // 2. 轮播图
            Swiper.autoPlay(true);
            // 2. 轮播图
            Swiper.loop(true);
            // 2. 轮播图
            Swiper.margin({ "id": 16777227, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
            // 2. 轮播图
            Swiper.borderRadius({ "id": 16777228, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
            // 2. 轮播图
            Swiper.clip(true);
        }, Swiper);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            ForEach.create();
            const forEachItemGenFunction = _item => {
                const item = _item;
                this.observeComponentCreation2((elmtId, isInitialRender) => {
                    Row.create();
                    Row.width(Constants.FULL_PERCENT);
                    Row.aspectRatio(Constants.BANNER_ASPECT_RATIO);
                }, Row);
                this.observeComponentCreation2((elmtId, isInitialRender) => {
                    Image.create(item);
                    Image.width(Constants.FULL_PERCENT);
                    Image.height(Constants.FULL_PERCENT);
                }, Image);
                Row.pop();
            };
            this.forEachUpdateFunction(elmtId, Constants.BANNER_IMG_LIST, forEachItemGenFunction, (item: Resource, index?: number) => JSON.stringify(item) + index, false, true);
        }, ForEach);
        ForEach.pop();
        // 2. 轮播图
        Swiper.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            // 3. 图片网格
            if (this.dbPhotoList.length === 0) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Column.create();
                        Column.layoutWeight(1);
                        Column.width('100%');
                        Column.justifyContent(FlexAlign.Center);
                    }, Column);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create('📷');
                        Text.fontSize(60);
                        Text.opacity(0.3);
                        Text.margin({ top: 50 });
                    }, Text);
                    Text.pop();
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create('相册空空如也');
                        Text.fontSize(16);
                        Text.fontColor(Color.Gray);
                        Text.margin({ top: 20 });
                    }, Text);
                    Text.pop();
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create('点击右下角 + 号添加照片');
                        Text.fontSize(14);
                        Text.fontColor('#CCCCCC');
                        Text.margin({ top: 10 });
                    }, Text);
                    Text.pop();
                    Column.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Grid.create();
                        Grid.columnsTemplate(Constants.INDEX_COLUMNS_TEMPLATE);
                        Grid.columnsGap({ "id": 16777227, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
                        Grid.rowsGap({ "id": 16777227, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
                        Grid.padding({ "id": 16777227, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
                        Grid.width(Constants.FULL_PERCENT);
                        Grid.layoutWeight(1);
                    }, Grid);
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        ForEach.create();
                        const forEachItemGenFunction = (_item, index: number) => {
                            const photo = _item;
                            {
                                const itemCreation2 = (elmtId, isInitialRender) => {
                                    GridItem.create(() => { }, false);
                                    GridItem.width(Constants.FULL_PERCENT);
                                    GridItem.aspectRatio(1);
                                };
                                const observedDeepRender = () => {
                                    this.observeComponentCreation2(itemCreation2, GridItem);
                                    this.PhotoItem.bind(this)(photo, index);
                                    GridItem.pop();
                                };
                                observedDeepRender();
                            }
                        };
                        this.forEachUpdateFunction(elmtId, this.dbPhotoList, forEachItemGenFunction, (photo: PhotoModel) => photo.id.toString(), true, false);
                    }, ForEach);
                    ForEach.pop();
                    Grid.pop();
                });
            }
        }, If);
        If.pop();
        Column.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 4. 悬浮添加按钮
            Button.createWithChild({ type: ButtonType.Circle, stateEffect: true });
            // 4. 悬浮添加按钮
            Button.width(60);
            // 4. 悬浮添加按钮
            Button.height(60);
            // 4. 悬浮添加按钮
            Button.margin({ right: 20, bottom: 30 });
            // 4. 悬浮添加按钮
            Button.backgroundColor('#007DFF');
            // 4. 悬浮添加按钮
            Button.shadow({ radius: 10, color: '#888888', offsetY: 5 });
            // 4. 悬浮添加按钮
            Button.onClick(() => {
                this.pickerDialogController.open();
            });
        }, Button);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create('+');
            Text.fontSize(40);
            Text.fontColor(Color.White);
            Text.fontWeight(300);
            Text.margin({ bottom: 4 });
        }, Text);
        Text.pop();
        // 4. 悬浮添加按钮
        Button.pop();
        Stack.pop();
    }
    PhotoItem(photo: PhotoModel, index: number, parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Stack.create({ alignContent: Alignment.BottomStart });
            Stack.width('100%');
            Stack.height('100%');
            Stack.onClick(() => {
                this.navigateToDetail(index);
            });
        }, Stack);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Image.create('file://' + photo.path);
            Image.width('100%');
            Image.height('100%');
            Image.objectFit(ImageFit.Cover);
            Image.alt({ "id": 16777217, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
        }, Image);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width('100%');
            Column.padding(8);
            Column.linearGradient({
                direction: GradientDirection.Bottom,
                colors: [[0x00000000, 0.0], [0xAA000000, 1.0]]
            });
            Column.alignItems(HorizontalAlign.Start);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(photo.name);
            Text.fontSize(12);
            Text.fontColor(Color.White);
            Text.maxLines(1);
            Text.textOverflow({ overflow: TextOverflow.Ellipsis });
            Text.width('100%');
            Text.textAlign(TextAlign.Start);
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (photo.category) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create(photo.category);
                        Text.fontSize(10);
                        Text.fontColor(Color.White);
                        Text.padding({
                            left: 6,
                            right: 6,
                            top: 2,
                            bottom: 2
                        });
                        Text.backgroundColor('rgba(0, 0, 0, 0.5)');
                        Text.borderRadius(4);
                        Text.margin({ top: 4 });
                    }, Text);
                    Text.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Column.pop();
        Stack.pop();
    }
    // ==========================================
    // 🆕 页面显示时的生命周期（从ListPage返回时触发）
    // ==========================================
    onPageShow() {
        console.info('[IndexPage] 页面显示，刷新数据');
        this.currentTabIndex = 0; // 确保回到首页Tab
        this.refreshData(); // 刷新图片列表
    }
    // ==========================================
    // 🆕 主构建函数 - 使用Tabs组件
    // ==========================================
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Tabs.create({ index: this.currentTabIndex });
            Tabs.barPosition(BarPosition.End);
            Tabs.scrollable(false);
            Tabs.animationDuration(300);
            Tabs.onChange((index: number) => {
                console.info(`[IndexPage] 切换到 Tab ${index}`);
                this.currentTabIndex = index;
                // ==========================================
                // 🎯 关键:点击"搜索"Tab时跳转到ListPage
                // ==========================================
                if (index === 1) {
                    router.pushUrl({
                        url: 'pages/ListPage'
                    }).then(() => {
                        console.info('[IndexPage] 成功跳转到搜索页');
                        // 跳转后立即切回首页Tab,避免UI错乱
                        this.currentTabIndex = 0;
                    }).catch((err: Error) => {
                        console.error(`[IndexPage] 跳转失败: ${err.message}`);
                        this.currentTabIndex = 0;
                    });
                }
            });
        }, Tabs);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            TabContent.create(() => {
                this.HomeContent.bind(this)();
            });
            TabContent.tabBar({ builder: () => {
                    this.TabBuilder.call(this, 0, '首页', '🏠');
                } });
        }, TabContent);
        TabContent.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            TabContent.create(() => {
                this.observeComponentCreation2((elmtId, isInitialRender) => {
                    Column.create();
                    Column.width('100%');
                    Column.height('100%');
                    Column.justifyContent(FlexAlign.Center);
                }, Column);
                this.observeComponentCreation2((elmtId, isInitialRender) => {
                    Text.create('正在跳转到搜索页...');
                    Text.fontSize(16);
                    Text.fontColor('#999999');
                }, Text);
                Text.pop();
                Column.pop();
            });
            TabContent.tabBar({ builder: () => {
                    this.TabBuilder.call(this, 1, '搜索', '🔍');
                } });
        }, TabContent);
        TabContent.pop();
        Tabs.pop();
    }
    // ==========================================
    // 🆕 Tab图标构建器
    // ==========================================
    TabBuilder(index: number, title: string, icon: string, parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width('100%');
            Column.height(50);
            Column.justifyContent(FlexAlign.Center);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(icon);
            Text.fontSize(24);
            Text.fontColor(this.currentTabIndex === index ? '#007DFF' : '#666666');
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(title);
            Text.fontSize(12);
            Text.fontColor(this.currentTabIndex === index ? '#007DFF' : '#666666');
            Text.margin({ top: 4 });
        }, Text);
        Text.pop();
        Column.pop();
    }
    private navigateToDetail(index: number) {
        this.selectedIndex = index;
        const photoPaths = this.dbPhotoList.map(photo => 'file://' + photo.path); // 🔧 添加 file:// 前缀
        console.info(`[ListPage] 跳转到详情页，索引: ${index}`);
        router.pushUrl({
            url: Constants.URL_DETAIL_LIST_PAGE,
            params: {
                photoArr: photoPaths,
            }
        }).catch((err: Error) => {
            console.error(`${Constants.TAG_LIST_PAGE}${err.message}`);
        });
    }
    rerender() {
        this.updateDirtyElements();
    }
    static getEntryName(): string {
        return "IndexPage";
    }
}
registerNamedRoute(() => new IndexPage(undefined, {}), "", { bundleName: "com.example.electronicalbum", moduleName: "entry", pagePath: "pages/IndexPage", pageFullPath: "entry/src/main/ets/pages/IndexPage", integratedHsp: "false", moduleType: "followWithHap" });
