/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

import router from '@ohos.router';
import CommonConstants from '../common/constants/CommonConstants';

class c1 {
  username: string | null = null
}
@Entry
@Component
struct Welcome {
  @State username: string = (router.getParams() as c1).username as string;
  @State message: Resource = $r('app.string.message_login_success');

  build() {
    Row() {
      Column() {
        Text(this.username)
          .fontSize($r('app.float.welcome_text_size'))
          .fontWeight(FontWeight.Bold)
        Text(this.message)
          .fontSize($r('app.float.welcome_text_size'))
          .fontWeight(FontWeight.Bold)
      }
      .width(CommonConstants.FULL_PARENT)
    }
    .height(CommonConstants.FULL_PARENT)
    .linearGradient({
      // Angle of linear gradient
      angle: CommonConstants.TRANSPARENT_ANGLE,
      // Direction of the linear gradient
      direction: GradientDirection.Bottom,
      // Color Description of Linear Gradients
      colors: [
        [$r('app.color.welcome_color_top'), CommonConstants.TOP],
        [$r('app.color.welcome_color_middle'), CommonConstants.MIDDLE],
        [$r('app.color.welcome_color_bottom'), CommonConstants.BOTTOM]
      ]
    })
  }
}