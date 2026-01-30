import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import { dbManager } from '../utils/DatabaseManager';
import { UserModel } from '../viewmodel/UserModel'; // 确保引入你的用户模型

@Entry
@Component
struct ChangePassword {
  // 1. 直接关联 AppStorage 中的用户信息
  // 如果 currentUser 改变，这里会自动同步
  @StorageLink('currentUser') currentUser: UserModel | null = null

  @State oldPassword: string = ''
  @State newPassword: string = ''
  @State confirmPassword: string = ''

  build() {
    Column() {
      // 顶部导航
      Row() {
        Image($r('app.media.back'))
          .width(24).height(24)
          .onClick(() => router.back())
        Text($r('app.string.title_change_pwd'))
          .fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 16 })
      }
      .width('100%').height(56).padding({ left: 16 })

      Column() {
        // 原密码输入
        Text($r('app.string.label_old_pwd')).width('100%').margin({ top: 20, bottom: 8 }).fontColor('#666')
        TextInput({ placeholder: $r('app.string.hint_enter_old_pwd'), text: this.oldPassword })
          .type(InputType.Password)
          .height(50)
          .onChange((v) => this.oldPassword = v)

        // 新密码输入
        Text($r('app.string.label_new_pwd')).width('100%').margin({ top: 20, bottom: 8 }).fontColor('#666')
        TextInput({ placeholder: $r('app.string.hint_enter_new_pwd'), text: this.newPassword })
          .type(InputType.Password)
          .height(50)
          .onChange((v) => this.newPassword = v)

        // 确认密码输入
        Text($r('app.string.label_confirm_new_pwd')).width('100%').margin({ top: 20, bottom: 8 }).fontColor('#666')
        TextInput({ placeholder: $r('app.string.hint_reenter_new_pwd'), text: this.confirmPassword })
          .type(InputType.Password)
          .height(50)
          .onChange((v) => this.confirmPassword = v)

        // 提交按钮
        Button($r('app.string.btn_confirm_change'))
          .width('100%')
          .height(50)
          .margin({ top: 60 })
          .onClick(() => this.onSubmit())
      }
      .padding(20)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F8F8')
  }

  // --- 核心修改逻辑 ---
  async onSubmit() {
    // 0. 安全检查
    if (!this.currentUser) {
      promptAction.showToast({ message: $r('app.string.toast_login_exception') });
      return;
    }

    // 1. 简单校验
    if (this.oldPassword !== this.currentUser.password) {
      promptAction.showToast({ message: $r('app.string.toast_old_pwd_wrong') });
      return;
    }

    if (this.newPassword.length < 6) {
      promptAction.showToast({ message: $r('app.string.toast_pwd_too_short') });
      return;
    }

    if (this.newPassword !== this.confirmPassword) {
      promptAction.showToast({ message: $r('app.string.toast_pwd_mismatch') });
      return;
    }

    try {
      // 2. 更新内存中的状态（这会自动触发 UI 更新）
      this.currentUser.password = this.newPassword;

      // 3. 同步到数据库持久化
      if (dbManager.user) {
        await dbManager.user.updateUser(this.currentUser);
        promptAction.showToast({ message: $r('app.string.toast_change_success') });

        // 延迟返回，让用户看到提示
        setTimeout(() => {
          router.back();
        }, 1000);
      }
    } catch (e) {
      promptAction.showToast({ message: $r('app.string.toast_save_fail') });
      console.error('Update Error: ' + JSON.stringify(e));
    }
  }
}