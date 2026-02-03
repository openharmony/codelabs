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
  @State isCollected: boolean = false;
  // 使用明确类型的菜品数据
  private dishData: Dish = {
    id: 'hongshaoyangrou',
    name: '红烧羊肉',
    description: '西北名菜·酥烂入味',
    image: $r('app.media.yangrou1'),
    ingredients: '羊腿肉、胡萝卜、白萝卜、大葱等',
    seasonings: '郫县豆瓣酱、生抽、老抽、料酒、冰糖等'
  };

  // 收藏状态变化处理
  onCollectChange(): void {
    const favorites: Dish[] = AppStorage.get(Constants.FAVORITE_LIST) || [];
    if (this.isCollected) {
      if (!favorites.some((item: Dish) => item.id === this.dishData.id)) {
        const newFavorites = [...favorites, this.dishData];
        AppStorage.set(Constants.FAVORITE_LIST, newFavorites);
      }
    } else {
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
        Image($r('app.media.return1'))
          .width(25)
          .height(25)
          .margin({ left: 10 })
          .onClick(() => router.back())

        Text('红烧羊肉')
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
          Text('红烧羊肉是西北地区传统名菜，选用优质羊腿肉，经过焯水去腥、炒制上色、慢火炖煮等工序，使羊肉酥烂入味，肥而不腻。搭配胡萝卜等时令蔬菜，既增添了甜味又平衡了营养，是冬季滋补的绝佳选择。')
            .fontSize(16)
            .lineHeight(22)
            .margin({ top: 15 })
            .textOverflow({ overflow: TextOverflow.None })

          // 食材准备
          Text('🍖 主要食材:')
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
          Text('👨‍🍳 详细做法:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20 })

          Text('1. 处理羊肉：将羊腿肉切块，冷水浸泡2小时去血水，冷水下锅焯水，加入料酒、姜片去腥，捞出洗净备用。\n\n' +
            '2. 炒香料：热锅凉油，放入葱段、姜片、蒜瓣、干辣椒、八角、桂皮小火炒香。\n\n' +
            '3. 炒制羊肉：加入羊肉大火翻炒至表面微黄，沿锅边淋入料酒去腥增香。\n\n' +
            '4. 调味上色：加入郫县豆瓣酱炒出红油，再加入生抽、老抽、冰糖翻炒均匀。\n\n' +
            '5. 炖煮：加入足量热水没过食材，大火烧开后转小火慢炖1小时。\n\n' +
            '6. 加配菜：放入胡萝卜块、白萝卜块继续炖煮30分钟。\n\n' +
            '7. 收汁：开盖大火收汁，根据口味加盐调味，撒上香菜即可出锅。')
            .fontSize(16)
            .margin({ top: 8 })
            .lineHeight(24)

          // 小贴士
          Text('💡 烹饪小贴士:')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20, bottom: 10 })

          Text('• 选用羊腿肉或羊腩肉，肥瘦相间口感更好\n' +
            '• 焯水时冷水下肉，能更好去除血沫和膻味\n' +
            '• 冰糖炒糖色能让羊肉颜色更红亮\n' +
            '• 炖煮时间要充足，羊肉才能酥烂入味\n' +
            '• 可加入土豆、山药等根茎类蔬菜一起炖煮')
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
              this.onCollectChange();
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