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

import { CommonConstants } from '../constants/CommonConstants';
import { PlayConstants } from '../constants/PlayConstants';

@Observed
export class PlayerModel {
  videoHeight: string = PlayConstants.PLAY_PLAYER_HEIGHT;
  videoWidth: string = CommonConstants.FULL_PERCENT;
  videoMargin: string = PlayConstants.MARGIN_ZERO;
  videoPosition: FlexAlign = FlexAlign.Center;
  playSpeed: number = PlayConstants.PLAY_SPEED;
  volume: number = PlayConstants.VOLUME;
  volumeShow: boolean = PlayConstants.VOLUME_SHOW;
  bright: number = PlayConstants.BRIGHT;
  brightShow: boolean = PlayConstants.BRIGHT_SHOW;
  currentTime: string = PlayConstants.PROGRESS_CURRENT_TIME;
  totalTime: string = PlayConstants.PROGRESS_TOTAL_TIME;
  progressVal: number = PlayConstants.PROGRESS_PROGRESS_VAL;
}