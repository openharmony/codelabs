@Component
export struct StatCard {
  @Prop title: string = '';
  @Prop value: string = '--';
  subtitle: string = '';
  onTap?: () => void;

  build() {
    Column() {
      Text(this.title)
        .fontSize(12)
        .opacity(0.75);

      Text(this.value)
        .fontSize(22)
        .fontWeight(FontWeight.Medium)
        .margin({ top: 4 });

      if (this.subtitle) {
        Text(this.subtitle)
          .fontSize(11)
          .opacity(0.65)
          .margin({ top: 2 });
      }
    }
    .padding(12)
    .backgroundColor(0x0D000000)
    .borderRadius(12)
    .onClick(() => {
      if (this.onTap) {this.onTap()};
    });
  }
}
