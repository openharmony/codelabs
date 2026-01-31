import { DateUtil } from '../../common/utils/DateUtil';

@Component
export struct CalendarView {
  @Prop recordDates: string[] = []; // 已打卡的日期列表
  private weeks: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]; // 展示13周（约三个月）
  private days: number[] = [0, 1, 2, 3, 4, 5, 6]; // 一周7天
  // 👇👇👇 新增这一行 👇👇👇
  // 定义一个回调函数，接收点击的日期字符串
  onDateClick: (date: string) => void = () => {};
  // 计算某个坐标点对应的日期
  getDateByCoordinate(weekIndex: number, dayIndex: number): string {
    let date = new Date();
    // 倒推日期：今天往前推 (12 - weekIndex) 周，再根据星期偏移
    const totalDaysBack = (12 - weekIndex) * 7 + (new Date().getDay() - dayIndex);
    date.setDate(date.getDate() - totalDaysBack);
    return DateUtil.formatDate(date.getTime());
  }

  // 判断该日期是否打卡
  isDateChecked(weekIndex: number, dayIndex: number): boolean {
    const targetDate = this.getDateByCoordinate(weekIndex, dayIndex);
    return this.recordDates.includes(targetDate);
  }

  build() {
    Column() {
      Row() {
        Text('最近三个月打卡分布').fontSize(14).fontColor('#666')
      }.width('100%').margin({ bottom: 10 })

      // 热力图核心网格
      Scroll() {
        Row({ space: 4 }) {
          ForEach(this.weeks, (week: number) => {
            Column({ space: 4 }) {
              ForEach(this.days, (day: number) => {
                // 如果日期超过了今天，就隐藏（不显示未来的方块）
                if (this.isFuture(week, day)) {
                  Rect()
                    .width(12).height(12)
                    .fill('transparent')
                } else {
                  // ✅ 关键修改：在这里添加 onClick 事件
                  Rect()
                    .width(12).height(12)
                    .radius(2)
                    // 打卡了深绿色，没打卡浅灰色
                    .fill(this.isDateChecked(week, day) ? '#28C060' : '#EBEDF0')
                    .onClick(() => {
                      // 1. 计算点击的日期
                      const dateStr = this.getDateByCoordinate(week, day);
                      // 2. 触发回调
                      this.onDateClick(dateStr);
                    })
                }
              })
            }
          })
        }
      }
      .scrollable(ScrollDirection.Horizontal)
      .scrollBar(BarState.Off)

      // 图例
      Row({ space: 8 }) {
        Text('少').fontSize(10).fontColor('#999')
        Rect().width(10).height(10).fill('#EBEDF0').radius(2)
        Rect().width(10).height(10).fill('#28C060').radius(2)
        Text('多').fontSize(10).fontColor('#999')
      }
      .width('100%')
      .justifyContent(FlexAlign.End)
      .margin({ top: 8 })
    }
    .padding(12)
  }

  // 辅助函数：判断坐标日期是否在未来
  isFuture(weekIndex: number, dayIndex: number): boolean {
    const targetStr = this.getDateByCoordinate(weekIndex, dayIndex);
    const todayStr = DateUtil.formatDate(new Date().getTime());
    return targetStr > todayStr;
  }
}