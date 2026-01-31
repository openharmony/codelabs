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
import dataPreferences from '@ohos.data.preferences'; // 【新增】导入首选项模块

// --- 新增：注册弹窗（增加行数并解决输入问题） ---
@CustomDialog
struct RegisterDialog {
  controller: CustomDialogController = new CustomDialogController({ builder: '' })
  @State regUser: string = ''
  @State regPass: string = ''
  @State regPass2: string = ''
  confirm: (u: string, p: string, p2: string) => void = () => {
  }

  build() {
    Column({ space: 15 }) {
      Text('新用户注册').fontSize(20).fontWeight(FontWeight.Bold).margin({ top: 10 })
      TextInput({ placeholder: '请输入账号' }).onChange(v => this.regUser = v).width('90%')
      TextInput({ placeholder: '请输入密码' }).type(InputType.Password).onChange(v => this.regPass = v).width('90%')
      TextInput({ placeholder: '确认密码' }).type(InputType.Password).onChange(v => this.regPass2 = v).width('90%')
      Row({ space: 20 }) {
        Button('取消').onClick(() => this.controller.close()).backgroundColor('#EEE').fontColor('#666').layoutWeight(1)
        Button('确定').onClick(() => {
          this.confirm(this.regUser, this.regPass, this.regPass2)
          this.controller.close()
        }).backgroundColor('#FF9800').layoutWeight(1)
      }.width('90%').margin({ bottom: 10 })
    }.padding(20).backgroundColor(Color.White).borderRadius(20)
  }
}

// --- 新增：找回密码弹窗 ---
@CustomDialog
struct ForgetDialog {
  controller: CustomDialogController = new CustomDialogController({ builder: '' })
  @State user: string = ''
  @State code: string = ''
  @State newP: string = ''
  @State newP2: string = ''
  confirm: (u: string, c: string, p: string, p2: string) => void = () => {
  }

  build() {
    Column({ space: 15 }) {
      Text('重置密码').fontSize(20).fontWeight(FontWeight.Bold).margin({ top: 10 })
      TextInput({ placeholder: '账号', text: this.user }).onChange(v => this.user = v).width('90%')
      TextInput({ placeholder: '验证码 (1234)' }).onChange(v => this.code = v).width('90%')
      TextInput({ placeholder: '新密码' }).type(InputType.Password).onChange(v => this.newP = v).width('90%')
      TextInput({ placeholder: '确认新密码' }).type(InputType.Password).onChange(v => this.newP2 = v).width('90%')
      Row({ space: 20 }) {
        Button('取消').onClick(() => this.controller.close()).backgroundColor('#EEE').fontColor('#666').layoutWeight(1)
        Button('重置').onClick(() => {
          this.confirm(this.user, this.code, this.newP, this.newP2)
          this.controller.close()
        }).backgroundColor('#FF9800').layoutWeight(1)
      }.width('90%').margin({ bottom: 10 })
    }.padding(20).backgroundColor(Color.White).borderRadius(20)
  }
}

type UserAccountMap = Record<string, string>;

