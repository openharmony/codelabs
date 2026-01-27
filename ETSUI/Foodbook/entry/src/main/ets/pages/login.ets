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

//login.ets
import { font, Prompt, router, AppStorageV2 } from '@kit.ArkUI';
import prompt from '@system.prompt';
import { text } from '@kit.ArkGraphics2D';

interface user_account {
  username: string;
  password: string;
  phoneNumber: string;
}

const USER_LIST_KEY = 'userList';

const initialUser: user_account = {
  username: 'admin',
  password: '1',
  phoneNumber: '17755102451'
};

@Entry
@Component
struct login {
  @State username: string = "";
  @State password: string = "";
  @State phoneNumber: string = "";
  @State smsCode: string = "";
  @State isSmsLogin: boolean = false;
  @State isForgotPassword: boolean = false;
  @State newPassword: string = "";
  @State confirmPassword: string = "";
  @State countdown: number = 0;
  @State timer: number = 0;
  @State agreeProtocol: boolean = false;
  @State animateShake: boolean = false;
  @State showRedBorder: boolean = false;
  @State showSmsNotification: boolean = false; // 控制短信通知显示
  @State smsNotificationText: string = ""; // 短信内容
  @State generatedCode: string = ""; // 生成的验证码

  aboutToAppear() {
    if (!AppStorage.has(USER_LIST_KEY)) {
      const userList: user_account[] = [];
      userList.push(initialUser);
      AppStorage.setOrCreate(USER_LIST_KEY, userList);
    }
  }

  aboutToDisappear() {
    if (this.timer) {
      clearInterval(this.timer);
    }
  }

  // 生成随机验证码
  generateRandomCode(): string {
    const code = Math.floor(1000 + Math.random() * 9000).toString();
    return code;
  }

  sendSmsCode() {
    if (!this.phoneNumber) {
      prompt.showDialog({
        title: '提示',
        message: '请输入手机号',
        buttons: [{ text: '确定', color: '#007DFF' }]
      });
      return;
    }

    if (!/^1[3-9]\d{9}$/.test(this.phoneNumber)) {
      prompt.showDialog({
        title: '提示',
        message: '手机号格式不正确',
        buttons: [{ text: '确定', color: '#007DFF' }]
      });
      return;
    }

    // 生成验证码
    this.generatedCode = this.generateRandomCode();
    this.smsNotificationText = `【食光记】验证码：${this.generatedCode}，用于${this.isForgotPassword ? '密码重置' : '登录'}，5分钟内有效。`;
    this.showSmsNotification = true;

    // 5秒后自动隐藏
    setTimeout(() => {
      this.showSmsNotification = false;
    }, 6000);

    this.countdown = 60;
    this.timer = setInterval(() => {
      if (this.countdown <= 0) {
        clearInterval(this.timer);
        return;
      }
      this.countdown--;
    }, 1000);
  }

