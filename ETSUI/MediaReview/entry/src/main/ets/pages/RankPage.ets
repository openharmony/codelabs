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
struct RankPage {
  // 榜单页：按类型（书/电影）与榜单标签筛选并排序展示
  @State currentRank: string = '口碑榜Top10'
  @State rankType: 'book' | 'movie' = 'book'
  @State ranks: string[] = []
  // 读取路由参数，决定展示图书榜或电影榜
  aboutToAppear() {
    const params = router.getParams() as Record<string, Object> | undefined
    if (params && params['type'] === 'movie') {
      this.rankType = 'movie'
      this.ranks = ['口碑榜Top10', '科幻Top5', '动作Top5', '喜剧Top5']
    } else {
      this.rankType = 'book'
      this.ranks = ['口碑榜Top10', '计算机Top5', '数学Top5', '科学技术Top5']
    }
  }

  // 根据当前榜单名称生成列表（口碑榜 Top10 / 标签 Top5）
  private getRankData(): (Book | Movie)[] {
    let list: (Book | Movie)[] = this.rankType === 'movie' ? [...initialMovies] : [...initialBooks]
    
    const sortFn = (a: Book | Movie, b: Book | Movie) => {
      if (b.rating !== a.rating) {
        return b.rating - a.rating
      }
      return b.ratingCount - a.ratingCount
    }

    if (this.currentRank === '口碑榜Top10') {
      list.sort(sortFn)
      return list.slice(0, 10)
    } 
    
    const filterTag = (tag: string) => {
      list = list.filter(b => b.tags.includes(tag))
      list.sort(sortFn)
      return list.slice(0, 5)
    }

    if (this.currentRank.endsWith('Top5')) {
      const tag = this.currentRank.replace('Top5', '')
      return filterTag(tag)
    }

    return list.slice(0, 10)
  }

  @Builder bookRow(item: Book | Movie, index: number) {
    Row({ space: 12 }) {
      Text(`${index + 1}`)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor(index < 3 ? '#D14343' : '#999')
        .width(24)
        .textAlign(TextAlign.Center)

      Image(item.coverRes)
        .width(60)
        .height(80)
        .borderRadius(6)
        .objectFit(ImageFit.Cover)

      Column({ space: 4 }) {
        Text(item.title)
          .fontSize(15)
          .fontWeight(FontWeight.Medium)
          .fontColor('#111')
          .maxLines(1)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
        
        Row({ space: 8 }) {
           Text(item.rating.toFixed(1))
             .fontSize(12)
             .fontColor('#F59E0B')
             .fontWeight(FontWeight.Bold)
           Text(`${item.ratingCount}人评价`)
             .fontSize(11)
             .fontColor('#999')
        }
        
        Text(this.rankType === 'book' ? (item as Book).author : (item as Movie).director)
          .fontSize(11)
          .fontColor('#666')
          .maxLines(1)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
      }
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Start)
    }
    .padding(10)
    .backgroundColor('#FFF')
    .borderRadius(12)
    .onClick(() => {
        router.pushUrl({
          url: 'pages/MediaDetail',
          params: { book: item, type: this.rankType }
        })
    })
  }

  @Builder rankTab(name: string) {
    if (this.currentRank === name) {
      Text(name)
        .fontSize(14)
        .fontWeight(FontWeight.Bold)
        .fontColor(Color.White)
        .linearGradient({
          angle: 135,
          colors: [[0xEE7752, 0.0], [0xE73C7E, 1.0]]
        })
        .padding({ top: 12, bottom: 12 })
        .borderRadius(12)
        .width('100%')
        .textAlign(TextAlign.Center)
        .shadow({ radius: 8, color: '#44E73C7E', offsetY: 4 })
        .onClick(() => {
          this.currentRank = name
        })
    } else {
      Text(name)
        .fontSize(13)
        .fontWeight(FontWeight.Medium)
        .fontColor('#666')
        .backgroundColor('transparent')
        .padding({ top: 12, bottom: 12 })
        .borderRadius(8)
        .width('100%')
        .textAlign(TextAlign.Center)
        .onClick(() => {
          this.currentRank = name
        })
    }
  }

  build() {
    Column() {
      Row() {
        Text('返回')
          .fontSize(14)
          .fontColor('#666')
          .onClick(() => router.back())
        Blank().layoutWeight(1)
        Text(this.rankType === 'movie' ? '电影排行榜' : '图书排行榜')
          .fontSize(17)
          .fontWeight(FontWeight.Bold)
        Blank().layoutWeight(1)
        Blank().width(30)
      }
      .padding({ left: 16, right: 16, top: 12, bottom: 12 })
      .width('100%')
      .backgroundColor('#FFF')

      Row() {
        List({ space: 10 }) {
          ForEach(this.getRankData(), (item: Book | Movie, index: number) => {
            ListItem() {
              this.bookRow(item, index)
            }
          })
          ListItem() {
            Column().height(20)
          }
        }
        .layoutWeight(1)
        .padding({ left: 12, right: 12, top: 12 })
        .scrollBar(BarState.Off)

        Column({ space: 2 }) {
          ForEach(this.ranks, (rank: string) => {
            this.rankTab(rank)
          })
        }
        .width(80)
        .padding({ left: 4, right: 4, top: 12 })
        .backgroundColor('#F3F4F6')
        .height('100%')
      }
      .layoutWeight(1)
      .alignItems(VerticalAlign.Top)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F3F4F6')
  }
}
