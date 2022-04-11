/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
import app from '@system.app';
import prompt from '@system.prompt';
export default {
  data: {
    title: '',
    titleAppBar: '',
    picFrame: '',
    cropping: '',
    adjust: '',
    picFraImgSrc: '',
    conBotFirImgSrc: '',
    conBotSecImgSrc: '',
    conBotThrImgSrc: '',
    conBotFouImgSrc: '',
    luminance: '',
    luminanceImgSrc: '',
    luminanceColor: '',
    contrast: '',
    contrastImgSrc: '',
    contrastColor: '',
    saturation: '',
    saturationImgSrc: '',
    saturationColor: '',
    cropWidth: 0,
    cropHeight: 0,
    cropTop: 38,
    cropLeft: 0,
    cropBoxLeftOne: 0,
    cropBoxLeftTwo: 0,
    cropBoxLeftThr: 0,
    cropBoxLeftFou: 0,
    cropOpWidth: 0,
    cropOpHeight: 0,
    brightnessValue: 10,
    oldBrightnessValue: 10,
    contrastValue: 10,
    oldContrastValue: 10,
    saturationValue: 10,
    oldSaturationValue: 10,
    brightnessImgData: null,
    contrastImgData: null,
    saturationImgData: null,
    dWidth: 0,
    dHeight: 0,
    proport: 0,
    sx: 0,
    sy: 0,
    dx: 0,
    dy: 0,
    offset: 30,
    showFlag1: true,
    showFlag2: false,
    showBrightness: true,
    showContrast: false,
    showSaturation: false
  },
  onInit() {
    this.cropWidth = 300;
    this.cropHeight = 224;
    this.cropLeft = this.offset;
    this.title = this.$t('strings.title');
    this.titleAppBar = this.$t('strings.titleAppBar');
    this.picFrame = this.$t('strings.picFrame');
    this.cropping = this.$t('strings.cropping');
    this.adjust = this.$t('strings.adjust');
    this.luminance = this.$t('strings.luminance');
    this.contrast = this.$t('strings.contrast');
    this.saturation = this.$t('strings.saturation');
    this.picFraImgSrc = this.$t('strings.picFraImgSrc');
    this.conBotFirImgSrc = 'common/images/picture_frame_white_blue.svg';
    this.conBotSecImgSrc = this.$t('strings.conBotSecImgSrc');
    this.conBotThrImgSrc = this.$t('strings.conBotThrImgSrc');
    this.conBotFouImgSrc = this.$t('strings.conBotFouImgSrc');
    this.luminanceImgSrc = 'common/images/luminance_blue.svg';
    this.contrastImgSrc = this.$t('strings.contrastImgSrc');
    this.saturationImgSrc = this.$t('strings.saturationImgSrc');
  },
  // 画布canvas初始化图片
  onShow() {
    const img = new Image();
    img.src = 'common/images/image.jpg';
    const el = this.$element('canvasOne');

    const ctx = el.getContext('2d');
    // 绘制原始图片
    this.dWidth = 300;
    this.dHeight = 224;
    ctx.drawImage(img, 0, 38, 300, 224);
    // 保存图片
    this.brightnessImgData = ctx.getImageData(0, 0, 300, 300);
    this.contrastImgData = ctx.getImageData(0, 0, 300, 300);
    this.saturationImgData = ctx.getImageData(0, 0, 300, 300);
    ctx.save();
    // 绘制完成,赋值裁剪框宽、高
    this.cropOpWidth = this.dWidth;
    this.cropOpHeight = this.dHeight;
    // 裁剪框左边距(原图)
    this.cropBoxLeftOne = (this.dWidth - this.cropOpWidth) / 2 + this.offset;
    // 裁剪框左边距(1:1)
    this.cropBoxLeftTwo = (this.dWidth - this.dHeight) / 2 + this.offset;
    // 裁剪框左边距(16:9)
    this.cropBoxLeftThr = (this.dWidth - this.cropOpWidth) / 2 + this.offset;
    // 裁剪框左边距(9:16)
    this.cropBoxLeftFou = (this.dWidth - this.dHeight * 9 / 16) / 2 + this.offset;
    // 裁剪框上边距(原图;图片相对于画布)
    this.cropBoxTopOne = 38;
    // 裁剪框上边距(1:1;图片相对于画布)
    this.cropBoxTopTwo = 38;
    // 裁剪框上边距(16:9;图片相对于画布)
    this.cropBoxTopThr = (300 - this.dWidth * 9 / 16) / 2;
    // 裁剪框上边距(9:16;图片相对于画布)
    this.cropBoxTopFou = 38;
  },
  // 裁剪页面
  showCropPage() {
    this.showFlag1 = true;
    this.showFlag2 = false;
    // proport:裁剪比例(0:原图、1:1比1、16/9:16比9、9/16:9:16)
    switch (this.proport) {
      case 0:
        this.conBotFirImage();
        break;
      case 1:
        this.conBotSecImage();
        break;
      case 16 / 9:
        this.conBotThrImage();
        break;
      case 9 / 16:
        this.conBotFouImage();
        break;
      default:
        break;
    }
  },
  // 调节页面
  showAdjustPage() {
    this.showFlag1 = false;
    this.showFlag2 = true;
    this.showBrightness = true;
    this.showContrast = false;
    this.showSaturation = false;
    this.luminanceColor = '#2788B9';
    this.contrastColor = '#ffffff';
    this.saturationColor = '#ffffff';
    switch (this.proport) {
      case 0:
        break;
      case 1:
        this.cropOne();
        break;
      case 16 / 9:
        this.cropThr();
        break;
      case 9 / 16:
        this.cropFour();
        break;
      default:
        break;
    }
  },
  // 1:1比例裁剪
  cropOne() {
    const el = this.$element('canvasOne');
    const ctx = el.getContext('2d');
    this.sx = (this.dWidth - this.cropWidth) / 2;
    // 300:裁剪后图片的宽度(最大值设置300)
    this.sy = (300 - this.cropHeight) / 2;
    let imageData;
    if (this.dHeight === 300) {
      // 9:16后宽度按比例变大
      imageData = ctx.getImageData(this.sx, this.sy, this.cropWidth + (300 - 224 * 9 / 16) / 2, this.cropWidth);
    } else {
      imageData = ctx.getImageData(this.sx, this.sy, this.cropWidth, this.cropWidth);
    }
    ctx.clearRect(0, 0, 500, 500);
    ctx.scale(this.cropOpWidth / this.cropWidth, this.cropOpHeight / this.cropHeight);
    this.dx = -(this.cropOpWidth - this.dWidth) / 2;
    this.dy = 0;
    ctx.putImageData(imageData, this.dx, this.dy);
    ctx.scale(this.cropWidth / 300, this.cropHeight / 300);
    // 保存图片
    this.brightnessImgData = ctx.getImageData(0, 0, 300, 300);
    this.contrastImgData = ctx.getImageData(0, 0, 300, 300);
    this.saturationImgData = ctx.getImageData(0, 0, 300, 300);
    ctx.save();
    // 裁剪后裁剪框距离左边距离(9:16)
    this.cropBoxLeftFou = (this.cropOpWidth - this.cropOpHeight * 9 / 16) / 2 + this.offset;
    // 裁剪后裁剪框距离左边距离(16:9)
    this.cropBoxLeftThr = this.offset;
    // 裁剪后裁剪框距离左边距离(1:1)
    this.cropBoxLeftTwo = this.offset;
    // 裁剪后裁剪框距离左边距离(原图)
    this.cropBoxLeftOne = this.offset;
    // 裁剪后裁剪框距离上边距离
    this.cropBoxTopOne = (this.cropOpHeight - this.cropOpWidth) / 2;
    this.cropBoxTopTwo = (this.cropOpHeight - this.cropOpWidth) / 2;
    this.cropBoxTopThr = (this.cropOpHeight - this.cropOpWidth * 9 / 16) / 2;
    this.cropBoxTopFou = (this.cropOpHeight - this.cropOpWidth) / 2;
    this.dWidth = this.cropOpWidth;
    this.dHeight = this.cropOpHeight;
  },
  // 16:9比例裁剪
  cropThr() {
    const el = this.$element('canvasOne');
    const ctx = el.getContext('2d');
    this.sx = (300 - this.dWidth) / 2;
    this.sy = (300 - this.cropHeight) / 2;

    const imageData = ctx.getImageData(this.sx, this.sy, this.cropWidth, this.cropHeight);
    ctx.clearRect(0, 0, 500, 500);
    if (this.dHeight === 300) {
      ctx.scale(300 / this.cropWidth, 300 / this.cropWidth);
      this.cropOpWidth = this.cropWidth * (300 / this.cropWidth);
      this.cropOpHeight = this.cropHeight * (300 / this.cropWidth);
    }
    this.dy = (300 - this.cropHeight * (300 / this.cropWidth)) / (2 * (300 / this.cropWidth));
    ctx.putImageData(imageData, 0, this.dy);
    if (this.dHeight === 300) {
      ctx.scale(this.cropWidth / 300, this.cropWidth / 300);
    }
    // 保存图片
    this.brightnessImgData = ctx.getImageData(0, 0, 300, 300);
    this.contrastImgData = ctx.getImageData(0, 0, 300, 300);
    this.saturationImgData = ctx.getImageData(0, 0, 300, 300);
    ctx.save();
    this.cropBoxLeftFou = (this.cropOpWidth - this.cropOpHeight * 9 / 16) / 2 + this.offset;
    this.cropBoxLeftThr = this.offset;
    this.cropBoxLeftTwo = (this.cropOpWidth - this.cropOpHeight * 1 / 1) / 2 + this.offset;
    this.cropBoxLeftOne = this.offset;
    this.cropBoxTopOne = (300 - this.cropOpHeight) / 2;
    this.cropBoxTopTwo = (300 - this.cropOpHeight) / 2;
    this.cropBoxTopThr = (300 - this.cropOpHeight) / 2;
    this.cropBoxTopFou = (300 - this.cropOpHeight) / 2;
    // 裁剪完图片宽高等于裁剪比例缩放后宽高
    this.dWidth = this.cropOpWidth;
    this.dHeight = this.cropOpHeight;
    // 裁剪框宽高等于裁剪比例缩放后宽高
    this.cropWidth = this.cropOpWidth;
    this.cropHeight = this.cropOpHeight;
  },
  // 9:16比例裁剪
  cropFour() {
    const el = this.$element('canvasOne');
    const ctx = el.getContext('2d');

    this.sx = (this.dWidth - this.cropWidth) / 2;
    this.sy = (300 - this.cropHeight) / 2;
    let imageData;
    if (this.dHeight === 300 && this.dWidth !== 300) {
      imageData = ctx.getImageData(this.sx, this.sy, this.cropWidth + (300 - 224 * 9 / 16) / 2, this.cropHeight);
    } else {
      imageData = ctx.getImageData(this.sx, this.sy, this.cropWidth, this.cropHeight);
    }
    ctx.clearRect(0, 0, 500, 500);
    if (this.dWidth === 300) {
      ctx.scale(300 / this.cropHeight, 300 / this.cropHeight);
      this.cropOpHeight = this.cropHeight * (300 / this.cropHeight);
      this.cropOpWidth = this.cropWidth * (300 / this.cropHeight);
    }
    if (this.dHeight === 300 && this.dWidth !== 300) {
      this.dx = 0;
    } else {
      this.dx = (300 - this.cropWidth * (300 / this.cropHeight)) / (2 * (300 / this.cropHeight));
    }
    this.dy = 0;
    ctx.putImageData(imageData, this.dx, this.dy);
    if (this.dWidth === 300) {
      ctx.scale(this.cropHeight / 300, this.cropHeight / 300);
    }
    // 保存图片
    this.brightnessImgData = ctx.getImageData(0, 0, 300, 300);
    this.contrastImgData = ctx.getImageData(0, 0, 300, 300);
    this.saturationImgData = ctx.getImageData(0, 0, 300, 300);
    ctx.save();
    // 裁剪后裁剪框距离左边距离(9:16)
    this.cropBoxLeftFou = (300 - this.cropOpWidth) / 2 + this.offset;
    // 裁剪后裁剪框距离左边距离(16:9)
    this.cropBoxLeftThr = (300 - this.cropOpWidth) / 2 + this.offset;
    // 裁剪后裁剪框距离左边距离(1:1)
    this.cropBoxLeftTwo = (300 - this.cropOpWidth) / 2 + this.offset;
    // 裁剪后裁剪框距离左边距离(原图)
    this.cropBoxLeftOne = (300 - this.cropOpWidth) / 2 + this.offset;
    // 裁剪后裁剪框相对于画布y坐标
    this.cropBoxTopOne = (this.cropOpHeight - this.cropOpWidth * 16 / 9) / 2;
    this.cropBoxTopTwo = (this.cropOpHeight - this.cropOpWidth) / 2;
    this.cropBoxTopThr = (this.cropOpHeight - this.cropOpWidth * 9 / 16) / 2;
    this.cropBoxTopFou = (this.cropOpHeight - this.cropOpWidth * 16 / 9) / 2;
    this.dWidth = this.cropOpWidth;
    this.dHeight = this.cropOpHeight;
  },

  // 原图尺寸
  conBotFirImage() {
    this.conBotFirImgSrc = 'common/images/picture_frame_white_blue.svg';
    this.conBotSecImgSrc = this.$t('strings.conBotSecImgSrc');
    this.conBotThrImgSrc = this.$t('strings.conBotThrImgSrc');
    this.conBotFouImgSrc = this.$t('strings.conBotFouImgSrc');
    this.cropTop = this.cropBoxTopOne;
    // 裁剪后图片宽高
    this.cropOpWidth = this.dWidth;
    this.cropOpHeight = this.dHeight;
    // 裁剪框的宽高
    this.cropWidth = this.dWidth;
    this.cropHeight = this.dHeight;
    this.cropLeft = this.cropBoxLeftOne;
    this.proport = 0;
  },
  // 1:1
  conBotSecImage() {
    this.conBotFirImgSrc = this.$t('strings.conBotFirImgSrc');
    this.conBotSecImgSrc = 'common/images/proportions_1-1_blue.png';
    this.conBotThrImgSrc = this.$t('strings.conBotThrImgSrc');
    this.conBotFouImgSrc = this.$t('strings.conBotFouImgSrc');
    this.cropLeft = this.cropBoxLeftTwo;
    if (this.dWidth < this.dHeight) {
      this.cropTop = (this.dHeight - this.dWidth) / 2;
      this.cropWidth = this.dWidth;
      this.cropHeight = this.dWidth;
    } else {
      this.cropTop = this.cropBoxTopTwo;
      this.cropWidth = this.dHeight;
      this.cropHeight = this.dHeight;
    }
    // 放到最大300
    this.cropOpWidth = 300;
    this.cropOpHeight = 300;
    this.proport = 1;
  },
  // 16:9
  conBotThrImage() {
    this.conBotFirImgSrc = this.$t('strings.conBotFirImgSrc');
    this.conBotSecImgSrc = this.$t('strings.conBotSecImgSrc');
    this.conBotThrImgSrc = 'common/images/proportions_16-9_blue.png';
    this.conBotFouImgSrc = this.$t('strings.conBotFouImgSrc');
    this.cropTop = this.cropBoxTopThr;
    this.cropWidth = this.dWidth;
    this.cropHeight = this.cropWidth * 9 / 16;
    this.cropOpWidth = 300;
    this.cropOpHeight = this.cropOpWidth * 9 / 16;
    this.cropLeft = this.cropBoxLeftThr;
    this.proport = 16 / 9;
  },
  // 9:16
  conBotFouImage() {
    this.conBotFirImgSrc = this.$t('strings.conBotFirImgSrc');
    this.conBotSecImgSrc = this.$t('strings.conBotSecImgSrc');
    this.conBotThrImgSrc = this.$t('strings.conBotThrImgSrc');
    this.conBotFouImgSrc = 'common/images/proportions_9-16_blue.png';
    this.cropTop = this.cropBoxTopFou;
    this.cropLeft = this.cropBoxLeftFou;
    this.cropWidth = this.dHeight * 9 / 16;
    this.cropHeight = this.dHeight;
    this.cropOpHeight = 300;
    this.cropOpWidth = this.cropOpHeight * 9 / 16;
    this.proport = 9 / 16;
  },
  // 亮度
  luminanceAdj() {
    this.luminanceColor = '#2788B9';
    this.contrastColor = '#ffffff';
    this.saturationColor = '#ffffff';
    this.luminanceImgSrc = 'common/images/luminance_blue.svg';
    this.contrastImgSrc = this.$t('strings.contrastImgSrc');
    this.saturationImgSrc = this.$t('strings.saturationImgSrc');
    this.showBrightness = true;
    this.showContrast = false;
    this.showSaturation = false;
    // 还原成裁剪后的图片
    const test = this.$element('canvasOne');
    const ctx = test.getContext('2d');
    ctx.restore();
    ctx.putImageData(this.brightnessImgData, 0, 0);
    // 将slider值设置成10(最大为10)
    this.brightnessValue = 10;
    this.contrastValue = 10;
    this.saturationValue = 10;
  },
  // 对比度
  contrastAdj() {
    this.luminanceColor = '#ffffff';
    this.contrastColor = '#2788B9';
    this.saturationColor = '#ffffff';
    this.luminanceImgSrc = this.$t('strings.luminanceImgSrc');
    this.contrastImgSrc = 'common/images/contrast_blue.svg';
    this.saturationImgSrc = this.$t('strings.saturationImgSrc');
    this.showBrightness = false;
    this.showContrast = true;
    this.showSaturation = false;
    // 还原成裁剪后的图片
    const test = this.$element('canvasOne');
    const ctx = test.getContext('2d');
    ctx.restore();
    ctx.putImageData(this.contrastImgData, 0, 0);
    // 将slider值设置成10(最大为10)
    this.brightnessValue = 10;
    this.contrastValue = 10;
    this.saturationValue = 10;
  },
  // 饱和度
  saturationAdj() {
    this.luminanceColor = '#ffffff';
    this.contrastColor = '#ffffff';
    this.saturationColor = '#2788B9';
    this.luminanceImgSrc = this.$t('strings.luminanceImgSrc');
    this.contrastImgSrc = this.$t('strings.contrastImgSrc');
    this.saturationImgSrc = 'common/images/saturation_blue.svg';
    this.showBrightness = false;
    this.showContrast = false;
    this.showSaturation = true;
    // 还原成裁剪后的图片
    const test = this.$element('canvasOne');
    const ctx = test.getContext('2d');
    ctx.restore();
    ctx.putImageData(this.saturationImgData, 0, 0);
    // 将slider值设置成10(最大为10)
    this.brightnessValue = 10;
    this.contrastValue = 10;
    this.saturationValue = 10;
  },
  // 亮度调节
  setBrightnessValue(e) {
    if (e.mode === 'start') {
      this.oldBrightnessValue = e.value;
    } else if (e.mode === 'end') {
      this.brightnessValue = e.value;
      if (e.value === 10) {
        const test = this.$element('canvasOne');
        const ctx = test.getContext('2d');
        ctx.restore();
        ctx.putImageData(this.brightnessImgData, 0, 0);
      } else {
        const adjustValue = e.value / this.oldBrightnessValue;
        this.adjustBrightness(adjustValue);
        this.oldBrightnessValue = e.value;
      }
    }
  },
  adjustBrightness(value) {
    const test = this.$element('canvasOne');
    const ctx = test.getContext('2d');
    const imgData = ctx.getImageData(0, 0, 300, 300);

    ctx.putImageData(this.changeBrightness(imgData, value), 0, 0);
  },
  changeBrightness(imgdata, value) {
    const data = imgdata.data;
    for (let i = 0; i < data.length; i += 4) {
      const hsv = this.rgb2hsv([data[i], data[i + 1], data[i + 2]]);
      hsv[2] *= value;
      const rgb = this.hsv2rgb([...hsv]);
      data[i] = rgb[0];
      data[i + 1] = rgb[1];
      data[i + 2] = rgb[2];
    }
    return imgdata;
  },
  // 对比度调节
  setContrastValue(e) {
    if (e.mode === 'start') {
      this.oldContrastValue = e.value;
    } else if (e.mode === 'end') {
      this.contrastValue = e.value;
      if (e.value === 10) {
        const test = this.$element('canvasOne');
        const ctx = test.getContext('2d');
        ctx.restore();
        ctx.putImageData(this.contrastImgData, 0, 0);
      } else {
        const adjustValue = e.value / this.oldContrastValue;
        this.adjustContrast(adjustValue);
        this.oldContrastValue = e.value;
      }
    }
  },
  adjustContrast(value) {
    const test = this.$element('canvasOne');
    const ctx = test.getContext('2d');
    const imgData = ctx.getImageData(0, 0, 300, 300);
    ctx.putImageData(this.changeContrast(imgData, value), 0, 0);
  },
  changeContrast(imgdata, value) {
    const data = imgdata.data;
    for (let i = 0; i < data.length; i += 4) {
      const hsv = this.rgb2hsv([data[i], data[i + 1], data[i + 2]]);
      hsv[0] *= value;
      const rgb = this.hsv2rgb([...hsv]);
      data[i] = rgb[0];
      data[i + 1] = rgb[1];
      data[i + 2] = rgb[2];
    }
    return imgdata;
  },
  // 饱和度调节
  setSaturationValue(e) {
    if (e.mode === 'start') {
      this.oldSaturationValue = e.value;
    } else if (e.mode === 'end') {
      this.saturationValue = e.value;
      if (e.value === 10) {
        const test = this.$element('canvasOne');
        const ctx = test.getContext('2d');
        ctx.restore();
        ctx.putImageData(this.saturationImgData, 0, 0);
      } else {
        const adjustValue = e.value / this.oldSaturationValue;
        this.adjustSaturation(adjustValue);
        this.oldSaturationValue = e.value;
      }
    }
  },
  adjustSaturation(value) {
    const test = this.$element('canvasOne');
    const ctx = test.getContext('2d');

    const imgData = ctx.getImageData(0, 0, 300, 300);
    ctx.putImageData(this.changeSaturation(imgData, value), 0, 0);
  },
  changeSaturation(imgdata, value) {
    const data = imgdata.data;
    for (let i = 0; i < data.length; i += 4) {
      const hsv = this.rgb2hsv([data[i], data[i + 1], data[i + 2]]);
      hsv[1] *= value;
      const rgb = this.hsv2rgb([...hsv]);
      data[i] = rgb[0];
      data[i + 1] = rgb[1];
      data[i + 2] = rgb[2];
    }
    return imgdata;
  },
  // RGB转HSV
  rgb2hsv(arr) {
    let rr;
    let gg;
    let bb;
    const r = arr[0] / 255;
    const g = arr[1] / 255;
    const b = arr[2] / 255;
    let h;
    let s;
    const v = Math.max(r, g, b);
    const diff = v - Math.min(r, g, b);
    const diffc = function(c) {
      return (v - c) / 6 / diff + 1 / 2;
    };

    if (diff === 0) {
      h = s = 0;
    } else {
      s = diff / v;
      rr = diffc(r);
      gg = diffc(g);
      bb = diffc(b);

      if (r === v) {
        h = bb - gg;
      } else if (g === v) {
        h = 1 / 3 + rr - bb;
      } else if (b === v) {
        h = 2 / 3 + gg - rr;
      }
      if (h < 0) {
        h += 1;
      } else if (h > 1) {
        h -= 1;
      }
    }
    return [Math.round(h * 360), Math.round(s * 100), Math.round(v * 100)];
  },
  // HSV转RGB
  hsv2rgb(hsv) {
    let _l = hsv[0];
    let _m = hsv[1];
    let _n = hsv[2];
    let newR;
    let newG;
    let newB;
    if (_m === 0) {
      _l = _m = _n = Math.round(255 * _n / 100);
      newR = _l;
      newG = _m;
      newB = _n;
    } else {
      _m = _m / 100;
      _n = _n / 100;
      const p = Math.floor(_l / 60) % 6;
      const f = _l / 60 - p;
      const a = _n * (1 - _m);
      const b = _n * (1 - _m * f);
      const c = _n * (1 - _m * (1 - f));
      switch (p) {
        case 0:
          newR = _n; newG = c; newB = a;
          break;
        case 1:
          newR = b; newG = _n; newB = a;
          break;
        case 2:
          newR = a; newG = _n; newB = c;
          break;
        case 3:
          newR = a; newG = b; newB = _n;
          break;
        case 4:
          newR = c; newG = a; newB = _n;
          break;
        case 5:
          newR = _n; newG = a; newB = b;
          break;
      }
      newR = Math.round(255 * newR);
      newG = Math.round(255 * newG);
      newB = Math.round(255 * newB);
    }
    return [newR, newG, newB];
  },
  // 回退
  rewind() {
    prompt.showToast({
      message: 'Please implement your rewind function'
    });
  },
  // 前进
  advance() {
    prompt.showToast({
      message: 'Please implement your advance function'
    });
  },
  // 保存
  save() {
    prompt.showToast({
      message: 'Please implement your save function'
    });
  },
  // 退出弹窗
  showDialog(e) {
    this.$element('simpleDialog').show();
  },
  cancelSchedule(e) {
    this.$element('simpleDialog').close();
  },
  setSchedule(e) {
    this.$element('simpleDialog').close();
    app.terminate();
  }
};
