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
import { User } from '../common/bean/User';
import { UserTableApi } from '../model/UserTableApi';
import PromptUtil from '../common/utils/PromptUtil';
import AesUtil from '../common/utils/AesUtil';
import CommonConstants from '../common/constants/CommonConstants';
import Logger from '../common/utils/Logger';

@Entry
@Component
struct Index {
  @State username: string = '';
  @State password: string = '';
  @State isLogin: boolean = false;
  private userList: User[] = [];
  private decryptPassword: string = '';
  private userTableApi = new UserTableApi();

  /**
   * Check whether the login button can be clicked.
   */
  isLoginAvailable() {
    this.isLogin = false;
    if (this.username.length > 0 && this.password.length > 0) {
      this.isLogin = true;
    }
  }

  build() {
    Column() {
      Image($r('app.media.ic_logo'))
        .width($r('app.float.logo_image_size'))
        .height($r('app.float.logo_image_size'))
        .margin({
          top: $r('app.float.logo_margin_top'),
          bottom: $r('app.float.logo_margin_bottom')
        })
      Text($r('app.string.login_page'))
        .fontSize($r('app.float.title_text_size'))
        .fontWeight(FontWeight.Medium)
        .fontColor($r('app.color.title_text_color'))
      Text($r('app.string.login_more'))
        .fontSize($r('app.float.normal_text_size'))
        .fontColor($r('app.color.login_text_color'))
        .margin({
          bottom: $r('app.float.login_bottom'),
          top: $r('app.float.login_top')
        })

      Column() {
        TextInput({ placeholder: $r('app.string.account'), text: this.username })
          .maxLength(CommonConstants.INPUT_ACCOUNT_LENGTH)
          .inputStyle()
          .onChange((value: string) => {
            this.username = value;
            this.isLoginAvailable();
          })
        Line().lineStyle()
        TextInput({ placeholder: $r('app.string.password'), text: this.password })
          .type(InputType.Password)
          .inputStyle()
          .onChange((value: string) => {
            this.password = value;
            this.isLoginAvailable();
          })
      }
      .backgroundColor(Color.White)
      .borderRadius($r('app.float.input_border_radius'))
      .padding({
        top: $r('app.float.input_padding_top'),
        bottom: $r('app.float.input_padding_top'),
        left: $r('app.float.input_padding_left'),
        right: $r('app.float.input_padding_left')
      })
      Row() {
        Text($r('app.string.message_login')).blueTextStyle()
        Text($r('app.string.forgot_password')).blueTextStyle()
      }
      .justifyContent(FlexAlign.SpaceBetween)
      .width(CommonConstants.FULL_PARENT)
      .padding({
        top: $r('app.float.forgot_margin_top'),
        left: $r('app.float.forgot_margin_left'),
        right: $r('app.float.forgot_margin_right')
      })
      Button($r('app.string.login'), { type: ButtonType.Capsule })
        .width(CommonConstants.BUTTON_WIDTH)
        .height($r('app.float.login_button_height'))
        .fontSize($r('app.float.normal_text_size'))
        .fontWeight(FontWeight.Medium)
        .backgroundColor(this.isLogin ? $r('app.color.login_button_color') : $r('app.color.button_color_unavailable'))
        .margin({
          top: $r('app.float.button_top'),
          bottom: $r('app.float.button_bottom')
        })
        .enabled(this.isLogin)
        .onClick(() => {
          // Verify the user name and password.
          if (!this.checkUserData()) {
            return;
          }
          // Processes information entered by users.
          this.dealUserData();
        })
      Text($r('app.string.register_account'))
        .fontColor($r('app.color.login_blue_color'))
        .fontSize($r('app.float.normal_text_size'))
        .fontWeight(FontWeight.Medium)
        .onClick(() => {
          router.pushUrl({
            url: CommonConstants.ROUTER_REGISTER_URL
          }).catch(err => {
              Logger.error(`Failed router to register page, code is ${err.code}, message is ${err.message}`);
            });
        })
    }
    .backgroundColor($r('app.color.background'))
    .height(CommonConstants.FULL_PARENT)
    .width(CommonConstants.FULL_PARENT)
    .padding({
      left: $r('app.float.page_padding_hor'),
      right: $r('app.float.page_padding_hor'),
      bottom: $r('app.float.login_page_bottom')
    })
  }

  /**
   * Verify the user name and password.
   *
   * @returns
   */
  checkUserData() {
    // Check whether the account or password is empty.
    if (this.username === '' || this.password === '') {
      PromptUtil.promptMessage($r('app.string.message_input_empty'), CommonConstants.PROMPT_TIME);
      return false;
    }
    // RegExp for matching username.
    let namePattern: RegExp = CommonConstants.REGEXP_NAME;
    if (!namePattern.exec(this.username)) {
      PromptUtil.promptMessage($r('app.string.message_name_format'), CommonConstants.PROMPT_TIME);
      return false;
    }
    // RegExp for matching passwords.
    let passwordPattern: RegExp = CommonConstants.REGEXP_PASSWORD;
    if (!passwordPattern.exec(this.password)) {
      PromptUtil.promptMessage($r('app.string.message_password_format'), CommonConstants.PROMPT_TIME);
      return false;
    }
    return true;
  }

  /**
   * Processes information entered by users.
   */
  async dealUserData() {
    this.userList = await this.userTableApi.queryUserByUsername(this.username);
    if (this.userList.length === 0) {
      PromptUtil.promptMessage($r('app.string.message_user_empty'), CommonConstants.PROMPT_TIME);
      return;
    }
    // Decrypt the password using the string decryption API.
    this.aesDecrypt();
  }

  /**
   * AES decryption.
   */
  aesDecrypt() {
    AesUtil.decrypt(this.userList[0]?.password, this.userList[0]?.authTag).then((data) => {
      this.decryptPassword = data;
      this.login();
    }).catch(err => {
      Logger.error(`AesDecrypt err cause ${JSON.stringify(err)}`);
      PromptUtil.promptMessage($r('app.string.message_sys_err'), CommonConstants.PROMPT_TIME);
    })
  }

  /**
   * Check whether the decrypted password is consistent with the password entered by the user.
   * If yes, the login is successful.
   */
  login() {
    let user = this.userList[0];
    if (this.decryptPassword === this.password) {
      // The login success page is displayed.
      router.replaceUrl({
        url: CommonConstants.ROUTER_WELCOME_URL,
        params: { 'username': user.username }
      }).catch(err => {
        Logger.error(`Failed router to welcome page, code is ${err.code}, message is ${err.message}`);
      });
    } else {
      PromptUtil.promptMessage($r('app.string.message_password_err'), CommonConstants.PROMPT_TIME);
    }
  }
}

@Extend(TextInput) function inputStyle() {
  .placeholderColor($r('app.color.placeholder_color'))
  .height($r('app.float.login_input_height'))
  .fontSize($r('app.float.big_text_size'))
  .backgroundColor(Color.White)
  .width(CommonConstants.FULL_PARENT)
  .padding({ left: CommonConstants.INPUT_PADDING_LEFT })
  .margin({ top: $r('app.float.input_margin_top') })
}

@Extend(Line) function lineStyle() {
  .width(CommonConstants.FULL_PARENT)
  .height($r('app.float.line_height'))
  .backgroundColor($r('app.color.line_color'))
}

@Extend(Text) function blueTextStyle() {
  .fontColor($r('app.color.login_blue_color'))
  .fontSize($r('app.float.small_text_size'))
  .fontWeight(FontWeight.Medium)
}