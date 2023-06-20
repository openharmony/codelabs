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

import CommonConstants from '../constants/CommonConstants';

/**
 * @returns current time plus AM/PM, eg: 12:30 PM
 */
export default function formatTime() {
  let time = new Date();
  let hourTip = '';
  if(time.getHours() >= CommonConstants.START_HOUR_OF_MORNING &&
  time.getHours() < CommonConstants.END_HOUR_OF_MORNING) {
    hourTip = 'AM';
  } else {
    hourTip = 'PM';
  }
  return `${time.toTimeString().split(' ')[0]} ${hourTip}`;
}