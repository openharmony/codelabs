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
import { router } from '@kit.ArkUI';
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
    id: 'nangua-suanai-shala',
    name: '南瓜酸奶沙拉',
    description: '低脂健康·清甜爽口',
    image: $r('app.media.shala'),
    ingredients: '贝贝南瓜、无糖酸奶、蓝莓、草莓、核桃、奇亚籽',
    seasonings: '蜂蜜、柠檬汁'
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

  addToHistory(): void {
    const history: Dish[] = AppStorage.get(Constants.HISTORY_LIST) || [];
    const filtered = history.filter((item: Dish) => item.id !== this.dishData.id);
    AppStorage.set(Constants.HISTORY_LIST, [this.dishData, ...filtered]);
  }

  aboutToAppear(): void {
    const favorites: Dish[] = AppStorage.get(Constants.FAVORITE_LIST) || [];
    this.isCollected = favorites.some((item: Dish) => item.id === this.dishData.id);
    this.addToHistory();
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

        Text('南瓜酸奶沙拉')
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
          Text('南瓜酸奶沙拉是一道低脂健康的轻食，以蒸熟的贝贝南瓜为主料，搭配无糖酸奶和各种新鲜水果，口感清甜爽口，富含膳食纤维和维生素。南瓜的香甜与酸奶的醇厚完美融合，再加上坚果和奇亚籽的脆香，是早餐、下午茶或晚餐的理想选择。')
            .fontSize(16)
            .lineHeight(22)
            .margin({ top: 15 })
            .textOverflow({ overflow: TextOverflow.None })

          // 食材准备
          Text('🥗 主要食材:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20 })

          Text(this.dishData.ingredients.split('、')
            .map(item => `• ${item}`)
            .join('\n'))
            .fontSize(16)
            .margin({ top: 8 })

          // 调料准备
          Text('🧂 所需调料:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20 })

          Text(this.dishData.seasonings.split('、')
            .map(item => `• ${item}`)
            .join('\n'))
            .fontSize(16)
            .margin({ top: 8 })

          // 制作步骤
          Text('👩‍🍳 详细做法:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20 })

          Text('1. 准备食材：贝贝南瓜去皮去籽，切成小块；蓝莓和草莓洗净，草莓切成两半；核桃掰成小块；奇亚籽提前用清水浸泡15分钟。\n\n' +
            '2. 蒸南瓜：将南瓜块放入蒸锅中，大火蒸15-20分钟，直到南瓜变软熟透。\n\n' +
            '3. 调味酸奶：将无糖酸奶放入碗中，加入适量蜂蜜和柠檬汁，搅拌均匀，根据个人口味调整甜度。\n\n' +
            '4. 摆盘：将蒸好的南瓜块铺在盘子底部，淋上调味好的酸奶。\n\n' +
            '5. 装饰：在酸奶上放上蓝莓、草莓和核桃块，最后撒上浸泡好的奇亚籽。\n\n' +
            '6. 享用：将所有食材混合均匀后即可食用，冷藏后口感更佳。')
            .fontSize(16)
            .margin({ top: 8 })
            .lineHeight(24)

          // 小贴士
          Text('💡 烹饪小贴士:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20, bottom: 10 })

          Text('• 建议选择贝贝南瓜，口感更粉糯香甜\n' +
            '• 酸奶建议选择无糖的，健康又低卡\n' +
            '• 奇亚籽浸泡后会膨胀，口感更好，也更容易消化\n' +
            '• 水果可以根据季节和个人喜好选择，如芒果、香蕉、猕猴桃等\n' +
            '• 蜂蜜的用量可以根据个人口味调整，也可以用枫糖浆代替')
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