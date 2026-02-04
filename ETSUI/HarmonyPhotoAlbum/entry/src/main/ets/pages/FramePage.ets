/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


import router from '@ohos.router';
import Constants from '../common/constants/Constants';
import { LazyImage } from '../components/LazyImage';

// 电子相框使用的预置图片分类（与 resources/base/media 一一对应）
const FRAME_IMAGES_SCENE: Resource[] = [
  $r('app.media.ic_scene_0'),
  $r('app.media.ic_scene_1'),
  $r('app.media.ic_scene_2')
];

const FRAME_IMAGES_LIFE: Resource[] = [
  $r('app.media.ic_life_0'),
  $r('app.media.ic_life_1'),
  $r('app.media.ic_life_2'),
  $r('app.media.ic_life_3'),
  $r('app.media.ic_life_4'),
  $r('app.media.ic_life_5')
];

const FRAME_IMAGES_FOOD: Resource[] = [
  $r('app.media.ic_food_0'),
  $r('app.media.ic_food_1')
];

const FRAME_IMAGES_MEN: Resource[] = [
  $r('app.media.ic_men_0'),
  $r('app.media.ic_men_2'),
  $r('app.media.ic_men_3')
];

// 如有需要，也可以把应用图标作为“其他”类别的一部分
const FRAME_IMAGES_ICON: Resource[] = [
  $r('app.media.icon')
];

@Entry
@Component
struct FramePage {
  // 每次分类切换时重新创建 controller，用于强制刷新 Swiper 状态
  private swiperController: SwiperController = new SwiperController();

  @State autoPlayInterval: number = 5000; // ms
  @State showControlPanel: boolean = true;

  // 当前电子相框分类
  @State currentCategory: string = '全部';

  // 当前分类对应的图片列表（用于驱动 Swiper，保证分类切换时即时更新）
  @State frameImages: Resource[] = [];

  // 可选分类（与资源目录对应）
  private readonly frameCategories: string[] = ['全部', '风景', '生活', '美食', '人物'];

  // 各分类对应的切换方法：每个按钮调用自己的方法，保证一一对应
  private showAllCategory() {
    console.info('[FramePage] 切换分类: 全部');
    this.currentCategory = '全部';
    this.frameImages = new Array<Resource>().concat(
      FRAME_IMAGES_SCENE,
      FRAME_IMAGES_LIFE,
      FRAME_IMAGES_FOOD,
      FRAME_IMAGES_MEN,
      FRAME_IMAGES_ICON
    );
    this.swiperController = new SwiperController();
  }

  private showSceneCategory() {
    console.info('[FramePage] 切换分类: 风景');
    this.currentCategory = '风景';
    this.frameImages = FRAME_IMAGES_SCENE;
    this.swiperController = new SwiperController();
  }

  private showLifeCategory() {
    console.info('[FramePage] 切换分类: 生活');
    this.currentCategory = '生活';
    this.frameImages = FRAME_IMAGES_LIFE;
    this.swiperController = new SwiperController();
  }

  private showFoodCategory() {
    console.info('[FramePage] 切换分类: 美食');
    this.currentCategory = '美食';
    this.frameImages = FRAME_IMAGES_FOOD;
    this.swiperController = new SwiperController();
  }

  private showMenCategory() {
    console.info('[FramePage] 切换分类: 人物');
    this.currentCategory = '人物';
    this.frameImages = FRAME_IMAGES_MEN;
    this.swiperController = new SwiperController();
  }
  
