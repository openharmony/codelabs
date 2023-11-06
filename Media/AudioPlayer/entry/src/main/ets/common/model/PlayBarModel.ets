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

import { MusicItem } from '../../viewmodel/MusicItem';
import { CommonConstants, PlayMode, PlayState } from '../constants/CommonConstants';

@Observed
export class PlayBarModel {
  playValue: number = CommonConstants.START_POSITION;
  playMode: number = PlayMode.LIST_LOOP;
  playStateIcon: Resource = $r('app.media.ic_play');
  playModeIcon: Resource = $r('app.media.ic_list_mode');
  totalValue: number = CommonConstants.TOTAL_VALUE;
  musicItem?: MusicItem;
  playState: number = PlayState.PLAY;
}