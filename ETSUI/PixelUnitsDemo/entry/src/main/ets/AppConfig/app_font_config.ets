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
enum FontSize {
  SMALLER = 18,
  SMALL = 24,
  MIDDLE = 28,
  LARGE = 32,
  LARGER = 35,
}

export enum AppFontSize {
  /**
   * Indicates that the fontSize of the app button.
   */
  BUTTON = FontSize.SMALL,
  BUTTON_LARGE = FontSize.LARGE,
  /**
   * Indicates that the fontSize of the app title.
   */
  TITLE = FontSize.LARGER,
  SUBTITLE = FontSize.LARGE,
  /**
   * Indicates that the fontSize of the app normal text.
   */
  TEXT = FontSize.SMALL,
  TEXT_BIG = FontSize.MIDDLE,
  TEXT_SMALL = FontSize.SMALLER
}