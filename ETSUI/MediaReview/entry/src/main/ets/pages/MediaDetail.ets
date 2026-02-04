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

import { router } from '@kit.ArkUI'
import promptAction from '@ohos.promptAction'
import Context from '@ohos.app.ability.common'
import { RdbUtil, DbReview, UserInfo } from '../utils/RdbUtil'
import { Book, initialBooks } from '../model/BookData'
import { Movie, CastMember, initialMovies } from '../model/MovieData'
import { RatingDialog } from './RatingDialog'

type MediaItem = Book | Movie

class Review {
  id: string
  userName: string
  avatarKey: string
  rating: number
  content: string
  createdAt: string
  likeCount: number

  constructor(id: string, userName: string, avatarKey: string, rating: number, content: string, createdAt: string, likeCount: number) {
    this.id = id
    this.userName = userName
    this.avatarKey = avatarKey
    this.rating = rating
    this.content = content
    this.createdAt = createdAt
    this.likeCount = likeCount
  }
}


@Entry
@Component
struct BookDetail {
  // 详情页：展示图书/电影信息，支持收藏与评分，统计从本地基准与数据库合并
  @State private hasBook: boolean = false
  @State private type: 'book' | 'movie' = 'book'
  @State private book: MediaItem = {
    id: '',
    title: '',
    author: '',
    year: '',
    publisher: '',
    tags: [],
    rating: 0,
    ratingCount: 0,
    summary: '',
    coverRes: $r('app.media.startIcon')
  } as Book
  
  @State private status: string = '想读'
  @State private userRating: number = 0
  @State private userReviewContent: string = ''
  @State private reviews: Review[] = []
  
  // 用独立状态驱动评分区刷新
  @State private bookRating: number = 0
  @State private bookRatingCount: number = 0
  @State private starCounts: number[] = [0, 0, 0, 0, 0]

  @State private userName: string = ''
  @State private userAvatarKey: string = 'user'
  @State private isFavorited: boolean = false
  private dialogController: CustomDialogController | null = null

  private readonly bookReviews: Review[] = [
    new Review('r1', '秋本丽子', 'qblz', 5, '结构清晰，案例很扎实。', '2026-01-01', 0),
    new Review('r2', '拟宝珠缠', 'nbzc', 4, '干货多，但需要边练边看。', '2026-01-03', 0),
    new Review('r3', '居间惠', 'jjh', 5, '适合反复翻，越看越有收获。', '2026-01-06', 0),
    new Review('r4', '中村由利', 'zcyl', 4, '内容密度高，建议边做笔记边读。', '2026-01-08', 0)
  ]

  private readonly movieReviews: Review[] = [
    new Review('r1', '秋本丽子', 'qblz', 5, '镜头语言太美了。', '2026-01-01', 0),
    new Review('r2', '拟宝珠缠', 'nbzc', 4, '剧情很精彩。', '2026-01-03', 0),
    new Review('r3', '居间惠', 'jjh', 5, '演技炸裂！', '2026-01-06', 0),
    new Review('r4', '中村由利', 'zcyl', 4, '配乐满分，完全沉浸在氛围里。', '2026-01-08', 0)
  ]

  // 读取路由参数并初始化页面数据
  async aboutToAppear(): Promise<void> {
    const params = router.getParams() as Record<string, Object> | undefined
    if (params && params['book']) {
      this.book = params['book'] as MediaItem
      this.type = (params['type'] as 'book' | 'movie') || 'book'
      this.hasBook = true
      this.status = this.type === 'movie' ? '想看' : '想读'
      
      this.bookRating = this.book.rating
      this.bookRatingCount = this.book.ratingCount

      await this.loadUser()
      if (this.userName.length > 0) {
        await this.checkFavorite()
      }
      await this.loadReviews()
      return
    }
    this.hasBook = false
  }

