/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

package com.huawei.smartlamp;

import java.util.Map;

/**
 * DeviceDataCallback
 *
 * @since 2022-01-19
 */
public interface DeviceDataCallback {
    /**
     * onResult
     *
     * @param errorCode errorCode
     * @param errorMessage errorMessage
     * @param dataMap dataMap
     */
    void onResult(int errorCode, String errorMessage, Map<String, Object> dataMap);
}
