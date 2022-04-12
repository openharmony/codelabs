"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.DefaultSyncTask = exports.Task = void 0;
const hvigor_log_js_1 = require("../log/hvigor-log.js");
/**
 * Hvigor可执行任务的基础类
 *
 * @since 2022/1/20
 */
class Task {
}
exports.Task = Task;
/**
 * Hvigor同步类型任务的基础类
 *
 * @since 2022/1/20
 */
class DefaultSyncTask extends Task {
    constructor(defaultModule) {
        super();
        this._log = hvigor_log_js_1.HvigorLogger.getLogger();
        this.registry = () => {
            return (cb) => {
                this._log.info("Sync...");
                this.doTaskAction(this.defaultModule);
                cb();
            };
        };
        this.defaultModule = defaultModule;
    }
}
exports.DefaultSyncTask = DefaultSyncTask;
//# sourceMappingURL=task.js.map