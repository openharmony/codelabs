if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface LazyImage_Params {
    // 对外属性
    src?: ImageSourceType;
    widthValue?: Length | LayoutPolicy;
    heightValue?: Length | LayoutPolicy;
    objectFit?: ImageFit;
    cornerRadius?: number | Resource | Length;
    placeholder?: Resource;
    errorHolder?: Resource;
    enableCache?: boolean;
    realSrc?: ImageSourceType | null;
    isLoading?: boolean;
    hasError?: boolean;
    isAppeared?: boolean;
}
import imageCache from "@normalized:N&&&entry/src/main/ets/common/utils/ImageCache&";
import type { ImageSourceType } from "@normalized:N&&&entry/src/main/ets/common/utils/ImageCache&";
import Logger from "@normalized:N&&&entry/src/main/ets/common/utils/Logger&";
export class LazyImage extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.src = '';
        this.widthValue = '100%';
        this.heightValue = '100%';
        this.objectFit = ImageFit.Cover;
        this.cornerRadius = 0;
        this.placeholder = undefined;
        this.errorHolder = undefined;
        this.enableCache = true;
        this.__realSrc = new ObservedPropertyObjectPU(null, this, "realSrc");
        this.__isLoading = new ObservedPropertySimplePU(false, this, "isLoading");
        this.__hasError = new ObservedPropertySimplePU(false, this, "hasError");
        this.__isAppeared = new ObservedPropertySimplePU(false, this, "isAppeared");
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: LazyImage_Params) {
        if (params.src !== undefined) {
            this.src = params.src;
        }
        if (params.widthValue !== undefined) {
            this.widthValue = params.widthValue;
        }
        if (params.heightValue !== undefined) {
            this.heightValue = params.heightValue;
        }
        if (params.objectFit !== undefined) {
            this.objectFit = params.objectFit;
        }
        if (params.cornerRadius !== undefined) {
            this.cornerRadius = params.cornerRadius;
        }
        if (params.placeholder !== undefined) {
            this.placeholder = params.placeholder;
        }
        if (params.errorHolder !== undefined) {
            this.errorHolder = params.errorHolder;
        }
        if (params.enableCache !== undefined) {
            this.enableCache = params.enableCache;
        }
        if (params.realSrc !== undefined) {
            this.realSrc = params.realSrc;
        }
        if (params.isLoading !== undefined) {
            this.isLoading = params.isLoading;
        }
        if (params.hasError !== undefined) {
            this.hasError = params.hasError;
        }
        if (params.isAppeared !== undefined) {
            this.isAppeared = params.isAppeared;
        }
    }
    updateStateVars(params: LazyImage_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__realSrc.purgeDependencyOnElmtId(rmElmtId);
        this.__isLoading.purgeDependencyOnElmtId(rmElmtId);
        this.__hasError.purgeDependencyOnElmtId(rmElmtId);
        this.__isAppeared.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__realSrc.aboutToBeDeleted();
        this.__isLoading.aboutToBeDeleted();
        this.__hasError.aboutToBeDeleted();
        this.__isAppeared.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    // 对外属性
    private src: ImageSourceType;
    private widthValue: Length | LayoutPolicy;
    private heightValue: Length | LayoutPolicy;
    private objectFit: ImageFit;
    private cornerRadius: number | Resource | Length;
    private placeholder?: Resource; // 加载中占位图
    private errorHolder?: Resource; // 加载失败占位图
    private enableCache: boolean;
    // 基类要求的函数实现
    width(value: Length): LazyImage {
        this.widthValue = value;
        return this;
    }
    height(value: Length): LazyImage {
        this.heightValue = value;
        return this;
    }
    // 内部状态
    private __realSrc: ObservedPropertyObjectPU<ImageSourceType | null>;
    get realSrc() {
        return this.__realSrc.get();
    }
    set realSrc(newValue: ImageSourceType | null) {
        this.__realSrc.set(newValue);
    }
    private __isLoading: ObservedPropertySimplePU<boolean>;
    get isLoading() {
        return this.__isLoading.get();
    }
    set isLoading(newValue: boolean) {
        this.__isLoading.set(newValue);
    }
    private __hasError: ObservedPropertySimplePU<boolean>;
    get hasError() {
        return this.__hasError.get();
    }
    set hasError(newValue: boolean) {
        this.__hasError.set(newValue);
    }
    private __isAppeared: ObservedPropertySimplePU<boolean>;
    get isAppeared() {
        return this.__isAppeared.get();
    }
    set isAppeared(newValue: boolean) {
        this.__isAppeared.set(newValue);
    }
    aboutToAppear() {
        this.isAppeared = true;
        this.tryLoad();
    }
    aboutToDisappear() {
        this.isAppeared = false;
    }
    private tryLoad() {
        if (!this.isAppeared || this.isLoading || this.realSrc) {
            return;
        }
        this.isLoading = true;
        this.hasError = false;
        // 简化：本地 file:// 或资源图，本质上不需要异步解码，这里只做缓存封装
        try {
            if (this.enableCache && imageCache.has(this.src)) {
                const cached = imageCache.get(this.src);
                if (cached) {
                    this.realSrc = cached;
                    this.isLoading = false;
                    return;
                }
            }
            // 未命中缓存，直接使用原 src，并写入缓存
            this.realSrc = this.src;
            if (this.enableCache) {
                imageCache.set(this.src);
            }
            this.isLoading = false;
        }
        catch (err) {
            Logger.error('[LazyImage] load error', JSON.stringify(err));
            this.hasError = true;
            this.isLoading = false;
        }
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            // 加载失败 -> 错误占位
            if (this.hasError && this.errorHolder) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Image.create(this.errorHolder);
                        Image.width(this.widthValue);
                        Image.height(this.heightValue);
                        Image.objectFit(this.objectFit);
                        Image.borderRadius(this.cornerRadius);
                    }, Image);
                });
            }
            else if (!this.realSrc) {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        If.create();
                        // 加载中或尚未开始 -> 占位
                        if (this.placeholder) {
                            this.ifElseBranchUpdateFunction(0, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    Image.create(this.placeholder);
                                    Image.width(this.widthValue);
                                    Image.height(this.heightValue);
                                    Image.objectFit(ImageFit.Contain);
                                    Image.borderRadius(this.cornerRadius);
                                    Image.opacity(0.5);
                                }, Image);
                            });
                        }
                        else {
                            this.ifElseBranchUpdateFunction(1, () => {
                                this.observeComponentCreation2((elmtId, isInitialRender) => {
                                    // 简单的灰色占位
                                    // 这里用一个透明矩形模拟占位，避免使用 Color.fromARGB（在当前版本中无该 API）
                                    // 改为使用背景色和透明度控制
                                    Blank.create();
                                    // 简单的灰色占位
                                    // 这里用一个透明矩形模拟占位，避免使用 Color.fromARGB（在当前版本中无该 API）
                                    // 改为使用背景色和透明度控制
                                    Blank.width(this.widthValue);
                                    // 简单的灰色占位
                                    // 这里用一个透明矩形模拟占位，避免使用 Color.fromARGB（在当前版本中无该 API）
                                    // 改为使用背景色和透明度控制
                                    Blank.height(this.heightValue);
                                }, Blank);
                                // 简单的灰色占位
                                // 这里用一个透明矩形模拟占位，避免使用 Color.fromARGB（在当前版本中无该 API）
                                // 改为使用背景色和透明度控制
                                Blank.pop();
                            });
                        }
                    }, If);
                    If.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(2, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        // 正常加载完成
                        Image.create(this.realSrc as ImageSourceType);
                        // 正常加载完成
                        Image.width(this.widthValue);
                        // 正常加载完成
                        Image.height(this.heightValue);
                        // 正常加载完成
                        Image.objectFit(this.objectFit);
                        // 正常加载完成
                        Image.borderRadius(this.cornerRadius);
                    }, Image);
                });
            }
        }, If);
        If.pop();
        Column.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
}
