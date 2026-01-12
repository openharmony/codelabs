//
// Created on 024 2023/10/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef application_native_hanwujiproxy_LogUtils
#define application_native_hanwujiproxy_LogUtils

#include "hilog/log.h"
#define LOG_DOMAIN 0X000001
#define LOG_TAG "Pr_411"
#ifdef __cplusplus
extern "C" {
#endif

inline const char *GetRawFileName(const char *path)
{
    char ch = '/';
    const char *start = path;
    // get the end of the string
    while (*start++) {;}
    while (--start != path && *start != ch) {;}
    return (*start == ch) ? ++start : path;
}
    
#define PRINT_OHOS_HILOG(op, fmt, ...)                                                          \
    OH_LOG_Print(LOG_APP, op, LOG_DOMAIN, LOG_TAG, "[%{public}s-(%{public}s:%{public}d)] " fmt, \
        __FUNCTION__, GetRawFileName(__FILE__), __LINE__, ##__VA_ARGS__)

#define LOG(fmt, ...) PRINT_OHOS_HILOG(LOG_INFO, fmt, ##__VA_ARGS__);
#define LOGE(fmt, ...) PRINT_OHOS_HILOG(LOG_ERROR, fmt, ##__VA_ARGS__);
#define LOGW(fmt, ...) PRINT_OHOS_HILOG(LOG_WARN, fmt, ##__VA_ARGS__);
#define LOGI(fmt, ...) PRINT_OHOS_HILOG(LOG_INFO, fmt, ##__VA_ARGS__);
#define LOGF(fmt, ...) PRINT_OHOS_HILOG(LOG_FATAL, fmt, ##__VA_ARGS__);
#define LOGD(fmt, ...) PRINT_OHOS_HILOG(LOG_DEBUG, fmt, ##__VA_ARGS__);

#ifdef __cplusplus
}
#endif

#endif //application_native_hanwujiproxy_LogUtils
