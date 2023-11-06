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
  static readonly START_IMAGE_WIDTH: string = '53.3%';
  static readonly START_IMAGE_HEIGHT: string = '24.6%';
  static readonly START_IMAGE_CONTAINER: string = '75.5%';
  /**
   * Music list page const.
   */
  static readonly TEXT_MARGIN_LEFT: string = '6.7%';
  static readonly TEXT_MARGIN_RIGHT: string = '6.7%';
  static readonly TEXT_MARGIN_TOP: string = '1.5%';
  static readonly TEXT_MARGIN_BOTTOM: string = '1.5%';
  static readonly TEXT_WIDTH: string = '86.7%';
  static readonly TEXT_HEIGHT: string = '6.7%';
  /**
   * Play page const.
   */
  static readonly NAVIGATOR_WIDTH: string = '6.7%';
  static readonly NAVIGATOR_HEIGHT: string = '3.1%';
  static readonly LAYOUT_WEIGHT: number = 1;
  static readonly NAVIGATOR_PADDING_LEFT: string = '6.7%';
  static readonly NAVIGATOR_PADDING_RIGHT: string = '6.7%';
  static readonly NAVIGATOR_PADDING_TOP: string = '1.9%';
  static readonly NAVIGATOR_PADDING_BOTTOM: string = '1.9%';
  static readonly CONTROL_WIDTH: string = '81.2%';
  /**
   * Control view const.
   */
  static readonly MODE_ICON_WIDTH: string = '7.5%';
  static readonly MODE_ICON_HEIGHT: string = '3.1%';
  static readonly MODE_ICON_MARGIN_RIGHT: string = '12%';
  static readonly PREVIOUS_WIDTH: string = '8.3%';
  static readonly PREVIOUS_HEIGHT: string = '3.8%';
  static readonly PREVIOUS_MARGIN_RIGHT: string = '9%';
  static readonly STATE_ICON_WIDTH: string = '18.9%';
  static readonly STATE_ICON_HEIGHT: string = '8.7%';
  static readonly STATE_ICON_MARGIN_RIGHT: string = '9%';
  static readonly NEXT_WIDTH: string = '8.3%';
  static readonly NEXT_HEIGHT: string = '3.8%';
  static readonly NEXT_MARGIN_RIGHT: string = '12%';
  static readonly PLAY_LIST_WIDTH: string = '7.5%';
  static readonly PLAY_LIST_HEIGHT: string = '3.1%';
  static readonly PAGE_MARGIN_TOP: string = '4.6%';
  /**
   * Music card view const.
   */
  static readonly IMAGE_HEIGHT: string = '41.7%';
  static readonly NAME_MARGIN_TOP: string = '3.1%';
  static readonly SINGER_MARGIN_TOP: string = '1.5%';
  static readonly LYRICS_MARGIN_TOP: string = '1%';
  /**
   * Music view const.
   */
  static readonly IMAGE_WIDTH: string = '17.8%';
  static readonly MUSIC_IMAGE_HEIGHT: string = '8.2%';
  static readonly ROW_MARGIN_LEFT: string = '4.4%';
  static readonly PAGE_MARGIN_LEFT: string = '3%';
  static readonly MUSIC_MARGIN_TOP: string = '1.5%';
  static readonly PAGE_MARGIN_RIGHT: string = '3%';
  static readonly PAGE_MARGIN_BUTTON: string = '1.5%';
  /**
   * List dialog const.
   */
  static readonly DIALOG_IMAGE_WIDTH: string = '6.7%';
  static readonly DIALOG_IMAGE_HEIGHT: string = '3.1%';
  static readonly IMAGE_MARGIN_RIGHT: string = '3.3%';
  static readonly ROW_WIDTH: string = '90%';
  static readonly ROW_MARGIN_TOP: string = '3%';
  static readonly ROW_MARGIN_BOTTOM: string = '3%';
  static readonly LIST_WIDTH: string = '90%';
  static readonly LIST_HEIGHT: string = '57.4%';
  static readonly DIALOG_MARGIN_LEFT: string = '3.3%';
  static readonly LIST_SPACE: number = 10;
  static readonly LIST_INITIAL_INDEX: number = 0;
  static readonly DIVIDER_STROKE_WIDTH: number = 1;
  static readonly DIVIDER_START_MARGIN: number = 0;
  static readonly END_MARGIN: number = 20;
  /**
   * List dialog music.
   */
  static readonly LIST_IMAGE_WIDTH: string = '4.4%';
  static readonly LIST_IMAGE_HEIGHT: string = '2.1%';
  static readonly LIST_MARGIN_RIGHT: string = '2.1%';
  static readonly LINE_HEIGHT: number = 21;
  static readonly LIST_MARGIN_TOP: string = '1.7%';
  static readonly MUSIC_MARGIN_BOTTOM: string = '1.7%';
  /**
   * Progress view const.
   */
  static readonly PROGRESS_MARGIN_RIGHT: string = '3.3%';
  static readonly PROGRESS_MARGIN_LEFT: string = '3.3%';
  static readonly VIEW_MARGIN_TOP: string = '8.2%';
  static readonly PROGRESS_MIN: number = 0;
  /**
   * BackgroundTask parameter.
   */
  static readonly BACKGROUND_REQUEST_CODE: number = 0;
  /**
   * Const of commonUtil.
   */
  static readonly SECOND_PRECISION: number = 0;
  static readonly SECOND_TO_MS: number = 1000;
  static readonly MIN_TO_SEC: number = 60;
  static readonly MAX_OF_INDIVIDUAL: number = 9;
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