"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.
 *
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.ProjectImpl = void 0;
const path_1 = __importDefault(require("path"));
const common_const_1 = require("../options/common-const");
const default_module_impl_1 = require("./default-module-impl");
/**
 * hvigor工程的基础root module
 *
 * @since 2021/12/27
 */
class ProjectImpl extends default_module_impl_1.DefaultModuleImpl {
    constructor(moduleName, moduleDir) {
        super(moduleName, moduleDir);
        this._projectStructureFile = path_1.default.resolve(moduleDir, common_const_1.HvigorCommonConst.PROJECT_CONFIG_FILE);
        this._subProjects = new Map();
    }
    findModuleByName(moduleName) {
        return this._subProjects.get(moduleName);
    }
    getProject() {
        return this;
    }
    getSubProjects() {
        return this._subProjects;
    }
    addSubProject(module) {
        this._subProjects.set(module.getName(), module);
    }
    getAllSubProjects() {
        const modules = [];
        for (const subModule of this._subProjects.values()) {
            modules.push(subModule);
        }
        return modules;
    }
}
exports.ProjectImpl = ProjectImpl;
//# sourceMappingURL=project-impl.js.map