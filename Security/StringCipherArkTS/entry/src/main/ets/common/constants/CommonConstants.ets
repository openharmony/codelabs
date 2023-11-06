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

/**
 * Common constants for all features.
 */
export default class CommonConstants {
  /**
   * Input length of the account.
   */
  static readonly INPUT_ACCOUNT_LENGTH: number = 14;

  /**
   *  Left padding of the input box.
   */
  static readonly INPUT_PADDING_LEFT: number = 0;

  /**
   * Register page url.
   */
  static readonly ROUTER_REGISTER_URL: string = 'pages/Register';

  /**
   * Main page url.
   */
  static readonly ROUTER_WELCOME_URL: string = 'pages/Welcome';

  /**
   * Default distance between the prompt and the bottom.
   */
  static readonly PROMPT_BOTTOM: string = '80vp';

  /**
   * The width or height of the component is spread across the parent component.
   */
  static readonly FULL_PARENT: string = '100%';

  /**
   * The width of button.
   */
  static readonly BUTTON_WIDTH: string = '90%';

  /**
   * Encryption/Decryption type.
   */
  static readonly ENCRYPTION_MODE: string = 'AES256';

  /**
   * Encryption/Decryption generator name.
   */
  static readonly GENERATOR_NAME: string = 'AES256|GCM|PKCS5';

  /**
   * Data of key.
   */
  static readonly KEY_DATA: Array<number> = [
    0xba, 0x3d, 0xc2, 0x71, 0x21, 0x1e, 0x30, 0x56,
    0xad, 0x47, 0xfc, 0x5a, 0x46, 0x39, 0xee, 0x7c,
    0x64, 0x0b, 0x88, 0x96, 0xe2, 0xfa, 0x77, 0xbc,
    0x71, 0xe3, 0x0f, 0x0f, 0x9e, 0x3c, 0xe5, 0xf9
  ];

  /**
   * Data of gcm iv.
   */
  static readonly GCM_IV_DATA: Array<number> = [
    0x88, 0x5a, 0xfc, 0x21, 0x30, 0xfa,
    0xfc, 0x0b, 0x96, 0x96, 0xe2, 0xfa
  ];

  /**
   * Data of gcm aad.
   */
  static readonly GCM_AAD_DATA: Array<number> = [
    0xe2, 0x49, 0x42, 0x72, 0x22, 0x1e, 0x64, 0x7c
  ];

  /**
   * Data of gcm tag.
   */
  static readonly GCM_TAG_DATA: Array<number> = [
    0x5a, 0x3d, 0xc2, 0x71, 0x21, 0x1e, 0xe2, 0x56,
    0xfa, 0x51, 0xfc, 0x0b, 0x48, 0x29, 0xee, 0x8c
  ];

  /**
   * Name of dataBase.
   */
  static readonly DATABASE_NAME: string = 'user.db';

  /**
   * Create table Sql.
   */
  static readonly CREATE_TABLE_SQL: string = 'CREATE TABLE IF NOT EXISTS user(' +
    'id INTEGER PRIMARY KEY AUTOINCREMENT, ' +
    'username TEXT NOT NULL, ' +
    'password TEXT NOT NULL, ' +
    'authTag TEXT NOT NULL)';

  /**
   * Table name.
   */
  static readonly TABLE_NAME: string = 'user';

  /**
   * Table columns.
   */
  static readonly TABLE_COLUMNS: Array<string> = ['id', 'username', 'password', 'authTag'];

  /**
   * The user name contains 1 to 14 characters,
   * including letters, digits, and underscores (_), or a combination of these characters.
   */
  static readonly REGEXP_NAME: RegExp = new RegExp(`^[\u4E00-\u9FA5A-Za-z0-9_]{1,14}$`);

  /**
   * The password is 4 - 14 digits, letters, or! @ $#% ^ & * or a combination of them.
   */
  static readonly REGEXP_PASSWORD: RegExp = new RegExp(`^[A-Za-z0-9!@$#%^&*]{4,14}$`);

  /**
   * Duration of prompt.
   */
  static readonly PROMPT_TIME: number = 3000;

  /**
   * Welcome page color transparent.
   */
  static readonly TOP: number = 0.2;

  /**
   *  Welcome page color transparent.
   */
  static readonly MIDDLE: number = 0.5;

  /**
   *  Welcome page color transparent.
   */
  static readonly BOTTOM: number = 1;

  /**
   *  Welcome page color angle.
   */
  static readonly TRANSPARENT_ANGLE: number = 180;
}