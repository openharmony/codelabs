/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import LazyDataSource from '../util/LazyDataSource';

export class DataSourceStyleAStyleA {
  private static instance: DataSourceStyleAStyleA;
  lazyDataSource: LazyDataSource<VerticalGraphicTextShareItemModel> = new LazyDataSource();
  static readonly IMAGES: Resource[] = [
    $r(('app.media.realImageGen_shop_3')),
    $r(('app.media.realImageGen_shop_2')),
    $r(('app.media.realImageGen_shop_6')),
    $r(('app.media.realImageGen_shop_3'))
  ];
  static readonly TITLES: string[] = ['智能手表', '无线耳机', '运动鞋', '保温杯'];
  static readonly SUBTITLES: string[] =
    ['多功能健康监测 超长续航', '降噪技术 舒适佩戴', '轻便透气 防滑耐磨', '保温保冷 便携设计'];
  static readonly PRICE_LABELS: string[] = ['¥599', '¥899', '¥399', '¥129'];
  static readonly PRICE_DETAIL: string[] =
    ['限时优惠 赠表带', '旗舰款 享24期免息', '新品首发 满减活动', '热销爆款 买一送一'];

  private constructor() {
  }

  public static getInstance(): DataSourceStyleAStyleA {
    if (!DataSourceStyleAStyleA.instance) {
      DataSourceStyleAStyleA.instance = new DataSourceStyleAStyleA();
    }
    return DataSourceStyleAStyleA.instance;
  }

  getResources(): Promise<void> {
    return new Promise((resolve: Function, reject: Function) => {
      try {
        const modelList: Array<VerticalGraphicTextShareItemModel> = [];
        for (let index = 0; index < DataSourceStyleAStyleA.IMAGES.length; index++) {
          modelList.push(new VerticalGraphicTextShareItemModel(
            DataSourceStyleAStyleA.IMAGES[index],
            DataSourceStyleAStyleA.TITLES[index],
            DataSourceStyleAStyleA.SUBTITLES[index],
            DataSourceStyleAStyleA.PRICE_LABELS[index],
            DataSourceStyleAStyleA.PRICE_DETAIL[index]
          ));
        }
        this.lazyDataSource.pushArrayData(modelList);
        resolve();
      } catch (error) {
        reject(error);
      }
    });
  }
}

export class VerticalGraphicTextShareItemModel {
  private image: Resource;
  private title: string;
  private subtitle: string;
  private price: string;
  private priceCaption: string;

  constructor(
    image: Resource,
    title: string,
    subtitle: string,
    price: string,
    priceCaption: string
  ) {
    this.image = image;
    this.title = title;
    this.subtitle = subtitle;
    this.price = price;
    this.priceCaption = priceCaption;
  }

  getImage(): Resource {
    return this.image;
  }

  getTitle(): string {
    return this.title;
  }

  getSubtitle(): string {
    return this.subtitle;
  }

  getPrice(): string {
    return this.price;
  }

  getPriceCaption(): string {
    return this.priceCaption;
  }
}