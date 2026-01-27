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
import router from '@ohos.router';
import { PassType, PasswordManager, RouterUtil } from '../common/Config';
import { PromptAction, UIContext } from '@kit.ArkUI';
import { BioAuthManager } from '../common/BioAuthConfig';
import { AlertDialog } from '@kit.ArkUI';

enum FingerprintStatus {
  IDLE = 'idle',
  WAITING = 'waiting',
  TOUCHED = 'touched',
  SCANNING = 'scanning',
  SUCCESS = 'success',
  FAILED = 'failed',
  LOCKED = 'locked'
}

@Entry
@Component
export struct FingerprintPage {
  private authType: PassType = PassType.FINGERPRINT;

  @State currentStatus: FingerprintStatus = FingerprintStatus.IDLE;
  @State progress: number = 0;
  @State attempts: number = 0;
  @State maxAttempts: number = 5;
  @State lockTime: number = 30;
  @State countdown: number = 0;
  @State isUsingMock: boolean = false;
  @State isSupported: boolean = false;

  uiContext: UIContext = this.getUIContext();
  promptAction: PromptAction = this.uiContext.getPromptAction();

  private resetTimer?: number;
  private scanTimer?: number;
  private lockTimer?: number;

  dialoglockFingerprint: CustomDialogController = new CustomDialogController({
    builder: AlertDialog({
      primaryTitle: '指纹识别已锁定',
      content: `连续失败${this.maxAttempts}次，请${this.lockTime}秒后再试`,
      primaryButton: {
        value: '使用其他方式',
        fontColor: '#007DFF',
        action: () => router.pushUrl({ url: 'pages/PasswordEntry' })
      },
      secondaryButton: {
        value: '知道了',
        fontColor: '#007DFF',
        action: () => router.pushUrl({ url: 'pages/Index' })
      }
    })
  });

  get isLocked(): boolean {
    return this.currentStatus === FingerprintStatus.LOCKED;
  }

  get canRetry(): boolean {
    return this.currentStatus === FingerprintStatus.FAILED &&
      this.attempts < this.maxAttempts;
  }

  async aboutToAppear() {
    const params = router.getParams() as Record<string, string>;
    if (params?.authType) {
      this.authType = params.authType as PassType;
    }
    await this.initFingerprintEnv();
  }

  aboutToDisappear() {
    this.clearAllTimers();
  }

  private clearAllTimers() {
    if (this.resetTimer) {
      clearTimeout(this.resetTimer);
      this.resetTimer = undefined;
    }
    if (this.scanTimer) {
      clearInterval(this.scanTimer);
      this.scanTimer = undefined;
    }
    if (this.lockTimer) {
      clearInterval(this.lockTimer);
      this.lockTimer = undefined;
    }
  }

  async initFingerprintEnv() {
    const ctx = this.uiContext.getHostContext();
    const isPreview = ctx?.applicationInfo?.debug === true;

    if (isPreview) {
      this.isUsingMock = true;
      this.isSupported = true;
      console.log('指纹认证: Preview 模式，使用模拟指纹');
      return;
    }

    this.isUsingMock = false;

    const supported = await BioAuthManager.checkFingerprintSupport();
    this.isSupported = supported;

    if (!supported) {
      this.promptAction.openToast({
        message: '当前设备不支持指纹识别',
        duration: 2000
      });
    }

    console.log(
      '指纹认证: 真机模式，指纹支持:',
      supported
    );
  }

  async startFingerprintAuth() {
    if (this.isLocked) {
      return;
    }

    if (!this.isUsingMock && !this.isSupported) {
      return;
    }


    if (this.currentStatus === FingerprintStatus.IDLE) {
      this.progress = 0;
    }

    this.currentStatus = FingerprintStatus.WAITING;

    if (this.isUsingMock) {
      this.simulateTouch();
      return;
    }

    try {
      const success = await BioAuthManager.authenticateFingerprint();
      success ? await this.handleSuccess() : await this.handleFailure();
    } catch (e) {
      console.error('指纹认证: 认证异常', e);
      await this.handleFailure();
    }
  }

  simulateTouch() {
    this.currentStatus = FingerprintStatus.TOUCHED;

    this.resetTimer = setTimeout(() => {
      this.scanFingerprint();
    }, 500);
  }

