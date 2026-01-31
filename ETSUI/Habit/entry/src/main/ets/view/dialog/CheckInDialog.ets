/**
 * 打卡心得输入弹窗
 */
@CustomDialog
export struct CheckInDialog {
  controller: CustomDialogController;
  // 确认回调：将选择的心情和输入的心得传回父组件
  onConfirm: (mood: number, note: string) => void = () => {};

  @State selectedMood: number = 1; // 默认心情索引
  @State note: string = '';

  // 心情选项配置
  moods: string[] = ['😐', '🙂', '😄', '🤩'];
  moodLabels: string[] = ['平淡', '开心', '超棒', '狂喜'];

  build() {
    Column() {
      Text('✨ 打卡成功')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 10 })

      Text('记录一下此刻的心情吧~')
        .fontSize(14)
        .fontColor('#999')
        .margin({ bottom: 20 })

      // 1. 心情选择区域
      Row({ space: 15 }) {
        ForEach(this.moods, (emoji: string, index: number) => {
          Column() {
            Text(emoji).fontSize(30)
            Text(this.moodLabels[index]).fontSize(10).fontColor('#666').margin({ top: 4 })
          }
          .padding(10)
          // 选中高亮逻辑
          .backgroundColor(this.selectedMood === index ? '#E6F2FF' : '#F5F5F5')
          .borderRadius(12)
          .border({ width: 2, color: this.selectedMood === index ? '#007DFF' : 'transparent' })
          .onClick(() => {
            this.selectedMood = index;
          })
        })
      }
      .margin({ bottom: 20 })

      // 2. 心得输入框
      TextArea({ placeholder: '写点什么...(选填)', text: this.note })
        .height(100)
        .backgroundColor('#F5F5F5')
        .borderRadius(12)
        .onChange((value) => {
          this.note = value;
        })
        .margin({ bottom: 20 })

      // 3. 底部按钮
      Row() {
        Button('跳过')
          .backgroundColor('#F5F5F5')
          .fontColor('#666')
          .layoutWeight(1)
          .margin({ right: 10 })
          .onClick(() => {
            // 跳过也算打卡，传默认心情和空文字
            this.onConfirm(1, '');
            this.controller.close();
          })

        Button('确定')
          .backgroundColor('#007DFF')
          .fontColor(Color.White)
          .layoutWeight(1)
          .onClick(() => {
            this.onConfirm(this.selectedMood, this.note);
            this.controller.close();
          })
      }
      .width('100%')
    }
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(24)
    .width('90%')
  }
}