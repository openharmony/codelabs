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

import { Point } from './Point';
import { RectF } from './Rect';
import { CropAngle, CropRatioType, CropTouchState } from './CropType';
import { ImageFilterCrop } from './ImageFilterCrop';
import { CropShow } from './CropShow';
import { MathUtils } from '../utils/MathUtils';
import { DrawingUtils } from '../utils/DrawingUtils';
import { PixelMapWrapper } from './PixelMapWrapper';
import Logger from '../utils/LoggerUtil';
import { CommonConstants } from '../common/constant/CommonConstants';

const TAG: string = 'editor_ImageEditCrop';

export class ImageEditCrop {
  private filter: ImageFilterCrop | undefined = undefined;
  private input: PixelMapWrapper | undefined = undefined;
  private isFlipHorizontal: boolean = false;
  private isFlipVertically: boolean = false;
  private rotationAngle: number = 0;
  private sliderAngle: number = 0;
  private cropRatio: CropRatioType = CropRatioType.RATIO_TYPE_FREE;
  private cropShow: CropShow;
  private isCropShowInitialized: boolean = false;
  private ctx: CanvasRenderingContext2D | undefined = undefined;
  private displayWidth: number = 0;
  private displayHeight: number = 0;
  private marginW: number = CommonConstants.DEFAULT_MARGIN_LENGTH;
  private marginH: number = CommonConstants.DEFAULT_MARGIN_LENGTH;
  private imageRatio: number = CommonConstants.DEFAULT_IMAGE_RATIO;
  private scale: number = CommonConstants.BASE_SCALE_VALUE;
  private timeoutId: number = 0;
  private timeout: number = CommonConstants.DEFAULT_TIMEOUT_MILLISECOND_1000;
  private isWaitingRefresh: boolean = false;
  private touchPoint: Point;
  private pinchPoint: Point;
  private state: CropTouchState = CropTouchState.NONE;
  private splitFraction: number = CommonConstants.DEFAULT_SPLIT_FRACTION;
  private canvasReady: boolean = false;
  private splitLineShow: boolean = false;

  constructor() {
    this.cropShow = new CropShow();
    this.touchPoint = new Point(0, 0);
    this.pinchPoint = new Point(0, 0);
  }

  /**
   * Image edit model init.
   * @param pixelMap
   */
  entry(pixelMap: PixelMapWrapper): void {
    Logger.info(TAG, `entry pixelMap: ${JSON.stringify(pixelMap)}`);
    this.input = pixelMap;
    this.filter = new ImageFilterCrop();
    this.initialize();
    if (this.isCropShowInitialized) {
      let limit = this.calcNewLimit();
      this.cropShow.init(limit, this.imageRatio);
    }
    this.refresh();
  }

  /**
   * Image edit model exit.
   * @returns
   */
  exit(): ImageFilterCrop | undefined {
    Logger.info(TAG, 'exit');
    this.saveFinalOperation();
    this.isCropShowInitialized = false;
    this.input = undefined;
    if (this.couldReset()) {
      this.clear();
    } else {
      this.filter = undefined;
    }
    return this.filter;
  }

  /**
   * Set canvas init.
   * @param context
   */
  setCanvasContext(context: CanvasRenderingContext2D): void {
    Logger.info(TAG, 'setCanvasContext');
    this.ctx = context;
    this.refresh();
  }

  /**
   * Set Canvas ready.
   * @param state
   */
  setCanvasReady(state: boolean): void {
    this.canvasReady = state;
  }

  /**
   * Set Canvas display state.
   * @param width
   * @param height
   */
  setCanvasSize(width: number, height: number): void {
    Logger.info(TAG, `setCanvasSize: width[${width}], height[${height}]`);
    this.displayWidth = width;
    this.displayHeight = height;
    let limit = this.calcNewLimit();
    if (this.isCropShowInitialized) {
      this.cropShow.syncLimitRect(limit);
      this.determineMaxScaleFactor();
    } else {
      this.cropShow.init(limit, this.imageRatio);
      this.isCropShowInitialized = true;
    }
    this.refresh();
  }

  /**
   * Clear canvas content.
   */
  clearCanvas(): void {
    if (this.ctx !== undefined) {
      this.ctx.clearRect(0, 0, this.displayWidth, this.displayHeight);
    }
  }

