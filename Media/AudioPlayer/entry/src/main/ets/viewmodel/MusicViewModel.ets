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
import Logger from '../common/utils/Logger';
import { MusicItem } from './MusicItem';
import { ResourceManagerUtil } from '../common/utils/ResourceManagerUtil';

/**
 * Log tag.
 */
const TAG = '[MusicViewModel]';

/**
 * Music data.
 */
const MUSIC_DATA: MusicItem[] = [
  {
    id: 0,
    name: 'CLASSIC',
    singer: 'classic',
    lyrics: 'cl_lyrics',
    path: 'classic.wav',
    isVip: false,
    image: $r('app.media.ic_music_icon'),
    rawFileDescriptor: {} as resourceManager.RawFileDescriptor
  },
  {
    id: 1,
    name: 'DYNAMIC',
    singer: 'dynamic',
    lyrics: 'cl_dynamic',
    path: 'dynamic.wav',
    isVip: false,
    image: $r('app.media.ic_music_icon'),
    rawFileDescriptor: {} as resourceManager.RawFileDescriptor
  },
  {
    id: 2,
    name: 'FRESHNESS',
    singer: 'freshness',
    lyrics: 'cl_freshness',
    path: 'freshness.wav',
    isVip: true,
    image: $r('app.media.ic_music_icon'),
    rawFileDescriptor: {} as resourceManager.RawFileDescriptor
  },
  {
    id: 3,
    name: 'LIVELY',
    singer: 'lively',
    lyrics: 'cl_lively',
    path: 'lively.mp3',
    isVip: false,
    image: $r('app.media.ic_music_icon'),
    rawFileDescriptor: {} as resourceManager.RawFileDescriptor
  },
  {
    id: 4,
    name: 'NATURE',
    singer: 'nature',
    lyrics: 'cl_nature',
    path: 'nature.mp3',
    isVip: false,
    image: $r('app.media.ic_music_icon'),
    rawFileDescriptor: {} as resourceManager.RawFileDescriptor
  },
  {
    id: 5,
    name: 'ROMANTIC',
    singer: 'romantic',
    lyrics: 'cl_romantic',
    path: 'romantic.wav',
    isVip: false,
    image: $r('app.media.ic_music_icon'),
    rawFileDescriptor: {} as resourceManager.RawFileDescriptor
  }
];


/**
 * Initialize music data.
 *
 * @param musicDataArray Music data array.
 */
export function initializeMusic(musicDataArray: Array<MusicItem>) {
  if (musicDataArray === undefined) {
    Logger.error(TAG, 'getNextMusic fail,context is empty.');
    return;
  }
  MUSIC_DATA.forEach((item: MusicItem) => {
    ResourceManagerUtil.getSrcPath(item.path, (value: resourceManager.RawFileDescriptor) => {
      item.rawFileDescriptor = value;
      musicDataArray.push(item);
      Logger.info(TAG, 'ResourceManagerUtil: ' + musicDataArray.length);
    })
  })
}