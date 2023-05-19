/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

export class Ratio {
  /**
   * Rect width.
   */
  private w: number;

  /**
   * Rect height.
   */
  private h: number;

  /**
   *  Constructor.
   * @param w
   * @param h
   */
  constructor(w: number, h: number) {
    this.w = w;
    this.h = h;
  }

  /**
   * Set value.
   * @param w
   * @param h
   */
  set(w: number, h: number): void {
    this.w = w;
    this.h = h;
  }

  /**
   * Get width.
   * @returns
   */
  getW(): number {
    return this.w;
  }

  /**
   * Get height.
   * @returns
   */
  getH(): number {
    return this.h;
  }

  /**
   * Get rate.
   * @returns
   */
  getRate(): number {
    return (this.w / this.h);
  }

  /**
   * Determine whether the current ratio is valid.
   * @returns
   */
  isValid(): boolean {
    return (this.w > 0 && this.h > 0);
  }
}