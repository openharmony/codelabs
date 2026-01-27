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
// pages/ProfilePage.ets
import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import {
  PassType,
  PasswordManager,
  RouterUtil
} from '../common/Config';
import common from '@ohos.app.ability.common';
import { Note, NoteManager } from '../common/NoteManager';


interface UserInfo {
  username: string;
  avatar: Resource;
  lastLogin: string;
  loginMethod: PassType;
}

interface AuthMethodConfig {
  type: PassType;
  name: string;
  icon: Resource;
  color: string;
  enabled: boolean;
  lastUsed: string;
}

@Entry
@Component
struct ProfilePage {
  @State oldPassword: string = '';
  @State newPassword: string = '';
  @State confirmPassword: string = '';
  @State showPassword: boolean = false;
  @State selectedAuthType: PassType | null = null;
  @State isSaving: boolean = false;

  // ===== 忘记密码相关状态 =====
  @State showResetDialog: boolean = false
  @State resetPassword: string = ''
  @State resetConfirmPassword: string = ''
  @State isResetting: boolean = false

  // 忘记密码时用于验证的密码
  @State verifyPassword: string = ''
  @State verifyAuthType: PassType | null = null

  @State currentMethod: AuthMethodConfig = {
    type: PassType.NUMBER,
    name: '',
    icon: $r('app.media.avatar_default'),
    color: '',
    enabled: true,
    lastUsed: ''
  };

  @State userInfo: UserInfo = {
    username: '鸿小蒙',
    avatar: $r('app.media.avatar_default'),
    lastLogin: this.getNowDateTime(),
    loginMethod: PassType.NUMBER
  };

  @State authMethods: AuthMethodConfig[] = [
    {
      type: PassType.NUMBER,
      name: '数字密码',
      icon: $r('app.media.ic_number'),
      color: '#4CAF50',
      enabled: true,
      lastUsed: '今天 14:30'
    },
    {
      type: PassType.GRAPH,
      name: '图形密码',
      icon: $r('app.media.ic_pattern'),
      color: '#2196F3',
      enabled: true,
      lastUsed: '昨天 09:15'
    },
    {
      type: PassType.FINGERPRINT,
      name: '指纹识别',
      icon: $r('app.media.ic_up'),
      color: '#FF9800',
      enabled: true,
      lastUsed: this.getNowDateTime()
    }
  ];

  // 便签相关状态
  @State notes: Note[] = [];
  @State showNoteDialog: boolean = false;

  // 便签搜索与编辑状态
  @State searchKeyword: string = '';
  @State currentNoteId: string = '';
  @State currentNoteTitle: string = '';
  @State currentNoteContent: string = '';
  @State currentNotePinned: boolean = false;

  private validatePasswordFormat(type: PassType, password: string): boolean {
    if (type === PassType.NUMBER) {
      // 6 位纯数字
      return /^\d{6}$/.test(password);
    }

    if (type === PassType.GRAPH) {
      // 只能是 1~9
      if (!/^[1-9]+$/.test(password)) {
        return false;
      }

      // 至少 4 个点
      if (password.length < 4) {
        return false;
      }

      // 所有点必须唯一
      const uniquePoints = new Set(password.split(''));
      return uniquePoints.size === password.length;
    }

    return true;
  }



  private getNowDateTime(): string {
    const now = new Date()

    const year = now.getFullYear()
    const month = (now.getMonth() + 1).toString().padStart(2, '0')
    const day = now.getDate().toString().padStart(2, '0')
    const hour = now.getHours().toString().padStart(2, '0')
    const minute = now.getMinutes().toString().padStart(2, '0')

    return `${year}-${month}-${day} ${hour}:${minute}`
  }


  // 弹窗控制器定义
  dialogController: CustomDialogController | null = new CustomDialogController({
    builder: ChangePasswordDialog({
      authMethod: this.currentMethod,
      oldPass: $oldPassword,
      newPass: $newPassword,
      confirmPass: $confirmPassword,
      showPass: $showPassword,
      onConfirm: () => {
        this.saveNewPassword();
      }
    }),
    autoCancel: true,
    alignment: DialogAlignment.Center,
    customStyle: true
  });


