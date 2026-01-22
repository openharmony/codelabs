import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import common from '@ohos.app.ability.common';
import RdbHelper from '../database/RdbHelper';
import AppAccount from '../model/AppAccount';

@Entry
@Component
struct AccountInputPage {
  // --- 状态变量 ---
  @State platform: string = '';
  @State account: string = '';
  @State password: string = '';

  // 【新增】密码强度状态
  @State strengthLevel: number = 0; // 0:无, 1:弱, 2:中, 3:强
  @State strengthColor: Color = Color.Gray;
  @State strengthText: string = '';

  // 核心变量
  private currentUserId: number = 0;
  private editAccountId: number = 0;
  private rdbHelper: RdbHelper | null = null;

  aboutToAppear() {
    try {
      let params = router.getParams() as Record<string, Object>;
      if (params) {
        if (params['userId']) this.currentUserId = params['userId'] as number;
        if (params['id']) {
          this.editAccountId = params['id'] as number;
          this.platform = params['platform'] as string;
          this.account = params['account'] as string;
          this.password = params['password'] as string;
          // 初始化时检测一次强度
          this.checkStrength(this.password);
        }
      }
    } catch (e) { console.error(`Get params failed: ${e}`); }

    try {
      let context = getContext(this) as common.UIAbilityContext;
      this.rdbHelper = new RdbHelper(context);
      this.rdbHelper.initRdbStore((isSuccess: boolean) => {});
    } catch (e) { console.error(`Init DB failed: ${e}`); }
  }

  safeToast(msg: string): void {
    try { promptAction.showToast({ message: msg }); } catch (e) {}
  }

  safeBack(): void {
    try { router.back(); } catch (e) {}
  }

  /**
   * 【新增】核心算法：检测密码强度
   */
  checkStrength(pwd: string) {
    if (pwd.length === 0) {
      this.strengthLevel = 0;
      this.strengthText = '';
      return;
    }

    let score = 0;
    // 1. 长度奖励
    if (pwd.length >= 6) score += 1;
    if (pwd.length >= 10) score += 1;

    // 2. 字符类型奖励
    const hasLower = /[a-z]/.test(pwd);
    const hasUpper = /[A-Z]/.test(pwd);
    const hasNum = /[0-9]/.test(pwd);
    const hasSpecial = /[^a-zA-Z0-9]/.test(pwd);

    if (hasLower) score += 1;
    if (hasUpper) score += 1;
    if (hasNum) score += 1;
    if (hasSpecial) score += 2; // 特殊符号权重高

    // 3. 评级逻辑
    if (score < 3) {
      this.strengthLevel = 1;
      this.strengthColor = Color.Red;
      this.strengthText = '弱 (建议加长或混合字符)';
    } else if (score < 5) {
      this.strengthLevel = 2;
      this.strengthColor = Color.Orange;
      this.strengthText = '中 (尚可)';
    } else {
      this.strengthLevel = 3;
      this.strengthColor = 0x00CC66; // 鲜绿色
      this.strengthText = '强 (非常安全)';
    }
  }

  generateRandomPassword(): string {
    const length = 12;
    const charset = 'ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnpqrstuvwxyz23456789!@#$%&*+?';
    let result = '';
    for (let i = 0; i < length; i++) {
      const randomIndex = Math.floor(Math.random() * charset.length);
      result += charset.charAt(randomIndex);
    }
    this.safeToast('已生成随机强密码');
    // 生成后也要检测强度
    this.checkStrength(result);
    return result;
  }

  handleSave() {
    if (this.platform === '' || this.account === '' || this.password === '') {
      this.safeToast('请填写完整信息');
      return;
    }
    if (this.currentUserId === 0) {
      this.safeToast('用户状态异常，请重新登录');
      return;
    }

    let newAccount = new AppAccount(this.platform, this.account, this.password, this.currentUserId);

    if (this.rdbHelper) {
      if (this.editAccountId > 0) {
        newAccount.setId(this.editAccountId);
        this.rdbHelper.updateAppAccount(newAccount).then((rows) => {
          if (rows >= 0) {
            this.safeToast('修改成功');
            setTimeout(() => this.safeBack(), 500);
          }
        });
      } else {
        this.rdbHelper.insertAppAccount(newAccount).then((rowId) => {
          if (rowId > 0) {
            this.safeToast('保存成功');
            setTimeout(() => this.safeBack(), 500);
          }
        });
      }
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('< 返回').fontSize(16).fontColor(Color.Gray).onClick(() => this.safeBack())
        Blank()
        Text(this.editAccountId > 0 ? '编辑账号' : '添加新账号').fontSize(18).fontWeight(FontWeight.Bold)
        Blank()
        Text('      ')
      }
      .width('100%').padding(15).backgroundColor(Color.White)

      Column({ space: 20 }) {
        // 平台
        Column({ space: 8 }) {
          Text('平台名称').fontSize(14).fontColor(Color.Gray).width('100%')
          TextInput({ text: this.platform, placeholder: '例如: 微信 / QQ' })
            .height(50).backgroundColor(0xF5F5F5).borderRadius(10)
            .onChange((val) => this.platform = val)
        }

        // 账号
        Column({ space: 8 }) {
          Text('账号 / 用户名').fontSize(14).fontColor(Color.Gray).width('100%')
          TextInput({ text: this.account, placeholder: '输入您的账号' })
            .height(50).backgroundColor(0xF5F5F5).borderRadius(10)
            .onChange((val) => this.account = val)
        }

        // 密码区域 (UI改造)
        Column({ space: 8 }) {
          Text('密码').fontSize(14).fontColor(Color.Gray).width('100%')

          Row({ space: 10 }) {
            TextInput({ text: this.password, placeholder: '输入密码' })
              .layoutWeight(1).height(50).backgroundColor(0xF5F5F5).borderRadius(10)
              .onChange((val) => {
                this.password = val;
                this.checkStrength(val); // 【核心】实时检测
              })
            Button('🎲 生成')
              .height(50).fontSize(14).backgroundColor(0xF0F8FF).fontColor(0x007DFF).borderRadius(10)
              .onClick(() => {
                this.password = this.generateRandomPassword();
              })
          }.width('100%')

          // 【新增】强度指示条
          if (this.password.length > 0) {
            Row({ space: 5 }) {
              // 三段式进度条
              Row().height(4).layoutWeight(1).borderRadius(2)
                .backgroundColor(this.strengthLevel >= 1 ? this.strengthColor : 0xE0E0E0)
              Row().height(4).layoutWeight(1).borderRadius(2)
                .backgroundColor(this.strengthLevel >= 2 ? this.strengthColor : 0xE0E0E0)
              Row().height(4).layoutWeight(1).borderRadius(2)
                .backgroundColor(this.strengthLevel >= 3 ? this.strengthColor : 0xE0E0E0)
            }.width('100%').margin({ top: 5 })

            // 文字提示
            Text(this.strengthText)
              .fontSize(12)
              .fontColor(this.strengthColor)
              .width('100%')
              .textAlign(TextAlign.End)
          }
        }

        Button('保 存')
          .width('100%').height(50).backgroundColor(0x007DFF).borderRadius(25)
          .fontSize(18).margin({ top: 30 })
          .onClick(() => this.handleSave())
      }
      .padding(20).width('100%').layoutWeight(1).backgroundColor(Color.White)
    }
    .width('100%').height('100%').backgroundColor(Color.White)
  }
}