  build() {
    Stack() {
      // 主页面内容
      Column() {
        Column({ space: 10 }) {
          Text().width(110).height(110).borderRadius(30).margin({ top: 30 })
            .backgroundImage($r('app.media.app')).backgroundImageSize(ImageSize.FILL)

          Text(this.isForgotPassword ? '找回密码' :
            this.isSmsLogin ? '短信验证码登录' : '登录界面')
            .fontSize(30)
            .fontWeight(FontWeight.Bold)

          Text(this.isForgotPassword ? '通过手机号找回密码' :
            this.isSmsLogin ? '使用短信验证码登录' : '登录账号以使用更多服务')

          Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
            if (this.isForgotPassword) {
              TextInput({ placeholder: '输入用户名' })
                .width(350)
                .onChange((value: string) => this.username = value)
                .maxLength(15)

              TextInput({ placeholder: '输入手机号' })
                .width(350)
                .type(InputType.Number)
                .onChange((value: string) => this.phoneNumber = value)
                .maxLength(11)

              Row() {
                TextInput({ placeholder: '输入验证码' })
                  .width(220)
                  .type(InputType.Number)
                  .onChange((value: string) => this.smsCode = value)
                  .maxLength(6)

                Button(this.countdown > 0 ? `${this.countdown}s后重发` : '获取验证码',
                  { type: ButtonType.Normal, stateEffect: true })
                  .width(120)
                  .height(40)
                  .fontSize(14)
                  .enabled(this.countdown === 0)
                  .onClick(() => this.sendSmsCode())
              }.width(350).justifyContent(FlexAlign.SpaceBetween)

              TextInput({ placeholder: '输入新密码' })
                .width(350)
                .type(InputType.Password)
                .onChange((value: string) => this.newPassword = value)
                .maxLength(15)

              TextInput({ placeholder: '确认新密码' })
                .width(350)
                .type(InputType.Password)
                .onChange((value: string) => this.confirmPassword = value)
                .maxLength(15)

              Button('重置密码', { type: ButtonType.Capsule, stateEffect: true })
                .fontSize(20).width(300).height(50)
                .onClick(() => this.handleResetPassword())

              Text('返回登录').fontColor(Color.Blue)
                .onClick(() => {
                  this.isForgotPassword = false;
                  this.username = '';
                  this.phoneNumber = '';
                  this.smsCode = '';
                  this.newPassword = '';
                  this.confirmPassword = '';
                })

            } else if (!this.isSmsLogin) {
              TextInput({ placeholder: '输入用户名' })
                .width(350)
                .onChange((value: string) => this.username = value)
                .maxLength(15)

              TextInput({ placeholder: '输入密码' })
                .width(350)
                .type(InputType.Password)
                .onChange((value: string) => this.password = value)
                .maxLength(15)

              Row() {
                Toggle({ type: ToggleType.Checkbox })
                  .selectedColor('#007DFF')
                  .accessibilityChecked(this.agreeProtocol)
                  .onChange((isOn: boolean) =>
                  {this.showRedBorder = !isOn;
                    this.agreeProtocol = isOn})
                  .size({ width: 20, height: 20 })
                Text('我已阅读并同意')
                  .onClick(() => this.agreeProtocol = !this.agreeProtocol)
                  .fontSize(14)
                  .margin({ left: 8 })
                Text('用户协议')
                  .fontSize(14)
                  .fontColor(Color.Blue)
                  .margin({ left: 4 })
                  .onClick(() => {
                    prompt.showDialog({
                      title: '用户协议',
                      message: '食光记 App 用户协议\n' +
                        '版本号：V8.2.8\n' +
                        '生效日期：2025年05月01日\n' +
                        '\n' +
                        '一、协议范围\n' +
                        '1. 适用主体：本协议由"食光记" App（以下简称"本平台"）运营方与用户共同签署，适用于用户对本平台全部功能的使用行为。\n' +
                        '2. 关联服务：用户使用本平台关联的小程序、网页端或第三方合作服务时，同样受本协议约束。\n' +
                        '\n' +
                        '二、账号与注册\n' +
                        '1. 账号注册：\n' +
                        '   - 用户需提供真实有效的手机号码完成注册，平台将通过短信验证码验证身份。\n' +
                        '   - 禁止使用他人身份信息注册，由此引发的法律责任由用户自行承担。\n' +
                        '2. 账号安全：\n' +
                        '   - 用户需妥善保管账号密码，因账号泄露导致的损失由用户承担。\n' +
                        '   - 发现账号异常应立即联系客服，平台将协助处理但不承担连带责任。\n' +
                        '\n' +
                        '三、服务内容与规范\n' +
                        '1. 核心功能：\n' +
                        '   - 提供美食记录、餐厅评价、食谱分享等社交功能\n' +
                        '   - 提供商家合作推广、优惠券发放等增值服务\n' +
                        '2. 用户义务：\n' +
                        '   - 不得发布违法、虚假、侵权或低俗内容（如伪造餐厅评分、传播未经授权的食谱）\n' +
                        '   - 禁止利用平台进行商业营销（如批量发布广告、诱导关注第三方账号）\n' +
                        '   - 尊重他人知识产权，未经授权不得转载或抄袭用户生成内容\n' +
                        '\n' +
                        '四、知识产权与内容管理\n' +
                        '1. 平台权利：\n' +
                        '   - 本平台的软件代码、商标标识、运营数据等知识产权归运营方所有\n' +
                        '   - 用户发布的内容（如照片、文字）视为授权平台在全球范围内免费使用、编辑、推广，但用户保留署名权\n' +
                        '2. 内容处理：\n' +
                        '   - 平台有权审核、删除违规内容，对多次违规用户永久封禁账号\n' +
                        '   - 因用户内容引发的侵权纠纷，由用户承担全部法律责任\n' +
                        '\n' +
                        '五、隐私保护\n' +
                        '1. 信息收集：\n' +
                        '   - 必要信息：注册手机号、设备标识符、地理位置（用于推荐附近餐厅）\n' +
                        '   - 可选信息：相册访问权限（用于上传图片）、麦克风权限（用于语音搜索）\n' +
                        '2. 数据使用：\n' +
                        '   - 仅用于优化服务、个性化推荐及合规统计\n' +
                        '   - 不会向第三方共享个人信息，法律强制要求除外\n' +
                        '3. 儿童保护：\n' +
                        '   - 未满14周岁用户需监护人同意方可使用，平台将采取特殊保护措施\n' +
                        '\n' +
                        '六、免责声明\n' +
                        '1. 服务风险：\n' +
                        '   - 平台不保证服务无中断或数据无丢失，因网络故障、不可抗力等导致的损失不承担责任\n' +
                        '   - 用户因使用平台内容（如餐厅推荐）产生的消费纠纷，由用户与商家自行解决\n' +
                        '2. 第三方链接：\n' +
                        '   - 平台可能包含第三方网站链接，其内容与本平台无关，用户需自行判断风险\n' +
                        '\n' +
                        '七、协议变更与终止\n' +
                        '1. 协议更新：\n' +
                        '   - 平台有权单方面修改协议，更新内容将通过App公告或弹窗通知\n' +
                        '   - 用户继续使用即视为接受新协议，否则应立即停止使用\n' +
                        '2. 终止服务：\n' +
                        '   - 用户可随时注销账号，但已发布内容仍受本协议约束\n' +
                        '   - 平台有权因用户违规永久封禁账号，且不退还已支付费用（如有）\n' +
                        '\n' +
                        '八、争议解决\n' +
                        '1. 法律适用：本协议受中华人民共和国法律管辖\n' +
                        '2. 争议处理：\n' +
                        '   - 双方应先友好协商，协商不成可向运营方所在地人民法院提起诉讼\n' +
                        '\n' +
                        '九、其他条款\n' +
                        '1. 联系方式：\n' +
                        '   - 客服邮箱：support@shiguangji.com\n' +
                        '   - 地址：XX省XX市XX区XX路XX号\n' +
                        '2. 条款独立性：若某条款被认定无效，不影响其他条款的法律效力\n' +
                        '\n' +
                        '十、特别提示\n' +
                        '请用户务必仔细阅读本协议，尤其是加粗条款。如不同意任何内容，请勿注册或使用本平台。',
                      buttons: [{ text: '确定', color: '#007DFF' }]
                    });
                  })
              }
              .width('80%')
              .justifyContent(FlexAlign.Center)
              .margin({ top: 10 })
              .border({
                width: this.showRedBorder ? 2 : 0, // 动态边框宽度
                color: this.showRedBorder ? Color.Red : Color.Transparent // 动态颜色
              })
              .borderRadius(8)
              .padding(8)
              .translate({ x: this.animateShake ? 10 : 0 })
              .animation({
                duration: 10,     //加快动画速度
                iterations: 20,     //增加晃动次数
                curve: Curve.EaseInOut
              })

              Row() {
                Text('短信验证码登录').width('80%').fontColor(Color.Blue)
                  .onClick(() => {
                    this.isSmsLogin = true;
                    this.isForgotPassword = false;
                  })
                Text('忘记密码').width('50%').fontColor(Color.Blue)
                  .onClick(() => {
                    this.isForgotPassword = true;
                    this.isSmsLogin = false;
                  })
              }.justifyContent(FlexAlign.Center)

              Button('登录', { type: ButtonType.Capsule, stateEffect: true })
                .fontSize(20).width(300).height(50)
                .onClick(() => this.handleNormalLogin())
            } else {
              TextInput({ placeholder: '输入手机号' })
                .width(350)
                .type(InputType.Number)
                .onChange((value: string) => this.phoneNumber = value)
                .maxLength(11)

              Row() {
                TextInput({ placeholder: '输入验证码' })
                  .width(220)
                  .type(InputType.Number)
                  .onChange((value: string) => this.smsCode = value)
                  .maxLength(6)

                Button(this.countdown > 0 ? `${this.countdown}s后重发` : '获取验证码',
                  { type: ButtonType.Normal, stateEffect: true })
                  .width(120)
                  .height(40)
                  .fontSize(14)
                  .enabled(this.countdown === 0)
                  .onClick(() => this.sendSmsCode())
              }.width(350).justifyContent(FlexAlign.SpaceBetween)

              Row() {
                Toggle({ type: ToggleType.Checkbox })
                  .accessibilityChecked(this.agreeProtocol)
                  .onChange((isOn: boolean) =>
                  {this.showRedBorder = !isOn;
                    this.agreeProtocol = isOn})
                  .size({ width: 20, height: 20 })
                Text('我已阅读并同意')
                  .onClick(() => {
                    this.agreeProtocol = !this.agreeProtocol;
                    this.showRedBorder = !this.agreeProtocol;
                  })
                  .fontSize(14)
                  .margin({ left: 8 })
                Text('用户协议')
                  .fontSize(14)
                  .fontColor(Color.Blue)
                  .margin({ left: 4 })
                  .onClick(() => {
                    prompt.showDialog({
                      title: '用户协议',
                      message: '食光记 App 用户协议\n' +
                        '版本号：V8.2.8\n' +
                        '生效日期：2025年05月01日\n' +
                        '\n' +
                        '一、协议范围\n' +
                        '1. 适用主体：本协议由"食光记" App（以下简称"本平台"）运营方与用户共同签署，适用于用户对本平台全部功能的使用行为。\n' +
                        '2. 关联服务：用户使用本平台关联的小程序、网页端或第三方合作服务时，同样受本协议约束。\n' +
                        '\n' +
                        '二、账号与注册\n' +
                        '1. 账号注册：\n' +
                        '   - 用户需提供真实有效的手机号码完成注册，平台将通过短信验证码验证身份。\n' +
                        '   - 禁止使用他人身份信息注册，由此引发的法律责任由用户自行承担。\n' +
                        '2. 账号安全：\n' +
                        '   - 用户需妥善保管账号密码，因账号泄露导致的损失由用户承担。\n' +
                        '   - 发现账号异常应立即联系客服，平台将协助处理但不承担连带责任。\n' +
                        '\n' +
                        '三、服务内容与规范\n' +
                        '1. 核心功能：\n' +
                        '   - 提供美食记录、餐厅评价、食谱分享等社交功能\n' +
                        '   - 提供商家合作推广、优惠券发放等增值服务\n' +
                        '2. 用户义务：\n' +
                        '   - 不得发布违法、虚假、侵权或低俗内容（如伪造餐厅评分、传播未经授权的食谱）\n' +
                        '   - 禁止利用平台进行商业营销（如批量发布广告、诱导关注第三方账号）\n' +
                        '   - 尊重他人知识产权，未经授权不得转载或抄袭用户生成内容\n' +
                        '\n' +
                        '四、知识产权与内容管理\n' +
                        '1. 平台权利：\n' +
                        '   - 本平台的软件代码、商标标识、运营数据等知识产权归运营方所有\n' +
                        '   - 用户发布的内容（如照片、文字）视为授权平台在全球范围内免费使用、编辑、推广，但用户保留署名权\n' +
                        '2. 内容处理：\n' +
                        '   - 平台有权审核、删除违规内容，对多次违规用户永久封禁账号\n' +
                        '   - 因用户内容引发的侵权纠纷，由用户承担全部法律责任\n' +
                        '\n' +
                        '五、隐私保护\n' +
                        '1. 信息收集：\n' +
                        '   - 必要信息：注册手机号、设备标识符、地理位置（用于推荐附近餐厅）\n' +
                        '   - 可选信息：相册访问权限（用于上传图片）、麦克风权限（用于语音搜索）\n' +
                        '2. 数据使用：\n' +
                        '   - 仅用于优化服务、个性化推荐及合规统计\n' +
                        '   - 不会向第三方共享个人信息，法律强制要求除外\n' +
                        '3. 儿童保护：\n' +
                        '   - 未满14周岁用户需监护人同意方可使用，平台将采取特殊保护措施\n' +
                        '\n' +
                        '六、免责声明\n' +
                        '1. 服务风险：\n' +
                        '   - 平台不保证服务无中断或数据无丢失，因网络故障、不可抗力等导致的损失不承担责任\n' +
                        '   - 用户因使用平台内容（如餐厅推荐）产生的消费纠纷，由用户与商家自行解决\n' +
                        '2. 第三方链接：\n' +
                        '   - 平台可能包含第三方网站链接，其内容与本平台无关，用户需自行判断风险\n' +
                        '\n' +
                        '七、协议变更与终止\n' +
                        '1. 协议更新：\n' +
                        '   - 平台有权单方面修改协议，更新内容将通过App公告或弹窗通知\n' +
                        '   - 用户继续使用即视为接受新协议，否则应立即停止使用\n' +
                        '2. 终止服务：\n' +
                        '   - 用户可随时注销账号，但已发布内容仍受本协议约束\n' +
                        '   - 平台有权因用户违规永久封禁账号，且不退还已支付费用（如有）\n' +
                        '\n' +
                        '八、争议解决\n' +
                        '1. 法律适用：本协议受中华人民共和国法律管辖\n' +
                        '2. 争议处理：\n' +
                        '   - 双方应先友好协商，协商不成可向运营方所在地人民法院提起诉讼\n' +
                        '\n' +
                        '九、其他条款\n' +
                        '1. 联系方式：\n' +
                        '   - 客服邮箱：support@shiguangji.com\n' +
                        '   - 地址：XX省XX市XX区XX路XX号\n' +
                        '2. 条款独立性：若某条款被认定无效，不影响其他条款的法律效力\n' +
                        '\n' +
                        '十、特别提示\n' +
                        '请用户务必仔细阅读本协议，尤其是加粗条款。如不同意任何内容，请勿注册或使用本平台。',
                      buttons: [{ text: '确定', color: '#007DFF' }]
                    });
                  })
              }
              .width('80%')
              .justifyContent(FlexAlign.Center)
              .margin({ top: 10 })
              .border({
                width: this.showRedBorder ? 2 : 0,
                color: this.showRedBorder ? Color.Red : Color.Transparent
              })
              .borderRadius(8)
              .padding(8)
              .translate({ x: this.animateShake ? 10 : 0 })
              .animation({
                duration: 30,
                iterations: 3,
                curve: Curve.EaseInOut
              })

              Button('登录', { type: ButtonType.Capsule, stateEffect: true })
                .fontSize(20).width(300).height(50)
                .onClick(() => this.handleSmsLogin())

              Text('返回账号密码登录').fontColor(Color.Blue)
                .onClick(() => {
                  this.isSmsLogin = false;
                  this.isForgotPassword = false;
                  this.phoneNumber = '';
                  this.smsCode = '';
                })
            }

            if (!this.isSmsLogin && !this.isForgotPassword) {
              Text('注册账号').fontColor(Color.Blue).height(20)
                .onClick(() => {
                  router.pushUrl({ url: 'pages/register' });
                })
            }
          }
          .height(this.isForgotPassword ? '45%' : (this.isSmsLogin ? '35%' : '30%'))
          .width('95%')
          .backgroundColor(Color.White)
          .margin({ top: 50 })
          .borderRadius(30)
        }
        .width('100%')
        .alignItems(HorizontalAlign.Center)

        // 固定在底部的其他登录方式模块
        Column() {
          Text('其他登录方式').margin({ bottom: 10 })
          Row() {
            Image($r('app.media.qqphoto')).width(55).height(55).borderRadius(20)
            Text().height(50).width(20)
            Image($r('app.media.wxphoto')).width(60).height(55).borderRadius(20)
            Text().height(50).width(20)
            Image($r('app.media.douyinphoto')).width(50).height(50).borderRadius(20)
          }
        }
        .width('100%')
        .margin({ bottom: 30 })
        .alignItems(HorizontalAlign.Center)
      }
      .height('100%')
      .width('100%')
      .backgroundImageSize(ImageSize.FILL)
      .justifyContent(FlexAlign.SpaceBetween)

      // 短信通知 - 使用绝对定位覆盖在页面上方
      if (this.showSmsNotification) {
        Column() {
          // 通知头部
          Row() {
            Image($r('app.media.app')) // 使用应用图标作为短信应用图标
              .width(30)
              .height(30)
              .margin({ right: 10 })
            Column() {
              Text('食光记')
                .fontSize(16)
                .fontColor('#000000')
                .fontWeight(FontWeight.Bold)
              Text('刚刚')
                .fontSize(12)
                .fontColor('#999999')
            }
            .alignItems(HorizontalAlign.Start)
            .layoutWeight(1)
          }
          .width('100%')
          .padding(10)
          .backgroundColor('#FFFFFF')

          // 短信内容气泡
          Column() {
            Text(this.smsNotificationText)
              .fontSize(14)
              .fontColor('#000000')
              .padding(10)
          }
          .width('90%')
          .backgroundColor('#EBF5FF')
          .borderRadius(10)
          .margin({ bottom: 10 })
        }
        .width('90%')
        .borderRadius(15)
        .backgroundColor('#F5F5F5')
        .shadow({ radius: 10, color: '#33000000', offsetX: 0, offsetY: 5 })
        .position({ x: '5%', y: '5%' }) // 绝对定位在页面顶部
        .transition({
          type: TransitionType.Insert,
          opacity: 0,
          translate: { y: -100 }
        })
        .transition({
          type: TransitionType.Delete,
          opacity: 0,
          translate: { y: -100 }
        })
      }
    }
    .height('100%')
    .width('100%')
  }

  private handleNormalLogin() {
    if (!this.agreeProtocol) {
      prompt.showToast({
        message: '请勾选用户协议',
        duration: 2000
      });
      this.showRedBorder = true; // 显示红框
      this.animateShake = true;
      setTimeout(() => {
        this.animateShake = false;
        this.showRedBorder = false; // 3秒后自动隐藏红框
      }, 2000); // 延长动画时间
      return;
    }

    if (!this.username) {
      this.showAlert('用户名为空');
      return;
    }

    if (!this.password) {
      this.showAlert('密码为空');
      return;
    }

    const userList: user_account[] = AppStorage.get(USER_LIST_KEY) || [];
    const foundUser = userList.find(user => user.username === this.username);

    if (!foundUser) {
      this.showAlert('账号不存在');
      return;
    }

    if (foundUser.password !== this.password) {
      this.showAlert('密码错误');
      return;
    }

    router.pushUrl({ url: 'pages/Index' });
  }

  private handleSmsLogin() {
    if (!this.agreeProtocol) {
      prompt.showToast({
        message: '请勾选用户协议！',
        duration: 2000
      });
      this.showRedBorder = true;
      this.animateShake = true;
      setTimeout(() => {
        this.animateShake = false;
        this.showRedBorder = false;
      }, 2000);
      return;
    }

    if (!this.phoneNumber) {
      this.showAlert('请输入手机号');
      return;
    }

    if (!this.smsCode) {
      this.showAlert('请输入验证码');
      return;
    }

    if (this.smsCode !== this.generatedCode) {
      this.showAlert('验证码错误');
      return;
    }

    prompt.showToast({ message: '登录成功', duration: 1000 });
    router.pushUrl({ url: 'pages/Index' });
  }

  private handleResetPassword() {
    if (!this.username) {
      this.showAlert('请输入用户名');
      return;
    }

    if (!this.smsCode) {
      this.showAlert('请输入验证码');
      return;
    }

    if (this.smsCode !== this.generatedCode) {
      this.showAlert('验证码错误');
      return;
    }

    if (this.newPassword !== this.confirmPassword) {
      this.showAlert('两次输入的密码不一致');
      return;
    }

    const userList: user_account[] = AppStorage.get(USER_LIST_KEY) || [];
    const userIndex = userList.findIndex(user => user.username === this.username);

    if (userIndex === -1) {
      this.showAlert('账号不存在');
      return;
    }

    userList[userIndex].password = this.newPassword;
    AppStorage.setOrCreate(USER_LIST_KEY, userList);

    prompt.showToast({ message: '密码重置成功', duration: 1000 });

    this.isForgotPassword = false;
    this.username = '';
    this.phoneNumber = '';
    this.smsCode = '';
    this.newPassword = '';
    this.confirmPassword = '';
  }

  private showAlert(message: string) {
    prompt.showDialog({
      title: '提示',
      message,
      buttons: [{ text: '确定', color: '#007DFF' }]
    });
  }
}