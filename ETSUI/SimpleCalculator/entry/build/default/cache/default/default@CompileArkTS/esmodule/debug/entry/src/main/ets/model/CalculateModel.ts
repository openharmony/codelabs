import { CommonConstants, Symbol } from "@bundle:com.example.simplecalculator/entry/ets/common/constants/CommonConstants";
import CalculateUtil from "@bundle:com.example.simplecalculator/entry/ets/common/util/CalculateUtil";
import CheckEmptyUtil from "@bundle:com.example.simplecalculator/entry/ets/common/util/CheckEmptyUtil";
import Logger from "@bundle:com.example.simplecalculator/entry/ets/common/util/Logger";
import type { IContext } from '../viewmodel/PressKeysItem';
export class CalculateModel {
    private expressions: Array<string> = [];
    private context: IContext;
    constructor(context: IContext) {
        this.context = context;
    }
    /**
     * Input Symbols.
     *
     * @param value Input Operators.
     */
    inputSymbol(value: string) {
        if (CheckEmptyUtil.isEmpty(value)) {
            return;
        }
        let len = this.expressions.length;
        switch (value) {
            case Symbol.CLEAN:
                this.expressions = [];
                this.context.calValue = '';
                break;
            case Symbol.DEL:
                this.inputDelete(len);
                break;
            case Symbol.EQU:
                if (len === 0) {
                    return;
                }
                this.getResult().then(result => {
                    if (!result) {
                        return;
                    }
                    this.context.inputValue = this.context.calValue;
                    this.context.calValue = '';
                    this.expressions = [];
                    this.expressions.push(this.context.inputValue);
                });
                break;
            default:
                this.inputOperators(len, value);
                break;
        }
        this.formatInputValue();
    }
    /**
     * Enter numbers.
     *
     * @param value Enter numbers.
     */
    inputNumber(value: string) {
        if (CheckEmptyUtil.isEmpty(value)) {
            return;
        }
        let len = this.expressions.length;
        let last = len > 0 ? this.expressions[len - 1] : '';
        let secondLast = len > 1 ? this.expressions[len - CommonConstants.TWO] : undefined;
        if (!this.validateEnter(last, value)) {
            return;
        }
        if (!last) {
            this.expressions.push(value);
        }
        else if (!secondLast) {
            this.expressions[len - 1] += value;
        }
        if (secondLast && CalculateUtil.isSymbol(secondLast)) {
            this.expressions[len - 1] += value;
        }
        if (secondLast && !CalculateUtil.isSymbol(secondLast)) {
            this.expressions.push(value);
        }
        this.formatInputValue();
        if (value !== CommonConstants.DOTS) {
            this.getResult();
        }
    }
    /**
     * Verify that you can enter.
     *
     * @param last Value of the last element.
     * @param value Current input value.
     * return Indicates whether to allow input.
     */
    validateEnter(last: string, value: string) {
        if (!last && value === CommonConstants.PERCENT_SIGN) {
            return false;
        }
        if ((last === CommonConstants.MIN) && (value === CommonConstants.PERCENT_SIGN)) {
            return false;
        }
        if (last.endsWith(CommonConstants.PERCENT_SIGN)) {
            return false;
        }
        if ((last.indexOf(CommonConstants.DOTS) !== -1) && (value === CommonConstants.DOTS)) {
            return false;
        }
        if ((last === '0') && (value !== CommonConstants.DOTS) &&
            (value !== CommonConstants.PERCENT_SIGN)) {
            return false;
        }
        return true;
    }
    /**
     * Delete Key Trigger.
     *
     * @param len Expression Length.
     */
    inputDelete(len: number) {
        if (len === 0) {
            return;
        }
        let last = this.expressions[len - 1];
        let lastLen = last.length;
        if (lastLen === 1) {
            this.expressions.pop();
            len = this.expressions.length;
        }
        else {
            this.expressions[len - 1] = last.slice(0, last.length - 1);
        }
        if (len === 0) {
            this.context.inputValue = '';
            this.context.calValue = '';
            return;
        }
        if (!CalculateUtil.isSymbol(this.expressions[len - 1])) {
            this.getResult();
        }
    }
    /**
     * Triggered when input is added, subtracted, multiplied, and divided.
     *
     * @param len Expression Length.
     * @param value Current Input Value.
     */
    inputOperators(len: number, value: string) {
        let last = len > 0 ? this.expressions[len - 1] : undefined;
        let secondLast = len > 1 ? this.expressions[len - CommonConstants.TWO] : undefined;
        if (!last && (value === Symbol.MIN)) {
            this.expressions.push(this.getSymbol(value));
            return;
        }
        if (!last) {
            return;
        }
        if (!CalculateUtil.isSymbol(last)) {
            this.expressions.push(this.getSymbol(value));
            return;
        }
        if ((value === Symbol.MIN) &&
            (last === CommonConstants.MIN || last === CommonConstants.ADD)) {
            this.expressions.pop();
            this.expressions.push(this.getSymbol(value));
            return;
        }
        if (!secondLast) {
            return;
        }
        if (value !== Symbol.MIN) {
            this.expressions.pop();
        }
        if (CalculateUtil.isSymbol(secondLast)) {
            this.expressions.pop();
        }
        this.expressions.push(this.getSymbol(value));
    }
    /**
     * Get Operator.
     *
     * @param value.
     * @return Operators.
     */
    getSymbol(value: string) {
        if (CheckEmptyUtil.isEmpty(value)) {
            return '';
        }
        let symbol = '';
        switch (value) {
            case Symbol.ADD:
                symbol = CommonConstants.ADD;
                break;
            case Symbol.MIN:
                symbol = CommonConstants.MIN;
                break;
            case Symbol.MUL:
                symbol = CommonConstants.MUL;
                break;
            case Symbol.DIV:
                symbol = CommonConstants.DIV;
                break;
            default:
                break;
        }
        return symbol;
    }
    /**
     * Make a deep copy of an expression.
     *
     * @return deep copy expression.
     */
    deepCopy(): Array<string> {
        let copyExpressions: Array<string> = Array.from(this.expressions);
        return copyExpressions;
    }
    /**
     * Obtaining Results.
     *
     * @return Whether the result is incorrect.
     */
    async getResult() {
        let calResult = CalculateUtil.parseExpression(this.deepCopy());
        if (calResult === 'NaN') {
            this.context.calValue = this.resourceToString({ "id": 16777220, "type": 10003, params: [], "bundleName": "com.example.simplecalculator", "moduleName": "entry" });
            return false;
        }
        this.context.calValue = calResult;
        return true;
    }
    /**
     * Number Formatting.
     *
     * @param value Formatting parameters.
     * @return Thousand percentile data.
     */
    resultFormat(value: string) {
        let reg = (value.indexOf('.') > -1) ? new RegExp("/(\d)(?=(\d{3})+\.)/g") : new RegExp("/(\d)(?=(?:\d{3})+$)/g");
        return value.replace(reg, '$1,');
    }
    /**
     * Convert a resource file to a string.
     *
     * @param resource Resource file.
     * @return Character string converted from the resource file.
     */
    resourceToString(resource: Resource): string {
        if (CheckEmptyUtil.isEmpty(resource)) {
            return '';
        }
        let result = '';
        try {
            result = getContext(this).resourceManager.getStringSync(resource.id);
        }
        catch (error) {
            Logger.error('[CalculateModel] getResourceString fail: ' + JSON.stringify(error));
        }
        return result;
    }
    /**
     * Thousands in the formatting result.
     */
    formatInputValue() {
        let deepExpressions: Array<string> = [];
        this.deepCopy().forEach((item: string, index: number) => {
            deepExpressions[index] = this.resultFormat(item);
        });
        this.context.inputValue = deepExpressions.join('');
    }
}
