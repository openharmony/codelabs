import prompt from '@system.prompt';
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { MediaType } from '../model/media/MediaConstants';
import router from '@system.router';
import { MediaLibView } from '../component/MediaLibView'

@Entry
@Component
struct media_lib {
  build() {
    Column() {
      MediaLibView({
        mediaTypes: [MediaType.VIDEO],
        onItemClick: (item) => {
          router.back({ uri:'pages/video_player',params: { fileId: item.id } })
        }
      })
    }.width('100%').height('100%')
  }
}