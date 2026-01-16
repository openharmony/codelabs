// Event.ts
export interface CalendarEvent {
  id: string;           // 主键
  title: string;         // 标题
  description: string;  // 描述
  startTime: number;     // 开始时间戳
  endTime: number;       // 结束时间戳
  allDay: boolean;       // 全天事件
  location: string;     // 地点
  calendarId: string;    // 日历分类
  color: string;         // 颜色标记
  reminder: number;      // 提醒时间（分钟）
  repeatRule: string;   // 重复规则
  completed: boolean;   // 完成情况
  created: number;       // 创建时间
  updated: number;       // 更新时间
}