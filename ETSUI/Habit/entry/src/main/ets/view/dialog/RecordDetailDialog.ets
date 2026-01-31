/**
 * 打卡记录详情查看弹窗
 */
@CustomDialog
export struct RecordDetailDialog {
  controller: CustomDialogController;

  // 接收外部传入的数据
  dateStr: string = '';
  mood: number = 1;
  note: string = '';

  moods: string[] = ['😐', '🙂', '😄', '🤩'];

  build() {
    Column() {
      // 日期
      Text(this.dateStr)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 20 })

      // 心情大图标
      Text(this.moods[this.mood])
        .fontSize(60)
        .margin({ bottom: 10 })

      // 心得内容展示
      if (this.note && this.note.length > 0) {
        Text(this.note)
          .fontSize(16)
          .fontColor('#333')
          .width('100%')
          .padding(15)
          .backgroundColor('#F5F5F5')
          .borderRadius(12)
          .margin({ bottom: 20 })
      } else {
        Text('这一天没有写心得哦~')
          .fontSize(14)
          .fontColor('#999')
          .margin({ bottom: 20 })
      }

      Button('关闭')
        .width('100%')
        .backgroundColor('#F5F5F5')
        .fontColor('#666')
        .onClick(() => {
          this.controller.close();
        })
    }
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(24)
    .width('80%')
  }
}