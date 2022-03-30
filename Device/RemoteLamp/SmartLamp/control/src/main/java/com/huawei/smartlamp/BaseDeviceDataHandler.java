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
 * BaseDeviceDataHandler
 *
 * @since 2022-01-19
 */
public abstract class BaseDeviceDataHandler {
    // the id of device
    String deviceId;

    // used to send data
    private final DeviceDataCallback deviceDataCallback;

    BaseDeviceDataHandler(String deviceId, DeviceDataCallback deviceDataCallback) {
        this.deviceId = deviceId;
        this.deviceDataCallback = deviceDataCallback;
    }

    /**
     * init profile data when first connecting device
     *
     * @param dataMap the data to send
     */
    final void initProfileData(Map<String, Object> dataMap) {
        this.deviceDataCallback.onResult(0, "", dataMap);
    }

    /**
     * modify device property from UI
     *
     * @param key key of data
     * @param value value of data
     */
    public abstract void modifyDeviceProperty(String key, Object value);

    /**
     * data changed from device
     *
     * @param dataMap the data to send
     */
    void onDeviceDataChange(Map<String, Object> dataMap) {
        this.deviceDataCallback.onResult(0, "", dataMap);
    }

    abstract void getData();
}
