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

import Constants from './CommonConstaint';
import offsetModel from '../../model/OffsetModel';

// Canvas drawing method class.
export class DrawUtil {
  private screenWidth: number = 0;
  private screenHeight: number = 0;
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private canvasContext: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  private offsetXLeft: number = 0;
  private offsetXRight: number = 0;
  private offsetYTop: number = 0;
  private offsetYBottom: number = 0;

  public initDrawUtil(canvasContext: CanvasRenderingContext2D, screenWidth: number, screenHeight: number) {
    this.canvasContext = canvasContext;
    this.screenWidth = screenWidth;
    this.screenHeight = screenHeight;
  }

  public draw() {
    this.offsetXLeft = offsetModel.getXLeft();
    this.offsetXRight = offsetModel.getXRight();
    this.offsetYTop = offsetModel.getYTop();
    this.offsetYBottom = offsetModel.getYBottom();

    this.drawScreenSelection();
    this.drawLines();
  }

  /**
   * Draw screen selection component.
   */
  private drawScreenSelection() {
    this.canvasContext.clearRect(0, 0, this.screenWidth, this.screenHeight);
    this.canvasContext.beginPath();

    this.canvasContext.moveTo(0, 0);
    this.canvasContext.lineTo(this.screenWidth, 0);
    this.canvasContext.lineTo(this.screenWidth, this.screenHeight);
    this.canvasContext.lineTo(0, this.screenHeight);
    this.canvasContext.closePath();

    this.canvasContext.moveTo(this.offsetXRight, this.offsetYTop);
    this.canvasContext.lineTo(this.offsetXLeft, this.offsetYTop);
    this.canvasContext.lineTo(this.offsetXLeft, this.offsetYBottom);
    this.canvasContext.lineTo(this.offsetXRight, this.offsetYBottom);

    this.canvasContext.globalAlpha = Constants.UNSELECT_AREA_ALPHA;
    this.canvasContext.fillStyle = Constants.UNSELECT_AREA_COLOR;
    this.canvasContext.closePath();
    this.canvasContext.fill();
  }

  /**
   * Draw box selection lines.
   */
  private drawLines() {
    this.canvasContext.beginPath();
    this.canvasContext.lineWidth = Constants.LINES_WIDTH;
    this.canvasContext.strokeStyle = Constants.LINES_COLOR;
    this.canvasContext.globalAlpha = Constants.LINES_ALPHA;
    this.canvasContext.lineJoin = Constants.LINES_STYLE;
    this.canvasContext.moveTo(
      (this.offsetXLeft + Constants.LINES_MAX_LENGTH),
      (this.offsetYTop - Constants.GAP_WIDTH)
    );
    this.canvasContext.lineTo(
      (this.offsetXLeft - Constants.GAP_WIDTH),
      (this.offsetYTop - Constants.GAP_WIDTH)
    );
    this.canvasContext.lineTo(
      (this.offsetXLeft - Constants.GAP_WIDTH),
      (this.offsetYTop + Constants.LINES_MAX_LENGTH)
    );

    this.canvasContext.moveTo(
      (this.offsetXRight - Constants.LINES_MAX_LENGTH),
      (this.offsetYTop - Constants.GAP_WIDTH)
    );
    this.canvasContext.lineTo(
      (this.offsetXRight + Constants.GAP_WIDTH),
      (this.offsetYTop - Constants.GAP_WIDTH)
    );
    this.canvasContext.lineTo(
      (this.offsetXRight + Constants.GAP_WIDTH),
      (this.offsetYTop + Constants.LINES_MAX_LENGTH)
    );

    this.canvasContext.moveTo(
      (this.offsetXLeft + Constants.LINES_MAX_LENGTH),
      (this.offsetYBottom + Constants.GAP_WIDTH)
    );
    this.canvasContext.lineTo(
      (this.offsetXLeft - Constants.GAP_WIDTH),
      (this.offsetYBottom + Constants.GAP_WIDTH)
    );
    this.canvasContext.lineTo(
      (this.offsetXLeft - Constants.GAP_WIDTH),
      (this.offsetYBottom - Constants.LINES_MAX_LENGTH)
    );

    this.canvasContext.moveTo(
      (this.offsetXRight - Constants.LINES_MAX_LENGTH),
      (this.offsetYBottom + Constants.GAP_WIDTH)
    );
    this.canvasContext.lineTo(
      (this.offsetXRight + Constants.GAP_WIDTH),
      (this.offsetYBottom + Constants.GAP_WIDTH)
    );
    this.canvasContext.lineTo(
      (this.offsetXRight + Constants.GAP_WIDTH),
      (this.offsetYBottom - Constants.LINES_MAX_LENGTH)
    );

    this.canvasContext.stroke();
  }
}

export default new DrawUtil();