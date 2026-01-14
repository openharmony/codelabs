import { router } from '@kit.ArkUI'
import { Book, initialBooks } from '../model/BookData'
import { Movie, initialMovies } from '../model/MovieData'
import { CommunityComp } from './CommunityComp'
import { MyPageComp } from './MyPage'

type MediaSection = 'book' | 'movie'

@Entry
@Component
struct Index {
  // 首页：图书/电影双分区推荐 + 底部 Tab（首页/广场/我的）
  @State private section: MediaSection = 'book'
  @State private recommendIndex: number = 0
  @State private recommendMovieIndex: number = 0
  @State private currentTab: number = 0
  @State private loginTip: string = ''
  // 用于触发子组件刷新用户态
  @State private userUpdateTrigger: number = 0

  private readonly featuredBooks: Book[] = initialBooks;
  private readonly featuredMovies: Movie[] = initialMovies;

  // 页面回到前台时通知子组件刷新
  onPageShow() {
    this.userUpdateTrigger++
  }

  private tabText(section: MediaSection): string {
    if (section === 'book') {
      return '图书'
    }
    return '电影'
  }

  private tabActive(section: MediaSection): boolean {
    return this.section === section
  }

  @Builder actionCard(title: string, subtitle: string, onClick: () => void, color1: number, color2: number) {
    Column({ space: 6 }) {
      Text(title)
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor(Color.White)
      Text(subtitle)
        .fontSize(12)
        .fontColor(Color.White)
    }
    .width('100%')
    .padding(14)
    .borderRadius(14)
    .linearGradient({
      angle: 135,
      colors: [[color1, 0.0], [color2, 1.0]]
    })
    .shadow({ radius: 8, color: `rgba(${color1 >> 16}, ${(color1 >> 8) & 0xFF}, ${color1 & 0xFF}, 0.3)`, offsetY: 4 })
    .onClick(onClick)
  }

  @Builder featuredCard(book: Book) {
    Column() {
      Image(book.coverRes)
        .width(116)
        .height(156)
        .borderRadius(12)
        .objectFit(ImageFit.Cover)
      Text(book.title)
        .fontSize(14)
        .fontWeight(FontWeight.Medium)
        .maxLines(1)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        .margin({ top: 10 })
      Row() {
        Text(book.rating.toFixed(1))
          .fontSize(12)
          .fontColor('#F59E0B')
        Text(`  ${book.ratingCount}人`)
          .fontSize(12)
          .fontColor('#8A8A8A')
      }
      .margin({ top: 4 })
    }
    .width(116)
    .onClick(() => {
      router.pushUrl({
        url: 'pages/MediaDetail',
        params: {
          book: book
        }
      })
    })
  }

