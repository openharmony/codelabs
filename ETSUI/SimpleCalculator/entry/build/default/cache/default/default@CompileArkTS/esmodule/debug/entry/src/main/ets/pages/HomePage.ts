if (!("finalizeConstruction" in ViewPU.prototype)) {
    Reflect.set(ViewPU.prototype, "finalizeConstruction", () => { });
}
interface HomePage_Params {
    model?: CalculateModel;
    inputValue?: string;
    calValue?: string;
}
import { CommonConstants } from "@bundle:com.example.simplecalculator/entry/ets/common/constants/CommonConstants";
import { CalculateModel } from "@bundle:com.example.simplecalculator/entry/ets/model/CalculateModel";
import type { PressKeysItem } from '../viewmodel/PressKeysItem';
import keysModel from "@bundle:com.example.simplecalculator/entry/ets/viewmodel/PresskeysViewModel";
class HomePage extends ViewPU {
    constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
        super(parent, __localStorage, elmtId, extraInfo);
        if (typeof paramsLambda === "function") {
            this.paramsGenerator_ = paramsLambda;
        }
        this.model = new CalculateModel(this);
        this.__inputValue = new ObservedPropertySimplePU('', this, "inputValue");
        this.__calValue = new ObservedPropertySimplePU('', this, "calValue");
        this.setInitiallyProvidedValue(params);
        this.finalizeConstruction();
    }
    setInitiallyProvidedValue(params: HomePage_Params) {
        if (params.model !== undefined) {
            this.model = params.model;
        }
        if (params.inputValue !== undefined) {
            this.inputValue = params.inputValue;
        }
        if (params.calValue !== undefined) {
            this.calValue = params.calValue;
        }
    }
    updateStateVars(params: HomePage_Params) {
    }
    purgeVariableDependenciesOnElmtId(rmElmtId) {
        this.__inputValue.purgeDependencyOnElmtId(rmElmtId);
        this.__calValue.purgeDependencyOnElmtId(rmElmtId);
    }
    aboutToBeDeleted() {
        this.__inputValue.aboutToBeDeleted();
        this.__calValue.aboutToBeDeleted();
        SubscriberManager.Get().delete(this.id__());
        this.aboutToBeDeletedInternal();
    }
    private model: CalculateModel;
    private __inputValue: ObservedPropertySimplePU<string>;
    get inputValue() {
        return this.__inputValue.get();
    }
    set inputValue(newValue: string) {
        this.__inputValue.set(newValue);
    }
    private __calValue: ObservedPropertySimplePU<string>;
    get calValue() {
        return this.__calValue.get();
    }
    set calValue(newValue: string) {
        this.__calValue.set(newValue);
    }
    initialRender() {
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.height(CommonConstants.FULL_PERCENT);
            Column.backgroundColor({ "id": 16777223, "type": 10001, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width(CommonConstants.FULL_PERCENT);
            Column.height({ "id": 16777235, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
            Column.alignItems(HorizontalAlign.End);
            Column.margin({
                right: { "id": 16777236, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" },
                top: { "id": 16777237, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }
            });
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            TextInput.create({ text: this.model.resultFormat(this.inputValue) });
            TextInput.height(CommonConstants.FULL_PERCENT);
            TextInput.fontSize((this.inputValue.length > CommonConstants.INPUT_LENGTH_MAX) ? { "id": 16777234, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" } : { "id": 16777233, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
            TextInput.enabled(false);
            TextInput.fontColor(Color.Black);
            TextInput.textAlign(TextAlign.End);
            TextInput.backgroundColor({ "id": 16777225, "type": 10001, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
        }, TextInput);
        Column.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.width(CommonConstants.FULL_PERCENT);
            Column.height({ "id": 16777242, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
            Column.alignItems(HorizontalAlign.End);
            Column.margin({
                right: { "id": 16777244, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" },
                bottom: { "id": 16777243, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }
            });
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Text.create(this.model.resultFormat(this.calValue));
            Text.fontSize({ "id": 16777234, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
            Text.fontColor({ "id": 16777228, "type": 10001, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
        }, Text);
        Text.pop();
        Column.pop();
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.layoutWeight(1);
            Column.width(CommonConstants.FULL_PERCENT);
            Column.backgroundColor({ "id": 16777226, "type": 10001, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
        }, Column);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.height(CommonConstants.FULL_PERCENT);
            Row.alignItems(VerticalAlign.Top);
            Row.margin({
                left: { "id": 16777240, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" },
                right: { "id": 16777241, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" }
            });
        }, Row);
        this.observeComponentCreation2((elmtId, isInitialRender) => {
            ForEach.create();
            const forEachItemGenFunction = (_item, columnItemIndex?: number) => {
                const columnItem = _item;
                this.observeComponentCreation2((elmtId, isInitialRender) => {
                    Column.create();
                    Column.layoutWeight(1);
                }, Column);
                this.observeComponentCreation2((elmtId, isInitialRender) => {
                    ForEach.create();
                    const forEachItemGenFunction = (_item, keyItemIndex?: number) => {
                        const keyItem = _item;
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            Column.create();
                            Column.layoutWeight(((columnItemIndex === (keysModel.getPressKeys().length - 1)) &&
                                (keyItemIndex === (columnItem.length - 1))) ? CommonConstants.TWO : 1);
                            Column.width(CommonConstants.FULL_PERCENT);
                            Column.justifyContent(FlexAlign.Center);
                        }, Column);
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            Column.create();
                            Column.width({ "id": 16777239, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
                            Column.height(((columnItemIndex === (keysModel.getPressKeys().length - 1)) &&
                                (keyItemIndex === (columnItem.length - 1))) ? { "id": 16777230, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" } : { "id": 16777238, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
                            Column.borderWidth(1);
                            Column.borderColor({ "id": 16777222, "type": 10001, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
                            Column.borderRadius({ "id": 16777229, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
                            Column.backgroundColor(((columnItemIndex === (keysModel.getPressKeys().length - 1)) &&
                                (keyItemIndex === (columnItem.length - 1))) ? { "id": 16777224, "type": 10001, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" } : Color.White);
                            Column.alignItems(HorizontalAlign.Center);
                            Column.justifyContent(FlexAlign.Center);
                            Column.onClick(() => {
                                if (keyItem.flag === 0) {
                                    this.model.inputSymbol(keyItem.value);
                                }
                                else {
                                    this.model.inputNumber(keyItem.value);
                                }
                            });
                        }, Column);
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            If.create();
                            if (keyItem.flag === 0) {
                                this.ifElseBranchUpdateFunction(0, () => {
                                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                                        Image.create(keyItem.source !== undefined ? keyItem.source : '');
                                        Image.width(keyItem.width);
                                        Image.height(keyItem.height);
                                    }, Image);
                                });
                            }
                            else {
                                this.ifElseBranchUpdateFunction(1, () => {
                                    this.observeComponentCreation2((elmtId, isInitialRender) => {
                                        Text.create(keyItem.value);
                                        Text.fontSize((keyItem.value === CommonConstants.DOTS) ? { "id": 16777232, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" } : { "id": 16777234, "type": 10002, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
                                        Text.width(keyItem.width);
                                        Text.height(keyItem.height);
                                    }, Text);
                                    Text.pop();
                                });
                            }
                        }, If);
                        If.pop();
                        Column.pop();
                        Column.pop();
                    };
                    this.forEachUpdateFunction(elmtId, columnItem, forEachItemGenFunction, (keyItem: PressKeysItem) => JSON.stringify(keyItem), true, false);
                }, ForEach);
                ForEach.pop();
                Column.pop();
            };
            this.forEachUpdateFunction(elmtId, keysModel.getPressKeys(), forEachItemGenFunction, (item: Array<Array<PressKeysItem>>) => JSON.stringify(item), true, false);
        }, ForEach);
        ForEach.pop();
        Row.pop();
        Column.pop();
        Column.pop();
    }
    rerender() {
        this.updateDirtyElements();
    }
    static getEntryName(): string {
        return "HomePage";
    }
}
registerNamedRoute(() => new HomePage(undefined, {}), "", { bundleName: "com.example.simplecalculator", moduleName: "entry", pagePath: "pages/HomePage", pageFullPath: "entry/src/main/ets/pages/HomePage", integratedHsp: "false", moduleType: "followWithHap" });
