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

let NextId = 0;
export class GoodsData {
  id: string;
  title: string;
  content: string;
  price: number;
  imgSrc: Resource;

  constructor(title: string, content: string, price: number, imgSrc: Resource) {
    this.id = `${NextId++}`;
    this.title = title;
    this.content = content;
    this.price = price;
    this.imgSrc = imgSrc;
  }
}

export class IconImage {
  id: string;
  imgSrc: Resource;

  constructor(imgSrc: Resource) {
    this.id = `${NextId++}`;
    this.imgSrc = imgSrc;
  }
}