#ifndef NATIVE_LOG_WRAPPER_H
#define NATIVE_LOG_WRAPPER_H

#include <hilog/log.h>

namespace OHOS {
namespace NATIVE {

#ifndef NATIVE_LOG_TAG
#define NATIVE_LOG_TAG "NATIVE"
#endif

inline constexpr const char *GetRawFileName(const char *path)
{
    char ch = '/';
    const char *start = path;
    // get the end of the string
    while (*start++) {;}
    while (--start != path && *start != ch) {;}
    return (*start == ch) ? ++start : path;
}

#define PRINT_OHOS_HILOG(op, fmt, ...)                                                             \
    OH_LOG_Print(LOG_APP, op, 0xFF00, NATIVE_LOG_TAG, "[%{public}s-%{public}s:%{public}d)] " fmt, \
        __FUNCTION__, OHOS::NATIVE::GetRawFileName(__FILE__), __LINE__, ##__VA_ARGS__)

#define LOGE(fmt, ...) PRINT_OHOS_HILOG(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) PRINT_OHOS_HILOG(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) PRINT_OHOS_HILOG(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOGF(fmt, ...) PRINT_OHOS_HILOG(LOG_FATAL, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) PRINT_OHOS_HILOG(LOG_DEBUG, fmt, ##__VA_ARGS__)

} // namespace NATIVE
} // namespace OHOS

#endif // NATIVE_LOG_WRAPPER_H
