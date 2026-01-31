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
// WebPage.ets
import webview from '@ohos.web.webview';
import router from '@ohos.router';

interface WebPageParams {
  url?: string;
}

@Entry
@Component
struct WebPage {
  @State url: string = (router.getParams() as WebPageParams)?.url || 'https://www.xiachufang.com';

  build() {
    Column() {
      Web({ src: this.url, controller: new webview.WebviewController() })
        .width('100%')
        .height('100%')
    }
    .width('100%')
    .height('100%')
  }
}