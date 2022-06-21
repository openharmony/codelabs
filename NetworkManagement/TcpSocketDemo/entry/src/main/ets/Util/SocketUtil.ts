
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
const TAG = '[TcpDemo.index]'

export default class SocketUtil{
  private ip: any
  constructor(localIp: any){
    this.ip = localIp
  }

  resolveIP() : any{
    if (this.ip < 0 || this.ip > 0xFFFFFFFF) {
      throw (`${TAG} The number is not normal!`);
    }
    return (this.ip >>> 24) + "." + (this.ip >> 16 & 0xFF) + "." + (this.ip >> 8 & 0xFF) + "." + (this.ip & 0xFF);
  }

}