/*
 * Copyright (c) 2026 Beijing Institute of Technology
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
import Context from '@ohos.app.ability.common'
import { RdbUtil, UserInfo, FavItem } from '../utils/RdbUtil'
import { Book, initialBooks } from '../model/BookData'
import { Movie, initialMovies } from '../model/MovieData'

@Entry
@Component
struct MyFavorites {
  // 我的收藏：从数据库读取收藏列表，再映射到本地内置条目用于展示
  @State private items: (Book | Movie)[] = []
  @State private userName: string = ''

  // 进入页面：先读用户，再拉收藏
  async aboutToAppear() {
    await this.loadUser()
    if (this.userName.length > 0) {
      await this.loadFavorites()
    }
  }

  private async loadUser(): Promise<void> {
    const ctx = getContext(this) as Context
    const u: UserInfo | null = await RdbUtil.getCurrentUser(ctx)
    if (u) {
      this.userName = u.name
    }
  }

  // 拉取收藏 id 列表，并按类型匹配到内置数据源
  private async loadFavorites(): Promise<void> {
    const ctx = getContext(this) as Context
    const favs: FavItem[] = await RdbUtil.getUserFavorites(ctx, this.userName)
    const list: (Book | Movie)[] = []
    
    for (const f of favs) {
      if (f.type === 'book') {
        const b = initialBooks.find(it => it.id === f.mediaId)
        if (b) list.push(b)
      } else {
        const m = initialMovies.find(it => it.id === f.mediaId)
        if (m) list.push(m)
      }
    }
    this.items = list
  }

  private getBadgeColor(text: string): string {
    if (text.indexOf('殿堂') >= 0) {
      return '#C59226'
    }
    if (text.indexOf('硬核') >= 0 || text.indexOf('经典') >= 0) {
      return '#E05448'
    }
    if (text.indexOf('入门') >= 0 || text.indexOf('佳作') >= 0) {
      return '#4C8FA9'
    }
    return '#888888'
  }

  @Builder badgePill(text: string) {
    Text(text)
      .fontSize(11)
      .fontWeight(FontWeight.Bold)
      .fontColor(this.getBadgeColor(text))
      .padding({ top: 4, bottom: 4 })
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

  // 通过字段判断条目类型
  private isMovie(item: Book | Movie): boolean {
    return (item as Movie).director !== undefined
  }

  @Builder itemRow(item: Book | Movie) {
    Row({ space: 12 }) {
      Row({ space: 12 }) {
        Image(item.coverRes)
          .width(78)
          .height(104)
          .borderRadius(12)
          .objectFit(ImageFit.Cover)

        Column({ space: 6 }) {
          Text(item.title)
            .fontSize(17)
            .fontWeight(FontWeight.Medium)
            .fontColor('#111')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Row({ space: 8 }) {
            Text(this.starsLine(this.ratingStars(item.rating)))
              .fontSize(12)
              .fontColor('#F59E0B')
            Text(item.rating.toFixed(1))
              .fontSize(12)
              .fontColor('#F59E0B')
          }

          Text(this.isMovie(item) ? 
               `${(item as Movie).director} · ${(item as Movie).year} · ${(item as Movie).studio}` : 
               `${(item as Book).author} · ${(item as Book).year} · ${(item as Book).publisher}`)
            .fontSize(12)
            .fontColor('#777')
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          if (item.badgeText && item.badgeText.length > 0) {
            this.badgePill(item.badgeText)
          }
        }
        .layoutWeight(1)
      }
      .layoutWeight(1)
      .onClick(() => {
        router.pushUrl({
          url: 'pages/MediaDetail',
          params: { book: item, type: this.isMovie(item) ? 'movie' : 'book' }
        })
      })
    }
    .padding(12)
    .borderRadius(14)
    .backgroundColor('#FFFFFF')
  }

  build() {
    Column() {
      Row() {
        Text('返回')
          .fontSize(14)
          .fontColor('#666')
          .onClick(() => router.back())
        Blank().layoutWeight(1)
        Text('我的收藏')
          .fontSize(17)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111')
        Blank().layoutWeight(1)
        Text(' ')
          .fontSize(14)
      }
      .padding({ left: 16, right: 16, top: 14, bottom: 10 })

      if (this.items.length <= 0) {
        Column({ space: 16 }) {
          Image($r('app.media.cosmos'))
            .width(120)
            .height(120)
            .opacity(0.5)
            .borderRadius(60)
          Text('暂无收藏内容')
            .fontSize(14)
            .fontColor('#999')
        }
        .width('100%')
        .height('60%')
        .justifyContent(FlexAlign.Center)
      } else {
        List({ space: 12 }) {
          ForEach(this.items, (item: Book | Movie) => {
            ListItem() {
              this.itemRow(item)
            }
          })
          ListItem() {
            Column().height(20)
          }
        }
        .padding({ left: 16, right: 16, top: 12 })
        .scrollBar(BarState.Auto)
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F3F4F6')
  }
}