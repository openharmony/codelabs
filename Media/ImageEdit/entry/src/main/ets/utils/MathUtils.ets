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

import { LineSegment } from '../common/bean/Line';
import { Point } from '../common/bean/Point';
import { RectF } from '../common/bean/Rect';
import { CropAngle } from '../common/bean/CropType';
import { CommonConstants } from '../common/constant/CommonConstants';

export abstract class MathUtils {
  /**
   * Round out rect.
   * @param rect
   */
  static roundOutRect(rect: RectF): void {
    let copy = { ...rect };
    rect.set(Math.round(copy.left), Math.round(copy.top), Math.round(copy.right), Math.round(copy.bottom));
  }

  /**
   * Normalized rectangle.
   * @param rect
   * @param width
   * @param height
   */
  static normalizeRect(rect: RectF, width: number, height: number): void {
    rect.left /= width;
    rect.right /= width;
    rect.top /= height;
    rect.bottom /= height;
  }

  /**
   * Restore rectangle.
   * @param rect
   * @param width
   * @param height
   */
  static revertRect(rect: RectF, width: number, height: number): void {
    rect.left *= width;
    rect.right *= width;
    rect.top *= height;
    rect.bottom *= height;
  }

  /**
   * Get rectangle endpoints.
   * @param rect
   * @returns
   */
  static rectToPoints(rect: RectF): Array<Point> {
    let points = [];
    points.push(new Point(rect.left, rect.top));
    points.push(new Point(rect.right, rect.top));
    points.push(new Point(rect.right, rect.bottom));
    points.push(new Point(rect.left, rect.bottom));
    return points;
  }

  /**
   * Convert width and height.
   * @param rect
   */
  static swapWidthHeight(rect: RectF): void {
    let centerX = rect.getCenterX();
    let centerY = rect.getCenterY();
    let halfWidth = rect.getWidth() / 2;
    let halfHeight = rect.getHeight() / 2;
    rect.left = centerX - halfHeight;
    rect.right = centerX + halfHeight;
    rect.top = centerY - halfWidth;
    rect.bottom = centerY + halfWidth;
  }

  /**
   * Rotate by point.
   * @param inputs
   * @param angle
   * @param origin
   * @returns
   */
  static rotatePoints(inputs: Array<Point>, angle: number, origin: Point): Array<Point> {
    let alpha = MathUtils.formulaAngle(-angle);
    let outputs = [];
    for (let input of inputs) {
      let dx = input.x - origin.x;
      let dy = input.y - origin.y;
      let output = new Point(origin.x, origin.y);
      output.x += Math.cos(alpha) * dx + Math.sin(alpha) * dy;
      output.y += -Math.sin(alpha) * dx + Math.cos(alpha) * dy;
      outputs.push(output);
    }
    return outputs;
  }

  /**
   * Calculate the largest marquee.
   * @param rect
   * @param limit
   * @param rate
   */
  static computeMaxRectWithinLimit(rect: RectF, limit: RectF, rate: number): void {
    let limitWidth = limit.getWidth();
    let limitHeight = limit.getHeight();
    let width = limitWidth;
    let height = limitHeight;
    if (rate > (limitWidth / limitHeight)) {
      height = width / rate;
      rect.left = limit.left;
      rect.top = limit.top + (limitHeight - height) / 2;
    } else {
      width = height * rate;
      rect.left = limit.left + (limitWidth - width) / 2;
      rect.top = limit.top;
    }
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;
  }

  /**
   * Scale rectangle by points.
   * @param rect
   * @param p
   * @param scale
   */
  static scaleRectBasedOnPoint(rect: RectF, p: Point, scale: number): void {
    let operate = { ...rect };
    operate.left = (rect.left - p.x) * scale + p.x;
    operate.right = (rect.right - p.x) * scale + p.x;
    operate.top = (rect.top - p.y) * scale + p.y;
    operate.bottom = (rect.bottom - p.y) * scale + p.y;
    rect.set(operate.left, operate.top, operate.right, operate.bottom);
  }

