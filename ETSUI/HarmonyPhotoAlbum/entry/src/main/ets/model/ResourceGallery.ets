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




export interface GalleryItem {
  id: number;
  src: Resource;
  name: string;
  category: string; // 如：风景 / 生活 / 美食 / 人物
  tags: string;     // 如："#日落 #海边"
}

// 统一图库：完全对应 entry/src/main/resources/base/media/ 下的所有图片
export const GALLERY_ITEMS: GalleryItem[] = [
  // 风景
  {
    id: 1,
    src: $r('app.media.ic_scene_0'),
    name: '城市夜景',
    category: '风景',
    tags: '#夜景 #城市'
  },
  {
    id: 2,
    src: $r('app.media.ic_scene_1'),
    name: '海边日落',
    category: '风景',
    tags: '#日落 #海边'
  },
  {
    id: 3,
    src: $r('app.media.ic_scene_2'),
    name: '山间清晨',
    category: '风景',
    tags: '#山 #清晨'
  },

  // 生活
  {
    id: 4,
    src: $r('app.media.ic_life_0'),
    name: '惬意生活',
    category: '生活',
    tags: '#日常 #记录'
  },
  {
    id: 5,
    src: $r('app.media.ic_life_1'),
    name: '午后时光',
    category: '生活',
    tags: '#咖啡 #午后'
  },
  {
    id: 6,
    src: $r('app.media.ic_life_2'),
    name: '街头一角',
    category: '生活',
    tags: '#街拍 #散步'
  },
  {
    id: 7,
    src: $r('app.media.ic_life_3'),
    name: '温馨时刻',
    category: '生活',
    tags: '#家庭 #温馨'
  },
  {
    id: 8,
    src: $r('app.media.ic_life_4'),
    name: '城市剪影',
    category: '生活',
    tags: '#剪影 #城市'
  },
  {
    id: 9,
    src: $r('app.media.ic_life_5'),
    name: '夜色霓虹',
    category: '生活',
    tags: '#夜景 #霓虹灯'
  },

  // 美食
  {
    id: 10,
    src: $r('app.media.ic_food_0'),
    name: '美味佳肴',
    category: '美食',
    tags: '#美食 #打卡'
  },
  {
    id: 11,
    src: $r('app.media.ic_food_1'),
    name: '甜品时刻',
    category: '美食',
    tags: '#甜品 #下午茶'
  },

  // 人物
  {
    id: 12,
    src: $r('app.media.ic_men_0'),
    name: '阳光少年',
    category: '人物',
    tags: '#笑容 #阳光'
  },
  {
    id: 13,
    src: $r('app.media.ic_men_2'),
    name: '城市旅人',
    category: '人物',
    tags: '#旅行 #背影'
  },
  {
    id: 14,
    src: $r('app.media.ic_men_3'),
    name: '思考瞬间',
    category: '人物',
    tags: '#思考 #肖像'
  },

  // 图标（作为特殊一张，可以归类为“生活”或“其他”）
  {
    id: 15,
    src: $r('app.media.icon'),
    name: '应用图标',
    category: '生活',
    tags: '#Icon #应用'
  }
];

