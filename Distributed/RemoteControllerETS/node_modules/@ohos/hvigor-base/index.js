"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __exportStar = (this && this.__exportStar) || function(m, exports) {
    for (var p in m) if (p !== "default" && !Object.prototype.hasOwnProperty.call(exports, p)) __createBinding(exports, m, p);
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.vigorConfigInst = exports.VigorConfig = exports.Hvigor = void 0;
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.
 *
 */
const path_1 = __importDefault(require("path"));
const json5_reader_js_1 = require("./src/util/json5-reader.js");
const project_impl_js_1 = require("./src/impl/project-impl.js");
const module_impl_js_1 = require("./src/impl/module-impl.js");
const common_const_js_1 = require("./src/options/common-const.js");
const validate_util_js_1 = require("./src/util/validate-util.js");
const hvigor_log_js_1 = require("./src/log/hvigor-log.js");
__exportStar(require("./src/plugin/plugin.js"), exports);
__exportStar(require("./src/impl/hvigor-module.js"), exports);
__exportStar(require("./src/task/task.js"), exports);
__exportStar(require("./src/options/common-const.js"), exports);
__exportStar(require("./src/log/hvigor-log.js"), exports);
const Undertaker = require('undertaker');
/**
 * 提供前端工程打包的工具的主入口
 *
 * @since 2021/12/09
 */
class Hvigor extends Undertaker {
    constructor() {
        super();
    }
}
exports.Hvigor = Hvigor;
/**
 * 提供前端工程配置信息的数据类
 *
 * @since 2021/12/09
 */
class VigorConfig {
    constructor() {
        this._log = hvigor_log_js_1.HvigorLogger.getLogger(VigorConfig.name);
        this._project = undefined;
        this._projectDir = "";
        this._projectConfigFile = "";
        this._extraConfig = new Map();
    }
    /**
     * 返回Project模型
     *
     * @returns {Project | undefined}
     */
    getProject() {
        return this._project;
    }
    /**
     * 返回在命令行中传递的额外参数
     *
     * @returns {Map<string, string>}
     */
    getExtraConfig() {
        return this._extraConfig;
    }
    setExtraConfig(value) {
        this._extraConfig = value;
    }
    /**
     * 以工程根路径下的配置文件，对工程进行初始化
     *
     * @param {string} projectConfigFile 工程级别build-profile.json5的路径
     * @param {string} projectRootDir 工程根路径
     * @returns {Project}
     */
    initRootProject(projectConfigFile, projectRootDir) {
        var _a;
        this._projectConfigFile = projectConfigFile;
        this._projectDir = projectRootDir;
        const projectImpl = new project_impl_js_1.ProjectImpl(path_1.default.basename(projectRootDir), projectRootDir);
        const projectStructureOpt = json5_reader_js_1.Json5Reader.getJson5Obj(this._projectConfigFile);
        const moduleOpts = projectStructureOpt.modules;
        if (moduleOpts.length === 0) {
            this._log.errorMessageExit(`No modules found,Please check root project ${common_const_js_1.HvigorCommonConst.PROJECT_CONFIG_FILE}
                at ${projectImpl.getModuleDir()}:1:1`);
        }
        (_a = projectStructureOpt.modules) === null || _a === void 0 ? void 0 : _a.forEach(module => {
            validate_util_js_1.ValidateUtil.validateModule(module);
            projectImpl.addSubProject(new module_impl_js_1.ModuleImpl(projectImpl, module.name, module.srcPath));
        });
        this._project = projectImpl;
        return projectImpl;
    }
}
exports.VigorConfig = VigorConfig;
exports.vigorConfigInst = new VigorConfig();
//# sourceMappingURL=index.js.map