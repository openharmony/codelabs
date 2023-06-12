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

import hilog from '@ohos.hilog';
import CommonConstants from '../../common/constants/commonConstants';

export default {
  data: {
    degrees: 0,
    rotateY: 0,
    imageHeight: 0,
    imageWidth: 0,
    isCropped: false,
    isRotated: false,
    isZoomed: false,
    isMirror: false,
    buttonList: [
      {
        'buttonType': CommonConstants.OperationType.CROP,
        'src': 'common/images/ic_crop.png',
        'description': ''
      },
      {
        'buttonType': CommonConstants.OperationType.ROTATE,
        'src': 'common/images/ic_rotate.png',
        'description': ''
      },
      {
        'buttonType': CommonConstants.OperationType.ZOOM,
        'src': 'common/images/ic_zoom.png',
        'description': ''
      },
      {
        'buttonType': CommonConstants.OperationType.MIRROR,
        'src': 'common/images/ic_mirror.png',
        'description': ''
      }
    ]
  },
  onInit() {
    this.buttonList[0].description = this.$t('strings.crop_description');
    this.buttonList[1].description = this.$t('strings.rotate_description');
    this.buttonList[2].description = this.$t('strings.zoom_description');
    this.buttonList[3].description = this.$t('strings.mirror_description');
  },
  onShow() {
    this.imageHeight = this.$element('picture').getBoundingClientRect().height;
    this.imageWidth = this.$element('picture').getBoundingClientRect().width;
  },
  pictureManipulation(buttonType) {
    if (this.isCropped || this.isRotated || this.isZoomed || this.isMirror) {
      this.$element('picture').setStyle('clipPath', 'inset(0, 0, 0, 0');
      this.$element('picture').setStyle('transform', 'rotate(0)');
      this.$element('picture').setStyle('height', this.imageHeight);
      this.$element('picture').setStyle('width', this.imageWidth);
      this.$element('picture').setStyle('transform', 'rotateY(0)');
      this.degrees = 0;
      this.rotateY = 0;
      this.isCropped = false;
      this.isRotated = false;
      this.isZoomed = false;
      this.isMirror = false;
    } else {
      switch (buttonType) {
        case CommonConstants.OperationType.CROP:
          this.$element('picture')
            .setStyle('clipPath', 'inset(0, 0, ' + (this.imageHeight / CommonConstants.SPLIT_IN_HALF) + ', 0');
          this.isCropped = true;
          break;
        case CommonConstants.OperationType.ROTATE:
          this.degrees = this.degrees + CommonConstants.ROTATE_DEGREE;
          this.$element('picture').setStyle('transform', 'rotate(' + this.degrees + ')');
          this.isRotated = true;
          break;
        case CommonConstants.OperationType.ZOOM:
          this.$element('picture').setStyle('height', this.imageHeight / CommonConstants.SPLIT_IN_HALF);
          this.$element('picture').setStyle('width', this.imageWidth / CommonConstants.SPLIT_IN_HALF);
          this.isZoomed = true;
          break;
        case CommonConstants.OperationType.MIRROR:
          this.rotateY = this.rotateY + CommonConstants.ROTATE_Y;
          this.$element('picture').setStyle('transform', 'rotateY(' + this.rotateY + ')');
          this.isMirror = true;
          break;
        default:
          hilog.info(0x0000, 'ImageOperation', '%{public}s', 'Operation type is wrong!');
          break;
      }
    }
  }
};