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

export default class TcpClient{

  private port: number = 8080
  private tcpSocket: any = null
  private address: string

  constructor(address: string,port: number){
    this.address = address
    this.port = port
  }

  getTCPSocket(localIp:string,callback) {
    console.info("tcp bind localIp:"+localIp)
    if(this.tcpSocket != null) {
      callback()
      return
    }
    console.info("tcp bind next:")
    this.tcpSocket = socket.constructTCPSocketInstance();
    console.info("tcp bind next next:")
    this.tcpSocket.bind({ address: localIp, port: this.port, family: 1 },err=> {
      if (!err) {
        console.log('bind success');
      }
      console.log('bind fail');
    })
  }

  sendMag(msg: string) {
     this.tcpSocket.connect({ address: {address: this.address, port:this.port, family: 1} , timeout: 6000},err => {
       if (!err) {
         this.tcpSocket.send({
           data:msg
         },err => {
           if(err) {
             console.info('send fail');
             return;
           }
           console.info('send success');
         })
       }
     });
  }

  closeTcp() {
    this.tcpSocket.close(err => {
      if (!err) {
        this.tcpSocket = null
        return
      }
    })
  }

  setOnMessageReceivedListener(callback) {
    this.tcpSocket.on("message", function (data) {
      console.log("TCP data: " + JSON.stringify(data))
      let buffer = data.message
      let dataView = new DataView(buffer)
      console.log("TCP length " + dataView.byteLength)
      let str = ""
      for (let i = 0;i < dataView.byteLength; ++i) {
        let c = String.fromCharCode(dataView.getUint8(i))
        if (c !== "\n") {
          str += c
        }
      }
      callback(str)
    })
  }
}