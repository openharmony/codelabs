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

import { XLocationEnum, YLocationEnum} from '../common/utils/CommonConstaint';
import CommonConstant from '../common/utils/CommonConstaint';

export class OffsetModel {
  private offsetXLeft: number = CommonConstant.DEFAULT_OFFSET_X_LEFT;
  private offsetXRight: number = CommonConstant.DEFAULT_OFFSET_X_RIGHT;
  private offsetYTop: number = CommonConstant.DEFAULT_OFFSET_Y_TOP;
  private offsetYBottom: number = CommonConstant.DEFAULT_OFFSET_Y_BOTTOM;
  private xLocationType: number = XLocationEnum.noChange;
  private yLocationType: number = YLocationEnum.noChange;
  private screenWidth: number = 0;
  private screenHeight: number = 0;
  private blackAreaWidth: number = 0;
  private blackAreaHeight: number = 0;

  public initOffsetModel(width: number, height: number) {
    this.screenWidth = width;
    this.screenHeight = height;
    this.blackAreaWidth = this.screenWidth * (1 - CommonConstant.X_SCALE_DOWN);
    this.blackAreaWidth = this.blackAreaWidth / CommonConstant.BLACK_AREA_NUM;
    this.blackAreaHeight = this.screenHeight * (1 - CommonConstant.Y_SCALE_DOWN);
    this.blackAreaHeight = this.blackAreaHeight / CommonConstant.BLACK_AREA_NUM;
  }

  /**
   * Reset the offset to the default value.
   */
  public resetDefaultOffSet() {
    this.offsetXLeft = CommonConstant.DEFAULT_OFFSET_X_LEFT;
    this.offsetXRight = CommonConstant.DEFAULT_OFFSET_X_RIGHT;
    this.offsetYTop = CommonConstant.DEFAULT_OFFSET_Y_TOP;
    this.offsetYBottom = CommonConstant.DEFAULT_OFFSET_Y_BOTTOM;
  }

  public getXLeft() {
    return this.offsetXLeft;
  }

  public getXRight() {
    return this.offsetXRight;
  }

  public getYTop() {
    return this.offsetYTop;
  }

  public getYBottom() {
    return this.offsetYBottom;
  }

  public getWidth() {
    return this.offsetXRight - this.offsetXLeft;
  }

  public getHeight() {
    return this.offsetYBottom - this.offsetYTop;
  }

  /**
   * Get x locationType.
   * @param offsetX
   */
  public setXLocationType(offsetX: number) {
    if (offsetX > this.offsetXRight - CommonConstant.OFFSET_RANGE &&
      offsetX < this.offsetXRight + CommonConstant.OFFSET_RANGE) {
      this.xLocationType = XLocationEnum.XRight;
    } else if (offsetX > this.offsetXLeft - CommonConstant.OFFSET_RANGE &&
      offsetX < this.offsetXLeft + CommonConstant.OFFSET_RANGE) {
      this.xLocationType = XLocationEnum.XLeft;
    } else {
      this.xLocationType = XLocationEnum.noChange;
    }
  }

  /**
   * Get y locationType.
   * @param offsetY
   */
  public setYLocationType(offsetY: number) {
    if (offsetY > this.offsetYBottom - CommonConstant.OFFSET_RANGE &&
      offsetY < this.offsetYBottom + CommonConstant.OFFSET_RANGE) {
      this.yLocationType = YLocationEnum.YBottom;
    } else if (offsetY > this.offsetYTop - CommonConstant.OFFSET_RANGE &&
      offsetY < this.offsetYTop + CommonConstant.OFFSET_RANGE) {
      this.yLocationType = YLocationEnum.YTop;
    } else {
      this.yLocationType = YLocationEnum.noChange;
    }
  }

  /**
   * Reset offset to the input value.
   * @param offsetX
   * @param offsetY
   */
  public resetOffsetXY(offsetX: number, offsetY: number) {
    if (this.xLocationType === XLocationEnum.XLeft) {
      this.offsetXLeft = this.offsetXRight - offsetX < CommonConstant.OFFSET_RANGE * 2 ?
        this.offsetXLeft : offsetX;
    }
    if (this.xLocationType === XLocationEnum.XRight) {
      this.offsetXRight = offsetX - this.offsetXLeft < CommonConstant.OFFSET_RANGE * 2 ?
        this.offsetXRight : offsetX;
    }

    if (this.yLocationType === YLocationEnum.YTop) {
      this.offsetYTop = this.offsetYBottom - offsetY < CommonConstant.OFFSET_RANGE * 2 ?
        this.offsetYTop : offsetY;
    }
    if (this.yLocationType ===  YLocationEnum.YBottom) {
      this.offsetYBottom = offsetY - this.offsetYTop < CommonConstant.OFFSET_RANGE * 2 ?
        this.offsetYBottom : offsetY;
    }

    this.checkOffsetXY();
  }

  /**
   * Check whether the offset exceeds the area.
   */
  private checkOffsetXY() {
    this.offsetXLeft = this.offsetXLeft < this.blackAreaWidth ? this.blackAreaWidth : this.offsetXLeft;
    this.offsetXRight = this.offsetXRight > this.screenWidth - this.blackAreaWidth ?
      this.screenWidth - this.blackAreaWidth : this.offsetXRight;
    this.offsetYTop = this.offsetYTop < this.blackAreaHeight ? this.blackAreaHeight : this.offsetYTop;
    this.offsetYBottom = this.offsetYBottom > this.screenHeight - this.blackAreaHeight ?
      this.screenHeight - this.blackAreaHeight : this.offsetYBottom;
  }
}

export default new OffsetModel();