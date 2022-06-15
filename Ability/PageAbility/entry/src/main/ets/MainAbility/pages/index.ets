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

import router from '@system.router';
import featureAbility from '@ohos.ability.featureAbility';
import app from '@system.app'

@Entry
@Component
struct Index {
  onBackPress() {
    app.terminate();
    return true;
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('This is ability1 index page')
        .fontSize(25)
        .fontWeight(FontWeight.Medium)
      Button() {
        Text('Click to ability1 second page')
          .fontSize(25)
          .fontWeight(FontWeight.Medium)
          .margin({ left: 20, right: 20 })
          .fontColor(Color.White)
      }
      .padding({ top: 20, left: 20, right: 20, bottom: 20 })
      .margin({ top: 20 })
      .onClick(() => {
        router.push({ uri: 'pages/second' })
      })

      Button() {
        Text('Click to ability2 index page')
          .fontSize(25)
          .margin({ left: 20, right: 20 })
          .fontColor(Color.White)
      }
      .padding({ top: 20, left: 20, right: 20, bottom: 20 })
      .margin({ top: 20 })
      .onClick(() => {
        //启动ability
        let str = {
          "want": {
            "bundleName": "com.example.pageabilityopenh",
            "abilityName": "com.example.pageabilityopenh.MainAbility2",
          },
        };
        featureAbility.startAbility(str)
          .then((data) => {
            console.info('Operation successful. Data: ' + JSON.stringify(data))
          }).catch((error) => {
          console.error('Operation failed. Cause: ' + JSON.stringify(error));
        })
      })

      Button() {
        Text('Click to ability2 second page')
          .fontSize(25)
          .margin({ left: 20, right: 20 })
          .fontColor(Color.White)
      }
      .padding({ top: 20, left: 20, right: 20, bottom: 20 })
      .margin({ top: 20 })
      .onClick(() => {
        //启动ability
        let str = {
          "want": {
            "bundleName": "com.example.pageabilityopenh",
            "abilityName": "com.example.pageabilityopenh.MainAbility2",
            "parameters": {
              url: 'pages/second'
            }
          },
        };
        featureAbility.startAbility(str)
          .then((data) => {
            console.info('Operation successful. Data: ' + JSON.stringify(data))
          }).catch((error) => {
          console.error('Operation failed. Cause: ' + JSON.stringify(error));
        })
      })
    }
    .width('100%')
    .height('100%')
  }
}