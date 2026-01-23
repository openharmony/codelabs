/*
 * Copyright (c) 2026 Beijing Institute of Technology.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSEt-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

  @Prop color: string = '#007DFF';
  // 鐐瑰嚮浜嬩欢
  @Prop onClick: () => void = () => {};
  // 鏄惁鏄剧ず
  @Prop visible: boolean = true;
  // 鏄惁鎵╁睍锛堟樉绀烘枃瀛楋級
  @State isExtended: boolean = false;

  // 鍔ㄧ敾鎺у埗鍣?  private animator: Animator = new Animator();

  build() {
    if (!this.visible) {
      // 濡傛灉涓嶆樉绀猴紝杩斿洖绌?      Blank();
    }

    // 鏍规嵁浣嶇疆璁剧疆瀵归綈鏂瑰紡
    let align: HorizontalAlign = HorizontalAlign.End;
    if (this.position === 'bottomCenter') {
      align = HorizontalAlign.Center;
    } else if (this.position === 'bottomLeft') {
      align = HorizontalAlign.Start;
    }

    Column() {
      // 涓绘寜閽?      Row() {
        // 鎸夐挳鍐呭
        Row() {
          // 鍥炬爣
          Image(this.icon)
            .width(24)
            .height(24)
            .fillColor(Color.White)

          // 鏂囧瓧锛堜粎鍦ㄦ墿灞曟椂鏄剧ず锛?          if (this.isExtended && this.label) {
            Text(this.label)
              .fontSize(16)
              .fontColor(Color.White)
              .margin({ left: 8 })
              .opacity(this.isExtended ? 1 : 0)
              .animation({
                duration: 200,
                curve: Curve.EaseInOut
              })
          }
        }
        .padding({ left: 16, right: 16, top: 12, bottom: 12 })
        .justifyContent(FlexAlign.Center)
      }
      .backgroundColor(this.color)
      .borderRadius(28) // 鍦嗗舰鎸夐挳
      .shadow({ radius: 4, color: '#40000000', offsetX: 0, offsetY: 2 })
      .onClick(() => {
        console.log('[FloatingActionButton] 鎸夐挳琚偣鍑?);
        this.onClick();

        // 鐐瑰嚮鍔ㄧ敾
        this.animateClick();
      })
      .onHover((isHover: boolean) => {
        // 榧犳爣鎮仠鏁堟灉
        if (isHover) {
          this.isExtended = true;
        } else {
          this.isExtended = false;
        }
      })
      .animation({
        duration: 300,
        curve: Curve.EaseOut
      })

      // 宸ュ叿鎻愮ず锛堝湪闈炴墿灞曟ā寮忎笅鏄剧ず锛?      if (!this.isExtended && this.label) {
        Text(this.label)
          .fontSize(12)
          .fontColor(Color.White)
          .backgroundColor('#80000000')
          .padding({ left: 8, right: 8, top: 4, bottom: 4 })
          .borderRadius(4)
          .margin({ top: 8 })
          .opacity(0.9)
      }
    }
    .position({ x: 0, y: 0 })
    .margin({
      bottom: this.getBottomMargin(),
      right: this.getRightMargin(),
      left: this.getLeftMargin()
    })
    .alignItems(align)
    .zIndex(1000) // 纭繚鍦ㄦ渶涓婂眰
  }

  // 鑾峰彇搴曢儴杈硅窛
  private getBottomMargin(): number {
    return 24;
  }

  // 鑾峰彇鍙充晶杈硅窛
  private getRightMargin(): number {
    if (this.position === 'bottomRight') {
      return 16;
    }
    return 0;
  }

  // 鑾峰彇宸︿晶杈硅窛
  private getLeftMargin(): number {
    if (this.position === 'bottomLeft') {
      return 16;
    }
    return 0;
  }

  // 鐐瑰嚮鍔ㄧ敾
  private animateClick(): void {
    // 鍒涘缓缂╂斁鍔ㄧ敾
    this.animator = {
      duration: 150,
      curve: Curve.EaseInOut,
      iterations: 1,
      playMode: PlayMode.Normal,
      onFinish: () => {
        // 鍔ㄧ敾瀹屾垚
      }
    };

    // 杩欓噷搴旇鐢ㄥ姩鐢籄PI瀹炵幇锛屼絾ArkUI澹版槑寮忚娉曚腑鐩存帴鍦ㄧ粍浠朵笂璁剧疆鍔ㄧ敾
    // 瀹為檯椤圭洰涓彲浠ヤ娇鐢ㄦ樉寮忓姩鐢籄PI
  }

  // 鏄剧ず鎸夐挳
  show(): void {
    this.visible = true;
  }

  // 闅愯棌鎸夐挳
  hide(): void {
    this.visible = false;
  }

  // 鍒囨崲鎸夐挳鐘舵€?  toggle(): void {
    this.visible = !this.visible;
  }
}




