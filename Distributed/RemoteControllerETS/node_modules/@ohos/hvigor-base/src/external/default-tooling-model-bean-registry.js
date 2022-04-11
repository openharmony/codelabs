"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.defaultModelRegistry = void 0;
const hvigor_log_js_1 = require("../log/hvigor-log.js");
/**
 * 默认的Hvigor的数据扩展模型的注册器
 *
 * @since 2022/2/19
 */
class DefaultToolingModelBeanRegistry {
    constructor() {
        this._log = hvigor_log_js_1.HvigorLogger.getLogger("ToolingModelBeanRegistry");
        this._modelMap = new Map();
    }
    registry(modelBean) {
        const beanId = modelBean.modelId;
        if (this._modelMap.has(beanId)) {
            this._log.error("Multiple model has bean registered,Please check!");
        }
        this._modelMap.set(beanId, modelBean);
    }
    getModelMap() {
        return this._modelMap;
    }
}
// 全局单例,保证不同插件中注册的模型数据不冲突
exports.defaultModelRegistry = new DefaultToolingModelBeanRegistry();
//# sourceMappingURL=default-tooling-model-bean-registry.js.map