  resetDialogController: CustomDialogController | null =
    new CustomDialogController({
      builder: ResetPasswordDialog({
        authMethod: this.currentMethod,
        verifyAuthType: this.verifyAuthType!,
        verifyPass: $verifyPassword,
        newPass: $resetPassword,
        confirmPass: $resetConfirmPassword,
        onConfirm: () => {
          this.resetPasswordDirectly()
        }
      }),
      autoCancel: true,
      alignment: DialogAlignment.Center,
      customStyle: true
    });

  aboutToAppear() {
    // 1. 初始化便签管理器
    const context = getContext(this) as common.UIAbilityContext;
    NoteManager.init(context);
    // 2. 加载便签数据
    this.loadNotes();
  }

  // 加载便签（支持搜索）
  async loadNotes() {
    if (this.searchKeyword) {
      this.notes = await NoteManager.searchNotes(this.searchKeyword);
    } else {
      this.notes = await NoteManager.getNotes();
    }
  }

  // 打开便签弹窗（新增或编辑）
  openNoteDialog(note?: Note) {
    if (note) {
      this.currentNoteId = note.id;
      this.currentNoteTitle = note.title;
      this.currentNoteContent = note.content;
      this.currentNotePinned = note.isPinned;
    } else {
      this.currentNoteId = '';
      this.currentNoteTitle = '';
      this.currentNoteContent = '';
      this.currentNotePinned = false;
    }
    this.showNoteDialog = true;
  }

  // 保存便签
  async handleSaveNote() {
    if (!this.currentNoteContent.trim() && !this.currentNoteTitle.trim()) {
      // 这里是同步的，原来的写法也能用，但建议统一风格
      this.getUIContext().getPromptAction().showToast({ message: '请至少输入标题或内容' });
      return;
    }

    const newNote: Note = {
      id: this.currentNoteId || Date.now().toString(),
      title: this.currentNoteTitle,
      content: this.currentNoteContent,
      isPinned: this.currentNotePinned,
      updateTime: new Date().toLocaleString() // 注意：这里最好用之前定义的 getNowDateTime() 保持格式统一
    };

    // 等待异步操作
    await NoteManager.saveNote(newNote);
    await this.loadNotes();

    // 关闭弹窗
    this.showNoteDialog = false;

    // 使用 getUIContext 来显示 Toast
    try {
      this.getUIContext().getPromptAction().showToast({
        message: '保存成功',
        duration: 2000
      });
    } catch (error) {
      console.error('Toast error:', error);
    }
  }

  // 删除便签
  async handleDeleteNote(id: string) {
    // 等待异步操作
    await NoteManager.deleteNote(id);
    await this.loadNotes();

    // 使用 getUIContext 来显示 Toast
    try {
      this.getUIContext().getPromptAction().showToast({
        message: '已删除',
        duration: 2000
      });
    } catch (error) {
      console.error('Toast error:', error);
    }
  }


  changeAuthPassword(method: AuthMethodConfig): void {
    this.selectedAuthType = method.type;
    this.currentMethod = method;
    this.oldPassword = '';
    this.newPassword = '';
    this.confirmPassword = '';
    this.showPassword = false;
    this.isSaving = false;
    if (this.dialogController) {
      this.dialogController.open();
    }
  }

  // pages/ProfilePage.ets

