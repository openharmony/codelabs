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
export default {
  data: {
    imgUrl: '/common/images/windmill.png',
    animationDuration: 5,
    animationDurationNum: 5000,
    speed: 50,
    minSpeed: 0,
    maxSpeed: 100,
    imageSize: 1,
    size: 50,
    minSize: 0,
    maxSize: 100,
    picSize: '100px',
    picSizeNum: 100
  },
  // 改变转速
  changeValue(e) {
    if (e.mode === 'end' || e.mode === 'click') {
      this.speed = e.value;
      this.animationDurationNum = (e.value == 0) ? 0 : (10000 - e.value * 95);
      this.animationDuration = this.animationDurationNum + 'ms';
    }
  },
  // 改变大小
  changeSize(e) {
    if (e.mode === 'end' || e.mode === 'click') {
      this.size = e.value;
      this.imageSize = this.size / 50 < 0.1 ? 0.1 : this.size / 50;
      this.picSize = this.picSizeNum * this.imageSize + 'px'
    }
  }
};
