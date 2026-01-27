/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { LengthMetrics, router } from '@kit.ArkUI';
import { Constants } from '../common/Constants';

// 定义菜品数据类型
interface Dish {
  id: string;
  name: string;
  description: string;
  image: Resource;
  ingredients: string;
  seasonings: string;
}

@Entry
@Component
struct DishDetailPage {
  @State isSharePanelVisible: boolean = false;
  @State isCollected: boolean = false; // 改为使用 @State 而不是 @Link

  // 使用明确类型的菜品数据
  private dishData: Dish = {
    id: 'aolaanhuangjinjichui',
    name: '奥尔良黄金鸡锤',
    description: '香酥入味·吮指回味',
    image: $r('app.media.jichui'), // 菜品图片用jichui
    ingredients: '鸡锤、奥尔良腌料、鸡蛋、面包糠等',
    seasonings: '料酒、生抽、盐、黑胡椒粉、蜂蜜等'
  };

  // 收藏状态变化处理
  onCollectChange(): void {
    const favorites: Dish[] = AppStorage.get(Constants.FAVORITE_LIST) || [];
    if (this.isCollected) {
      // 添加到收藏
      if (!favorites.some((item: Dish) => item.id === this.dishData.id)) {
        const newFavorites = [...favorites, this.dishData];
        AppStorage.set(Constants.FAVORITE_LIST, newFavorites);
      }
    } else {
      // 从收藏移除
      const newFavorites = favorites.filter((item: Dish) => item.id !== this.dishData.id);
      AppStorage.set(Constants.FAVORITE_LIST, newFavorites);
    }
  }

  aboutToAppear(): void {
    const favorites: Dish[] = AppStorage.get(Constants.FAVORITE_LIST) || [];
    this.isCollected = favorites.some((item: Dish) => item.id === this.dishData.id);
  }

