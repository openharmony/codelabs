/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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


import { ENABLE_LOG } from '../constants/Config'

export enum LogLevel {
  DEBUG,
  INFO,
  WARN,
  ERROR
}

export class Logger {
  static debug(tag: string, message: string): void {
    if (!ENABLE_LOG) return
    console.debug(`[DEBUG][${tag}] ${message}`)
  }

  static info(tag: string, message: string): void {
    if (!ENABLE_LOG) return
    console.info(`[INFO][${tag}] ${message}`)
  }

  static warn(tag: string, message: string): void {
    console.warn(`[WARN][${tag}] ${message}`)
  }

  static error(tag: string, message: string): void {
    console.error(`[ERROR][${tag}] ${message}`)
  }
}