  async saveNewPassword(): Promise<void> {
    if (this.isSaving || !this.selectedAuthType) return;

    // 1. 本地逻辑校验 (这些不涉及异步，没问题)
    if (this.newPassword !== this.confirmPassword) {
      promptAction.showToast({ message: '新密码两次输入不一致' });
      this.confirmPassword = '';
      return;
    }
    if (this.oldPassword.trim() === '' || this.newPassword.trim() === '') {
      promptAction.showToast({ message: '密码不能为空' });
      return;
    }

    if (!this.validatePasswordFormat(this.selectedAuthType!, this.newPassword.trim())) {
      if (this.selectedAuthType === PassType.NUMBER) {
        promptAction.showToast({ message: '数字密码必须为 6 位数字' });
      } else if (this.selectedAuthType === PassType.GRAPH) {
        promptAction.showToast({ message: '数字不能重复且至少需要连接 4 个点' });
      }
      this.newPassword = '';
      this.confirmPassword = '';
      return;
    }

    // 核心修复：提前获取 UIContext 和 commonContext
    const uiContext = this.getUIContext();
    const context = getContext(this) as common.Context;
    this.isSaving = true;

    try {
      const success: boolean = await PasswordManager.updatePassword(
        context,
        this.selectedAuthType,
        this.oldPassword.trim(),
        this.newPassword.trim()
      );

      // 使用 uiContext.runScopedTask 确保 UI 操作回到主线程执行上下文
      uiContext.runScopedTask(() => {
        if (success) {
          promptAction.showToast({ message: '修改成功' });

          // 显式关闭弹窗
          if (this.dialogController) {
            this.dialogController.close();
          }

          // 更新状态
          this.updateLastUsedTime(this.selectedAuthType!);
          this.oldPassword = '';
          this.newPassword = '';
          this.confirmPassword = '';
        } else {
          promptAction.showToast({ message: '原密码输入错误' });
          this.oldPassword = '';
        }
      });

    } catch (err) {
      console.error('Password update exception');
      // 即使出错也要尝试在 UI 线程提示
      uiContext.runScopedTask(() => {
        promptAction.showToast({ message: '系统繁忙，请稍后重试' });
      });
    } finally {
      this.isSaving = false;
    }
  }


  async resetPasswordDirectly(): Promise<void> {
    if (this.isResetting || !this.selectedAuthType) return

    if (this.resetPassword !== this.resetConfirmPassword) {
      promptAction.showToast({ message: '两次密码输入不一致' })
      this.resetConfirmPassword = ''
      return
    }

    if (this.resetPassword.trim() === '') {
      promptAction.showToast({ message: '密码不能为空' })
      return
    }

    if (!this.validatePasswordFormat(this.selectedAuthType!, this.resetPassword.trim())) {
      if (this.selectedAuthType === PassType.NUMBER) {
        promptAction.showToast({ message: '数字密码必须为 6 位数字' });
      } else if (this.selectedAuthType === PassType.GRAPH) {
        promptAction.showToast({ message: '数字不能重复且至少需要连接 4 个点' });
      }
      this.resetPassword = '';
      this.resetConfirmPassword = '';
      return;
    }



    const uiContext = this.getUIContext()
    const context = getContext(this) as common.Context
    this.isResetting = true

    // 先验证身份
    const verified = await PasswordManager.verifyPassword(
      context,
      this.verifyAuthType!,
      this.verifyPassword.trim()
    )

    if (!verified) {
      uiContext.runScopedTask(() => {
        promptAction.showToast({ message: '验证密码错误' })
        this.verifyPassword = ''
      })
      this.isResetting = false
      return
    }


    try {
      const success = await PasswordManager.resetPassword(
        context,
        this.selectedAuthType,
        this.resetPassword.trim()
      )

      uiContext.runScopedTask(() => {
        if (success) {
          promptAction.showToast({ message: '密码重置成功' })
          this.resetDialogController?.close()
          this.updateLastUsedTime(this.selectedAuthType!)
          this.resetPassword = ''
          this.resetConfirmPassword = ''
        } else {
          promptAction.showToast({ message: '密码重置失败' })
        }
      })
    } catch (e) {
      uiContext.runScopedTask(() => {
        promptAction.showToast({ message: '系统繁忙，请稍后重试' })
      })
    } finally {
      this.isResetting = false
    }
  }


  updateLastUsedTime(authType: PassType): void {
    const now = new Date();
    const timeStr = `${now.getHours().toString().padStart(2, '0')}:${now.getMinutes().toString().padStart(2, '0')}`;
    this.authMethods = this.authMethods.map((m: AuthMethodConfig): AuthMethodConfig => {
      if (m.type === authType) {
        return {
          type: m.type,
          name: m.name,
          icon: m.icon,
          color: m.color,
          enabled: m.enabled,
          lastUsed: `今天 ${timeStr}`
        } as AuthMethodConfig;
      }
      return m;
    });
  }


