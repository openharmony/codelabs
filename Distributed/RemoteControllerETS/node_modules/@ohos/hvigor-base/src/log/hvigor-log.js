"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 */
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
exports.HvigorLogger = void 0;
const log4js_1 = __importDefault(require("log4js"));
const default_configuration_js_1 = require("./default-configuration.js");
__exportStar(require("log4js"), exports);
/**
 * 基于log4js封装的HvigorLogger
 *
 * @since 2022/03/02
 */
class HvigorLogger {
    constructor(category) {
        log4js_1.default.configure((0, default_configuration_js_1.getConfiguration)());
        this._logger = log4js_1.default.getLogger(category);
        this._logger.level = (0, default_configuration_js_1.getLevel)();
    }
    /**
     * 获取对于类别的HvigorLogger实例
     *
     * @param category 默认是default
     */
    static getLogger(category) {
        return new HvigorLogger(category);
    }
    log(level, ...args) {
        this._logger.log(level, ...args);
    }
    logArray(level, array, title) {
        if (title) {
            this.log(level, title);
        }
        array.forEach(value => this.log(level, value));
    }
    debug(message, ...args) {
        this._logger.debug(message, ...args);
    }
    info(message, ...args) {
        this._logger.info(message, ...args);
    }
    warn(message, ...args) {
        if (message === undefined || message === "") {
            return;
        }
        this._logger.warn(message, ...args);
    }
    error(message, ...args) {
        this._logger.error(message, ...args);
    }
    errorMessageExit(message, ...args) {
        this._logger.error(message, ...args);
        process.exit(-1);
    }
    errorExit(e, message, ...args) {
        if (message) {
            this._logger.error(message, args);
        }
        this._logger.error(e.stack);
        process.exit(-1);
    }
    errorNoExit(e) {
        this._logger.error(e.stack);
    }
    setLevel(level) {
        (0, default_configuration_js_1.setLevel)(level);
        log4js_1.default.shutdown();
        log4js_1.default.configure((0, default_configuration_js_1.getConfiguration)());
    }
    getLevel() {
        return this._logger.level;
    }
    configure(configuration) {
        const result = Object.assign(Object.assign({}, (0, default_configuration_js_1.getConfiguration)()), configuration);
        (0, default_configuration_js_1.setConfiguration)(result);
        log4js_1.default.shutdown();
        log4js_1.default.configure(result);
    }
    getConfig() {
        return (0, default_configuration_js_1.getConfiguration)();
    }
}
exports.HvigorLogger = HvigorLogger;
//# sourceMappingURL=hvigor-log.js.map