  onPageShow(): void {
    if (this.hasBook) {
      void this.loadUser().then(() => {
        if (this.userName.length > 0) {
          void this.checkFavorite()
        }
      })
    }
  }

  @Builder statusButton(text: string) {
    Text(text)
      .fontSize(13)
      .fontColor(this.status === text ? '#111' : '#666')
      .padding({ left: 14, right: 14, top: 9, bottom: 9 })
      .borderRadius(999)
      .backgroundColor(this.status === text ? '#EDEFF3' : '#FFFFFF')
      .border({ width: 0.5, color: this.status === text ? '#E0E3EA' : '#ECECEC' })
      .onClick(() => {
        this.status = text
      })
  }

  @Builder ratingStar(index: number) {
    Text(this.userRating >= index ? '★' : '☆')
      .fontSize(22)
      .fontColor(this.userRating >= index ? '#F59E0B' : '#C7C7C7')
  }

  // 读取当前登录用户
  private async loadUser(): Promise<void> {
    const ctx = getContext(this) as Context
    const u: UserInfo | null = await RdbUtil.getCurrentUser(ctx)
    if (u) {
      this.userName = u.name
      this.userAvatarKey = u.avatarKey
    }
  }

  // 查询当前条目是否已收藏
  private async checkFavorite(): Promise<void> {
    const ctx = getContext(this) as Context
    this.isFavorited = await RdbUtil.isFavorite(ctx, this.userName, this.book.id)
  }

  // 切换收藏状态，未登录时跳转登录
  private async toggleFavorite(): Promise<void> {
    if (this.userName.length <= 0) {
      this.goLogin('请登录后收藏')
      return
    }
    const ctx = getContext(this) as Context
    if (this.isFavorited) {
      const ok = await RdbUtil.removeFavorite(ctx, this.userName, this.book.id)
      if (ok) {
        this.isFavorited = false
        promptAction.showToast({ message: '已取消收藏' })
      }
    } else {
      const ok = await RdbUtil.addFavorite(ctx, this.userName, this.book.id, this.type)
      if (ok) {
        this.isFavorited = true
        promptAction.showToast({ message: '收藏成功' })
      }
    }
  }

  private goLogin(tip: string): void {
    promptAction.showToast({ message: tip })
    router.pushUrl({ url: 'pages/MyPage', params: { backOnSuccess: true } })
  }

  // 读取评分/评价：数据库优先，空时回退到内置示例
  private async loadReviews(): Promise<void> {
    const ctx = getContext(this) as Context
    const fromDb = await RdbUtil.queryReviews(ctx, this.book.id)
    const fallback = this.type === 'movie' ? this.movieReviews : this.bookReviews

    let allReviews: Review[] = []
    if (fromDb && fromDb.length > 0) {
      const stored: Review[] = fromDb.map((it: DbReview) => {
        return new Review(it.id, it.userName, it.avatarKey, it.rating, it.content, it.createdAt, it.likeCount)
      })
      allReviews = [...stored, ...fallback]
    } else {
      allReviews = [...fallback]
    }

    this.reviews = allReviews.filter(r => r.content.trim().length > 0)

    if (this.userName.length > 0) {
      const ratingEntry = allReviews.find(r => r.userName === this.userName && r.id.startsWith(`rate_${this.book.id}_`))
      if (ratingEntry) {
        this.userRating = ratingEntry.rating
      } else {
        const myReviews = allReviews.filter(r => r.userName === this.userName && r.rating > 0)
        if (myReviews.length > 0) {
          this.userRating = myReviews[0].rating
        } else {
          this.userRating = 0
        }
      }
    }

    await this.refreshStats()
  }

