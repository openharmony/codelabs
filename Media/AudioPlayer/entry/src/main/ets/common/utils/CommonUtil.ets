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

import { CommonConstants } from '../constants/CommonConstants';

/**
 * Public tool class.
 */
export class CommonUtil {
  /**
   * Convert milliseconds to component second format.
   *
   * @param ms Milliseconds.
   * @returns
   */
  public static formatDuration(ms: number): string {
    if (ms < 0) {
      ms = 0;
    }
    const secondPrecision = CommonConstants.COMMON_UTIL.SECOND_PRECISION;
    const secondDuration = ms / CommonConstants.COMMON_UTIL.SECOND_TO_MS;
    let min = Math.floor(secondDuration / CommonConstants.COMMON_UTIL.MIN_TO_SEC);
    let sec = Math.round(secondDuration - min * CommonConstants.COMMON_UTIL.MIN_TO_SEC);
    let secStr = sec.toFixed(secondPrecision);
    if (parseInt(secStr) <= CommonConstants.COMMON_UTIL.MAX_OF_INDIVIDUAL) {
      secStr = `0${secStr}`;
    }
    return `${min}:${secStr}`;
  }

  /**
   * Obtain n non-repetitive random numbers between min and max.
   *
   * @param n Number of random.
   * @returns
   */
  public static getRandomNumber(n: number) {
    // Generates an array from 0 to n-1.
    let arr: number[] = Array.from(Array(n), (v, k) => k);
    // Randomly disrupted.
    for (let i = 0; i < n; i++) {
      let j = Math.floor(Math.random() * (arr.length - i) + i);
      [arr[i], arr[j]] = [arr[j], arr[i]];
    }
    arr.length = n;
    return arr;
  }

  /**
   * Check obj is empty or not.
   *
   * @param obj Object.
   * @returns
   */
  public static isEmpty(obj) {
    return typeof obj === 'undefined' || obj == null || obj === '';
  }
}