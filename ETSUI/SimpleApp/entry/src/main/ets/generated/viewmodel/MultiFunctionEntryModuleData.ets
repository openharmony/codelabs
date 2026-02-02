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

export class Data {
  private static instance: Data;
  lazyDataSource: LazyDataSource<DataSource> = new LazyDataSource();
  static readonly ICONS: Resource[] =
    [$r('app.media.lib_icon_96'), $r('app.media.lib_icon_16'), $r('app.media.lib_icon_203'),
      $r('app.media.lib_icon_10')];
  static readonly TITLES: string[] = ['扫一扫', '付款码', '卡包', '出行'];
  static readonly DESCRIPTIONS: string[] = ['', '', '', ''];

  constructor() {
  }

  public static getInstance(): Data {
    if (!Data.instance) {
      Data.instance =
        new Data();
    }
    return Data.instance;
  }

  /**
   * Data loaded on current view converts network data into rendering data
   */
  getResources(): Promise<void> {
    /**
     * use your web request here
     */
    return new Promise((resolve: Function) => {
      const modelList: Array<DataSource> = [];
      for (let index = 0; index < Data.ICONS.length;
        index++) {
        modelList.push(new DataSource(
          Data.ICONS[index],
          Data.TITLES[index],
          Data.DESCRIPTIONS[index]));
      }
      this.lazyDataSource.pushArrayData(modelList);
      resolve();
    });
  }
}

export class DataSource {
  private icon: Resource;
  private title: string;
  private description: string;

  constructor(icon: Resource, title: string, description: string) {
    this.icon = icon;
    this.title = title;
    this.description = description;
  }

  getIcon(): Resource {
    return this.icon;
  }

  getTitle(): string {
    return this.title;
  }

  getDescription(): string {
    return this.description;
  }
}