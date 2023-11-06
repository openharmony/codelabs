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

interface XLocationEnumInterface {
  XLeft: number,
  XRight: number,
  noChange: number
}

interface YLocationEnumInterface {
  YTop: number,
  YBottom: number,
  noChange: number
}

/**
 * The x location to be changed.
 */
export const XLocationEnum: XLocationEnumInterface = {
  XLeft: 0,
  XRight: 1,
  noChange: -1
}

/**
 * The y location to be changed.
 */
export const YLocationEnum: YLocationEnumInterface = {
  YTop: 0,
  YBottom: 1,
  noChange: -1
}

export interface ScaleInterface {
  x: number,
  y: number
}

/**
 * Common constants for project.
 */
export default class CommonConstants {
  /**
   * Minimum finger moving distance.
   */
  static readonly MINIMUM_FINGER_DISTANCE: number = 50;

  /**
   * Scale-down in the x direction.
   */
  static readonly X_SCALE_DOWN: number = 0.85;

  /**
   * Scale-down in the y direction.
   */
  static readonly Y_SCALE_DOWN: number = 0.85;

  /**
   * No scale-down in the x and y direction.
   */
  static readonly NO_SCALE_DOWN: number = 1;

  /**
   * Font Weight of the title.
   */
  static readonly TITLE_FONT_WEIGHT: number = 700;

  /**
   * Top margin of the main flexLayout.
   */
  static readonly MAIN_TOP_MARGIN: string = '10%';

  /**
   * Full width and height.
   */
  static readonly FULL_PARAM: string = '100%';

  /**
   * Offset determination range.
   */
  static readonly OFFSET_RANGE: number = 15;

  /**
   * Default value for offsetXLeft.
   */
  static readonly DEFAULT_OFFSET_X_LEFT: number = 120;

  /**
   * Default value for offsetXRight.
   */
  static readonly DEFAULT_OFFSET_X_RIGHT: number = 370;

  /**
   * Default value for offsetYTop.
   */
  static readonly DEFAULT_OFFSET_Y_TOP: number = 200;

  /**
   * Default value for offsetYBottom.
   */
  static readonly DEFAULT_OFFSET_Y_BOTTOM: number = 450;

  /**
   * Number of black areas in a single direction.
   */
  static readonly BLACK_AREA_NUM: number = 2;

  /**
   * Color of the unselected area.
   */
  static readonly UNSELECT_AREA_COLOR: string = '#FF000000';

  /**
   * Alpha of the unselected area.
   */
  static readonly UNSELECT_AREA_ALPHA: number = 0.6;

  /**
   * Color of the box selection lines.
   */
  static readonly LINES_COLOR: string = '#FFFFFFFF';

  /**
   * Line Width of the box selection lines.
   */
  static readonly LINES_WIDTH: number = 3;

  /**
   * Alpha of the box selection lines.
   */
  static readonly LINES_ALPHA: number = 1;

  /**
   * Maximum line length.
   */
  static readonly LINES_MAX_LENGTH: number = 32;

  /**
   * Gap Width.
   */
  static readonly GAP_WIDTH: number = 2 + CommonConstants.LINES_WIDTH / 2;

  /**
   * Line Style.
   */
  static readonly LINES_STYLE: CanvasLineJoin = 'miter';

  /**
   * ToolBar width.
   */
  static readonly TOOLBAR_WIDTH: string = '85%';

  /**
   * Image Margin.
   */
  static readonly IMAGE_MARGIN: string = '5%';

  /**
   * Screenshot Dialog Margin.
   */
  static readonly SCREENSHOT_DIALOG_MARGIN: Margin = {
    bottom: '15%',
    right: $r('app.float.main_margin_size')
  }

  /**
   * Width when width first.
   */
  static readonly WIDTH_FIRST: string = '30%';

  /**
   * Height when height first.
   */
  static readonly HEIGHT_FIRST: number = 30;

  /**
   * Width and height of the image on the ScreenshotDialog.
   */
  static readonly IMAGE_ON_DIALOG: string = '90%';
}