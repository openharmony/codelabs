"use strict";
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.getLevel = exports.setLevel = exports.getConfiguration = exports.setConfiguration = void 0;
const log4js_1 = require("log4js");
let configuration = {
    "appenders": {
        "debug": {
            "type": "stdout",
            "layout": {
                "type": "pattern",
                "pattern": "> hvigor %p %c %[%m%]"
            }
        },
        "info": {
            "type": "stdout",
            "layout": {
                "type": "pattern",
                "pattern": "> hvigor %[%m%]"
            }
        },
        "wrong": {
            "type": "stderr",
            "layout": {
                "type": "pattern",
                "pattern": "> hvigor %[%p: %m%]"
            }
        },
        "just-debug": { "type": "logLevelFilter", "appender": "debug", "level": "debug", "maxLevel": "debug" },
        "just-info": { "type": "logLevelFilter", "appender": "info", "level": "info", "maxLevel": "info" },
        "just-wrong": { "type": "logLevelFilter", "appender": "wrong", "level": "warn", "maxLevel": "error" }
    },
    "categories": {
        "default": { "appenders": ["just-debug", "just-info", "just-wrong"], "level": "debug" }
    }
};
const setConfiguration = (config) => {
    configuration = config;
};
exports.setConfiguration = setConfiguration;
const getConfiguration = () => {
    return configuration;
};
exports.getConfiguration = getConfiguration;
let contextLevel = log4js_1.levels.DEBUG;
const setLevel = (level) => {
    contextLevel = level;
    const categories = configuration.categories;
    for (const category in categories) {
        categories[category].level = level.levelStr;
    }
};
exports.setLevel = setLevel;
const getLevel = () => {
    return contextLevel;
};
exports.getLevel = getLevel;
//# sourceMappingURL=default-configuration.js.map