  async scanFingerprint() {
    this.progress = 0;
    this.currentStatus = FingerprintStatus.SCANNING;

    this.scanTimer = setInterval(() => {
      this.progress += 10;
      if (this.progress >= 100) {
        clearInterval(this.scanTimer);
        this.scanTimer = undefined;
        this.processScanResult();
      }
    }, 100);
  }

  async processScanResult() {
    const isSuccess = Math.random() > 0.3;
    isSuccess ? await this.handleSuccess() : await this.handleFailure();
  }

  async handleSuccess() {
    this.clearAllTimers();

    this.currentStatus = FingerprintStatus.SUCCESS;
    this.attempts = 0;

    await this.playSuccessAnimation();
    PasswordManager.setLastUsedAuth(this.authType);

    this.promptAction.openToast({
      message: '指纹验证成功！',
      duration: 1000
    });

    await new Promise<void>(resolve => setTimeout(resolve, 1000));
    RouterUtil.goToHome();
  }

  async handleFailure() {
    this.clearAllTimers();

    this.attempts++;
    this.currentStatus = FingerprintStatus.FAILED;

    if (this.attempts >= this.maxAttempts) {
      await this.lockFingerprint();
      return;
    }

    const remaining = this.maxAttempts - this.attempts;
    this.promptAction.openToast({
      message: `指纹识别失败，还剩${remaining}次尝试`,
      duration: 2000
    });

    await this.playErrorAnimation();

    this.resetTimer = setTimeout(() => {
      this.resetScanner();
    }, 3000);
  }

  async lockFingerprint() {
    this.clearAllTimers();

    this.currentStatus = FingerprintStatus.LOCKED;
    this.countdown = this.lockTime;
    this.dialoglockFingerprint.open();

    this.lockTimer = setInterval(() => {
      this.countdown--;
      if (this.countdown <= 0) {
        clearInterval(this.lockTimer);
        this.lockTimer = undefined;
        this.resetLock();
      }
    }, 1000);
  }

  resetLock() {
    this.currentStatus = FingerprintStatus.IDLE;
    this.attempts = 0;
    this.countdown = 0;

    this.promptAction.openToast({
      message: '指纹识别已解锁，可以重试',
      duration: 2000
    });
  }

  resetScanner() {
    if (!this.isLocked) {
      this.currentStatus = FingerprintStatus.IDLE;
      this.progress = 0;
    }
  }

  private async playSuccessAnimation() {
    for (let i = 0; i < 3; i++) {
      await new Promise<void>(r => setTimeout(r, 150));
    }
  }

  private async playErrorAnimation() {
    for (let i = 0; i < 3; i++) {
      await new Promise<void>(r => setTimeout(r, 100));
    }
  }

  goBack() {
    router.back();
  }

  switchToOtherAuth() {
    router.pushUrl({ url: 'pages/PasswordEntry' });
  }

  getStatusText(): string {
    switch (this.currentStatus) {
      case FingerprintStatus.IDLE:
        return '点击开始指纹识别';
      case FingerprintStatus.WAITING:
        return '等待手指触摸...';
      case FingerprintStatus.TOUCHED:
        return '手指已就位';
      case FingerprintStatus.SCANNING:
        return `扫描中... ${this.progress}%`;
      case FingerprintStatus.SUCCESS:
        return '识别成功！';
      case FingerprintStatus.FAILED:
        return `识别失败 (${this.attempts}/${this.maxAttempts})`;
      case FingerprintStatus.LOCKED:
        return `已锁定，${this.countdown}秒后重试`;
      default:
        return '指纹识别';
    }
  }

  // 获取指纹图标颜色
  getFingerprintColor(): string {
    switch (this.currentStatus) {
      case FingerprintStatus.SUCCESS:
        return '#4CD964'; // 绿色
      case FingerprintStatus.FAILED:
        return '#FF3B30'; // 红色
      case FingerprintStatus.SCANNING:
        return '#007AFF'; // 蓝色
      case FingerprintStatus.LOCKED:
        return '#8E8E93'; // 灰色
      default:
        return '#007AFF'; // 蓝色
    }
  }

