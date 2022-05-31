// @ts-nocheck
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

import router from '@ohos.router';
import ChatMsg from '../Model/ChatMsg';
const TAG = '[TcpDemo.index]'
import TcpClient from '../Socket/TcpClient';

@Entry
@Component
export struct ChatPage {
  @State msgArr: ChatMsg[]= []
  private localIp: string = router.getParams().localIp
  private oppositeIp: string = router.getParams().oppositeIp
  private sendMsg: string = ""
  private tcpClient: TcpClient = null;
  private scroller: Scroller = new Scroller()

  onPageShow() {
    console.info("oppositeIp :"+ this.oppositeIp +"localIp:"+this.localIp)
    this.tcpClient = new TcpClient(this.oppositeIp,8080);
    this.tcpClient.getTCPSocket(this.localIp,() => {
      console.info("tcp bind success")
    })
    this.tcpClient.setOnMessageReceivedListener((str) => {
      console.log(`${TAG} udp on message array buffer:${str}`);
      this.msgArr.push(new ChatMsg(false, str))
      this.scroller.scrollPage({ next: true })
    })
  }

  aboutToDisappear() {
    this.tcpClient.closeTcp()
  }

  build() {
    Column() {
    Title({ title: `对端ip:`+this.oppositeIp })
      Scroll(this.scroller) {
        Column() {
          ForEach(this.msgArr, (item) => {
            if (item.isSend) {
              RightMessageBox({ msgStr: item.message })
            } else {
              LeftMessageBox({ msgStr: item.message })
            }
          }, item => item.toString())
        }.width('100%')
      }.layoutWeight(1)
      .width("100%")

      Divider().color(0xCCCCCC)

      Row() {
        TextInput({ placeholder: '', text: '' })
          .height(60)
          .fontSize(30)
          .width('70%')
          .margin(10)
          .onChange((value: string) => {
            this.sendMsg = value
          })

        Button("发送")
          .height(60)
          .width(100)
          .margin(10)
          .fontSize(30)
          .onClick(() => {
            this.msgArr.push(new ChatMsg(true, this.sendMsg))
            this.scroller.scrollPage({ next: true })
            this.tcpClient.sendMag(this.sendMsg)
          })
      }
    }.height("100%")
    .width("100%")
  }
}

@Component
struct Title {
  private title: string
  build() {
    Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text(this.title).fontSize(30).fontColor("#fdfdfd")
    }.height(80).backgroundColor("#333534")
  }
}

@Component
struct LeftMessageBox {
  private msgStr: string
  build() {
    Row() {
      Image($r("app.media.kang")).width(100).height(100).margin({ left: 10, right: 10 })
      Text(this.msgStr)
        .fontSize(30)
        .backgroundColor('#ff78dd4d')
        .padding(15)
        .borderRadius(10)
    }
    .width('100%')
    .margin(20)
    .alignItems(VerticalAlign.Center)
  }
}

@Component
struct RightMessageBox {
  private msgStr: string
  build() {
    Row() {
      Image($r("app.media.xiong")).width(100).height(100).margin({ left: 10, right: 10 })
      Text(this.msgStr)
        .fontSize(30)
        .backgroundColor('#ff78dd4d')
        .padding(15)
        .borderRadius(10)
    }
    .width('100%')
    .margin(20)
    .alignItems(VerticalAlign.Center)
    .direction(Direction.Rtl)
  }
}