"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.
 *
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.DefaultModuleImpl = void 0;
const path_1 = __importDefault(require("path"));
const common_const_1 = require("../options/common-const");
/**
 * hvigor中module的默认公共父类
 *
 * @since 2022/1/8
 */
class DefaultModuleImpl {
    constructor(moduleName, modulePath) {
        this._moduleName = moduleName;
        this._modulePath = modulePath;
        this._packageJsonPath = path_1.default.resolve(modulePath, "package.json");
        this._buildFilePath = path_1.default.resolve(modulePath, common_const_1.HvigorCommonConst.BUILD_FILE_NAME);
    }
    /**
     * 获取构建的hvigorfile.js路径
     */
    getBuildFilePath() {
        return this._buildFilePath;
    }
    /**
     * 获取模块的路径
     */
    getModuleDir() {
        return this._modulePath;
    }
    /**
     * 获取模块的名称
     */
    getName() {
        return this._moduleName;
    }
    /**
     * 获取模块下的package.json路径
     */
    getPackageJsonPath() {
        return this._packageJsonPath;
    }
    /**
     * 绑定具体的Plugin对象到Module对象上
     *
     * @param plugin
     * @protected
     */
    bindPlugin(plugin) {
        this._plugin = plugin;
    }
    /**
     * 获取Module对象的plugin对象
     *
     * @returns {HvigorPlugin | undefined}
     */
    getPlugin() {
        return this._plugin;
    }
}
exports.DefaultModuleImpl = DefaultModuleImpl;
//# sourceMappingURL=default-module-impl.js.map