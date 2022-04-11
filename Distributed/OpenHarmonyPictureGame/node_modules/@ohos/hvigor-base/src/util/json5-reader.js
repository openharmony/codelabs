"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
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
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.Json5Reader = void 0;
const json5 = __importStar(require("json5"));
const path_1 = __importDefault(require("path"));
const hvigor_log_js_1 = require("../log/hvigor-log.js");
const fs = __importStar(require("fs"));
class Json5Reader {
    /**
     * 获取json5对象
     *
     * @param json5path json5路径
     * @param encodingStr 编码方式
     */
    static getJson5Obj(json5path, encodingStr = "utf-8") {
        const text = fs.readFileSync(path_1.default.resolve(json5path), { encoding: encodingStr });
        try {
            return json5.parse(text);
        }
        catch (e) {
            Json5Reader.logger.errorMessageExit(`${json5path} is not the correct JSON/JSON5 format.`);
        }
    }
}
exports.Json5Reader = Json5Reader;
Json5Reader.logger = hvigor_log_js_1.HvigorLogger.getLogger(Json5Reader.name);
//# sourceMappingURL=json5-reader.js.map