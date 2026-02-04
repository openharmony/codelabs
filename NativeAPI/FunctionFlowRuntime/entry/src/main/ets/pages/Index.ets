/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.All Rights Reserved.
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
import libaki, { FunctionClass, SortClass } from 'libentry.so';

export const sleep = (delay: number = 200) => new Promise<void>((resolve) => setTimeout(resolve, delay));

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize($r('app.float.page_text_font_size'))
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = 'FFRT';
            let fun = new FunctionClass(5);
            let ret1 = fun.FfrtSerialQueue();
            let ret2 = fun.FfrtConcurrentQueue();
            let ret3 = libaki.Add(3, 5);
            let ret4 = libaki.ProcessFfrtQueue();
            let ret5 = libaki.ComputeFfrtQueue();
            let sort = new SortClass(1);
            let ret6 = sort.FfrtConcurrentQueue();

            sleep(10000);
            this.message = ret3.toString();
            console.info("FFRT_FUNCTION FfrtSerialQueue ret: ", ret1);
            console.info("FFRT_FUNCTION FfrtConcurrentQueue ret: ", ret2);
            console.info("FFRT_FUNCTION ProcessFfrtQueue ret: ", ret4);
            console.info("FFRT_FUNCTION ComputeFfrtQueue ret: ", ret5);
            console.info("FFRT_FUNCTION sort FfrtSerialQueue ret: ", ret6);
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