  @Builder buildUserHeader() {
    Column({ space: 15 }) {
      Row() {
        Button('←')
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => RouterUtil.goBack())

        Text('个人中心')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .layoutWeight(1)
          .textAlign(TextAlign.Center)

        Blank()
          .width(40)
      }
      .width('100%')
      Row({ space: 20 })
      {
        Image(this.userInfo.avatar)
          .width(80)
          .height(80)
          .borderRadius(40)

        Column({ space: 8 })
        {
          Text(this.userInfo.username)
            .fontSize(22)
            .fontWeight(FontWeight.Bold)

          Text(`最后登录: ${this.userInfo.lastLogin}`)
            .fontSize(13)
            .fontColor('#666')

        }
        .alignItems(HorizontalAlign.Start)
      }
      .width('100%')
      .padding(20)
      .backgroundColor(Color.White)
      .borderRadius(15)

    }
    .padding(20)

  }

  @Builder buildAuthMethodCard(method: AuthMethodConfig) {
    Row() {
      Column() {
        Image(method.icon)
          .width(30)
          .height(30)
      }
      .width(50)
      .height(50)
      .backgroundColor(method.color)
      .borderRadius(12)
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)

      Column({ space: 4 })
      {
        Text(method.name)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)

        Text(`最后使用: ${method.lastUsed}`)
          .fontSize(12)
          .fontColor('#999')

      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
      .margin({ left: 15 })
      Row({ space: 6 }) {
        // ===== 指纹：只显示 启用 / 禁用 =====
        if (method.type === PassType.FINGERPRINT) {
          Blank()
            .width(8)
            .height(8)
            .backgroundColor('#4CAF50')
            .borderRadius(4)

          Text('已启用')
            .fontSize(12)
            .fontColor('#4CAF50')
        }
        Row({ space: 10 }) {


          // ===== 非指纹：原来的逻辑 =====
          if (method.type !== PassType.FINGERPRINT) {
            Button('修改')
              .height(28)
              .fontSize(12)
              .backgroundColor('#E8F4FF')
              .fontColor('#007DFF')
              .onClick(() => this.changeAuthPassword(method))

            Button('忘记密码')
              .height(28)
              .fontSize(12)
              .backgroundColor('#FFF7E6')
              .fontColor('#FF9500')
              .onClick(() => {
                this.currentMethod = method
                this.selectedAuthType = method.type

                this.verifyAuthType =
                  method.type === PassType.NUMBER
                    ? PassType.GRAPH
                    : PassType.NUMBER

                this.verifyPassword = ''
                this.resetPassword = ''
                this.resetConfirmPassword = ''
                this.isResetting = false

                this.resetDialogController?.open()
              })
          }
        }
      }

    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(10)


  }

  // 跳转到课程表页面
  navigateToSchedule() {
    router.pushUrl({
      url: 'pages/SchedulePage' // 确保您已经在 main_pages.json 中配置了这个路径
    });
  }

  // 构建功能入口卡片（课表）
  @Builder buildFeatureCard() {
    Column() {
      Text('常用功能')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .width('100%')
        .margin({ bottom: 10 })

      Row() {
        // 左侧图标
        Column() {
          Text('📅') // 使用 Emoji 作为图标，或者使用图片
            .fontSize(24)
        }
        .width(50)
        .height(50)
        .backgroundColor('#E3F2FD') // 浅蓝色背景
        .borderRadius(12)
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)

        // 中间文字
        Column({ space: 4 }) {
          Text('我的课表')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)

          Text('查看本学期课程安排')
            .fontSize(12)
            .fontColor('#999')
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)
        .margin({ left: 15 })

        // 右侧箭头
        Text('>')
          .fontSize(18)
          .fontColor('#CCCCCC')
      }
      .width('100%')
      .padding(15)
      .backgroundColor(Color.White)
      .borderRadius(10)
      .onClick(() => {
        this.navigateToSchedule();
      })
    }
    .padding(20) // 外边距，与上面的认证方式保持一致
  }

  // 便签 UI 构建器
  // 便签列表卡片
  @Builder buildNotesCard() {
    Column({ space: 15 }) {
      // 标题栏
      Row() {
        Text('我的便签')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)

        Blank()

        Button('+ 新建')
          .height(28)
          .fontSize(12)
          .backgroundColor('#E8F4FF')
          .fontColor('#007DFF')
          .onClick(() => this.openNoteDialog())
      }
      .width('100%')

      // 搜索栏
      Search({ placeholder: '搜索便签...', value: this.searchKeyword })
        .width('100%')
        .height(36)
        .backgroundColor('#F5F5F5')
        .onChange((value: string) => {
          this.searchKeyword = value;
          this.loadNotes();
        })

      // 列表
      if (this.notes.length === 0) {
        Text(this.searchKeyword ? '无搜索结果' : '暂无便签')
          .fontSize(14)
          .fontColor('#999')
          .width('100%')
          .textAlign(TextAlign.Center)
          .padding(20)
      } else {
        List({ space: 10 }) {
          ForEach(this.notes, (note: Note) => {
            ListItem() {
              Row() {
                Column({ space: 5 }) {
                  Row({ space: 5 }) {
                    if (note.isPinned)
                      Text('')
                        .fontSize(12)

                    Text(note.title || '无标题')
                      .fontSize(16)
                      .fontWeight(FontWeight.Bold)
                      .maxLines(1)
                      .textOverflow({ overflow: TextOverflow.Ellipsis })
                  }
                  Text(note.content)
                    .fontSize(14)
                    .fontColor('#666')
                    .maxLines(1)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })

                  Text(note.updateTime)
                    .fontSize(10)
                    .fontColor('#CCC')

                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start)

                Button('删除')
                  .type(ButtonType.Normal)
                  .backgroundColor(Color.Transparent)
                  .fontColor('#FF3B30')
                  .fontSize(12)
                  .onClick(() => this.handleDeleteNote(note.id))
              }
              .width('100%')
              .padding(10)
              .backgroundColor(note.isPinned ? '#FFF8E1' : '#F9F9F9')
              .borderRadius(8)
              .onClick(() => this.openNoteDialog(note))
            }
          })
        }
        .width('100%')
        .height('auto')
        .constraintSize({ maxHeight: 300 })
      }
    }
    .width('100%')
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(15)
    .margin({ bottom: 20 }) // 注意这里加了 margin bottom
  }

  // 便签编辑弹窗 (全屏遮罩)
  @Builder buildNoteEditDialog() {
    if (this.showNoteDialog) {
      Stack() {
        Column()
          .width('100%')
          .height('100%')
          .backgroundColor('#00000040')
          .onClick(() => this.showNoteDialog = false)

        Column({ space: 15 }) {
          Text(this.currentNoteId ? '编辑便签' : '新建便签')
            .fontSize(18)
            .fontWeight(FontWeight.Bold).width('100%')

          TextInput({ text: this.currentNoteTitle, placeholder: '标题' })
            .height(40)
            .backgroundColor('#F5F5F5')
            .borderRadius(8)
            .onChange(v => this.currentNoteTitle = v)

          TextArea({ text: this.currentNoteContent, placeholder: '内容' })
            .height(100)
            .backgroundColor('#F5F5F5')
            .borderRadius(8)
            .onChange(v => this.currentNoteContent = v)

          Row() {
            Text('置顶')
              .fontSize(16)

            Blank()

            Toggle({ type: ToggleType.Switch, isOn: this.currentNotePinned })
              .onChange(v => this.currentNotePinned = v)

          }
          .width('100%')

          Row({ space: 15 }) {
            Button('取消')
              .layoutWeight(1)
              .backgroundColor('#F5F5F5')
              .fontColor('#666')
              .onClick(() => this.showNoteDialog = false)

            Button('保存')
              .layoutWeight(1)
              .onClick(() => this.handleSaveNote())

          }
        }
        .width(300)
        .padding(20)
        .backgroundColor(Color.White)
        .borderRadius(15)
      }
      .width('100%')
      .height('100%')
      .zIndex(999)
    }
  }

  build() {
    // 根布局改为 Stack，以便弹窗可以覆盖在最上层
    Stack() {
      // 主页面内容
      Column() {
        this.buildUserHeader()

        // 添加 Scroll，防止内容过多导致无法滚动
        Scroll() {
          Column() {
            // 原有的认证方式列表
            Column() {
              Text('认证方式管理')
                .fontSize(18)
                .fontWeight(FontWeight.Bold)
                .width('100%')
                .margin({ top: 0, bottom: 10 })

              ForEach(this.authMethods,
                (m: AuthMethodConfig) => {
                  this.buildAuthMethodCard(m)
                },
                (m: AuthMethodConfig) => m.type.toString())
            }
            .padding(20)

            this.buildFeatureCard()
            // 在认证列表下方添加便签卡片
            // 注意：因为上面已经有 padding，这里通过 margin 调整间距
            Column() {
              this.buildNotesCard()
            }
            .padding({ left: 20, right: 20 })

            // 退出登录按钮
            Button('退出登录')
              .width('80%')
              .height(50)
              .backgroundColor('#FF3B30')
              .fontColor(Color.White)
              .margin({ top: 20, bottom: 30 }) // 底部多留点白
              .onClick(() => {
                router.pushUrl({ url: 'pages/Index' })
              })
          }
        }
        .layoutWeight(1) // 让 Scroll 占据剩余空间
        .scrollBar(BarState.Off)
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F8F9FA')

      // 将编辑弹窗放在 Stack 的最上层
      this.buildNoteEditDialog()
    }
  }
}

