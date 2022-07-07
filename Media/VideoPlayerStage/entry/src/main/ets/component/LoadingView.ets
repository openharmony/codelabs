/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

@Component
export default struct LoadingView {
  private animTasker
  @State rotateAngle: number = 0

  aboutToAppear() {
    this.animTasker = setInterval(() => {
      this.rotateAngle = ((this.rotateAngle == 360) ? 0 : (this.rotateAngle + 1))
    }, 10)
  }

  aboutToDisappear() {
    clearInterval(this.animTasker)
  }

  build() {
    Stack({ alignContent: Alignment.Center }) {
      Image($r('app.media.ic_loading'))
        .width('50px')
        .height('50px')
        .objectFit(ImageFit.Fill)
        .rotate({
          z: 1,
          centerX: '50%',
          centerY: '50%',
          angle: this.rotateAngle,
        })
    }.width('100%').height('100%').backgroundColor('#00000000')
  }
}