@Observed
export default class Habit {
  id: number = -1;
  name: string = '';
  icon: string = 'app.media.icon_default';
  color: string = '#FFCC00';
  frequency: string = '1111111';
  currentCount: number = 0;
  targetCount: number = 1;
  createTime: number = new Date().getTime();
  isChecked: boolean = false;
  categoryId: number = 1; // 默认归属 ID=1 (全部)
  reminderTime: string = ''; // 默认为空，表示不提醒
  reminderId: number = -1;

  constructor(name: string, color: string, targetCount: number = 1, categoryId: number = 1) {
    this.name = name;
    this.color = color;
    this.targetCount = targetCount;
    this.categoryId = categoryId; // 🆕 赋值
  }
}