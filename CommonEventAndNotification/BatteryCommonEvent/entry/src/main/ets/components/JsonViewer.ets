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
import { JsonUtil } from '../utils/JsonUtil';

interface PlainObject {}

@Component
export struct JsonViewer {
  obj: PlainObject = {};
  maxLines: number = 40;
  expanded: boolean = false;

  private getText(): string {
    const text = JsonUtil.safeStringify(this.obj as Object, 2);
    if (this.expanded) {return text};
    const lines = text.split('\n');
    if (lines.length <= this.maxLines) {return text};
    return lines.slice(0, this.maxLines).join('\n') + '\n...';
  }

  build() {
    Column({ space: 8 }) {
      Row() {
        Text('Raw JSON').fontSize(14).fontWeight(FontWeight.Medium).layoutWeight(1);
        Text(this.expanded ? 'Collapse' : 'Expand')
          .fontSize(12)
          .opacity(0.75)
          .onClick(() => { this.expanded = !this.expanded; });
      }
      Scroll() {
        Text(this.getText())
          .fontSize(12)
          .fontFamily('monospace')
          .lineHeight(18);
      }
      .height(this.expanded ? 360 : 220)
      .backgroundColor(0x0F000000)
      .borderRadius(10)
      .padding(12);
    }
  }
}
