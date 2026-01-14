import { BillItem } from '../model/BillItem';

interface DailyStat {
  income: number;
  expense: number;
}

@Component
export struct CalendarView {
  @Link selectedDate: Date; // 当前选中的日期
  @Prop bills: BillItem[]; // 所有账单数据
  @State currentMonth: Date = new Date(); // 当前展示的月份

  aboutToAppear() {
    // 初始化当前月份为选中日期的月份
    this.currentMonth = new Date(this.selectedDate.getFullYear(), this.selectedDate.getMonth(), 1);
  }

  // 获取当月天数
  getDaysInMonth(year: number, month: number): number {
    return new Date(year, month + 1, 0).getDate();
  }

  // 获取当月第一天是星期几 (0-6)
  getFirstDayOfWeek(year: number, month: number): number {
    return new Date(year, month, 1).getDay();
  }

  // 获取每日收支统计
  getDailyStats(): Record<number, DailyStat> {
    const stats: Record<number, DailyStat> = {};
    const year = this.currentMonth.getFullYear();
    const month = this.currentMonth.getMonth();

    this.bills.forEach(bill => {
      const dateParts = bill.date.split('/'); // 格式: YYYY/MM/DD
      if (dateParts.length === 3) {
        const bYear = parseInt(dateParts[0]);
        const bMonth = parseInt(dateParts[1]) - 1;
        const bDay = parseInt(dateParts[2]);

        if (bYear === year && bMonth === month) {
          if (!stats[bDay]) {
            let stat: DailyStat = { income: 0, expense: 0 };
            stats[bDay] = stat;
          }
          if (bill.direction === 'income') {
            stats[bDay].income += bill.amount;
          } else {
            stats[bDay].expense += bill.amount;
          }
        }
      }
    });
    return stats;
  }

  build() {
    Column() {
      // 月份切换器
      Row() {
        Button('<')
          .type(ButtonType.Normal)
          .backgroundColor(Color.Transparent)
          .fontColor("#666666")
          .fontSize(18)
          .width(40)
          .height(40)
          .onClick(() => {
            this.currentMonth = new Date(this.currentMonth.getFullYear(), this.currentMonth.getMonth() - 1, 1);
          })
        
        Text(`${this.currentMonth.getFullYear()}年${this.currentMonth.getMonth() + 1}月`)
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor("#333333")
        
        Button('>')
          .type(ButtonType.Normal)
          .backgroundColor(Color.Transparent)
          .fontColor("#666666")
          .fontSize(18)
          .width(40)
          .height(40)
          .onClick(() => {
            this.currentMonth = new Date(this.currentMonth.getFullYear(), this.currentMonth.getMonth() + 1, 1);
          })
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .margin({ bottom: 10 })

      // 星期表头
      Grid() {
        ForEach(['日', '一', '二', '三', '四', '五', '六'], (day: string) => {
          GridItem() {
            Text(day)
              .fontSize(12)
              .fontColor('#999999')
          }
        })
      }
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr')
      .height(24)
      .width("100%")

      // 日期网格
      Grid() {
        // 填充月初空白
        ForEach(Array(this.getFirstDayOfWeek(this.currentMonth.getFullYear(), this.currentMonth.getMonth())).fill(0), (_: number) => {
          GridItem() {}
        })

        // 渲染每一天
        ForEach(Array(this.getDaysInMonth(this.currentMonth.getFullYear(), this.currentMonth.getMonth())).fill(0).map((_: number, i: number) => i + 1), (day: number) => {
          GridItem() {
            Column() {
              Text(day.toString())
                .fontSize(14)
                .fontColor(this.isSelected(day) ? Color.White : '#333333')
                .fontWeight(this.isToday(day) ? FontWeight.Bold : FontWeight.Normal)
              
              // 收支小圆点
              Row({ space: 3 }) {
                if (this.getDailyStats()[day]?.expense > 0) {
                  Circle({ width: 4, height: 4 }).fill(this.isSelected(day) ? Color.White : '#FF3B30')
                }
                if (this.getDailyStats()[day]?.income > 0) {
                  Circle({ width: 4, height: 4 }).fill(this.isSelected(day) ? Color.White : '#34C759')
                }
              }
              .margin({ top: 4 })
              .height(4)
            }
            .width(36)
            .height(36)
            .borderRadius(18)
            .backgroundColor(this.isSelected(day) ? '#4A6CF7' : (this.isToday(day) ? '#F0F2F5' : Color.Transparent))
            .justifyContent(FlexAlign.Center)
            .onClick(() => {
               this.selectedDate = new Date(this.currentMonth.getFullYear(), this.currentMonth.getMonth(), day);
            })
          }
        })
      }
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr')
      .rowsGap(8)
      .width("100%")
      .height(220) // 调整高度，更紧凑
    }
    .padding(16)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({ radius: 4, color: "#0D000000", offsetY: 2 })
  }

  isSelected(day: number): boolean {
    return this.selectedDate.getDate() === day && 
           this.selectedDate.getMonth() === this.currentMonth.getMonth() && 
           this.selectedDate.getFullYear() === this.currentMonth.getFullYear();
  }

  isToday(day: number): boolean {
    const today = new Date();
    return today.getDate() === day && 
           today.getMonth() === this.currentMonth.getMonth() && 
           today.getFullYear() === this.currentMonth.getFullYear();
  }
}
