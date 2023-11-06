/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

export const WEB_PAGE_URI: string = 'pages/WebPage';

export const LOCAL_PATH: Resource = $rawfile('index.html');

/**
 * The host address of the server.
 */
export const CLOUD_PATH: string = 'http://xxx.xxx.xxx.xxx:9588/index.html';

export const WEB_ALERT_DIALOG_TEXT_VALUE: string = '恭喜您抽中:';

export class WebConstant {
  static readonly FULL_WIDTH: string = '100%'
  static readonly FULL_HEIGHT: string = '100%'
  static readonly WIDTH: string = '93.3%'
  static readonly HEIGHT: string = '55.9%'
  static readonly MARGIN_TOP: string = '7.1%'
  static readonly MARGIN_LEFT: string = '3.3%'
  static readonly MARGIN_RIGHT: string = '3.3%'
  static readonly TOP_ROW_HEIGHT: string = '7.2%'
  static readonly IMAGE_WIDTH: string = '5.6%'
  static readonly IMAGE_HEIGHT: string = '32%'
  static readonly IMAGE_MARGIN_LEFT: string = '7.2%'
  static readonly TEXT_VALUE_WIDTH: string = '35.6%'
  static readonly TEXT_VALUE_HEIGHT: string = '3.1%'
  static readonly TEXT_VALUE_MARGIN_TOP: string = '3.1%'
  static readonly TEXT_VALUE_FONT_SIZE: number = 18
  static readonly TEXT_VALUE_FONT_WEIGHT: number = 500
  static readonly TIP_TEXT_VALUE_WIDTH: string = '15.6%'
  static readonly TIP_TEXT_VALUE_HEIGHT: string = '2.4%'
  static readonly TIP_TEXT_VALUE_FONT_SIZE: number = 14
  static readonly TIP_TEXT_VALUE_MARGIN_TOP: string = '0.9%'
  static readonly TIP_TEXT_VALUE_OPACITY: number = 0.6
  static readonly TOP_TEXT_WIDTH: string = '82.2%'
  static readonly TOP_TEXT_HEIGHT: string = '50%'
  static readonly TOP_TEXT_MARGIN_LEFT: string = '5%'
  static readonly TOP_TEXT_FONT_SIZE: number = 20
  static readonly BUTTON_WIDTH: string = '86.7%'
  static readonly BUTTON_HEIGHT: string = '5.1%'
  static readonly BUTTON_MARGIN_TOP: string = '10%'
  static readonly BUTTON_BORDER_RADIUS: string = '20'
  static readonly BUTTON_FONT_SIZE: number = 16
  static readonly DURATION: number = 3000
  static readonly PROGRESS_MIN: number = 0
  static readonly PROGRESS_MAX: number = 100
  static readonly PROGRESS_STEP: number = 6.67
  static readonly MILLI_SECONDS: number = 100
  static readonly PROGRESS_STROKE_WIDTH: number = 15
  static readonly PROGRESS_SCALE_COUNT: number = 15
  static readonly PROGRESS_SCALE_WIDTH: number = 5
  static readonly PROGRESS_WIDTH: number = 80
  static readonly PROGRESS_POSITION_X: string = '40%'
  static readonly PROGRESS_POSITION_Y: string = '35%'
}

export class MainConstant {
  static readonly FULL_HEIGHT: string = '100%'
  static readonly IMAGE_HEIGHT: string = '38.7%'
  static readonly BUTTON_MARGIN_TOP_BUTTON: string = '3.1%'
  static readonly BUTTON_HEIGHT: string = '5.1%'
  static readonly BUTTON_MARGIN_TOP: string = '4.6%'
  static readonly BUTTON_WIDTH: string = '86.7%'
  static readonly BUTTON_BORDER_RADIUS: number = 20
  static readonly BUTTON_FONT_SIZE: number = 16
}



