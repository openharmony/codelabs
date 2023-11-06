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

import { RectF } from './Rect';
import { PixelMapWrapper } from './PixelMapWrapper';
import { MathUtils } from '../utils/MathUtils';

export class ImageFilterCrop {
  private cropRect: RectF = new RectF();

  cropImage(pixelMap: PixelMapWrapper, realCropRect: RectF, callback: () => void) {
    let offWidth = realCropRect.getWidth();
    let offHeight = realCropRect.getHeight();
    if (pixelMap.pixelMap!== undefined) {
      pixelMap.pixelMap.crop({
        size: { height: vp2px(offHeight), width: vp2px(offWidth) },
        x: vp2px(realCropRect.left),
        y: vp2px(realCropRect.top)
      }, callback);
    }
  }

  readerNew(pixelMap: PixelMapWrapper, callback: () => void) {
    let width = pixelMap.width;
    let height = pixelMap.height;
    let realCropRect = new RectF();
    realCropRect.set(this.cropRect.left, this.cropRect.top, this.cropRect.right, this.cropRect.bottom);
    MathUtils.revertRect(realCropRect, width, height);
    this.cropImage(pixelMap, realCropRect, callback);
  }

  setCropRect(rect: RectF) {
    this.cropRect.set(rect.left, rect.top, rect.right, rect.bottom);
  }
}