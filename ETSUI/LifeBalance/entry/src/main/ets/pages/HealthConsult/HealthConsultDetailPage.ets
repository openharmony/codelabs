/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import router from '@ohos.router';
import webview from '@ohos.web.webview';

/**
 * The health consult detail page.
 */
@Entry
@Component
export default struct HealthConsultDetailPage {
  @State articleFile: string = '';
  // 1. 【新增】引入全局缩放变量
  @StorageProp('appFontScale') fontScale: number = 1.0;
  controller: webview.WebviewController = new webview.WebviewController();

  onPageShow() {
    const params = router.getParams() as (Record<string, Object> | null | undefined)
    // 防止崩溃
    if (params !== null && params !== undefined) {
      this.articleFile = `article${params['id']}.html`;
    }
  }

  build() {
    Column() {
      Flex({
        justifyContent: FlexAlign.SpaceBetween,
        alignItems: ItemAlign.Center
      }) {
        SymbolGlyph($r('sys.symbol.chevron_left'))
          .fontSize(28 * this.fontScale) // 2. 【修改】返回图标变大
          .fontWeight(FontWeight.Medium)
          .padding(4)
          .onClick(e => {
            router.back() // 跳回上一页
          })

        Text('健康咨询')
          .fontSize(24 * this.fontScale) // 3. 【修改】标题文字变大
          .fontWeight(FontWeight.Bold)

        // fake icon (占位用)
        SymbolGlyph($r('sys.symbol.chevron_left'))
          .fontSize(28 * this.fontScale) // 4. 【修改】占位图标也要变大，保持对称
          .padding(4)
          .opacity(0)
      }
      .padding({
        top: 8,
        bottom: 8,
        left: 12,
        right: 12
      })
      .backgroundColor('#fff')

      // 渲染html
      // 【修改】使用 if 判断，只有当 articleFile 有值时才渲染 Web
      if (this.articleFile) {
        Web({ src: $rawfile(this.articleFile), controller: this.controller })
          .layoutWeight(1)
          .textZoomRatio(this.fontScale * 100)
      } else {
        // 加载时显示一个 Loading 占位，避免布局坍塌
        Text('加载中...')
          .layoutWeight(1)
          .width('100%')
          .textAlign(TextAlign.Center)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}