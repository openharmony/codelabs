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

export interface CastMember {
  name: string
  role: string
  avatar: Resource
}

export interface Movie {
  id: string
  title: string
  director: string
  year: string
  studio: string
  region: string
  cast: CastMember[]
  tags: string[]
  rating: number
  ratingCount: number
  summary: string
  coverRes: Resource
  badgeText?: string
  baseRating?: number
  baseRatingCount?: number
}

export const initialMovies: Movie[] = [
  {
    id: 'movie_way_of_the_dragon',
    title: '猛龙过江',
    director: '李小龙',
    year: '1972',
    studio: '嘉禾电影',
    region: '中国香港',
    cast: [
      { name: '李小龙', role: '导演 / 饰 唐龙', avatar: $r('app.media.lxl') },
      { name: '苗可秀', role: '饰 陈清华', avatar: $r('app.media.mkx') }
    ],
    tags: ['动作'],
    rating: 8.3,
    ratingCount: 100,
    summary: '李小龙自编自导自演，罗马斗兽场决战成为影史经典。',
    coverRes: $r('app.media.mlgj'),
    badgeText: '动作经典'
  },
  {
    id: 'movie_drunken_master_2',
    title: '醉拳2',
    director: '刘家良',
    year: '1994',
    studio: '嘉禾电影',
    region: '中国香港',
    cast: [
      { name: '刘家良', role: '导演', avatar: $r('app.media.ljl') },
      { name: '成龙', role: '饰 黄飞鸿', avatar: $r('app.media.cl') },
      { name: '梅艳芳', role: '饰 黄麒英妻', avatar: $r('app.media.myf') }
    ],
    tags: ['动作'],
    rating: 8.1,
    ratingCount: 100,
    summary: '成龙功夫喜剧巅峰之作，动作设计行云流水，诙谐幽默中见真功夫。',
    coverRes: $r('app.media.zq2'),
    badgeText: '动作经典'
  },
  {
    id: 'movie_king_of_comedy',
    title: '喜剧之王',
    director: '周星驰 / 李力持',
    year: '1999',
    studio: '星辉海外',
    region: '中国香港',
    cast: [
      { name: '周星驰', role: '导演 / 饰 尹天仇', avatar: $r('app.media.zxc') },
      { name: '张柏芝', role: '饰 柳飘飘', avatar: $r('app.media.zbz') },
      { name: '莫文蔚', role: '饰 杜鹃儿', avatar: $r('app.media.mwm') }
    ],
    tags: ['喜剧'],
    rating: 8.9,
    ratingCount: 100,
    summary: '其实我是一个演员。周星驰半自传式作品，笑中带泪的草根奋斗史。',
    coverRes: $r('app.media.xjzw'),
    badgeText: '喜剧经典'
  },
  {
    id: 'movie_from_beijing_with_love',
    title: '国产凌凌漆',
    director: '李力持 / 周星驰',
    year: '1994',
    studio: '永盛电影',
    region: '中国香港',
    cast: [
      { name: '李力持', role: '导演 / 饰 凌凌漆', avatar: $r('app.media.llc') },
      { name: '周星驰', role: '导演 / 饰 凌凌漆', avatar: $r('app.media.zxc') },
      { name: '袁咏仪', role: '饰 李香琴', avatar: $r('app.media.yyl') },
      { name: '陈宝莲', role: '饰 爱美神', avatar: $r('app.media.cbl') }
    ],
    tags: ['喜剧'],
    rating: 8.4,
    ratingCount: 100,
    summary: '后现代解构主义喜剧，对经典特工片的无厘头致敬与颠覆。',
    coverRes: $r('app.media.gcllq'),
    badgeText: '喜剧经典'
  },
  {
    id: 'movie_king_of_beggars',
    title: '武状元苏乞儿',
    director: '陈嘉上',
    year: '1992',
    studio: '永盛电影',
    region: '中国香港',
    cast: [
      { name: '陈嘉上', role: '导演', avatar: $r('app.media.cjs') },
      { name: '周星驰', role: '饰 苏灿', avatar: $r('app.media.zxc') },
      { name: '张敏', role: '饰 如霜', avatar: $r('app.media.zm') }
    ],
    tags: ['喜剧'],
    rating: 8.2,
    ratingCount: 100,
    summary: '从富家子弟到丐帮帮主，周星驰演绎的大起大落与悲喜人生。',
    coverRes: $r('app.media.wzysqe'),
    badgeText: '喜剧佳作'
  },
  {
    id: 'movie_fist_of_fury',
    title: '精武门',
    director: '罗维',
    year: '1972',
    studio: '嘉禾电影',
    region: '中国香港',
    cast: [
      { name: '罗维', role: '导演', avatar: $r('app.media.lw') },
      { name: '李小龙', role: '饰 陈真', avatar: $r('app.media.lxl') },
      { name: '苗可秀', role: '饰 袁丽儿', avatar: $r('app.media.mkx2') }
    ],
    tags: ['动作'],
    rating: 8.6,
    ratingCount: 100,
    summary: '李小龙经典之作，中国人不是东亚病夫！民族气节与实战功夫的完美结合。',
    coverRes: $r('app.media.jwm'),
    badgeText: '动作经典'
  },
  {
    id: 'movie_interstellar',
    title: '星际穿越',
    director: 'Christopher Nolan',
    year: '2014',
    studio: 'Warner Bros.',
    region: '美国',
    cast: [
      { name: 'Christopher Nolan', role: '导演', avatar: $r('app.media.C_N') },
      { name: 'Matthew McConaughey', role: '饰 Cooper', avatar: $r('app.media.M_M') },
      { name: 'Jessica Chastain', role: '饰 Murph', avatar: $r('app.media.J_C')},
      { name: 'Anne Hathaway', role: '饰 Brand', avatar: $r('app.media.A_H') }
    ],
    tags: ['科幻'],
    rating: 9.5,
    ratingCount: 100,
    summary: '爱是穿越时空的唯一维度。硬科幻与人文情感的巅峰之作。',
    coverRes: $r('app.media.interstellar'),
    badgeText: '殿堂级'
  },
  {
    id: 'movie_inception',
    title: '盗梦空间',
    director: 'Christopher Nolan',
    year: '2010',
    studio: 'Warner Bros.',
    region: '美国 / 英国',
    cast: [
      { name: 'Christopher Nolan', role: '导演', avatar: $r('app.media.C_N') },
      { name: 'Leonardo DiCaprio', role: '饰 Cobb', avatar: $r('app.media.L_D') },
      { name: 'Joseph Gordon-Levitt', role: '饰 Arthur', avatar: $r('app.media.J_G_L') }
    ],
    tags: ['科幻'],
    rating: 9.2,
    ratingCount: 100,
    summary: '既然做梦，就做大点。多层梦境的视觉奇观与逻辑迷宫。',
    coverRes: $r('app.media.inception')
  }
]