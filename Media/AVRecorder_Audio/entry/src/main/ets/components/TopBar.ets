@Component
export struct TopBar {
  @Prop title: string = '';
  @BuilderParam trailing?: () => void;

  build() {
    Row() {
      Text(this.title)
        .fontSize(20)
        .fontWeight(FontWeight.Bold);
      Blank();
      if (this.trailing) {
        this.trailing();
      }
    }
    .padding({ left: 16, right: 16, top: 12, bottom: 12 })
    .backgroundColor('#f1f1f1')
    .width('100%')
  }
}
