/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// pages/PasswordEntry.ets
import router from '@ohos.router';
import { PassType,PasswordManager } from '../common/Config';
import promptAction from '@ohos.promptAction';



@Entry
@Component
export struct PasswordEntry {
  @State currentUser: string = '鸿小蒙';

  // 跳转到对应认证页面
  navigateToAuth(type: PassType) {

    // ====== 指纹入口统一拦截 =====
    if (type === PassType.FINGERPRINT) {
      const enabled = PasswordManager.isFingerprintEnabled()

      if (!enabled) {
        // 被禁用：不给进
        promptAction.showToast({
          message: '指纹识别已禁用，请使用其他认证方式'
        })
        return
      }
    }

    let targetPage = '';

    switch (type) {
      case PassType.NUMBER:
        targetPage = 'pages/NumberPassPage';
        break;
      case PassType.GRAPH:
        targetPage = 'pages/GraphPassPage';
        break;
      case PassType.FINGERPRINT:
        targetPage = 'pages/FingerprintPage';
        break;
    }

    // 传递认证类型参数给目标页面
    router.pushUrl({
      url: targetPage,
      params: { authType: type }
    });
  }

  // 进入个人中心
  goToProfile() {
    router.pushUrl({
      url: 'pages/ProfilePage'
    });
  }

  build() {
    Column({ space: 25 }) {
      // 顶部标题栏
      Row({ space: 25 }) {
        Text('👋')
          .fontSize(24)
        // Image($r('app.media.ic_welcome'))
        //   .width(24)
        //   .height(24)

        Column({ space: 5 }) {
          Text(`欢迎回来，${this.currentUser}`)
            .fontSize(18)
            .fontWeight(FontWeight.Bold)

          Text('请选择认证方式登录')
            .fontSize(14)
            .fontColor(Color.Gray)
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)

        // // 设置/个人中心按钮
        // Button() {
        //   Image($r('app.media.ic_setting'))
        //     .width(24)
        //     .height(24)
        // }
        //   .width(40)
        //   .height(40)
        //   .backgroundColor(Color.Transparent)
        //   .onClick(() => this.goToProfile())
      }
      .width('100%')
      .padding({ left: 20, right: 20, top: 24, bottom: 28 })
      .backgroundColor('#f8f9fa')

      // 主内容区域
      Scroll() {
        Column({ space: 25 }) {
          // 数字密码认证卡片
          this.buildAuthCard(
            '数字密码',
            '使用6位数字密码登录',
            PassType.NUMBER,
            '#4CAF50',
            '🔢'// 数字锁图标
          )

          // 图形密码认证卡片
          this.buildAuthCard(
            '图形密码',
            '连接至少4个点的图案',
            PassType.GRAPH,
            '#2196F3',
            '💠'// 图案锁图标
          )

          // 指纹认证卡片
          // this.buildAuthCard(
          //   '指纹识别',
          //   '使用已录入的指纹',
          //   PassType.FINGERPRINT,
          //   '#FF9800',
          //   $r('app.media.ic_up') // 指纹图标
          // )


          // 底部说明
          Text('请根据您的偏好选择认证方式\n系统将记住您的选择')
            .fontSize(12)
            .fontColor(Color.Gray)
            .textAlign(TextAlign.Center)
            .margin({ top: 30 })
        }
        .width('100%')
        .padding(20)
      }
      .scrollable(ScrollDirection.Vertical)
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }

  // 构建认证卡片组件
  // 修改整个 buildAuthCard 方法：
  @Builder
  buildAuthCard(
    title: string,
    description: string,
    type: PassType,
    color: ResourceColor,
    icon: string  // 明确为 string 类型
  ) {
    Column() {
      Row({ space: 15 }) {
        // 左侧图标容器
        Column() {
          Text(icon)
            .fontSize(24)
        }
        .width(60)
        .height(60)
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .backgroundColor(color)
        .opacity(0.7)
        .borderRadius(10)

        // 中间文字描述
        Column({ space: 5 }) {
          Text(title)
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333')

          Text(description)
            .fontSize(14)
            .fontColor(Color.Gray)
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)

        // 右侧箭头
        Text('›')
          .fontSize(24)
          .fontColor(Color.Gray)
          .width(20)
          .height(20)
          .textAlign(TextAlign.Center)
      }
      .width('100%')
      .padding(15)
    }
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(8)
    .shadow({
      radius: 8,
      color: Color.Black,
      offsetX: 0,
      offsetY: 2
    })
    .onClick(() => this.navigateToAuth(type))
    .padding({ left: 12, right: 12 })
    .margin({ top: 8, bottom: 8 })
  }
  getIconRes(type: PassType): string {
    switch (type) {
      case PassType.NUMBER:
        return '🔢'
      case PassType.GRAPH:
        return '💠'
      case PassType.FINGERPRINT:
        return '👆'
      default:
        return '🔢'
    }
  }
}