export interface HealthTabData {
  id: number;
  name: string;
}

export const HEALTH_TAB_DATA: HealthTabData[] = [
  { id: 0, name: "全部" },
  { id: 1, name: "营养指南" },
  { id: 2, name: "运动科学" },
  { id: 3, name: "心理健康" },
  { id: 4, name: "慢病管理" },
  { id: 5, name: "季节养生" },
  { id: 6, name: "职场健康" },
  { id: 7, name: "急救常识" },
]

export const HEALTH_TAB_ID_TO_NAME: Array<string> = [
  "全部",
  "营养指南",
  "运动科学",
  "心理健康",
  "慢病管理",
  "季节养生",
  "职场健康",
  "急救常识"
]