@Component
export struct LoginPage {
  onLoginSuccess?: () => void
  @State user: string = ''
  @State pass: string = ''
  @State isRememberPwd: boolean = false
  @State isLoginLoading: boolean = false
  @State loginErrorCount: number = 0
  @State userErrorMsg: string = ''
  @State passErrorMsg: string = ''
  @State pwdLevelText: string = ''
  @State pwdLevelColor: Color = Color.Grey
  @State isShowRegister: boolean = false
  @State regUser: string = ''
  @State regPass: string = ''
  @State regPass2: string = ''
  @State regErrorMsg: string = ''
  @State isShowForgetPwd: boolean = false
  @State forgetUser: string = ''
  @State forgetCode: string = ''
  @State newPwd: string = ''
  @State newPwd2: string = ''
  @State forgetErrorMsg: string = ''
  @State verifyCode: string = '1234'
  @State isPageInited: boolean = false
  @State savedUser: string = 'admin' // 默认账号
  @State savedPass: string = '123456' // 默认密码
  // --- 插入弹窗控制器，关联刚才定义的弹窗 ---
  regDialog: CustomDialogController = new CustomDialogController({
    builder: RegisterDialog({
      confirm: (u, p, p2) => {
        this.regUser = u;
        this.regPass = p;
        this.regPass2 = p2
        this.handleRegister() // 调用你原有的注册逻辑
      }
    }),
    autoCancel: true, alignment: DialogAlignment.Center
  })
  forgetDialog: CustomDialogController = new CustomDialogController({
    builder: ForgetDialog({
      confirm: (u, c, p, p2) => {
        this.forgetUser = u;
        this.forgetCode = c;
        this.newPwd = p;
        this.newPwd2 = p2
        this.handleForgetPwd() // 调用你原有的重置逻辑
      }
    }),
    autoCancel: true, alignment: DialogAlignment.Center
  })

  async aboutToAppear() {
    try {
      await this.loadAccountFromDisk();
      this.initLoginPage()
      console.info('登录页面初始化完成')
    } catch (error) {
      console.error('登录页面初始化异常：' + error)
    }
  }

  aboutToDisappear(): void {
    try {
      this.resetLoginState()
      console.info('登录页面已销毁')
    } catch (error) {
      console.error('登录页面销毁异常：' + error)
    }
  }

