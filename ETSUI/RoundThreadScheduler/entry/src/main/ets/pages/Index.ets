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

import { SharedState } from '../model/SharedState'
import { RoundScheduler } from '../scheduler/RoundScheduler'

@Entry
@Component
struct Index {
  private shared: SharedState = new SharedState()
  private scheduler: RoundScheduler =
    new RoundScheduler(this.shared)

  private result: number = 0

  build() {
    Column() {
      Text('ArkTS 多线程安全调度示例')
        .fontSize(22)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 20 })

      Text(
        '示例说明：10 个线程并发执行 +1 操作，' +
        '共 10 轮，使用 Barrier 保证轮次同步'
      )
        .fontSize(14)
        .margin({ bottom: 20 })

      Button('启动调度')
        .onClick(() => {
          this.scheduler.start()
          this.result = this.shared.getValue()
        })
        .margin({ bottom: 20 })

      Divider()

      Column() {
        Text('执行结果')
          .fontSize(18)
          .margin({ top: 10 })

        Text(`当前共享值：${this.result}`)
          .fontSize(16)
          .margin({ top: 10 })

        Text(
          '期望结果：100'
        )
          .fontSize(14)
          .fontColor(Color.Gray)
          .margin({ top: 5 })
      }
      .margin({ top: 20 })
    }
    .width('100%')
    .height('100%')
    .padding(20)
  }
}
