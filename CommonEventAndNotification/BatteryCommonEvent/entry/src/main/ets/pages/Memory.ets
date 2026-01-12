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
import router from '@ohos.router'
import hidebug from '@ohos.hidebug'

function formatKb(kb: number): string {
  if (kb <= 0) {
    return '0B'
  }
  const bytes: number = kb * 1024
  if (bytes < 1024) {
    return `${bytes}B`
  }
  const kb2: number = bytes / 1024
  if (kb2 < 1024) {
    return `${kb2.toFixed(1)}KB`
  }
  const mb: number = kb2 / 1024
  if (mb < 1024) {
    return `${mb.toFixed(1)}MB`
  }
  const gb: number = mb / 1024
  return `${gb.toFixed(2)}GB`
}

@Entry
@Component
export struct Memory {
  @State pssKb: number = 0
  @State vssKb: number = 0

  aboutToAppear(): void {
    this.refresh()
  }

  private refresh(): void {
    const pss: bigint = hidebug.getPss()
    const vss: bigint = hidebug.getVss()
    this.pssKb = Number(pss)
    this.vssKb = Number(vss)
  }

  private back(): void {
    router.back()
  }

  @Builder
  private Card(title: string, value: string) {
    Column({ space: 6 }) {
      Text(title)
        .fontSize(14)
        .fontWeight(FontWeight.Medium)

      Text(value)
        .fontSize(22)
        .fontWeight(FontWeight.Medium)
    }
    .width('100%')
    .padding(12)
    .backgroundColor(0x0D000000)
    .borderRadius(12)
  }

  build() {
    Column() {

      Row() {
        Button('返回')
        .onClick(() => this.back())

        Text('内存')
          .layoutWeight(1)
          .textAlign(TextAlign.Center)
          .fontSize(18)
          .fontWeight(FontWeight.Medium)

        Button('刷新')
        .onClick(() => this.refresh())
      }
      .width('100%')
      .padding({ left: 16, right: 16, top: 16, bottom: 10 })


      Row({ space: 12 }) {
        Column() {
          this.Card('实际占用（PSS�?, formatKb(this.pssKb))
        }
        .layoutWeight(1)

        Column() {
          this.Card('虚拟占用（VSS�?, formatKb(this.vssKb))
        }
        .layoutWeight(1)
      }
      .width('100%')
      .padding({ left: 16, right: 16, top: 8 })

      Blank().layoutWeight(1)
    }
    .width('100%')
    .height('100%')
  }
}
