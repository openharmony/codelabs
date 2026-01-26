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
import router from '@ohos.router';
import { CourseDataService } from '../service/CourseDataService';
import { UserInfo } from '../model/CourseModel';
import picker from '@ohos.file.picker';
import fs from '@ohos.file.fs';
import promptAction from '@ohos.promptAction';

@Entry
@Component
struct EditProfilePage {
  private courseService: CourseDataService = CourseDataService.getInstance();
  @State userName: string = '';
  @State signature: string = '';
  @State currentAvatarColor: string = '#007DFF';
  // 预设头像颜色列表
  private avatarColors: string[] = [
    '#007DFF',
    '#FF6B6B',
    '#4ECDC4',
    '#45B7D1',
    '#96CEB4',
    '#FFEAA7'
  ];

  aboutToAppear() {
    // 从 AppStorage 获取最新数据，确保与个人中心同步
    this.userName = AppStorage.get<string>('userName') || '学生';
    this.signature = AppStorage.get<string>('userSignature') || '认真上课，天天向上！';
    this.currentAvatarColor = AppStorage.get<string>('userAvatar') || '#007DFF';
  }

  private loadUserInfo() {
    // Deprecated: using AppStorage for initialization
    this.courseService.getUserInfo().then((userInfo: UserInfo | null) => {
      if (userInfo) {
        this.userName = userInfo.name || '学生';
        this.signature = userInfo.signature || '认真上课，天天向上！';
        if (userInfo.avatar) {
          this.currentAvatarColor = userInfo.avatar;
        }
      }
    }).catch((error: Error) => {
      console.error('Failed to load user info:', error);
    });
  }

  private saveUserInfo() {
    const userInfo: UserInfo = {
      name: this.userName,
      avatar: this.currentAvatarColor,
      signature: this.signature
    };

    this.courseService.saveUserInfo(userInfo).then(() => {
      promptAction.showToast({ message: '保存成功' });
      setTimeout(() => {
        router.back();
      }, 1000);
    }).catch((error: Error) => {
      console.error('Failed to save user info:', error);
      promptAction.showToast({ message: '保存失败', duration: 2000 });
    });
  }

  private changeAvatarColor(): void {
    // 随机选择一个新的颜色
    let newColor: string;
    do {
      const randomIndex = Math.floor(Math.random() * this.avatarColors.length);
      newColor = this.avatarColors[randomIndex];
    } while (newColor === this.currentAvatarColor && this.avatarColors.length > 1);

    this.currentAvatarColor = newColor;
  }

  private async selectAvatarImage() {
    try {
      const photoSelectOptions = new picker.PhotoSelectOptions();
      photoSelectOptions.MIMEType = picker.PhotoViewMIMETypes.IMAGE_TYPE;
      photoSelectOptions.maxSelectNumber = 1;
      const photoViewPicker = new picker.PhotoViewPicker();
      const photoSelectResult = await photoViewPicker.select(photoSelectOptions);

      if (photoSelectResult && photoSelectResult.photoUris.length > 0) {
        const uri = photoSelectResult.photoUris[0];
        const context = getContext(this);
        const fileDir = context.filesDir;
        const fileName = `avatar_${Date.now()}.jpg`;
        const destPath = `${fileDir}/${fileName}`;

        // 复制文件到沙箱
        const file = fs.openSync(uri, fs.OpenMode.READ_ONLY);
        const destFile = fs.openSync(destPath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE | fs.OpenMode.TRUNC);

        // 读取并写入
        const bufferSize = 4096;
        let buffer = new ArrayBuffer(bufferSize);
        let readLen = fs.readSync(file.fd, buffer);
        while (readLen > 0) {
          fs.writeSync(destFile.fd, buffer, { length: readLen });
          readLen = fs.readSync(file.fd, buffer);
        }

        fs.closeSync(file);
        fs.closeSync(destFile);

        this.currentAvatarColor = `file://${destPath}`;
      }
    } catch (err) {
      console.error('PhotoViewPicker failed', err);
    }
  }

