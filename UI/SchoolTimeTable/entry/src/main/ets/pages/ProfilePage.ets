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

@Entry
@Component
struct ProfilePage {
  private courseService: CourseDataService = CourseDataService.getInstance();
  @StorageProp('userName') userName: string = '学生';
  @StorageProp('userAvatar') avatarColor: string = 'bit_logo';
  @StorageProp('userSignature') signature: string = '认真上课，天天向上！';

  aboutToAppear() {
    // Data is synced via AppStorage
  }

  onPageShow() {
    // Data is synced via AppStorage
  }

  private loadUserInfo() {
    // Deprecated: using AppStorage
  }

  build() {
    Column() {
      // Header
      Row() {
        Button() {
          Text('←')
            .fontSize(20)
            .fontColor('#333333')
        }
        .width(40)
        .height(40)
        .backgroundColor('#FFFFFF')
        .onClick(() => {
          router.back();
        })

        Text('个人中心')
          .fontSize(20)
          .fontWeight(700)
          .fontColor('#333333')
          .layoutWeight(1)
      }
      .width('100%')
      .padding(12)
      .backgroundColor('#FFFFFF')

      Scroll() {
        Column() {
          // User Info Card
          Column() {
            Row() {
              Stack() {
                if (this.avatarColor.startsWith('#')) {
                  Circle({ width: 80, height: 80 })
                    .fill(this.avatarColor)
                } else if (this.avatarColor.startsWith('file://') || this.avatarColor.startsWith('/')) {
                  Image(this.avatarColor)
                    .width(80)
                    .height(80)
                    .borderRadius(40)
                    .objectFit(ImageFit.Cover)
                } else {
                  Image($r(`app.media.${this.avatarColor}`))
                    .width(80)
                    .height(80)
                    .borderRadius(40)
                }
              }
              .margin({ right: 12 })

              Column() {
                Text(this.userName)
                  .fontSize(20)
                  .fontWeight(700)
                  .fontColor('#333333')

                Text(this.signature)
                  .fontSize(12)
                  .fontColor('#999999')
                  .margin({ top: 4 })
                  .maxLines(2)
              }
              .layoutWeight(1)
              .alignItems(HorizontalAlign.Start)
            }
            .width('100%')
          }
          .width('100%')
          .padding(20)
          .backgroundColor('#FFFFFF')

          // Menu Section
          Column() {
            // Edit Profile
            Row() {
              Text('编辑资料')
                .fontSize(16)
                .fontColor('#333333')
                .layoutWeight(1)

              Image($r('sys.media.ohos_ic_public_arrow_right'))
                .width(20)
                .height(20)
                .fillColor('#999999')
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .onClick(() => {
              router.pushUrl({ url: 'pages/EditProfilePage' });
            })

            // Clear Data
            Row() {
              Text('清空数据')
                .fontSize(16)
                .fontColor('#333333')
                .layoutWeight(1)

              Image($r('sys.media.ohos_ic_public_arrow_right'))
                .width(20)
                .height(20)
                .fillColor('#999999')
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .onClick(() => {
              console.log('清空数据');
            })

            // Import Course
            Row() {
              Text('课表导入')
                .fontSize(16)
                .fontColor('#333333')
                .layoutWeight(1)

              Image($r('sys.media.ohos_ic_public_arrow_right'))
                .width(20)
                .height(20)
                .fillColor('#999999')
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .onClick(() => {
              console.log('课表导入');
            })

            // Export Course
            Row() {
              Text('课表导出')
                .fontSize(16)
                .fontColor('#333333')
                .layoutWeight(1)

              Image($r('sys.media.ohos_ic_public_arrow_right'))
                .width(20)
                .height(20)
                .fillColor('#999999')
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .onClick(() => {
              console.log('课表导出');
            })

            // Logout
            Row() {
              Text('退出登录')
                .fontSize(16)
                .fontColor('#333333')
                .layoutWeight(1)

              Image($r('sys.media.ohos_ic_public_arrow_right'))
                .width(20)
                .height(20)
                .fillColor('#999999')
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .onClick(() => {
              console.log('退出登录');
            })

            // About
            Row() {
              Text('关于')
                .fontSize(16)
                .fontColor('#333333')
                .layoutWeight(1)

              Image($r('sys.media.ohos_ic_public_arrow_right'))
                .width(20)
                .height(20)
                .fillColor('#999999')
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .onClick(() => {
              router.pushUrl({ url: 'pages/AboutPage' });
            })
          }
          .width('100%')
        }
        .width('100%')
        .padding(16)
        .margin({ top: 16 })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}