  /**
   * Rotate and flip.
   */
  onMirrorChange(): void {
    Logger.debug(TAG, 'onMirrorChange');
    if (this.isWaitingRefresh) {
      this.clearDelayRefresh();
      this.cropShow.enlargeCropArea();
    } else {
      if (MathUtils.isOddRotation(this.rotationAngle)) {
        this.isFlipVertically = !this.isFlipVertically;
      } else {
        this.isFlipHorizontal = !this.isFlipHorizontal;
      }
      this.cropShow.setFlip(this.isFlipHorizontal, this.isFlipVertically);
    }
    this.refresh();
  }

  /**
   * Determine whether to rotate.
   */
  onRotationAngleChange(): void {
    Logger.debug(TAG, 'onRotationAngleChange');
    if (this.isWaitingRefresh) {
      this.clearDelayRefresh();
      this.cropShow.enlargeCropArea();
    } else {
      this.rotationAngle = (this.rotationAngle - CropAngle.ONE_QUARTER_CIRCLE_ANGLE) % CropAngle.CIRCLE_ANGLE;
      this.cropShow.syncRotationAngle(this.rotationAngle);
    }
    this.refresh();
  }

  /***
   * Change rotate angle.
   */
  onSliderAngleChange(angle: number): void {
    Logger.debug(TAG, `onSliderAngleChange: angle[${angle}]`);
    if (this.isWaitingRefresh) {
      this.clearDelayRefresh();
      this.cropShow.enlargeCropArea();
      this.refresh();
    }
    this.sliderAngle = angle;
    this.cropShow.syncHorizontalAngle(this.sliderAngle);
    this.refresh();
  }

  /**
   * Fix ratio canvas refresh.
   * @param ratio
   */
  onFixedRatioChange(ratio: CropRatioType): void {
    Logger.debug(TAG, `onFixedRatioChange: ratio[${ratio}]`);
    if (this.isWaitingRefresh) {
      this.clearDelayRefresh();
      this.cropShow.enlargeCropArea();
    }
    this.cropRatio = ratio;
    this.cropShow.setRatio(ratio);
    this.endImageDrag();
    this.refresh();
  }

  /**
   * Touch start.
   * @param x
   * @param y
   */
  onTouchStart(x: number, y: number): void {
    if (this.state !== CropTouchState.NONE) {
      Logger.debug(TAG, 'onTouchStart: touch state is not none!');
      return;
    }

    if (this.isWaitingRefresh) {
      this.clearDelayRefresh();
    }

    Logger.debug(TAG, `onTouchStart: [x: ${x}, y: ${y}]`);
    if (this.cropShow.isCropRectTouch(x, y)) {
      this.state = CropTouchState.CROP_MOVE;
    } else {
      this.state = CropTouchState.IMAGE_DRAG;
    }
    this.splitLineShow = true;
    this.touchPoint.set(x, y);
  }

  /**
   * Touch move.
   * @param x
   * @param y
   */
  onTouchMove(x: number, y: number): void {
    Logger.debug(TAG, `onTouchMove: [state: ${this.state}] [x: ${x}, y: ${y}]`);
    let offsetX = x - this.touchPoint.x;
    let offsetY = y - this.touchPoint.y;
    if (this.state === CropTouchState.CROP_MOVE) {
      this.cropShow.moveCropRect(offsetX, offsetY);
    } else if (this.state === CropTouchState.IMAGE_DRAG) {
      this.onImageDrag(offsetX, offsetY);
    } else {
      return;
    }
    this.refresh();
    this.touchPoint.set(x, y);
  }

  /**
   * Touch end.
   */
  onTouchEnd(): void {
    Logger.debug(TAG, `onTouchEnd: [state: ${this.state}]`);
    if (this.state === CropTouchState.CROP_MOVE) {
      this.cropShow.endCropRectMove();
    } else if (this.state === CropTouchState.IMAGE_DRAG) {
      this.endImageDrag();
      this.refresh();
    } else {
      return;
    }
    this.state = CropTouchState.NONE;
    if (this.isWaitingRefresh) {
      this.clearDelayRefresh();
    }
    this.delayRefresh(this.timeout);
  }

  /**
   * Pinch start.
   * @param x
   * @param y
   * @param scale
   */
  onPinchStart(x: number, y: number, scale: number): void {
    Logger.debug(TAG, `onPinchStart: event[x: ${x}, y: ${y}]`);
    this.splitLineShow = true;
    this.state = CropTouchState.IMAGE_SCALE;
    this.pinchPoint.set(x, y);
    this.scale = scale;
  }

