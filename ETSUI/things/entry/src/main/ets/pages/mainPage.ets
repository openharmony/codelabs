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
import { ColorMetrics } from '@kit.ArkUI'
import router from '@ohos.router';

@Entry
@Component
export struct GalleryMainPage {
  // 页面标题
  private pageTitle: string = "图像增强引擎"
  // Swiper轮播图数据（模拟轮播图片资源，使用网络图片保证可直接运行）

  listScroller: Scroller = new Scroller();
  FullScroller: Scroller = new Scroller();
  @State scrollBarColor: ColorMetrics = ColorMetrics.rgba(24, 35, 48, 0.4);
  private swiperImages: Array<string> = [
    "app.media.01",
    "app.media.Hz301",
    "app.media.Retinex01",
    "app.media.02",
    "app.media.Hz302",
    "app.media.Retinex02",
    "app.media.03",
    "app.media.Hz303",
    "app.media.Retinex03",
    "app.media.04",
    "app.media.Hz304",
    "app.media.Retinex04",
    "app.media.05",
    "app.media.Hz305",
    "app.media.Retinex05",
    "app.media.06",
    "app.media.Hz306",
    "app.media.Retinex06",
    "app.media.07",
    "app.media.Hz307",
    "app.media.Retinex07",
    "app.media.08",
    "app.media.Hz308",
    "app.media.Retinex08",
    "app.media.09",
    "app.media.Hz309",
    "app.media.Retinex09",
    "app.media.10",
    "app.media.Hz310",
    "app.media.Retinex10",
  ]
  // 图库List列表数据（模拟多组图片分类/图片项）
  private galleryListData: Array<GalleryListItem> = [
    {
      categoryName: "低光照照片",
      categorySpecial: "原始图片",
      imageList: [
        "app.media.01",
        "app.media.02",
        "app.media.03",
        "app.media.04",
        "app.media.05",
        "app.media.06",
        "app.media.07",
        "app.media.08",
        "app.media.09",
        "app.media.10",
        "app.media.11",
        "app.media.12",
        "app.media.13",
        "app.media.14",
        "app.media.15",
        "app.media.16",
        "app.media.17",
        "app.media.18",
        "app.media.19",
        "app.media.20",
      ]
    },
    {
      categoryName: "Hz增强",
      categorySpecial: "此栏为原始图片",
      imageList: [
        "app.media.Hz301",
        "app.media.Hz302",
        "app.media.Hz303",
        "app.media.Hz304",
        "app.media.Hz305",
        "app.media.Hz306",
        "app.media.Hz307",
        "app.media.Hz308",
        "app.media.Hz309",
        "app.media.Hz310",
        "app.media.Hz311",
        "app.media.Hz312",
        "app.media.Hz313",
        "app.media.Hz314",
        "app.media.Hz315",
        "app.media.Hz316",
        "app.media.Hz317",
        "app.media.Hz318",
        "app.media.Hz319",
        "app.media.Hz320",
      ]
    },
    {
      categoryName: "Retinex方法",
      categorySpecial: "此栏为Retinex增强",
      imageList: [
        "app.media.Retinex01",
        "app.media.Retinex02",
        "app.media.Retinex03",
        "app.media.Retinex04",
        "app.media.Retinex05",
        "app.media.Retinex06",
        "app.media.Retinex07",
        "app.media.Retinex08",
        "app.media.Retinex09",
        "app.media.Retinex10",
        "app.media.Retinex11",
        "app.media.Retinex12",
        "app.media.Retinex13",
        "app.media.Retinex14",
        "app.media.Retinex15",
        "app.media.Retinex16",
        "app.media.Retinex17",
        "app.media.Retinex18",
        "app.media.Retinex19",
        "app.media.Retinex20",
      ]
    },
    {
      categoryName: "DNN",
      categorySpecial: "此栏为DNN增强",
      imageList: [
        "app.media.DeepCNN_231_300_E501",
        "app.media.DeepCNN_231_300_E502",
        "app.media.DeepCNN_231_300_E503",
        "app.media.DeepCNN_231_300_E504",
        "app.media.DeepCNN_231_300_E505",
        "app.media.DeepCNN_231_300_E506",
        "app.media.DeepCNN_231_300_E507",
        "app.media.DeepCNN_231_300_E508",
        "app.media.DeepCNN_231_300_E509",
        "app.media.DeepCNN_231_300_E510",
        "app.media.DeepCNN_231_300_E511",
        "app.media.DeepCNN_231_300_E512",
        "app.media.DeepCNN_231_300_E513",
        "app.media.DeepCNN_231_300_E514",
        "app.media.DeepCNN_231_300_E515",
        "app.media.DeepCNN_231_300_E516",
        "app.media.DeepCNN_231_300_E517",
        "app.media.DeepCNN_231_300_E518",
        "app.media.DeepCNN_231_300_E519",
        "app.media.DeepCNN_231_300_E520",
      ]
    },
    {
      categoryName: "Hz+DNN",
      categorySpecial: "使用Hz+DNN",
      imageList:[
        "app.media.ReHZreHALF_231_300_E601",
        "app.media.ReHZreHALF_231_300_E602",
        "app.media.ReHZreHALF_231_300_E603",
        "app.media.ReHZreHALF_231_300_E604",
        "app.media.ReHZreHALF_231_300_E605",
        "app.media.ReHZreHALF_231_300_E606",
        "app.media.ReHZreHALF_231_300_E607",
        "app.media.ReHZreHALF_231_300_E608",
        "app.media.ReHZreHALF_231_300_E609",
        "app.media.ReHZreHALF_231_300_E610",
        "app.media.ReHZreHALF_231_300_E611",
        "app.media.ReHZreHALF_231_300_E612",
        "app.media.ReHZreHALF_231_300_E613",
        "app.media.ReHZreHALF_231_300_E614",
        "app.media.ReHZreHALF_231_300_E615",
        "app.media.ReHZreHALF_231_300_E616",
        "app.media.ReHZreHALF_231_300_E617",
        "app.media.ReHZreHALF_231_300_E618",
        "app.media.ReHZreHALF_231_300_E619",
        "app.media.ReHZreHALF_231_300_E620",
      ]
    }
  ]
  // Swiper自动播放控制
  private swiperController: SwiperController = new SwiperController()
  // 自动播放定时器
  private autoPlayTimer: number | null = null

