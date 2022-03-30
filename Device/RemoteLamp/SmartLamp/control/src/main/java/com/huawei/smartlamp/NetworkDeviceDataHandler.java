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

import com.huawei.ailifeability.netconfig.api.NetConfigApi;
import com.huawei.ailifeability.netconfig.model.CommonInfo;

import ohos.eventhandler.EventHandler;
import ohos.eventhandler.EventRunner;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;

import java.util.HashMap;
import java.util.Map;

/**
 * NetworkDeviceDataHandler
 *
 * @since 2022-01-19
 */
public class NetworkDeviceDataHandler extends BaseDeviceDataHandler {
    private static final HiLogLabel LABEL_LOG = new HiLogLabel(HiLog.DEBUG, 0, "MessageDataHandler");
    private static String sessionId = null;
    private final DeviceDataModel dataModel;
    private final EventHandler mainHandler;
    private final Map<String, Object> dataMap = new HashMap<>(1);

    NetworkDeviceDataHandler(String deviceId, DeviceDataCallback deviceDataCallback) {
        super(deviceId, deviceDataCallback);
        dataModel = new DeviceDataModel() {
            @Override
            public Map<String, Object> recoverData() {
                dataMap.put("switch/on", 0);
                return dataMap;
            }

            @Override
            public void modifyProperty(String key, Object value) {
                Map<String, Object> tmpMap = new HashMap<>(1);
                tmpMap.put(key, value);
                extracted(key, (int) value, tmpMap);
            }
        };
        mainHandler = new EventHandler(EventRunner.getMainEventRunner());
        mainHandler.postTask(() -> initProfileData(dataModel.recoverData()));
    }

    private void extracted(String key, int value, Map<String, Object> tmpMap) {
        if ("switch/on".equals(key)) {
            if (value == 0) {
                sendMessage("010000", tmpMap);
                tmpMap.put("shutdown/mode", 0);
            } else {
                sendMessage("010100", tmpMap);
            }
        }
    }

    public static void setSessionId(String id) {
        sessionId = id;
    }

    private void sendMessage(String message, Map<String, Object> tmpMap) {
        CommonInfo commonInfo = new CommonInfo();
        commonInfo.setSessionId(sessionId);
        HiLog.error(LABEL_LOG, "sessionId " + sessionId);
        NetConfigApi.getInstance().sendMessage(commonInfo, message, (code, controlMessage, str) -> {
            if (code == 0) {
                HiLog.info(LABEL_LOG, "send message success " + message);
                dataMap.putAll(tmpMap);
                mainHandler.postTask(() -> onDeviceDataChange(dataMap));
            } else {
                HiLog.error(LABEL_LOG, "send message fail code = "
                        + code + " control message = " + controlMessage);
            }
        });
    }

    @Override
    public void modifyDeviceProperty(String key, Object value) {
        dataModel.modifyProperty(key, value);
    }

    @Override
    void getData() {
        onDeviceDataChange(dataMap);
    }

    /**
     * DeviceDataModel
     *
     * @since 2022-01-19
     */
    private interface DeviceDataModel {
        Map<String, Object> recoverData();

        void modifyProperty(String key, Object value);
    }
}
