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

/**
 * Song Attributes.
 */
export interface MusicItemProperties {
  /**
   * Identifier.
   */
  id: number;

  /**
   * Music name.
   */
  name: string;

  /**
   * Singer.
   */
  singer: string;

  /**
   * Lyricist.
   */
  lyrics: string;

  /**
   * Resource Path.
   */
  path: string;

  /**
   * Member song or not.
   */
  isVip: boolean;

  /**
   * Song icon.
   */
  image: Resource;
}

/**
 * Song entity class.
 */
export class MusicItem {
  /**
   * Identifier.
   */
  id: number;

  /**
   * Music name.
   */
  name: string;

  /**
   * Singer.
   */
  singer: string;

  /**
   * Lyricist.
   */
  lyrics: string;

  /**
   * Resource Path.
   */
  path: string;

  /**
   * Member song or not.
   */
  isVip: boolean;

  /**
   * Song icon.
   */
  image: Resource;

  /**
   * Resource descriptor.
   */
  rawFileDescriptor: resourceManager.RawFileDescriptor;

  constructor(musicItemProperties: MusicItemProperties, rawFileDescriptor: resourceManager.RawFileDescriptor) {
    this.id = musicItemProperties.id;
    this.name = musicItemProperties.name;
    this.singer = musicItemProperties.singer;
    this.lyrics = musicItemProperties.lyrics;
    this.path = musicItemProperties.path;
    this.isVip = musicItemProperties.isVip;
    this.image = musicItemProperties.image;
    this.rawFileDescriptor = rawFileDescriptor;
  }
}