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
import { Book, initialBooks } from '../model/BookData'
import { Movie, initialMovies } from '../model/MovieData'

@Entry
@Component
struct BookSearch {
  // 搜索页：支持图书/电影检索、分类筛选与排序展示
  @State private query: string = ''
  @State private selectedCategory: string = '全部'
  @State private wantIds: string[] = []
  @State private searchType: 'book' | 'movie' = 'book'

  @State private categories: string[] = ['全部', '计算机', '科学技术', '数学']
  private items: (Book | Movie)[] = initialBooks

  // 通过路由参数 type 切换书/电影数据源与分类
  aboutToAppear() {
    const params = router.getParams() as Record<string, Object> | undefined
    if (params && params['type'] === 'movie') {
      this.searchType = 'movie'
      this.categories = ['全部', '科幻', '动作', '喜剧']
      this.items = initialMovies
    } else {
      this.searchType = 'book'
      this.categories = ['全部', '计算机', '科学技术', '数学']
      this.items = initialBooks
    }
  }

  @Builder pill(text: string, active: boolean, onClick: () => void) {
    Text(text)
      .fontSize(13)
      .fontColor(active ? '#111' : '#666')
      .padding({ left: 12, right: 12, top: 8, bottom: 8 })
      .borderRadius(999)
      .backgroundColor(active ? '#EDEFF3' : '#FFFFFF')
      .border({ width: 0.5, color: active ? '#E0E3EA' : '#ECECEC' })
      .onClick(onClick)
  }

  @Builder categoryTab(text: string, active: boolean, onClick: () => void) {
    Column() {
      Text(text)
        .fontSize(15)
        .fontWeight(active ? FontWeight.Bold : FontWeight.Medium)
        .fontColor(active ? '#111' : '#7A7A7A')
      Blank()
        .height(3)
        .width(18)
        .borderRadius(2)
        .backgroundColor(active ? '#111' : '#00000000')
        .margin({ top: 8 })
    }
    .padding({ top: 8, bottom: 8 })
    .onClick(onClick)
  }

  private getBadgeColor(text: string): string {
    if (text.indexOf('殿堂') >= 0) {
      return '#C59226'
    }
    if (text.indexOf('经典') >= 0) {
      return '#E05448'
    }
    if (text.indexOf('佳作') >= 0) {
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

  // 过滤并排序：关键词 AND 匹配 + 分类筛选 + 按评分/人数降序
  private filteredItems(): (Book | Movie)[] {
    const q = this.query.trim().toLowerCase()
    const keywords = q.split(/\s+/).filter(k => k.length > 0)
    const cat = this.selectedCategory

    const base = this.items.filter((b: Book | Movie) => {
      // 关键词匹配逻辑：所有关键词都必须匹配（AND）
      let hitQuery = true
      if (keywords.length > 0) {
        let fullText = ''
        if (this.searchType === 'book') {
          const book = b as Book
          fullText = (book.title + ' ' + book.author + ' ' + book.publisher + ' ' + book.year).toLowerCase()
        } else {
          const movie = b as Movie
          const castNames = movie.cast.map(c => c.name).join(' ')
          fullText = (movie.title + ' ' + movie.director + ' ' + castNames + ' ' + movie.year).toLowerCase()
        }
        
        for (const k of keywords) {
          if (!fullText.includes(k)) {
            hitQuery = false
            break
          }
        }
      }

      const hitCat = cat === '全部' || b.tags.includes(cat)
      return hitQuery && hitCat
    })

    const sorted = base.slice()
    if (keywords.length > 0) {
      sorted.sort((a: Book | Movie, b: Book | Movie) => {
        if (b.rating !== a.rating) {
          return b.rating - a.rating
        }
        return b.ratingCount - a.ratingCount
      })
    }
    return sorted
  }

  // 本地“想读/想看”状态（仅用于当前页面展示）
  private isWanted(id: string): boolean {
    return this.wantIds.indexOf(id) >= 0
  }

  // 切换“想读/想看”状态
  private toggleWanted(id: string): void {
    const idx = this.wantIds.indexOf(id)
    if (idx >= 0) {
      const next: string[] = []
      for (let i = 0; i < this.wantIds.length; i++) {
        if (this.wantIds[i] !== id) {
          next.push(this.wantIds[i])
        }
      }
      this.wantIds = next
      return
    }
    this.wantIds = [...this.wantIds, id]
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

  @Builder bookRow(item: Book | Movie) {
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

          Text(this.searchType === 'book' ? 
               `${(item as Book).author} · ${(item as Book).year} · ${(item as Book).publisher}` : 
               `${(item as Movie).director} · ${(item as Movie).year} · ${(item as Movie).studio}`)
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
          params: { book: item, type: this.searchType }
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
        Text(this.searchType === 'movie' ? '找电影' : '找图书')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111')
        Blank().layoutWeight(1)
        Text('返回')
          .fontSize(14)
          .fontColor('#666')
          .onClick(() => router.back())
      }
      .padding({ left: 16, right: 16, top: 14, bottom: 10 })

      Column({ space: 10 }) {
        TextInput({ placeholder: this.searchType === 'movie' ? '电影名 / 导演 / 演员' : '书名 / 作者 / 出版社', text: this.query })
          .onChange((v: string) => {
            this.query = v
          })
          .onSubmit(() => {

          })
          .cancelButton({
            style: CancelButtonStyle.INPUT
          })
          .height(44)
          .padding({ left: 12, right: 12 })
          .borderRadius(12)
          .backgroundColor('#FFFFFF')

        List({ space: 18 }) {
          ForEach(this.categories, (c: string) => {
            ListItem() {
              Column() {
                Text(c)
                  .fontSize(15)
                  .fontWeight(this.selectedCategory === c ? FontWeight.Bold : FontWeight.Medium)
                  .fontColor(this.selectedCategory === c ? '#111' : '#7A7A7A')
                Blank()
                  .height(3)
                  .width(18)
                  .borderRadius(2)
                  .backgroundColor(this.selectedCategory === c ? '#111' : 'transparent')
                  .margin({ top: 8 })
              }
              .padding({ top: 8, bottom: 8 })
              .onClick(() => {
                this.selectedCategory = c
              })
            }
          }, (c: string) => c)
        }
        .listDirection(Axis.Horizontal)
        .scrollBar(BarState.Off)
        .height(40)
      }
      .padding({ left: 16, right: 16 })


      Divider().strokeWidth(0.5).color('#EAEAEA').margin({ top: 12 }).width('100%')

      List({ space: 12 }) {
        if (this.filteredItems().length <= 0) {
          ListItem() {
            Column({ space: 8 }) {
              Text(this.searchType === 'movie' ? '没有匹配的电影' : '没有匹配的图书')
                .fontSize(14)
                .fontColor('#666')
              Text('试试切换分类或清空搜索关键词')
                .fontSize(12)
                .fontColor('#8A8A8A')
            }
            .padding(16)
            .borderRadius(14)
            .backgroundColor('#FFFFFF')
          }
        }

        ForEach(this.filteredItems(), (b: Book | Movie) => {
          ListItem() {
            this.bookRow(b)
          }
        }, (b: Book | Movie) => b.id)

        ListItem() {
          Column() {
          }
          .height(20)
        }
      }
      .padding({ left: 16, right: 16, top: 12 })
      .scrollBar(BarState.Auto)
      .width('100%')
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F3F4F6')
  }
}