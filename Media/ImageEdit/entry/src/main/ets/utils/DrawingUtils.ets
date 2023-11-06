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

import { RectF } from '../viewmodel/Rect';
import { MathUtils } from './MathUtils';
import { CommonConstants } from '../common/constant/CommonConstants';

export abstract class DrawingUtils {
  /**
   * Draw mask.
   * @param ctx
   * @param outer
   * @param inner
   * @param isShow
   */
  static drawMask(ctx: CanvasRenderingContext2D, outer: RectF, inner: RectF, isShow: boolean) {
    // ctx.fillStyle = DrawingUtils.DEFAULT_MASK_STYLE;
    if (isShow) {
      ctx.fillStyle = CommonConstants.DEFAULT_MASK_STYLE;
    } else {
      ctx.fillStyle = `rgba(0, 0, 0, 1)`;
    }
    ctx.fillRect(outer.left, outer.top, outer.getWidth(), inner.top - outer.top);
    ctx.fillRect(outer.left, inner.top, inner.left - outer.left, inner.getHeight());
    ctx.fillRect(inner.right, inner.top, outer.right - inner.right, inner.getHeight());
    ctx.fillRect(outer.left, inner.bottom, outer.getWidth(), outer.bottom - inner.bottom);
  }

  /**
   * Draw crop thick line.
   * @param ctx
   * @param crop
   */
  static drawCropButton(ctx: CanvasRenderingContext2D, crop: RectF) {
    let vp3 = CommonConstants.DEFAULT_BUTTON_WIDTH;
    let padding = CommonConstants.DEFAULT_BUTTON_PADDING;
    ctx.lineWidth = vp3;
    ctx.strokeStyle = CommonConstants.DEFAULT_BUTTON_COLOR;
    let cornerLength = CommonConstants.DEFAULT_BUTTON_LENGTH;
    DrawingUtils.drawCornerButton(ctx, crop, vp3, padding, cornerLength);
    DrawingUtils.drawLineButton(ctx, crop, vp3, padding, cornerLength);
  }

  /**
   * Draw rect.
   * @param ctx
   * @param crop
   */
  static drawRect(ctx: CanvasRenderingContext2D, crop: RectF) {
    // ctx.lineWidth = DrawingUtils.DEFAULT_LINE_WIDTH;
    ctx.lineWidth = CommonConstants.DEFAULT_LINE_RECT_WIDTH;
    ctx.strokeStyle = CommonConstants.DEFAULT_RECT_LINE_COLOR;
    let points = MathUtils.rectToPoints(crop);
    for (let i = 0; i < points.length; i++) {
      let j = (i + 1) % points.length;
      DrawingUtils.drawLine(ctx, points[i].x, points[i].y, points[j].x, points[j].y);
    }
  }

  /**
   * Draw split line.
   * @param ctx
   * @param crop
   * @param split
   * @param isShow
   */
  static drawSplitLine(ctx: CanvasRenderingContext2D, crop: RectF, split: number, isShow: boolean) {
    if (!isShow) {
      return;
    }
    ctx.lineWidth = CommonConstants.DEFAULT_LINE_WIDTH;
    ctx.strokeStyle = CommonConstants.DEFAULT_LINE_COLOR;
    let w = Math.ceil(crop.getWidth() / split);
    let h = Math.ceil(crop.getHeight() / split);
    for (let i = 1; i < split; i++) {
      let x = crop.left + w * i;
      let y = crop.top + h * i;
      DrawingUtils.drawLine(ctx, x, crop.top, x, crop.bottom);
      DrawingUtils.drawLine(ctx, crop.left, y, crop.right, y);
    }
  }

  /**
   * Draw line.
   * @param ctx
   * @param srcX
   * @param srcY
   * @param dstX
   * @param dstY
   */
  static drawLine(ctx: CanvasRenderingContext2D, srcX: number, srcY: number, dstX: number, dstY: number) {
    ctx.beginPath();
    ctx.moveTo(srcX, srcY);
    ctx.lineTo(dstX, dstY);
    ctx.stroke();
  }

  /**
   * Draw corner line.
   * @param ctx
   * @param crop
   * @param vp3
   * @param padding
   * @param cornerLength
   */
  private static drawCornerButton(ctx: CanvasRenderingContext2D, crop: RectF,
                                  vp3: number, padding: number, cornerLength: number) {
    // left top conner button
    vp3 = vp3 / 2;
    let startX = crop.left - vp3 - padding;
    let startY = crop.top - vp3;
    let stopX = startX + cornerLength;
    let stopY = startY;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);
    startX = crop.left - vp3;
    startY = crop.top - vp3 - padding;
    stopX = startX;
    stopY = startY + cornerLength;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);

    // right top conner button
    startX = crop.right + vp3 + padding;
    startY = crop.top - vp3;
    stopX = startX - cornerLength;
    stopY = startY;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);
    startX = crop.right + vp3;
    startY = crop.top - vp3 - padding;
    stopX = startX;
    stopY = startY + cornerLength;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);

    // left bottom conner button
    startX = crop.left - vp3;
    startY = crop.bottom + vp3 + padding;
    stopX = startX;
    stopY = startY - cornerLength;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);
    startX = crop.left - vp3 - padding;
    startY = crop.bottom + vp3;
    stopX = startX + cornerLength;
    stopY = startY;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);

    // right bottom conner button
    startX = crop.right + vp3 + padding;
    startY = crop.bottom + vp3;
    stopX = startX - cornerLength;
    stopY = startY;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);
    startX = crop.right + vp3;
    startY = crop.bottom + vp3 + padding;
    stopX = startX;
    stopY = startY - cornerLength;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);
  }

  /**
   * Draw line.
   * @param ctx
   * @param crop
   * @param vp3
   * @param padding
   * @param cornerLength
   */
  private static drawLineButton(ctx: CanvasRenderingContext2D, crop: RectF,
                                vp3: number, padding: number, cornerLength: number) {
    vp3 = vp3 / 2;
    // top button
    let startX = crop.getCenterX() - cornerLength / 2;
    let startY = crop.top - vp3;
    let stopX = startX + cornerLength;
    let stopY = startY;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);

    // bottom button
    startX = crop.getCenterX() - cornerLength / 2;
    startY = crop.bottom + vp3;
    stopX = startX + cornerLength;
    stopY = startY;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);

    // left button
    startX = crop.left - vp3;
    startY = crop.getCenterY() - cornerLength / 2;
    stopX = startX;
    stopY = startY + cornerLength;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);

    // right button
    startX = crop.right + vp3;
    startY = crop.getCenterY() - cornerLength / 2;
    stopX = startX;
    stopY = startY + cornerLength;
    DrawingUtils.drawLine(ctx, startX, startY, stopX, stopY);
  }
}