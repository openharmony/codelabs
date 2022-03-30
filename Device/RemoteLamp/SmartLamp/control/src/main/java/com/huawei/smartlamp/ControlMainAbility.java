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

import ohos.aafwk.content.Intent;
import ohos.ace.ability.AceAbility;
import ohos.utils.zson.ZSONObject;

/**
 * ControlMainAbility
 *
 * @since 2022-01-19
 */
public class ControlMainAbility extends AceAbility {
    private static final String START_PARAMS = "__startParams";
    private static final String SESSION_ID = "session_id";
    private static final String FA_DEVICE_ID = "device_id";

    @Override
    public void onStart(Intent intent) {
        if (intent.getParams().keySet().contains(START_PARAMS)) {
            ZSONObject zsonObject = ZSONObject.stringToZSON((String) intent.getParams().getParam(START_PARAMS));
            String sessionId = zsonObject.getString(SESSION_ID);
            NetworkDeviceDataHandler.setSessionId(sessionId);
        }
        intent.setParam("window_modal", 1);

        String deviceId = intent.getStringParam(FA_DEVICE_ID);

        DataHandlerAbility.register(deviceId);
        super.onStart(intent);
    }
}
