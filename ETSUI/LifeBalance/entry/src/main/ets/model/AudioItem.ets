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
export interface AudioItem {
  id: string
  title: string
  desc: string
  source: Resource
}

export const AUDIO_DATA: AudioItem[] = [
  {
    id: '1',
    title: '正念呼吸背景音',
    desc: '轻柔的环境音，帮助专注当下。',
    source: $rawfile('mindfulness.wav')
  },
  {
    id: '2',
    title: '冥想钟声',
    desc: '适合结束时的提示音。',
    source: $rawfile('meditation.mp3')
  }
]
