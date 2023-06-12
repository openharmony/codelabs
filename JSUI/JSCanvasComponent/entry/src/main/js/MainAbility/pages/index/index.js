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

import resourceManager from '@ohos.resourceManager';
import featureAbility from '@ohos.ability.featureAbility';
import CommonConstants from '../../common/constants/commonConstants';
import ColorConstants from '../../common/constants/colorConstants';
import Logger from '../../common/utils/logger';

export default {
  data: {
    screenWidth: 0,
    screenHeight: 0,
    startAngle: 0,
    avgAngle: 0,
    rotateDegree: 0,
    infinite: 0,
    prizeData: {
      'message': '',
      'imageSrc': ''
    },
    ctx: null,
    disabledFlag: false,
    playState: CommonConstants.PLAY_STATE.pause
  },

  /**
   * Triggered during page initialization.
   */
  onInit() {
    // Obtains the screen density of the current device.
    resourceManager.getResourceManager((error, mgr) => {
      if(error) {
        Logger.error(`[index][onInit]getResourceManager error is ${JSON.stringify(error)}`);
        return;
      }
      mgr.getDeviceCapability().then(value => {
        let screenDensity = value.screenDensity;
        // Obtains the size of the screen, excluding the size of the status bar.
        featureAbility.getWindow().then((data) => {
          let windowProperties = data.getWindowProperties();
          this.screenWidth = windowProperties.windowRect.width / screenDensity;
          this.screenHeight = windowProperties.windowRect.height / screenDensity;
        });
      }).catch(err => {
        Logger.error(`[index][onInit]getDeviceCapability error is ${JSON.stringify(err)}`);
      });
    });
  },

  /**
   * Triggered when the page is displayed.
   */
  onShow() {
    if (this.ctx !== null) {
      return;
    }
    // Obtains the CanvasRenderingContext2D.
    this.ctx = this.$refs.canvas.getContext('2d');
    this.avgAngle = CommonConstants.CIRCLE / CommonConstants.COUNT;
    this.draw();
  },

  /**
   * Draw the raffle round turntable.
   */
  draw() {
    // Translates the canvas along the X and Y axes by a specified distance.
    this.ctx.translate(this.screenWidth / CommonConstants.HALF,
      this.screenHeight / CommonConstants.HALF);
    // Draw the petals on the outer disc.
    this.drawFlower();
    // Draw outer disc, small circle.
    this.drawOutCircle();
    // Draw the inner disc.
    this.drawInnerCircle();
    // Draw the interior fan-shaped raffle area.
    this.drawInnerArc();
    // Draw text in the internal fan area.
    this.drawArcText();
    // Draw the picture corresponding to the prize in the internal fan area.
    this.drawImage();
  },

  /**
   * Draw the petals on the outer disc.
   */
  drawFlower() {
    let beginAngle = this.startAngle + this.avgAngle;
    const pointY = this.screenWidth * CommonConstants.FLOWER_POINT_Y_RATIOS;
    const radius = this.screenWidth * CommonConstants.FLOWER_RADIUS_RATIOS;
    const innerRadius = this.screenWidth * CommonConstants.FLOWER_INNER_RATIOS;
    for (let i = 0; i < CommonConstants.COUNT; i++) {
      this.ctx.save();
      this.ctx.rotate(beginAngle * Math.PI / CommonConstants.HALF_CIRCLE);
      this.ctx.beginPath();
      this.ctx.fillStyle = ColorConstants.FLOWER_OUT_COLOR;
      this.ctx.arc(0, -pointY, radius, 0, Math.PI * CommonConstants.HALF);
      this.ctx.fill();

      this.ctx.beginPath();
      this.ctx.fillStyle = ColorConstants.FLOWER_INNER_COLOR;
      this.ctx.arc(0, -pointY, innerRadius, 0, Math.PI * CommonConstants.HALF);
      this.ctx.fill();
      beginAngle += this.avgAngle;
      this.ctx.restore();
    }
  },

  /**
   * Draw outer disc, small circle.
   */
  drawOutCircle() {
    // Draw outer disc.
    this.ctx.beginPath();
    this.ctx.fillStyle = ColorConstants.OUT_CIRCLE_COLOR;
    this.ctx.arc(0, 0, this.screenWidth * CommonConstants.OUT_CIRCLE_RATIOS, 0, Math.PI * CommonConstants.HALF);
    this.ctx.fill();

    // Draw small circle.
    let beginAngle = this.startAngle;
    for (let i = 0; i < CommonConstants.SMALL_CIRCLE_COUNT; i++) {
      this.ctx.save();
      this.ctx.rotate(beginAngle * Math.PI / CommonConstants.HALF_CIRCLE);

      this.ctx.beginPath();
      this.ctx.fillStyle = ColorConstants.WHITE_COLOR;
      this.ctx.arc(this.screenWidth * CommonConstants.SMALL_CIRCLE_RATIOS, 0,
        CommonConstants.SMALL_CIRCLE_RADIUS, 0, Math.PI * CommonConstants.HALF);
      this.ctx.fill();
      beginAngle = beginAngle + CommonConstants.CIRCLE / CommonConstants.SMALL_CIRCLE_COUNT;
      this.ctx.restore();
    }
  },

  /**
   * Draw the inner disc.
   */
  drawInnerCircle() {
    this.ctx.beginPath();
    this.ctx.fillStyle = ColorConstants.INNER_CIRCLE_COLOR;
    this.ctx.arc(0, 0, this.screenWidth * CommonConstants.INNER_CIRCLE_RATIOS, 0, Math.PI * CommonConstants.HALF);
    this.ctx.fill();

    this.ctx.beginPath();
    this.ctx.fillStyle = ColorConstants.WHITE_COLOR;
    this.ctx.arc(0, 0, this.screenWidth * CommonConstants.INNER_WHITE_CIRCLE_RATIOS,
      0, Math.PI * CommonConstants.HALF);
    this.ctx.fill();
  },

  /**
   * Draw the interior fan-shaped raffle area.
   */
  drawInnerArc() {
    let colors = [
      ColorConstants.ARC_PINK_COLOR, ColorConstants.ARC_YELLOW_COLOR,
      ColorConstants.ARC_GREEN_COLOR, ColorConstants.ARC_PINK_COLOR,
      ColorConstants.ARC_YELLOW_COLOR, ColorConstants.ARC_GREEN_COLOR
    ];
    let radius = this.screenWidth * CommonConstants.INNER_ARC_RATIOS;
    for (let i = 0; i < CommonConstants.COUNT; i++) {
      this.ctx.beginPath();
      this.ctx.fillStyle = colors[i];
      this.ctx.arc(0, 0, radius, this.startAngle * Math.PI / CommonConstants.HALF_CIRCLE,
        (this.startAngle + this.avgAngle) * Math.PI / CommonConstants.HALF_CIRCLE);
      this.ctx.fill();
      this.ctx.lineTo(0, 0);
      this.ctx.fill();
      this.startAngle += this.avgAngle;
    }
  },

  /**
   * Draw text in the internal fan area.
   */
  drawArcText() {
    this.ctx.textAlign = CommonConstants.TEXT_ALIGN;
    this.ctx.textBaseline = CommonConstants.TEXT_BASE_LINE;
    this.ctx.fillStyle = ColorConstants.TEXT_COLOR;
    this.ctx.font = CommonConstants.CANVAS_FONT;
    let textArrays = [
      this.$t('strings.text_smile'),
      this.$t('strings.text_hamburger'),
      this.$t('strings.text_cake'),
      this.$t('strings.text_smile'),
      this.$t('strings.text_beer'),
      this.$t('strings.text_watermelon')
    ];
    let arcTextStartAngle = CommonConstants.ARC_START_ANGLE;
    let arcTextEndAngle = CommonConstants.ARC_END_ANGLE;
    for (let i = 0; i < CommonConstants.COUNT; i++) {
      this.drawCircularText(textArrays[i],
        (this.startAngle + arcTextStartAngle) * Math.PI / CommonConstants.HALF_CIRCLE,
        (this.startAngle + arcTextEndAngle) * Math.PI / CommonConstants.HALF_CIRCLE);
      this.startAngle += this.avgAngle;
    }
  },

  /**
   * Draw Arc Text.
   *
   * @param textString textString.
   * @param startAngle startAngle.
   * @param endAngle endAngle.
   */
  drawCircularText(textString, startAngle, endAngle) {
    let circleText = {
      x: 0,
      y: 0,
      radius: this.screenWidth * CommonConstants.INNER_ARC_RATIOS
    };
    // The radius of the circle.
    let radius = circleText.radius - circleText.radius / CommonConstants.COUNT;
    // The radians occupied by each letter.
    let angleDecrement = (startAngle - endAngle) / (textString.length - 1);
    let angle = startAngle;
    let index = 0;
    let character;

    while (index < textString.length) {
      character = textString.charAt(index);
      this.ctx.save();
      this.ctx.beginPath();
      this.ctx.translate(circleText.x + Math.cos(angle) * radius,
        circleText.y - Math.sin(angle) * radius);
      this.ctx.rotate(Math.PI / CommonConstants.HALF - angle);
      this.ctx.fillText(character, 0, 0);
      angle -= angleDecrement;
      index++;
      this.ctx.restore();
    }
  },

  /**
   * Draw the picture corresponding to the prize in the internal fan area.
   */
  drawImage() {
    let beginAngle = this.startAngle;
    let imageSrc = [
      CommonConstants.WATERMELON_IMAGE_URL, CommonConstants.BEER_IMAGE_URL,
      CommonConstants.SMILE_IMAGE_URL, CommonConstants.CAKE_IMAGE_URL,
      CommonConstants.HAMBURG_IMAGE_URL, CommonConstants.SMILE_IMAGE_URL
    ];
    let image = new Image();
    for (let i = 0; i < CommonConstants.COUNT; i++) {
      image.src = imageSrc[i];
      this.ctx.save();
      this.ctx.beginPath();
      this.ctx.rotate(beginAngle * Math.PI / CommonConstants.HALF_CIRCLE);
      this.ctx.drawImage(image, this.screenWidth * CommonConstants.IMAGE_DX_RATIOS,
        this.screenWidth * CommonConstants.IMAGE_DY_RATIOS, CommonConstants.IMAGE_SIZE,
        CommonConstants.IMAGE_SIZE);
      beginAngle += this.avgAngle;
      this.ctx.restore();
    }
  },

  startAnimator() {
    this.disabledFlag = !this.disabledFlag;
    let randomAngle = Math.round(Math.random() * CommonConstants.CIRCLE);
    // Obtaining prize information.
    this.showPrizeData(randomAngle);
    if (this.infinite === 0) {
      // Permanent rotation.
      this.infinite = -1;
    }
    setTimeout(() => {
      this.infinite = 0;
      this.playState = CommonConstants.PLAY_STATE.pause;
      this.$element('prize-dialog').show();
    }, CommonConstants.DURATION);
    this.rotateDegree = CommonConstants.CIRCLE * CommonConstants.FIVE_CIRCLE +
      CommonConstants.ANGLE - randomAngle;
    this.playState = CommonConstants.PLAY_STATE.running;
  },

  /**
   * Displaying information about prizes.
   *
   * @param randomAngle randomAngle.
   */
  showPrizeData(randomAngle) {
    for (let i = 1; i <= CommonConstants.COUNT; i++) {
      if (randomAngle <= i * this.avgAngle) {
        return this.getPrizeData(i);
      }
    }
    return;
  },

  /**
   * Obtaining information about prizes.
   *
   * @param scopeNum scopeNum.
   */
  getPrizeData(scopeNum) {
    switch (scopeNum) {
      case CommonConstants.WATERMELON_PART:
        this.prizeData.message = this.$t('strings.prize_text_watermelon');
        this.prizeData.imageSrc = CommonConstants.WATERMELON_IMAGE_URL;
        break;
      case CommonConstants.BEER_PART:
        this.prizeData.message = this.$t('strings.prize_text_beer');
        this.prizeData.imageSrc = CommonConstants.BEER_IMAGE_URL;
        break;
      case CommonConstants.SMILE_PART:
        this.prizeData.message = this.$t('strings.prize_text_smile');
        this.prizeData.imageSrc = CommonConstants.SMILE_IMAGE_URL;
        break;
      case CommonConstants.CAKE_PART:
        this.prizeData.message = this.$t('strings.prize_text_cake');
        this.prizeData.imageSrc = CommonConstants.CAKE_IMAGE_URL;
        break;
      case CommonConstants.HAMBURG_PART:
        this.prizeData.message = this.$t('strings.prize_text_hamburger');
        this.prizeData.imageSrc = CommonConstants.HAMBURG_IMAGE_URL;
        break;
      case CommonConstants.THANKS_PART:
        this.prizeData.message = this.$t('strings.prize_text_smile');
        this.prizeData.imageSrc = CommonConstants.SMILE_IMAGE_URL;
        break;
      default:
        break;
    }
  },

  closeDialog() {
    this.$element('prize-dialog').close();
    this.disabledFlag = !this.disabledFlag;
  }
};