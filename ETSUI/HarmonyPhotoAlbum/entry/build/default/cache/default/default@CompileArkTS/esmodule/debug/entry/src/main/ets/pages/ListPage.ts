if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface ListPage_Params {
    searchText?: string;
    selectedCategory?: string;
    displayPhotos?: PhotoModel[];
    isLoading?: boolean;
    selectedIndex?: number;
}
import router from "@ohos:router";
import Constants from "@normalized:N&&&entry/src/main/ets/common/constants/Constants&";
import { SearchBar } from "@normalized:N&&&entry/src/main/ets/components/SearchBar&";
import { CategoryFilter } from "@normalized:N&&&entry/src/main/ets/components/CategoryFilter&";
import PhotoService from "@normalized:N&&&entry/src/main/ets/service/PhotoService&";
import type { PhotoModel } from '../model/PhotoModel';
import { LazyImage } from "@normalized:N&&&entry/src/main/ets/components/LazyImage&";
import Logger from "@normalized:N&&&entry/src/main/ets/common/utils/Logger&";
class ListPage extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.__searchText = new ObservedPropertySimplePU('', this, "searchText");
        this.__selectedCategory = new ObservedPropertySimplePU(Constants.ALL_CATEGORY, this, "selectedCategory");
        this.__displayPhotos = new ObservedPropertyObjectPU([], this, "displayPhotos");
        this.__isLoading = new ObservedPropertySimplePU(true, this, "isLoading");
        this.__selectedIndex = this.createStorageLink('selectedIndex', 0, "selectedIndex");
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: ListPage_Params) {
        if (params.searchText !== undefined) {
            this.searchText = params.searchText;
        }
        if (params.selectedCategory !== undefined) {
            this.selectedCategory = params.selectedCategory;
        }
        if (params.displayPhotos !== undefined) {
            this.displayPhotos = params.displayPhotos;
        }
        if (params.isLoading !== undefined) {
            this.isLoading = params.isLoading;
        }
    }
    updateStateVars(params: ListPage_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__searchText.purgeDependencyOnElmtId(rmElmtId);
        this.__selectedCategory.purgeDependencyOnElmtId(rmElmtId);
        this.__displayPhotos.purgeDependencyOnElmtId(rmElmtId);
        this.__isLoading.purgeDependencyOnElmtId(rmElmtId);
        this.__selectedIndex.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__searchText.aboutToBeDeleted();
        this.__selectedCategory.aboutToBeDeleted();
        this.__displayPhotos.aboutToBeDeleted();
        this.__isLoading.aboutToBeDeleted();
        this.__selectedIndex.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    // 状态变量
    private __searchText: ObservedPropertySimplePU<string>;
    get searchText() {
        return this.__searchText.get();
    }
    set searchText(newValue: string) {
        this.__searchText.set(newValue);
    }
    private __selectedCategory: ObservedPropertySimplePU<string>;
    get selectedCategory() {
        return this.__selectedCategory.get();
    }
    set selectedCategory(newValue: string) {
        this.__selectedCategory.set(newValue);
    }
    private __displayPhotos: ObservedPropertyObjectPU<PhotoModel[]>;
    get displayPhotos() {
        return this.__displayPhotos.get();
    }
    set displayPhotos(newValue: PhotoModel[]) {
        this.__displayPhotos.set(newValue);
    }
    private __isLoading: ObservedPropertySimplePU<boolean>;
    get isLoading() {
        return this.__isLoading.get();
    }
    set isLoading(newValue: boolean) {
        this.__isLoading.set(newValue);
    }
    private __selectedIndex: ObservedPropertyAbstractPU<number>;
    get selectedIndex() {
        return this.__selectedIndex.get();
    }
    set selectedIndex(newValue: number) {
        this.__selectedIndex.set(newValue);
    }
    // ==========================================
    // 🔧 修复：直接使用数据库数据
    // ==========================================
    aboutToAppear() {
        console.info('[ListPage] ========== 页面加载开始 ==========');
        this.loadPhotos();
    }
    // ==========================================
    // 🆕 页面显示时刷新数据（从详情页返回时）
    // ==========================================
    onPageShow() {
        console.info('[ListPage] 页面显示，刷新数据');
        this.loadPhotos();
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Navigation.create(new NavPathStack(), { moduleName: "entry", pagePath: "entry/src/main/ets/pages/ListPage", isUserCreateStack: false });
            Navigation.title(Constants.PAGE_TITLE);
            Navigation.hideBackButton(false);
            Navigation.titleMode(NavigationTitleMode.Mini);
            Navigation.toolbarConfiguration([
                {
                    value: '返回首页',
                    action: () => {
                        console.info('[ListPage] 工具栏-返回首页');
                        router.back();
                    }
                }
            ]);
        }, Navigation);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width('100%');
            Column.height('100%');
            Column.backgroundColor('#F8F8F8');
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            __Common__.create();
            __Common__.margin({ left: 16, right: 16 });
        }, __Common__);
        {
            this.observeComponentCreation2((elmtId, isInitialRender) => {
                if (isInitialRender) {
                    let componentCall = new 
                    // ==========================================
                    // ✅ 搜索栏（始终显示）
                    // ==========================================
                    SearchBar(this, {
                        searchText: this.__searchText,
                        onSearch: (text: string) => {
                            console.info(`[ListPage] 搜索关键词: ${text}`);
                            this.handleSearch(text);
                        }
                    }, undefined, elmtId, () => { }, { page: "entry/src/main/ets/pages/ListPage.ets", line: 62, col: 9 });
                    ViewPU.create(componentCall);
                    let paramsLambda = () => {
                        return {
                            searchText: this.searchText,
                            onSearch: (text: string) => {
                                console.info(`[ListPage] 搜索关键词: ${text}`);
                                this.handleSearch(text);
                            }
                        };
                    };
                    componentCall.paramsGenerator_ = paramsLambda;
                }
                else {
                    this.updateStateVarsOfChildByElmtId(elmtId, {});
                }
            }, { name: "SearchBar" });
        }
        __Common__.pop();
        {
            this.observeComponentCreation2((elmtId, isInitialRender) => {
                if (isInitialRender) {
                    let componentCall = new 
                    // ==========================================
                    // ✅ 分类筛选（始终显示）
                    // ==========================================
                    CategoryFilter(this, {
                        selectedCategory: this.__selectedCategory,
                        onCategoryChange: (category: string) => {
                            console.info(`[ListPage] 切换分类: ${category}`);
                            this.handleCategoryChange(category);
                        }
                    }, undefined, elmtId, () => { }, { page: "entry/src/main/ets/pages/ListPage.ets", line: 74, col: 9 });
                    ViewPU.create(componentCall);
                    let paramsLambda = () => {
                        return {
                            selectedCategory: this.selectedCategory,
                            onCategoryChange: (category: string) => {
                                console.info(`[ListPage] 切换分类: ${category}`);
                                this.handleCategoryChange(category);
                            }
                        };
                    };
                    componentCall.paramsGenerator_ = paramsLambda;
                }
                else {
                    this.updateStateVarsOfChildByElmtId(elmtId, {});
                }
            }, { name: "CategoryFilter" });
        }
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            // ==========================================
            // 图片网格
            // ==========================================
            if (this.isLoading) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.LoadingView.bind(this)();
                });
            }
            else if (this.displayPhotos.length > 0) {
                this.ifElseBranchUpdateFunction(1, () => {
                    this.PhotoGrid.bind(this)();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(2, () => {
                    this.EmptyView.bind(this)();
                });
            }
        }, If);
        If.pop();
        Column.pop();
        Navigation.pop();
    }
    // ==========================================
    // 图片网格
    // ==========================================
    PhotoGrid(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Grid.create();
            Grid.columnsTemplate(Constants.GRID_COLUMNS_TEMPLATE);
            Grid.rowsGap(Constants.LIST_ITEM_SPACE);
            Grid.columnsGap(Constants.LIST_ITEM_SPACE);
            Grid.layoutWeight(1);
            Grid.cachedCount(10);
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
            this.forEachUpdateFunction(elmtId, this.displayPhotos, forEachItemGenFunction, (photo: PhotoModel) => photo.id.toString(), true, false);
        }, ForEach);
        ForEach.pop();
        Grid.pop();
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
        {
            this.observeComponentCreation2((elmtId, isInitialRender) => {
                if (isInitialRender) {
                    let componentCall = new 
                    // 图片 - 🔧 修复：添加 file:// 前缀
                    LazyImage(this, {
                        src: 'file://' + photo.path,
                        widthValue: '100%',
                        heightValue: '100%',
                        objectFit: ImageFit.Cover,
                        placeholder: Constants.PLACEHOLDER_IMAGE,
                        errorHolder: Constants.ERROR_IMAGE,
                        cornerRadius: 8
                    }, undefined, elmtId, () => { }, { page: "entry/src/main/ets/pages/ListPage.ets", line: 139, col: 7 });
                    ViewPU.create(componentCall);
                    let paramsLambda = () => {
                        return {
                            src: 'file://' + photo.path,
                            widthValue: '100%',
                            heightValue: '100%',
                            objectFit: ImageFit.Cover,
                            placeholder: Constants.PLACEHOLDER_IMAGE,
                            errorHolder: Constants.ERROR_IMAGE,
                            cornerRadius: 8
                        };
                    };
                    componentCall.paramsGenerator_ = paramsLambda;
                }
                else {
                    this.updateStateVarsOfChildByElmtId(elmtId, {});
                }
            }, { name: "LazyImage" });
        }
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 图片信息叠加层
            Column.create();
            // 图片信息叠加层
            Column.width('100%');
            // 图片信息叠加层
            Column.padding(8);
            // 图片信息叠加层
            Column.linearGradient({
                direction: GradientDirection.Bottom,
                colors: [[0x00000000, 0.0], [0xAA000000, 1.0]]
            });
            // 图片信息叠加层
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
                        Text.padding({ left: 6, right: 6, top: 2, bottom: 2 });
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
        // 图片信息叠加层
        Column.pop();
        Stack.pop();
    }
    // ==========================================
    // 加载中视图
    // ==========================================
    LoadingView(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.layoutWeight(1);
            Column.justifyContent(FlexAlign.Center);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            LoadingProgress.create();
            LoadingProgress.width(40);
            LoadingProgress.height(40);
            LoadingProgress.color('#007DFF');
        }, LoadingProgress);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create('加载中...');
            Text.fontSize(14);
            Text.fontColor('#999999');
            Text.margin({ top: 16 });
        }, Text);
        Text.pop();
        Column.pop();
    }
    // ==========================================
    // 空状态视图
    // ==========================================
    EmptyView(parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.layoutWeight(1);
            Column.justifyContent(FlexAlign.Center);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create('📷');
            Text.fontSize(60);
            Text.opacity(0.4);
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create('暂无图片');
            Text.fontSize(14);
            Text.fontColor('#999999');
            Text.margin({ top: 16 });
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(this.searchText ? '试试其他搜索词' : '快去添加图片吧');
            Text.fontSize(12);
            Text.fontColor('#CCCCCC');
            Text.margin({ top: 8 });
        }, Text);
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            // ==========================================
            // 🆕 添加返回首页按钮
            // ==========================================
            if (!this.searchText && this.displayPhotos.length === 0) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Button.createWithLabel('返回首页上传');
                        Button.fontSize(14);
                        Button.margin({ top: 20 });
                        Button.onClick(() => {
                            router.back();
                        });
                    }, Button);
                    Button.pop();
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
        }, If);
        If.pop();
        Column.pop();
    }
    // ==========================================
    // 数据加载与搜索逻辑
    // ==========================================
    private loadPhotos() {
        console.info('[ListPage] 开始加载图片...');
        this.isLoading = true;
        PhotoService.queryAll()
            .then((photos: PhotoModel[]) => {
            this.displayPhotos = photos;
            this.isLoading = false;
            console.info(`[ListPage] ✅ 加载成功，共 ${photos.length} 张图片`);
            // 👇 如果数据库为空，提示用户
            if (photos.length === 0) {
                console.warn('[ListPage] ⚠️ 数据库中没有图片，请先上传图片');
            }
        })
            .catch((err: Error) => {
            console.error(`[ListPage] ❌ 加载失败: ${err.message}`);
            Logger.showError('图片加载失败，请稍后重试');
            this.displayPhotos = [];
            this.isLoading = false;
        });
    }
    private handleSearch(keyword: string) {
        console.info(`[ListPage] 执行搜索，关键词: "${keyword}", 分类: "${this.selectedCategory}"`);
        this.isLoading = true;
        PhotoService.searchPhotosByCategoryAndKeyword(this.selectedCategory, keyword)
            .then((photos: PhotoModel[]) => {
            this.displayPhotos = photos;
            this.isLoading = false;
            console.info(`[ListPage] ✅ 搜索成功，找到 ${photos.length} 张图片`);
        })
            .catch((err: Error) => {
            console.error(`[ListPage] ❌ 搜索失败: ${err.message}`);
            Logger.showError('搜索失败，请稍后重试');
            this.displayPhotos = [];
            this.isLoading = false;
        });
    }
    private handleCategoryChange(category: string) {
        console.info(`[ListPage] 切换分类到: "${category}"`);
        this.isLoading = true;
        PhotoService.searchPhotosByCategoryAndKeyword(category, this.searchText)
            .then((photos: PhotoModel[]) => {
            this.displayPhotos = photos;
            this.isLoading = false;
            console.info(`[ListPage] ✅ 分类切换成功，共 ${photos.length} 张图片`);
        })
            .catch((err: Error) => {
            console.error(`[ListPage] ❌ 分类切换失败: ${err.message}`);
            Logger.showError('切换分类失败，请稍后重试');
            this.displayPhotos = [];
            this.isLoading = false;
        });
    }
    private navigateToDetail(index: number) {
        this.selectedIndex = index;
        const photoPaths = this.displayPhotos.map(photo => 'file://' + photo.path); // 🔧 添加 file:// 前缀
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
        return "ListPage";
    }
}
registerNamedRoute(() => new ListPage(undefined, {}), "", { bundleName: "com.example.electronicalbum", moduleName: "entry", pagePath: "pages/ListPage", pageFullPath: "entry/src/main/ets/pages/ListPage", integratedHsp: "false", moduleType: "followWithHap" });
