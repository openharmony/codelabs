/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
export interface VideoItem {
  id: string;
  title: string;
  source: Resource;
  desc: string
  cover: ResourceStr;
  orientation: 'landscape' | 'portrait'
}

export const VIDEO_DATA: VideoItem[] = [
  {
    id: "1",
    title: '正念呼吸',
    desc: '帮助你快速平静下来，适合睡前或焦虑时。',
    cover: $rawfile('cover1.png'),
    source: $rawfile('video1.mp4'),
    orientation: 'portrait'
  },
  {
    id: "2",
    title: '身体扫描冥想',
    desc: '一步步觉察身体的放松与紧张。',
    cover: $rawfile('cover2.png'),
    source: $rawfile('video2.mp4'),
    orientation: 'portrait'
  },
  {
    id: "3",
    title: '正念行走',
    desc: '在行走中练习正念，关注每一步的感受，提升专注力。',
    cover: $rawfile('cover3.png'),
    source: $rawfile('video3.mp4'),
    orientation: 'portrait'
  },
  {
    id: "4",
    title: '冥想钟声与放松音',
    desc: '适合冥想结束或阶段切换时聆听，帮助平稳过渡。',
    cover: $rawfile('cover4.png'),
    source: $rawfile('video4.mp4'),
    orientation: 'portrait'
  }
]
