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

import router from '@ohos.router';
import CommonConstants from '../../common/constant/commonConstants';

export default {
  data: {
    swiperVideos: [],
    horizontal_description: [],
    horizontal_videos: []
  },

  /**
   * Initialize data.
   */
  onInit() {
    this.swiperVideos = CommonConstants.SWIPER_VIDEOS;
    this.horizontal_description =  [this.$t('strings.recently'), this.$t('strings.featured')],
    this.horizontal_videos = [
    {
        'image': 'common/images/ic_video1.png',
        'name': this.$t('strings.video_first')
      },
    {
        'image': 'common/images/ic_video2.png',
        'name': this.$t('strings.video_second')
      },
    {
        'image': 'common/images/ic_video3.png',
        'name': this.$t('strings.video_third')
      }
    ]
  },

  /**
   * The playback page is displayed.
   */
  playVideo() {
    router.push({
      url: CommonConstants.PLAY_PAGE
    });
  },
};

