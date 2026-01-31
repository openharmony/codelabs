@Component
export struct ThemeColorPicker { // 🔴 改名了：从 ColorPicker 改为 ThemeColorPicker
  // 双向绑定：子组件改变颜色，父组件也要变
  @Link selectedColor: string;

  // 预设颜色盘
  private readonly colors: string[] = [
    '#FF5733', '#33FF57', '#3357FF', '#F333FF',
    '#33FFF3', '#F3FF33', '#FF8C33', '#8C33FF',
    '#FF3333', '#33FF8C', '#8CFF33', '#5733FF'
  ];

  build() {
    Column() {
      Text("选择主题色")
        .fontSize(14)
        .fontColor('#999999')
        .width('100%')
        .margin({ bottom: 10 })

      // 使用 Grid 布局
      Grid() {
        ForEach(this.colors, (item: string) => {
          GridItem() {
            this.ColorCircle(item)
          }
        })
      }
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr') // 6列
      .rowsGap(10)
      .columnsGap(10)
      .height(100)
    }
    .padding(10)
    .backgroundColor(Color.White)
    .borderRadius(12)
  }

  /**
   * 抽取子组件构建函数
   */
  @Builder ColorCircle(color: string) {
    Stack() {
      // 颜色本体
      Circle({ width: 30, height: 30 })
        .fill(color)
        .onClick(() => {
          this.selectedColor = color;
        })

      // 选中标记 (只有选中才显示)
      if (this.selectedColor === color) {
        Circle({ width: 36, height: 36 })
          .fill(Color.Transparent)
          .stroke(Color.Black)
          .strokeWidth(2)
      }
    }
    .height(40)
    .width(40)
  }
}