  // 刷新评分统计：把内置基准数据与数据库增量合并
  private async refreshStats() {
    const ctx = getContext(this) as Context
    const stats = await RdbUtil.getRatingStats(ctx, this.book.id)
    
    let memItem: Book | Movie | undefined
    if (this.type === 'book') {
      memItem = initialBooks.find(b => b.id === this.book.id)
    } else {
      memItem = initialMovies.find(m => m.id === this.book.id)
    }

    if (memItem) {
      if (memItem.baseRating === undefined) {
        memItem.baseRating = memItem.rating
        memItem.baseRatingCount = memItem.ratingCount
      }
      const baseRating = memItem.baseRating ?? 0
      const baseCount = memItem.baseRatingCount ?? 0

      const totalCount = baseCount + stats.count
      const totalSum10 = (baseRating * baseCount) + (stats.sum * 2)
      const newRating = totalCount > 0 ? (totalSum10 / totalCount) : 0

      memItem.rating = newRating
      memItem.ratingCount = totalCount
      
      this.book.rating = newRating
      this.book.ratingCount = totalCount
      this.bookRating = newRating
      this.bookRatingCount = totalCount

      const dbStar = await RdbUtil.getStarStats(ctx, this.book.id)
      const baseCounts = RdbUtil.computeBaseDistribution(baseRating, baseCount)
      const merged = baseCounts.map((v, i) => v + (dbStar.counts[i] ?? 0))
      this.starCounts = merged
    } else {
      if (stats.count > 0) {
        const avg10 = stats.rating * 2
        this.book.rating = avg10
        this.book.ratingCount = stats.count
        this.bookRating = avg10
        this.bookRatingCount = stats.count
      }
      const dbStar = await RdbUtil.getStarStats(ctx, this.book.id)
      this.starCounts = [...dbStar.counts]
    }
  }

  // 打开评分弹窗
  private openRatingDialog() {
    if (this.userName.length <= 0) {
      this.goLogin('请登录后评分')
      return
    }
    this.dialogController = new CustomDialogController({
      builder: RatingDialog({
        rating: this.userRating,
        content: this.userReviewContent,
        isExist: this.userRating > 0,
        action: (rating: number, content: string): void => { this.saveReview(rating, content) },
        deleteAction: () => { this.deleteRating() }
      }),
      alignment: DialogAlignment.Center,
      autoCancel: true,
      customStyle: true
    })
    this.dialogController.open()
  }

  private async deleteRating() {
    if (this.userRating <= 0) return
    const ctx = getContext(this) as Context
    const myReview = this.reviews.find(r => r.userName === this.userName)
    if (myReview) {
      await RdbUtil.deleteReview(ctx, this.book.id, myReview.id)
      this.reviews = this.reviews.filter(r => r.id !== myReview.id)
    }
    
    this.userRating = 0
    this.userReviewContent = ''
    await this.refreshStats()
    promptAction.showToast({ message: '评价已删除' })
  }

  // 保存评分/评价：同一用户对同一条目复用同一个记录 id
  private async saveReview(rating: number, content: string) {
    if (this.userName.length <= 0) return
    if (rating <= 0) { promptAction.showToast({ message: '请先选择星级' }); return }
    
    const ctx = getContext(this) as Context
    const now = new Date()
    const yyyy = `${now.getFullYear()}`
    const mm = `${now.getMonth() + 1}`.padStart(2, '0')
    const dd = `${now.getDate()}`.padStart(2, '0')
    const dateStr = `${yyyy}-${mm}-${dd}`
    
    let reviewId = `review_${this.book.id}_${this.userName}`
    const existing = this.reviews.find(r => r.userName === this.userName)
    if (existing) {
      reviewId = existing.id
    }

    const review = new Review(
      reviewId,
      this.userName,
      this.userAvatarKey,
      rating,
      content,
      dateStr,
      0
    )

    await RdbUtil.upsertReview(
      ctx,
      this.book.id,
      new DbReview(review.id, review.userName, review.avatarKey, review.rating, review.content, review.createdAt, review.likeCount)
    )

    this.userRating = rating
    this.userReviewContent = content
    await this.loadReviews()
    promptAction.showToast({ message: '评价发布成功' })
  }

