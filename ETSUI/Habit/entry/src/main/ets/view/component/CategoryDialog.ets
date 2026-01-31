@CustomDialog
export struct CategoryDialog {
  controller: CustomDialogController;
  // 定义一个回调函数，把输入的名字传回给父组件
  confirm: (text: string) => void = (text: string) => {};

  @State text: string = '';

  build() {
    Column() {
      Text('新建分类')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 20 })

      TextInput({ placeholder: '请输入分类名称 (如: 阅读)' })
        .height(50)
        .width('90%')
        .backgroundColor('#F5F5F5')
        .borderRadius(10)
        .onChange((value: string) => {
          this.text = value;
        })

      Row() {
        Button('取消')
          .onClick(() => {
            this.controller.close();
          })
          .backgroundColor(Color.White)
          .fontColor('#666')
          .width('40%')

        Button('确定')
          .onClick(() => {
            if (this.text.trim().length > 0) {
              this.confirm(this.text); // 调用回调
              this.controller.close();
            }
          })
          .backgroundColor('#007DFF')
          .width('40%')
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceAround)
      .margin({ top: 20, bottom: 20 })
    }
    .backgroundColor(Color.White)
    .borderRadius(20)
    .width('80%')
  }
}