  /**
   * Pinch update.
   * @param scale
   */
  onPinchUpdate(scale: number): void {
    Logger.debug(TAG, `onPinchUpdate: scale[${scale}]`);
    if (this.state === CropTouchState.IMAGE_SCALE) {
      let factor = scale / this.scale;
      if (!this.cropShow.couldEnlargeImage()) {
        factor = factor > CommonConstants.BASE_SCALE_VALUE ? CommonConstants.BASE_SCALE_VALUE : factor;
      }
      let image = this.cropShow.getImageRect();
      MathUtils.scaleRectBasedOnPoint(image, this.pinchPoint, factor);
      this.cropShow.setImageRect(image);
      this.refresh();
      this.scale *= factor;
    }
  }

  /**
   * Pinch end.
   */
  onPinchEnd(): void {
    Logger.debug(TAG, 'onPinchEnd');
    let crop = this.cropShow.getCropRect();
    let points = MathUtils.rectToPoints(crop);
    let tX = this.isFlipHorizontal ? -1 : 1;
    let tY = this.isFlipVertically ? -1 : 1;
    let angle = -(this.rotationAngle * tX * tY + this.sliderAngle);
    let displayCenter = new Point(this.displayWidth / 2, this.displayHeight / 2);
    let rotated = MathUtils.rotatePoints(points, angle, displayCenter);

    let flipImage = this.cropShow.getCurrentFlipImage();
    let origin = new Point(crop.getCenterX(), crop.getCenterY());
    let centerOffsetX = origin.x - flipImage.getCenterX();
    let centerOffsetY = origin.y - flipImage.getCenterY();
    flipImage.move(centerOffsetX, centerOffsetY);
    let scale = MathUtils.findSuitableScale(rotated, flipImage, origin);
    flipImage.move(-centerOffsetX, -centerOffsetY);

    MathUtils.scaleRectBasedOnPoint(flipImage, origin, scale);
    let offsets = MathUtils.fixImageMove(rotated, flipImage);

    let image = this.cropShow.getImageRect();
    MathUtils.scaleRectBasedOnPoint(image, origin, scale);
    image.move(offsets[0] * tX, offsets[1] * tY);
    this.cropShow.setImageRect(image);
    this.refresh();
    this.state = CropTouchState.NONE;
    this.delayRefresh(this.timeout);
    this.scale = CommonConstants.BASE_SCALE_VALUE;
  }

  /**
   * Current image whether can reset.
   * @returns true means can reset, false means not.
   */
  couldReset(): boolean {
    let crop = this.cropShow.getCropRect();
    MathUtils.roundOutRect(crop);
    let image = this.cropShow.getImageRect();
    MathUtils.roundOutRect(image);
    if (
      this.isFlipHorizontal !== false ||
        this.isFlipVertically !== false ||
        this.rotationAngle !== 0 || this.sliderAngle !== 0 ||
        this.cropRatio !== CropRatioType.RATIO_TYPE_FREE ||
        !MathUtils.areRectsSame(crop, image)
    ) {
      return true;
    }
    return false;
  }

  /**
   * Reset.
   */
  reset(): void {
    Logger.debug(TAG, 'reset');
    let limit = this.calcNewLimit();
    this.cropShow.init(limit, this.imageRatio);
    this.initialize();
    this.refresh();
  }

  /**
   * Init and clear old content.
   */
  private initialize(): void {
    if (this.input !== undefined) {
      this.imageRatio = this.input.width / this.input.height;
    }
    this.determineMaxScaleFactor();
    this.clear();
  }

  /**
   * When image changing, calc limit size again.
   * @returns
   */
  private calcNewLimit(): RectF {
    let limit = new RectF();
    limit.set(this.marginW, this.marginH, this.displayWidth - this.marginW, this.displayHeight - this.marginH);
    return limit;
  }

  /**
   * Calc max scale factor.
   */
  private determineMaxScaleFactor(): void {
    if (this.input == undefined) {
      return;
    }
    let scaleFactorW = this.input.width / px2vp(CommonConstants.DEFAULT_MIN_SIDE_LENGTH_EDIT);
    let scaleFactorH = this.input.height / px2vp(CommonConstants.DEFAULT_MIN_SIDE_LENGTH_EDIT);
    this.cropShow.setMaxScaleFactor(scaleFactorW, scaleFactorH);
  }

  /**
   * Save final operation.
   */
  private saveFinalOperation(): void {
    let crop = this.cropShow.getCropRect();
    let image = this.cropShow.getImageRect();
    crop.move(-image.left, -image.top);
    MathUtils.normalizeRect(crop, image.getWidth(), image.getHeight());
    if (this.filter !== undefined) {
      this.filter.setCropRect(crop);
    }
  }

