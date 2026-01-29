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
        Text('修改密码')
          .fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 16 })
      }
      .width('100%').height(56).padding({ left: 16 })

      Column() {
        // 原密码输入
        Text('原密码').width('100%').margin({ top: 20, bottom: 8 }).fontColor('#666')
        TextInput({ placeholder: '请输入原密码', text: this.oldPassword })
          .type(InputType.Password)
          .height(50)
          .onChange((v) => this.oldPassword = v)

        // 新密码输入
        Text('新密码').width('100%').margin({ top: 20, bottom: 8 }).fontColor('#666')
        TextInput({ placeholder: '请输入新密码', text: this.newPassword })
          .type(InputType.Password)
          .height(50)
          .onChange((v) => this.newPassword = v)

        // 确认密码输入
        Text('确认新密码').width('100%').margin({ top: 20, bottom: 8 }).fontColor('#666')
        TextInput({ placeholder: '请再次输入新密码', text: this.confirmPassword })
          .type(InputType.Password)
          .height(50)
          .onChange((v) => this.confirmPassword = v)

        // 提交按钮
        Button('确认修改')
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
      promptAction.showToast({ message: '登录状态异常' });
      return;
    }

    // 1. 简单校验
    if (this.oldPassword !== this.currentUser.password) {
      promptAction.showToast({ message: '原密码输入不正确' });
      return;
    }

    if (this.newPassword.length < 6) {
      promptAction.showToast({ message: '新密码不能少于6位' });
      return;
    }

    if (this.newPassword !== this.confirmPassword) {
      promptAction.showToast({ message: '两次新密码输入不一致' });
      return;
    }

    try {
      // 2. 更新内存中的状态（这会自动触发 UI 更新）
      this.currentUser.password = this.newPassword;

      // 3. 同步到数据库持久化
      if (dbManager.user) {
        await dbManager.user.updateUser(this.currentUser);
        promptAction.showToast({ message: '修改成功' });

        // 延迟返回，让用户看到提示
        setTimeout(() => {
          router.back();
        }, 1000);
      }
    } catch (e) {
      promptAction.showToast({ message: '保存失败，请重试' });
      console.error('Update Error: ' + JSON.stringify(e));
    }
  }
}