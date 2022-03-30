/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import rpc from "@ohos.rpc";
import commonEvent from '@ohos.commonEvent';


class FirstServiceAbilityStub extends rpc.RemoteObject {
    constructor(des) {
        if (typeof des === 'string') {
            super(des);
        } else {
            return null;
        }
    }

    onRemoteRequest(code, data, reply, option) {
        var publishCallBack;
        if (code === 1) {
            // 从data中接收数据
            let arr = data.readStringArray();
            // 回复接收成功标识
            reply.writeInt(100);
            // 公共事件相关信息
            var params = {
                dataList: arr
            }
            var options = {
                // 公共事件的初始代码
                code: 1,
                // 公共事件的初始数据
                data: 'init data',
                // 有序公共事件
                isOrdered: true,
                bundleName: 'com.example.helloworld0218',
                parameters: params

            }
            // 发布公共事件回调
            publishCallBack = function () {

            }
            // 发布公共事件
            commonEvent.publish("publish_moveImage", options, publishCallBack);

        } else {
        }
        return true;
    }
}

export default {
    // 创建Service的时候调用，用于Service的初始化
    onStart() {

    },
    // 在Service销毁时调用。Service应通过实现此方法来清理任何资源，如关闭线程、注册的侦听器等。
    onStop() {

    },
    // 在Ability和Service连接时调用，该方法返回IRemoteObject对象，开发者可以在该回调函数中生成对应Service的IPC通信通道
    onConnect(want) {

        try {
            let value = JSON.stringify(want);

        } catch (error) {

        }
        return new FirstServiceAbilityStub("[pictureGame] first ts service stub");
    },
    // 在Ability与绑定的Service断开连接时调用
    onDisconnect(want) {

        let value = JSON.stringify(want);

    },
    // 在Service创建完成之后调用，该方法在客户端每次启动该Service时都会调用
    onCommand(want, startId) {

        let value = JSON.stringify(want);

    }
};