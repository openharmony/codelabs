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
import AesUtil from '../common/utils/AesUtil';
import { User } from '../common/bean/User';
import { UserTableApi } from '../model/UserTableApi';
import CommonConstants from '../common/constants/CommonConstants';
import PromptUtil from '../common/utils/PromptUtil';
import Logger from '../common/utils/Logger';

@Entry
@Component
struct Register {
  @State username: string = '';
  @State password: string = '';
  @State confirmPassword: string = '';
  @State isRegisterAvailable: boolean = false;
  @State isPromptName: boolean = false;
  @State isPromptPassword: boolean = false;
  private userList: User[] = [];
  private userTableApi = new UserTableApi();

  /**
   * Check whether the registration button can be clicked.
   */
  isRegister() {
    this.isRegisterAvailable = false;
    let isAvailable = (this.username.length > 0) && (this.password.length > 0) && (this.confirmPassword.length > 0);
    if (isAvailable) {
      this.isRegisterAvailable = true;
    }
  }

  build() {
    Row() {
      Column() {
        Image($r('app.media.ic_logo'))
          .width($r('app.float.logo_image_size'))
          .height($r('app.float.logo_image_size'))
          .margin({
            top: $r('app.float.logo_margin_top'),
            bottom: $r('app.float.logo_margin_bottom')
          })
        Text($r('app.string.register_page'))
          .fontSize($r('app.float.title_text_size'))
          .fontWeight(FontWeight.Medium)
          .fontColor($r('app.color.title_text_color'))

        Column() {
          TextInput({ placeholder: $r('app.string.register_name') })
            .maxLength(CommonConstants.INPUT_ACCOUNT_LENGTH)
            .inputStyle()
            .onChange((value: string) => {
              this.username = value;
              this.isRegister();
            })
          if (this.isPromptName) {
            Text($r('app.string.message_name_tip'))
              .fontSize($r('app.float.forgot_margin_top'))
              .fontColor(Color.Red)
          }
          Line().lineStyle()
          TextInput({ placeholder: $r('app.string.register_password') })
            .type(InputType.Password)
            .inputStyle()
            .onChange((value: string) => {
              this.password = value;
              this.isRegister();
            })
          if (this.isPromptPassword) {
            Text($r('app.string.message_password_tip'))
              .fontSize($r('app.float.forgot_margin_top'))
              .fontColor(Color.Red)
              .width(CommonConstants.FULL_PARENT)
              .textAlign(TextAlign.Start)
          }
          Line().lineStyle()
          TextInput({ placeholder: $r('app.string.register_confirm_password') })
            .type(InputType.Password)
            .inputStyle()
            .onChange((value: string) => {
              this.confirmPassword = value;
              this.isRegister();
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
        .margin({
          top: $r('app.float.register_margin_top')
        })

        Button($r('app.string.register_button'), { type: ButtonType.Capsule })
          .width(CommonConstants.BUTTON_WIDTH)
          .height($r('app.float.login_button_height'))
          .fontSize($r('app.float.normal_text_size'))
          .fontWeight(FontWeight.Medium)
          .backgroundColor(
            this.isRegisterAvailable ? $r('app.color.login_button_color') : $r('app.color.button_color_unavailable')
          )
          .margin({
            top: $r('app.float.register_button_top'),
            bottom: $r('app.float.button_bottom')
          })
          .enabled(this.isRegisterAvailable)
          .onClick(async () => {
            // Verify the user name, nickname, password, and confirm password.
            if (!this.checkUserData()) {
              return;
            }
            // Registered user information.
            this.registerUser();
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
    .height(CommonConstants.FULL_PARENT)
  }

  /**
   * User name, nickname, password, and confirm password data verification.
   *
   * @returns Check whether the check is passed.
   */
  checkUserData(): boolean {
    if (this.username === '' || this.password === '' || this.confirmPassword === '') {
      PromptUtil.promptMessage($r('app.string.message_register_empty'), CommonConstants.PROMPT_TIME);
      return false;
    }
    // RegExp for matching username.
    let namePattern = CommonConstants.REGEXP_NAME;
    if (!namePattern.exec(this.username)) {
      this.isPromptName = true;
      PromptUtil.promptMessage($r('app.string.message_name_format'), CommonConstants.PROMPT_TIME);
      return false;
    }
    this.isPromptName = false;
    // RegExp for matching passwords.
    let passwordPattern = CommonConstants.REGEXP_PASSWORD;
    if (!passwordPattern.exec(this.password)) {
      this.isPromptPassword = true;
      PromptUtil.promptMessage($r('app.string.message_password_format'), CommonConstants.PROMPT_TIME);
      return false;
    }
    this.isPromptPassword = false;
    // Check whether the password and confirm password are the same.
    if (this.password !== this.confirmPassword) {
      PromptUtil.promptMessage($r('app.string.message_password_prompt'), CommonConstants.PROMPT_TIME);
      return false;
    }
    return true;
  }

  /**
   * Registered user information
   */
  async registerUser() {
    // Check whether the user name has been registered before registration.
    this.userList = await this.userTableApi.queryUserByUsername(this.username);
    if (this.userList.length !== 0) {
      PromptUtil.promptMessage($r('app.string.message_user_exist'), CommonConstants.PROMPT_TIME);
      return;
    }
    this.aesEncrypt();
  }

  /**
   * Encrypts the password using a character string.
   * After the password is encrypted, the encrypted password is stored.
   */
  aesEncrypt() {
    AesUtil.encrypt(this.password).then((data: User) => {
      this.insertUser(data.password, data.authTag);
    }).catch(err => {
      Logger.error(`AesEecrypt err cause ${err}`);
      PromptUtil.promptMessage($r('app.string.message_sys_err'), CommonConstants.PROMPT_TIME)
    });
  }

  /**
   * Save the encrypted user information to the database.
   *
   * @param encryptPassword Ciphertext.
   * @param authTag GCM authTag.
   */
  insertUser(encryptPassword: string, authTag: string) {
    let user = new User(null, this.username, encryptPassword, authTag);
    this.userTableApi.insertUserData(user);
    PromptUtil.promptMessage($r('app.string.message_register_success'), CommonConstants.PROMPT_TIME);
    // After the registration is successful, the login page is displayed.
    router.back();
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