  aboutToAppear() {
    // 从路由参数中读取分类（由 FrameCategoryPage 传入）
    let category: string = '全部';
    try {
      const params: Record<string, Object> = router.getParams() as Record<string, Object>;
      const fromRouter: string | undefined = params && params[Constants.PARAM_FRAME_CATEGORY] as string;
      if (fromRouter) {
        category = fromRouter;
      }
    } catch (e) {
      console.warn('[FramePage] 读取路由参数失败，使用默认分类: 全部');
    }

    switch (category) {
      case '风景':
        this.showSceneCategory();
        break;
      case '生活':
        this.showLifeCategory();
        break;
      case '美食':
        this.showFoodCategory();
        break;
      case '人物':
        this.showMenCategory();
        break;
      case '全部':
      default:
        this.showAllCategory();
        break;
    }
  }
  build() {
    Stack({ alignContent: Alignment.Bottom }) {
      // 背景全屏轮播
      if (this.frameImages.length > 0) {
        Swiper(this.swiperController) {
          ForEach(this.frameImages, (img: Resource, index: number) => {
            Stack() {
              LazyImage({
                src: img,
                widthValue: Constants.FULL_PERCENT,
                heightValue: Constants.FULL_PERCENT,
                objectFit: ImageFit.Cover,
                placeholder: Constants.PLACEHOLDER_IMAGE,
                errorHolder: Constants.ERROR_IMAGE
              })
            }
            .width(Constants.FULL_PERCENT)
            .height(Constants.FULL_PERCENT)
          }, (img: Resource, index?: number) => `${index}`)
        }
        .autoPlay(this.frameImages.length > 1)
        .interval(this.autoPlayInterval)
        .loop(this.frameImages.length > 1)
        .duration(Constants.TRANSITION_DURATION_LONG)
        .width(Constants.FULL_PERCENT)
        .height(Constants.FULL_PERCENT)
        .indicator(false)
        .onClick(() => {
          // 点击切换控制面板显隐
          this.showControlPanel = !this.showControlPanel;
        })
      } else {
        // 无照片时的占位提示
        Column() {
          Text('📷')
            .fontSize(60)
            .opacity(0.3)
            .margin({ bottom: 16 })

          Text('暂无可播放的照片')
            .fontSize(18)
            .fontColor('#EEEEEE')

          Text('请确认资源图片是否存在')
            .fontSize(14)
            .fontColor('#BBBBBB')
            .margin({ top: 8 })
        }
        .width(Constants.FULL_PERCENT)
        .height(Constants.FULL_PERCENT)
        .justifyContent(FlexAlign.Center)
        .backgroundColor(Color.Black)
      }

      // 顶部返回 + 标题栏 + 底部控制面板
      if (this.showControlPanel) {
        Column() {
          // 顶部导航
          Row({ space: 12 }) {
            Button({ type: ButtonType.Circle }) {
              Text('<')
                .fontSize(18)
                .fontColor(Color.White)
            }
            .backgroundColor('rgba(0,0,0,0.4)')
            .width(40)
            .height(40)
            .onClick(() => {
              router.back();
            })

            Column() {
              Text('沉浸式电子相框')
                .fontSize(18)
                .fontColor(Color.White)
                .fontWeight(FontWeight.Medium)

              // 显示当前正在播放的分类
              Text(`正在播放: ${this.currentCategory}`)
                .fontSize(12)
                .fontColor('#DDDDDD')
            }
          }
          .padding({ top: 24, left: 16, right: 16 })

          Blank().layoutWeight(1)

          // 底部控制面板：调整轮播间隔
          Column() {
            Row() {
              Text('轮播间隔: ')
                .fontSize(14)
                .fontColor(Color.White)

              Text(`${Math.round(this.autoPlayInterval / 1000)} 秒`)
                .fontSize(16)
                .fontColor('#FFD166')
                .fontWeight(FontWeight.Bold)
            }
            .margin({ bottom: 8 })

            Slider({
              value: this.autoPlayInterval,
              min: 2000,
              max: 15000,
              step: 1000
            })
              .blockColor('#FFD166')
              .trackColor('rgba(255,255,255,0.3)')
              .selectedColor('#FFD166')
              .onChange((value: number) => {
                this.autoPlayInterval = value;
              })
          }
          .padding({ left: 24, right: 24, bottom: 32 })
          .backgroundColor('rgba(0,0,0,0.5)')
          .borderRadius({ topLeft: 24, topRight: 24, bottomLeft: 0, bottomRight: 0 })
        }
        .width(Constants.FULL_PERCENT)
        .height(Constants.FULL_PERCENT)
      }
    }
    .width(Constants.FULL_PERCENT)
    .height(Constants.FULL_PERCENT)
    .backgroundColor(Color.Black)
  }
}