@CustomDialog
struct ChangePasswordDialog {
  controller?: CustomDialogController
  authMethod: AuthMethodConfig = {
    type: PassType.NUMBER,
    name: '',
    icon:$r('app.media.avatar_default') ,
    color: '',
    enabled: true,
    lastUsed: ''
  }
  @Link oldPass: string
  @Link newPass: string
  @Link confirmPass: string
  @Link showPass: boolean
  onConfirm: () => void = () => {}

  build() {
    Column({ space: 15 })
    {
      Text(`修改${this.authMethod.name}`)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)

      TextInput({ text: this.oldPass, placeholder: '原密码' })
        .type(this.showPass ? InputType.Normal : InputType.Password)
        .onChange(v => this.oldPass = v)

      TextInput({ text: this.newPass, placeholder: '新密码' })
        .type(this.showPass ? InputType.Normal : InputType.Password)
        .onChange(v => this.newPass = v)

      TextInput({ text: this.confirmPass, placeholder: '确认新密码' })
        .type(this.showPass ? InputType.Normal : InputType.Password)
        .onChange(v => this.confirmPass = v)

      Row() {
        Checkbox()
          .select(this.showPass)
          .onChange(v => this.showPass = v)

        Text('显示密码')
          .fontSize(14)
          .margin({ left: 5 })

      }
      .width('100%')


      Row({ space: 10 }) {
        Button('取消')
          .layoutWeight(1)
          .backgroundColor('#F2F2F2')
          .fontColor('#666')
          .onClick(() => this.controller?.close())


        Button('确定修改')
          .layoutWeight(1)
          .onClick(() => {
            this.onConfirm()
          })
      }
      .width('100%')
      .margin({ top: 10 })

    }
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(15)
    .width(310)
  }
}



