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
import { BioAuthManager, BioAuthConfig } from '../common/BioAuthConfig';
import { PromptAction, UIContext } from '@kit.ArkUI';
import { AlertDialog } from '@kit.ArkUI';

enum FaceRecStatus {
  IDLE = 'idle',
  PREPARING = 'preparing',
  POSITIONING = 'positioning',
  SCANNING = 'scanning',
  VERIFYING = 'verifying',
  SUCCESS = 'success',
  FAILED = 'failed',
  LOCKED = 'locked'
}

enum FacePosition {
  TOO_CLOSE = 'too_close',
  TOO_FAR = 'too_far',
  TOO_LEFT = 'too_left',
  TOO_RIGHT = 'too_right',
  TOO_UP = 'too_up',
  TOO_DOWN = 'too_down',
  PERFECT = 'perfect'
}

@Entry
@Component
export struct FaceRecPage {
  private authType: PassType = PassType.FACE;

  @State currentStatus: FaceRecStatus = FaceRecStatus.IDLE;
  @State facePosition: FacePosition = FacePosition.PERFECT;
  @State progress: number = 0;
  @State scanQuality: number = 80;
  @State attempts: number = 0;
  @State maxAttempts: number = BioAuthConfig.FACE.MAX_ATTEMPTS;
  @State lockTime: number = BioAuthConfig.FACE.LOCK_TIME;
  @State countdown: number = 0;
  @State isUsingMock: boolean = true;
  @State faceBoxX: number = 0;
  @State faceBoxY: number = 0;
  @State faceBoxSize: number = 100;
  @State showGuide: boolean = true;
  @State faceSupported: boolean = true;

  uiContext: UIContext = this.getUIContext();
  promptAction: PromptAction = this.uiContext.getPromptAction();

  private prepareTimer?: number;
  private positionTimer?: number;
  private scanTimer?: number;
  private resetTimer?: number;
  private lockTimer?: number;

