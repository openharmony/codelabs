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

package com.huawei.smartlamp.widget.controller;

import ohos.app.Context;
import ohos.data.DatabaseHelper;
import ohos.data.preferences.Preferences;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.utils.zson.ZSONObject;

import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

/**
 * FormControllerManager
 *
 * @since 2022-01-19
 */
public class FormControllerManager {
    private static final HiLogLabel TAG = new HiLogLabel(HiLog.DEBUG, 0x0, FormControllerManager.class.getName());
    private static final String PACKAGE_PATH = "com.huawei.smartlamp.widget";
    private static final String SHARED_SP_NAME = "form_info_sp.xml";
    private static final String FORM_NAME = "formName";
    private static final String DIMENSION = "dimension";
    private static final int ACS_NUM = 32;
    private static final int DEFAULT_CAPACITY = 32;
    private static volatile FormControllerManager managerInstance = null;
    private final Map<Long, FormController> controllerHashMap = new HashMap<>(DEFAULT_CAPACITY);

    private final Context context;

    private final Preferences preferences;

    /**
     * Constructor with context.
     *
     * @param context instance of Context.
     */
    private FormControllerManager(Context context) {
        this.context = context;
        DatabaseHelper databaseHelper = new DatabaseHelper(this.context.getApplicationContext());
        preferences = databaseHelper.getPreferences(SHARED_SP_NAME);
    }

    /**
     * Singleton mode.
     *
     * @param context instance of Context.
     * @return FormControllerManager instance.
     */
    public static synchronized FormControllerManager getInstance(Context context) {
        if (managerInstance == null) {
            synchronized (FormControllerManager.class) {
                if (managerInstance == null) {
                    managerInstance = new FormControllerManager(context);
                }
            }
        }
        return managerInstance;
    }

    /**
     * Save the form id and form name.
     *
     * @param formId form id.
     * @param formName form name.
     * @param dimension form dimension
     * @return FormController form controller
     */
    public FormController createFormController(long formId, String formName, int dimension) {
        synchronized (controllerHashMap) {
            if (formId < 0 || formName.isEmpty()) {
                return null;
            }
            HiLog.info(TAG,
                    "saveFormId() formId: " + formId + ", formName: " + formName + ", preferences: " + preferences);
            if (preferences != null) {
                ZSONObject formObj = new ZSONObject();
                formObj.put(FORM_NAME, formName);
                formObj.put(DIMENSION, dimension);
                preferences.putString(Long.toString(formId), ZSONObject.toZSONString(formObj));
                preferences.flushSync();
            }

            // Create controller instance.
            FormController controller = newInstance(formName, dimension, context);

            // Cache the controller.
            if (controller != null) {
                if (!controllerHashMap.containsKey(formId)) {
                    controllerHashMap.put(formId, controller);
                }
            }

            return controller;
        }
    }

    /**
     * Get the form controller instance.
     *
     * @param formId form id.
     * @return the instance of form controller.
     */
    public FormController getController(long formId) {
        synchronized (controllerHashMap) {
            if (controllerHashMap.containsKey(formId)) {
                return controllerHashMap.get(formId);
            }
            Map<String, ?> forms = preferences.getAll();
            String formIdString = Long.toString(formId);
            if (forms.containsKey(formIdString)) {
                ZSONObject formObj = ZSONObject.stringToZSON((String) forms.get(formIdString));
                String formName = formObj.getString(FORM_NAME);
                int dimension = formObj.getIntValue(DIMENSION);
                FormController controller = newInstance(formName, dimension, context);
                controllerHashMap.put(formId, controller);
            }
            return controllerHashMap.get(formId);
        }
    }

    private FormController newInstance(String formName, int dimension, Context abilityContext) {
        FormController ctrInstance = null;
        if (formName == null || formName.isEmpty()) {
            HiLog.error(TAG, "newInstance() get empty form name");
            return ctrInstance;
        }
        try {
            String className = PACKAGE_PATH + "." + formName.toLowerCase(Locale.ROOT) + "."
                    + getClassNameByFormName(formName);
            Class<?> clazz = Class.forName(className);
            if (clazz != null) {
                Object controllerInstance = clazz.getConstructor(Context.class, String.class, Integer.class)
                        .newInstance(abilityContext, formName, dimension);
                if (controllerInstance instanceof FormController) {
                    ctrInstance = (FormController) controllerInstance;
                }
            }
        } catch (NoSuchMethodException | InstantiationException | IllegalArgumentException | InvocationTargetException
                | IllegalAccessException | ClassNotFoundException | SecurityException exception) {
            HiLog.error(TAG, "newInstance() get exception: " + exception.getMessage());
        }
        return ctrInstance;
    }

    /**
     * Delete a form controller
     *
     * @param formId form id
     */
    public void deleteFormController(long formId) {
        synchronized (controllerHashMap) {
            preferences.delete(Long.toString(formId));
            preferences.flushSync();
            controllerHashMap.remove(formId);
        }
    }

    private String getClassNameByFormName(String formName) {
        String[] strings = formName.split("_");
        StringBuilder result = new StringBuilder(DEFAULT_CAPACITY);
        for (String string : strings) {
            result.append(string);
        }
        char[] chars = result.toString().toCharArray();
        chars[0] = (chars[0] >= 'a' && chars[0] <= 'z')
                ? (char) (chars[0] - ACS_NUM) : chars[0];
        return String.copyValueOf(chars) + "Impl";
    }
}
