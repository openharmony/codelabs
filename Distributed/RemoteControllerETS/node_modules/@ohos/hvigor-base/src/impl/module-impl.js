"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.
 *
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.ModuleImpl = void 0;
const default_module_impl_1 = require("./default-module-impl");
/**
 * hvigor工程的基础module
 *
 * @since 2021/12/27
 */
class ModuleImpl extends default_module_impl_1.DefaultModuleImpl {
    constructor(project, moduleName, moduleDir) {
        super(moduleName, moduleDir);
        this._project = project;
    }
    findModuleByName(moduleName) {
        return this._project.getSubProjects().get(moduleName);
    }
    getProject() {
        return this._project;
    }
}
exports.ModuleImpl = ModuleImpl;
//# sourceMappingURL=module-impl.js.map