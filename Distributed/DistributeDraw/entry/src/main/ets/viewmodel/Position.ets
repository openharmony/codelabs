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

export default class Position {
  /**
   * Indicates whether the position is the start position.
   */
  isFirstPosition: boolean;

  /**
   * Indicates whether the position is the end position.
   */
  isEndPosition: boolean;

  /**
   * Position x.
   */
  positionX: number;

  /**
   * Position y.
   */
  positionY: number;

  constructor(isFirstPosition: boolean, isEndPosition: boolean, positionX: number, positionY: number) {
    this.isFirstPosition = isFirstPosition;
    this.isEndPosition = isEndPosition;
    this.positionX = positionX;
    this.positionY = positionY;
  }
}