  /**
   * Determine if two lines intersect.
   * @param line1
   * @param line2
   * @returns true means intersect, false means not intersect.
   */
  static hasIntersection(line1: LineSegment, line2: LineSegment): boolean {
    let p1 = line1.start;
    let p2 = line1.end;
    let p3 = line2.start;
    let p4 = line2.end;
    if (Math.max(p1.x, p2.x) < Math.min(p3.x, p4.x) || Math.max(p1.y, p2.y) < Math.min(p3.y, p4.y)
    || Math.max(p3.x, p4.x) < Math.min(p1.x, p2.x) || Math.max(p3.y, p4.y) < Math.min(p1.y, p2.y)) {
      return false;
    }

    if ((((p1.x - p3.x) * (p4.y - p3.y) - (p1.y - p3.y) * (p4.x - p3.x))
    * ((p2.x - p3.x) * (p4.y - p3.y) - (p2.y - p3.y) * (p4.x - p3.x))) >= 0
    || (((p3.x - p1.x) * (p2.y - p1.y) - (p3.y - p1.y) * (p2.x - p1.x))
    * ((p4.x - p1.x) * (p2.y - p1.y) - (p4.y - p1.y) * (p2.x - p1.x))) >= 0) {
      return false;
    }
    return true;
  }

  /**
   * Get the intersection point of two lines.
   * @param line1
   * @param line2
   * @returns Intersection coordinates.
   */
  static getIntersection(line1: LineSegment, line2: LineSegment): Point {
    let yStartSpacing1 = line1.start.y - line1.end.y;
    let yEndSpacing1 = line1.end.x - line1.start.x;
    let resultLine1 = yStartSpacing1 * line1.start.x + yEndSpacing1 * line1.start.y;

    let yStartSpacing2 = line2.start.y - line2.end.y;
    let yEndSpacing2 = line2.end.x - line2.start.x;
    let resultLine2 = yStartSpacing2 * line2.start.x + yEndSpacing2 * line2.start.y;

    let kRate = yStartSpacing1 * yEndSpacing2 - yStartSpacing2 * yEndSpacing1;
    if (Math.abs(kRate) < CommonConstants.EQUALITY_THRESHOLD) {
      return undefined;
    }

    let intersectionX1 = yEndSpacing2 / kRate;
    let intersectionX2 = -yEndSpacing1 / kRate;
    let intersectionY1 = -yStartSpacing2 / kRate;
    let intersectionY2 = yStartSpacing1 / kRate;

    let xIntersection = intersectionX1 * resultLine1 + intersectionX2 * resultLine2;
    let yIntersection = intersectionY1 * resultLine1 + intersectionY2 * resultLine2;
    return new Point(xIntersection, yIntersection);
  }

  /**
   * Find suitable scale.
   * @param points
   * @param rect
   * @param origin
   * @returns scale.
   */
  static findSuitableScale(points: Array<Point>, rect: RectF, origin: Point): number {
    let scale = 1;
    let temp = 1;
    for (let point of points) {
      if (point.x < rect.left) {
        temp = (origin.x - point.x) / (origin.x - rect.left);
        scale = Math.max(scale, temp);
      }
      if (point.x > rect.right) {
        temp = (point.x - origin.x) / (rect.right - origin.x);
        scale = Math.max(scale, temp);
      }
      if (point.y < rect.top) {
        temp = (origin.y - point.y) / (origin.y - rect.top);
        scale = Math.max(scale, temp);
      }
      if (point.y > rect.bottom) {
        temp = (point.y - origin.y) / (rect.bottom - origin.y);
        scale = Math.max(scale, temp);
      }
    }
    return scale;
  }

  /**
   * Image move fix.
   * @param rotated
   * @param flipImage
   * @returns offset.
   */
  static fixImageMove(rotated: Array<Point>, flipImage: RectF): Array<number> {
    let offsetX = 0;
    let offsetY = 0;
    for (let point of rotated) {
      if (point.x < flipImage.left) {
        offsetX = Math.min(offsetX, point.x - flipImage.left);
      } else if (point.x > flipImage.right) {
        offsetX = Math.max(offsetX, point.x - flipImage.right);
      }
      if (point.y < flipImage.top) {
        offsetY = Math.min(offsetY, point.y - flipImage.top);
      } else if (point.y > flipImage.bottom) {
        offsetY = Math.max(offsetY, point.y - flipImage.bottom);
      }
    }
    return [offsetX, offsetY];
  }

  /**
   * Confirm whether it is an odd rotation.
   * @param angle
   * @returns true means is odd, false means is not odd.
   */
  static isOddRotation(angle: number): boolean {
    if (angle == -CropAngle.ONE_QUARTER_CIRCLE_ANGLE || angle == -CropAngle.THREE_QUARTER_CIRCLE_ANGLE) {
      return true;
    }
    return false;
  }

