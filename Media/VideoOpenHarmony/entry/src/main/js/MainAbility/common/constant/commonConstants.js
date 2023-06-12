/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Common constants for all features.
 */
export default class CommonConstants {
  /**
   * Swiper image.
   */
  static SWIPER_VIDEOS = [
    '/common/images/ic_banner1.png',
    '/common/images/ic_banner2.png',
    '/common/images/ic_banner3.png'
  ];

  /**
   * Playback page path.
   */
  static PLAY_PAGE = 'pages/videoPage/videoPage';

  /**
   * Icon on the playback page.
   */
  static PLAY_ICONS = {
    backSrc: '/common/images/ic_back.png',
    pauseIcon: '/common/images/ic_pause.png',
    playIcon: '/common/images/ic_play.png',
    publicPlayIcon: '/common/images/ic_public_play.png',
  }

  /**
   * Video Address.
   */
  static VIDEO_SRC = '/common/video/video.mp4';
}