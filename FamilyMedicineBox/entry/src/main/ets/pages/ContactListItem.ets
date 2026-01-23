/*
 * Copyright (c) 2025 Beijing Institute of Technology.
 *
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

        Circle({ width: 40, height: 40 })
          .fill('#EDF2F7')
        Text(this.contact.name.substring(0, 1))
          .fontSize(18)
          .fontColor('#4A90E2')
          .fontWeight(FontWeight.Bold)
      }
      .margin({ right: 15 })

      Column() {
        Row() {
          Text(this.contact.name)
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .fontColor('#333333')

          Text(this.contact.company)
            .fontSize(12)
            .fontColor(Color.White)
            .backgroundColor('#FF9500')
            .borderRadius(4)
            .padding({ left: 4, right: 4, top: 2, bottom: 2 })
            .margin({ left: 10 })
        }
        .width('100%')

        Text(`鏈夋晥鏈熻嚦: ${this.contact.phone}`)
          .fontSize(14)
          .fontColor('#999999')
          .margin({ top: 5 })
      }
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Start)
    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({ radius: 5, color: '#0000000D', offsetX: 0, offsetY: 2 })
    .margin({ bottom: 10 })
  }
}

