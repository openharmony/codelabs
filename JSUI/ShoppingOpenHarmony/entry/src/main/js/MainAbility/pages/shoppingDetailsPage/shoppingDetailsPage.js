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

import router from '@system.router';
import prompt from '@system.prompt';
import app from '@system.app';

export default {
  data: {
    pageInfo: {
      scoring: '评分',
      sharing: '分享',
      views: '浏览量',
      exit: '退出',
      annualPrice: '年货节 ￥9999',
      productTitle: '【新品】HUAWEI MateBook X Pro 2021款',
      productIntroduction: '13.9英寸全新11代酷睿i7 16G 512G 商务轻薄笔记本 3K触控全面屏 锐炬显卡 多屏协同 翡冷翠',
      marqueeCustomData: '此商品活动中，请尽快购买！',
      shipment: '发货',
      nextDayReach: '次日达',
      select: '选择',
      rewardTo: '配送至：',
      guarantee: '保障',
      guaranteeContent: '假一赔一 - 满66包邮',
      nowSell: '立即抢购',
      inventory: '库存999件',
      selectRewardTime: '选择配送时间：',
      selectRewardCity: '选择配送城市：',
      saleFlash: '抢购中',
      justMoment: '请稍等......',
      softwareScore: '软件评分',
      ratingReason: '评分理由',
      confirm: '确认',
      cancel: '取消',
      ratingPlaceholder: '请输入评分理由(非必填)'
    },
    clearFlag: -1,
    zeroFlag: 0,
    oneFlag: 1,
    twoFlag: 2,
    threeFlag: 3,
    fourFlag: 4,
    textColor: '#FF3536',
    flagNum: 0,
    ratingReason: '',
    ratingNum: 0,
    scrollAmount: 10,
    loop: '-1',
    marqueeDir: 'right',
    interval: null,
    progress: {
      percent: 0,
      secondaryPercent: 0
    },
    swiperList: [1, 2, 3, 4, 5, 6, 7, 8],
    swiperLists: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14],
    newDate: JSON.stringify(new Date()).substring(1, 11),
    cityList: [['中国'], ['湖北省'], ['武汉市', '黄石市', '十堰市', '宜昌市', '襄阳市', '鄂州市', '荆门市'
    , '孝感市', '荆州市', '黄冈市', '咸宁市', '随州市', '恩施', '仙桃市', '潜江市', '天门市', '神农架林区']],
    selectCityList: [['中国'], ['湖北省'], ['武汉市']],
    selectCityString: '',
    contentList: [
      {
        title: '屏幕尺寸',
        content: '13.9英寸'
      }, {
        title: '运行内存',
        content: '16GB'
      }, {
        title: '传播名',
        content: 'HUAWEI MateBook X Pro'
      }, {
        title: '屏幕色彩',
        content: '100% sRGB（典型值）'
      }, {
        title: '电池容量',
        content: '56Wh（额定容量）'
      }, {
        title: '存储容量',
        content: '512GB'
      }, {
        title: '分辨率',
        content: '3000x2000'
      }, {
        title: 'CPU型号',
        content: '第11代英特尔® 酷睿™ i5-1135G7 处理器'
      }, {
        title: 'CPU核数',
        content: '4核'
      }, {
        title: '上市时间',
        content: '2021年1月'
      }
    ]
  },
  onInit() {
  },
  onShow() {
    this.selectCityString = this.selectCityList.join('-');
  },
  selectCity() {
    this.$element('simpleDialog').show();
    prompt.showToast({
      message: '选择'
    });
  },
  selectSafeguard() {
    prompt.showToast({
      message: '保障'
    });
  },
  cancelSchedule() {
    this.$element('simpleDialog').close();
    prompt.showToast({
      message: 'close dialog'
    });
  },
  backPage() {
    router.push({
      uri: 'pages/homepage/homepage'
    });
  },
  buy() {
    const PROGRESSQUARTERS = 25;
    const PROGRESSONEHALF = 50;
    const PROGRESSTHREEQUARTERS = 75;
    const PROGRESSONEPERCENT = 100;
    if (this.flagNum === 0) {
      this.textColor = '#27FF50';
    } else if (this.flagNum === this.oneFlag) {
      this.textColor = '#A774FF';
    } else if (this.flagNum === this.twoFlag) {
      this.textColor = '#FFF244';
    } else {
      this.flagNum = this.clearFlag;
      this.textColor = '#291CFF';
    }
    this.flagNum += this.oneFlag;
    this.$element('simpleDialog1').show();
    const interval = setInterval(() => {
      if ((this.progress.secondaryPercent === PROGRESSQUARTERS || this.progress.secondaryPercent === PROGRESSONEHALF
      || this.progress.secondaryPercent === PROGRESSTHREEQUARTERS) && this.progress.percent !== PROGRESSONEPERCENT) {
        this.progress.percent += this.oneFlag;
        this.progress.secondaryPercent += this.oneFlag;
      } else if (this.progress.secondaryPercent === PROGRESSONEPERCENT
      && this.progress.percent !== PROGRESSONEPERCENT) {
        this.progress.percent += this.oneFlag;
        this.progress.secondaryPercent = this.zeroFlag;
      } else if (this.progress.secondaryPercent !== PROGRESSONEPERCENT) {
        this.progress.secondaryPercent += this.oneFlag;
      } else if (this.progress.percent === PROGRESSONEPERCENT
      && this.progress.secondaryPercent === PROGRESSONEPERCENT) {
        this.$element('simpleDialog').close();
        prompt.showToast({
          message: '抢购成功......'
        });
        clearInterval(interval);
      } else {
        prompt.showToast({
          message: '正在处理中......'
        });
      }
      this.interval = interval;
    }, 30);
  },
  cancelBuyDialog() {
    const PROGRESSONEPERCENT = 100;
    if (this.progress.secondaryPercent === PROGRESSONEPERCENT && this.progress.percent === PROGRESSONEPERCENT) {
      prompt.showToast({
        message: '抢购成功......'
      });
    }
    this.$element('simpleDialog').close();
    this.progress.secondaryPercent = this.zeroFlag;
    this.progress.percent = this.zeroFlag;
    clearInterval(this.interval);
  },
  changeDate(e) {
    this.newDate = e.year + '-' + (e.month + this.oneFlag) + '-' + e.day;
  },
  cancelDate() {
    prompt.showToast({
      message: '取消'
    });
  },
  changeCity(e) {
    this.selectCityList = e.newValue;
    this.selectCityString = e.newValue.join('-');
  },
  onMenuSelected(e) {
    if (e.value === 'Item-3') {
      router.push({
        uri: 'pages/viewsChart/viewsChart'
      });
    } else if (e.value === 'Item-4') {
      prompt.showToast({
        message: '程序退出中......'
      });
      setTimeout(function() {
        app.terminate();
      }, 2000);
    }
  },
  onTextClick() {
    this.$element('apiMenu').show({
      x: 720,
      y: 38
    });
  },
  cancelRatingDialog() {
    this.$element('ratingDialog').close();
    prompt.showToast({
      message: '取消......'
    });
  },
  confirmRatingInfo() {
    this.$element('ratingDialog').close();
    prompt.showToast({
      message: '评分：' + this.ratingNum + '    评论理由：' + this.ratingReason
    });
  },
  ratingChange(e) {
    this.ratingNum = e.rating;
    prompt.showToast({
      message: e.rating
    });
  },
  ratingReasonChange(e) {
    this.ratingReason = e.text;
    prompt.showToast({
      message: JSON.stringify(e.text)
    });
  }
};
