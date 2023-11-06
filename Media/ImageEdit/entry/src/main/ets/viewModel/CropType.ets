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

export enum CropTouchState {
  /**
   * Crop state.
   */
  NONE,

  /**
   * Move cropping frame.
   */
  CROP_MOVE,

  /**
   * Move image.
   */
  IMAGE_DRAG,

  /**
   * Scale image.
   */
  IMAGE_SCALE
}

export enum CropRatioType {
  /**
   * Free choice.
   */
  RATIO_TYPE_FREE,

  /**
   * Horizontal frame.
   */
  RATIO_TYPE_HORIZONTAL,

  /**
   * Vertical frame.
   */
  RATIO_TYPE_VERTICAL,

  /**
   * 1:1 frame.
   */
  RATIO_TYPE_1_1,

  /**
   * 16:9 frame.
   */
  RATIO_TYPE_16_9,

  /**
   * 9:16 frame.
   */
  RATIO_TYPE_9_16,

  /**
   * 4:3 frame.
   */
  RATIO_TYPE_4_3,

  /**
   * 3:4 frame.
   */
  RATIO_TYPE_3_4,

  /**
   * 3:2 frame.
   */
  RATIO_TYPE_3_2,

  /**
   * 2:3 frame.
   */
  RATIO_TYPE_2_3
}

export enum CropAngle {
  /**
   * Crop angle 90.
   */
  ONE_QUARTER_CIRCLE_ANGLE = 90,

  /**
   * Crop angle 180.
   */
  HALF_CIRCLE_ANGLE = 180,

  /**
   * Crop angle 270.
   */
  THREE_QUARTER_CIRCLE_ANGLE = 270,

  /**
   * Crop angle 360.
   */
  CIRCLE_ANGLE = 360
}