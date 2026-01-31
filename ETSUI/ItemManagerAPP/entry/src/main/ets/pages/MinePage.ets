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
import { promptAction } from '@kit.ArkUI';
import picker from '@ohos.file.picker'; // 图片选择器
import { UserInfo, AppInfo } from '../model/types';
import { itemStore } from '../store';
import userInfoRdb from '../data/UserRdb';
import common from '@ohos.app.ability.common';

@Component
export struct MinePage {
  // 页面状态
  @State showConfirmDialog: boolean = false;
  @State showNoDataDialog: boolean = false;
  @State showSuccessDialog: boolean = false;
  @State showHelpDialog: boolean = false;
  @State showEditDialog: boolean = false;
  @State showAvatarSelectDialog: boolean = false; // 头像选择弹窗
  @State nicknameInput: string = ''; // 移除默认值，从数据库加载
  @State isClearing: boolean = false;
  @State itemCount: number = 0;
  @State isLoading: boolean = true;
  @State isUserInfoLoading: boolean = true; // 新增：用户信息加载状态
  // 预设头像库（表情）
  private avatarLibrary: string[] = [
    '👤', '👨‍💻', '👩‍💻', '🧑‍💻', '🐱', '🐶', '🦁', '🐰', '🦊', '🐻'
  ];
  // 用户信息（初始化为空，从数据库加载，无 id 字段）
  @State userInfo: UserInfo = {
    avatar: '',
    nickname: '',
    joinDate: ''
  };
  // 应用信息
  private appInfo: AppInfo = {
    name: '轻物志',
    version: '1.0.0',
    description: '智能物品管理助手',
    developer: 'OpenHarmony课程小组',
    copyright: '© 2026 OpenHarmony课程小组'
  };

  // ========== 页面生命周期（核心适配：无 id 逻辑） ==========
  async onCreate() {
    console.info('MinePage 创建，开始初始化数据');
    // 并行加载物品数量和用户信息（无 id 相关逻辑）
    await Promise.all([
      this.initItemCount(),
      this.loadUserInfo()// 加载用户信息时无需处理 id
    ]);
  }

  aboutToAppear(): void {
    console.info('我的页面显示');
    itemStore.onItemCountChange(this.handleItemCountChange);

    // 每次页面显示时重新加载最新用户信息（无 id）
    this.loadUserInfo().catch((error: Error) => {
      console.error('页面显示时加载用户信息失败:', error.message);
      this.isUserInfoLoading = false;
    });

    if (this.isLoading) {
      this.initItemCount().catch((error: Error) => {
        console.error('页面显示时加载数据失败:', error.message);
        this.isLoading = false;
      });
    }
  }

  aboutToDisappear(): void {
    console.info('我的页面消失');
    itemStore.offItemCountChange(this.handleItemCountChange);
  }

  // ========== 核心初始化方法（无 id 逻辑） ==========
  private async initItemCount(): Promise<void> {
    try {
      console.info('开始从数据库初始化物品数量');
      const count = await itemStore.getItemCount();
      this.itemCount = count;
      console.info(`初始化物品数量成功: ${count}个`);
    } catch (error) {
      console.error('初始化物品数量失败:', error.message);
      this.itemCount = 0;
      this.showErrorToast('加载物品数量失败');
    } finally {
      this.isLoading = false;
    }
  }

  // 核心适配：加载用户信息时无需处理 id（UserRdb 已移除 id）
  // 新增：加载用户信息前先初始化数据库（核心修复）
  private async loadUserInfo(): Promise<void> {
    try {
      console.info('开始从数据库加载用户信息（无 id 字段）');
      this.isUserInfoLoading = true;

      // 关键：先初始化数据库（获取上下文，这里假设你能拿到 UIAbilityContext）
      // 注意：如果 MinePage 没有直接的 context，需要从 EntryAbility 或全局获取
      const context = getContext(this) as common.UIAbilityContext;
      await userInfoRdb.getRdbStore(context);

      // 再查询用户信息
      const userInfo = await userInfoRdb.queryUserInfo();
      this.userInfo = userInfo;
      this.nicknameInput = userInfo.nickname; // 同步到编辑框
      console.info('加载用户信息成功:', JSON.stringify(userInfo));
    } catch (error) {
      console.error('加载用户信息失败:', error.message);
      // 加载失败时使用默认值（无 id）
      this.userInfo = {
        avatar: '👤',
        nickname: '轻物志用户',
        joinDate: '2026年1月'
      };
      this.nicknameInput = this.userInfo.nickname;
      this.showErrorToast('加载用户信息失败');
    } finally {
      this.isUserInfoLoading = false;
    }
  }

