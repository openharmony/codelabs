/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import { Contact } from '../model/Contact'

/**
 * 药品 / 联系人 列表项组件
 * ArkTS 严格模式实现（无 ! / 无 any）
 */
@Component
export default struct ContactListItem {
  /**
   * 通过默认值避免 definite assignment 语法
   */
  @Prop contact: Contact = new Contact('', '')

  build() {
    Row() {
      Row() {
        /**
         * 左侧信息区域
         */
        Column() {
          Text(this.contact.name)
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .fontColor('#333333')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Text(this.contact.phone)
            .fontSize(12)
            .fontColor('#999999')
            .margin({ top: 4 })
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)

        /**
         * 右侧箭头（ArkUI 合法占位）
         */
        Text('›')
          .fontSize(22)
          .fontColor('#C0C0C0')
          .margin({ left: 8 })
      }
      .width('100%')
      .padding({
        left: 12,
        right: 12,
        top: 10,
        bottom: 10
      })
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .margin({ top: 6, bottom: 6 })
    .shadow({
      radius: 8,
      color: '#00000010',
      offsetX: 0,
      offsetY: 2
    })
  }
}
