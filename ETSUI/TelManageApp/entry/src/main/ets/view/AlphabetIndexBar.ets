/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * 组件功能：联系人字母索引条。
 * 作用：展示 A-Z/# 列表，并支持点击/滑动选择字母。
 * 输出：通过回调通知外部选中字母。
 */

@Component
export struct AlphabetIndexBar {
  @Prop letters: string[] = [];
  @Link selectedLetter: string;
  @State barHeight: number = 0;
  @State isTouching: boolean = false;
  @State currentLetter: string = '';

  /**
   * 构建组件 UI。
   * 输入：无。
   * 输出：无，渲染字母索引条。
   */
  build(): void {
    Stack() {
      this.buildLetterBar();
      this.buildFloatingTip();
    }
    .width(70)
    .height('100%')
  }

  /**
   * 构建字母栏主体。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildLetterBar(): void {
    Column() {
      this.buildIndexLabel();
      this.buildLetterColumn();
      this.buildIndexDot();
    }
    .width(32)
    .padding({ top: 8, bottom: 8 })
    .backgroundColor('#ffffff')
    .borderRadius(16)
    .shadow({
      radius: 2,
      color: '#12000000',
      offsetX: 0,
      offsetY: 1
    })
    .onAreaChange((_oldValue, newValue) => {
      this.barHeight = this.normalizeLength(newValue.height);
    })
    .onTouch((event) => {
      this.handleTouchEvent(event);
    })
  }

  /**
   * 构建悬浮提示。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildFloatingTip(): void {
    if (this.isTouching && this.currentLetter) {
      Text(this.currentLetter)
        .width(64)
        .height(64)
        .fontSize(30)
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Center)
        .lineHeight(64)
        .backgroundColor('#00000099')
        .fontColor(Color.White)
        .borderRadius(12)
    }
  }

  /**
   * 构建索引标记文本。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildIndexLabel(): void {
    Text('索引')
      .fontSize(9)
      .fontColor('#999')
      .margin({ bottom: 4 })
  }

  /**
   * 构建底部装饰点。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildIndexDot(): void {
    Text('•')
      .fontSize(12)
      .fontColor('#bbb')
      .margin({ top: 4 })
  }

  /**
   * 构建字母列表 UI。
   * 输入：无。
   * 输出：无。
   */
  @Builder
  private buildLetterColumn(): void {
    Column() {
      ForEach(this.letters, (letter: string) => {
        Text(letter)
          .width(24)
          .height(18)
          .fontSize(this.isLetterSelected(letter) ? 12 : 10)
          .fontWeight(this.isLetterSelected(letter) ? FontWeight.Bold : FontWeight.Normal)
          .textAlign(TextAlign.Center)
          .lineHeight(18)
          .fontColor(this.isLetterSelected(letter) ? '#ffffff' : '#666')
          .backgroundColor(this.isLetterSelected(letter) ? '#007DFF' : 'transparent')
          .borderRadius(9)
          .margin({ bottom: 2 })
          .onClick(() => {
            this.selectLetter(letter);
          })
      }, (letter: string) => letter)
    }
    .width('100%')
    .alignItems(HorizontalAlign.Center)
  }

  /**
   * 处理触摸事件并更新选中字母。
   * 输入：event 触摸事件。
   * 输出：无。
   */
  private handleTouchEvent(event: TouchEvent): void {
    if (this.letters.length === 0) {
      return;
    }

    const typeValue = this.normalizeTouchType(event.type);
    if (typeValue === TouchKind.Down || typeValue === TouchKind.Move) {
      this.isTouching = true;
      this.updateLetterByPosition(event.touches[0].y);
      return;
    }

    if (typeValue === TouchKind.Up || typeValue === TouchKind.Cancel) {
      this.isTouching = false;
    }
  }

  /**
   * 根据触摸位置计算选中字母。
   * 输入：offsetY 触摸位置 Y。
   * 输出：无。
   */
  private updateLetterByPosition(offsetY: number): void {
    if (this.barHeight <= 0) {
      return;
    }

    const itemHeight = this.barHeight / this.letters.length;
    const rawIndex = Math.floor(offsetY / itemHeight);
    const index = Math.min(Math.max(rawIndex, 0), this.letters.length - 1);
    const letter = this.letters[index];

    if (letter !== this.currentLetter) {
      this.selectLetter(letter);
    }
  }

  /**
   * 设置当前选中字母。
   * 输入：letter 字母。
   * 输出：无。
   */
  private selectLetter(letter: string): void {
    this.currentLetter = letter;
    this.selectedLetter = letter;
  }

  /**
   * 判断字母是否选中。
   * 输入：letter 字母。
   * 输出：是否选中。
   */
  private isLetterSelected(letter: string): boolean {
    return this.selectedLetter === letter;
  }

  /**
   * 归一化长度为数字。
   * 输入：lengthValue 长度值。
   * 输出：数值型高度。
   */
  private normalizeLength(lengthValue: Length): number {
    if (typeof lengthValue === 'number') {
      return lengthValue;
    }
    return Number.parseFloat(String(lengthValue));
  }

  /**
   * 归一化触摸类型为数字。
   * 输入：typeValue 触摸类型。
   * 输出：数值类型。
   */
  private normalizeTouchType(typeValue: TouchType): number {
    return Number(typeValue);
  }
}

enum TouchKind {
  Down = 0,
  Up = 1,
  Move = 2,
  Cancel = 3
}

