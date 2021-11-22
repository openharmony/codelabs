/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import prompt from '@system.prompt';

export default {
  data: {
    id: 'canvas',
    img: null,
    plane: null,
    ctx: null,
    screenDensity: 3,
    centerX: 192 * 1.8,
    centerY: 350 * 1.8,
    circle: 360,
    count: 6,
    padding: 50,
    startAngle: 0.0,
    avgAngle: 0.0,
    colors: ['#FFA3AE', '#FFDE4E', '#76E2DB', '#FFA3AE', '#FFDE4E', '#76E2DB'], // 颜色集合
    playState: 'paused',
    rotateDegree: 0,
    infinite: 0
  },

  // 页面显示时触发
  onShow() {
    if (this.ctx !== null) {
      return;
    }
    this.ctx = this.$element(this.id).getContext('2d');
    this.avgAngle = this.circle / this.count;
    this.draw();
  },

  // 开始画
  draw() {
    // 将画布沿X、Y轴平移指定距离
    this.ctx.translate(this.centerX + 10, this.centerY);
    // 画外部圆盘的花瓣
    this.drawFlower();
    // 画外部圆盘、小圈圈、五角星
    this.drawOutCircleAndFive();
    // 画内部扇形抽奖区域
    this.drawInnerArc();
    // 画内部扇形区域文字
    this.drawArcText();
    // 画内部扇形区域奖品对应的图片
    this.drawImage();
    // 画中心圆盘和指针
    this.drawCenter();
  },

  // 画外部圆盘的花瓣
  drawFlower() {
    let beginAngle = this.startAngle + this.avgAngle;
    const radius = this.centerX - this.padding;
    for (let i = 0; i < this.count; i++) {
      this.ctx.save();
      this.ctx.beginPath();
      this.ctx.fillStyle = '#F3B468';
      this.ctx.rotate(beginAngle * Math.PI / 180);
      this.ctx.arc(-radius / 2, radius / 2, radius / 2, 0, Math.PI * 2);
      this.ctx.fill();

      this.ctx.beginPath();
      this.ctx.fillStyle = '#E588B9';
      this.ctx.arc(-radius / 2, radius / 2, (radius - this.padding) / 2, 0, Math.PI * 2);
      beginAngle += this.avgAngle;
      this.ctx.fill();
      this.ctx.restore();
    }
  },

  // 画外部圆盘、小圈圈、五角星
  drawOutCircleAndFive() {
    this.ctx.beginPath();
    this.ctx.fillStyle = '#ED6D56';
    this.ctx.arc(0, 0, this.centerX - this.padding, 0, Math.PI * 2);
    this.ctx.fill();
    let beginAngle = this.startAngle + this.avgAngle;
    for (let i = 0; i < this.count * 3; i++) {
      this.ctx.save();
      if (0 === i % 2) {
        // 画小圆圈
        this.ctx.beginPath();
        this.ctx.rotate(beginAngle * Math.PI / 180);
        this.ctx.fillStyle = '#FFFFFF';
        this.ctx.arc(this.centerX - this.padding - this.padding / 2, 0, 5, 0, Math.PI * 2);
        this.ctx.fill();
      } else {
        // 画五角星
        this.paintFiveStart(beginAngle);
      }
      beginAngle = beginAngle + this.avgAngle / 3;
      this.ctx.restore();
    }
  },

  // 画五角星
  paintFiveStart(beginAngle) {
    // 画五角星的path
    this.ctx.beginPath();
    this.ctx.rotate(beginAngle * Math.PI / 180);
    this.ctx.fillStyle = '#FFFF00';
    const points = this.fivePoints(this.centerX - this.padding - this.padding / 2, 0, this.padding / 2);
    for (let i = 0; i < points.length - 1; i = i + 2) {
      this.ctx.lineTo(points[i], points[i + 1]);
    }
    this.ctx.closePath();
    this.ctx.fill();
  },

  // 获取五角星的五个顶点
  fivePoints(pointXa, pointYa, sideLength) {
    const radian = 18 * Math.PI / 180;
    const pointXb = pointXa + sideLength / 2;
    const num = sideLength * Math.sin(radian);
    const pointXc = pointXa + num;
    const pointXd = pointXa - num;
    const pointXe = pointXa - sideLength / 2;
    const pointYb = pointYa + Math.sqrt(Math.pow(pointXc - pointXd, 2) - Math.pow(sideLength / 2, 2));
    const pointYc = pointYa + Math.cos(radian) * sideLength;
    const pointYd = pointYc;
    const pointYe = pointYb;
    const points = [pointXa, pointYa, pointXd, pointYd, pointXb, pointYb,
      pointXe, pointYe, pointXc, pointYc, pointXa, pointYa];
    return points;
  },

  // 画内部扇形抽奖区域
  drawInnerArc() {
    const radius = this.centerX - this.padding * 2;
    for (let i = 0; i < this.count; i++) {
      this.ctx.beginPath();
      this.ctx.fillStyle = this.colors[i];
      this.ctx.arc(0, 0, radius, this.startAngle * Math.PI / 180, (this.startAngle + this.avgAngle) * Math.PI / 180);
      this.ctx.lineTo(0, 0);
      this.ctx.fill();
      this.startAngle += this.avgAngle;
    }
  },

  // 画内部扇形区域文字
  drawArcText() {
    this.ctx.textAlign = 'center';
    this.ctx.textBaseLine = 'middle';
    this.ctx.fillStyle = '#EA86A4';
    this.ctx.font = this.padding + 'px sans-serif';
    const textArrays = ['恭喜发财', '华为耳机', '华为手机', '恭喜发财', '华为平板', '华为手表'];
    for (let i = 0; i < this.count; i++) {
      this.drawCircularText(textArrays[i], (this.startAngle + this.avgAngle * 3 / 4) * Math.PI / 180,
        (this.startAngle + this.avgAngle / 4) * Math.PI / 180);
      this.startAngle += this.avgAngle;
    }
  },

  // 绘制圆弧文本
  drawCircularText(textString, startAngle, endAngle) {
    const circleText = {
      x: 0,
      y: 0,
      radius: this.centerX - this.padding * 2
    };
    // 圆的半径
    const radius = circleText.radius - circleText.radius / 5;
    // 每个字母占的弧度
    const angleDecrement = (startAngle - endAngle) / (textString.length - 1);
    let angle = parseFloat(startAngle);
    let index = 0;
    let character;

    while (index < textString.length) {
      character = textString.charAt(index);
      this.ctx.save();
      this.ctx.beginPath();
      this.ctx.translate(circleText.x + Math.cos(angle) * radius, circleText.y - Math.sin(angle) * radius);
      this.ctx.rotate(Math.PI / 2 - angle);
      this.ctx.fillText(character, 0, 0);
      angle -= angleDecrement;
      index++;
      this.ctx.restore();
    }
  },

  // 画内部扇形区域奖品对应的图片
  drawImage() {
    let beginAngle = this.startAngle + this.avgAngle / 2;
    const imageSrc = ['common/images/watch.png', 'common/images/tablet.png', 'common/images/thanks.png',
      'common/images/phone.png', 'common/images/headset.png', 'common/images/thanks.png'];
    const img = new Image();
    for (let i = 0; i < this.count; i++) {
      img.src = imageSrc[i];
      this.ctx.save();
      this.ctx.beginPath();
      this.ctx.rotate(beginAngle * Math.PI / 180);
      this.ctx.drawImage(img, this.centerX / 3, -48 / 2);
      beginAngle += this.avgAngle;
      this.ctx.restore();
    }
  },

  touchStart(event) {
    // 获取屏幕上点击的坐标
    const floatX = event.touches[0].globalX;
    const floatY = event.touches[0].globalY;
    const radius = this.centerX / 7 + this.padding / 2;
    const isScopeX = this.centerX - radius < floatX && this.centerX + radius > floatX;
    const isScopeY = this.centerY - radius < floatY && this.centerY + radius > floatY;
    if (isScopeX && isScopeY && this.playState !== 'running') {
      this.startAnimator();
    }
  },

  // 画中心圆盘和指针
  drawCenter() {
    const nine = 10;
    const centerCtx = this.$element('center').getContext('2d');
    centerCtx.translate(this.centerX, this.centerY);

    // 画大指针
    centerCtx.beginPath();
    centerCtx.fillStyle = '#F6C8D8';
    centerCtx.moveTo(-this.centerX / nine, 0);
    centerCtx.lineTo(this.centerX / nine, 0);
    centerCtx.lineTo(0, -this.centerX / 3);
    centerCtx.closePath();
    centerCtx.fill();

    // 画内部大圆
    centerCtx.beginPath();
    centerCtx.fillStyle = '#F6C8D8';
    centerCtx.arc(0, 0, this.centerX / 7 + this.padding / 2, 0, Math.PI * 2);
    centerCtx.fill();
    // 画内部小圆
    centerCtx.beginPath();
    centerCtx.fillStyle = '#FFFFFF';
    centerCtx.arc(0, 0, this.centerX / 7, 0, Math.PI * 2);
    centerCtx.fill();

    // 画小指针
    centerCtx.beginPath();
    centerCtx.fillStyle = '#FFFFFF';
    centerCtx.moveTo(-this.centerX / 18, 0);
    centerCtx.lineTo(this.centerX / 18, 0);
    centerCtx.lineTo(0, -this.centerX / 3 + this.padding / 2);
    centerCtx.closePath();
    centerCtx.fill();

    // 画中心圆弧文字
    const text = '开始';
    centerCtx.textAlign = 'center';
    centerCtx.fillStyle = '#EA86A4';
    centerCtx.font = this.padding - 10 + 'px sans-serif';
    centerCtx.beginPath();
    centerCtx.fillText(text, 0, this.ctx.measureText(text).width / 5);
  },

  startAnimator() {
    const angle = 270;
    const randomAngle = Math.random() * this.circle;
    this.startAngle = this.circle * 5 - randomAngle + angle;
    if (this.infinite === 0) {
      this.infinite = -1; // 永久旋转
    }
    setTimeout(() => {
      this.infinite = 0;
      this.playState = 'pause';
      this.showPrizeMessage(randomAngle);
    }, 4000);

    this.rotateDegree = this.startAngle;
    this.playState = 'running';
  },

  showPrizeMessage(randomAngle) {
    if (randomAngle >= 0 && randomAngle < this.avgAngle) {
      prompt.showToast({
        message: '恭喜您中了一块华为手表',
        duration: 3000
      });
    } else if (randomAngle >= this.avgAngle && randomAngle < 2 * this.avgAngle) {
      prompt.showToast({
        message: '恭喜您中了一台华为平板',
        duration: 3000
      });
    } else if (randomAngle >= 2 * this.avgAngle && randomAngle < 3 * this.avgAngle) {
      prompt.showToast({
        message: 'sorry，您没有中奖',
        duration: 3000
      });
    } else if (randomAngle >= 3 * this.avgAngle && randomAngle < 4 * this.avgAngle) {
      prompt.showToast({
        message: '恭喜您中了一部华为手机',
        duration: 3000
      });
    } else if (randomAngle >= 4 * this.avgAngle && randomAngle < 5 * this.avgAngle) {
      prompt.showToast({
        message: '恭喜您中了一副华为耳机',
        duration: 3000
      });
    } else if (randomAngle >= 5 * this.avgAngle && randomAngle < 6 * this.avgAngle) {
      prompt.showToast({
        message: 'sorry，您没有中奖',
        duration: 3000
      });
    }
  }
};
