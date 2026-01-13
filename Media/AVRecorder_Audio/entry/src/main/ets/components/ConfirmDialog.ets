@ComponentV2
export struct ConfirmDialog {
  @Param title: string = 'Confirm';
  @Param message: string = '';
  @Event onConfirm?: () => void;
  @Event onCancel?: () => void;

  build() {
    Column() {
      Text(this.title)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .margin({ bottom: 8 });
      Text(this.message)
        .fontSize(14)
        .fontColor('#666666')
        .margin({ bottom: 16 });
      Row({ space: 12 }) {
        Button('Cancel')
          .onClick(() => this.onCancel && this.onCancel());
        Button('Confirm')
          .onClick(() => this.onConfirm && this.onConfirm());
      }
      .justifyContent(FlexAlign.End)
      .width('100%');
    }
    .padding(20)
    .width('100%')
  }
}
