/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
 
export default class SongInfo {
  private name: string
  private fileUri: string
  private duration: number

  constructor(name, fileUri, duration) {
    this.name = name;
    this.fileUri = fileUri;
    this.duration = duration;
  }

  setName(name: string) {
    this.name = name
  }

  getName() {
    return this.name
  }

  setFileUri(uri: string) {
    this.fileUri = uri
  }

  getFileUri() {
    return this.fileUri
  }

  setDuration(duration: number) {
    this.duration = duration
  }

  getDuration() {
    return this.duration
  }
}