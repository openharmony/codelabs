import { promptAction } from '@kit.ArkUI';
import { UserInfo, AppInfo } from '../model/types';
import { itemStore } from '../store';

@Component
export struct MinePage {
  // 页面状态
  @State showConfirmDialog: boolean = false;
  @State showNoDataDialog: boolean = false;
  @State showSuccessDialog: boolean = false;
  @State showHelpDialog: boolean = false;
  @State showEditDialog: boolean = false;
  @State nicknameInput: string = '轻物志用户';
  @State isClearing: boolean = false;
  @State itemCount: number = 0;

  // 用户信息
  @State userInfo: UserInfo = {
    avatar: '👤',
    nickname: '轻物志用户',
    joinDate: '2026年1月'
  };

  // 应用信息
  private appInfo: AppInfo = {
    name: '轻物志',
    version: '1.0.0',
    description: '智能物品管理助手',
    developer: 'OpenHarmony课程小组',
    copyright: '© 2026 OpenHarmony课程小组'
  };

  // ========== 页面生命周期 ==========
  // 页面显示时
  aboutToAppear(): void {
    console.info('我的页面显示');
    // 首次加载：从全局状态获取数据
    this.loadFromStore();
    // 监听全局状态变化
    itemStore.onItemCountChange(this.handleItemCountChange);
  }

  // 页面消失时
  aboutToDisappear(): void {
    console.info('我的页面消失');
    // 移除全局状态监听
    itemStore.offItemCountChange(this.handleItemCountChange);
  }

  // ========== 全局状态处理 ==========
  // 处理物品数量变化（监听回调）
  private handleItemCountChange = (count: number): void => {
    console.info(`接收到全局状态更新: ${count}个物品`);
    this.itemCount = count;
  };

  // 从全局状态加载数据
  private async loadFromStore(): Promise<void> {
    console.info('从全局状态加载物品数量');
    // 更新全局状态（会触发监听回调）
    await itemStore.updateItemCount();
  }

  // ========== 核心功能方法 ==========
  // 处理清空数据 - 总是先检查
  private async handleClearData(): Promise<void> {
    console.info('点击清空数据按钮');
    try {
      // 总是先刷新物品数量（从全局状态）
      await itemStore.updateItemCount();
      if (this.itemCount === 0) {
        // 没有数据，提示用户
        this.showNoDataDialog = true;
      } else {
        // 有数据，显示确认对话框
        this.showConfirmDialog = true;
      }
    } catch (error) {
      console.error('检查物品数量失败:', error);
      this.showErrorToast('检查数据失败');
    }
  }

  // 执行清空操作 - 使用 ItemStore 的方法
  private async executeClearData(): Promise<void> {
    if (this.isClearing) {
      return; // 防止重复点击
    }
    this.isClearing = true;
    this.showConfirmDialog = false;
    try {
      console.info('开始清空数据库...');
      // 使用 ItemStore 的方法清空数据库
      await itemStore.clearAllItems();
      console.info('数据库清空成功');
      // 显示成功提示
      this.showSuccessDialog = true;
    } catch (error) {
      console.error('清空数据失败:', error);
      this.showErrorToast('清空失败，请重试');
    } finally {
      this.isClearing = false;
    }
  }

  // 显示错误提示
  private showErrorToast(message: string): void {
    try {
      promptAction.showToast({
        message: message,
        duration: 1500
      });
    } catch (error) {
      console.error('显示Toast出错:', error);
    }
  }

  // 处理编辑资料
  private handleEditProfile(): void {
    this.nicknameInput = this.userInfo.nickname;
    this.showEditDialog = true;
  }

  // 保存昵称修改
  private saveNickname(): void {
    if (this.nicknameInput.trim().length === 0) {
      this.showErrorToast('昵称不能为空');
      return;
    }
    if (this.nicknameInput.length > 12) {
      this.showErrorToast('昵称不能超过12个字符');
      return;
    }
    // 更新 @State 变量，触发UI刷新
    this.userInfo.nickname = this.nicknameInput.trim();
    this.showEditDialog = false;
    this.showToast('昵称修改成功');
  }

  // 显示Toast消息
  private showToast(message: string): void {
    try {
      promptAction.showToast({
        message: message,
        duration: 1500
      });
    } catch (error) {
      console.error('显示Toast出错:', error);
    }
  }

  // 安全执行清空操作
  private safeExecuteClearData(): void {
    this.executeClearData().catch((error: Error) => {
      console.error('执行清空操作出错:', error);
    });
  }

  // 安全处理清空数据
  private safeHandleClearData(): void {
    this.handleClearData().catch((error: Error) => {
      console.error('处理清空数据出错:', error);
    });
  }

