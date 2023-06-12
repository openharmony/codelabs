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

/**
 * Common constants for all features.
 */
export class CommonConstants {
  /**
   * FontWeight bolder.
   */
  static readonly FONT_WEIGHT_BOLDER: number = 500;

  /**
   * Weight of 100%.
   */
  static readonly FULL_WIDTH: string = '100%';

  /**
   * Height of 100%.
   */
  static readonly FULL_HEIGHT: string = '100%';

  /**
   * Music list page.
   */
  static readonly PAGE_MUSIC_LIST: string = 'pages/MusicList';

  /**
   * Play page.
   */
  static readonly PAGE_PLAY: string = 'pages/Play';

  /**
   * Start up delayTime.
   */
  static readonly DELAY_TIME: number = 1000;

  /**
   * Start position.
   */
  static readonly START_POSITION: number = 0;

  /**
   * Total value.
   */
  static readonly TOTAL_VALUE: number = 100;

  /**
   * Divider color.
   */
  static readonly DIVIDER_COLOR: number = 0x33000000;

  /**
   * Start up page const.
   */
  static readonly START_UP_PAGE = {
    IMAGE_WIDTH: '53.3%',
    IMAGE_HEIGHT: '24.6%',
    IMAGE_CONTAINER: '75.5%'
  }

  /**
   * Music list page const.
   */
  static readonly MUSIC_LIST_PAGE = {
    TEXT_MARGIN_LEFT: '6.7%',
    TEXT_MARGIN_RIGHT: '6.7%',
    TEXT_MARGIN_TOP: '1.5%',
    TEXT_MARGIN_BOTTOM: '1.5%',
    TEXT_WIDTH: '86.7%',
    TEXT_HEIGHT: '6.7%'
  }

  /**
   * Play page const.
   */
  static readonly PLAY_PAGE = {
    NAVIGATOR_WIDTH: '6.7%',
    NAVIGATOR_HEIGHT: '3.1%',
    LAYOUT_WEIGHT: 1,
    NAVIGATOR_PADDING_LEFT: '6.7%',
    NAVIGATOR_PADDING_RIGHT: '6.7%',
    NAVIGATOR_PADDING_TOP: '1.9%',
    NAVIGATOR_PADDING_BOTTOM: '1.9%',
    CONTROL_WIDTH: '81.2%'
  }

  /**
   * Control view const.
   */
  static readonly CONTROL_VIEW = {
    MODE_ICON_WIDTH: '7.5%',
    MODE_ICON_HEIGHT: '3.1%',
    MODE_ICON_MARGIN_RIGHT: '12%',
    PREVIOUS_WIDTH: '8.3%',
    PREVIOUS_HEIGHT: '3.8%',
    PREVIOUS_MARGIN_RIGHT: '9%',
    STATE_ICON_WIDTH: '18.9%',
    STATE_ICON_HEIGHT: '8.7%',
    STATE_ICON_MARGIN_RIGHT: '9%',
    NEXT_WIDTH: '8.3%',
    NEXT_HEIGHT: '3.8%',
    NEXT_MARGIN_RIGHT: '12%',
    PLAY_LIST_WIDTH: '7.5%',
    PLAY_LIST_HEIGHT: '3.1%',
    PAGE_MARGIN_TOP: '4.6%'
  }

  /**
   * Music card view const.
   */
  static readonly MUSIC_CARD_VIEW = {
    IMAGE_HEIGHT: '41.7%',
    NAME_MARGIN_TOP: '3.1%',
    SINGER_MARGIN_TOP: '1.5%',
    LYRICS_MARGIN_TOP: '1%'
  }

  /**
   * Music view const.
   */
  static readonly MUSIC_VIEW = {
    IMAGE_WIDTH: '17.8%',
    IMAGE_HEIGHT: '8.2%',
    ROW_MARGIN_LEFT: '4.4%',
    PAGE_MARGIN_LEFT: '3%',
    PAGE_MARGIN_TOP: '1.5%',
    PAGE_MARGIN_RIGHT: '3%',
    PAGE_MARGIN_BUTTON: '1.5%'
  }

  /**
   * List dialog const.
   */
  static readonly LIST_DIALOG = {
    IMAGE_WIDTH: '6.7%',
    IMAGE_HEIGHT: '3.1%',
    IMAGE_MARGIN_RIGHT: '3.3%',
    ROW_WIDTH: '90%',
    ROW_MARGIN_TOP: '3%',
    ROW_MARGIN_BOTTOM: '3%',
    LIST_WIDTH: '90%',
    LIST_HEIGHT: '57.4%',
    PAGE_MARGIN_LEFT: '3.3%',
    LIST_SPACE: 10,
    LIST_INITIAL_INDEX: 0,
    DIVIDER_STROKE_WIDTH: 1,
    DIVIDER_START_MARGIN: 0,
    END_MARGIN: 20
  }

  /**
   * List dialog music.
   */
  static readonly LIST_DIALOG_MUSIC = {
    IMAGE_WIDTH: '4.4%',
    IMAGE_HEIGHT: '2.1%',
    IMAGE_MARGIN_RIGHT: '2.1%',
    LINE_HEIGHT: 21,
    MUSIC_MARGIN_TOP: '1.7%',
    MUSIC_MARGIN_BOTTOM: '1.7%'
  }

  /**
   * Progress view const.
   */
  static readonly PROGRESS_VIEW = {
    TEXT_MARGIN_RIGHT: '3.3%',
    TEXT_MARGIN_LEFT: '3.3%',
    VIEW_MARGIN_TOP: '8.2%',
    PROGRESS_MIN: 0
  }

  /**
   * BackgroundTask parameter.
   */
  static readonly BACKGROUND_REQUEST_CODE: number = 0;

  /**
   * Const of commonUtil.
   */
  static readonly COMMON_UTIL = {
    SECOND_PRECISION: 0,
    SECOND_TO_MS: 1000,
    MIN_TO_SEC: 60,
    MAX_OF_INDIVIDUAL: 9
  }

  /**
   * Default id.
   */
  static readonly DEFAULT_TIME_ID: number = -1;

  /**
   * Number of modes.
   */
  static readonly MODE_QUANTITY: number = 3;

  /**
   * Number of musics.
   */
  static readonly MUSICS_QUANTITY: number = 6;

  /**
   * Bundle name.
   */
  static readonly BUNDLE_NAME: string = 'com.example.audioplayer';

  /**
   * Ability name.
   */
  static readonly ABILITY_NAME: string = 'EntryAbility';
}

/**
 * Play mode.
 */
export enum PlayMode {
  LIST_LOOP = 0,
  SINGLE_LOOP = 1,
  RANDOM_PLAY = 2
}

/**
 * Play state.
 */
export enum PlayState {
  PLAY = 0,
  PAUSE = 1
}

/**
 * AVPlayer state.
 */
export enum PlayerState {
  IDLE = -1,
  INITIALIZED = 1
}

/**
 * State description.
 */
export enum StateEvent {
  IDLE = 'idle',
  INITIALIZED = 'initialized',
  PREPARED = 'prepared',
  PLAYING = 'playing',
  COMPLETED = 'completed'
}