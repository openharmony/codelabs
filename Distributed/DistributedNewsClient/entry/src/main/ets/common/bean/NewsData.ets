/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

export class NewsData {
  /**
   * The Id of news.
   */
  newsId: string;

  /**
   * The title of news.
   */
  title: Resource;

  /**
   * The type string of news.
   */
  newsTypeStr: string;

  /**
   * The type of news.
   */
  newsType: Resource;

  /**
   * The image url of news.
   */
  imgUrl: Resource;

  /**
   * The reads of news.
   */
  reads: string;

  /**
   * The likes of news.
   */
  likes: string;

  /**
   * The content of news.
   */
  content: Resource;

  /**
   * The label of news.
   */
  label: Resource;

  /**
   * The color of label.
   */
  redLabel: boolean;

  /**
   * The from of news.
   */
  from: Resource;

  constructor(newsId: string, title: Resource, newsTypeStr: string, newsType: Resource, imgUrl: Resource, reads: string,
              likes: string, content: Resource, label: Resource, redLabel: boolean, from: Resource) {
    this.newsId = newsId;
    this.title = title;
    this.newsTypeStr = newsTypeStr;
    this.newsType = newsType;
    this.imgUrl = imgUrl;
    this.reads = reads;
    this.likes = likes;
    this.content = content;
    this.label = label;
    this.redLabel = redLabel;
    this.from = from;
  }
}