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

import { VideoItem } from '../../viewmodel/VideoItem'

@Observed
export class HomeTabModel {
  videoList: Array<VideoItem> = [];
  name: string = '';
  src: string = '';
  linkCheck: Resource = $r('app.string.link_check');
  confirmAdd: Resource = $r('app.string.confirm_add');
  loadColor: Resource = $r('app.color.index_tab_selected_font_color');
  confirm: () => void = () => {
  };
  controller?: CustomDialogController;
}