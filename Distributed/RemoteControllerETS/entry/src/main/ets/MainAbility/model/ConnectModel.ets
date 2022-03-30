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
import RemoteDeviceModel from './RemoteDeviceModel.ets';
import prompt from '@system.prompt';
import featureAbility from '@ohos.ability.featureAbility';
import rpc from "@ohos.rpc";

var remoteDeviceModel = new RemoteDeviceModel;

export default class ConnectModel {
  connectedAbility;
  mRemote;

  constructor() {
  }

  // 连接远端Service
  async onConnectRemoteService(deviceId) {
    var that = this

    // 连接成功的回调
    async function onConnectCallback(element, remote) {
      console.log('[handle]connectRemoteService onConnectCallback:' + remote)
      that.mRemote = remote
    }
    // Service异常死亡的回调
    function onDisconnectCallback(element) {
      console.log('[handle]connectRemoteService onDisconnectCallback')
      prompt.showToast({
        message: "onDisconnectCallback onFailed: " + element
      });
    }
    // 连接失败的回调
    function onFailedCallback(code) {
      console.log('[handle]connectRemoteService onFailedCallback')
      prompt.showToast({
        message: "onConnectRemoteService onFailed: " + code
      });
    }

    this.connectedAbility = await featureAbility.connectAbility(
      {
        deviceId: deviceId,
        bundleName: "com.example.helloworld0218",
        abilityName: "com.example.helloworld0218.ServiceAbility",
      },
      {
        onConnect: onConnectCallback,
        onDisconnect: onDisconnectCallback,
        onFailed: onFailedCallback,
      },
    );
  }

  // 连接成功后发送消息
  // 传入code
  async sendMessageToRemoteService(dataList, code) {
    console.log('[handle]connectRemoteService sendMessageToRemoteService:')
    if (this.mRemote == null) {
      console.log('[handle]connectRemoteService mRemote == null')
      prompt.showToast({
        message: "mRemote is null",
        duration: 30000
      });
      return;
    }

    let option = new rpc.MessageOption();
    let data = new rpc.MessageParcel();
    let reply = new rpc.MessageParcel();
    data.writeString(JSON.parse(dataList));
    await this.mRemote.sendRequest(code, data, reply, option);
    let msg = reply.readInt();
    console.log('[controller]connectRemoteService msg:' + msg)
  }

  // 断开连接
  async onDisconnectService() {
    if (this.connectedAbility == null) {
      return;
    }
    await featureAbility.disconnectAbility(this.connectedAbility);
    this.connectedAbility = null;
    prompt.showToast({
      message: "onDisconnectService disconnect done"
    });
}

}