  // ========== 全局状态处理 ==========
  private handleItemCountChange = (count: number): void => {
    console.info(`接收到全局状态更新: ${count}个物品`);
    this.itemCount = count;
    this.isLoading = false;
  };

  private async loadFromStore(): Promise<void> {
    console.info('从全局状态加载物品数量');
    try {
      const count = await itemStore.getItemCount();
      this.itemCount = count;
      await itemStore.updateItemCount();
    } catch (error) {
      console.error('加载物品数量失败:', error.message);
      this.itemCount = 0;
    }
  }

  // ========== 核心功能方法 ==========
  private async handleClearData(): Promise<void> {
    console.info('点击清空数据按钮');
    try {
      const count = await itemStore.getItemCount();
      this.itemCount = count;

      if (this.itemCount === 0) {
        this.showNoDataDialog = true;
      } else {
        this.showConfirmDialog = true;
      }
    } catch (error) {
      console.error('检查物品数量失败:', error.message);
      this.showErrorToast('检查数据失败');
    }
  }

  private async executeClearData(): Promise<void> {
    if (this.isClearing) {
      return;
    }
    this.isClearing = true;
    this.showConfirmDialog = false;
    try {
      console.info('开始清空数据库...');
      await itemStore.clearAllItems();
      console.info('数据库清空成功');
      this.itemCount = 0;
      this.showSuccessDialog = true;
    } catch (error) {
      console.error('清空数据失败:', error.message);
      this.showErrorToast('清空失败，请重试');
    } finally {
      this.isClearing = false;
    }
  }

  // ========== 头像选择功能（核心适配：无 id 更新） ==========
  // 打开头像选择弹窗
  private openAvatarSelectDialog(): void {
    this.showEditDialog = false;
    this.showAvatarSelectDialog = true;
  }

  // 选择预设表情头像（无需传递 id，UserRdb 已移除 id 条件）
  private async selectAvatar(avatar: string): Promise<void> {
    try {
      // 直接更新头像，UserRdb 内部无需 id 条件
      await userInfoRdb.updateAvatar(avatar);
      // 更新页面状态（无 id 相关处理）
      this.userInfo.avatar = avatar;
      this.showAvatarSelectDialog = false;
      this.showToast('头像更换成功');
    } catch (error) {
      console.error('更换头像失败:', error.message);
      this.showErrorToast('更换头像失败，请重试');
    }
  }

  // 从相册选择头像（无需传递 id）
  private async selectAvatarFromAlbum(): Promise<void> {
    try {
      const photoPicker = new picker.PhotoViewPicker();
      const selectOptions = new picker.PhotoSelectOptions();
      // 配置选择参数：仅选1张图片，只选图片类型
      selectOptions.maxSelectNumber = 1;
      selectOptions.MIMEType = picker.PhotoViewMIMETypes.IMAGE_TYPE;

      // 调用选择器
      const result = await photoPicker.select(selectOptions);

      // 处理选择结果
      if (result.photoUris && result.photoUris.length > 0) {
        const imageUri = result.photoUris[0];
        // 直接更新头像，无需 id
        await userInfoRdb.updateAvatar(imageUri);
        // 更新页面状态
        this.userInfo.avatar = imageUri;
        this.showAvatarSelectDialog = false;
        promptAction.showToast({ message: '头像选择成功', duration: 1500 });
      }
    } catch (err) {
      console.error("选择头像图片失败:", err.message);
      promptAction.showToast({ message: '头像选择失败，请重试', duration: 2000 });
    }
  }

  // ========== 编辑资料相关（核心适配：无 id 更新昵称） ==========
  private handleEditProfile(): void {
    this.nicknameInput = this.userInfo.nickname;
    this.showEditDialog = true;
  }

