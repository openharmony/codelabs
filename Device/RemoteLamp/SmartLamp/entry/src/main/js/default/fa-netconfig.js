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
const CONSTANT = {
  BUNDLE_NAME: 'com.huawei.ailifeability',
  ABILITY_NAME: 'com.huawei.ailifeability.NetConfigAbility',
  DISCOVERY_BY_NAN_CODE: 1001,
  CONNECT_DEVICE_CODE: 1003,
  GET_WIFI_LIST_CODE: 1004,
  CONFIG_DEVICE_NET_CODE: 1006,
  DISCONNECT_DEVICE_CODE: 1007,
  SUBSCRIBE_EVENT_CODE: 1008,
  REGISTER_MSG_RECEIVE: 1010,
  REGISTER_DISCONNECT_CALLBACK: 1011
};
var EventObserver = {
  isInited: false,
  callbackMap: new Map(),
  register: function(item) {
    EventObserver.callbackMap.set(item.messageCode, item.callback);
  },
  notify: function(result) {
    if (result.messageCode && EventObserver.callbackMap.get(result.messageCode) != undefined) {
      EventObserver.callbackMap.get(result.messageCode)(result);
    } else {
    }
  }
};

function getSubscribeParam() {
  const action = {};
  action.bundleName = CONSTANT.BUNDLE_NAME;
  action.abilityName = CONSTANT.ABILITY_NAME;
  action.messageCode = CONSTANT.SUBSCRIBE_EVENT_CODE;
  action.abilityType = 1;
  action.syncOption = 0;

  return action;
}

function callAbilityFunc(callCode, argsObj, callbackFunc) {
  const action = {};
  action.bundleName = CONSTANT.BUNDLE_NAME;
  action.abilityName = CONSTANT.ABILITY_NAME;
  action.messageCode = callCode;
  action.abilityType = 1;
  action.data = argsObj;

  if (callbackFunc != undefined) {
    EventObserver.register({
      messageCode: callCode,
      callback: callbackFunc
    });
  }

  return FeatureAbility.callAbility(action);
}

function callAbility(callCode, argsObj, callbackFunc) {
  if (EventObserver.isInited) {
    return callAbilityFunc(callCode, argsObj, callbackFunc);
  } else {
    return FeatureAbility.subscribeAbilityEvent(getSubscribeParam(), (result) => {
      const ret = JSON.parse(result);
      if (ret.data) {
        EventObserver.notify(ret.data);
      } else {
      }
    }).then((result) => {
      const ret = JSON.parse(result);
      if (ret.code !== 0) {
      }

      EventObserver.isInited = true;
      return callAbilityFunc(callCode, argsObj, callbackFunc);
    });
  }
}

const FANetConfig = {
  discoveryByNAN: function(scanInfo, callbackFunc) {
    if (!(typeof scanInfo.duration === 'number' && scanInfo.duration % 1 === 0)) {
      return;
    }
    if (!(typeof scanInfo.lockTime === 'number' && scanInfo.lockTime % 1 === 0)) {
      return;
    }

    if (typeof callbackFunc !== 'function') {
      return;
    }
    return callAbility(CONSTANT.DISCOVERY_BY_NAN_CODE, {
      scanNanInfo: scanInfo
    }, callbackFunc);
  },
  connectDevice: function(connectInfo, callbackFunc) {
    const isConnectInfoNull = connectInfo == null || connectInfo == undefined;
    if (isConnectInfoNull) {
      return;
    }

    if (typeof connectInfo.targetDeviceId !== 'string') {
      return;
    }

    if (!(typeof connectInfo.type === 'number' && connectInfo.type % 1 === 0)) {
      return;
    }

    if (typeof connectInfo.pin !== 'string') {
      return;
    }

    if (typeof connectInfo.password !== 'string') {
      return;
    }

    if (typeof connectInfo.sessionId !== 'string') {
      return;
    }

    if (typeof callbackFunc !== 'function') {
      return;
    }

    return callAbility(CONSTANT.CONNECT_DEVICE_CODE, {
      connectInfo: connectInfo
    }, callbackFunc);
  },
  getWifiList: function(callbackFunc) {
    if (typeof callbackFunc !== 'function') {
      return;
    }
    return callAbility(CONSTANT.GET_WIFI_LIST_CODE, {}, callbackFunc);
  },
  configDeviceNet: function(deviceInfo, accountInfo, netConfigInfo, callbackFunc) {
    if (typeof deviceInfo !== 'string') {
      return;
    }

    if (typeof accountInfo !== 'string') {
      return;
    }

    const isNetConfigInfoNull = netConfigInfo == null || netConfigInfo == undefined;
    if (isNetConfigInfoNull) {
      return;
    }

    if (typeof netConfigInfo.ssid !== 'string') {
      return;
    }

    if (typeof netConfigInfo.isDefaultPassword !== 'boolean') {
      return;
    }

    if (!(typeof netConfigInfo.channel === 'number' && netConfigInfo.channel % 1 === 0)) {
      return;
    }

    if (typeof netConfigInfo.sessionId !== 'string') {
    }

    if (!(typeof netConfigInfo.type === 'number' && netConfigInfo.type % 1 === 0)) {
      return;
    }

    if (typeof callbackFunc !== 'function') {
      return;
    }
    return callAbility(CONSTANT.CONFIG_DEVICE_NET_CODE, {
      deviceInfo: deviceInfo,
      accountInfo: accountInfo,
      netConfigInfo: netConfigInfo
    }, callbackFunc);
  },
  disconnectDevice: function(commonInfo, callbackFunc) {
    if (commonInfo == null || commonInfo == undefined) {
      return;
    }
    if (typeof commonInfo.sessionId !== 'string') {
      return;
    }

    if (typeof callbackFunc !== 'function') {
      return;
    }
    return callAbility(CONSTANT.DISCONNECT_DEVICE_CODE, {
      commonInfo: commonInfo
    }, callbackFunc);
  },
  registerMsgReceive: function(commonInfo, callbackFunc) {
    if (commonInfo == null || commonInfo == undefined) {
      return;
    }
    if (typeof commonInfo.sessionId !== 'string') {
      return;
    }

    if (typeof callbackFunc !== 'function') {
      return;
    }
    return callAbility(CONSTANT.REGISTER_MSG_RECEIVE, {
      commonInfo: commonInfo
    }, callbackFunc);
  },
  registerDisconnectCallback: function(commonInfo, callbackFunc) {
    if (commonInfo == null || commonInfo == undefined) {
      return;
    }
    if (typeof commonInfo.sessionId !== 'string') {
      return;
    }

    if (typeof callbackFunc !== 'function') {
      return;
    }
    return callAbility(CONSTANT.REGISTER_DISCONNECT_CALLBACK, {
      commonInfo: commonInfo
    }, callbackFunc);
  }
};

export default FANetConfig;