  /**
   * Set initial state.
   */
  private clear(): void {
    this.cropRatio = CropRatioType.RATIO_TYPE_FREE;
    this.isFlipHorizontal = false;
    this.isFlipVertically = false;
    this.rotationAngle = 0;
    this.sliderAngle = 0;
  }

  /**
   * Refresh.
   */
  private refresh(): void {
    if (this.ctx !== undefined && this.input !== undefined && this.canvasReady) {
      this.drawImage();
      this.drawCrop();
    }
  }

  /**
   * Delay refresh.
   * @param delay
   */
  private delayRefresh(delay: number): void {
    this.isWaitingRefresh = true;
    this.timeoutId = setTimeout(() => {
      this.cropShow.enlargeCropArea();
      this.splitLineShow = false;
      this.refresh();
      this.isWaitingRefresh = false;
    }, delay);
  }

  /**
   * Clear delay refresh.
   */
  private clearDelayRefresh(): void {
    clearTimeout(this.timeoutId);
    this.isWaitingRefresh = false;
  }

  /**
   * Draw image.
   */
  private drawImage(): void {
    if (this.ctx !== undefined) {
      this.ctx.save();
      this.clearCanvas();

      let x = this.displayWidth / 2;
      let y = this.displayHeight / 2;
      this.ctx.translate(this.isFlipHorizontal ? 2 * x : 0, this.isFlipVertically ? 2 * y : 0);

      let tX = this.isFlipHorizontal ? -1 : 1;
      let tY = this.isFlipVertically ? -1 : 1;
      this.ctx.scale(tX, tY);

      this.ctx.translate(x, y);
      this.ctx.rotate(MathUtils.formulaAngle(this.rotationAngle * tX * tY + this.sliderAngle));
      this.ctx.translate(-x, -y);

      let image = this.cropShow.getImageRect();
      MathUtils.roundOutRect(image);
      if (this.input !== undefined) {
        this.ctx.drawImage(this.input.pixelMap, image.left, image.top, image.getWidth(), image.getHeight());
      }
      this.ctx.restore();
    }
  }

  /**
   * Draw crop rect.
   */
  private drawCrop(): void {
    let crop = this.cropShow.getCropRect();
    MathUtils.roundOutRect(crop);
    let display = new RectF();
    display.set(0, 0, this.displayWidth, this.displayHeight);
    if (this.ctx !== undefined) {
      DrawingUtils.drawMask(this.ctx, display, crop, this.splitLineShow);
      if (this.splitLineShow) {
        DrawingUtils.drawSplitLine(this.ctx, crop, this.splitFraction, this.splitLineShow);
      }
      DrawingUtils.drawRect(this.ctx, crop);
      DrawingUtils.drawCropButton(this.ctx, crop);
    }
  }

  /**
   * Drag image.
   * @param offsetX
   * @param offsetY
   */
  private onImageDrag(offsetX: number, offsetY: number): void {
    let tX = this.isFlipHorizontal ? -1 : 1;
    let tY = this.isFlipVertically ? -1 : 1;
    let alpha = MathUtils.formulaAngle(this.rotationAngle * tX * tY + this.sliderAngle);
    let x = Math.cos(alpha) * offsetX * tX + Math.sin(alpha) * offsetY * tY;
    let y = -Math.sin(alpha) * offsetX * tX + Math.cos(alpha) * offsetY * tY;
    let image = this.cropShow.getImageRect();
    image.move(x, y);
    this.cropShow.setImageRect(image);
  }

  /**
   * End drag image.
   */
  private endImageDrag(): void {
    let crop = this.cropShow.getCropRect();
    let points = MathUtils.rectToPoints(crop);
    let tX = this.isFlipHorizontal ? -1 : 1;
    let tY = this.isFlipVertically ? -1 : 1;
    let angle = -(this.rotationAngle * tX * tY + this.sliderAngle);
    let displayCenter = new Point(this.displayWidth / 2, this.displayHeight / 2);
    let rotated = MathUtils.rotatePoints(points, angle, displayCenter);

    let flipImage = this.cropShow.getCurrentFlipImage();
    let offsets = MathUtils.fixImageMove(rotated, flipImage);
    let image = this.cropShow.getImageRect();
    image.move(offsets[0] * tX, offsets[1] * tY);
    this.cropShow.setImageRect(image);
  }
}