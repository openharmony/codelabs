if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface DetailPage_Params {
    scroller?: Scroller;
    photoArr?: Array<Resource>;
    preOffsetX?: number;
    preOffsetY?: number;
    currentScale?: number;
    deviceWidth?: number;
    smallImgWidth?: number;
    imageWidth?: number;
    selectedIndex?: number;
    isScaling?: boolean;
    imgScale?: number;
    imgOffSetX?: number;
    imgOffSetY?: number;
    bgOpacity?: number;
}
import display from "@ohos:display";
import router from "@ohos:router";
import Constants from "@normalized:N&&&entry/src/main/ets/common/constants/Constants&";
import { LazyImage } from "@normalized:N&&&entry/src/main/ets/components/LazyImage&";
class DetailPage extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.scroller = new Scroller();
        this.photoArr = (router.getParams() as Record<string, Array<Resource>>)[`${Constants.PARAM_PHOTO_ARR_KEY}`];
        this.preOffsetX = 0;
        this.preOffsetY = 0;
        this.currentScale = 1;
        this.__deviceWidth = new ObservedPropertySimplePU(Constants.DEFAULT_WIDTH, this, "deviceWidth");
        this.__smallImgWidth = new ObservedPropertySimplePU((Constants.DEFAULT_WIDTH - Constants.LIST_ITEM_SPACE * (Constants.SHOW_COUNT - 1)) /
            Constants.SHOW_COUNT, this, "smallImgWidth");
        this.__imageWidth = new ObservedPropertySimplePU(this.deviceWidth + this.smallImgWidth, this, "imageWidth");
        this.__selectedIndex = this.createStorageLink('selectedIndex', 0, "selectedIndex");
        this.__isScaling = new ObservedPropertySimplePU(true, this, "isScaling");
        this.__imgScale = new ObservedPropertySimplePU(1, this, "imgScale");
        this.__imgOffSetX = new ObservedPropertySimplePU(0, this, "imgOffSetX");
        this.__imgOffSetY = new ObservedPropertySimplePU(0, this, "imgOffSetY");
        this.__bgOpacity = new ObservedPropertySimplePU(0, this, "bgOpacity");
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: DetailPage_Params) {
        if (params.scroller !== undefined) {
            this.scroller = params.scroller;
        }
        if (params.photoArr !== undefined) {
            this.photoArr = params.photoArr;
        }
        if (params.preOffsetX !== undefined) {
            this.preOffsetX = params.preOffsetX;
        }
        if (params.preOffsetY !== undefined) {
            this.preOffsetY = params.preOffsetY;
        }
        if (params.currentScale !== undefined) {
            this.currentScale = params.currentScale;
        }
        if (params.deviceWidth !== undefined) {
            this.deviceWidth = params.deviceWidth;
        }
        if (params.smallImgWidth !== undefined) {
            this.smallImgWidth = params.smallImgWidth;
        }
        if (params.imageWidth !== undefined) {
            this.imageWidth = params.imageWidth;
        }
        if (params.isScaling !== undefined) {
            this.isScaling = params.isScaling;
        }
        if (params.imgScale !== undefined) {
            this.imgScale = params.imgScale;
        }
        if (params.imgOffSetX !== undefined) {
            this.imgOffSetX = params.imgOffSetX;
        }
        if (params.imgOffSetY !== undefined) {
            this.imgOffSetY = params.imgOffSetY;
        }
        if (params.bgOpacity !== undefined) {
            this.bgOpacity = params.bgOpacity;
        }
    }
    updateStateVars(params: DetailPage_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__deviceWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__smallImgWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__imageWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__selectedIndex.purgeDependencyOnElmtId(rmElmtId);
        this.__isScaling.purgeDependencyOnElmtId(rmElmtId);
        this.__imgScale.purgeDependencyOnElmtId(rmElmtId);
        this.__imgOffSetX.purgeDependencyOnElmtId(rmElmtId);
        this.__imgOffSetY.purgeDependencyOnElmtId(rmElmtId);
        this.__bgOpacity.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__deviceWidth.aboutToBeDeleted();
        this.__smallImgWidth.aboutToBeDeleted();
        this.__imageWidth.aboutToBeDeleted();
        this.__selectedIndex.aboutToBeDeleted();
        this.__isScaling.aboutToBeDeleted();
        this.__imgScale.aboutToBeDeleted();
        this.__imgOffSetX.aboutToBeDeleted();
        this.__imgOffSetY.aboutToBeDeleted();
        this.__bgOpacity.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    private scroller: Scroller;
    private photoArr: Array<Resource>;
    private preOffsetX: number;
    private preOffsetY: number;
    private currentScale: number;
    private __deviceWidth: ObservedPropertySimplePU<number>;
    get deviceWidth() {
        return this.__deviceWidth.get();
    }
    set deviceWidth(newValue: number) {
        this.__deviceWidth.set(newValue);
    }
    private __smallImgWidth: ObservedPropertySimplePU<number>;
    get smallImgWidth() {
        return this.__smallImgWidth.get();
    }
    set smallImgWidth(newValue: number) {
        this.__smallImgWidth.set(newValue);
    }
    private __imageWidth: ObservedPropertySimplePU<number>;
    get imageWidth() {
        return this.__imageWidth.get();
    }
    set imageWidth(newValue: number) {
        this.__imageWidth.set(newValue);
    }
    private __selectedIndex: ObservedPropertyAbstractPU<number>;
    get selectedIndex() {
        return this.__selectedIndex.get();
    }
    set selectedIndex(newValue: number) {
        this.__selectedIndex.set(newValue);
    }
    private __isScaling: ObservedPropertySimplePU<boolean>;
    get isScaling() {
        return this.__isScaling.get();
    }
    set isScaling(newValue: boolean) {
        this.__isScaling.set(newValue);
    }
    private __imgScale: ObservedPropertySimplePU<number>;
    get imgScale() {
        return this.__imgScale.get();
    }
    set imgScale(newValue: number) {
        this.__imgScale.set(newValue);
    }
    private __imgOffSetX: ObservedPropertySimplePU<number>;
    get imgOffSetX() {
        return this.__imgOffSetX.get();
    }
    set imgOffSetX(newValue: number) {
        this.__imgOffSetX.set(newValue);
    }
    private __imgOffSetY: ObservedPropertySimplePU<number>;
    get imgOffSetY() {
        return this.__imgOffSetY.get();
    }
    set imgOffSetY(newValue: number) {
        this.__imgOffSetY.set(newValue);
    }
    private __bgOpacity: ObservedPropertySimplePU<number>;
    get bgOpacity() {
        return this.__bgOpacity.get();
    }
    set bgOpacity(newValue: number) {
        this.__bgOpacity.set(newValue);
    }
    aboutToAppear() {
        let displayClass: display.Display = display.getDefaultDisplaySync();
        let width = displayClass?.width / displayClass.densityPixels ?? Constants.DEFAULT_WIDTH;
        this.deviceWidth = width;
        this.smallImgWidth = (width - Constants.LIST_ITEM_SPACE * (Constants.SHOW_COUNT - 1)) / Constants.SHOW_COUNT;
        this.imageWidth = this.deviceWidth + this.smallImgWidth;
    }
    resetImg(): void {
        this.imgScale = Constants.MIN_ZOOM_SCALE;
        this.currentScale = Constants.MIN_ZOOM_SCALE;
        this.preOffsetX = 0;
        this.preOffsetY = 0;
    }
    handlePanEnd(): void {
        const threshold = this.imageWidth * Constants.SWIPE_THRESHOLD_RATIO;
        if (Math.abs(this.imgOffSetX) > threshold) {
            if (this.imgOffSetX > 0 && this.selectedIndex > 0) {
                this.selectedIndex -= 1;
            }
            else if (this.imgOffSetX < 0 && this.selectedIndex < (this.photoArr.length - 1)) {
                this.selectedIndex += 1;
            }
            this.isScaling = false;
            this.resetImg();
            this.scroller.scrollTo({ xOffset: this.selectedIndex * this.imageWidth, yOffset: 0 });
        }
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Stack.create();
            Stack.offset({ x: -(this.smallImgWidth / Constants.DOUBLE_NUMBER) });
            Stack.width(this.imageWidth);
            Stack.height(Constants.FULL_PERCENT);
            Stack.backgroundColor({ "id": 16777224, "type": 10001, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" });
        }, Stack);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            List.create({ scroller: this.scroller, initialIndex: this.selectedIndex });
            List.onScrollStop(() => {
                let currentIndex = Math.round(((this.scroller.currentOffset().xOffset as number) +
                    (this.imageWidth / Constants.DOUBLE_NUMBER)) / this.imageWidth);
                this.selectedIndex = currentIndex;
                this.scroller.scrollTo({ xOffset: currentIndex * this.imageWidth, yOffset: 0 });
            });
            List.width(Constants.FULL_PERCENT);
            List.height(Constants.FULL_PERCENT);
            List.listDirection(Axis.Horizontal);
            List.visibility(this.isScaling ? Visibility.Hidden : Visibility.Visible);
        }, List);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            ForEach.create();
            const forEachItemGenFunction = _item => {
                const img = _item;
                {
                    const itemCreation = (elmtId, isInitialRender) => {
                        ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
                        ListItem.create(deepRenderFunction, true);
                        if (!isInitialRender) {
                            ListItem.pop();
                        }
                        ViewStackProcessor.StopGetAccessRecording();
                    };
                    const itemCreation2 = (elmtId, isInitialRender) => {
                        ListItem.create(deepRenderFunction, true);
                        Gesture.create(GesturePriority.Low);
                        GestureGroup.create(GestureMode.Exclusive);
                        PinchGesture.create({ fingers: Constants.DOUBLE_NUMBER });
                        PinchGesture.onActionStart(() => {
                            this.resetImg();
                            this.isScaling = true;
                            this.imgOffSetX = 0;
                            this.imgOffSetY = 0;
                        });
                        PinchGesture.onActionUpdate((event?: GestureEvent) => {
                            if (event) {
                                this.imgScale = this.currentScale * event.scale;
                                // 限制缩放范围
                                if (this.imgScale < Constants.MIN_ZOOM_SCALE) {
                                    this.imgScale = Constants.MIN_ZOOM_SCALE;
                                }
                                else if (this.imgScale > Constants.MAX_ZOOM_SCALE) {
                                    this.imgScale = Constants.MAX_ZOOM_SCALE;
                                }
                            }
                        });
                        PinchGesture.onActionEnd(() => {
                            if (this.imgScale < Constants.MIN_ZOOM_SCALE) {
                                this.resetImg();
                                this.imgOffSetX = 0;
                                this.imgOffSetY = 0;
                            }
                            else if (this.imgScale > Constants.MAX_ZOOM_SCALE) {
                                this.imgScale = Constants.MAX_ZOOM_SCALE;
                                this.currentScale = this.imgScale;
                            }
                            else {
                                this.currentScale = this.imgScale;
                            }
                        });
                        PinchGesture.pop();
                        PanGesture.create();
                        PanGesture.onActionStart(() => {
                            this.resetImg();
                            this.isScaling = true;
                        });
                        PanGesture.onActionUpdate((event?: GestureEvent) => {
                            if (event) {
                                this.imgOffSetX = this.preOffsetX + event.offsetX;
                                this.imgOffSetY = this.preOffsetY + event.offsetY;
                            }
                        });
                        PanGesture.pop();
                        GestureGroup.pop();
                        Gesture.pop();
                        ListItem.padding({
                            left: this.smallImgWidth / Constants.DOUBLE_NUMBER,
                            right: this.smallImgWidth / Constants.DOUBLE_NUMBER
                        });
                        ListItem.width(this.imageWidth);
                    };
                    const deepRenderFunction = (elmtId, isInitialRender) => {
                        itemCreation(elmtId, isInitialRender);
                        {
                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                if (isInitialRender) {
                                    let componentCall = new LazyImage(this, {
                                        src: img,
                                        widthValue: this.imageWidth,
                                        heightValue: Constants.FULL_PERCENT,
                                        objectFit: ImageFit.Contain,
                                        placeholder: Constants.PLACEHOLDER_IMAGE,
                                        errorHolder: Constants.ERROR_IMAGE
                                    }, undefined, elmtId, () => { }, { page: "entry/src/main/ets/pages/DetailPage.ets", line: 73, col: 13 });
                                    ViewPU.create(componentCall);
                                    let paramsLambda = () => {
                                        return {
                                            src: img,
                                            widthValue: this.imageWidth,
                                            heightValue: Constants.FULL_PERCENT,
                                            objectFit: ImageFit.Contain,
                                            placeholder: Constants.PLACEHOLDER_IMAGE,
                                            errorHolder: Constants.ERROR_IMAGE
                                        };
                                    };
                                    componentCall.paramsGenerator_ = paramsLambda;
                                }
                                else {
                                    this.updateStateVarsOfChildByElmtId(elmtId, {});
                                }
                            }, { name: "LazyImage" });
                        }
                        ListItem.pop();
                    };
                    this.observeComponentCreation2(itemCreation2, ListItem);
                    ListItem.pop();
                }
            };
            this.forEachUpdateFunction(elmtId, this.photoArr, forEachItemGenFunction, (item: Resource) => JSON.stringify(item), false, false);
        }, ForEach);
        ForEach.pop();
        List.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Gesture.create(GesturePriority.Low);
            GestureGroup.create(GestureMode.Exclusive);
            PinchGesture.create({ fingers: Constants.DOUBLE_NUMBER });
            PinchGesture.onActionUpdate((event?: GestureEvent) => {
                if (event) {
                    this.imgScale = this.currentScale * event.scale;
                }
            });
            PinchGesture.onActionEnd(() => {
                if (this.imgScale < 1) {
                    this.resetImg();
                    this.imgOffSetX = 0;
                    this.imgOffSetY = 0;
                }
                else {
                    this.currentScale = this.imgScale;
                }
            });
            PinchGesture.pop();
            PanGesture.create();
            PanGesture.onActionStart(() => {
                this.preOffsetX = this.imgOffSetX;
                this.preOffsetY = this.imgOffSetY;
            });
            PanGesture.onActionUpdate((event?: GestureEvent) => {
                if (event) {
                    this.imgOffSetX = this.preOffsetX + event.offsetX;
                    this.imgOffSetY = this.preOffsetY + event.offsetY;
                }
            });
            PanGesture.onActionEnd(() => this.handlePanEnd());
            PanGesture.pop();
            GestureGroup.pop();
            Gesture.pop();
            Row.padding({
                left: this.smallImgWidth / Constants.DOUBLE_NUMBER,
                right: this.smallImgWidth / Constants.DOUBLE_NUMBER
            });
            Row.width(this.imageWidth);
            Row.height(Constants.FULL_PERCENT);
            Row.visibility(this.isScaling ? Visibility.Visible : Visibility.Hidden);
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            __Common__.create();
            __Common__.position({ x: this.imgOffSetX, y: this.imgOffSetY });
            __Common__.scale({ x: this.imgScale, y: this.imgScale });
            __Common__.onClick(() => router.back());
        }, __Common__);
        {
            this.observeComponentCreation2((elmtId, isInitialRender) => {
                if (isInitialRender) {
                    let componentCall = new LazyImage(this, {
                        src: this.photoArr[this.selectedIndex],
                        widthValue: this.imageWidth,
                        heightValue: Constants.FULL_PERCENT,
                        objectFit: ImageFit.Contain,
                        placeholder: Constants.PLACEHOLDER_IMAGE,
                        errorHolder: Constants.ERROR_IMAGE
                    }, undefined, elmtId, () => { }, { page: "entry/src/main/ets/pages/DetailPage.ets", line: 143, col: 9 });
                    ViewPU.create(componentCall);
                    let paramsLambda = () => {
                        return {
                            src: this.photoArr[this.selectedIndex],
                            widthValue: this.imageWidth,
                            heightValue: Constants.FULL_PERCENT,
                            objectFit: ImageFit.Contain,
                            placeholder: Constants.PLACEHOLDER_IMAGE,
                            errorHolder: Constants.ERROR_IMAGE
                        };
                    };
                    componentCall.paramsGenerator_ = paramsLambda;
                }
                else {
                    this.updateStateVarsOfChildByElmtId(elmtId, {});
                }
            }, { name: "LazyImage" });
        }
        __Common__.pop();
        Row.pop();
        Stack.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
    static getEntryName(): string {
        return "DetailPage";
    }
}
registerNamedRoute(() => new DetailPage(undefined, {}), "", { bundleName: "com.example.electronicalbum", moduleName: "entry", pagePath: "pages/DetailPage", pageFullPath: "entry/src/main/ets/pages/DetailPage", integratedHsp: "false", moduleType: "followWithHap" });
