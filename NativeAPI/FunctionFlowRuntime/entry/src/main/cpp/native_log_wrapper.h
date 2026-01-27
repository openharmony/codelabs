/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef APPLICATION_NATIVE_HANWUJIPROXY_LOGUTILS
#define APPLICATION_NATIVE_HANWUJIPROXY_LOGUTILS
#include "hilog/log.h"
#define LOG_DOMAIN 0X000001
#define LOG_TAG "PR_FFRT"
#ifdef __cplusplus
extern "C" {
#endif

inline const char *GetRawFileName(const char *path) {
    char ch = '/';
    const char *start = path;
    // get end of the string
    while (*start) {
        start++;
    }
    while (start > path && *start != ch) {
        start--;
    }
    return (*start == ch) ? start : path;
}

#define PRINT_OHOS_HILOG(op, fmt, ...)                                                                                 \
    OH_LOG_Print(LOG_APP, op, LOG_DOMAIN, LOG_TAG, "[%{public}s-(%{public}s:%{public}d)] " fmt, __FUNCTION__,          \
                 GetRawFileName(__FILE__), __LINE__, ##__VA_ARGS__)

#define LOG(fmt, ...) PRINT_OHOS_HILOG(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) PRINT_OHOS_HILOG(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) PRINT_OHOS_HILOG(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) PRINT_OHOS_HILOG(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOGF(fmt, ...) PRINT_OHOS_HILOG(LOG_FATAL, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) PRINT_OHOS_HILOG(LOG_DEBUG, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // application_native_hanwujiproxy_LogUtils