  build() {
    Column({ space: 20 }) {
      Text('🐾').fontSize(80).margin({ top: 60 })
      Text('宠物记录簿').fontSize(26).fontWeight(FontWeight.Bold).fontColor('#333')
      Text('您的专属宠物成长记录助手').fontSize(14).fontColor('#999').margin({ bottom: 10 })

      Column({ space: 4 }) {
        TextInput({ placeholder: '账号（支持手机号/邮箱/用户名）', text: this.user })
          .onChange((value: string) => {
            this.user = value
            this.checkAccountFormat(value)
          })
          .onFocus(() => {
            this.userErrorMsg = ''
          })
          .height(50)
          .width('80%')
          .padding({ left: 15, right: 15 })
          .backgroundColor('#F7F8FA')
          .borderRadius(8)
        if (this.userErrorMsg) {
          Text(this.userErrorMsg)
            .fontSize(12)
            .fontColor(Color.Red)
            .width('80%')
            .textAlign(TextAlign.Start)
        }
      }

      Column({ space: 4 }) {
        TextInput({ placeholder: '密码（6-16位，支持数字/字母/特殊字符）', text: this.pass })
          .onChange((value: string) => {
            this.pass = value
            this.checkPasswordFormat(value)
          })
          .onFocus(() => {
            this.passErrorMsg = ''
          })
          .height(50)
          .width('80%')
          .padding({ left: 15, right: 15 })
          .backgroundColor('#F7F8FA')
          .borderRadius(8)
        if (this.passErrorMsg) {
          Text(this.passErrorMsg)
            .fontSize(12)
            .fontColor(Color.Red)
            .width('80%')
            .textAlign(TextAlign.Start)
        } else {
          Text(this.pwdLevelText)
            .fontSize(12)
            .fontColor(this.pwdLevelColor)
            .width('80%')
            .textAlign(TextAlign.Start)
        }
      }

      Row({ space: 0 }) {
        Checkbox({ name: 'remember' })
          .select(this.isRememberPwd)
          .onChange((checked: boolean) => {
            this.isRememberPwd = checked

          })
        Text('记住密码').fontSize(12).fontColor('#666').margin({ left: 5, right: 20 })
      }
      .width('80%')
      .justifyContent(FlexAlign.Start)
      .margin({ top: 5, bottom: 5 })

      Button(this.isLoginLoading ? '登录中...' : '立即进入')
        .width('80%')
        .height(50)
        .backgroundColor('#FF9800')
        .borderRadius(8)
        .fontWeight(FontWeight.Bold) // 组长建议：加粗显得更专业
        .enabled(!this.isLoginLoading)
        .onClick(() => {
          const isUserOk: boolean = this.checkAccountFormat(this.user)
          const isPassOk: boolean = this.checkPasswordFormat(this.pass)

          if (!isUserOk || !isPassOk) {
            AlertDialog.show({
              message: this.userErrorMsg || this.passErrorMsg,
              primaryButton: {
                value: '知道了',
                action: (): void => {
                }
              }
            })
            return
          }

          this.isLoginLoading = true

          setTimeout(async () => {
            let pref = await dataPreferences.getPreferences(getContext(this), 'AccountStore');
            let jsonStr = await pref.get('all_users_json', '{}') as string;

            // 【关键修改点】：同样使用 Record 类型
            let userMap: Record<string, string> = JSON.parse(jsonStr) as Record<string, string>;

            // 1. 保底 admin 逻辑
            if (this.user === 'admin' && this.pass === '123456') {
              this.loginSuccessHandle();
            }
            // 2. 从硬盘全量库里匹配
            // 注意：ArkTS 中推荐使用 Object.keys().includes() 或直接判断
            else if (userMap[this.user] && userMap[this.user] === this.pass) {
              this.loginSuccessHandle();
            } else {
              this.loginFailHandle();
            }
            this.isLoginLoading = false
          }, 800)
        })

      Row({ space: 30 }) {
        Text('忘记密码？')
          .fontSize(12).fontColor('#FF9800')
          .onClick(() => {
            this.forgetDialog.open() // 改为调用控制器
          })
        Text('还没有账号？点击注册')
          .fontSize(12).fontColor('#999')
          .onClick(() => {
            this.regDialog.open() // 改为调用控制器
          })
      }.margin({ top: 10 })

      Text('登录须知：\n1. 测试账号：admin 密码：123456\n2. 注册账号仅做本地缓存，无需真实信息\n3. 记住密码功能可自动保存您的账号信息')
        .fontSize(10)
        .fontColor('#CCCCCC')
        .width('80%')
        .textAlign(TextAlign.Center)
        .margin({ top: 20 })

    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
    .padding({ left: 20, right: 20 })
    // ✅ 修复报错：ItemAlign → HorizontalAlign
    .alignItems(HorizontalAlign.Center)
  }

  private initLoginPage(): void {
    if (this.isRememberPwd) {
      this.user = this.savedUser;
      this.pass = this.savedPass;
    }
    this.isPageInited = true
  }

  private resetLoginState(): void {
    this.userErrorMsg = ''
    this.passErrorMsg = ''
    this.loginErrorCount = 0
    this.isLoginLoading = false
    this.regErrorMsg = ''
    this.forgetErrorMsg = ''
  }

  private async saveAccountToDisk(username: string, password: string) {
    try {
      let pref = await dataPreferences.getPreferences(getContext(this), 'AccountStore');

      // 读取全家桶字符串
      let jsonStr = await pref.get('all_users_json', '{}') as string;

      // 【关键修改点】：解析时指定类型为 Record<string, string>
      let userMap: Record<string, string> = JSON.parse(jsonStr) as Record<string, string>;

      // 往里添人
      userMap[username] = password;

      // 重新打包
      await pref.put('all_users_json', JSON.stringify(userMap));
      await pref.flush();

      console.info(`PetDiary: 账号库更新成功，当前账号数: ${Object.keys(userMap).length}`);
    } catch (err) {
      console.error('PetDiary: 存盘逻辑异常');
    }
  }

  /**
   * 【组长增量代码】：从手机磁盘读取曾经注册过的账号
   */
  private async loadAccountFromDisk() {
    try {
      let pref = await dataPreferences.getPreferences(getContext(this), 'AccountStore');

      // 1. 加载所有注册过的用户库（你原有的逻辑）
      const u = await pref.get('saved_user', 'admin') as string;
      const p = await pref.get('saved_pass', '123456') as string;
      this.savedUser = u;
      this.savedPass = p;

      // 2. 【核心修改】：加载“记住密码”相关的状态
      this.isRememberPwd = await pref.get('is_remember', false) as boolean;

      if (this.isRememberPwd) {
        // 如果上次选了记住密码，把账号密码填回输入框变量
        this.user = await pref.get('last_input_user', '') as string;
        this.pass = await pref.get('last_input_pass', '') as string;
      }

      console.info(`PetDiary: 记忆已唤醒。记住密码：${this.isRememberPwd}，当前账号：${this.user}`);
    } catch (err) {
      console.warn('PetDiary: 读取存档异常');
    }
  }

  private checkAccountFormat(account: string): boolean {
    const trimAccount: string = account.trim()
    this.user = trimAccount
    if (trimAccount === '') {
      this.userErrorMsg = '账号不能为空，请输入您的账号'
      return false
    }
    if (trimAccount.length < 3 || trimAccount.length > 20) {
      this.userErrorMsg = '账号长度需在3-20个字符之间'
      return false
    }
    const reg: RegExp = /^[a-zA-Z0-9_\u4e00-\u9fa5]+$/
    if (!reg.test(trimAccount)) {
      this.userErrorMsg = '账号仅支持汉字、字母、数字和下划线'
      return false
    }
    this.userErrorMsg = ''
    return true
  }

  private checkPasswordFormat(password: string): boolean {
    const trimPwd: string = password.trim()
    this.pass = trimPwd
    if (trimPwd === '') {
      this.passErrorMsg = '密码不能为空，请输入您的密码'
      this.pwdLevelText = ''
      this.pwdLevelColor = Color.Grey
      return false
    }
    if (trimPwd.length < 6 || trimPwd.length > 16) {
      this.passErrorMsg = '密码长度需在6-16个字符之间'
      this.pwdLevelText = ''
      this.pwdLevelColor = Color.Grey
      return false
    }
    this.checkPwdLevel(trimPwd)
    this.passErrorMsg = ''
    return true
  }

  private checkPwdLevel(password: string): void {
    let level: number = 0
    if (/[0-9]/.test(password)) {
      level++
    }
    if (/[a-z]/.test(password)) {
      level++
    }
    if (/[A-Z]/.test(password)) {
      level++
    }
    if (/[^a-zA-Z0-9]/.test(password)) {
      level++
    }

    if (level < 2) {
      this.pwdLevelText = '密码强度：弱'
      this.pwdLevelColor = Color.Red
    } else if (level === 2 || level === 3) {
      this.pwdLevelText = '密码强度：中'
      this.pwdLevelColor = Color.Orange
    } else {
      this.pwdLevelText = '密码强度：强'
      this.pwdLevelColor = Color.Green
    }
  }

  private checkRegisterInfo(): boolean {
    this.regErrorMsg = ''
    if (this.regUser.trim() === '') {
      this.regErrorMsg = '注册账号不能为空'
      return false
    }
    if (!this.checkAccountFormat(this.regUser)) {
      this.regErrorMsg = this.userErrorMsg
      return false
    }
    if (this.regPass.trim() === '') {
      this.regErrorMsg = '注册密码不能为空'
      return false
    }
    if (!this.checkPasswordFormat(this.regPass)) {
      this.regErrorMsg = this.passErrorMsg
      return false
    }
    if (this.regPass !== this.regPass2) {
      this.regErrorMsg = '两次输入的密码不一致，请重新输入'
      return false
    }
    return true
  }

  private checkForgetPwdInfo(): boolean {
    this.forgetErrorMsg = ''
    if (this.forgetUser.trim() === '') {
      this.forgetErrorMsg = '请输入您的账号'
      return false
    }
    if (this.forgetCode.trim() !== this.verifyCode) {
      this.forgetErrorMsg = '验证码错误，正确验证码为：1234'
      return false
    }
    if (this.newPwd.trim() === '') {
      this.forgetErrorMsg = '请输入新密码'
      return false
    }
    if (this.newPwd !== this.newPwd2) {
      this.forgetErrorMsg = '两次输入的新密码不一致'
      return false
    }
    return true
  }

  /**
   * 【新增函数】：专门负责保存登录勾选状态和账号密码
   */
  private async saveLoginStatusToDisk() {
    let pref = await dataPreferences.getPreferences(getContext(this), 'AccountStore');

    // 保存两个勾选框的状态
    await pref.put('is_remember', this.isRememberPwd);

    if (this.isRememberPwd) {
      // 如果勾选了记住密码，把当前输入的这一对存入“快捷填充栏”
      await pref.put('last_input_user', this.user);
      await pref.put('last_input_pass', this.pass);
    } else {
      // 如果没勾选，清理掉硬盘里的快捷填充信息
      await pref.put('last_input_user', '');
      await pref.put('last_input_pass', '');
    }
    await pref.flush();
    console.info('PetDiary: 登录偏好设置已更新到硬盘');
  }

  private loginSuccessHandle(): void {
    // 【核心动作】：在全局存储中锁定当前用户身份
    // 这一行代码就像是在手机内存里挂了一个全局牌子
    AppStorage.setOrCreate('currentUserName', this.user);
    this.saveLoginStatusToDisk();
    const loginTime = new Date().getTime();
    AppStorage.setOrCreate('lastLoginTime', loginTime);
    console.info(`PetDiary: 用户[${this.user}]于${loginTime}登录成功，身份已全局绑定`);

    this.onLoginSuccess?.()
    this.loginErrorCount = 0
  }

  private loginFailHandle(): void {
    this.loginErrorCount++
    this.isLoginLoading = false
    if (this.loginErrorCount >= 3) {
      AlertDialog.show({
        title: '登录失败',
        message: `您已连续输错${this.loginErrorCount}次，账号：admin，密码：123456`,
        primaryButton: {
          value: '知道了',
          action: (): void => {
            this.loginErrorCount = 0
          }
        }
      })
    } else {
      AlertDialog.show({
        message: '账号或密码错误，请重试！',
        primaryButton: {
          value: '知道了',
          action: (): void => {
          }
        }
      })
    }
  }

  private handleRegister(): void {
    if (this.checkRegisterInfo()) {
      // --- 【关键修改】：将注册的信息存入模拟存档 ---
      this.savedUser = this.regUser
      this.savedPass = this.regPass
      this.saveAccountToDisk(this.regUser, this.regPass);
      AlertDialog.show({
        title: '注册成功',
        message: `恭喜，账号【${this.regUser}】已生效！`,
        primaryButton: {
          value: '立即去登录',
          action: (): void => {
            this.isShowRegister = false;
            this.user = this.regUser // 自动填入刚注册的账号
            this.pass = '' // 清空密码框让用户手动输一次
          }
        }
      })
    } else {
      AlertDialog.show({
        message: this.regErrorMsg,
        primaryButton: {
          value: '知道了',
          action: (): void => {
          }
        }
      })
    }
  }

  private handleForgetPwd(): void {
    if (this.checkForgetPwdInfo()) {
      // --- 【关键新增：同步更新内存和硬盘里的密码】 ---
      this.savedPass = this.newPwd; // 更新当前内存变量
      this.saveAccountToDisk(this.forgetUser, this.newPwd); // 调用你写好的存盘函数，把新密码刻进硬盘
      // -------------------------------------------

      AlertDialog.show({
        title: '密码重置成功',
        message: '您的密码已重置成功，请使用新密码登录',
        primaryButton: {
          value: '确定',
          action: (): void => {
            this.isShowForgetPwd = false;
            this.pass = this.newPwd // 自动把新密码填入输入框
          }
        }
      })
    } else {
      AlertDialog.show({
        message: this.forgetErrorMsg,
        primaryButton: {
          value: '知道了',
          action: (): void => {
          }
        }
      })
    }
  }
}