  // ========== 主要构建方法 ==========
  // 个人资料
  @Builder
  private buildUserInfoCard() {
    Column() {
      Row({ space: 16 }) {
        // 简约头像
        Column() {
          Column() {
            Text('👤')
              .fontSize(40)
              .fontColor('#4A6CF7')
          }
          .width(80)
          .height(80)
          .backgroundColor('#F0F5FF')
          .borderRadius(40)
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center)
          .shadow({ radius: 6, color: '#20000000', offsetY: 3 })
        }
        .width(80)
        .height(80)

        // 用户信息
        Column({ space: 8 }) {
          // 昵称和编辑图标（内联设计）
          Row({ space: 4 }) {
            // 昵称（可点击）
            Text(this.userInfo.nickname)
              .fontSize(22)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333')
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.Ellipsis })
              .onClick(() => {
                this.handleEditProfile();
              })

            // 小笔图标（可点击）
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

          // 加入时间和物品数量
          Row({ space: 24 }) {
            Column({ space: 2 }) {
              Text('加入时间')
                .fontSize(12)
                .fontColor('#666')
              Text(this.userInfo.joinDate)
                .fontSize(14)
                .fontColor('#333')
                .fontWeight(FontWeight.Medium)
            }

            Column({ space: 2 }) {
              Text('物品数量')
                .fontSize(12)
                .fontColor('#666')
              Text(`${this.itemCount}个`)
                .fontSize(14)
                .fontColor('#333')
                .fontWeight(FontWeight.Medium)
            }
          }
          .margin({ top: 8 })
        }
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)
      }
      .padding({ left: 20, right: 20, top: 24, bottom: 24 })
    }
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(20)
    .margin({ top: 16, bottom: 16 })
    .shadow({ radius: 12, color: '#10000000', offsetY: 4 })
  }

  // 数据管理卡片
  @Builder
  private buildDataManagementCard() {
    Column() {
      Text('数据管理')
        .fontSize(14)
        .fontColor('#666')
        .margin({ bottom: 8 })
        .width('100%')

      Column() {
        // 直接构建清空数据项
        Row() {
          // 图标
          Text('🗑️')
            .fontSize(20)
            .margin({ right: 12 })
            .width(24)

          // 内容
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

          // 右侧箭头
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

  // 关于与帮助卡片
  @Builder
  private buildAboutCard() {
    Column() {
      Text('关于与帮助')
        .fontSize(14)
        .fontColor('#666')
        .margin({ bottom: 8 })
        .width('100%')

      Column() {
        // 应用信息
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

        // 帮助
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

        // 开发者信息
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

  // 底部信息
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

  // ========== 对话框构建方法 ==========
  // 清空数据确认对话框
  @Builder
  private buildConfirmDialog() {
    Column() {
      Column() {
        // 标题
        Text('确认清空数据')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ bottom: 12 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 警告图标
        Text('⚠️')
          .fontSize(48)
          .margin({ bottom: 16 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 数据统计
        Text(`当前共有 ${this.itemCount} 个物品记录`)
          .fontSize(14)
          .fontColor('#666')
          .margin({ bottom: 16 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 警告说明
        Text('此操作将永久删除所有物品数据，且无法恢复！')
          .fontSize(14)
          .fontColor('#F53F3F')
          .margin({ bottom: 24 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 按钮区域
        Row() {
          // 取消按钮
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

          // 确认清空按钮
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

  // 无数据提示对话框
  @Builder
  private buildNoDataDialog() {
    Column() {
      Column() {
        // 信息图标
        Text('📭')
          .fontSize(48)
          .margin({ bottom: 16 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 标题
        Text('暂无数据')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ bottom: 8 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 提示信息
        Text('当前没有物品记录，无需清空')
          .fontSize(14)
          .fontColor('#666')
          .margin({ bottom: 24 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 确定按钮
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

  // 清空成功对话框
  @Builder
  private buildSuccessDialog() {
    Column() {
      Column() {
        // 成功图标
        Text('✅')
          .fontSize(48)
          .margin({ bottom: 16 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 标题
        Text('数据已清空')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ bottom: 8 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 提示信息
        Text('所有物品数据已被安全清除')
          .fontSize(14)
          .fontColor('#666')
          .margin({ bottom: 24 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 确定按钮
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

  // 编辑资料对话框
  @Builder
  private buildEditDialog() {
    Column() {
      Column() {
        // 标题
        Text('编辑资料')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ bottom: 20 })
          .width('100%')
          .textAlign(TextAlign.Center)

        // 头像
        Row() {
          Column() {
            Text('👤')
              .fontSize(32)
              .fontColor('#4A6CF7')
          }
          .width(64)
          .height(64)
          .backgroundColor('#F0F5FF')
          .borderRadius(32)
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center)
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

          TextInput({ text: this.nicknameInput, placeholder: '请输入昵称' })
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
          // 取消按钮
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

          // 保存按钮
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



  // 帮助对话框
  @Builder
  private buildHelpDialog() {
    Column() {
      Column() {
        // 标题栏
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

        // 内容区域
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

            // 步骤1
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

            // 步骤2
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

            // 步骤3
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

            // 步骤4
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

            // 步骤5
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

            // 使用建议
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

        // 开始使用按钮
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

  // 加载中遮罩
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

  // ========== 主构建方法 ==========
  build() {
    Column() {
      // 滚动内容区域
      Scroll() {
        Column() {
          // 用户信息卡片
          this.buildUserInfoCard()
          // 数据管理卡片
          this.buildDataManagementCard()
          // 关于与帮助卡片
          this.buildAboutCard()
          // 底部版权信息
          this.buildFooter()
        }
        .padding({ left: 16, right: 16, bottom: 32 })
      }
      .scrollBar(BarState.Off)
      .layoutWeight(1)

      // 3. 清空数据确认对话框
      if (this.showConfirmDialog) {
        this.buildConfirmDialog()
      }

      // 4. 无数据提示对话框
      if (this.showNoDataDialog) {
        this.buildNoDataDialog()
      }

      // 5. 清空成功对话框
      if (this.showSuccessDialog) {
        this.buildSuccessDialog()
      }

      // 6. 编辑资料对话框
      if (this.showEditDialog) {
        this.buildEditDialog()
      }

      // 7. 帮助对话框
      if (this.showHelpDialog) {
        this.buildHelpDialog()
      }

      // 8. 加载中遮罩
      if (this.isClearing) {
        this.buildLoadingOverlay()
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F9FAFB')
  }
}