  private currentSwiperIndex:number =0

  private  errorThing:Array<ErrorAttention>=[
    {
      errorThing:"背景黄色噪声",
      errorDetail:"输入图片若存在大量极暗像素点使用含有HZ的方法处理将会导致大量噪声被放大大",
      imageList:[
        "app.media.Hz308",
        "app.media.Hz312",
        "app.media.Hz313",
        "app.media.Hz314",
      ]
    },
    {
      errorThing:"部分区域过曝光",
      errorDetail:"所有算法都有可能出现局部内容过曝光现象",
      imageList:[
        "app.media.ReHZreHALF_231_300_E616",
        "app.media.Hz303",
        "app.media.Hz305",
        "app.media.Hz309",
      ]
    },
    {
      errorThing:"DNN亮度增强不明显",
      errorDetail:"DNN算法在增强有些图片时会出现整体增强效果不够明显，增强幅度不够大",
      imageList:[
        "app.media.DeepCNN_231_300_E511",
        "app.media.DeepCNN_231_300_E512",
        "app.media.DeepCNN_231_300_E513",
        "app.media.DeepCNN_231_300_E514",
      ]
    }
  ]

  // 页面加载完成生命周期
  onPageShow() {
    this.startSwiperAutoPlay()
  }

  // 页面隐藏生命周期
  onPageHide() {
    this.stopSwiperAutoPlay()
  }

  // 销毁生命周期
  aboutToDisappear() {
    this.stopSwiperAutoPlay()
  }

