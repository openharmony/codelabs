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
import promptAction from '@ohos.promptAction';
import { ExportCourseDialog } from '../components/ExportCourseDialog';
import { ImportCourseDialog } from '../components/ImportCourseDialog';

@Component
export struct ProfileView {
  private courseService: CourseDataService = CourseDataService.getInstance();
  @StorageProp('userName') userName: string = '学生';
  @StorageProp('userAvatar') avatarColor: string = 'bit_logo';
  @StorageProp('userSignature') signature: string = '认真上课，天天向上！';
  @Link refreshFlag: number;

  private exportDialogController: CustomDialogController = new CustomDialogController({
    builder: ExportCourseDialog(),
    autoCancel: true,
    customStyle: true,
    alignment: DialogAlignment.Center
  });

  private importDialogController: CustomDialogController = new CustomDialogController({
    builder: ImportCourseDialog(),
    autoCancel: true,
    customStyle: true,
    alignment: DialogAlignment.Center
  });

  aboutToAppear() {
    try {
      const context = getContext(this);
      if (context) {
        this.courseService.init(context).catch((err: Error) => {
          console.error('Failed to init service in ProfileView:', err);
        });
      }
    } catch (error) {
      console.error('Failed to get context in ProfileView:', error);
    }
  }

  @Builder
  MenuItem(title: string, onClick: () => void) {
    Row() {
      Text(title)
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
    .onClick(onClick)
  }

  build() {
    Stack({ alignContent: Alignment.Center }) {
      // Watermark
      Image($r('app.media.peace_dove'))
        .width(200)
        .opacity(0.1)
        .objectFit(ImageFit.Contain)
        .hitTestBehavior(HitTestMode.None)
        .zIndex(100)

      Column() {
        // Header
        Row() {
          Text('个人中心')
          .fontSize(20)
          .fontWeight(700)
          .fontColor('#333333')
          .layoutWeight(1)
          .padding({ left: 12 })
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
              .margin({ right: 16 })

              Column() {
                Text(this.userName)
                  .fontSize(22)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#333333')

                Text(this.signature)
                  .fontSize(14)
                  .fontColor('#999999')
                  .margin({ top: 8 })
                  .maxLines(2)
                  .textOverflow({ overflow: TextOverflow.Ellipsis })
              }
              .layoutWeight(1)
              .alignItems(HorizontalAlign.Start)
              .justifyContent(FlexAlign.Center)
            }
            .width('100%')
          }
          .width('100%')
          .padding(24)
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .shadow({ radius: 8, color: 'rgba(0,0,0,0.05)', offsetY: 2 })

          // Menu Section
          Column() {
            // Edit Profile
            this.MenuItem('编辑资料', () => {
              router.pushUrl({ url: 'pages/EditProfilePage' });
            })
            
            Divider().color('#F5F5F5').margin({ left: 16, right: 16 })

            // Import Course
            this.MenuItem('课表导入', () => {
              this.importDialogController.open();
            })

            Divider().color('#F5F5F5').margin({ left: 16, right: 16 })

            // Export Course
            this.MenuItem('课表导出', () => {
              this.exportDialogController.open();
            })

            Divider().color('#F5F5F5').margin({ left: 16, right: 16 })

            // Clear Data
            this.MenuItem('清空课表', () => {
              promptAction.showDialog({
                title: '确认清空',
                message: '是否确认清空课表？',
                buttons: [
                  { text: '否', color: '#007DFF' },
                  { text: '是', color: '#FF0000' }
                ]
              }).then(async (result) => {
                if (result.index === 1) {
                  await this.courseService.clearAllCourses();
                  this.refreshFlag++;
                  promptAction.showToast({ message: '课表已清空', duration: 2000 });
                }
              });
            })

            Divider().color('#F5F5F5').margin({ left: 16, right: 16 })

            // About
            this.MenuItem('关于', () => {
               router.pushUrl({ url: 'pages/AboutPage' });
            })

            Divider().color('#F5F5F5').margin({ left: 16, right: 16 })

            // Logout
            this.MenuItem('退出登录', async () => {
              promptAction.showDialog({
                title: '提示',
                message: '确定要退出登录吗？',
                buttons: [
                  { text: '取消', color: '#007DFF' },
                  { text: '确定', color: '#FF0000' }
                ]
              }).then(async (result) => {
                if (result.index === 1) {
                  await this.courseService.logout();
                  router.replaceUrl({ url: 'pages/SplashPage' });
                }
              });
            })
          }
          .width('100%')
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ top: 16 })
          .padding({ top: 8, bottom: 8 })
          .shadow({ radius: 8, color: 'rgba(0,0,0,0.05)', offsetY: 2 })
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
    .width('100%')
    .height('100%')
    .alignContent(Alignment.Center)
  }
}
