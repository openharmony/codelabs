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
//Index.ets
import {  router } from '@kit.ArkUI';

// 定义菜品数据类型
export interface Dish {
  id: string;
  name: string;
  description: string;
  image: Resource;
  ingredients: string;
  seasonings: string;
  routeUrl: string; // 新增路由路径字段
}

// 定义收藏列表类型
type FavoriteList = Dish[];

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  myscr: Scroller = new Scroller();
  myscrhor: Scroller = new Scroller();
  @State currentIndex: number = 0;
  @State cur: number = 0;
  @State expwidth: string = "60%";
  @State dewidth: string = "30%";
  @State button1width: string = this.expwidth;
  @State button2width: string = this.dewidth;
  myindic: DotIndicator = new DotIndicator();
  myswiscr: SwiperController = new SwiperController();
  @StorageProp('favoriteList') favorites: FavoriteList = [];
  @StorageProp('historyList') histories: FavoriteList = [];
  @StorageProp('likeList') likes: Dish[] = []; // 点赞列表

  @Styles myfun() {
    .width('95%').height('22%')
    .borderRadius(10).backgroundImageSize(ImageSize.FILL)
  }

  @Styles mystl() {
    .width('100%').height('20%')
  }

  @Styles stlfull() {
    .width('100%').height('50%')
  }

  @Styles dished() {
    .height('100%').width('40%').borderRadius(10)
    .margin({ left: 10 }).backgroundImageSize(ImageSize.FILL)
  }

  @Builder mytabbar(title: string, Index: number, selectedimg: Resource, defimg: Resource) {
    Column() {
      Text().height(45).width(45)
        .backgroundImage(Index === this.currentIndex? selectedimg : defimg).backgroundImageSize(ImageSize.FILL)
      Text(title).fontSize(14).textAlign(TextAlign.Center).fontColor(Index === this.currentIndex? Color.Red : Color.Black)
        .height(20).width(60)
    }
  }

  @Builder FavoriteList() {
    Column() {
      if (this.favorites.length === 0) {
        Column() {
          Text().width(170).height(170).backgroundImage($r('app.media.kong')).backgroundImageSize(ImageSize.FILL)
          Text('暂无收藏').margin({top:10}).fontColor('#9a9a9a')
        }
      } else {
        Scroll() {
          Column() {
            ForEach(this.favorites, (item: Dish) => {
              Row() {
                Image(item.image)
                  .height(120)
                  .width(120)
                  .margin({ left: 10 })
                  .borderRadius(10)
                  .objectFit(ImageFit.Cover)

                Column() {
                  Text(item.name)
                    .fontSize(18)
                    .fontWeight(FontWeight.Bold)
                    .margin({ bottom: 5 })

                  Text(item.description)
                    .fontSize(14)
                    .fontColor('#666666')
                    .margin({ bottom: 8 })

                  Text(`主要食材: ${item.ingredients}`)
                    .fontSize(12)
                    .fontColor('#888888')
                    .margin({ bottom: 5 })

                  Text(`调料: ${item.seasonings}`)
                    .fontSize(12)
                    .fontColor('#888888')
                }
                .margin({ left: 15 })
                .layoutWeight(1)
              }
              .width('100%')
              .height(140)
              .backgroundColor(Color.White)
              .borderRadius(10)
              .margin({ top: 10 })
              // 原代码中的点击事件修改为：
              .onClick(() => {
                router.pushUrl({
                  url: 'pages/dishDetail',
                  params: {
                    id: item.id,
                    name: item.name,
                    description: item.description,
                    image: item.image.id, // 关键！传递资源ID
                    ingredients: item.ingredients,
                    seasonings: item.seasonings
                  }
                });
              })
            })
          }
          .padding(10)
          .width('100%')
        }
      }
    }
  }

  @Builder HistoryList() {
    Column() {
      if (this.histories.length === 0) {
        Column() {
          Text().width(170).height(170).backgroundImage($r('app.media.kong')).backgroundImageSize(ImageSize.FILL)
          Text('暂无浏览历史').margin({top:10}).fontColor('#9a9a9a')
        }
      } else {
        Scroll() {
          Column() {
            ForEach(this.histories, (item: Dish) => {
              Row() {
                Image(item.image)
                  .height(120)
                  .width(120)
                  .margin({ left: 10 })
                  .borderRadius(10)
                  .objectFit(ImageFit.Cover)

                Column() {
                  Text(item.name)
                    .fontSize(18)
                    .fontWeight(FontWeight.Bold)
                    .margin({ bottom: 5 })

                  Text(item.description)
                    .fontSize(14)
                    .fontColor('#666666')
                    .margin({ bottom: 8 })

                  Text(`主要食材: ${item.ingredients}`)
                    .fontSize(12)
                    .fontColor('#888888')
                    .margin({ bottom: 5 })

                  Text(`调料: ${item.seasonings}`)
                    .fontSize(12)
                    .fontColor('#888888')
                }
                .margin({ left: 15 })
                .layoutWeight(1)
              }
              .width('100%')
              .height(140)
              .backgroundColor(Color.White)
              .borderRadius(10)
              .margin({ top: 10 })
              .onClick(() => {
                router.pushUrl({
                  url: 'pages/dishDetail',
                  params: {
                    id: item.id,
                    name: item.name,
                    description: item.description,
                    image: item.image.id,
                    ingredients: item.ingredients,
                    seasonings: item.seasonings
                  }
                });
              })
            })
          }
          .padding(10)
          .width('100%')
        }
      }
    }
  }

  build() {
    Tabs() {
      // 推荐页面
      TabContent() {
        Column() {
          // 顶部搜索框
          Row({ space: 10 }) {

            Search({ placeholder: '搜索菜谱、食材' })
              .backgroundColor('#efefef')
              .width(240)
              .height('60%')
              .onSubmit((value: string) => {

                let searchUrl = `https://www.xiachufang.com/search/?keyword=${encodeURIComponent(value)}`;
                router.pushUrl({
                  url: 'pages/web', // 你需要创建一个Web页面组件
                  params: {
                    url: searchUrl
                  }
                });
              })

          }
          .height('8%')
          .width('100%')
          .margin({ top: 5 })
          .justifyContent(FlexAlign.Center)

          // 滚动页面
          Column() {
            Scroll(this.myscr) {
              Column() {
                Stack({ alignContent: Alignment.Top }) {
                  Swiper(this.myswiscr) {

                    Column() {
                      Text('今日早餐')
                        .fontWeight(FontWeight.Bold)
                        .width('100%')
                        .height('9%')
                        .padding({ left: 10 })
                        .fontSize(25)

                      Row({ space: 10 }) {
                        // 左边大图 - 高度增加到320
                        Column() {
                          Text()
                            .height(340) // 增加高度
                            .width('100%')
                            .backgroundImage($r('app.media.xiaomi')) // 主推菜品图片
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/xiaomi',
                              })
                            })
                        }
                        .width('62%')

                        // 右边两小图 - 也改为长方形
                        Column({ space: 10 }) {
                          // 上方小图 - 高度增加并保持长方形比例
                          Text()
                            .height(165) // 增加高度 (320总高度减去间距10再除以2)
                            .width('100%')
                            .backgroundImage($r('app.media.baozi'))
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/baozi',
                              })
                            })

                          // 下方小图 - 高度增加并保持长方形比例
                          Text()
                            .height(165)
                            .width('100%')
                            .backgroundImage($r('app.media.bing'))
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/bing',
                              })
                            })
                        }
                        .width('34%')
                      }
                      .width('95%')
                      .margin({ top: 10 })
                      .height(340) // 调整整体高度以匹配
                    }

                    Column() {
                      Text('今日午餐')
                        .fontWeight(FontWeight.Bold)
                        .width('100%')
                        .height('9%')
                        .padding({ left: 10 })
                        .fontSize(25)
                      Row({ space: 10 }) {
                        // 左边大图
                        Column() {
                          Text()
                            .height(340) // 高度调整为占满右侧两图高度
                            .width('100%')
                            .backgroundImage($r('app.media.yu')) // 主推菜品图片
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/yu',
                              })
                            })
                        }
                        .width('64%')

                        // 右边两小图
                        Column({ space: 10 }) {
                          // 上方小图
                          Text()
                            .height(165) // 280总高度减去间距10
                            .width('100%')
                            .backgroundImage($r('app.media.jichi')) // 早餐小食1
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/jichi',
                              })
                            })

                          // 下方小图
                          Text()
                            .height(165)
                            .width('100%')
                            .backgroundImage($r('app.media.jiyou')) // 早餐小食2
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/jiyou',
                              })
                            })
                        }
                        .width('32%')
                      }
                      .width('95%')
                      .margin({ top: 10 })
                      .height(340) // 固定高度确保对齐
                    }

                    Column() {
                      Text('今日下午茶')
                        .fontWeight(FontWeight.Bold)
                        .width('100%')
                        .height('9%')
                        .padding({ left: 10 })
                        .fontSize(25)
                      Row({ space: 10 }) {
                        // 左边大图
                        Column() {
                          Text()
                            .height(340) // 高度调整为占满右侧两图高度
                            .width('100%')
                            .backgroundImage($r('app.media.wanzi')) // 主推菜品图片
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/wanzi',
                              })
                            })
                        }
                        .width('64%')

                        // 右边两小图
                        Column({ space: 10 }) {
                          // 上方小图
                          Text()
                            .height(165) // 280总高度减去间距10
                            .width('100%')
                            .backgroundImage($r('app.media.suannai')) // 早餐小食1
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/suannai',
                              })
                            })

                          // 下方小图
                          Text()
                            .height(165)
                            .width('100%')
                            .backgroundImage($r('app.media.bingtang')) // 早餐小食2
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/bingtang',
                              })
                            })
                        }
                        .width('32%')
                      }
                      .width('95%')
                      .margin({ top: 10 })
                      .height(340) // 固定高度确保对齐
                    }

                    Column() {
                      Text('今日晚餐')
                        .fontWeight(FontWeight.Bold)
                        .width('100%')
                        .height('9%')
                        .padding({ left: 10 })
                        .fontSize(25)
                      Row({ space: 10 }) {
                        // 左边大图
                        Column() {
                          Text()
                            .height(340) // 高度调整为占满右侧两图高度
                            .width('100%')
                            .backgroundImage($r('app.media.paiguguo')) // 主推菜品图片
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/paiguguo',
                              })
                            })
                        }
                        .width('64%')

                        // 右边两小图
                        Column({ space: 10 }) {
                          // 上方小图
                          Text()
                            .height(165) // 280总高度减去间距10
                            .width('100%')
                            .backgroundImage($r("app.media.xiangcai")) // 早餐小食1
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/xiangcai',
                              })
                            })

                          // 下方小图
                          Text()
                            .height(165)
                            .width('100%')
                            .backgroundImage($r('app.media.youcai')) // 早餐小食2
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/youcai',
                              })
                            })
                        }
                        .width('32%')
                      }
                      .width('95%')
                      .margin({ top: 10 })
                      .height(340) // 固定高度确保对齐
                    }

                    Column() {
                      Text('今日夜宵')
                        .fontWeight(FontWeight.Bold)
                        .width('100%')
                        .height('9%')
                        .padding({ left: 10 })
                        .fontSize(25)
                      Row({ space: 10 }) {
                        // 左边大图
                        Column() {
                          Text()
                            .height(340) // 高度调整为占满右侧两图高度
                            .width('100%')
                            .backgroundImage($r('app.media.jichui')) // 主推菜品图片
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/jichui',
                              })
                            })
                        }
                        .width('64%')

                        // 右边两小图
                        Column({ space: 10 }) {
                          // 上方小图
                          Text()
                            .height(165) // 280总高度减去间距10
                            .width('100%')
                            .backgroundImage($r('app.media.shala')) // 早餐小食1
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/shala',
                              })
                            })

                          // 下方小图
                          Text()
                            .height(165)
                            .width('100%')
                            .backgroundImage($r('app.media.kaodoufu')) // 早餐小食2
                            .backgroundImageSize(ImageSize.Cover)
                            .borderRadius(10)
                            .onClick(() => {
                              router.pushUrl({
                                url: 'newpages/kaodoufu',
                              })
                            })
                        }
                        .width('32%')
                      }
                      .width('95%')
                      .margin({ top: 10 })
                      .height(340) // 固定高度确保对齐
                    }

                  }
                  .autoPlay(true)
                  .indicator(this.myindic.selectedColor(Color.Red))
                  .indicator(this.myindic.selectedItemWidth(10))
                  .indicator(this.myindic.itemWidth(10))
                  .indicator(this.myindic.itemHeight(10))

                  Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.End, alignItems: ItemAlign.Center }) {
                    Text('更多食谱推荐 >')
                      .fontWeight(FontWeight.Bold)
                      .width('40%')
                      .height('75%')
                      .backgroundColor('#ffe854')
                      .textAlign(TextAlign.Center)
                      .margin({ right: 15 })
                      .fontSize(16)
                      .borderRadius(40)
                      .onClick(() => {
                        router.pushUrl({
                          url: 'pages/web', // 使用同一个Web页面组件
                          params: {
                            url: 'https://www.xiachufang.com/category/'
                          }
                        });
                      })
                  }
                  .width('100%')
                  .height('8%')
                }

                Column() {
                  Text('二十四节气养生食谱')
                    .backgroundColor(Color.Green)
                    .fontSize(15)
                    .fontColor(Color.White)
                    .height('10%')
                    .width(150)
                    .textAlign(TextAlign.Center)
                    .alignSelf(ItemAlign.Start)
                    .borderRadius({ topLeft: 15, bottomRight: 15 })
                  Text('立夏').fontWeight(FontWeight.Bold).fontSize(30).height('15%')
                  Text('5月5日-5月20日').height('8%').fontSize(12).fontColor('#b0b0b0')
                  Row({ space: 10 }) {
                    Text("鸭蛋")
                      .width('12%')
                      .height(25)
                      .textAlign(TextAlign.Center)
                      .borderRadius(20)
                      .fontColor('#6a9c5e')
                      .backgroundColor('#eff3ee')
                    Text("桂圆")
                      .width('12%')
                      .height(25)
                      .textAlign(TextAlign.Center)
                      .borderRadius(20)
                      .fontColor('#6a9c5e')
                      .backgroundColor('#eff3ee')
                    Text("苋菜")
                      .width('12%')
                      .height(25)
                      .textAlign(TextAlign.Center)
                      .borderRadius(20)
                      .fontColor('#6a9c5e')
                      .backgroundColor('#eff3ee')
                    Text("蒜苔")
                      .width('12%')
                      .height(25)
                      .textAlign(TextAlign.Center)
                      .borderRadius(20)
                      .fontColor('#6a9c5e')
                      .backgroundColor('#eff3ee')
                    Text("樱桃")
                      .width('12%')
                      .height(25)
                      .textAlign(TextAlign.Center)
                      .borderRadius(20)
                      .fontColor('#6a9c5e')
                      .backgroundColor('#eff3ee')
                  }
                  .height('10%')
                  .margin({ top: '2%' })

                  Row({ space: 10 }) {
                    Row() {
                      Text('补夏强身，要喝莲子粥。')
                        .width('90%')
                        .height('100%')
                        .fontSize(18)
                        .fontColor('#6f6564')
                      Text('>')
                        .width('10%')
                        .height('100%')
                        .fontSize(18)
                        .fontColor('#6f6564')
                    }
                    .width('48%')
                    .height('97%')
                    .borderRadius(10)
                    .backgroundColor('#ffedeb')

                    Row() {
                      Text('健脾养胃，多喝滋补汤。')
                        .width('90%')
                        .height('100%')
                        .fontSize(18)
                        .fontColor('#6f6564')
                      Text('>')
                        .width('10%')
                        .height('100%')
                        .fontSize(18)
                        .fontColor('#6f6564')
                    }
                    .width('48%')
                    .height('97%')
                    .borderRadius(10)
                    .backgroundColor('#fef4ea')
                  }
                  .height('25%')
                  .width('100%')
                  .justifyContent(FlexAlign.Center)
                  .margin({ top: 5 })

                  Row({ space: 10 }) {
                    Row() {
                      Text('清热利湿，绿豆要多吃。')
                        .width('90%')
                        .height('100%')
                        .fontSize(18)
                        .fontColor('#6f6564')
                      Text('>')
                        .width('10%')
                        .height('100%')
                        .fontSize(18)
                        .fontColor('#6f6564')
                    }
                    .width('48%')
                    .height('97%')
                    .borderRadius(10)
                    .backgroundColor('#f6f3fe')

                    Row() {
                      Text('养心安神，多吃红色食物。')
                        .width('90%')
                        .height('100%')
                        .fontSize(18)
                        .fontColor('#6f6564')
                      Text('>')
                        .width('10%')
                        .height('100%')
                        .fontSize(18)
                        .fontColor('#6f6564')
                    }
                    .width('48%')
                    .height('97%')
                    .borderRadius(10)
                    .backgroundColor('#edf3ff')
                  }
                  .height('25%')
                  .width('100%')
                  .justifyContent(FlexAlign.Center)
                  .margin({ top: 5 })
                }
                .backgroundColor('#fdfdfd')
                .height('41%')
                .width('95%')
                .borderRadius(15)
                .margin({ top: 10 })
                .onClick(() => {
                  router.pushUrl({
                    url: 'pages/myfun'
                  })
                })

                Column() {
                  Button({ type: ButtonType.Normal })
                    .height('23%')
                    .width('95%')
                    .margin({ top: 5 })
                    .backgroundImage($r('app.media.lixia1'))
                    .backgroundImageSize(ImageSize.FILL)
                    .onClick(() => {
                      router.pushUrl({
                        url: 'pages/myfun'
                      })
                    })
                  Text('每日热门菜谱视频')
                    .height('7%')
                    .width('50%')
                    .fontSize(20)
                    .fontWeight(FontWeight.Bold)
                    .backgroundColor(Color.White)
                    .alignSelf(ItemAlign.Start)
                    .padding({ left: 10 })
                  Scroll(this.myscrhor) {
                    Row({ space: 10 }) {
                      Text()
                        .height('100%')
                        .width('40%')
                        .margin({ left: 10 })
                        .backgroundImage($r('app.media.ji'))
                        .backgroundImageSize(ImageSize.Contain)
                        .onClick(() => {
                          router.pushUrl({
                            url: 'pages/web',
                            params:{
                              url:'https://tv.sohu.com/v/dXMvMzM4NDUwMTk5LzQ1NzI2OTU5MS5zaHRtbA==.html'
                            }
                          })
                        })
                      Text()
                        .height('100%')
                        .width('40%')
                        .backgroundImage($r('app.media.yang'))
                        .backgroundImageSize(ImageSize.Contain)
                        .onClick(() => {
                          router.pushUrl({
                            url: 'pages/web',
                            params:{
                              url:'https://tv.sohu.com/v/dXMvMzM1OTQxODM4LzI2MTQzMjUzNi5zaHRtbA==.html'
                            }
                          })
                        })
                      Text()
                        .height('100%')
                        .width('40%')
                        .backgroundImage($r('app.media.yangrou'))
                        .backgroundImageSize(ImageSize.Contain)
                        .onClick(() => {
                          router.pushUrl({
                            url: 'pages/web',
                            params:{
                              url:'https://tv.sohu.com/v/dXMvMzM4OTE1OTQ0LzQxNjc4MTU5Ny5zaHRtbA==.html'
                            }
                          })
                        })
                      Text()
                        .height('100%')
                        .width('40%')
                        .margin({ right: 10 })
                        .backgroundImage($r('app.media.muer'))
                        .backgroundImageSize(ImageSize.Contain)
                        .onClick(() => {
                          router.pushUrl({
                            url: 'pages/web',
                            params:{
                              url:'https://tv.sohu.com/v/dXMvNDkxNjMwNDcvMTg2NTY0MjAuc2h0bWw=.html'
                            }
                          })
                        })
                    }
                    .height("33%")
                    .justifyContent(FlexAlign.Center)
                  }
                  .scrollable(ScrollDirection.Horizontal)
                  .scrollBar(BarState.Off)

                  Row() {
                    Text()
                      .height('100%')
                      .width('40%')
                      .margin({ left: 10 })
                      .backgroundImage($r('app.media.jianbao'))
                      .backgroundImageSize(ImageSize.Contain)
                    Column() {
                      Text('饺子皮版水煎包')
                        .height('20%')
                        .width('100%')
                        .fontSize(20)
                      Row({ space: 10 }) {
                        Text('饺子皮')
                          .height('100%')
                          .width('30%')
                          .fontSize(13)
                          .backgroundColor('#fdf1c7')
                          .textAlign(TextAlign.Center)
                          .borderRadius(10)
                        Text('粉丝')
                          .height('100%')
                          .width('18%')
                          .fontSize(13)
                          .backgroundColor('#fdf1c7')
                          .textAlign(TextAlign.Center)
                          .borderRadius(10)
                        Text('胡萝卜')
                          .height('100%')
                          .width('25%')
                          .fontSize(13)
                          .backgroundColor('#fdf1c7')
                          .textAlign(TextAlign.Center)
                          .borderRadius(10)
                      }
                      .height('15%')
                      .width('100%')
                    }
                    .height('100%')
                    .width('50%')
                    .margin({ left: 10 })
                  }
                  .height('24%')
                  .width('100%')
                  .margin({ top: 10 })
                  .onClick(() => {
                    router.pushUrl({
                      url: 'newpages/jiaozi'
                    })
                  })

                  Row() {
                    Text()
                      .height('100%')
                      .width('40%')
                      .margin({ left: 10 })
                      .backgroundImage($r('app.media.mogu'))
                      .backgroundImageSize(ImageSize.Contain)
                    Column() {
                      Text('椒盐蘑菇')
                        .height('20%')
                        .width('100%')
                        .fontSize(20)
                      Row({ space: 10 }) {
                        Text('平菇')
                          .height('100%')
                          .width('25%')
                          .fontSize(13)
                          .backgroundColor('#fdf1c7')
                          .textAlign(TextAlign.Center)
                          .borderRadius(10)
                      }
                      .height('15%')
                      .width('100%')
                      .backgroundColor(Color.White)
                    }
                    .height('100%')
                    .width('50%')
                    .margin({ left: 10 })
                  }
                  .height('24%')
                  .width('100%')
                  .margin({ top: 7 })
                  .onClick(() => {
                    router.pushUrl({
                      url: 'newpages/muer'
                    })
                  })
                }
                .width('100%')
              }
              .width('100%')
            }
            .scrollable(ScrollDirection.Vertical)
            .scrollBar(BarState.Off)
          }
          .height('92%')
          .width('100%')
        }
        .width('100%')
      }
      .tabBar(this.mytabbar('推荐', 0, $r('app.media.home_selected'), $r('app.media.home_normal')))

      // 热门榜
      TabContent() {
        Scroll() {
          Column() {
            Text()
              .width('100%')
              .height('25%')
              .backgroundImage($r('app.media.PaihbTop'))
              .backgroundImageSize(ImageSize.FILL)
            Row() {
              Text()
                .backgroundImage($r('app.media.TuDouNiuRou'))
                .dished()
              Column() {
                Text('土豆炖牛肉')
                  .height('20%')
                  .width('100%')
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                Row({ space: 10 }) {
                  Text('土豆')
                    .height('100%')
                    .width('18%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                  Text('牛肉')
                    .height('100%')
                    .width('18%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                }
                .height('15%')
                .width('100%')
              }
              .height('100%')
              .width('50%')
              .margin({ left: 10 })
            }
            .mystl()
            .onClick(() => {
              router.pushUrl({
                url: 'newpages/tudouniurou'
              })
            })

            Row() {
              Text()
                .backgroundImage($r('app.media.PaiGu'))
                .dished()
              Column() {
                Text('红烧排骨')
                  .height('20%')
                  .width('100%')
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                Text('蛋白质含量高')
                  .fontSize(14)
                  .width('100%')
                Row({ space: 10 }) {
                  Text('猪小排')
                    .height('100%')
                    .width('30%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                }
                .height('15%')
                .width('100%')
                .margin({ top: 8 })
              }
              .height('100%')
              .width('50%')
              .margin({ left: 10 })
            }
            .mystl()
            .margin({ top: 20 })
            .onClick(() => {
              router.pushUrl({
                url: 'newpages/paigu'
              })
            })

            Row() {
              Text()
                .backgroundImage($r('app.media.QieZi'))
                .dished()
              Column() {
                Text('素烧茄子')
                  .height('20%')
                  .width('100%')
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                Row() {
                  Text('茄子(紫皮，长)')
                    .height('100%')
                    .width('60%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                }
                .height('15%')
                .width('100%')
              }
              .height('100%')
              .width('50%')
              .margin({ left: 10 })
            }
            .mystl()
            .margin({ top: 20 })
            .onClick(() => {
              router.pushUrl({
                url: 'newpages/qiezi'
              })
            })

            Row() {
              Text()
                .backgroundImage($r('app.media.XianXiaWanZi'))
                .dished()
              Column() {
                Text('鲜虾丸子')
                  .height('20%')
                  .width('100%')
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                Row({ space: 10 }) {
                  Text('虾肉')
                    .height('100%')
                    .width('18%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                }
                .height('15%')
                .width('100%')
              }
              .height('100%')
              .width('50%')
              .margin({ left: 10 })
            }
            .mystl()
            .margin({ top: 20 })
            .onClick(() => {
              router.pushUrl({
                url: 'newpages/xiawan'
              })
            })

            Row() {
              Text()
                .backgroundImage($r('app.media.doufu'))
                .dished()
              Column() {
                Text('麻婆豆腐')
                  .height('20%')
                  .width('100%')
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                Text('维E对改善血夜循环，加速烧伤冻伤的愈合有作用')
                  .fontSize(14)
                  .width('100%')
                Row({ space: 10 }) {
                  Text('豆腐')
                    .height('100%')
                    .width('18%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                  Text('肉末')
                    .height('100%')
                    .width('18%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                  Text('小葱')
                    .height('100%')
                    .width('18%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                  Text('大蒜')
                    .height('100%')
                    .width('18%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                }
                .height('15%')
                .width('100%')
                .margin({ top: 8 })
                Row({ space: 10 }) {
                  Text('郫县豆瓣酱')
                    .height('100%')
                    .width('45%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                }
                .height('15%')
                .width('100%')
                .margin({ top: 8 })
              }
              .height('100%')
              .width('50%')
              .margin({ left: 10 })
            }
            .mystl()
            .margin({ top: 20 })
            .onClick(() => {
              router.pushUrl({
                url: 'newpages/doufu'
              })
            })

            Row() {
              Text()
                .backgroundImage($r('app.media.tang'))
                .dished()
              Column() {
                Text('疙瘩汤')
                  .height('20%')
                  .width('100%')
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                Row({ space: 10 }) {
                  Text('面粉')
                    .height('100%')
                    .width('18%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                }
                .height('15%')
                .width('100%')
              }
              .height('100%')
              .width('50%')
              .margin({ left: 10 })
            }
            .mystl()
            .margin({ top: 20 })
            .onClick(() => {
              router.pushUrl({
                url: 'newpages/tang'
              })
            })

            Row() {
              Text()
                .backgroundImage($r('app.media.laziji'))
                .dished()
              Column() {
                Text('辣子鸡')
                  .height('20%')
                  .width('100%')
                  .fontSize(20)
                  .fontWeight(FontWeight.Bold)
                Row({ space: 10 }) {
                  Text('黑脚土鸡')
                    .height('100%')
                    .width('35%')
                    .fontSize(13)
                    .backgroundColor('#fdf1c7')
                    .textAlign(TextAlign.Center)
                    .borderRadius(10)
                }
                .height('15%')
                .width('100%')
              }
              .height('100%')
              .width('50%')
              .margin({ left: 10 })
            }
            .mystl()
            .margin({ top: 20 })
            .onClick(() => {
              router.pushUrl({
                url: 'newpages/laziji'
              })
            })

            Row() {
              Text('更多内容')
                .fontSize(16)
                .height('100%') // Make text take full height of row
                .margin({ right: 8 }) // Add some spacing between texts

              Text("==>>")
                .fontSize(16)
                .fontColor(Color.Blue)
                .decoration({ type: TextDecorationType.Underline, color: Color.Blue })
                .height('100%') // Make text take full height of row
                .onClick(() => {
                  router.pushUrl({
                    url: 'pages/web',
                    params: {
                      url: 'https://www.xiachufang.com/explore/'
                    }
                  })
                })
            }
            .height(60)
            .alignItems(VerticalAlign.Center)

          }
          .width('100%')
        }
      }
      .tabBar(this.mytabbar('热门榜', 1, $r('app.media.Paihb2'), $r('app.media.Paihb1')))

      // 我的
      TabContent() {
        Column() {
          Column() {
            Row({ space: 10 }) {
              Image($r('app.media.shezhi'))
                .height(35)
                .width(35)
                .margin({ right: 10 })
                .objectFit(ImageFit.Fill)
                .onClick(() => {
                  router.pushUrl({
                    url: 'pages/set up'
                  })
                })
            }
            // .height('12%')
            .width('100%')
            .justifyContent(FlexAlign.End)
            Row() {
              Text("Hi，\ncc")
                .fontSize(30)
                .fontWeight(FontWeight.Bold)
                .margin({ left: 35 })
              Image($r('app.media.cat'))
                .height(130)
                .width(130)
                .borderRadius(90)
                .backgroundColor(Color.Green)
                .margin({ top: '200', left: '30' })
                .objectFit(ImageFit.Fill)
                .onClick(() => {
                  router.pushUrl({
                    url: 'pages/edit'
                  })
                })
            }
            .height(100)
            // .height('20%')
            .width('100%')
            .margin({ top: 10 })

            Row() {
              Column() {
                Text("0")
                  .fontWeight(FontWeight.Bold)
                  .fontSize(25)
                  .width('23%')
                  .textAlign(TextAlign.Center)
                  .height('50%')
                Text("访客")
                  .fontSize(15)
                  .width('23%')
                  .textAlign(TextAlign.Center)
                  .height('30%')
              }
              .height('100%')
              Column() {
                Text("0")
                  .fontWeight(FontWeight.Bold)
                  .fontSize(25)
                  .width('23%')
                  .textAlign(TextAlign.Center)
                  .height('50%')
                Text("粉丝")
                  .fontSize(15)
                  .width('23%')
                  .textAlign(TextAlign.Center)
                  .height('30%')
              }
              .height('100%')
              Column() {
                Text("0")
                  .fontWeight(FontWeight.Bold)
                  .fontSize(25)
                  .width('23%')
                  .textAlign(TextAlign.Center)
                  .height('50%')
                Text("关注")
                  .fontSize(15)
                  .width('23%')
                  .textAlign(TextAlign.Center)
                  .height('30%')
              }
              .height('100%')
              Column() {
                Text("0")
                  .fontWeight(FontWeight.Bold)
                  .fontSize(25)
                  .width('23%')
                  .textAlign(TextAlign.Center)
                  .height('50%')
                Text("发布")
                  .fontSize(15)
                  .width('23%')
                  .textAlign(TextAlign.Center)
                  .height('30%')
              }
              .height('100%')
            }
            // .height('18%')
            .height(50)
            .width('100%')
            .justifyContent(FlexAlign.Center)
            .margin({ top: '45%' })
          }
          .backgroundColor(Color.Orange)
          // .stlfull()
          .width("100%").height(420)
          .backgroundImage($r('app.media.context2'))
          .backgroundImageSize(ImageSize.FILL)

          Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
            Button({ type: ButtonType.Normal }) {
              Text('我的收藏')
                .fontWeight(this.cur === 0 ? FontWeight.Bold : FontWeight.Normal)
                .fontSize(20)
            }
            .onClick(() => {
              this.cur = 0;
              this.button1width = this.expwidth;
              this.button2width = this.dewidth;
            })
            .width(this.button1width)
            .height('95%')
            .margin({ left: 10 })
            .backgroundColor(this.cur === 0 ? '#ffe854' : '#eef2f1')
            .borderRadius(10)
            .animation({
              duration: 100,
              curve: Curve.Linear,
              iterations: 1,
            })
            Button({ type: ButtonType.Normal }) {
              Text('浏览历史')
                .fontWeight(this.cur === 1 ? FontWeight.Bold : FontWeight.Normal)
                .fontSize(20)
            }
            .onClick(() => {
              this.cur = 1;
              this.button1width = this.dewidth;
              this.button2width = this.expwidth;
            })
            .width(this.button2width)
            .height('95%')
            .margin({ right: 10 })
            .backgroundColor(this.cur === 1 ? '#ffe854' : '#eef2f1')
            .borderRadius(10)
            .animation({
              duration: 100,
              curve: Curve.Linear,
              iterations: 1,
            })
          }
          .margin({ top: 10 })
          .height('7%')

          Tabs({ index: this.cur }) {
            TabContent() {
              this.FavoriteList()
            }
            TabContent() {
              this.HistoryList()
            }
          }
          .height('40%')
          .barHeight(0)
          .animationDuration(1)
          .margin({ top: 10 })
          .scrollable(false)
        }
      }
      .tabBar(this.mytabbar('我的', 2, $r('app.media.me2'), $r('app.media.me1')))
    }
    .onChange((index: number) => {
      this.currentIndex = index
    })
    .barPosition(BarPosition.End)
    .barHeight('8%')
    .scrollable(false)
    .animationDuration(1)
  }
}
