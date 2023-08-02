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

import CommonConstants from '../common/constants/CommonConstants';
import { NewsData } from '../common/bean/NewsData';

export class NewsDataModel {
  static getNewsByType(news: NewsData[], type: string): NewsData[] {
    if (type === CommonConstants.ALL_TITLE[0]) {
      return news;
    }
    return news.filter(item => (item.newsTypeStr === type));
  }

  static getTypeByStr(type: string): Resource {
    let newsType: Resource;
    switch (type) {
      case CommonConstants.ALL_TITLE[0]:
        newsType = $r('app.string.tabs_all');
        break;
      case CommonConstants.ALL_TITLE[1]:
        newsType = $r('app.string.tabs_health');
        break;
      case CommonConstants.ALL_TITLE[2]:
        newsType = $r('app.string.tabs_finance');
        break;
      case CommonConstants.ALL_TITLE[3]:
        newsType = $r('app.string.tabs_technology');
        break;
      case CommonConstants.ALL_TITLE[4]:
        newsType = $r('app.string.tabs_sports');
        break;
      case CommonConstants.ALL_TITLE[5]:
        newsType = $r('app.string.tabs_internet');
        break;
      case CommonConstants.ALL_TITLE[6]:
        newsType = $r('app.string.tabs_game');
        break;
      default:
        break;
    }
    return newsType;
  }

  static getNewsComposition(): NewsData[] {
    let newsComposition: NewsData[] = [];
    for (let i: number = 0; i < CommonConstants.NEWS_ID.length; i++) {
      let newsId: string = CommonConstants.NEWS_ID[i];
      let newsTypeStr: string = CommonConstants.ALL_TITLE[i % CommonConstants.TITLES_NUMBER + 1];
      let newsType: Resource = this.getTypeByStr(newsTypeStr);
      let title: Resource;
      let content: Resource;
      let imgUrl: Resource;
      let label: Resource;
      let redLabel: boolean;
      let from: Resource;
      switch (i % CommonConstants.IMAGES_NUMBER) {
        case 0:
          imgUrl = $r('app.media.ic_news_image1');
          label = $r('app.string.news_label_top');
          from = $r('app.string.xxx_daily');
          redLabel = true;
          break;
        case 1:
          imgUrl = $r('app.media.ic_news_image2');
          label = $r('app.string.news_label_more_comment');
          from = $r('app.string.xx_daily');
          redLabel = true;
          break;
        case 2:
          imgUrl = $r('app.media.ic_news_image3');
          label = $r('app.string.news_label_recommend');
          from = $r('app.string.xx_com');
          redLabel = false;
          break;
        case 3:
          imgUrl = $r('app.media.ic_news_image4');
          label = $r('app.string.news_label_recommend');
          from = $r('app.string.news_client');
          redLabel = false;
          break;
        default:
          break;
      }
      switch (i % CommonConstants.NEWS_ID.length) {
        case 0:
          title = $r('app.string.news_1_title');
          content = $r('app.string.news_1_content');
          break;
        case 1:
          title = $r('app.string.news_2_title');
          content = $r('app.string.news_2_content');
          break;
        case 2:
          title = $r('app.string.news_3_title');
          content = $r('app.string.news_3_content');
          break;
        case 3:
          title = $r('app.string.news_4_title');
          content = $r('app.string.news_4_content');
          break;
        case 4:
          title = $r('app.string.news_5_title');
          content = $r('app.string.news_5_content');
          break;
        case 5:
          title = $r('app.string.news_6_title');
          content = $r('app.string.news_6_content');
          break;
        case 6:
          title = $r('app.string.news_7_title');
          content = $r('app.string.news_7_content');
          break;
        case 7:
          title = $r('app.string.news_8_title');
          content = $r('app.string.news_8_content');
          break;
        case 8:
          title = $r('app.string.news_9_title');
          content = $r('app.string.news_9_content');
          break;
        case 9:
          title = $r('app.string.news_10_title');
          content = $r('app.string.news_10_content');
          break;
        case 10:
          title = $r('app.string.news_11_title');
          content = $r('app.string.news_11_content');
          break;
        case 11:
          title = $r('app.string.news_12_title');
          content = $r('app.string.news_12_content');
          break;
        case 12:
          title = $r('app.string.news_13_title');
          content = $r('app.string.news_13_content');
          break;
        case 13:
          title = $r('app.string.news_14_title');
          content = $r('app.string.news_14_content');
          break;
        case 14:
          title = $r('app.string.news_15_title');
          content = $r('app.string.news_15_content');
          break;
        case 15:
          title = $r('app.string.news_16_title');
          content = $r('app.string.news_16_content');
          break;
        case 16:
          title = $r('app.string.news_17_title');
          content = $r('app.string.news_17_content');
          break;
        case 17:
          title = $r('app.string.news_18_title');
          content = $r('app.string.news_18_content');
          break;
        case 18:
          title = $r('app.string.news_19_title');
          content = $r('app.string.news_19_content');
          break;
        default:
          break;
      }

      let reads: string = CommonConstants.NEWS_READS[i];
      let likes: string = CommonConstants.NEWS_LIKES[i];
      let newsItem: NewsData = new NewsData(newsId, title, newsTypeStr, newsType, imgUrl, reads, likes, content, label,
        redLabel, from);
      newsComposition.push(newsItem);
    }
    return newsComposition;
  }
}