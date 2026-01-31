import { DateUtil } from '../../common/utils/DateUtil';

// 1. 定义一个简单的类，包含日期和状态
class DayInfo {
  dateStr: string;
  isDone: boolean;

  constructor(dateStr: string, isDone: boolean) {
    this.dateStr = dateStr;
    this.isDone = isDone;
  }
}

@Component
export struct HeatMap {
  @Prop @Watch('onDatesChange') recordDates: string[];

  // 2. 修改这里：不再存纯字符串，而是存包含状态的对象
  @State weeks: Array<Array<DayInfo>> = [];

  aboutToAppear() {
    this.generateCalendar();
  }

  onDatesChange() {
    this.generateCalendar();
  }

  generateCalendar() {
    let result: Array<Array<DayInfo>> = [];
    let now = new Date();

    // 找到本周的周日作为结束点
    let dayOfWeek = now.getDay();

    // 生成过去 16 周
    for (let w = 0; w < 16; w++) {
      let weekData: DayInfo[] = [];
      for (let d = 0; d < 7; d++) {
        let delta = dayOfWeek + (w * 7) - d;
        let targetDate = new Date();
        targetDate.setDate(now.getDate() - delta);

        let dateStr = DateUtil.formatDate(targetDate.getTime());

        // 3. 核心修复：在这里直接计算好 isDone
        // 这样生成的 DayInfo 对象就是新的，ForEach 一定会刷新
        let isDone = this.recordDates.includes(dateStr);

        weekData.unshift(new DayInfo(dateStr, isDone));
      }
      result.unshift(weekData);
    }
    this.weeks = result;
  }

  build() {
    Column() {
      Text("打卡热力图 (近4个月)")
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .margin({ bottom: 10 })

      Scroll() {
        Row({ space: 4 }) {
          // 外层循环：周
          ForEach(this.weeks, (week: DayInfo[]) => {
            Column({ space: 4 }) {
              // 内层循环：天
              ForEach(week, (day: DayInfo) => {
                // 4. 直接使用 day.isDone，无需计算
                this.DayCell(day)
              })
            }
          })
        }
      }
      .scrollable(ScrollDirection.Horizontal)
      .scrollBar(BarState.Off)
      .align(Alignment.End)
    }
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(16)
    .width('100%')
  }

  // 5. 修改 Builder 接收对象
  @Builder DayCell(day: DayInfo) {
    Rect()
      .width(12)
      .height(12)
      .radius(2)
      // 绿色代表已打卡
      .fill(day.isDone ? '#00CC66' : '#EBEDF0')
      .onClick(() => {
        console.info(`点击了: ${day.dateStr}, 状态: ${day.isDone}`);
      })
  }
}