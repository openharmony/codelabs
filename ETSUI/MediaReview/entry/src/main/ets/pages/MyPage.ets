/*
 * Copyright (c) 2026 Beijing Institute of Technology
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

import { router } from '@kit.ArkUI'
import promptAction from '@ohos.promptAction'
import Context from '@ohos.app.ability.common'
import { RdbUtil, UserInfo, UserStats } from '../utils/RdbUtil'

type AuthMode = 'login' | 'register'

@Component
export struct MyPageComp {
  // 我的页：登录/注册/退出 + 用户统计与入口菜单
  @Prop tip: string = ''
  @Prop isPageMode: boolean = false
  @Prop @Watch('onTriggerChange') updateTrigger: number = 0
  @State private backOnSuccess: boolean = false

  @State private userName: string = ''
  @State private userAvatarKey: string = 'user'
  @State private stats: UserStats = {
    reviewCount: 0,
    favCount: 0,
    bookReviewCount: 0,
    movieReviewCount: 0
  }

  @State private mode: AuthMode = 'login'
  @State private name: string = ''
  @State private password: string = ''
  @State private password2: string = ''

  private readonly defaultAvatarKey: string = 'user'

  // 读取路由参数（提示文案、登录成功后是否返回）并加载用户态
  async aboutToAppear(): Promise<void> {
    const params = router.getParams() as Record<string, Object> | undefined
    if (params && params['tip']) {
      this.tip = `${params['tip']}`
    }
    this.backOnSuccess = !!(params && params['backOnSuccess'])
    if (this.tip.length > 0) {
      promptAction.showToast({ message: this.tip })
    }
    await this.loadUser()
  }

  onPageShow(): void {
    void this.loadUser()
  }

  private onTriggerChange() {
    this.loadUser()
  }

  // 从工具层读取当前用户与统计信息
  private async loadUser(): Promise<void> {
    const ctx = getContext(this) as Context
    const u: UserInfo | null = await RdbUtil.getCurrentUser(ctx)
    if (u) {
      this.userName = u.name
      this.userAvatarKey = u.avatarKey
      this.stats = await RdbUtil.getUserStats(ctx, u.name)
      return
    }
    this.userName = ''
    this.userAvatarKey = this.defaultAvatarKey
  }

  private avatarRes(key: string): Resource {
    return $r('app.media.user')
  }

  @Builder navItem(label: string, active: boolean, onClick: () => void) {
    Column() {
      Text(label)
        .fontSize(12)
        .fontWeight(active ? FontWeight.Bold : FontWeight.Medium)
        .fontColor(active ? '#111' : '#7A7A7A')
    }
    .width('50%')
    .padding({ top: 12, bottom: 12 })
    .alignItems(HorizontalAlign.Center)
    .onClick(onClick)
  }

  @Builder statItem(count: string, label: string) {
    Column({ space: 4 }) {
      Text(count)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor('#111')
      Text(label)
        .fontSize(12)
        .fontColor('#999')
    }
    .layoutWeight(1)
    .alignItems(HorizontalAlign.Center)
  }

  @Builder menuItem(title: string, showArrow: boolean = true, onClick?: () => void) {
    Row() {
      Text(title)
        .fontSize(15)
        .fontColor('#333')
        .layoutWeight(1)
      if (showArrow) {
        Text('>')
          .fontSize(14)
          .fontColor('#CCC')
      }
    }
    .padding({ left: 16, right: 16, top: 16, bottom: 16 })
    .backgroundColor('#FFFFFF')
    .onClick(() => {
      if (onClick) {
        onClick()
      } else {
        promptAction.showToast({ message: '功能开发中' })
      }
    })
  }

  @Builder modePill(label: string, active: boolean, onClick: () => void) {
    Text(label)
      .fontSize(13)
      .fontWeight(active ? FontWeight.Bold : FontWeight.Medium)
      .fontColor(active ? '#111' : '#666')
      .padding({ left: 14, right: 14, top: 9, bottom: 9 })
      .borderRadius(999)
      .backgroundColor(active ? '#EDEFF3' : '#FFFFFF')
      .border({ width: 0.5, color: active ? '#E0E3EA' : '#ECECEC' })
      .onClick(onClick)
  }

  // 登录：校验输入后走本地账号体系
  private async doLogin(): Promise<void> {
    const n = this.name.trim()
    if (n.length <= 0) {
      promptAction.showToast({ message: '请输入昵称' })
      return
    }
    if (this.password.length <= 0) {
      promptAction.showToast({ message: '请输入密码' })
      return
    }

    const ctx = getContext(this) as Context
    const u = await RdbUtil.loginUser(ctx, n, this.password)
    if (!u) {
      promptAction.showToast({ message: '登录失败：昵称或密码不正确' })
      return
    }

    this.userName = u.name
    this.userAvatarKey = u.avatarKey
    this.password = ''
    this.password2 = ''
    promptAction.showToast({ message: '登录成功' })
    if (this.backOnSuccess) {
      router.back()
    }
  }

  // 注册：成功后自动登录（或兜底写入当前用户）
  private async doRegister(): Promise<void> {
    const n = this.name.trim()
    if (n.length <= 0) {
      promptAction.showToast({ message: '请输入昵称' })
      return
    }
    if (this.password.length <= 0) {
      promptAction.showToast({ message: '请输入密码' })
      return
    }
    if (this.password !== this.password2) {
      promptAction.showToast({ message: '两次密码不一致' })
      return
    }

    const ctx = getContext(this) as Context
    const ok = await RdbUtil.registerUser(ctx, n, this.password, this.defaultAvatarKey)
    if (!ok) {
      promptAction.showToast({ message: '注册失败：昵称可能已存在' })
      return
    }

    const u = await RdbUtil.loginUser(ctx, n, this.password)
    if (u) {
      this.userName = u.name
      this.userAvatarKey = u.avatarKey
    } else {
      const ok2 = await RdbUtil.setCurrentUser(ctx, n, this.defaultAvatarKey)
      if (!ok2) {
        promptAction.showToast({ message: '注册成功，但自动登录失败' })
        return
      }
      this.userName = n
      this.userAvatarKey = this.defaultAvatarKey
    }
    this.password = ''
    this.password2 = ''
    promptAction.showToast({ message: '注册成功' })
    if (this.backOnSuccess) {
      router.back()
    }
  }

  // 退出：清除当前用户登录态
  private async logout(): Promise<void> {
    const ctx = getContext(this) as Context
    const ok = await RdbUtil.clearCurrentUser(ctx)
    if (ok) {
      this.userName = ''
      this.userAvatarKey = this.defaultAvatarKey
      this.name = ''
      promptAction.showToast({ message: '已退出登录' })
      return
    }
    promptAction.showToast({ message: '退出失败，请重试' })
  }

  build() {
    Column() {
      if (this.isPageMode) {
        Row() {
          Text('‹')
            .fontSize(32)
            .fontWeight(FontWeight.Lighter)
            .fontColor('#111')
            .onClick(() => {
              router.back()
            })
            .padding({ right: 16 })
          
          Text('用户登录')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111')
        }
        .width('100%')
        .padding({ left: 16, right: 16, top: 12, bottom: 12 })
        .backgroundColor('#FFFFFF')
        .alignItems(VerticalAlign.Center)
      }

      Scroll() {
        Column({ space: 12 }) {
          if (this.userName.length > 0) {
            Column({ space: 16 }) {
              Row({ space: 10 }) {
                Image(this.avatarRes(this.userAvatarKey))
                  .width(60)
                  .height(60)
                  .borderRadius(30)
                  .objectFit(ImageFit.Cover)

                Column({ space: 6 }) {
                  Text(this.userName)
                    .fontSize(18)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#111')
                  Text('普通用户')
                    .fontSize(11)
                    .fontColor('#B45309')
                    .padding({ left: 6, right: 6, top: 2, bottom: 2 })
                    .backgroundColor('#FDECC8')
                    .borderRadius(4)
                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start)

                Text('退出')
                  .fontSize(12)
                  .fontColor('#666')
                  .padding({ left: 12, right: 12, top: 6, bottom: 6 })
                  .borderRadius(12)
                  .backgroundColor('#F5F5F5')
                  .onClick(() => {
                    void this.logout()
                  })
              }

              Divider().color('#F5F5F5')
            }
            .padding(20)
            .borderRadius(16)
            .backgroundColor('#FFFFFF')
            .margin({ left: 16, right: 16, top: 10 })

            Column() {
              this.menuItem('我的收藏', true, () => {
                router.pushUrl({ url: 'pages/MyFavorites' })
              })
            }
            .borderRadius(16)
            .backgroundColor('#FFFFFF')
            .margin({ left: 16, right: 16 })

            Column() {
              this.menuItem('设置')
              Divider().color('#F9F9F9').padding({ left: 16 })
              this.menuItem('关于我们')
            }
            .borderRadius(16)
            .backgroundColor('#FFFFFF')
            .margin({ left: 16, right: 16 })

          } else {
            Column({ space: 12 }) {
              Row({ space: 10 }) {
                Text('登录')
                  .fontSize(13)
                  .fontWeight(this.mode === 'login' ? FontWeight.Bold : FontWeight.Medium)
                  .fontColor(this.mode === 'login' ? '#FFF' : '#666')
                  .padding({ left: 14, right: 14, top: 9, bottom: 9 })
                  .borderRadius(999)
                  .backgroundColor(this.mode === 'login' ? '#111' : '#F5F5F5')
                  .onClick(() => {
                    this.mode = 'login'
                    this.name = ''
                    this.password = ''
                    this.password2 = ''
                  })

                Text('注册')
                  .fontSize(13)
                  .fontWeight(this.mode === 'register' ? FontWeight.Bold : FontWeight.Medium)
                  .fontColor(this.mode === 'register' ? '#FFF' : '#666')
                  .padding({ left: 14, right: 14, top: 9, bottom: 9 })
                  .borderRadius(999)
                  .backgroundColor(this.mode === 'register' ? '#111' : '#F5F5F5')
                  .onClick(() => {
                    this.mode = 'register'
                    this.name = ''
                    this.password = ''
                    this.password2 = ''
                  })
              }

              TextInput({ placeholder: '昵称', text: this.name })
                .onChange((v: string) => {
                  this.name = v
                })
                .height(46)
                .padding({ left: 12, right: 12 })
                .borderRadius(12)
                .backgroundColor('#FFFFFF')

              TextInput({ placeholder: '密码', text: this.password })
                .type(InputType.Password)
                .onChange((v: string) => {
                  this.password = v
                })
                .height(46)
                .padding({ left: 12, right: 12 })
                .borderRadius(12)
                .backgroundColor('#FFFFFF')

              if (this.mode === 'register') {
                TextInput({ placeholder: '确认密码', text: this.password2 })
                  .type(InputType.Password)
                  .onChange((v: string) => {
                    this.password2 = v
                  })
                  .height(46)
                  .padding({ left: 12, right: 12 })
                  .borderRadius(12)
                  .backgroundColor('#FFFFFF')
              }

              Button(this.mode === 'login' ? '登录' : '注册并登录')
                .type(ButtonType.Capsule)
                .backgroundColor('#111')
                .fontColor('#FFF')
                .onClick(() => {
                  if (this.mode === 'login') {
                    void this.doLogin()
                    return
                  }
                  void this.doRegister()
                })
                .margin({ top: 6 })
            }
            .padding(16)
            .borderRadius(18)
            .backgroundColor('#FFFFFF')
            .margin({ left: 16, right: 16, top: 10 })
          }

          Blank().height(20)
        }
      }
      .layoutWeight(1)
      .scrollBar(BarState.Auto)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F6F4F1')
  }
}

@Entry
@Component
struct MyPage {
  build() {
    Column() {
      MyPageComp({ tip: '', isPageMode: true })
    }
    .height('100%')
    .width('100%')
  }
}