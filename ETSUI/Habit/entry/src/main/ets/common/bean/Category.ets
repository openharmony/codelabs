export default class Category {
  id: number = -1;
  name: string = '';
  color: string = '#CCCCCC'; // 分类也有颜色

  constructor(name: string, color: string) {
    this.name = name;
    this.color = color;
  }
}