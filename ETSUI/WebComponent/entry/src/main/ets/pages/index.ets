/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

@Entry
@Component
struct Index {
  // web组件控制器可以控制Web组件各种行为
  webController: WebController = new WebController()

  build() {
    Row() {
      Column() {
        Text("Web组件外")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        Button("点击抽奖")
          .fontSize(40)
          .height("8%")
          .width("30%")
          .onClick(() => {
            this.webController.runJavaScript({ script: 'start()' });
            console.log("点击抽奖按钮")
          })
        Text("-----------------------------")
          .fontSize(50)
        Text("Web组件内")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        // web组件加载本地H5
        Web({ src: $rawfile('index.html'), controller: this.webController })
          // 设置是否开启通过$rawfile(filepath/filename)访问应用中rawfile路径的文件， 默认启用。
          .fileAccess(true)
          // 设置是否允许自动加载图片资源，默认允许。
          .imageAccess(true)
          // 设置是否允许执行JavaScript脚本，默认允许执行。
          .javaScriptAccess(true)
        .backgroundColor(Color.Pink)
      }
      .width('100%')
    }
    .height('100%')
  }
}