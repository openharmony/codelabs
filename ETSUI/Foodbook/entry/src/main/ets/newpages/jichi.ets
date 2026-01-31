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
    id: 'jichibaofan',
    name: '鸡翅包饭',
    description: '创意美食·外焦里嫩',
    image: $r('app.media.jichi'), // 菜品图片用jichi
    ingredients: '鸡翅、米饭、胡萝卜、青豆、玉米粒、香菇等',
    seasonings: '生抽、老抽、蚝油、料酒、盐、黑胡椒粉、白糖等'
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

        Text('鸡翅包饭')
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
          Text('鸡翅包饭是一道创意美食，将鸡翅去骨后填入香喷喷的米饭和各种配料，经过烤制后，鸡翅外皮金黄酥脆，内部的米饭吸收了鸡翅的香味，口感丰富，外焦里嫩，是一道颜值与美味并存的佳肴，深受大人和小孩的喜爱。')
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

          Text('1. 准备食材：鸡翅洗净，用剪刀剪开鸡翅背面，去除骨头；米饭蒸熟备用；胡萝卜、香菇切丁；青豆、玉米粒焯水备用。\n\n' +
            '2. 炒米饭：锅中倒油，油热后放入胡萝卜丁和香菇丁翻炒至变软，加入青豆、玉米粒继续翻炒。\n\n' +
            '3. 调味：加入生抽、老抽、蚝油、盐、黑胡椒粉、白糖调味，翻炒均匀后倒入蒸熟的米饭，继续翻炒至米饭入味。\n\n' +
            '4. 酿鸡翅：将炒好的米饭填入去骨的鸡翅中，用牙签固定开口处。\n\n' +
            '5. 腌制：将酿好的鸡翅放入碗中，加入生抽、料酒、黑胡椒粉腌制30分钟。\n\n' +
            '6. 烤制：将腌制好的鸡翅放入预热好的烤箱中，200℃烤20分钟左右，至鸡翅表面金黄酥脆。\n\n' +
            '7. 装盘：取出烤好的鸡翅，去掉牙签，装盘即可享用。')
            .fontSize(16)
            .margin({ top: 8 })
            .lineHeight(24)

          // 小贴士
          Text('💡 烹饪小贴士:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20, bottom: 10 })

          Text('• 去骨时要小心，尽量保持鸡翅的完整\n' +
            '• 米饭不要填得太满，以免烤制时溢出\n' +
            '• 用牙签固定鸡翅时，要插牢，防止烤制时散开\n' +
            '• 烤制时间和温度可以根据自己的烤箱适当调整\n' +
            '• 可以根据个人口味在米饭中加入其他配料，如火腿、虾仁等')
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