  dialoglockFace: CustomDialogController = new CustomDialogController({
    builder: AlertDialog({
      primaryTitle: '人脸识别已锁定',
      content: `连续失败${this.maxAttempts}次，请${this.lockTime}秒后再试\n建议：确保环境光线充足，面部无遮挡`,
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
    return this.currentStatus === FaceRecStatus.LOCKED;
  }

  get canStartScan(): boolean {
    if (!this.faceSupported) return false;
    return !this.isLocked && this.currentStatus === FaceRecStatus.IDLE;
  }

  get isScanning(): boolean {
    return this.currentStatus === FaceRecStatus.SCANNING ||
      this.currentStatus === FaceRecStatus.VERIFYING;
  }

  aboutToAppear() {
    const params = router.getParams() as Record<string, string>;
    if (params?.authType) {
      this.authType = params.authType as PassType;
    }

    this.checkFaceRecSupport();
    this.faceBoxX = 150;
    this.faceBoxY = 150;
  }

  aboutToDisappear() {
    this.clearAllTimers();
  }

  private clearAllTimers() {
    if (this.prepareTimer) clearTimeout(this.prepareTimer);
    if (this.positionTimer) clearInterval(this.positionTimer);
    if (this.scanTimer) clearInterval(this.scanTimer);
    if (this.resetTimer) clearTimeout(this.resetTimer);
    if (this.lockTimer) clearInterval(this.lockTimer);

    this.prepareTimer = undefined;
    this.positionTimer = undefined;
    this.scanTimer = undefined;
    this.resetTimer = undefined;
    this.lockTimer = undefined;
  }

  async checkFaceRecSupport() {
    const ctx = this.uiContext.getHostContext();
    const isDebug = !!ctx?.applicationInfo?.debug;

    if (isDebug) {
      this.isUsingMock = true;
      this.faceSupported = true;
      console.info('人脸识别: Preview 模拟模式');
      return;
    }

    const supported = await BioAuthManager.checkFaceSupport();
    this.faceSupported = supported;
    this.isUsingMock = false;

    if (!supported) {
      this.promptAction.openToast({
        message: '当前设备不支持人脸识别，请使用其他方式',
        duration: 2500
      });
    }

    console.info('人脸识别: ', supported ? '真机支持人脸' : '真机不支持人脸');
  }


  async startFaceRecognition() {
    if (!this.canStartScan) return;

    this.showGuide = false;

    if (this.isUsingMock) {
      this.currentStatus = FaceRecStatus.PREPARING;
      this.prepareTimer = setTimeout(() => {
        this.currentStatus = FaceRecStatus.POSITIONING;
        this.startFacePositioning();
      }, 800);
    } else {
      this.currentStatus = FaceRecStatus.VERIFYING;
      this.processFaceScan();
    }
  }

  startFacePositioning() {
    this.positionTimer = setInterval(() => {
      const offsetX = Math.random() * 80 - 40;
      const offsetY = Math.random() * 60 - 30;
      const offsetSize = Math.random() * 30 - 15;

      this.faceBoxX = 150 + offsetX;
      this.faceBoxY = 150 + offsetY;
      this.faceBoxSize = 100 + offsetSize;

      this.updateFacePosition(offsetX, offsetY, offsetSize);

      if (this.facePosition === FacePosition.PERFECT) {
        clearInterval(this.positionTimer);
        this.positionTimer = undefined;
        this.startFaceScanning();
      }
    }, 200);
  }

  updateFacePosition(offsetX: number, offsetY: number, offsetSize: number) {
    if (offsetSize > 20) {
      this.facePosition = FacePosition.TOO_CLOSE;
    } else if (offsetSize < -15) {
      this.facePosition = FacePosition.TOO_FAR;
    } else if (offsetX > 30) {
      this.facePosition = FacePosition.TOO_RIGHT;
    } else if (offsetX < -30) {
      this.facePosition = FacePosition.TOO_LEFT;
    } else if (offsetY > 20) {
      this.facePosition = FacePosition.TOO_DOWN;
    } else if (offsetY < -20) {
      this.facePosition = FacePosition.TOO_UP;
    } else {
      this.facePosition = FacePosition.PERFECT;
    }
  }


  async startFaceScanning() {
    this.currentStatus = FaceRecStatus.SCANNING;
    this.progress = 0;

    this.scanTimer = setInterval(() => {
      this.progress += 5;
      this.scanQuality = 70 + Math.random() * 30;

      if (this.progress >= 100) {
        clearInterval(this.scanTimer);
        this.scanTimer = undefined;
        this.processFaceScan();
      }
    }, 100);
  }

  async processFaceScan() {
    this.currentStatus = FaceRecStatus.VERIFYING;

    this.prepareTimer = setTimeout(async () => {
      if (this.isUsingMock) {
        await this.handleMockVerification();
      } else {
        await this.handleRealVerification();
      }
    }, 600);
  }

  async handleMockVerification() {
    const isSuccess = Math.random() < (0.7 * this.scanQuality / 100);
    isSuccess ? await this.handleSuccess() : await this.handleFailure();
  }

  async handleRealVerification() {
    try {
      const success = await BioAuthManager.authenticateFace();
      success ? await this.handleSuccess() : await this.handleFailure();
    } catch (e) {
      console.error('人脸识别异常', e);
      await this.handleFailure();
    }
  }

  async handleSuccess() {
    this.clearAllTimers();

    this.currentStatus = FaceRecStatus.SUCCESS;
    this.attempts = 0;

    await this.playSuccessAnimation();

    PasswordManager.setLastUsedAuth(this.authType);
    await new Promise<void>(r => setTimeout(r, 800));
    RouterUtil.goToHome();
  }

  async handleFailure() {
    this.clearAllTimers();

    this.attempts++;
    await this.playErrorAnimation();
    this.promptAction.openToast({
      message: `人脸识别失败，还可尝试 ${this.maxAttempts - this.attempts} 次`,
      duration: 2000
    });
    this.currentStatus = FaceRecStatus.FAILED;

    if (this.attempts >= this.maxAttempts) {
      await this.lockFaceRecognition();
      return;
    }

    this.resetTimer = setTimeout(() => {
      this.resetScanner();
    }, 3000);
  }

  async lockFaceRecognition() {
    this.clearAllTimers();

    this.currentStatus = FaceRecStatus.LOCKED;
    this.countdown = this.lockTime;
    this.dialoglockFace.open();

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
    this.currentStatus = FaceRecStatus.IDLE;
    this.attempts = 0;
    this.countdown = 0;
    this.showGuide = true;

    this.promptAction.openToast({
      message: '人脸识别已解锁，可以重试',
      duration: 2000
    });
  }

  resetScanner() {
    if (!this.isLocked) {
      this.currentStatus = FaceRecStatus.IDLE;
      this.progress = 0;
      this.facePosition = FacePosition.PERFECT;
      this.showGuide = true;
      this.faceBoxX = 150;
      this.faceBoxY = 150;
      this.faceBoxSize = 100;
    }
  }

// 播放成功动画
  private async playSuccessAnimation() {
    // 人脸框变为绿色并放大
    for (let i = 0; i < 3; i++) {
      await new Promise<void>(resolve => setTimeout(resolve, 200));
    }
  }

  // 播放失败动画
  private async playErrorAnimation() {
    // 人脸框闪烁红色
    for (let i = 0; i < 3; i++) {
      await new Promise<void>(resolve => setTimeout(resolve, 150));
    }
  }

  // 获取位置提示文本
  getPositionText(): string {
    switch (this.facePosition) {
      case FacePosition.TOO_CLOSE:
        return '请稍微远离摄像头';
      case FacePosition.TOO_FAR:
        return '请靠近一些';
      case FacePosition.TOO_LEFT:
        return '请向右移动一点';
      case FacePosition.TOO_RIGHT:
        return '请向左移动一点';
      case FacePosition.TOO_UP:
        return '请稍微向下移动';
      case FacePosition.TOO_DOWN:
        return '请稍微向上移动';
      case FacePosition.PERFECT:
        return '位置完美，保持不动';
      default:
        return '请将面部置于框内';
    }
  }

  // 获取状态描述文本
  getStatusText(): string {
    switch (this.currentStatus) {
      case FaceRecStatus.IDLE:
        return '点击开始人脸识别';
      case FaceRecStatus.PREPARING:
        return '准备中...';
      case FaceRecStatus.POSITIONING:
        return '正在定位人脸...';
      case FaceRecStatus.SCANNING:
        return `扫描中... ${this.progress}%`;
      case FaceRecStatus.VERIFYING:
        return '正在验证身份...';
      case FaceRecStatus.SUCCESS:
        return '识别成功！';
      case FaceRecStatus.FAILED:
        return `识别失败 (${this.attempts}/${this.maxAttempts})`;
      case FaceRecStatus.LOCKED:
        return `已锁定，${this.countdown}秒后重试`;
      default:
        return '人脸识别';
    }
  }

  // 获取人脸框颜色
  getFaceBoxColor(): string {
    switch (this.currentStatus) {
      case FaceRecStatus.SUCCESS:
        return '#4CD964'; // 绿色
      case FaceRecStatus.FAILED:
        return '#FF3B30'; // 红色
      case FaceRecStatus.SCANNING:
      case FaceRecStatus.VERIFYING:
        return '#007AFF'; // 蓝色
      case FaceRecStatus.POSITIONING:
        return this.facePosition === FacePosition.PERFECT ? '#4CD964' : '#FF9500'; // 绿色或橙色
      case FaceRecStatus.LOCKED:
        return '#8E8E93'; // 灰色
      default:
        return '#007AFF'; // 蓝色
    }
  }

  // 获取按钮文本
  getButtonText(): string {
    if (!this.faceSupported) {
      return '设备不支持人脸识别';
    }
    if (this.isLocked) {
      return '已锁定';
    } else if (this.isScanning) {
      return '识别中...';
    } else if (this.currentStatus === FaceRecStatus.SUCCESS) {
      return '验证成功';
    } else if (this.currentStatus === FaceRecStatus.FAILED && this.attempts < this.maxAttempts) {
      return '重新尝试';
    } else {
      return '开始人脸识别';
    }
  }

  // 按钮是否可用
  isButtonEnabled(): boolean {
    return this.canStartScan;
  }

  // 返回上一页
  goBack() {
    router.back();
  }

  // 切换到其他认证方式
  switchToOtherAuth() {
    router.pushUrl({
      url: 'pages/PasswordEntry'
    });
  }

  build() {
    Column({ space: 0 }) {
      // 顶部导航栏
      this.buildHeader()

      // 人脸识别区域
      this.buildFaceRecArea()

      // 状态和控制区域
      this.buildControlArea()
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#000000') // 黑色背景模拟摄像头预览
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
        .backgroundColor('#1A1A1A')  // 深灰色背景
        .fontColor(Color.White)
        .borderRadius(20)
        .border({ width: 1, color: '#FFFFFF40' }) // 可选：加个浅色边框
        .onClick(() => this.goBack())

      Text('人脸识别')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor(Color.White)
        .layoutWeight(1)
        .textAlign(TextAlign.Center)

      // 模式指示器
      if (this.isUsingMock) {
        Text('模拟')
          .fontSize(12)
          .fontColor('#FF9500')
          .padding({ left: 8, right: 8, top: 4, bottom: 4 })
          .backgroundColor('#FFF3E080')
          .borderRadius(10)
      }
    }
    .width('100%')
    .padding({ left: 15, right: 15, top: 10, bottom: 10 })
    .backgroundColor('#00000080')
  }

  // 构建人脸识别区域
  @Builder
  buildFaceRecArea() {
    Stack() {
      // 摄像头预览背景（模拟）
      Column()
        .width('100%')
        .height(400)
        .backgroundColor('#000000')
        .border({
          width: 2,
          color: '#555555',
          radius: 8,
          style: BorderStyle.Dashed // 虚线边框
        })
      // 引导框和提示
      if (this.showGuide && this.currentStatus === FaceRecStatus.IDLE) {
        this.buildGuideOverlay()
      }

      // 人脸定位框（模拟）
      if (this.isUsingMock &&
        this.currentStatus === FaceRecStatus.POSITIONING ||
        this.currentStatus === FaceRecStatus.SCANNING ||
        this.currentStatus === FaceRecStatus.VERIFYING) {
        this.buildFaceBox()
      }

      // 扫描效果
      if (this.isUsingMock && this.currentStatus === FaceRecStatus.SCANNING) {
        this.buildScanningEffect()
      }

      // 成功/失败覆盖层
      if (this.currentStatus === FaceRecStatus.SUCCESS ||
        this.currentStatus === FaceRecStatus.FAILED) {
        this.buildResultOverlay()
      }
    }
    .width('100%')
    .height(400)
    .margin({ top: 10 })
  }

  // 构建引导覆盖层
  @Builder
  buildGuideOverlay() {
    Column({ space: 20 }) {
      // 人脸图标
      Text('👤')
        .fontSize(60)
        .margin({ bottom: 10 })

      Text('人脸识别')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor(Color.White)

      Text('请将面部对准摄像头')
        .fontSize(16)
        .fontColor('#CCCCCC')
        .margin({ bottom: 20 })

      // 引导提示列表
      Column({ space: 12 }) {
        Row({ space: 10 }) {
          Text('✓')
            .fontSize(16)
            .fontColor('#4CD964')
          Text('确保环境光线充足')
            .fontSize(14)
            .fontColor('#CCCCCC')
        }

        Row({ space: 10 }) {
          Text('✓')
            .fontSize(16)
            .fontColor('#4CD964')
          Text('面部无遮挡（眼镜、口罩等）')
            .fontSize(14)
            .fontColor('#CCCCCC')
        }

        Row({ space: 10 }) {
          Text('✓')
            .fontSize(16)
            .fontColor('#4CD964')
          Text('保持手机稳定')
            .fontSize(14)
            .fontColor('#CCCCCC')
        }
      }
      .alignItems(HorizontalAlign.Start)
      .margin({ left: 50, right: 50 })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .backgroundColor('#00000080')
  }

  // 构建人脸框
  @Builder
  buildFaceBox() {
    Stack() {
      // 外框
      Column()
        .width(this.faceBoxSize)
        .height(this.faceBoxSize)
        .border({
          width: 3,
          color: this.getFaceBoxColor(),
          style: BorderStyle.Solid
        })
        .borderRadius(20)
        .position({ x: this.faceBoxX - this.faceBoxSize/2, y: this.faceBoxY - this.faceBoxSize/2 })

      // 四个角
      // 左上角
      Column()
        .width(20)
        .height(20)
        .border({
          width: 4,
          color: this.getFaceBoxColor(),
          style: BorderStyle.Solid
        })
        // 分别设置每个角的圆角
        .borderRadius({
          topLeft: 0,        // 左上角不圆角
          topRight: 4,
          bottomLeft: 4,
          bottomRight: 4
        })
        .clip(true)
        .position({ x: this.faceBoxX - this.faceBoxSize/2 - 10, y: this.faceBoxY - this.faceBoxSize/2 - 10 })
      // 右上角
      Column()
        .width(20)
        .height(20)
        .border({
          width: 4,
          color: this.getFaceBoxColor(),
          style: BorderStyle.Solid
        })
        .borderRadius(4)
        // 分别设置每个角的圆角
        .borderRadius({
          topLeft: 4,        // 右上角不圆角
          topRight: 0,
          bottomLeft: 4,
          bottomRight: 4
        })
        .clip(true)
        .position({ x: this.faceBoxX + this.faceBoxSize/2 - 10, y: this.faceBoxY - this.faceBoxSize/2 - 10 })

      // 左下角
      Column()
        .width(20)
        .height(20)
        .border({
          width: 4,
          color: this.getFaceBoxColor(),
          style: BorderStyle.Solid
        })
        .borderRadius(4)
        // 分别设置每个角的圆角
        .borderRadius({
          topLeft: 4,        // 左下角不圆角
          topRight: 4,
          bottomLeft: 0,
          bottomRight: 4
        })
        .clip(true)
        .position({ x: this.faceBoxX - this.faceBoxSize/2 - 10, y: this.faceBoxY + this.faceBoxSize/2 - 10 })

      // 右下角
      Column()
        .width(20)
        .height(20)
        .border({
          width: 4,
          color: this.getFaceBoxColor(),
          style: BorderStyle.Solid
        })
        .borderRadius(4)
        // 分别设置每个角的圆角
        .borderRadius({
          topLeft: 4,        // 右下角不圆角
          topRight: 4,
          bottomLeft: 4,
          bottomRight: 0
        })
        .clip(true)
        .position({ x: this.faceBoxX + this.faceBoxSize/2 - 10, y: this.faceBoxY + this.faceBoxSize/2 - 10 })

      // 位置提示文本
      if (this.currentStatus === FaceRecStatus.POSITIONING) {
        Text(this.getPositionText())
          .fontSize(16)
          .fontColor(this.getFaceBoxColor())
          .fontWeight(FontWeight.Medium)
          .position({ x: this.faceBoxX - 100, y: this.faceBoxY + this.faceBoxSize/2 + 20 })
          .width(200)
          .textAlign(TextAlign.Center)
      }
    }
  }

  // 构建扫描效果
  @Builder
  buildScanningEffect() {
    // 扫描线效果
    Column()
      .width(this.faceBoxSize)
      .height(4)
      .backgroundColor('#007AFF80')
      .position({
        x: this.faceBoxX - this.faceBoxSize/2,
        y: this.faceBoxY - this.faceBoxSize/2 + (this.progress/100) * this.faceBoxSize
      })

    // 质量指示器
    Text(`质量: ${this.scanQuality.toFixed(0)}%`)
      .fontSize(14)
      .fontColor('#FFFFFF')
      .position({ x: this.faceBoxX + this.faceBoxSize/2 + 20, y: this.faceBoxY })
  }

  // 构建结果覆盖层
  @Builder
  buildResultOverlay() {
    Column({ space: 15 }) {
      if (this.currentStatus === FaceRecStatus.SUCCESS) {
        Text('✓')
          .fontSize(60)
          .fontColor('#4CD964')

        Text('验证成功')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#4CD964')
      } else {
        Text('✗')
          .fontSize(60)
          .fontColor('#FF3B30')

        Text('验证失败')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#FF3B30')
      }
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .backgroundColor('#00000080')
  }

  // 构建控制区域
  @Builder
  buildControlArea() {
    Column({ space: 20 }) {
      // 状态文本
      Text(this.getStatusText())
        .fontSize(18)
        .fontColor(Color.White)
        .fontWeight(FontWeight.Medium)
        .margin({ top: 20 })

      // 进度条（扫描时显示）
      if (this.isScanning) {
        Column({ space: 10 }) {
          Progress({
            value: this.progress,
            total: 100,
            type: ProgressType.Linear
          })
            .width('80%')
            .height(6)
            .linearGradient({
              angle: 90,
              colors: [[0x007AFF, 0], [0x4CD964, 1]]
            })
            .backgroundColor('#FFFFFF40')
            .borderRadius(3)

          if (this.currentStatus === FaceRecStatus.SCANNING) {
            Text('正在采集面部特征...')
              .fontSize(14)
              .fontColor('#CCCCCC')
          } else if (this.currentStatus === FaceRecStatus.VERIFYING) {
            Text('正在与注册信息比对...')
              .fontSize(14)
              .fontColor('#CCCCCC')
          }
        }
        .margin({ top: 10 })
      }

      // 尝试次数和锁定信息
      Column({ space: 10 }) {
        if (this.attempts > 0 && !this.isLocked) {
          Text(`尝试次数: ${this.attempts}/${this.maxAttempts}`)
            .fontSize(14)
            .fontColor(this.attempts >= 2 ? '#FF9500' : '#CCCCCC')
        }

        if (this.isLocked) {
          Column({ space: 15 }) {
            Text(`锁定剩余时间: ${this.countdown}秒`)
              .fontSize(16)
              .fontColor('#FF3B30')
              .fontWeight(FontWeight.Medium)

            Progress({
              value: this.countdown,
              total: this.lockTime,
              type: ProgressType.Ring
            })
              .width(60)
              .height(60)
              .color('#FF3B30')

            Text('连续多次识别失败，请稍后再试')
              .fontSize(12)
              .fontColor('#FF9500')
              .margin({ top: 10 })
          }
          .margin({ top: 20 })
        }
      }
      .margin({ top: 20 })

      // 操作按钮
      Column({ space: 15 }) {
        // 主操作按钮
        Button(this.getButtonText())
          .width('80%')
          .height(50)
          .backgroundColor(this.isButtonEnabled() ? '#007AFF' : '#666666')
          .fontSize(18)
          .fontColor(Color.White)
          .borderRadius(25)
          .enabled(this.isButtonEnabled())
          .onClick(() => this.startFaceRecognition())

        // 切换认证方式按钮
        if (this.currentStatus !== FaceRecStatus.SCANNING &&
          this.currentStatus !== FaceRecStatus.VERIFYING &&
          this.currentStatus !== FaceRecStatus.SUCCESS) {
          Button('使用其他方式认证')
            .width('80%')
            .height(40)
            .backgroundColor('#1A1A1A')  // 深灰色背景
            .fontSize(14)
            .fontColor(Color.White)     // 白色文字
            .borderRadius(20)
            .border({ width: 1, color: '#FFFFFF40' }) // 可选：加个浅色边框
            .onClick(() => this.switchToOtherAuth())
        }
      }
      .margin({ top: 30, bottom: 40 })
    }
    .width('100%')
    .padding(20)
    .alignItems(HorizontalAlign.Center)
  }
}