  @Builder homeContent() {
    Column() {
      Stack() {
        Column() {
        }
        .width('100%')
        .height(178)
        .backgroundColor('#1A120D')

        Image($r('app.media.universe'))
          .width('100%')
          .height(178)
          .objectFit(ImageFit.Cover)
          .opacity(0.18)

        Column({ space: 10 }) {
          Text('书影鉴')
            .fontSize(26)
            .fontWeight(FontWeight.Bold)
            .fontColor('#F7F2EA')
            .width('100%')
            .textAlign(TextAlign.Center)

          Text('发现好内容，给出你的评分与评论')
            .fontSize(13)
            .fontColor('#D7CBBE')
            .width('100%')
            .textAlign(TextAlign.Center)
        }
        .padding({ left: 16, right: 16, top: 18 })
      }

      Row() {
        ForEach((['book', 'movie'] as MediaSection[]), (it: MediaSection) => {
          Column() {
            Text(this.tabText(it))
              .fontSize(16)
              .fontWeight(this.tabActive(it) ? FontWeight.Bold : FontWeight.Medium)
              .fontColor(this.tabActive(it) ? '#111' : '#7A7A7A')
            Blank()
              .height(3)
              .width(18)
              .borderRadius(2)
              .backgroundColor(this.tabActive(it) ? '#111' : '#00000000')
              .margin({ top: 8 })
          }
          .padding({ top: 10, bottom: 10 })
          .width('50%')
          .alignItems(HorizontalAlign.Center)
          .onClick(() => {
            this.section = it
          })
        })
      }
      .width('100%')
      .padding({ left: 16, right: 16 })
      .margin({ top: -18 })
      .borderRadius(16)
      .backgroundColor('#FFFFFF')

      Scroll() {
        Column({ space: 16 }) {
          if (this.section === 'book') {
            Row({ space: 12 }) {
              Column() {
                this.actionCard('找图书', '分类/检索', () => {
                  router.pushUrl({ url: 'pages/MediaSearch' })
                }, 0x2193b0, 0x6dd5ed)
              }
              .layoutWeight(1)

              Column() {
                this.actionCard('榜单', '热门与口碑', () => {
                  router.pushUrl({ url: 'pages/RankPage' })
                }, 0x11998e, 0x38ef7d)
              }
              .layoutWeight(1)
            }
            .width('100%')

            Column({ space: 10 }) {
              Row() {
                Text('为您推荐')
                  .fontSize(18)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111')
                Blank().layoutWeight(1)
              }

              Grid() {
                ForEach(this.featuredBooks.slice(this.recommendIndex, this.recommendIndex + 4), (b: Book) => {
                  GridItem() {
                    Column({ space: 8 }) {
                      Image(b.coverRes)
                        .width('100%')
                        .height(168)
                        .borderRadius(14)
                        .objectFit(ImageFit.Cover)

                      Text(b.title)
                        .fontSize(14)
                        .fontWeight(FontWeight.Medium)
                        .fontColor('#111')
                        .maxLines(1)
                        .textOverflow({ overflow: TextOverflow.Ellipsis })

                      Row({ space: 8 }) {
                        Text(b.rating.toFixed(1))
                          .fontSize(12)
                          .fontColor('#F59E0B')
                        Text(`${b.ratingCount}人`)
                          .fontSize(12)
                          .fontColor('#8A8A8A')
                      }
                    }
                    .onClick(() => {
                      router.pushUrl({
                        url: 'pages/MediaDetail',
                        params: { book: b }
                      })
                    })
                  }
                })
              }
              .columnsTemplate('1fr 1fr')
              .columnsGap(12)
              .rowsGap(12)

              Row() {
                Text('↻ 换一批')
                  .fontSize(13)
                  .fontColor('#111')
                  .fontWeight(FontWeight.Medium)
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
              .padding({ top: 12, bottom: 4 })
              .onClick(() => {
                this.recommendIndex = (this.recommendIndex + 4) % this.featuredBooks.length
              })
            }
            .padding(14)
            .borderRadius(16)
            .backgroundColor('#FFFFFF')
          }

          if (this.section === 'movie') {
            Row({ space: 12 }) {
              Column() {
                this.actionCard('找电影', '分类/检索', () => {
                  router.pushUrl({
                    url: 'pages/MediaSearch',
                    params: { type: 'movie' }
                  })
                }, 0x8E2DE2, 0x4A00E0)
              }
              .layoutWeight(1)

              Column() {
                this.actionCard('榜单', '热门与口碑', () => {
                  router.pushUrl({
                    url: 'pages/RankPage',
                    params: { type: 'movie' }
                  })
                }, 0xDA4453, 0x89216B)
              }
              .layoutWeight(1)
            }
            .width('100%')

            Column({ space: 10 }) {
              Row() {
                Text('为您推荐')
                  .fontSize(18)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111')
                Blank().layoutWeight(1)
              }

              Grid() {
                ForEach(this.featuredMovies.slice(this.recommendMovieIndex, this.recommendMovieIndex + 4), (m: Movie) => {
                  GridItem() {
                    Column({ space: 8 }) {
                      Image(m.coverRes)
                        .width('100%')
                        .height(168)
                        .borderRadius(14)
                        .objectFit(ImageFit.Cover)

                      Text(m.title)
                        .fontSize(14)
                        .fontWeight(FontWeight.Medium)
                        .fontColor('#111')
                        .maxLines(1)
                        .textOverflow({ overflow: TextOverflow.Ellipsis })

                      Row({ space: 8 }) {
                        Text(m.rating.toFixed(1))
                          .fontSize(12)
                          .fontColor('#F59E0B')
                        Text(`${m.ratingCount}人`)
                          .fontSize(12)
                          .fontColor('#8A8A8A')
                      }
                    }
                    .onClick(() => {
                      router.pushUrl({
                        url: 'pages/MediaDetail',
                        params: { book: m, type: 'movie' }
                      })
                    })
                  }
                })
              }
              .columnsTemplate('1fr 1fr')
              .columnsGap(12)
              .rowsGap(12)

              Row() {
                Text('↻ 换一批')
                  .fontSize(13)
                  .fontColor('#111')
                  .fontWeight(FontWeight.Medium)
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
              .padding({ top: 12, bottom: 4 })
              .onClick(() => {
                this.recommendMovieIndex = (this.recommendMovieIndex + 4) % this.featuredMovies.length
              })
            }
            .padding(14)
            .borderRadius(16)
            .backgroundColor('#FFFFFF')
          }

          Blank().height(20)
        }
        .padding({ left: 16, right: 16, top: 10 })
      }
      .scrollBar(BarState.Auto)
      .backgroundColor('#F3F4F6')
      .width('100%')
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
  }

  @Builder TabBuilder(title: string, index: number) {
    Column() {
      Text(title)
        .fontSize(12)
        .fontWeight(this.currentTab === index ? FontWeight.Bold : FontWeight.Medium)
        .fontColor(this.currentTab === index ? '#111' : '#7A7A7A')
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .backgroundColor('#FFFFFF')
  }

  build() {
    Column() {
      Tabs({ barPosition: BarPosition.End, index: this.currentTab }) {
        TabContent() {
          this.homeContent()
        }
        .tabBar(this.TabBuilder('首页', 0))

        TabContent() {
          CommunityComp({ 
            updateTrigger: this.userUpdateTrigger,
            onNeedLogin: (tip?: string) => {
              router.pushUrl({
                url: 'pages/MyPage',
                params: {
                  tip: tip ?? '请登录后发布动态',
                  backOnSuccess: true,
                  isPageMode: true
                }
              })
            }
          })
        }
        .tabBar(this.TabBuilder('广场', 1))

        TabContent() {
          MyPageComp({ tip: this.loginTip, updateTrigger: this.userUpdateTrigger })
        }
        .tabBar(this.TabBuilder('我的', 2))
      }
      .vertical(false)
      .scrollable(false)
      .barMode(BarMode.Fixed)
      .onChange((index: number) => {
        this.currentTab = index
        this.userUpdateTrigger++
        if (index === 2) {
        }
      })
      .width('100%')
      .height('100%')
      .backgroundColor('#FFFFFF')
    }
    .height('100%')
    .width('100%')
  }
}