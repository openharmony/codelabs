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

export default class CommonConstants {
  /**
   * Preference instance name.
   */
  static readonly PREFERENCE_INSTANCE_NAME: string = 'myStore';

  /**
   * Preferences note key.
   */
  static readonly PREFERENCE_NOTE_KEY: string = 'noteIdArr';

  /**
   * Add note.
   */
  static readonly ADD_NOTE: string = 'add';

  /**
   * Modify note.
   */
  static readonly MODIFY_NOTE: string = 'modify';

  /**
   * Save dialog.
   */
  static readonly SAVE_DIALOG: string = 'save';

  /**
   * Delete dialog.
   */
  static readonly DELETE_DIALOG: string = 'delete';

  /**
   * Start hour of morning.
   */
  static readonly START_HOUR_OF_MORNING: number = 0;

  /**
   * End hour of morning.
   */
  static readonly END_HOUR_OF_MORNING: number = 12;

  /**
   * Morning of Day.
   */
  static readonly MORNING_OF_DAY: string = 'AM';

  /**
   * Post meridiem.
   */
  static readonly POST_MERIDIEM: string = 'PM';
}