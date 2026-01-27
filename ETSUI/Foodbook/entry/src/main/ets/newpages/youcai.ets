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
    id: 'qiangchaoyoucai',
    name: '炝炒油菜',
    description: '家常快手·脆嫩鲜香',
    image: $r('app.media.youcai'), // 菜品图片用youcai
    ingredients: '油菜、干辣椒、蒜片、姜片等',
    seasonings: '盐、生抽、白糖、料酒、食用油等'
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

        Text('炝炒油菜')
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
          Text('炝炒油菜是一道经典的家常快手菜，以油菜为主料，通过"炝"的烹饪方法，激发出干辣椒和蒜片的香味，使油菜保持脆嫩的口感和鲜艳的色泽。这道菜不仅美味可口，还富含维生素和膳食纤维，清爽解腻，是餐桌上的常客。')
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

          Text('1. 准备食材：将油菜洗净，沥干水分，切成段；干辣椒剪成小段，去籽；准备好蒜片和姜片。\n\n' +
            '2. 焯水：锅中烧开水，加入少许盐和食用油，放入油菜焯水10-15秒，捞出过冷水，沥干水分。\n\n' +
            '3. 炝锅：热锅凉油，放入干辣椒段、蒜片和姜片，小火炒出香味。\n\n' +
            '4. 炒油菜：转大火，迅速放入油菜段翻炒均匀。\n\n' +
            '5. 调味：加入盐、生抽、白糖和料酒，继续翻炒至油菜入味。\n\n' +
            '6. 出锅：翻炒均匀后，立即关火，装盘上桌。')
            .fontSize(16)
            .margin({ top: 8 })
            .lineHeight(24)

          // 小贴士
          Text('💡 烹饪小贴士:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20, bottom: 10 })

          Text('• 油菜焯水时间不宜过长，以免失去脆嫩口感\n' +
            '• 焯水时加入少许盐和食用油，可以保持油菜的色泽\n' +
            '• 干辣椒不要炒糊，否则会有苦味\n' +
            '• 翻炒要迅速，保持大火快炒，以保留油菜的营养\n' +
            '• 可以根据个人口味加入适量的花椒，增加麻香味道')
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