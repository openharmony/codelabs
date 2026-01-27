@Component
export struct SportControlButton {
  @Prop isRunning: boolean;
  @Prop isPaused: boolean;
  onStart: () => void = () => {};
  onPause: () => void = () => {};
  onResume: () => void = () => {};
  onStop: () => void = () => {};

  build() {
    Row() {
      if (!this.isRunning) {
        this.Button('开始运动', '#4ECDC4', this.onStart)
      } else if (this.isPaused) {
        this.Button('继续运动', '#7AC36C', this.onResume)
        this.Button('结束运动', '#FF6B6B', this.onStop)
      } else {
        this.Button('暂停运动', '#F5BE14', this.onPause)
        this.Button('结束运动', '#FF6B6B', this.onStop)
      }
    }
    .width('100%')
    .justifyContent(FlexAlign.SpaceEvenly)
  }

  @Builder
  Button(text: string, color: string, onClick: () => void) {
    Text(text)
      .fontSize(16)
      .fontWeight(FontWeight.Bold)
      .fontColor('#FFFFFF')
      .width(120)
      .height(48)
      .textAlign(TextAlign.Center)
      .backgroundColor(color)
      .borderRadius(24)
      .onClick(onClick)
  }
}