  private avatarRes(key: string): Resource {
    if (key === 'qblz') {
      return $r('app.media.qblz')
    }
    if (key === 'nbzc') {
      return $r('app.media.nbzc')
    }
    if (key === 'jjh') {
      return $r('app.media.jjh')
    }
    if (key === 'zcyl') {
      return $r('app.media.zcyl')
    }
    return $r('app.media.user')
  }

  @Builder avatar(key: string) {
    Image(this.avatarRes(key))
      .width(28)
      .height(28)
      .borderRadius(8)
      .objectFit(ImageFit.Cover)
  }

  @Builder reviewRow(r: Review) {
    Column({ space: 8 }) {
      Row({ space: 10 }) {
        this.avatar(r.avatarKey)
        Text(r.userName)
          .fontSize(14)
          .fontWeight(FontWeight.Medium)
          .fontColor('#111')
        Blank().layoutWeight(1)
        Text(r.createdAt)
          .fontSize(12)
          .fontColor('#8A8A8A')
      }
      Row({ space: 8 }) {
        if (r.rating > 0) {
          Text('★'.repeat(r.rating) + '☆'.repeat(5 - r.rating))
            .fontSize(12)
            .fontColor('#F59E0B')
          Text('·')
            .fontSize(12)
            .fontColor('#C0C0C0')
        }
      }
      Text(r.content)
        .fontSize(13)
        .fontColor('#333')
        .maxLines(5)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    .padding(12)
    .borderRadius(14)
    .backgroundColor('#FFFFFF')
  }

  private ratingStars(rating10: number): number {
    let v: number = Math.round(rating10) / 2
    if (v < 0) {
      v = 0
    }
    if (v > 5) {
      v = 5
    }
    return v
  }

  private starsLine(stars: number): string {
    const full = Math.floor(stars)
    const half = stars % 1 >= 0.5 ? 1 : 0
    return '★★★★★'.slice(0, full) + (half === 1 ? '⯪' : '') + '☆☆☆☆☆'.slice(0, 5 - full - half)
  }

  private distPercent(stars: number): number {
    const total = this.starCounts.reduce((a, b) => a + b, 0)
    if (total <= 0) return 0
    const idx = Math.max(1, Math.min(5, stars)) - 1
    return Math.round(100 * (this.starCounts[idx] || 0) / total)
  }

  private barWidth(stars: number): number {
    return Math.round(132 * this.distPercent(stars) / 100)
  }

  build() {
    Column() {
      Row() {
        Text('返回')
          .fontSize(14)
          .fontColor('#666')
          .onClick(() => router.back())
        Blank().layoutWeight(1)
        Text(this.type === 'movie' ? '电影详情' : '图书详情')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#111')
        Blank().layoutWeight(1)
        Text(' ')
          .fontSize(14)
      }
      .padding({ left: 16, right: 16, top: 14, bottom: 10 })

      Scroll() {
        if (!this.hasBook) {
          Column({ space: 10 }) {
            Text(this.type === 'movie' ? '未获取到电影参数' : '未获取到图书参数')
              .fontSize(14)
              .fontColor('#666')
            Button('返回')
              .type(ButtonType.Capsule)
              .backgroundColor('#111')
              .fontColor('#FFF')
              .onClick(() => router.back())
          }
          .padding(16)
          .borderRadius(16)
          .backgroundColor('#FFFFFF')
          .margin({ left: 16, right: 16, top: 16 })
        } else {
          Column({ space: 14 }) {
            Stack() {
              Column() {
              }
                .width('100%')
                .height(210)
                .borderRadius(18)
                .backgroundColor('#FFFFFF')

              Image($r('app.media.universe'))
                .width('100%')
                .height(210)
                .borderRadius(18)
                .objectFit(ImageFit.Cover)
                .opacity(0)

              Column() {
              }
                .width('100%')
                .height(210)
                .borderRadius(18)
                .backgroundColor('#FFFFFF')

              Row({ space: 14 }) {
                Image(this.book.coverRes)
                  .width(104)
                  .height(140)
                  .borderRadius(14)
                  .objectFit(ImageFit.Cover)

                Column({ space: 8 }) {
                  Text(this.book.title)
                    .fontSize(20)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#111')
                    .maxLines(2)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })

                  Text(this.type === 'movie' ? 
                       `${(this.book as Movie).region} · ${(this.book as Movie).year} · ${(this.book as Movie).studio}` :
                       `${(this.book as Book).author} · ${(this.book as Book).year} · ${(this.book as Book).publisher}`)
                    .fontSize(12)
                    .fontColor('#5F5F5F')
                    .maxLines(2)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })

                  Row() {
                    Text(this.isFavorited ? '♥ 已收藏' : '♡ 收藏')
                      .fontSize(12)
                      .fontWeight(this.isFavorited ? FontWeight.Bold : FontWeight.Normal)
                      .fontColor(this.isFavorited ? '#D14343' : '#333')
                      .padding({ left: 14, right: 14, top: 7, bottom: 7 })
                      .backgroundColor(this.isFavorited ? '#FDECEC' : '#F5F5F5')
                      .borderRadius(16)
                      .onClick(() => this.toggleFavorite())
                  }
                  .margin({ top: 6, bottom: 6 })

                  Row({ space: 8 }) {
                    Text(this.bookRating.toFixed(1))
                      .fontSize(14)
                      .fontColor('#D47D00')
                      .fontWeight(FontWeight.Bold)
                    Text(this.starsLine(this.ratingStars(this.bookRating)))
                      .fontSize(12)
                      .fontColor('#D47D00')
                  }

                }
                .layoutWeight(1)
              }
              .padding(14)
            }
            .borderRadius(18)
            .backgroundColor('#FFFFFF')
            .border({ width: 0.5, color: '#EFE7DC' })

            Column({ space: 10 }) {
              Text('简介')
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111')
              Text(this.book.summary)
                .fontSize(13)
                .fontColor('#555')
                .lineHeight(20)
            }
            .padding(14)
            .borderRadius(18)
            .backgroundColor('#FFFFFF')
            .border({ width: 0.5, color: '#EFE7DC' })

            if (this.type === 'movie') {
              Column({ space: 10 }) {
                Text('演职员表（部分）')
                  .fontSize(16)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111')

                List({ space: 12 }) {
                  ForEach((this.book as Movie).cast, (c: CastMember) => {
                    ListItem() {
                      Column({ space: 6 }) {
                        Image(c.avatar)
                          .width(60)
                          .height(60)
                          .borderRadius(30)
                          .objectFit(ImageFit.Cover)
                        Text(c.name)
                          .fontSize(12)
                          .fontColor('#111')
                          .maxLines(1)
                          .textOverflow({ overflow: TextOverflow.Ellipsis })
                        Text(c.role)
                          .fontSize(10)
                          .fontColor('#888')
                          .maxLines(1)
                          .textOverflow({ overflow: TextOverflow.Ellipsis })
                      }
                      .width(80)
                    }
                  })
                }
                .listDirection(Axis.Horizontal)
                .scrollBar(BarState.Off)
                .width('100%')
                .height(110)
              }
              .padding(14)
              .borderRadius(18)
              .backgroundColor('#FFFFFF')
              .border({ width: 0.5, color: '#EFE7DC' })
              .alignItems(HorizontalAlign.Start)
            }

            Row({ space: 14 }) {
              Column({ space: 6 }) {
                Text(this.type === 'movie' ? '书影鉴评分' : '书影鉴评分')
                  .fontSize(12)
                  .fontColor('#7A6F66')
                Text(this.bookRating.toFixed(1))
                  .fontSize(46)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111')
                Text(this.starsLine(this.ratingStars(this.bookRating)))
                  .fontSize(14)
                  .fontColor('#D47D00')
                Text(`${this.bookRatingCount}人评分`)
                  .fontSize(12)
                  .fontColor('#7A6F66')
              }

              Column({ space: 6 }) {
                ForEach([5, 4, 3, 2, 1], (s: number) => {
                  Row({ space: 10 }) {
                    Text(`${s}★`)
                      .fontSize(11)
                      .fontColor('#7A6F66')
                      .width(26)

                    Stack() {
                      Column() {
                      }
                        .width(132)
                        .height(6)
                        .borderRadius(3)
                        .backgroundColor('#EFE7DC')
                      Column() {
                      }
                        .width(this.barWidth(s))
                        .height(6)
                        .borderRadius(3)
                        .backgroundColor('#D47D00')
                    }

                    Text(`${this.distPercent(s)}%`)
                      .fontSize(11)
                      .fontColor('#7A6F66')
                      .width(34)
                  }
                })
              }
              .layoutWeight(1)
            }
            .padding(14)
            .borderRadius(18)
            .backgroundColor('#FFFFFF')
            .border({ width: 0.5, color: '#EFE7DC' })

            Column({ space: 10 }) {
              Text('我的评分')
                .fontSize(16)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111')

              if (this.userName.length > 0) {
                Row({ space: 10 }) {
                  this.avatar(this.userAvatarKey)
                  Text(this.userName)
                    .fontSize(14)
                    .fontWeight(FontWeight.Medium)
                    .fontColor('#111')
                }
              } else {
                Text('未登录，点击评分将跳转登录')
                  .fontSize(12)
                  .fontColor('#8A8A8A')
              }

              Row() {
                if (this.userRating > 0) {
                  Column({ space: 4 }) {
                     Text('已评价')
                       .fontSize(14)
                       .fontColor('#666')
                     Row({ space: 2 }) {
                       Text('★'.repeat(this.userRating) + '☆'.repeat(5 - this.userRating))
                         .fontSize(14)
                         .fontColor('#F59E0B')
                     }
                  }
                  .alignItems(HorizontalAlign.Start)
                  
                  Blank().layoutWeight(1)

                  Button('修改评分')
                    .type(ButtonType.Normal)
                    .borderRadius(8)
                    .backgroundColor('#F5F5F5')
                    .fontColor('#666')
                    .fontSize(12)
                    .height(28)
                    .onClick(() => {
                      this.openRatingDialog()
                    })
                } else {
                  Button('鉴赏评分')
                    .type(ButtonType.Capsule)
                    .backgroundColor('#FFF')
                    .fontColor('#D47D00')
                    .border({ width: 1, color: '#D47D00' })
                    .width('100%')
                    .height(40)
                    .onClick(() => {
                       this.openRatingDialog()
                    })
                }
              }
              .width('100%')
              .margin({ top: 8 })
            }
            .alignItems(HorizontalAlign.Start)
            .padding(14)
            .borderRadius(18)
            .backgroundColor('#FFFFFF')
            .border({ width: 0.5, color: '#EFE7DC' })

            Column({ space: 10 }) {
              Row({ space: 6 }) {
                Text(this.type === 'movie' ? '影评' : '书评')
                  .fontSize(16)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111')
                Text(`(${this.reviews.length})`)
                  .fontSize(14)
                  .fontColor('#7A6F66')
                Blank().layoutWeight(1)
              }

              ForEach(this.reviews, (r: Review) => {
                this.reviewRow(r)
              })
            }
            .padding(14)
            .borderRadius(18)
            .backgroundColor('#FFFFFF')
            .border({ width: 0.5, color: '#EFE7DC' })

            Blank().height(24)
          }
          .padding({ left: 16, right: 16, top: 12 })
        }
      }
      .scrollBar(BarState.Auto)
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F6F4F1')
  }
}