  build() {
    Column() {
      // 1. 顶部标题栏
      Row() {
        Image($r('app.media.return1')) // 返回图标
          .width(25)
          .height(25)
          .margin({ left: 10 })
          .onClick(() => router.back())

        Text('奥尔良黄金鸡锤')
          .fontSize(25)
          .fontWeight(FontWeight.Bold)
          .margin({ left: '32%' })
          .layoutWeight(1)

      }
      .width('100%')
      .height(50)
      .backgroundColor('#FFFFFF')
      .border({ width: 1, color: '#F0F0F0' })

      // 2. 可滚动内容区域
      Scroll() {
        Column() {
          // 菜品名称和收藏按钮
          Row() {
            Column() {
              Text(this.dishData.name)
                .fontSize(24)
                .fontWeight(FontWeight.Bold)
              Text(this.dishData.description)
                .fontSize(14)
                .fontColor('#666666')
                .margin({ top: 5 })
            }
            .layoutWeight(1)
          }

          Divider().color('#F0F0F0').margin({ top: 10, bottom: 10 })

          // 菜品图片
          Image(this.dishData.image)
            .width('100%')
            .height(250)
            .objectFit(ImageFit.Cover)
            .borderRadius(8)

          // 菜品描述
          Text('奥尔良黄金鸡锤是一道融合了中西风味的美味小吃，以鸡锤为主要食材，用奥尔良风味腌料腌制入味后，裹上面包糠炸至金黄酥脆。外皮香酥可口，内里肉质鲜嫩多汁，带有浓郁的奥尔良风味，甜中带辣，口感丰富，是家庭聚餐、朋友聚会时的绝佳选择，也是孩子们喜爱的美食之一。')
            .fontSize(16)
            .lineHeight(22)
            .margin({ top: 15 })
            .textOverflow({ overflow: TextOverflow.None })

          // 食材准备
          Text('🥔 主要食材:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20 })

          Text(this.dishData.ingredients.split('、').map(item => `• ${item}`).join('\n'))
            .fontSize(16)
            .margin({ top: 8 })

          // 调料准备
          Text('🧂 所需调料:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20 })

          Text(this.dishData.seasonings.split('、').map(item => `• ${item}`).join('\n'))
            .fontSize(16)
            .margin({ top: 8 })

          // 制作步骤
          Text('👨‍🍳 详细做法:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20 })

          Text('1. 准备食材：将鸡锤洗净，用刀在表面划几刀，方便入味；准备好奥尔良腌料、鸡蛋和面包糠。\n\n' +
            '2. 腌制：将鸡锤放入碗中，加入奥尔良腌料、料酒、生抽、盐和黑胡椒粉，搅拌均匀，盖上保鲜膜，放入冰箱冷藏腌制2-4小时。\n\n' +
            '3. 裹粉：取出腌制好的鸡锤，依次裹上一层干淀粉、鸡蛋液和面包糠。\n\n' +
            '4. 炸制：锅中倒入适量油，油温烧至六成热（约160℃），放入鸡锤，中小火炸至金黄酥脆，捞出沥干油份。\n\n' +
            '5. 二次复炸：将炸好的鸡锤再次放入油温八成热（约200℃）的锅中，快速复炸10-15秒，使外皮更加酥脆。\n\n' +
            '6. 装盘：将炸好的鸡锤装盘，可根据个人口味淋上一些蜂蜜或撒上一些熟芝麻，即可享用。')
            .fontSize(16)
            .margin({ top: 8 })
            .lineHeight(24)

          // 小贴士
          Text('💡 烹饪小贴士:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20, bottom: 10 })

          Text('• 鸡锤腌制时间越长越入味，建议腌制4小时以上\n' +
            '• 炸制时油温不宜过高，以免外皮炸糊而内里未熟\n' +
            '• 二次复炸可以使鸡锤外皮更加酥脆，口感更好\n' +
            '• 没有面包糠可以用馒头屑代替\n' +
            '• 鸡锤可以提前腌制好，放入冰箱冷冻保存，随时取用')
            .fontSize(16)
            .margin({ bottom: 20 })
        }
        .padding(15)
        .width('100%')
      }
      .width('100%')
      .layoutWeight(1)
      .scrollBar(BarState.On)
      .scrollBarColor('#cccccc')

      // 3. 底部按钮
      Column() {
        Row() {
          Button(this.isCollected ? '已收藏' : '加入收藏')
            .width('70%')
            .height(45)
            .backgroundColor(this.isCollected ? '#FF5722' : '#999999')
            .fontColor(Color.White)
            .onClick(() => {
              this.isCollected = !this.isCollected;
              this.onCollectChange(); // 手动调用收藏状态变更处理
            })

          Button('分享')
            .width('25%')
            .height(45)
            .margin({ left: 10 })
            .backgroundColor('#4285F4')
            .fontColor(Color.White)
            .onClick(() => {
              this.isSharePanelVisible = true;
            })
        }
        .width('100%')
        .justifyContent(FlexAlign.Center)
      }
      .width('100%')
      .margin({ top: 10 })

      // 4. 分享面板
      if (this.isSharePanelVisible) {
        Column() {
          // 分享选项
          Row() {
            Column() {
              Image($r('app.media.wxphoto'))
                .width(50)
                .height(50)
              Text('微信好友')
                .fontSize(12)
                .margin({ top: 5 })
            }
            .margin({ right: 20 })
            .onClick(() => {
              this.isSharePanelVisible = false;
            })

            Column() {
              Image($r('app.media.moment'))
                .width(50)
                .height(50)
              Text('朋友圈')
                .fontSize(12)
                .margin({ top: 5 })
            }
            .margin({ right: 20 })
            .onClick(() => {
              this.isSharePanelVisible = false;
            })

            Column() {
              Image($r('app.media.weibo'))
                .width(50)
                .height(50)
              Text('微博')
                .fontSize(12)
                .margin({ top: 5 })
            }
            .margin({ right: 20 })
            .onClick(() => {
              this.isSharePanelVisible = false;
            })

            Column() {
              Image($r('app.media.qqphoto'))
                .width(50)
                .height(50)
              Text('QQ')
                .fontSize(12)
                .margin({ top: 5 })
            }
            .onClick(() => {
              this.isSharePanelVisible = false;
            })
          }
          .margin({ top: 20, bottom: 20 })

          // 其他选项
          Row() {
            Column() {
              Image($r('app.media.link'))
                .width(50)
                .height(50)
              Text('复制链接')
                .fontSize(12)
                .margin({ top: 5 })
            }
            .margin({ right: 20 })
            .onClick(() => {
              this.isSharePanelVisible = false;
            })

            Column() {
              Image($r('app.media.more'))
                .width(50)
                .height(50)
              Text('更多')
                .fontSize(12)
                .margin({ top: 5 })
            }
            .onClick(() => {
              this.isSharePanelVisible = false;
            })
          }
          .margin({ bottom: 20 })

          // 取消按钮
          Button('取消')
            .width('90%')
            .height(45)
            .backgroundColor('#F5F5F5')
            .fontColor('#333333')
            .onClick(() => {
              this.isSharePanelVisible = false;
            })
        }
        .width('100%')
        .padding(20)
        .backgroundColor(Color.White)
        .borderRadius(20)
        .position({ x: 0, y: '100%' })
        .translate({ y: this.isSharePanelVisible ? -200 : 0 })
        .animation({ duration: 300, curve: Curve.EaseOut })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
    .onClick(() => {
      if (this.isSharePanelVisible) {
        this.isSharePanelVisible = false;
      }
    })
  }
}