  /**
   * If line intersect, limit.
   * @param outerLine
   * @param diagonal
   * @param rect
   */
  static limitCornerIfLineIntersect(outerLine: LineSegment, diagonal: LineSegment, rect: RectF): void {
    let origin = new Point(rect.getCenterX(), rect.getCenterY());
    if (MathUtils.hasIntersection(outerLine, diagonal)) {
      let intersection = MathUtils.getIntersection(outerLine, diagonal);
      if (intersection == undefined) {
        return;
      }
      if (intersection.x < origin.x) {
        rect.left = intersection.x;
      } else {
        rect.right = intersection.x;
      }
      if (intersection.y < origin.y) {
        rect.top = intersection.y;
      } else {
        rect.bottom = intersection.y;
      }
    }
  }

  /**
   * Rotating, limit in rect.
   * @param rect
   * @param outerLines
   */
  static limitRectInRotated(rect: RectF, outerLines: Array<LineSegment>): void {
    let copy = new RectF();
    copy.set(rect.left, rect.top, rect.right, rect.bottom);
    let diagonal1 = new LineSegment(new Point(copy.left, copy.top), new Point(copy.right, copy.bottom));
    for (let line of outerLines) {
      MathUtils.limitCornerIfLineIntersect(line, diagonal1, copy);
    }

    let diagonal2 = new LineSegment(new Point(copy.left, copy.bottom), new Point(copy.right, copy.top));
    for (let line of outerLines) {
      MathUtils.limitCornerIfLineIntersect(line, diagonal2, copy);
    }
    rect.set(copy.left, copy.top, copy.right, copy.bottom);
  }

  /**
   * Based on point rotating, limit rect.
   * @param baseIndex
   * @param rect
   * @param rotatedLines
   */
  static limitRectInRotatedBasedOnPoint(baseIndex: number, rect: RectF, rotatedLines: Array<LineSegment>): void {
    let points = MathUtils.rectToPoints(rect);
    let base = points[baseIndex];
    points.splice(baseIndex, 1);
    let scale = 1;
    for (let point of points) {
      let line = new LineSegment(base, point);
      for (let rotatedLine of rotatedLines) {
        if (MathUtils.hasIntersection(line, rotatedLine)) {
          let p = MathUtils.getIntersection(line, rotatedLine);
          if (p == undefined) {
            continue;
          }
          let tempScale
            = Math.hypot(p.x - base.x, p.y - base.y) / Math.hypot(point.x - base.x, point.y - base.y);
          scale = Math.min(scale, (tempScale > CommonConstants.EQUALITY_THRESHOLD) ? tempScale : 1);
        }
      }
    }
    MathUtils.scaleRectBasedOnPoint(rect, base, scale);
  }

  /**
   * Get max fixed rect size.
   * @param rate
   * @param maxWidth
   * @param maxHeight
   * @returns rect width and height.
   */
  static getMaxFixedRectSize(rate: number, maxWidth: number, maxHeight: number): Array<number> {
    let width = 0;
    let height = 0;
    if (rate > (maxWidth / maxHeight)) {
      width = maxWidth;
      height = width / rate;
    } else {
      height = maxHeight;
      width = height * rate;
    }
    return [width, height];
  }

  /**
   * Get min fixed rect size.
   * @param rate
   * @param minLength
   * @returns rect width and height.
   */
  static getMinFixedRectSize(rate: number, minLength: number): Array<number> {
    let width = minLength;
    let height = minLength;
    if (rate > 1) {
      width = height * rate;
    } else {
      height = width / rate;
    }
    return [width, height];
  }

  /**
   * Determine whether it is the same rectangle.
   * @param rect1
   * @param rect2
   * @returns true means same, false means not same.
   */
  static areRectsSame(rect1: RectF, rect2: RectF): boolean {
    if (rect1.left == rect2.left && rect1.top == rect2.top
    && rect1.right == rect2.right && rect1.bottom == rect2.bottom) {
      return true;
    }
    return false;
  }

  /**
   * Calculate radian value.
   * @param angle
   * @returns
   */
  static formulaAngle(angle: number): number {
    return angle * Math.PI / CropAngle.HALF_CIRCLE_ANGLE;
  }
}