  // 获取按钮文本
  getButtonText(): string {
    if (this.isLocked) {
      return '已锁定';
    } else if (this.currentStatus === FingerprintStatus.SCANNING) {
      return '扫描中...';
    } else if (this.currentStatus === FingerprintStatus.SUCCESS) {
      return '验证成功';
    } else if (this.currentStatus === FingerprintStatus.FAILED && this.canRetry) {
      return '重试';
    } else {
      return '开始指纹识别';
    }
  }

  // 按钮是否可用
  isButtonEnabled(): boolean {
    return !this.isLocked &&
      this.currentStatus !== FingerprintStatus.SCANNING &&
      this.currentStatus !== FingerprintStatus.SUCCESS;
  }

  build() {
    Column({ space: 0 }) {
      // 顶部导航栏
      this.buildHeader()

      // 指纹识别区域
      this.buildFingerprintArea()

      // 进度和状态显示
      this.buildStatusArea()

      // 操作按钮区域
      this.buildActionArea()
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }

  // 构建顶部导航栏
  @Builder
  buildHeader() {
    Row({ space: 10 }) {
      // 返回按钮
      Button('←')
        .width(40)
        .height(40)
        .fontSize(20)
        .backgroundColor(Color.Transparent)
        .fontColor('#007DFF')
        .onClick(() => this.goBack())

      Text('指纹识别')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .layoutWeight(1)
        .textAlign(TextAlign.Center)

      // 模式指示器
      if (this.isUsingMock) {
        Text('模拟')
          .fontSize(12)
          .fontColor('#FF9500')
          .padding({ left: 8, right: 8, top: 4, bottom: 4 })
          .backgroundColor('#FFF3E0')
          .borderRadius(10)
      }
    }
    .width('100%')
    .padding({ left: 15, right: 15, top: 10, bottom: 10 })
    .backgroundColor(Color.White)
    .shadow({
      radius: 2,
      color: '#E0E0E0',
      offsetX: 0,
      offsetY: 1
    })
  }

  // 构建指纹识别区域
  @Builder
  buildFingerprintArea() {
    Column({ space: 20 }) {
      // 指纹图标
      Stack() {
        // 外圈
        Circle({ width: 200, height: 200 })
          .fill(this.getFingerprintColor())
          .opacity(0.1)

        // 指纹图标（使用SVG或自定义图形）
        Column({ space: 5 }) {
          // 指纹中心圆形
          Circle({ width: 60, height: 60 })
            .fill(this.getFingerprintColor())
            .opacity(0.3)

          // 指纹纹路（简化表示）
          this.buildFingerprintLines()
        }

        // 扫描动画效果
        if (this.currentStatus === FingerprintStatus.SCANNING) {
          Circle({ width: 200, height: 200 })
            .fill(this.getFingerprintColor())
            .opacity(0.05 * (this.progress / 100))
        }

        // 成功/失败覆盖层
        if (this.currentStatus === FingerprintStatus.SUCCESS ||
          this.currentStatus === FingerprintStatus.FAILED) {
          Column() {
            if (this.currentStatus === FingerprintStatus.SUCCESS) {
              Text('✓')
                .fontSize(50)
                .fontColor('#4CD964')
            } else {
              Text('✗')
                .fontSize(50)
                .fontColor('#FF3B30')
            }
          }
          .width(200)
          .height(200)
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center)
          .backgroundColor('#FFFFFF80')
          .borderRadius(100)
        }
      }
      .width(200)
      .height(200)
      .margin({ top: 40 })

      // 状态提示文本
      Text(this.getStatusText())
        .fontSize(18)
        .fontColor('#333')
        .margin({ top: 20 })

      // 详细描述
      if (this.currentStatus === FingerprintStatus.WAITING) {
        Text('请用手指触摸手机背部的指纹传感器')
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Center)
          .margin({ top: 10, left: 40, right: 40 })
      } else if (this.currentStatus === FingerprintStatus.SCANNING) {
        Text('正在扫描指纹特征...')
          .fontSize(14)
          .fontColor('#666')
          .margin({ top: 10 })
      }
    }
    .width('100%')
    .padding(20)
    .alignItems(HorizontalAlign.Center)
  }

  // 构建指纹纹路（简化图形）
  @Builder
  buildFingerprintLines() {
    Column({ space: 8 }) {
      // 顶部纹路
      Row({ space: 12 }) {
        ForEach(Array.from({ length: 5 }), (item: number, i: number) => {
          Line()
            .width(20)
            .height(4)
            .backgroundColor(this.getFingerprintColor())
        })
      }

      // 中部纹路
      Row({ space: 10 }) {
        ForEach(Array.from({ length: 6 }), (item: number, i: number) => {
          Line()
            .width(16)
            .height(4)
            .backgroundColor(this.getFingerprintColor())
        })
      }

      // 底部纹路
      Row({ space: 8 }) {
        ForEach(Array.from({ length: 7 }), (item: number, i: number) => {
          Line()
            .width(12)
            .height(4)
            .backgroundColor(this.getFingerprintColor())
        })
      }
    }
    .margin({ top: 10 })
  }

  // 构建状态区域
  @Builder
  buildStatusArea() {
    Column({ space: 15 }) {
      // 进度条（扫描时显示）
      if (this.currentStatus === FingerprintStatus.SCANNING) {
        Progress({
          value: this.progress,
          total: 100,
          type: ProgressType.Linear
        })
          .width('80%')
          .height(6)
          // 直接使用颜色值
          .color('#007AFF')  // 或者使用单一颜色
          .backgroundColor('#E0E0E0')
          .borderRadius(3)
      }

      // 尝试次数显示
      if (this.attempts > 0 && !this.isLocked) {
        Text(`尝试次数: ${this.attempts}/${this.maxAttempts}`)
          .fontSize(14)
          .fontColor(this.attempts >= 3 ? '#FF3B30' : '#FF9500')
      }

      // 锁定倒计时
      if (this.isLocked) {
        Column({ space: 10 }) {
          Text(`锁定剩余时间: ${this.countdown}秒`)
            .fontSize(16)
            .fontColor('#FF3B30')
            .fontWeight(FontWeight.Medium)

          Progress({
            value: this.countdown,
            total: this.lockTime,
            type: ProgressType.Ring
          })
            .width(80)
            .height(80)
            .color('#FF3B30')
        }
        .margin({ top: 20 })
      }

      // 使用提示
      if (this.currentStatus === FingerprintStatus.IDLE) {
        Column({ space: 8 }) {
          Text('使用提示:')
            .fontSize(14)
            .fontColor('#666')
            .fontWeight(FontWeight.Medium)

          Text('1. 保持手指干燥清洁')
            .fontSize(12)
            .fontColor('#666')

          Text('2. 完全覆盖传感器区域')
            .fontSize(12)
            .fontColor('#666')

          Text('3. 不要用力按压')
            .fontSize(12)
            .fontColor('#666')
        }
        .padding(15)
        .backgroundColor('#F8F8F8')
        .borderRadius(10)
        .margin({ top: 20, left: 30, right: 30 })
      }
    }
    .width('100%')
    .padding(20)
    .alignItems(HorizontalAlign.Center)
  }

  // 构建操作按钮区域
  @Builder
  buildActionArea() {
    Column({ space: 20 }) {
      // 主操作按钮
      Button(this.getButtonText())
        .width('80%')
        .height(50)
        .backgroundColor(this.isButtonEnabled() ? '#007DFF' : '#CCCCCC')
        .fontSize(18)
        .fontColor(Color.White)
        .borderRadius(25)
        .enabled(this.isButtonEnabled())
        .onClick(() => this.startFingerprintAuth())

      // 切换认证方式按钮
      if (this.currentStatus !== FingerprintStatus.SCANNING &&
        this.currentStatus !== FingerprintStatus.SUCCESS) {
        Button('使用其他方式认证')
          .width('80%')
          .height(40)
          .backgroundColor(Color.Transparent)
          .fontSize(14)
          .fontColor('#666')
          .onClick(() => this.switchToOtherAuth())
      }

      // 技术支持信息
      if (this.isUsingMock) {
        Text('当前为模拟模式，真实设备会自动调用指纹API')
          .fontSize(11)
          .fontColor('#999')
          .textAlign(TextAlign.Center)
          .margin({ top: 20, left: 30, right: 30 })
      }
    }
    .width('100%')
    .padding({ top: 20, bottom: 40 })
    .alignItems(HorizontalAlign.Center)
  }
}