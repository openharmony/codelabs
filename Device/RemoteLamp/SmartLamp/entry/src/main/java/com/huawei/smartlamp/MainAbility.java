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

import com.huawei.ailifeability.NetConfigAbility;
import com.huawei.smartlamp.widget.controller.FormController;
import com.huawei.smartlamp.widget.controller.FormControllerManager;

import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.ability.ProviderFormInfo;
import ohos.aafwk.content.Intent;
import ohos.aafwk.content.IntentParams;
import ohos.ace.ability.AceAbility;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;

import java.util.Objects;

/**
 * MainAbility
 *
 * @since 2022-01-19
 */
public class MainAbility extends AceAbility {
    /**
     * 默认尺寸
     */
    public static final int DEFAULT_DIMENSION_2X2 = 2;
    private static final int HALF_MODEL = 3;
    private static final String DEFAULT_MODULE = "default";
    private static final String JS_MODULE = DEFAULT_MODULE;
    private static final int INVALID_FORM_ID = -1;
    private static final HiLogLabel TAG = new HiLogLabel(HiLog.DEBUG, 0x0, MainAbility.class.getName());
    private static String productId;
    private String topWidgetSlice;

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        intent.setParam("window_modal", HALF_MODEL); // 1: 半模态（窄）, 2: 全屏， 3: 半模态（宽）
        setInstanceName(JS_MODULE);

        // 读取产品信息
        Object productInfo = Objects.requireNonNull(intent.getParams()).getParam("productInfo");
        if (productInfo != null) {
            productId = (String) productInfo;
        }

        // 从intent中读取的nanSessionId用于配网，同时需要将abilityContext和sessionId注册到NetConfigAbility中
        String sessionId = intent.getStringParam("nanSessionId");
        HiLog.info(TAG, "[smart_lamp] MainAbility sessionId: " + sessionId);
        if ("null".equals(sessionId) || "".equals(sessionId)) {
            NetConfigAbility.register(this, null);
        } else {
            NetConfigAbility.register(this, sessionId);
        }

        IntentParams intentParams = intent.getParams();
        if (intentParams != null) {
            intentParams.setParam("productId", productId);
            intentParams.setParam("productName", "LED");
            intentParams.setParam("sessionId", sessionId);
            setPageParams(null, intentParams);
        }
        super.onStart(intent);
    }

    @Override
    public void onStop() {
        super.onStop();
    }

    @Override
    protected ProviderFormInfo onCreateForm(Intent intent) {
        HiLog.info(TAG, "onCreateForm");
        long formId = intent.getLongParam(AbilitySlice.PARAM_FORM_IDENTITY_KEY, INVALID_FORM_ID);
        String formName = intent.getStringParam(AbilitySlice.PARAM_FORM_NAME_KEY);
        int dimension = intent.getIntParam(AbilitySlice.PARAM_FORM_DIMENSION_KEY, DEFAULT_DIMENSION_2X2);
        HiLog.info(TAG, "onCreateForm: formId=" + formId + ",formName=" + formName);
        FormControllerManager formControllerManager = FormControllerManager.getInstance(this);
        FormController formController = formControllerManager.getController(formId);
        formController = (formController == null) ? formControllerManager.createFormController(formId,
                formName, dimension) : formController;
        if (formController == null) {
            HiLog.error(TAG, "Get null controller. formId: " + formId + ", formName: " + formName);
            return null;
        }
        return formController.bindFormData(formId);
    }

    @Override
    protected void onUpdateForm(long formId) {
        HiLog.info(TAG, "onUpdateForm");
        super.onUpdateForm(formId);
        FormControllerManager formControllerManager = FormControllerManager.getInstance(this);
        FormController formController = formControllerManager.getController(formId);
        formController.onUpdateFormData(formId);
    }

    @Override
    protected void onDeleteForm(long formId) {
        HiLog.info(TAG, "onDeleteForm: formId=" + formId);
        super.onDeleteForm(formId);
        FormControllerManager formControllerManager = FormControllerManager.getInstance(this);
        FormController formController = formControllerManager.getController(formId);
        formController.onDeleteForm(formId);
        formControllerManager.deleteFormController(formId);
    }

    @Override
    protected void onTriggerFormEvent(long formId, String message) {
        HiLog.info(TAG, "onTriggerFormEvent: " + message);
        super.onTriggerFormEvent(formId, message);
        FormControllerManager formControllerManager = FormControllerManager.getInstance(this);
        FormController formController = formControllerManager.getController(formId);
        formController.onTriggerFormEvent(formId, message);
    }

    @Override
    public void onNewIntent(Intent intent) {
        // Only response to it when starting from a service widget.
        if (intentFromWidget(intent)) {
            String newWidgetSlice = getRoutePageSlice(intent);
            if (topWidgetSlice == null || !topWidgetSlice.equals(newWidgetSlice)) {
                topWidgetSlice = newWidgetSlice;
                restart();
            }
        } else {
            if (topWidgetSlice != null) {
                topWidgetSlice = null;
                restart();
            }
        }
    }

    private boolean intentFromWidget(Intent intent) {
        long formId = intent.getLongParam(AbilitySlice.PARAM_FORM_IDENTITY_KEY, INVALID_FORM_ID);
        return formId != INVALID_FORM_ID;
    }

    private String getRoutePageSlice(Intent intent) {
        long formId = intent.getLongParam(AbilitySlice.PARAM_FORM_IDENTITY_KEY, INVALID_FORM_ID);
        if (formId == INVALID_FORM_ID) {
            return null;
        }
        FormControllerManager formControllerManager = FormControllerManager.getInstance(this);
        FormController formController = formControllerManager.getController(formId);
        if (formController == null) {
            return null;
        }
        Class<? extends AbilitySlice> clazz = formController.getRoutePageSlice(intent);
        if (clazz == null) {
            return null;
        }
        return clazz.getName();
    }
}
