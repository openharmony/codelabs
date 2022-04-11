"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.ValidateUtil = void 0;
const hvigor_log_js_1 = require("../log/hvigor-log.js");
/**
 * 工程初始化时校验build-profile.json5
 *
 * @since 2022/3/10
 */
class ValidateUtil {
    static validateModule(module) {
        if (module.name === undefined) {
            this.logger.errorMessageExit(`Project level build-profile.json5 lose required property: module-name`);
        }
        if (module.srcPath === undefined) {
            this.logger.errorMessageExit(`Project level build-profile.json5 lose required property: module-srcPath`);
        }
    }
}
exports.ValidateUtil = ValidateUtil;
ValidateUtil.logger = hvigor_log_js_1.HvigorLogger.getLogger(ValidateUtil.name);
//# sourceMappingURL=validate-util.js.map