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

import socket from '@ohos.net.socket';

const TAG = '[UdpDemo.UdpClient]'

export default class UdpClient {
    private localIp: string= ''
    private oppositeIp: string= ''
    private udp: any= null

    constructor(localIp: string, oppositeIp: string) {
        this.localIp = localIp
        this.oppositeIp = oppositeIp
        this.udp = socket.constructUDPSocketInstance();
        console.log(`${TAG} localIp is:${localIp}`);
        console.log(`${TAG} oppositeIp is:${oppositeIp}`);
    }

    bindUdp() {
        let promise = this.udp.bind({
            address: this.localIp, port: 9000, family: 1
        });
        promise.then(() => {
            console.log(`${TAG} udp bind success`);
        }).catch(err => {
            console.log(`${TAG} udp bind failed:${JSON.stringify(err)}`);
        });
    }

    sendMsg(msg: string) {
        let promise = this.udp.send({
            data: msg,
            address: {
                address: this.oppositeIp,
                port: 9000,
                family: 1
            }
        });
        promise.then(() => {
            console.log(`${TAG} udp send success:${msg}`);
        }).catch(err => {
            console.log(`${TAG} udp send fail:${JSON.stringify(err)}`);
        });
    }

    onMessage(callback) {
        this.udp.on('message', value => {
            console.log(`${TAG} udp on message:${value.message}`);
            // 收到的是ArrayBuffer 需要进行转换解析
            let dataView = new DataView(value.message)
            console.log(`${TAG} udp message length:${dataView.byteLength}`);
            let str = ""
            for (let i = 0;i < dataView.byteLength; ++i) {
                let c = String.fromCharCode(dataView.getUint8(i))
                if (c !== "\n") {
                    str += c
                }
            }
            console.log(`${TAG} udp on message array buffer:${str}`);
            callback(str)
        });
    }

    closeUdp() {
        let promise = this.udp.close();
        promise.then(() => {
            console.log(`${TAG} udp close success`);
        }).catch(err => {
            console.log(`${TAG} udp close fail:${JSON.stringify(err)}`);
        });
    }
}