  private showAvatarOptions() {
    promptAction.showActionMenu({
      title: '更换头像',
      buttons: [
        { text: '选择本地图片', color: '#007DFF' },
        { text: '随机颜色头像', color: '#007DFF' }
      ]
    }).then(result => {
      if (result.index === 0) {
        this.selectAvatarImage();
      } else if (result.index === 1) {
        this.changeAvatarColor();
      }
    });
  }

  build() {
    Column() {
      // Header
      Row() {
        Text('←')
          .fontSize(20)
          .fontColor('#333333')
          .width(44)
          .height(44)
          .textAlign(TextAlign.Center)
          .onClick(() => {
            router.back();
          })
        Text('编辑资料')
          .fontSize(20)
          .fontWeight(700)
          .fontColor('#333333')
          .layoutWeight(1)
      }
      .width('100%')
      .padding({
        left: 16,
        right: 16,
        top: 12,
        bottom: 12
      })
      .backgroundColor('#FFFFFF')

      Scroll() {
        Column() {
          // Avatar Section
          Column() {
            Column() {
              Stack() {
                if (this.currentAvatarColor.startsWith('#')) {
                  Circle({ width: 100, height: 100 })
                    .fill(this.currentAvatarColor)
                } else if (this.currentAvatarColor.startsWith('file://') || this.currentAvatarColor.startsWith('/')) {
                  Image(this.currentAvatarColor)
                    .width(100)
                    .height(100)
                    .borderRadius(50)
                    .objectFit(ImageFit.Cover)
                } else {
                  Image($r(`app.media.${this.currentAvatarColor}`))
                    .width(100)
                    .height(100)
                    .borderRadius(50)
                }
                Text('点击更换')
                  .fontSize(14)
                  .fontColor('#FFFFFF')
                  .margin({ top: this.currentAvatarColor.startsWith('#') ? 0 : 30 }) // 图片时稍微下移，或者加背景
              }
            }
            .width(100)
            .height(100)
            .onClick(() => {
              this.showAvatarOptions();
            })
          }
          .width('100%')
          .padding({ top: 32, bottom: 32 })
          .backgroundColor('#FFFFFF')
          .alignItems(HorizontalAlign.Center)
          .margin({ bottom: 16 })

          // User Name Section
          Column() {
            Text('用户名')
              .fontSize(14)
              .fontColor('#666666')
              .margin({ bottom: 8 })

            TextInput({
              text: this.userName,
              placeholder: '请输入用户名'
            })
              .width('100%')
              .fontSize(16)
              .fontColor('#333333')
              .placeholderColor('#CCCCCC')
              .backgroundColor('#F5F5F5')
              .borderRadius(8)
              .padding({
                left: 12,
                right: 12,
                top: 12,
                bottom: 12
              })
              .onChange((value: string) => {
                this.userName = value;
              })
          }
          .width('100%')
          .padding({
            left: 20,
            right: 20,
            top: 20,
            bottom: 20
          })
          .backgroundColor('#FFFFFF')
          .margin({ bottom: 16 })

          // Signature Section
          Column() {
            Text('个性签名')
              .fontSize(14)
              .fontColor('#666666')
              .margin({ bottom: 8 })

            TextArea({
              text: this.signature,
              placeholder: '请输入个性签名'
            })
              .width('100%')
              .height(100)
              .fontSize(16)
              .fontColor('#333333')
              .placeholderColor('#CCCCCC')
              .backgroundColor('#F5F5F5')
              .borderRadius(8)
              .padding({
                left: 12,
                right: 12,
                top: 12,
                bottom: 12
              })
              .onChange((value: string) => {
                this.signature = value;
              })
          }
          .width('100%')
          .padding({
            left: 20,
            right: 20,
            top: 20,
            bottom: 20
          })
          .backgroundColor('#FFFFFF')
          .margin({ bottom: 16 })

          // Save Button
          Button('保存')
            .width('90%')
            .height(48)
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
            .fontColor('#FFFFFF')
            .backgroundColor('#007DFF')
            .borderRadius(24)
            .margin({ bottom: 20 })
            .onClick(() => {
              this.saveUserInfo();
            })
        }
        .width('100%')
        .padding({ left: 16, right: 16, top: 16 })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}
