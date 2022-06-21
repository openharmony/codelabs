/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import {GoodsData,IconImage} from './GoodsData'
import {Menu1, ImageItem} from './Menu'
import {ArsData} from './ArsData'

export function initializeOnStartup(): Array<GoodsData> {
  let GoodsDataArray: Array<GoodsData> = []
  GoodsComposition.forEach(item => {
    console.log(item.title);
    GoodsDataArray.push(new GoodsData(item.title, item.content, item.price, item.imgSrc));
  })
  return GoodsDataArray;
}

export function getIconPath(): Array<IconImage> {
  let IconPath: Array<IconImage> = []
  iconList.forEach(item => {
    IconPath.push(new IconImage(item.imgSrc));
  })
  return IconPath;
}

export function getIconPathSelect(): Array<IconImage> {
  let IconPathSelect: Array<IconImage> = []
  iconListSelect.forEach(item => {

    IconPathSelect.push(new IconImage( item.imgSrc));
  })
  return IconPathSelect;
}

export function getDetailImages(): Array<IconImage> {
  let detailImages: Array<IconImage> = []
  detailImageList.forEach(item => {

    detailImages.push(new IconImage( item.imgSrc));
  })
  return detailImages;
}


export function getMenu(): Array<Menu1> {
  let MenuArray: Array<Menu1> = []
  MyMenu.forEach(item => {
    MenuArray.push(new Menu1(item.title, item.num));
  })
  return MenuArray;
}

export function getTrans(): Array<ImageItem> {
  let ImageItemArray: Array<ImageItem> = []
  MyTrans.forEach(item => {
    ImageItemArray.push(new ImageItem(item.title, item.imageSrc));
  })
  return ImageItemArray;
}

export function getMore(): Array<ImageItem> {
  let ImageItemArray: Array<ImageItem> = []
  MyMore.forEach(item => {
    ImageItemArray.push(new ImageItem(item.title, item.imageSrc));
  })
  return ImageItemArray;
}

export function getArs(): Array<ArsData> {
  let ArsItemArray: Array<ArsData> = []
  ArsList.forEach(item => {
    ArsItemArray.push(new ArsData(item.title, item.content));
  })
  return ArsItemArray;
}

const GoodsComposition: any[] = [
  {
    "title": 'HUAWEI nova 8 Pro ',
    "content": 'Goes on sale: 10:08',
    "price": '3999',
    "imgSrc": $r("app.media.HW1")
  },
  {
    "title": 'HUAWEI Mate 30E Pro 5G',
    "content": '3 interest-free payments ',
    "price": '5299',
    "imgSrc": $r("app.media.HW2")
  },
  {
    "title": 'HUAWEI MatePad Pro',
    "content": 'Flagship ',
    "price": '3799',
    "imgSrc": $r("app.media.HW3")
  },
  {
    "title": 'HUAWEI Nova 8 Pro',
    "content": 'New arrival ',
    "price": '3999',
    "imgSrc": $r("app.media.HW4")
  },
  {
    "title": 'HUAWEI WATCH FIT',
    "content": 'Versatile',
    "price": '769',
    "imgSrc": $r("app.media.HW5")
  },
  {
    "title": 'HUAWEI nova 8 Pro ',
    "content": 'Goes on sale: 10:08',
    "price": '3999',
    "imgSrc": $r("app.media.HW6")
  },
  {
    "title": 'HUAWEI Mate 30E Pro 5G',
    "content": '3 interest-free payments ',
    "price": '5299',
    "imgSrc": $r("app.media.HW7")
  },
  {
    "title": 'HUAWEI MatePad Pro',
    "content": 'Flagship ',
    "price": '3799',
    "imgSrc": $r("app.media.HW8")
  },
  {
    "title": 'HUAWEI Nova 8 Pro',
    "content": 'New arrival ',
    "price": '3999',
    "imgSrc": $r("app.media.HW9")
  },
  {
    "title": 'HUAWEI WATCH FIT',
    "content": 'Versatile',
    "price": '769',
    "imgSrc": $r("app.media.HW10")
  },
]

const iconList: any[] = [
  {
    "imgSrc": $r('app.media.icon_buy1')
  },
  {
    "imgSrc": $r('app.media.icon_shopping_cart')
  },
  {
    "imgSrc": $r('app.media.icon_my')
  },
]

const iconListSelect: any[] = [
  {
    "imgSrc": $r('app.media.icon_home')
  },
  {
    "imgSrc": $r('app.media.icon_shopping_cart_select')
  },
  {
    "imgSrc": $r('app.media.icon_my_select')
  },
]

const detailImageList: any[] = [
  {
    "imgSrc": $r('app.media.computer1')
  },
  {
    "imgSrc": $r('app.media.computer2')
  },
  {
    "imgSrc": $r('app.media.computer3')
  },
  {
    "imgSrc": $r('app.media.computer4')
  },
  {
    "imgSrc": $r('app.media.computer5')
  },
  {
    "imgSrc": $r('app.media.computer6')
  },
  {
    "imgSrc": $r('app.media.computer3')
  },
]


const MyMenu: any[] = [
  {
    'title': 'Favorites',
    'num': '10'
  },
  {
    'title': 'Searched',
    'num': '1000'
  },
  {
    'title': 'Following',
    'num': '100'
  },
  {
    'title': 'Followers',
    'num': '10000'
  }
]

const MyTrans: any[] = [
  {
    'title': 'Post: 520',
    'imageSrc': $r("app.media.icon_menu_release")
  },
  {
    'title': 'Sold: 520',
    'imageSrc':$r("app.media.icon_menu_sell")
  },
  {
    'title': 'Bought: 10',
    'imageSrc': $r("app.media.icon_menu_buy")
  }
]

const MyMore: any[] = [
  {
    'title': 'Guide',
    'imageSrc': $r("app.media.icon_menu_buy")
  },
  {
    'title': 'Create',
    'imageSrc': $r("app.media.icon_menu_buy")
  },
  {
    'title': 'Poster',
    'imageSrc': $r("app.media.icon_menu_buy")
  },
  {
    'title': 'Games',
    'imageSrc': $r("app.media.icon_menu_buy")
  },
  {
    'title': 'Jobber',
    'imageSrc': $r("app.media.icon_menu_buy")
  },
  {
    'title': 'Myself',
    'imageSrc': $r("app.media.icon_menu_buy")
  },
  {
    'title': 'About',
    'imageSrc': $r("app.media.icon_menu_buy")
  },
  {
    'title': 'Rental',
    'imageSrc': $r("app.media.icon_menu_buy")
  },
  {
    'title': 'Author',
    'imageSrc': $r("app.media.icon_menu_buy")
  },

]

const ArsList: any[] = [
  {
    'title': 'Display Size',
    'content': '13.9 inches',
  },
  {
    'title': 'Memory',
    'content': '16 GB',
  },
  {
    'title': 'Marketing Name',
    'content': 'HUAWEI MateBook X Pro',
  },
  {
    'title': 'Color Gamut',
    'content': '100% sRGB color gamut (Typical)',
  },
  {
    'title': 'Battery',
    'content': '56 Wh (rated capacity)',
  },
  {
    'title': 'Storage',
    'content': '512 GB',
  },
  {
    'title': 'Resolution',
    'content': '3000x2000',
  },
  {
    'title': 'Processor',
    'content': '11th Gen Intel® Core™ i7-1165G7 Processor',
  },
  {
    'title': 'CPU Cores',
    'content': '4',
  },
  {
    'title': 'Launch Time',
    'content': 'January 2021',
  }
]