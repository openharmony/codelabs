@Component
export struct WaveformView {
  @Prop samples: number[] = [];

  build() {
    Row({ space: 4 }) {
      ForEach(this.samples, (value: number, index?: number) => {
        Column() {
          Blank()
            .width(4)
            .height(6 + value * 36)
            .backgroundColor(index !== undefined && index % 2 === 0 ? '#1f6f6d' : '#f2c1a0')
            .borderRadius(3);
        }
      }, (value: number, index?: number) => `${index}`)
    }
    .padding({ left: 12, right: 12, top: 8, bottom: 8 })
    .backgroundColor('#f1ebe1')
    .borderRadius(16)
    .height(70)
    .alignItems(VerticalAlign.Bottom)
  }
}