  // 启动Swiper自动轮播
  private startSwiperAutoPlay() {
    this.autoPlayTimer = setInterval(() => {
      this.swiperController.showNext()
    }, 5000) as number
  }

  // 停止Swiper自动轮播
  private stopSwiperAutoPlay() {
    if (this.autoPlayTimer) {
      clearInterval(this.autoPlayTimer)
      this.autoPlayTimer = null
    }
  }

  // 构建页面UI
  build() {
    Stack(){
      Scroll(){
        Column() {
          // 1. 页面标题栏
          Text(this.pageTitle)
            .fontSize(24)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 20, bottom: 15 })
            .alignSelf(ItemAlign.Center)

          // 2. 循环播放Swiper组件
          Swiper(this.swiperController) {
            ForEach(this.swiperImages, (imageUrl: string) => {
              Image($r(imageUrl))
                .width('100%')
                .height(200)
                .objectFit(ImageFit.Cover)
                .borderRadius(12)
            }, (imageUrl: string) => imageUrl)
          }
          .width('90%')
          .height(200)
          .borderRadius(12)
          .indicator(true) // 显示页码指示器
          .loop(true) // 开启循环播放（核心要求）
          .autoPlay(false) // 手动控制自动播放，避免和定时器冲突
          .margin({ bottom: 20 })
          .onChange((index:number) => {
            // 切换时重置定时器，优化用户体验
            this.stopSwiperAutoPlay()
            this.startSwiperAutoPlay()
            this.currentSwiperIndex = index
          })
          .onTouch((event) => {
            if (event.type === TouchType.Down) {
              this.stopSwiperAutoPlay()
            } else if (event.type === TouchType.Up) {
              this.startSwiperAutoPlay()
            }
          })
          .onClick(() => {
            router.pushUrl({
              url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
              params: {
                imageUrl: this.swiperImages[this.currentSwiperIndex]
              }
            }).catch((e:Error) => {
              console.error("跳转全屏图片页面失败：", e)
            })
          })

          .margin({
            top: 20,
            bottom: 20,
            left: 20,
            right: 20
          })


          // 3. List框架展示图库内容（核心要求）
          List({space:10,scroller:this.listScroller}) {
            ListItem() {
              Column() {
                // 分类标题
                Text(this.galleryListData[0].categoryName)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10 })

                // 显示当前图片分类的特殊说明信息，并启用自动换行功能
                Text(this.galleryListData[0].categorySpecial)
                // 设置文本字体大小为18
                  .fontSize(18)
                  // 设置文本字体为中等加粗
                  .fontWeight(FontWeight.Medium)
                  // 设置文本左对齐
                  .alignSelf(ItemAlign.Start)
                  // 设置文本底部外边距为10
                  .margin({ bottom: 10 })
                // 启用文本自动换行功能
                // .textOverflow({ overflow: TextOverflow.Wrap })


                // 分类下的图片网格
                Scroll() {
                  Row() {
                    Image($r(this.galleryListData[0].imageList[0]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[0]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[1]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[1]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[2]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[2]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[3]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[3]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[4]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[4]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[5]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[5]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[6]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[6]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[7]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[7]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[8]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[8]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[9]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[9]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[10]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[10]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[11]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[11]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[12]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[12]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[13]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[13]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[0].imageList[14]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[0].imageList[14]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                  }
                  .width('100%')
                  .justifyContent(FlexAlign.Start)
                }
                .edgeEffect(EdgeEffect.Spring)
                .backgroundColor('#DCDCDC')
                .scrollable(ScrollDirection.Horizontal)
                .width("1700")
              }
              .width('100%')
              .padding(15)
              .backgroundColor('#f8f9fa')
              .borderRadius(12)
              .margin({ bottom: 15 })
            }

            ListItem() {
              Column() {
                // 分类标题
                Text(this.galleryListData[1].categoryName)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10 })

                // 显示当前图片分类的特殊说明信息，并启用自动换行功能
                Text(this.galleryListData[1].categorySpecial)
                // 设置文本字体大小为18
                  .fontSize(18)
                  // 设置文本字体为中等加粗
                  .fontWeight(FontWeight.Medium)
                  // 设置文本左对齐
                  .alignSelf(ItemAlign.Start)
                  // 设置文本底部外边距为10
                  .margin({ bottom: 10 })

                Row() {
                  // 第一个按钮
                  Button('上传未处理图片')
                    .width('48%')
                    .height(40)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#000000')
                    .fontFamily('宋体')
                    .backgroundColor('#87CEEB')
                    .borderRadius(8)
                    .margin({ right: '4%' })
                    .onClick(() => {
                      AlertDialog.show({
                        title: '注意',
                        message: '本功能未开发完全请等待后续版本',
                        confirm: {
                          value: '确定',
                          action: () => {
                          }
                        }
                      })
                    })
                  // 第二个按钮
                  Button('图片预览')
                    .width('48%')
                    .height(40)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#000000')
                    .fontFamily('宋体')
                    .backgroundColor('#87CEEB')
                    .borderRadius(8)
                    .onClick(() => {
                      AlertDialog.show({
                        title: '注意',
                        message: '本功能未开发完全请等待后续版本',
                        confirm: {
                          value: '确定',
                          action: () => {
                          }
                        }
                      })
                    })
                }
                .width('100%')
                .margin({ bottom: 10 })

                Scroll() {
                  Row() {
                    Stack(){
                      Image($r(this.galleryListData[1].imageList[0]))
                        .width(100)
                        .height(100)
                        .objectFit(ImageFit.Cover)
                        .borderRadius(8)
                        .margin({ right: 10 })
                    }
                    .onClick(() => {
                      router.pushUrl({
                        url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                        params: {
                          imageUrl: this.galleryListData[1].imageList[0]
                        }
                      }).catch((e:Error) => {
                        console.error("跳转全屏图片页面失败：", e)
                      })
                    })
                    Image($r(this.galleryListData[1].imageList[1]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[1]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[2]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[2]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[3]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[3]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[4]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[4]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[5]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[5]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[6]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[6]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[7]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[7]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[8]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[8]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[9]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[0]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[10]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[10]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[11]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[11]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[12]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[12]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[13]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[13]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[1].imageList[14]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[1].imageList[14]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                  }
                  .width('100%')
                  .justifyContent(FlexAlign.Start)
                }
                .edgeEffect(EdgeEffect.Spring)
                .backgroundColor('#DCDCDC')
                .scrollable(ScrollDirection.Horizontal)
                .width("1700")
              }
              .width('100%')
              .padding(15)
              .backgroundColor('#f8f9fa')
              .borderRadius(12)
              .margin({ bottom: 15 })
            }

            ListItem() {
              Column() {
                // 分类标题
                Text(this.galleryListData[2].categoryName)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10 })

                Text(this.galleryListData[2].categorySpecial)
                // 设置文本字体大小为18
                  .fontSize(18)
                  // 设置文本字体为中等加粗
                  .fontWeight(FontWeight.Medium)
                  // 设置文本左对齐
                  .alignSelf(ItemAlign.Start)
                  // 设置文本底部外边距为10
                  .margin({ bottom: 10 })
                Row() {
                  // 第一个按钮
                  Button('上传未处理图片')
                    .width('48%')
                    .height(40)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#000000')
                    .fontFamily('宋体')
                    .backgroundColor('#87CEEB')
                    .borderRadius(8)
                    .margin({ right: '4%' })
                    .onClick(() => {
                      AlertDialog.show({
                        title: '注意',
                        message: '本功能未开发完全请等待后续版本',
                        confirm: {
                          value: '确定',
                          action: () => {
                          }
                        }
                      })
                    })
                  // 第二个按钮
                  Button('图片预览')
                    .width('48%')
                    .height(40)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#000000')
                    .fontFamily('宋体')
                    .backgroundColor('#87CEEB')
                    .borderRadius(8)
                    .onClick(() => {
                      AlertDialog.show({
                        title: '注意',
                        message: '本功能未开发完全请等待后续版本',
                        confirm: {
                          value: '确定',
                          action: () => {
                          }
                        }
                      })
                    })
                }
                .width('100%')
                .margin({ bottom: 10 })
                Scroll(){
                  Row() {
                    Image($r(this.galleryListData[2].imageList[0]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[0]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[1]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[1]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[2]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[2]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[3]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[3]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[4]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[4]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[5]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[5]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[6]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[6]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[7]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[7]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[8]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[8]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[9]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[9]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[10]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[10]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[11]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[11]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[12]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[12]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[13]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[13]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[2].imageList[14]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[2].imageList[14]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                  }
                  .width('100%')
                  .justifyContent(FlexAlign.Start)
                }
                .edgeEffect(EdgeEffect.Fade)
                .backgroundColor('#DCDCDC')
                .scrollable(ScrollDirection.Horizontal)
                .width("1700")
              }
              .width('100%')
              .padding(15)
              .backgroundColor('#f8f9fa')
              .borderRadius(12)
              .margin({ bottom: 15 })
            }

            ListItem() {
              Column() {
                // 分类标题
                Text(this.galleryListData[3].categoryName)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10 })

                Text(this.galleryListData[3].categorySpecial)
                // 设置文本字体大小为18
                  .fontSize(18)
                  // 设置文本字体为中等加粗
                  .fontWeight(FontWeight.Medium)
                  // 设置文本左对齐
                  .alignSelf(ItemAlign.Start)
                  // 设置文本底部外边距为10
                  .margin({ bottom: 10 })
                Row() {
                  // 第一个按钮
                  Button('上传未处理图片')
                    .width('48%')
                    .height(40)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#000000')
                    .fontFamily('宋体')
                    .backgroundColor('#87CEEB')
                    .borderRadius(8)
                    .margin({ right: '4%' })
                    .onClick(() => {
                      AlertDialog.show({
                        title: '注意',
                        message: '本功能未开发完全请等待后续版本',
                        confirm: {
                          value: '确定',
                          action: () => {
                          }
                        }
                      })
                    })
                  // 第二个按钮
                  Button('图片预览')
                    .width('48%')
                    .height(40)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#000000')
                    .fontFamily('宋体')
                    .backgroundColor('#87CEEB')
                    .borderRadius(8)
                    .onClick(() => {
                      AlertDialog.show({
                        title: '注意',
                        message: '本功能未开发完全请等待后续版本',
                        confirm: {
                          value: '确定',
                          action: () => {
                          }
                        }
                      })
                    })
                }
                .width('100%')
                .margin({ bottom: 10 })


                Scroll(){
                  Row() {
                    Image($r(this.galleryListData[3].imageList[0]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[0]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[1]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[1]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[2]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[2]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[3]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[3]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[4]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[4]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[5]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[5]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[6]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[6]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[7]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[7]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[8]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[8]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[9]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[9]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[10]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[10]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[11]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[11]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[12]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[12]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[13]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[13]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[3].imageList[14]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[14]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                  }
                  .width('100%')
                  .justifyContent(FlexAlign.Start)
                }
                .edgeEffect(EdgeEffect.Fade)
                .backgroundColor('#DCDCDC')
                .scrollable(ScrollDirection.Horizontal)
                .width("1700")
              }
              .width('100%')
              .padding(15)
              .backgroundColor('#f8f9fa')
              .borderRadius(12)
              .margin({ bottom: 15 })
            }

            ListItem(){
              Column() {
                // 分类标题
                Text(this.galleryListData[4].categoryName)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10 })

                Text(this.galleryListData[4].categorySpecial)
                // 设置文本字体大小为18
                  .fontSize(18)
                  // 设置文本字体为中等加粗
                  .fontWeight(FontWeight.Medium)
                  // 设置文本左对齐
                  .alignSelf(ItemAlign.Start)
                  // 设置文本底部外边距为10
                  .margin({ bottom: 10 })
                Row() {
                  // 第一个按钮
                  Button('上传未处理图片')
                    .width('48%')
                    .height(40)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#000000')
                    .fontFamily('宋体')
                    .backgroundColor('#87CEEB')
                    .borderRadius(8)
                    .margin({ right: '4%' })
                    .onClick(() => {
                      AlertDialog.show({
                        title: '注意',
                        message: '本功能未开发完全请等待后续版本',
                        confirm: {
                          value: '确定',
                          action: () => {
                          }
                        }
                      })
                    })
                  // 第二个按钮
                  Button('图片预览')
                    .width('48%')
                    .height(40)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#000000')
                    .fontFamily('宋体')
                    .backgroundColor('#87CEEB')
                    .borderRadius(8)
                    .onClick(() => {
                      AlertDialog.show({
                        title: '注意',
                        message: '本功能未开发完全请等待后续版本',
                        confirm: {
                          value: '确定',
                          action: () => {
                          }
                        }
                      })
                    })
                }
                .width('100%')
                .margin({ bottom: 10 })


                Scroll(){
                  Row() {
                    Image($r(this.galleryListData[4].imageList[0]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[0]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[1]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[1]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[2]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[2]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[3]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[3]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[4]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[4]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[5]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[5]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[6]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[6]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[7]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[7]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[8]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[8]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[9]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[9]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[10]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[10]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[11]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[11]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[12]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[12]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[13]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[4].imageList[13]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.galleryListData[4].imageList[14]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.galleryListData[3].imageList[14]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                  }
                  .width('100%')
                  .justifyContent(FlexAlign.Start)
                }
                .edgeEffect(EdgeEffect.Fade)
                .backgroundColor('#DCDCDC')
                .scrollable(ScrollDirection.Horizontal)
                .width("1700")
              }
              .width('100%')
              .padding(15)
              .backgroundColor('#f8f9fa')
              .borderRadius(12)
              .margin({ bottom: 15 })
            }

            ListItem(){
              Column(){
                Text("以下为算法使用注意事项")
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                  .margin({ top: 20, bottom: 10 })
                  .width('100%')
                  .textAlign(TextAlign.Center)
                Divider()
                  .strokeWidth(1)
                  .color('#CCCCCC')
                  .margin({ bottom: 20 })
              }
            }

            ListItem(){
              Column() {
                // 分类标题
                Text(this.errorThing[0].errorThing)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10 })

                Text(this.errorThing[0].errorDetail)
                // 设置文本字体大小为18
                  .fontSize(18)
                  // 设置文本字体为中等加粗
                  .fontWeight(FontWeight.Medium)
                  // 设置文本左对齐
                  .alignSelf(ItemAlign.Start)
                  // 设置文本底部外边距为10
                  .margin({ bottom: 10 })

                Text("样例如下：")
                  .fontSize(16)
                  .fontColor('#666666')
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10, top: 10 })


                Scroll(){
                  Row() {
                    Image($r(this.errorThing[0].imageList[0]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[0].imageList[0]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[0].imageList[1]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[0].imageList[1]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[0].imageList[2]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[0].imageList[2]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[0].imageList[3]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[0].imageList[3]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                  }
                  .width('100%')
                  .justifyContent(FlexAlign.Start)
                }
                .edgeEffect(EdgeEffect.Fade)
                .backgroundColor('#DCDCDC')
                .scrollable(ScrollDirection.Horizontal)
                .width("450")
              }
              .width('100%')
              .padding(15)
              .backgroundColor('#f8f9fa')
              .borderRadius(12)
              .margin({ bottom: 15 })
            }

            ListItem(){
              Column() {
                // 分类标题
                Text(this.errorThing[1].errorThing)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10 })

                Text(this.errorThing[1].errorDetail)
                // 设置文本字体大小为18
                  .fontSize(18)
                  // 设置文本字体为中等加粗
                  .fontWeight(FontWeight.Medium)
                  // 设置文本左对齐
                  .alignSelf(ItemAlign.Start)
                  // 设置文本底部外边距为10
                  .margin({ bottom: 10 })

                Text("样例如下：")
                  .fontSize(16)
                  .fontColor('#666666')
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10, top: 10 })


                Scroll(){
                  Row() {
                    Image($r(this.errorThing[1].imageList[0]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[1].imageList[0]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[1].imageList[1]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[1].imageList[1]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[1].imageList[2]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[1].imageList[2]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[1].imageList[3]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[1].imageList[3]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                  }
                  .width('100%')
                  .justifyContent(FlexAlign.Start)
                }
                .edgeEffect(EdgeEffect.Fade)
                .backgroundColor('#DCDCDC')
                .scrollable(ScrollDirection.Horizontal)
                .width("450")
              }
              .width('100%')
              .padding(15)
              .backgroundColor('#f8f9fa')
              .borderRadius(12)
              .margin({ bottom: 15 })
            }

            ListItem(){
              Column() {
                // 分类标题
                Text(this.errorThing[2].errorThing)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10 })

                Text(this.errorThing[2].errorDetail)
                // 设置文本字体大小为18
                  .fontSize(18)
                  // 设置文本字体为中等加粗
                  .fontWeight(FontWeight.Medium)
                  // 设置文本左对齐
                  .alignSelf(ItemAlign.Start)
                  // 设置文本底部外边距为10
                  .margin({ bottom: 10 })

                Text("样例如下：")
                  .fontSize(16)
                  .fontColor('#666666')
                  .fontWeight(FontWeight.Medium)
                  .alignSelf(ItemAlign.Start)
                  .margin({ bottom: 10, top: 10 })


                Scroll(){
                  Row() {
                    Image($r(this.errorThing[2].imageList[0]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[2].imageList[0]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[2].imageList[1]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[2].imageList[1]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[2].imageList[2]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[2].imageList[2]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                    Image($r(this.errorThing[2].imageList[3]))
                      .width(100)
                      .height(100)
                      .objectFit(ImageFit.Cover)
                      .borderRadius(8)
                      .margin({ right: 10 })
                      .onClick(() => {
                        router.pushUrl({
                          url: "pages/fullPage", // 注意：对应你的页面存放路径，需在config.json中配置
                          params: {
                            imageUrl: this.errorThing[2].imageList[3]
                          }
                        }).catch((e:Error) => {
                          console.error("跳转全屏图片页面失败：", e)
                        })
                      })
                  }
                  .width('100%')
                  .justifyContent(FlexAlign.Start)
                }
                .edgeEffect(EdgeEffect.Fade)
                .backgroundColor('#DCDCDC')
                .scrollable(ScrollDirection.Horizontal)
                .width("450")
              }
              .width('100%')
              .padding(15)
              .backgroundColor('#f8f9fa')
              .borderRadius(12)
              .margin({ bottom: 15 })
            }
          }
          .width('90%')
          .flexGrow(1)
          .divider({ strokeWidth: 0 }) // 隐藏List默认分割线
          .scrollBar(BarState.Off)
          .nestedScroll({
            scrollForward: NestedScrollMode.PARENT_FIRST,
            scrollBackward: NestedScrollMode.SELF_FIRST
          })
          .height('100%')
        }
        .width('100%')
        .height('100%')
        .backgroundColor('#f0f2f5')
        .padding({ left: 10, right: 10 })
      }
      .edgeEffect(EdgeEffect.Spring)
      .backgroundColor('#DCDCDC')
      .scrollBar(BarState.Off)
      .width('100%')
      .height('2600')

      ScrollBar({scroller:this.listScroller})
        .position({ right: 0 })
        .enableNestedScroll(true)
        .scrollBarColor(this.scrollBarColor)
    }
  }
}

// 图库列表项数据模型
interface GalleryListItem {
  categoryName: string // 图片分类名称
  categorySpecial: string //说明
  imageList: Array<string> // 分类下的图片列表
}

interface ErrorAttention{
  errorThing:string
  errorDetail:string
  imageList:Array<string>
}