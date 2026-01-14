import promptAction from '@ohos.promptAction'

@CustomDialog
export struct RatingDialog {
  // 评分弹窗：外部传入当前评分/内容，通过回调提交或删除
  @State rating: number = 0
  @State content: string = ''
  controller: CustomDialogController
  action: (rating: number, content: string) => void = () => {}
  deleteAction: () => void = () => {}
  @State isExist: boolean = false

  // 仅负责收集用户输入；提交校验在此处做最小化处理
  build() {
    Column() {
      Text('评分')
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 12, bottom: 12 })

      Row({ space: 5 }) {
        ForEach([1, 2, 3, 4, 5], (item: number) => {
          Text(this.rating >= item ? '★' : '☆')
            .fontSize(28)
            .fontColor(this.rating >= item ? '#F59E0B' : '#DDD')
            .onClick(() => {
              this.rating = item
            })
        })
      }
      .margin({ bottom: 12 })

      TextArea({ text: this.content, placeholder: '写几句评价吧（可选）' })
        .height(100)
        .backgroundColor('#F5F5F5')
        .borderRadius(8)
        .onChange((value: string) => {
          this.content = value
        })

      Row({ space: 12 }) {
        if (this.isExist) {
          Button('删除')
            .layoutWeight(1)
            .backgroundColor('#FFF0F0')
            .fontColor('#D14343')
            .onClick(() => {
              this.deleteAction()
              this.controller.close()
            })
        }

        Button('取消')
          .layoutWeight(1)
          .backgroundColor('#F0F0F0')
          .fontColor('#666')
          .onClick(() => this.controller.close())

        Button('提交')
          .layoutWeight(1)
          .backgroundColor(this.rating > 0 ? '#D47D00' : '#E5B97A')
          .fontColor('#FFF')
          .opacity(this.rating > 0 ? 1.0 : 0.6)
          .onClick(() => {
            if (this.rating <= 0) {
              promptAction.showToast({ message: '请先选择星级' })
              return
            }
            this.action(this.rating, this.content)
            this.controller.close()
          })
      }
      .margin({ top: 12 })
    }
    .padding(16)
    .backgroundColor('#FFF')
    .borderRadius(16)
    .width('80%')
  }
}