  // 保存昵称（无需传递 id，UserRdb 内部无 id 条件）
  private async saveNickname(): Promise<void> {
    try {
      const trimmedNickname = this.nicknameInput.trim();
      if (trimmedNickname.length === 0) {
        this.showErrorToast('昵称不能为空');
        return;
      }
      if (trimmedNickname.length > 12) {
        this.showErrorToast('昵称不能超过12个字符');
        return;
      }

      // 直接更新昵称，UserRdb 内部无需 id 条件
      await userInfoRdb.updateNickname(trimmedNickname);
      // 更新页面状态（无 id 相关处理）
      this.userInfo.nickname = trimmedNickname;
      this.showEditDialog = false;
      this.showToast('昵称修改成功');
    } catch (error) {
      console.error('保存昵称失败:', error.message);
      this.showErrorToast(error instanceof Error ? error.message : '昵称修改失败');
    }
  }

  // ========== 提示工具方法 ==========
  private showErrorToast(message: string): void {
    try {
      promptAction.showToast({
        message: message,
        duration: 1500
      });
    } catch (error) {
      console.error('显示Toast出错:', error.message);
    }
  }

  private showToast(message: string): void {
    try {
      promptAction.showToast({
        message: message,
        duration: 1500
      });
    } catch (error) {
      console.error('显示Toast出错:', error.message);
    }
  }

  private safeExecuteClearData(): void {
    this.executeClearData().catch((error: Error) => {
      console.error('执行清空操作出错:', error.message);
    });
  }

  private safeHandleClearData(): void {
    this.handleClearData().catch((error: Error) => {
      console.error('处理清空数据出错:', error.message);
    });
  }

