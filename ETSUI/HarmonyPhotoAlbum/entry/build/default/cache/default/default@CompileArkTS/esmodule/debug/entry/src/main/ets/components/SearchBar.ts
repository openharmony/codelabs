if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface SearchBar_Params {
    searchText?: string;
    onSearch?: (text: string) => void;
    debounceTimer?: number;
    debounceDelay?: number;
}
export class SearchBar extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.__searchText = new SynchedPropertySimpleTwoWayPU(params.searchText, this, "searchText");
        this.onSearch = undefined;
        this.debounceTimer = -1;
        this.debounceDelay = 300;
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: SearchBar_Params) {
        if (params.onSearch !== undefined) {
            this.onSearch = params.onSearch;
        }
        if (params.debounceTimer !== undefined) {
            this.debounceTimer = params.debounceTimer;
        }
        if (params.debounceDelay !== undefined) {
            this.debounceDelay = params.debounceDelay;
        }
    }
    updateStateVars(params: SearchBar_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__searchText.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__searchText.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    private __searchText: SynchedPropertySimpleTwoWayPU<string>; // 搜索文本（双向绑定）
    get searchText() {
        return this.__searchText.get();
    }
    set searchText(newValue: string) {
        this.__searchText.set(newValue);
    }
    private onSearch?: (text: string) => void; // 搜索回调
    private debounceTimer: number; // 防抖定时器
    private debounceDelay: number; // 防抖延迟（毫秒）
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.width('100%');
            Row.height(48);
            Row.backgroundColor('#F5F5F5');
            Row.borderRadius(24);
            Row.margin({ top: 8, bottom: 8 });
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 搜索图标
            Text.create('🔍');
            // 搜索图标
            Text.fontSize(18);
            // 搜索图标
            Text.margin({ left: 12, right: 8 });
        }, Text);
        // 搜索图标
        Text.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            // 搜索输入框
            TextInput.create({
                placeholder: '搜索图片名称或标签',
                text: this.searchText
            });
            // 搜索输入框
            TextInput.layoutWeight(1);
            // 搜索输入框
            TextInput.height(40);
            // 搜索输入框
            TextInput.backgroundColor(Color.Transparent);
            // 搜索输入框
            TextInput.placeholderColor('#999999');
            // 搜索输入框
            TextInput.fontSize(14);
            // 搜索输入框
            TextInput.onChange((value: string) => {
                this.searchText = value;
                this.handleSearchWithDebounce(value);
            });
        }, TextInput);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            // 清除按钮
            if (this.searchText.length > 0) {
                this.ifElseBranchUpdateFunction(0, () => {
                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                        Text.create('✖');
                        Text.fontSize(14);
                        Text.fontColor('#666666');
                        Text.margin({ right: 12 });
                        Text.onClick(() => {
                            this.searchText = '';
                            this.onSearch?.('');
                        });
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
        Row.pop();
    }
    /**
     * 防抖搜索处理
     */
    private handleSearchWithDebounce(text: string) {
        // 清除之前的定时器
        if (this.debounceTimer !== -1) {
            clearTimeout(this.debounceTimer);
        }
        // 设置新的定时器
        this.debounceTimer = setTimeout(() => {
            this.onSearch?.(text);
            this.debounceTimer = -1;
        }, this.debounceDelay);
    }
    rerender() {
        this.updateDirtyElements();
    }
}
