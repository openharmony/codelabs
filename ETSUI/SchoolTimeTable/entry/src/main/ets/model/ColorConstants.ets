export class ColorConstants {
  static readonly MACARON_COLORS: string[] = [
    '#FFB7B2', // 马卡龙粉
    '#FFDAC1', // 马卡龙桃
    '#E2F0CB', // 马卡龙绿
    '#B5EAD7', // 马卡龙薄荷
    '#C7CEEA', // 马卡龙紫
    '#F8B195', // 柔和橙
    '#F67280', // 柔和红
    '#C06C84', // 浆果色
    '#6C5B7B', // 深紫
    '#355C7D', // 深蓝灰
    '#99B898', // 抹茶绿
    '#FECEAB'// 杏色
  ];

  static getRandomColor(): string {
    const index = Math.floor(Math.random() * ColorConstants.MACARON_COLORS.length);
    return ColorConstants.MACARON_COLORS[index];
  }
}
