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
import promptAction from '@ohos.promptAction';
import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { UserModel } from '../viewmodel/UserModel';
import { dbManager } from '../utils/DatabaseManager';
import { LocationPickerDialog } from '../components/LocationPickerDialog';

@Entry
@Component
struct EditProfile {
  // 从全局状态获取当前用户，修改这里会直接同步 App 展示
  @StorageLink('currentUser') currentUser: UserModel | null = null
  // 临时状态，用于页面显示，点击保存后再同步给 currentUser
  @State tempNickname: string = ''
  @State tempAvatar: string = ''
  @State tempLocation: string = ''
  // 城市选择弹窗控制器
  dialogController: CustomDialogController = new CustomDialogController({
    builder: LocationPickerDialog({
      confirm: (province: string, city: string) => {
        // 这里必须修改 @State 变量，页面才会立刻变化
        this.tempLocation = `中国 ${province} ${city}`;
        console.info('弹窗选择成功: ' + this.tempLocation);
      }
    }),
    alignment: DialogAlignment.Bottom,
    customStyle: true
  })

  aboutToAppear() {
    if (this.currentUser) {
      this.tempNickname = this.currentUser.nickName;
      this.tempAvatar = this.currentUser.avatarPath || '';
      this.tempLocation = this.currentUser.country || '';
    }
  }

  // 调用系统图库选择图片
  async selectImage() {
    try {
      // 创建图库选择选项
      let photoSelectOptions = new photoAccessHelper.PhotoSelectOptions();
      // 设置媒体类型为图片
      photoSelectOptions.MIMEType = photoAccessHelper.PhotoViewMIMETypes.IMAGE_TYPE;
      // 设置最大选择数量为 1
      photoSelectOptions.maxSelectNumber = 1;

      // 创建图库选择器实例
      let photoPicker = new photoAccessHelper.PhotoViewPicker();

      // 调用 select 方法并处理返回结果
      photoPicker.select(photoSelectOptions).then((photoSelectResult: photoAccessHelper.PhotoSelectResult) => {
        if (photoSelectResult && photoSelectResult.photoUris.length > 0) {
          // 拿到图片的 URI 路径
          this.tempAvatar = photoSelectResult.photoUris[0];
          console.info('PhotoViewPicker.select successfully, Uri: ' + this.tempAvatar);
        }
      }).catch((err: BusinessError) => {
        console.error(`PhotoViewPicker.select failed: Code: ${err.code}, Message: ${err.message}`);
      });

    } catch (err) {
      const error = err as BusinessError;
      console.error('PhotoPicker 异常: ' + error.message);
    }
  }

  build() {
    Column() {
      // --- 顶部导航 ---
      Row() {
        Image($r('app.media.back'))
          .width(24).height(24)
          .onClick(() => router.back())
        Text($r('app.string.title_edit_profile'))
          .fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 16 })
        Blank()
        Text($r('app.string.btn_save'))
          .fontSize(16).fontColor(Color.Blue)
          .onClick(() => this.handleSave())
      }
      .width('100%').height(56).padding({ left: 16, right: 16 })

      // --- 头像编辑区 ---
      Column() {
        Image(this.tempAvatar || $r('app.media.user_default'))
          .width(100)
          .height(100)
          .borderRadius(50)
          .margin({ top: 30, bottom: 10 })
          .objectFit(ImageFit.Cover)

        Text($r('app.string.label_change_avatar'))
          .fontSize(14).fontColor(Color.Blue)
          .onClick(() => this.selectImage())
      }
      .width('100%').alignItems(HorizontalAlign.Center)

      // --- 信息列表区 ---
      List() {
        // 昵称编辑
        ListItem() {
          this.InfoItem($r('app.string.label_nickname'), this.tempNickname, true)
        }

        // 地区选择
        ListItem() {
          // this.InfoItem($r('app.string.label_region'), this.tempLocation, false, () => {
          //   this.dialogController.open()
          // })
          Row() {
            Text($r('app.string.label_region')).width(80).fontSize(16)

            // 直接在这里引用 this.tempLocation，不要通过函数传参
            Text(this.tempLocation || $r('app.string.label_not_set'))
              .layoutWeight(1)
              .textAlign(TextAlign.End)
              .fontColor('#666')

            Image($r('app.media.arrow_right')).width(16).opacity(0.3)
          }
          .width('100%').height(60).padding({ left: 16, right: 16 })
          .onClick(() => {
            this.dialogController.open()
          })
        }
      }
      .margin({ top: 30 })
      .backgroundColor(Color.White)
      .divider({ strokeWidth: 0.5, color: '#f0f0f0', startMargin: 16 })
    }
    .width('100%').height('100%').backgroundColor('#F1F3F5')
  }

  // 封装信息条目
  @Builder
  InfoItem(label: ResourceStr, value: ResourceStr, isInput: boolean, onClick?: () => void) {
    Row() {
      Text(label).width(80).fontSize(16).fontColor('#333')

      if (isInput) {
        TextInput({ text: value })
          .layoutWeight(1)
          .fontColor('#666')
          .textAlign(TextAlign.End)
          .backgroundColor(Color.Transparent)
          .onChange((v) => this.tempNickname = v)
      } else {
        Text(value || $r('app.string.label_not_set'))
          .layoutWeight(1)
          .fontColor('#666')
          .textAlign(TextAlign.End)
          .onClick(() => {
            if (onClick) {
              onClick()
            }
          })
      }

      Image($r('app.media.arrow_right'))
        .width(16).height(16).margin({ left: 8 }).opacity(0.3)
    }
    .width('100%').height(60).padding({ left: 16, right: 16 })
  }

  // --- 保存逻辑 ---
  async handleSave() {
    if (!this.currentUser) {
      return;
    }

    try {
      // 更新内存对象
      this.currentUser.nickName = this.tempNickname;
      this.currentUser.avatarPath = this.tempAvatar;
      this.currentUser.country = this.tempLocation;

      // 持久化到数据库
      if (dbManager.user) {
        const rows = await dbManager.user.updateUser(this.currentUser);
        if (rows > 0) {
          promptAction.showToast({ message: $r('app.string.toast_profile_updated') });
          // 更新 AppStorage 强制刷新全局 UI
          AppStorage.Set('currentUser', this.currentUser);
          setTimeout(() => router.back(), 1000);
        } else {
          promptAction.showToast({ message: $r('app.string.toast_update_fail') });
        }
      }
    } catch (e) {
      console.error('Save Profile Error: ' + JSON.stringify(e));
      promptAction.showToast({ message: $r('app.string.toast_exception') });
    }
  }
}