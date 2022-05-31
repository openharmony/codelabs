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

// @ts-nocheck
import http from '@ohos.net.http';
@Entry
@Component
struct Index {
  @State msgArr: string[] = ['1-您好！这是使用基于TS扩展的类Web开发范式 实现的一个人工智能聊天窗口，有什么想和我聊的吗？',
  ]
  private sendMsg: string = ""
  scroller: Scroller = new Scroller()
  build() {
    Column() {
      Title({ title: "智能聊天机器人" })
      Scroll(this.scroller) {
        Column() {
          ForEach(this.msgArr, (item) => {
            DialogBox({ chatMsg: item })
          }, item => item)
        }.width('100%')
      }.layoutWeight(1)
      Divider().color(0xCCCCCC)
      Row() {
        TextInput({ placeholder: '', text: '' })
          .height(60)
          .fontSize(30)
          .margin(10)
          .width('80%')
          .onChange((value: string) => {
            this.sendMsg = value
          })
        Button("发送").height(50)
          .width(100)
          .margin(10)
          .onClick(() => {
            this.msgArr.push("2-" + this.sendMsg)
            this.scroller.scrollPage({ next: true })
            let sendMsg = "http://******" + this.sendMsg // 用户可根据需要，选择合适的智能机器人聊天API进行测试
            this.sendMsg = ""
            let httpRequest = http.createHttp();
            httpRequest.request(sendMsg,
              {
                method: http.RequestMethod.GET,
                header: {
                  'Content-Type': 'application/json'
                },
                readTimeout: 60000,
                connectTimeout: 60000
              }, (err, data) => {
                if (!err) {
                  console.info('httpRequest:' + data.result);
                  this.msgArr.push("1-" + JSON.parse(data.result)
                  .content)
                  this.scroller.scrollPage({ next: true })
                } else {
                  console.info('httpRequest:' + err.data);
                }
              });
          })
      }
    }.height("100%")
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
struct DialogBox {
  private chatMsg: string

  build() {
    Row() {
      if (this.chatMsg.search("1-") != -1) {
        LeftDialogBox({ imageSrc: "person(0).jpg", chatMsg: this.chatMsg.replace("1-", "") })
      } else if (this.chatMsg.search("2-") != -1) {
        RightDialogBox({ imageSrc: "person(1).png", chatMsg: this.chatMsg.replace("2-", "") })
      }
    }
  }
}

@Component
struct LeftDialogBox {
  private imageSrc: string
  private chatMsg: string

  build() {
    Row() {
      Image($rawfile(this.imageSrc)).width(80).height(80).margin({ left: 10, right: 10 })
      Text(this.chatMsg)
        .fontSize(25)
        .backgroundColor("#d7f6f3f3")
        .padding(10)
        .borderRadius(10)
    }
    .width('100%')
    .padding({ top: 20, bottom: 20 , right: 280})
    .alignItems(VerticalAlign.Top)
  }
}

@Component
struct RightDialogBox {
  private imageSrc: string
  private chatMsg: string

  build() {
    Row() {
      Image($rawfile(this.imageSrc)).width(80).height(80).margin({ left: 10, right: 10 })
      Text(this.chatMsg)
        .fontSize(25)
        .backgroundColor("#d7f6f3f3")
        .padding(10)
        .borderRadius(10)
    }
    .width('100%')
    .padding({ top: 20, bottom: 20 , left: 280})
    .alignItems(VerticalAlign.Top)
    .direction(Direction.Rtl)
  }
}