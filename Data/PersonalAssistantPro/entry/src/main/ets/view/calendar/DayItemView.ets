// entry/src/main/ets/view/calendar/DayItemView.ets
import { CalendarModel } from '../../viewmodel/CalendarViewModel';

@Component
export struct DayItemView {
  @ObjectLink dayModel: CalendarModel;

  // 选中的日期时间戳
  @Prop selectedTimestamp: number;

  build() {
    Column() {
      // 日期数字容器
      Column() {
        Text(this.dayModel.day.toString())
          .fontSize(16)
          .fontWeight(this.dayModel.isToday ? FontWeight.Bold : FontWeight.Normal)
          .fontColor(this.getTextColor())
      }
      .width(36)
      .height(36)
      .borderRadius(18) // 圆形
      .backgroundColor(this.getBackgroundColor())
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)

      // 如果有日程，可以在下面加个小圆点 (增加复杂度)
      if (this.dayModel.isToday) {
        Circle({ width: 4, height: 4 })
          .fill('#007DFF')
          .margin({ top: 4 })
      } else {
        // 占位，保持高度对齐
        Blank().height(4).margin({ top: 4 })
      }
    }
    .width('100%')
    .height(54)
    .justifyContent(FlexAlign.Center)
    .opacity(this.dayModel.isCurrentMonth ? 1 : 0.4) // 非本月日期变淡
  }

  /**
   * 获取文字颜色
   */
  private getTextColor(): string {
    if (this.isSelected()) {
      return '#FFFFFF'; // 选中变白
    }
    if (this.dayModel.isToday) {
      return '#007DFF'; // 今天变蓝
    }
    return '#182431'; // 普通黑色
  }

  /**
   * 获取背景颜色
   */
  private getBackgroundColor(): string {
    if (this.isSelected()) {
      return '#007DFF'; // 选中背景蓝
    }
    return 'transparent';
  }

  /**
   * 判断是否被选中
   */
  private isSelected(): boolean {
    return this.dayModel.timestamp === this.selectedTimestamp;
  }
}