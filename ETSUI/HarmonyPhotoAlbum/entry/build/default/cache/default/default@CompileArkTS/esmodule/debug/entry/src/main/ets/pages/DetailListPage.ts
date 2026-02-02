if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface DetailListPage_Params {
    smallScroller?: Scroller;
    bigScroller?: Scroller;
    deviceWidth?: number;
    smallImgWidth?: number;
    imageWidth?: number;
    photoArr?: Array<ResourceStr>;
    smallPhotoArr?: Array<ResourceStr>;
    selectedIndex?: number;
}
import display from "@ohos:display";
import router from "@ohos:router";
import Constants from "@normalized:N&&&entry/src/main/ets/common/constants/Constants&";
import { LazyImage } from "@normalized:N&&&entry/src/main/ets/components/LazyImage&";
enum scrollTypeEnum {
    STOP = "onScrollStop",
    SCROLL = "onScroll"
}
;
class DetailListPage extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.smallScroller = new Scroller();
        this.bigScroller = new Scroller();
        this.__deviceWidth = new ObservedPropertySimplePU(Constants.DEFAULT_WIDTH, this, "deviceWidth");
        this.__smallImgWidth = new ObservedPropertySimplePU((this.deviceWidth - Constants.LIST_ITEM_SPACE * (Constants.SHOW_COUNT - 1)) /
            Constants.SHOW_COUNT, this, "smallImgWidth");
        this.__imageWidth = new ObservedPropertySimplePU(this.deviceWidth + this.smallImgWidth, this, "imageWidth");
        this.photoArr = (router.getParams() as Record<string, Array<ResourceStr>>)[`${Constants.PARAM_PHOTO_ARR_KEY}`];
        this.smallPhotoArr = new Array<ResourceStr>().concat(Constants.CACHE_IMG_LIST, (router.getParams() as Record<string, Array<ResourceStr>>)[`${Constants.PARAM_PHOTO_ARR_KEY}`], Constants.CACHE_IMG_LIST);
        this.__selectedIndex = this.createStorageLink('selectedIndex', 0, "selectedIndex");
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: DetailListPage_Params) {
        if (params.smallScroller !== undefined) {
            this.smallScroller = params.smallScroller;
        }
        if (params.bigScroller !== undefined) {
            this.bigScroller = params.bigScroller;
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
        if (params.photoArr !== undefined) {
            this.photoArr = params.photoArr;
        }
        if (params.smallPhotoArr !== undefined) {
            this.smallPhotoArr = params.smallPhotoArr;
        }
    }
    updateStateVars(params: DetailListPage_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__deviceWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__smallImgWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__imageWidth.purgeDependencyOnElmtId(rmElmtId);
        this.__selectedIndex.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__deviceWidth.aboutToBeDeleted();
        this.__smallImgWidth.aboutToBeDeleted();
        this.__imageWidth.aboutToBeDeleted();
        this.__selectedIndex.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    private smallScroller: Scroller;
    private bigScroller: Scroller;
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
    private photoArr: Array<ResourceStr>;
    private smallPhotoArr: Array<ResourceStr>;
    private __selectedIndex: ObservedPropertyAbstractPU<number>;
    get selectedIndex() {
        return this.__selectedIndex.get();
    }
    set selectedIndex(newValue: number) {
        this.__selectedIndex.set(newValue);
    }
    SmallImgItemBuilder(img: Resource, index?: number, parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            if (index && index > (Constants.CACHE_IMG_SIZE - 1) && index < (this.smallPhotoArr.length - Constants.CACHE_IMG_SIZE)) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        __Common__.create();
                        __Common__.onClick(() => this.smallImgClickAction(index));
                    }, __Common__);
                    {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            if (isInitialRender) {
                                let componentCall = new LazyImage(this, {
                                    src: img,
                                    widthValue: '100%',
                                    heightValue: '100%',
                                    objectFit: ImageFit.Cover,
                                    placeholder: Constants.PLACEHOLDER_IMAGE,
                                    errorHolder: Constants.ERROR_IMAGE,
                                    cornerRadius: { "id": 16777228, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" }
                                }, undefined, elmtId, () => { }, { page: "entry/src/main/ets/pages/DetailListPage.ets", line: 43, col: 7 });
                                ViewPU.create(componentCall);
                                let paramsLambda = () => {
                                    return {
                                        src: img,
                                        widthValue: '100%',
                                        heightValue: '100%',
                                        objectFit: ImageFit.Cover,
                                        placeholder: Constants.PLACEHOLDER_IMAGE,
                                        errorHolder: Constants.ERROR_IMAGE,
                                        cornerRadius: { "id": 16777228, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" }
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
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
    }
    aboutToAppear() {
        let displayClass: display.Display = display.getDefaultDisplaySync();
        let width = displayClass?.width / displayClass.densityPixels ?? Constants.DEFAULT_WIDTH;
        this.deviceWidth = width;
        this.smallImgWidth = (width - Constants.LIST_ITEM_SPACE * (Constants.SHOW_COUNT - 1)) / Constants.SHOW_COUNT;
        this.imageWidth = this.deviceWidth + this.smallImgWidth;
    }
    onPageShow() {
        this.smallScroller.scrollToIndex(this.selectedIndex);
        this.bigScroller.scrollToIndex(this.selectedIndex);
    }
    goDetailPage(): void {
        router.pushUrl({
            url: Constants.URL_DETAIL_PAGE,
            params: { photoArr: this.photoArr }
        });
    }
    smallImgClickAction(index: number): void {
        this.selectedIndex = index - Constants.CACHE_IMG_SIZE;
        this.smallScroller.scrollToIndex(this.selectedIndex);
        this.bigScroller.scrollToIndex(this.selectedIndex);
    }
    smallScrollAction(type: scrollTypeEnum): void {
        this.selectedIndex = Math.round(((this.smallScroller.currentOffset().xOffset as number) +
            this.smallImgWidth / Constants.DOUBLE_NUMBER) / (this.smallImgWidth + Constants.LIST_ITEM_SPACE));
        if (type === scrollTypeEnum.SCROLL) {
            this.bigScroller.scrollTo({ xOffset: this.selectedIndex * this.imageWidth, yOffset: 0 });
        }
        else {
            this.smallScroller.scrollTo({ xOffset: this.selectedIndex * this.smallImgWidth, yOffset: 0 });
        }
    }
    bigScrollAction(type: scrollTypeEnum): void {
        let smallWidth = this.smallImgWidth + Constants.LIST_ITEM_SPACE;
        this.selectedIndex = Math.round(((this.bigScroller.currentOffset().xOffset as number) +
            smallWidth / Constants.DOUBLE_NUMBER) / this.imageWidth);
        if (type === scrollTypeEnum.SCROLL) {
            this.smallScroller.scrollTo({ xOffset: this.selectedIndex * smallWidth, yOffset: 0 });
        }
        else {
            this.bigScroller.scrollTo({ xOffset: this.selectedIndex * this.imageWidth, yOffset: 0 });
        }
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Navigation.create(new NavPathStack(), { moduleName: "entry", pagePath: "entry/src/main/ets/pages/DetailListPage", isUserCreateStack: false });
            Navigation.title(Constants.PAGE_TITLE);
            Navigation.hideBackButton(false);
            Navigation.titleMode(NavigationTitleMode.Mini);
        }, Navigation);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Stack.create({ alignContent: Alignment.Bottom });
            Stack.width(this.imageWidth);
            Stack.height(Constants.FULL_PERCENT);
        }, Stack);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            List.create({ scroller: this.bigScroller, initialIndex: this.selectedIndex });
            List.onScroll((scrollOffset, scrollState) => {
                if (scrollState === ScrollState.Fling) {
                    this.bigScrollAction(scrollTypeEnum.SCROLL);
                }
            });
            List.onScrollStop(() => this.bigScrollAction(scrollTypeEnum.STOP));
            List.width(Constants.FULL_PERCENT);
            List.height(Constants.FULL_PERCENT);
            List.padding({ bottom: this.smallImgWidth * Constants.DOUBLE_NUMBER });
            List.listDirection(Axis.Horizontal);
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
                        ListItem.padding({
                            left: this.smallImgWidth / Constants.DOUBLE_NUMBER,
                            right: this.smallImgWidth / Constants.DOUBLE_NUMBER
                        });
                        ListItem.width(this.imageWidth);
                    };
                    const deepRenderFunction = (elmtId, isInitialRender) => {
                        itemCreation(elmtId, isInitialRender);
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            __Common__.create();
                            Gesture.create(GesturePriority.Low);
                            PinchGesture.create({ fingers: Constants.DOUBLE_NUMBER });
                            PinchGesture.onActionStart(() => this.goDetailPage());
                            PinchGesture.pop();
                            Gesture.pop();
                            __Common__.onClick(() => this.goDetailPage());
                        }, __Common__);
                        {
                            this.observeComponentCreation2((elmtId, isInitialRender) => {
                                if (isInitialRender) {
                                    let componentCall = new LazyImage(this, {
                                        src: img,
                                        widthValue: Constants.FULL_PERCENT,
                                        heightValue: Constants.FULL_PERCENT,
                                        objectFit: ImageFit.Contain,
                                        placeholder: Constants.PLACEHOLDER_IMAGE,
                                        errorHolder: Constants.ERROR_IMAGE
                                    }, undefined, elmtId, () => { }, { page: "entry/src/main/ets/pages/DetailListPage.ets", line: 109, col: 15 });
                                    ViewPU.create(componentCall);
                                    let paramsLambda = () => {
                                        return {
                                            src: img,
                                            widthValue: Constants.FULL_PERCENT,
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
            List.create({
                scroller: this.smallScroller,
                space: Constants.LIST_ITEM_SPACE,
                initialIndex: this.selectedIndex
            });
            List.listDirection(Axis.Horizontal);
            List.onScroll((scrollOffset, scrollState) => {
                if (scrollState === ScrollState.Fling) {
                    this.smallScrollAction(scrollTypeEnum.SCROLL);
                }
            });
            List.onScrollStop(() => this.smallScrollAction(scrollTypeEnum.STOP));
            List.margin({ top: { "id": 16777226, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" }, bottom: { "id": 16777226, "type": 10002, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" } });
            List.height(this.smallImgWidth);
            List.width(Constants.FULL_PERCENT);
        }, List);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            ForEach.create();
            const forEachItemGenFunction = (_item, index?: number) => {
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
                        ListItem.width(this.smallImgWidth);
                        ListItem.aspectRatio(1);
                    };
                    const deepRenderFunction = (elmtId, isInitialRender) => {
                        itemCreation(elmtId, isInitialRender);
                        this.SmallImgItemBuilder.bind(this)(img, index);
                        ListItem.pop();
                    };
                    this.observeComponentCreation2(itemCreation2, ListItem);
                    ListItem.pop();
                }
            };
            this.forEachUpdateFunction(elmtId, this.smallPhotoArr, forEachItemGenFunction, (item: Resource) => JSON.stringify(item), true, false);
        }, ForEach);
        ForEach.pop();
        List.pop();
        Stack.pop();
        Navigation.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
    static getEntryName(): string {
        return "DetailListPage";
    }
}
registerNamedRoute(() => new DetailListPage(undefined, {}), "", { bundleName: "com.example.electronicalbum", moduleName: "entry", pagePath: "pages/DetailListPage", pageFullPath: "entry/src/main/ets/pages/DetailListPage", integratedHsp: "false", moduleType: "followWithHap" });
