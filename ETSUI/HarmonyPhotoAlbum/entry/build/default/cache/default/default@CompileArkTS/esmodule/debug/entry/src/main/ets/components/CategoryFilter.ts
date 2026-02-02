if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface CategoryFilter_Params {
    selectedCategory?: string;
    categories?: string[];
    onCategoryChange?: (category: string) => void;
}
import Constants from "@normalized:N&&&entry/src/main/ets/common/constants/Constants&";
export class CategoryFilter extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.__selectedCategory = new SynchedPropertySimpleTwoWayPU(params.selectedCategory, this, "selectedCategory");
        this.__categories = new ObservedPropertyObjectPU([], this, "categories");
        this.onCategoryChange = undefined;
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: CategoryFilter_Params) {
        if (params.categories !== undefined) {
            this.categories = params.categories;
        }
        if (params.onCategoryChange !== undefined) {
            this.onCategoryChange = params.onCategoryChange;
        }
    }
    updateStateVars(params: CategoryFilter_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__selectedCategory.purgeDependencyOnElmtId(rmElmtId);
        this.__categories.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__selectedCategory.aboutToBeDeleted();
        this.__categories.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    private __selectedCategory: SynchedPropertySimpleTwoWayPU<string>; // 当前选中的分类（双向绑定）
    get selectedCategory() {
        return this.__selectedCategory.get();
    }
    set selectedCategory(newValue: string) {
        this.__selectedCategory.set(newValue);
    }
    private __categories: ObservedPropertyObjectPU<string[]>; // 分类列表
    get categories() {
        return this.__categories.get();
    }
    set categories(newValue: string[]) {
        this.__categories.set(newValue);
    }
    private onCategoryChange?: (category: string) => void; // 分类切换回调
    aboutToAppear() {
        // 初始化分类列表：全部 + 预定义分类
        this.categories = [Constants.ALL_CATEGORY, ...Constants.CATEGORIES];
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width('100%');
            Column.alignItems(HorizontalAlign.Start);
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 标题
            Text.create('分类');
            // 标题
            Text.fontSize(14);
            // 标题
            Text.fontColor('#999999');
            // 标题
            Text.margin({ left: 16, top: 8, bottom: 8 });
            // 标题
            Text.alignSelf(ItemAlign.Start);
        }, Text);
        // 标题
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 分类列表（横向滚动）
            Scroll.create();
            // 分类列表（横向滚动）
            Scroll.scrollable(ScrollDirection.Horizontal);
            // 分类列表（横向滚动）
            Scroll.scrollBar(BarState.Off);
            // 分类列表（横向滚动）
            Scroll.width('100%');
        }, Scroll);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.padding({ left: 12, right: 12 });
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            ForEach.create();
            const forEachItemGenFunction = _item => {
                const category = _item;
                this.CategoryItem.bind(this)(category);
            };
            this.forEachUpdateFunction(elmtId, this.categories, forEachItemGenFunction);
        }, ForEach);
        ForEach.pop();
        Row.pop();
        // 分类列表（横向滚动）
        Scroll.pop();
        Column.pop();
    }
    /**
     * 分类选项
     */
    CategoryItem(category: string, parent = null) {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(category);
            Context.animation({
                duration: 200,
                curve: Curve.EaseInOut
            });
            Text.fontSize(14);
            Text.fontColor(this.selectedCategory === category ? Color.White : '#333333');
            Text.padding({ left: 16, right: 16, top: 8, bottom: 8 });
            Text.margin({ left: 4, right: 4 });
            Text.backgroundColor(this.selectedCategory === category ? '#007DFF' : '#E8E8E8');
            Text.borderRadius(16);
            Text.onClick(() => {
                this.selectedCategory = category;
                this.onCategoryChange?.(category);
            });
            Context.animation(null);
        }, Text);
        Text.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
}
