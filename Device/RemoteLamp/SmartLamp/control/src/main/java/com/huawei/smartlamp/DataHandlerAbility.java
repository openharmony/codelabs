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

import ohos.ace.ability.AceInternalAbility;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.rpc.IRemoteObject;
import ohos.rpc.MessageOption;
import ohos.rpc.MessageParcel;
import ohos.rpc.RemoteException;
import ohos.utils.zson.ZSONObject;

import java.util.Map;

/**
 * DataHandlerAbility
 *
 * @since 2022-01-19
 */
public class DataHandlerAbility extends AceInternalAbility {
    private static final HiLogLabel LABEL_LOG = new HiLogLabel(HiLog.DEBUG, 0, "DataHandlerAbility");
    private static final String BUNDLE_NAME = "com.huawei.smartlamp.hmservice";
    private static final String ABILITY_NAME = "com.huawei.smartlamp.DataHandlerAbility";
    private static final int ACTION_MESSAGE_CODE_SUBSCRIBE = 1001;
    private static final int ACTION_MESSAGE_CODE_DATA_CHANGED = 1004;
    private static final int ACTION_MESSAGE_CODE_INIT_DEVICE_DATA = 1005;
    private final String deviceId;
    private IRemoteObject remoteObjectHandler;
    private final DeviceDataCallback deviceDataCallback = new DeviceDataCallback() {
        /**
         * onResult
         *
         * @param code errorCode
         * @param msg errorMessage
         * @param map the data to send
         */
        public void onResult(int code, String msg, Map<String, Object> map) {
            HiLog.info(LABEL_LOG, "send device data to js: " + map.toString());
            sendData(map);
        }
    };
    private BaseDeviceDataHandler deviceDataHandler;

    /**
     * IDataHandlerAbility.
     *
     * @param deviceId device id
     */
    public DataHandlerAbility(String deviceId) {
        super(BUNDLE_NAME, ABILITY_NAME);
        this.deviceId = deviceId;
    }

    /**
     * Internal ability registration.
     *
     * @param deviceId device id
     */
    public static synchronized void register(String deviceId) {
        DataHandlerAbility instance = new DataHandlerAbility(deviceId);
        instance.onRegister();
    }

    private BaseDeviceDataHandler getDeviceDataHandler() {
        return new NetworkDeviceDataHandler(deviceId, deviceDataCallback);
    }

    private void sendData(Map<String, Object> dataMap) {
        MessageParcel data = MessageParcel.obtain();
        MessageParcel reply = MessageParcel.obtain();
        MessageOption option = new MessageOption();
        data.writeString(ZSONObject.toZSONString(dataMap));
        try {
            remoteObjectHandler.sendRequest(0, data, reply, option);
        } catch (RemoteException e) {
            HiLog.error(LABEL_LOG, "failed to send data to js");
        }
        reply.reclaim();
        data.reclaim();
    }

    private boolean onRemoteRequest(int code, MessageParcel data, MessageParcel reply) {
        switch (code) {
            case ACTION_MESSAGE_CODE_SUBSCRIBE: {
                HiLog.info(LABEL_LOG, "ACTION_MESSAGE_CODE_SUBSCRIBE");
                remoteObjectHandler = data.readRemoteObject();
                break;
            }
            case ACTION_MESSAGE_CODE_INIT_DEVICE_DATA: {
                if (deviceDataHandler == null) {
                    deviceDataHandler = getDeviceDataHandler();
                }
                break;
            }
            case ACTION_MESSAGE_CODE_DATA_CHANGED: {
                String zsonStr = data.readString();
                ZSONObject zsonObj = ZSONObject.stringToZSON(zsonStr);
                for (Map.Entry<String, Object> entry : zsonObj.entrySet()) {
                    deviceDataHandler.modifyDeviceProperty(entry.getKey(), entry.getValue());
                }
                break;
            }
            default: {
                reply.writeString("service not defined");
                return false;
            }
        }
        return true;
    }

    private void onRegister() {
        this.setInternalAbilityHandler((code, data, reply, option) -> this.onRemoteRequest(code, data, reply));
    }
}
