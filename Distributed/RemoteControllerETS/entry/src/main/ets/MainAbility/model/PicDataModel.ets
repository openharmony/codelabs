/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { PicData } from './PicData'

const PicsDataList: any[] = [
  {'name': 'Castle', 'image': $rawfile('castle.png'), 'video': $rawfile('example.mp4')},
  {'name': 'Ducks', 'image': $rawfile('duck.png'), 'video': $rawfile('example.mp4')},
  {'name': 'Snow', 'image': $rawfile('snow.png'), 'video': $rawfile('example.mp4')},
  {'name': 'Sunny', 'image': $rawfile('sunny.png'), 'video': $rawfile('example.mp4')},
  {'name': 'Sunset', 'image': $rawfile('sunset.png'), 'video': $rawfile('example.mp4')},
  {'name': 'Cloudy', 'image': $rawfile('cloudy.png'), 'video': $rawfile('example.mp4')}
]

export function initializeOnStartup(): Array<PicData> {
  let PicDataArray: Array<PicData> = []
  PicsDataList.forEach(item => {
    PicDataArray.push(new PicData(item.name, item.image, item.video));
  })
  return PicDataArray;
}