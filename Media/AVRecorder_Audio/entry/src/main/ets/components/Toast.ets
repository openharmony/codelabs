@Component
export struct Toast {
  @Prop message: string = '';
  @Prop visible: boolean = false;

  build() {
    if (this.visible && this.message) {
      Column() {
        Text(this.message)
          .fontSize(14)
          .fontColor('#ffffff')
          .padding({ left: 16, right: 16, top: 8, bottom: 8 })
      }
      .backgroundColor('#333333')
      .borderRadius(18)
      .position({ x: 20, y: '85%' })
      .shadow({ radius: 12, color: '#55000000', offsetX: 0, offsetY: 6 })
    }
  }
}