  // ========== 构建方法（无 id 相关修改，仅保留适配注释） ==========
  @Builder
  private buildLoadingPlaceholder() {
    Column() {
      LoadingProgress()
        .color('#4A6CF7')
        .width(24)
        .height(24)
      Text('加载中...')
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 8 })
    }
    .width('100%')
    .height(24)
    .justifyContent(FlexAlign.Center)
  }

  // 新增：用户信息加载占位符
  @Builder
  private buildUserInfoLoadingPlaceholder() {
    Column() {
      // 头像占位
      Column() {
        LoadingProgress()
          .color('#4A6CF7')
          .width(24)
          .height(24)
      }
      .width(80)
      .height(80)
      .backgroundColor('#F0F5FF')
      .borderRadius(40)
      .justifyContent(FlexAlign.Center)

      // 昵称占位
      Text('加载中...')
        .fontSize(22)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333')
        .margin({ top: 8 })
    }
    .width('100%')
    .justifyContent(FlexAlign.Center)
  }

  // 个人资料卡片（无 id 相关修改）
  @Builder
  private buildUserInfoCard() {
    Column() {
      Row({ space: 16 }) {
        // 头像区域（可点击）
        Column() {
          if (this.isUserInfoLoading) {
            // 加载中占位符
            this.buildUserInfoLoadingPlaceholder()
          } else {
            // 区分是表情头像还是图片头像
            if (this.userInfo.avatar.startsWith('file://') || this.userInfo.avatar.startsWith('data:image/')) {
              // 图片头像
              Image(this.userInfo.avatar)
                .width(80)
                .height(80)
                .borderRadius(40)
                .objectFit(ImageFit.Cover)
                .backgroundColor('#F0F5FF')
                .shadow({ radius: 6, color: '#20000000', offsetY: 3 })
            } else {
              // 表情头像（处理空值，无 id 相关）
              Text(this.userInfo.avatar || '👤')
                .fontSize(40)
                .fontColor('#4A6CF7')
                .width(80)
                .height(80)
                .backgroundColor('#F0F5FF')
                .borderRadius(40)
                .textAlign(TextAlign.Center)
                .shadow({ radius: 6, color: '#20000000', offsetY: 3 })
            }
          }
        }
        .onClick(() => {
          if (!this.isUserInfoLoading) {
            this.openAvatarSelectDialog();
          }
        })

        // 用户信息
        Column({ space: 8 }) {
          if (this.isUserInfoLoading) {
            // 加载中占位
            this.buildLoadingPlaceholder()
          } else {
            Row({ space: 4 }) {
              Text(this.userInfo.nickname || '轻物志用户')
                .fontSize(22)
                .fontWeight(FontWeight.Bold)
                .fontColor('#333')
                .maxLines(1)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .onClick(() => {
                  this.handleEditProfile();
                })

              Text('✏️')
                .fontSize(13)
                .fontColor('#CCCCCC')
                .margin({ left: 4 })
                .opacity(0.7)
                .onClick(() => {
                  this.handleEditProfile();
                })
            }
            .width('100%')
            .alignItems(VerticalAlign.Center)

            Row({ space: 24 }) {
              Column({ space: 2 }) {
                Text('加入时间')
                  .fontSize(12)
                  .fontColor('#666')
                Text(this.userInfo.joinDate || '2026年1月')
                  .fontSize(14)
                  .fontColor('#333')
                  .fontWeight(FontWeight.Medium)
              }

              Column({ space: 2 }) {
                Text('物品数量')
                  .fontSize(12)
                  .fontColor('#666')
                if (this.isLoading) {
                  this.buildLoadingPlaceholder()
                } else {
                  Text(`${this.itemCount}个`)
                    .fontSize(14)
                    .fontColor('#333')
                    .fontWeight(FontWeight.Medium)
                }
              }
            }
            .margin({ top: 8 })
          }
        }
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)
      }
      .padding({
        left: 20,
        right: 20,
        top: 24,
        bottom: 24
      })
    }
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(20)
    .margin({ top: 16, bottom: 16 })
    .shadow({ radius: 12, color: '#10000000', offsetY: 4 })
  }

  // 头像选择弹窗（无 id 相关修改）
  @Builder
  private buildAvatarSelectDialog() {
    Column() {
      Column() {
        // 标题栏（关闭按钮+标题）
        Row() {
          Text('更换头像')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333')
          Text('✕')
            .fontSize(18)
            .fontColor('#999')
            .onClick(() => {
              this.showAvatarSelectDialog = false;
            })
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)
        .margin({ bottom: 20 })

        // 核心修复：用外层 Column 包裹 Grid，限制最大高度
        Column() {
          Grid() {
            ForEach(this.avatarLibrary, (avatar: string) => {
              GridItem() {
                Column() {
                  Text(avatar)
                    .fontSize(32)
                  // 选中状态标识（无 id 相关）
                  if (avatar === this.userInfo.avatar) {
                    Text('✓')
                      .fontSize(12)
                      .fontColor('#4A6CF7')
                      .backgroundColor(Color.White)
                      .borderRadius(10)
                      .width(20)
                      .height(20)
                      .position({ right: 0, bottom: 0 })
                  }
                }
                .width(60)
                .height(60)
                .justifyContent(FlexAlign.Center)
                .alignItems(HorizontalAlign.Center)
                .backgroundColor(this.userInfo.avatar === avatar ? '#E6EEFF' : '#F9FAFB')
                .borderRadius(30)
                .onClick(() => {
                  this.selectAvatar(avatar);
                })
              }
            })
          }
          .columnsTemplate('1fr 1fr 1fr 1fr 1fr')
          .columnsGap(12)
          .rowsGap(12)
          .width('100%') // Grid 宽度充满容器
          .height('100%') // Grid 高度自适应外层容器
        }
        .height(144) // 外层容器限制最大高度（2行×60+12间隙）
        .width('100%')
        .margin({ bottom: 20 })

        // 从相册选择按钮（确保样式可见）
        Button('从相册选择')
          .width('100%')
          .height(44)
          .backgroundColor('#F0F2F5')
          .fontColor('#666')
          .fontSize(14)
          .borderRadius(8)
          .opacity(1)
          .visibility(Visibility.Visible)
          .onClick(async () => {
            await this.selectAvatarFromAlbum();
          })
      }
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(20)
      .margin({ left: 24, right: 24 })
      .height('60%') // 弹窗内容最大高度（ArkTS 支持 Column 的 maxHeight）
      .width('80%')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .position({ x: 0, y: 0 })
    .zIndex(104)
  }

  // 编辑资料对话框（无 id 相关修改）
  @Builder
  private buildEditDialog() {
    Column() {
      Column() {
        Text('编辑资料')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ bottom: 20 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 头像区域（可点击）
        Row() {
          Column() {
            if (this.isUserInfoLoading) {
              // 加载中占位
              this.buildUserInfoLoadingPlaceholder()
            } else {
              if (this.userInfo.avatar.startsWith('file://') || this.userInfo.avatar.startsWith('data:image/')) {
                // 图片头像
                Image(this.userInfo.avatar)
                  .width(64)
                  .height(64)
                  .borderRadius(32)
                  .objectFit(ImageFit.Cover)
                  .backgroundColor('#F0F5FF')
              } else {
                // 表情头像（处理空值，无 id）
                Text(this.userInfo.avatar || '👤')
                  .fontSize(32)
                  .fontColor('#4A6CF7')
                  .width(64)
                  .height(64)
                  .backgroundColor('#F0F5FF')
                  .borderRadius(32)
                  .textAlign(TextAlign.Center)
              }
              Text('点击更换')
                .fontSize(12)
                .fontColor('#999')
                .margin({ top: 8 })
            }
          }
          .onClick(() => {
            if (!this.isUserInfoLoading) {
              this.openAvatarSelectDialog();
            }
          })
        }
        .width('100%')
        .margin({ bottom: 20 })
        .justifyContent(FlexAlign.Center)

        // 昵称编辑
        Column() {
          Text('昵称')
            .fontSize(14)
            .fontColor('#666')
            .margin({ bottom: 8 })
            .width('100%')
            .textAlign(TextAlign.Start)

          TextInput({
            text: this.nicknameInput || '',
            placeholder: '请输入昵称'
          })
            .width('100%')
            .height(44)
            .backgroundColor('#F8F9FA')
            .fontSize(15)
            .fontColor('#333')
            .placeholderColor('#999')
            .placeholderFont({ size: 15, weight: FontWeight.Normal })
            .maxLength(12)
            .borderRadius(8)
            .padding({ left: 12, right: 12 })
            .onChange((value: string) => {
              this.nicknameInput = value;
            })
        }
        .width('100%')
        .margin({ bottom: 24 })

        // 按钮区域
        Row() {
          Button('取消')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#F0F2F5')
            .fontColor('#666')
            .fontSize(14)
            .borderRadius(8)
            .onClick(() => {
              this.showEditDialog = false;
            })

          Button('保存')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#4A6CF7')
            .fontColor(Color.White)
            .fontSize(14)
            .borderRadius(8)
            .margin({ left: 12 })
            .onClick(() => {
              this.saveNickname();
            })
        }
      }
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(20)
      .margin({ left: 24, right: 24 })
      .shadow({ radius: 32, color: '#40000000' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
    .zIndex(103)
  }

  // 数据管理卡片（无 id 相关修改）
  @Builder
  private buildDataManagementCard() {
    Column() {
      Text('数据管理')
        .fontSize(14)
        .fontColor('#666')
        .margin({ bottom: 8 })
        .width('100%')

      Column() {
        Row() {
          Text('🗑️')
            .fontSize(20)
            .margin({ right: 12 })
            .width(24)

          Column({ space: 2 }) {
            Text('清空所有数据')
              .fontSize(16)
              .fontColor('#333')
              .fontWeight(FontWeight.Medium)
            Text('永久删除所有物品记录')
              .fontSize(12)
              .fontColor('#999')
          }
          .layoutWeight(1)

          Text('›')
            .fontSize(20)
            .fontColor('#999')
        }
        .width('100%')
        .height(56)
        .alignItems(VerticalAlign.Center)
        .onClick(() => {
          this.safeHandleClearData();
        })
      }
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius(12)
      .padding(12)
      .shadow({ radius: 4, color: '#0D000000', offsetY: 2 })
    }
    .width('100%')
    .margin({ bottom: 16 })
  }

  // 关于与帮助卡片（无 id 相关修改）
  @Builder
  private buildAboutCard() {
    Column() {
      Text('关于与帮助')
        .fontSize(14)
        .fontColor('#666')
        .margin({ bottom: 8 })
        .width('100%')

      Column() {
        Row() {
          Text('📱')
            .fontSize(20)
            .margin({ right: 12 })
            .width(24)

          Column({ space: 2 }) {
            Text(this.appInfo.name)
              .fontSize(16)
              .fontColor('#333')
              .fontWeight(FontWeight.Medium)
            Text('版本 ' + this.appInfo.version)
              .fontSize(12)
              .fontColor('#999')
          }
          .layoutWeight(1)
        }
        .width('100%')
        .height(56)
        .alignItems(VerticalAlign.Center)

        Divider()
          .color('#F0F0F0')
          .margin({ left: 36, right: 0 })

        Row() {
          Text('🎯')
            .fontSize(20)
            .margin({ right: 12 })
            .width(24)

          Column({ space: 2 }) {
            Text('帮助')
              .fontSize(16)
              .fontColor('#333')
              .fontWeight(FontWeight.Medium)
            Text('使用教程与操作指南')
              .fontSize(12)
              .fontColor('#999')
          }
          .layoutWeight(1)

          Text('›')
            .fontSize(20)
            .fontColor('#999')
        }
        .width('100%')
        .height(56)
        .alignItems(VerticalAlign.Center)
        .onClick(() => {
          this.showHelpDialog = true;
        })

        Divider()
          .color('#F0F0F0')
          .margin({ left: 36, right: 0 })

        Row() {
          Text('👨‍💻')
            .fontSize(20)
            .margin({ right: 12 })
            .width(24)

          Column({ space: 2 }) {
            Text('开发者信息')
              .fontSize(16)
              .fontColor('#333')
              .fontWeight(FontWeight.Medium)
            Text(this.appInfo.developer)
              .fontSize(12)
              .fontColor('#999')
          }
          .layoutWeight(1)
        }
        .width('100%')
        .height(56)
        .alignItems(VerticalAlign.Center)
      }
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius(12)
      .padding(12)
      .shadow({ radius: 4, color: '#0D000000', offsetY: 2 })
    }
    .width('100%')
    .margin({ bottom: 16 })
  }

  // 底部信息（无 id 相关修改）
  @Builder
  private buildFooter() {
    Column() {
      Text(this.appInfo.copyright)
        .fontSize(12)
        .fontColor('#999')
      Text('基于 OpenHarmony 开发')
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 4 })
    }
    .width('100%')
    .padding({ top: 24, bottom: 32 })
    .alignItems(HorizontalAlign.Center)
  }

  // 清空数据确认对话框（无 id 相关修改）
  @Builder
  private buildConfirmDialog() {
    Column() {
      Column() {
        Text('确认清空数据')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ bottom: 12 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Text('⚠️')
          .fontSize(48)
          .margin({ bottom: 16 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Text(`当前共有 ${this.itemCount} 个物品记录`)
          .fontSize(14)
          .fontColor('#666')
          .margin({ bottom: 16 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Text('此操作将永久删除所有物品数据，且无法恢复！')
          .fontSize(14)
          .fontColor('#F53F3F')
          .margin({ bottom: 24 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Row() {
          Button('取消')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#F0F2F5')
            .fontColor('#666')
            .fontSize(14)
            .borderRadius(8)
            .onClick(() => {
              console.info('取消清空数据');
              this.showConfirmDialog = false;
            })

          Button('确认清空')
            .layoutWeight(1)
            .height(44)
            .backgroundColor('#F53F3F')
            .fontColor(Color.White)
            .fontSize(14)
            .borderRadius(8)
            .margin({ left: 12 })
            .onClick(() => {
              this.safeExecuteClearData();
            })
        }
      }
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(20)
      .margin({ left: 24, right: 24 })
      .shadow({ radius: 32, color: '#40000000' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
    .zIndex(100)
  }

  // 无数据提示对话框（无 id 相关修改）
  @Builder
  private buildNoDataDialog() {
    Column() {
      Column() {
        Text('📭')
          .fontSize(48)
          .margin({ bottom: 16 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Text('暂无数据')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ bottom: 8 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Text('当前没有物品记录，无需清空')
          .fontSize(14)
          .fontColor('#666')
          .margin({ bottom: 24 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Button('知道了')
          .width('100%')
          .height(44)
          .backgroundColor('#4A6CF7')
          .fontColor(Color.White)
          .fontSize(14)
          .borderRadius(8)
          .onClick(() => {
            this.showNoDataDialog = false;
          })
      }
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(20)
      .margin({ left: 24, right: 24 })
      .shadow({ radius: 32, color: '#40000000' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
    .zIndex(101)
  }

  // 清空成功对话框（无 id 相关修改）
  @Builder
  private buildSuccessDialog() {
    Column() {
      Column() {
        Text('✅')
          .fontSize(48)
          .margin({ bottom: 16 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Text('数据已清空')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ bottom: 8 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Text('所有物品数据已被安全清除')
          .fontSize(14)
          .fontColor('#666')
          .margin({ bottom: 24 })
          .width('100%')
          .textAlign(TextAlign.Center)

        Button('知道了')
          .width('100%')
          .height(44)
          .backgroundColor('#4A6CF7')
          .fontColor(Color.White)
          .fontSize(14)
          .borderRadius(8)
          .onClick(() => {
            this.showSuccessDialog = false;
          })
      }
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(20)
      .margin({ left: 24, right: 24 })
      .shadow({ radius: 32, color: '#40000000' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
    .zIndex(102)
  }

  // 帮助对话框（无 id 相关修改）
  @Builder
  private buildHelpDialog() {
    Column() {
      Column() {
        Row() {
          Text('🎯')
            .fontSize(24)
            .margin({ right: 8 })
          Text('使用帮助')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333')
        }
        .width('100%')
        .margin({ bottom: 20 })
        .justifyContent(FlexAlign.Start)

        Scroll() {
          Column({ space: 16 }) {
            Column() {
              Text('欢迎使用轻物志！以下是详细的使用指南，帮助你快速上手。')
                .fontSize(13)
                .fontColor('#666')
                .lineHeight(18)
            }
            .padding(12)
            .backgroundColor('#F9FAFB')
            .borderRadius(10)
            .width('100%')

            Row({ space: 12 }) {
              Column() {
                Text('1')
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(Color.White)
              }
              .width(24)
              .height(24)
              .backgroundColor('#4A6CF7')
              .borderRadius(12)
              .justifyContent(FlexAlign.Center)
              .alignItems(HorizontalAlign.Center)
              .flexShrink(0)

              Column({ space: 4 }) {
                Text('添加物品')
                  .fontSize(15)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333')
                Text('点击底部"添加"标签 → 填写物品信息 → 选择分类 → 点击"添加物品"按钮完成')
                  .fontSize(12)
                  .fontColor('#666')
                  .lineHeight(16)
              }
              .layoutWeight(1)
            }
            .width('100%')
            .padding(12)
            .backgroundColor('#F9FAFB')
            .borderRadius(10)

            Row({ space: 12 }) {
              Column() {
                Text('2')
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(Color.White)
              }
              .width(24)
              .height(24)
              .backgroundColor('#34C759')
              .borderRadius(12)
              .justifyContent(FlexAlign.Center)
              .alignItems(HorizontalAlign.Center)
              .flexShrink(0)

              Column({ space: 4 }) {
                Text('查看与搜索')
                  .fontSize(15)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333')
                Text('点击"物品列表"查看所有物品 → 使用顶部搜索框快速查找 → 支持按名称、分类、位置搜索')
                  .fontSize(12)
                  .fontColor('#666')
                  .lineHeight(16)
              }
              .layoutWeight(1)
            }
            .width('100%')
            .padding(12)
            .backgroundColor('#F9FAFB')
            .borderRadius(10)

            Row({ space: 12 }) {
              Column() {
                Text('3')
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(Color.White)
              }
              .width(24)
              .height(24)
              .backgroundColor('#FF9500')
              .borderRadius(12)
              .justifyContent(FlexAlign.Center)
              .alignItems(HorizontalAlign.Center)
              .flexShrink(0)

              Column({ space: 4 }) {
                Text('编辑与删除')
                  .fontSize(15)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333')
                Text('在物品列表向左滑动 → 点击"编辑"修改信息 → 点击"删除"移除记录')
                  .fontSize(12)
                  .fontColor('#666')
                  .lineHeight(16)
              }
              .layoutWeight(1)
            }
            .width('100%')
            .padding(12)
            .backgroundColor('#F9FAFB')
            .borderRadius(10)

            Row({ space: 12 }) {
              Column() {
                Text('4')
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(Color.White)
              }
              .width(24)
              .height(24)
              .backgroundColor('#5856D6')
              .borderRadius(12)
              .justifyContent(FlexAlign.Center)
              .alignItems(HorizontalAlign.Center)
              .flexShrink(0)

              Column({ space: 4 }) {
                Text('查看统计')
                  .fontSize(15)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333')
                Text('点击"统计"标签 → 查看物品分类分布 → 分析使用状态统计 → 图表可视化展示')
                  .fontSize(12)
                  .fontColor('#666')
                  .lineHeight(16)
              }
              .layoutWeight(1)
            }
            .width('100%')
            .padding(12)
            .backgroundColor('#F9FAFB')
            .borderRadius(10)

            Row({ space: 12 }) {
              Column() {
                Text('5')
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(Color.White)
              }
              .width(24)
              .height(24)
              .backgroundColor('#FF2D55')
              .borderRadius(12)
              .justifyContent(FlexAlign.Center)
              .alignItems(HorizontalAlign.Center)
              .flexShrink(0)

              Column({ space: 4 }) {
                Text('管理数据')
                  .fontSize(15)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333')
                Text('进入"我的"页面 → 点击"清空所有数据" → 确认后安全删除所有记录')
                  .fontSize(12)
                  .fontColor('#666')
                  .lineHeight(16)
              }
              .layoutWeight(1)
            }
            .width('100%')
            .padding(12)
            .backgroundColor('#F9FAFB')
            .borderRadius(10)

            Column() {
              Row() {
                Text('💡')
                  .fontSize(16)
                  .margin({ right: 8 })
                Text('使用建议')
                  .fontSize(14)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333')
              }
              .width('100%')
              .margin({ bottom: 8 })

              Text('• 为物品选择合适的分类，方便后续查找和管理\n• 详细记录物品位置，需要时能快速找到\n• 定期使用统计功能了解物品分布情况\n• 重要物品可添加详细备注信息')
                .fontSize(12)
                .fontColor('#666')
                .lineHeight(18)
            }
            .padding(12)
            .backgroundColor('#F9FAFB')
            .borderRadius(10)
            .width('100%')
          }
          .padding({ right: 4 })
        }
        .layoutWeight(1)
        .margin({ bottom: 20 })
        .scrollBar(BarState.Auto)

        Button('开始使用')
          .width('100%')
          .height(44)
          .backgroundColor('#4A6CF7')
          .fontColor(Color.White)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .borderRadius(8)
          .onClick(() => {
            this.showHelpDialog = false;
          })
      }
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(20)
      .width('90%')
      .height('75%')
      .shadow({ radius: 32, color: '#40000000' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .position({ x: 0, y: 0 })
    .zIndex(201)
  }

  // 加载中遮罩（无 id 相关修改）
  @Builder
  private buildLoadingOverlay() {
    Column() {
      LoadingProgress()
        .color(Color.White)
        .width(48)
        .height(48)

      Text('正在清空数据...')
        .fontSize(16)
        .fontColor(Color.White)
        .margin({ top: 16 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#80000000')
    .justifyContent(FlexAlign.Center)
    .position({ x: 0, y: 0 })
    .zIndex(300)
  }

  // ========== 主构建方法（无 id 相关修改） ==========
  build() {
    Column() {
      // 滚动内容区域
      Scroll() {
        Column() {
          this.buildUserInfoCard()
          this.buildDataManagementCard()
          this.buildAboutCard()
          this.buildFooter()
        }
        .padding({ left: 16, right: 16, bottom: 32 })
      }
      .scrollBar(BarState.Off)
      .layoutWeight(1)

      // 弹窗和遮罩层
      if (this.showConfirmDialog) {
        this.buildConfirmDialog()
      }
      if (this.showNoDataDialog) {
        this.buildNoDataDialog()
      }
      if (this.showSuccessDialog) {
        this.buildSuccessDialog()
      }
      if (this.showEditDialog) {
        this.buildEditDialog()
      }
      if (this.showHelpDialog) {
        this.buildHelpDialog()
      }
      if (this.showAvatarSelectDialog) {
        this.buildAvatarSelectDialog()
      }
      if (this.isClearing) {
        this.buildLoadingOverlay()
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F9FAFB')
  }
}