@CustomDialog
struct ResetPasswordDialog {
  controller?: CustomDialogController
  verifyAuthType: PassType = PassType.NUMBER
  authMethod: AuthMethodConfig = {
    type: PassType.NUMBER,
    name: '',
    icon:$r('app.media.avatar_default') ,
    color: '',
    enabled: true,
    lastUsed: ''
  }
  @Link verifyPass: string
  @Link newPass: string
  @Link confirmPass: string
  onConfirm: () => void = () => {}

  build() {
    Column({ space: 15 }) {
      Text(`重置${this.authMethod.name}`)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)

      Text(
        this.verifyAuthType === PassType.NUMBER
          ? '请输入原数字密码'
          : '请输入原图形密码'
      )
        .fontSize(14)
        .fontColor('#666')

      TextInput({
        text: this.verifyPass,
        placeholder: '验证密码'
      })
        .type(InputType.Password)
        .onChange(v => this.verifyPass = v)

      TextInput({ text: this.newPass, placeholder: '新密码' })
        .type(InputType.Password)
        .onChange(v => this.newPass = v)

      TextInput({ text: this.confirmPass, placeholder: '确认新密码' })
        .type(InputType.Password)
        .onChange(v => this.confirmPass = v)

      Row({ space: 10 }) {
        Button('取消')
          .layoutWeight(1)
          .backgroundColor('#F2F2F2')
          .fontColor('#666')
          .onClick(() => this.controller?.close())

        Button('确认重置')
          .layoutWeight(1)
          .onClick(() => this.onConfirm())
      }
      .margin({ top: 10 })
    }
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(15)
    .width(300)
  }
}