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
// pages/GraphPassPage.ets
import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import { PassType, PasswordManager, RouterUtil } from '../common/Config';
import { PromptAction, UIContext } from '@kit.ArkUI';
import { AlertDialog } from '@kit.ArkUI';
import common from '@ohos.app.ability.common';
// 九宫格点定义
interface PatternPoint {
  id: number;
  x: number;
  y: number;
  isActive: boolean;
  isSelected: boolean;
  isConnected: boolean;
}

// 触摸点定义
interface TouchPosition {
  x: number;
  y: number;
}

@Entry
@Component
export struct GraphPassPage {
  // 从路由参数获取认证类型
  private authType: PassType = PassType.GRAPH;

  @State points: PatternPoint[] = []; // 九宫格点数组
  @State selectedPoints: number[] = []; // 已选中的点ID序列
  @State currentPath: number[] = []; // 当前绘制路径（用于连线）
  @State isDrawing: boolean = false; // 是否正在绘制
  @State isVerifying: boolean = false; // 是否正在验证
  @State attempts: number = 0; // 尝试次数
  @State maxAttempts: number = 5; // 最大尝试次数
  @State gridSize: number = 300; // 九宫格大小
  uiContext: UIContext = this.getUIContext();
  promptAction: PromptAction = this.uiContext.getPromptAction();
  dialogControllerConfirm: CustomDialogController = new CustomDialogController({
    builder: AlertDialog({
      primaryTitle: '忘记图案？',
      content: '1. 请联系管理员重置图案\n2. 或使用其他认证方式登录',
      primaryButton: {
        value: '使用其他方式',
        fontColor:'#007DFF',
        action: () => {router.pushUrl({
          url: 'pages/PasswordEntry'
        });},
      },
      secondaryButton: {
        value:'取消',
        fontColor:'#999',
        action: () => {router.pushUrl({
          url: 'pages/Index'
        });}
      },
    }),
  })
  dialogFailure: CustomDialogController = new CustomDialogController({
    builder: AlertDialog({
      primaryTitle: '验证失败',
      content: `您已连续失败${this.maxAttempts}次，请稍后再试`,
      primaryButton: {
        value: '返回',
        fontColor:'#007DFF',
        action: () => {router.pushUrl({
          url: 'pages/Index'
        });},
      },
    }),
  })

  // 初始化九宫格点
  private initPoints() {
    const newPoints: PatternPoint[] = [];
    const spacing = this.gridSize / 3;

    for (let row = 0; row < 3; row++) {
      for (let col = 0; col < 3; col++) {
        const id = row * 3 + col + 1; // 1-9
        newPoints.push({
          id,
          x: col * spacing + spacing / 2,
          y: row * spacing + spacing / 2,
          isActive: false,
          isSelected: false,
          isConnected: false
        });
      }
    }
    this.points = newPoints;
  }

  aboutToAppear() {
    // 从路由参数获取认证类型
    const params = router.getParams() as Record<string, string>;
    if (params && params.authType) {
      this.authType = params.authType as PassType;
    }

    this.initPoints();
  }

  // 处理触摸开始
  handleTouchStart(event: TouchEvent) {
    if (this.isVerifying) return;

    const touch = event.touches[0];
    // 使用触摸位置相对于容器的坐标
    const touchPosition: TouchPosition = {
      x: touch.displayX || touch.x || 0,
      y: touch.displayY || touch.y || 0
    };
    this.checkTouchPoint(touchPosition);
    this.isDrawing = true;
  }

  // 处理触摸移动
  handleTouchMove(event: TouchEvent) {
    if (!this.isDrawing || this.isVerifying) return;

    const touch = event.touches[0];
    // 使用触摸位置相对于容器的坐标
    const touchPosition: TouchPosition = {
      x: touch.displayX || touch.x || 0,
      y: touch.displayY || touch.y || 0
    };
    this.checkTouchPoint(touchPosition);
  }

  // 处理触摸结束
  handleTouchEnd() {
    if (!this.isDrawing) return;

    this.isDrawing = false;

    // 至少需要连接4个点
    if (this.selectedPoints.length >= 4) {
      this.verifyPattern();
    } else {
      if (this.selectedPoints.length > 0) {
        promptAction.showToast({
          message: '请连接至少4个点',
          duration: 1500
        });
      }
      this.resetPattern();
    }
  }

  // 检查触摸点是否在九宫格点上
  private checkTouchPoint(touch: TouchPosition) {
    const touchX = touch.x;
    const touchY = touch.y;
    const radius = 30; // 触摸半径

    for (const point of this.points) {
      const distance = Math.sqrt(
        Math.pow(touchX - point.x, 2) + Math.pow(touchY - point.y, 2)
      );

      if (distance <= radius && !point.isSelected) {
        // 选中该点
        point.isSelected = true;
        point.isActive = true;

        // 如果已经有选中的点，标记中间点
        if (this.selectedPoints.length > 0) {
          const lastPointId = this.selectedPoints[this.selectedPoints.length - 1];
          this.markMiddlePoints(lastPointId, point.id);
        }

        this.selectedPoints.push(point.id);
        break;
      }
    }

    this.points = [...this.points];
  }

  // 标记两个点之间的中间点（用于实现类似Android的九宫格逻辑）
  private markMiddlePoints(fromId: number, toId: number) {
    // 将点ID转换为0-based索引
    const fromIndex = fromId - 1;
    const toIndex = toId - 1;

    const fromRow = Math.floor(fromIndex / 3);
    const fromCol = fromIndex % 3;
    const toRow = Math.floor(toIndex / 3);
    const toCol = toIndex % 3;

    // 检查是否在同一行或同一列或对角线
    if (fromRow === toRow && Math.abs(fromCol - toCol) === 2) {
      // 同一行，间隔一个点
      const middleId = fromRow * 3 + (fromCol + toCol) / 2 + 1;
      this.selectMiddlePoint(middleId);
    } else if (fromCol === toCol && Math.abs(fromRow - toRow) === 2) {
      // 同一列，间隔一个点
      const middleId = ((fromRow + toRow) / 2) * 3 + fromCol + 1;
      this.selectMiddlePoint(middleId);
    } else if (Math.abs(fromRow - toRow) === 2 && Math.abs(fromCol - toCol) === 2) {
      // 对角线，间隔一个点
      const middleId = 5; // 中心点永远是5
      this.selectMiddlePoint(middleId);
    }
  }

  // 选中中间点
  private selectMiddlePoint(pointId: number) {
    const point = this.points.find(p => p.id === pointId);
    if (point && !point.isSelected) {
      point.isSelected = true;
      point.isActive = true;
      point.isConnected = true; // 标记为自动连接的点
      this.selectedPoints.push(pointId);
    }
  }

  // 验证图案
  async verifyPattern() {
    if (this.selectedPoints.length < 4) {
      promptAction.showToast({
        message: '请连接至少4个点',
        duration: 2000
      });
      return;
    }

    this.isVerifying = true;

    // 将选中的点ID序列转换为字符串（如"123578"）
    const pattern = this.selectedPoints.join('');

    // 模拟网络延迟
    await new Promise<void>(resolve => setTimeout(resolve, 500));

    try {
      // 【修改点 1】：传入 getContext(this) 以支持 Preferences 磁盘读取
      // 接口签名匹配：verifyPassword(context, type, input)
      const isValid = await PasswordManager.verifyPassword(
        getContext(this),
        this.authType,
        pattern
      );

      if (isValid) {
        // 验证成功
        console.info('图案验证匹配成功');
        await this.handleSuccess();
      } else {
        // 验证失败
        console.warn('图案验证不匹配');
        await this.handleFailure();
      }
    } catch (error) {
      // 【修改点 2】：确保 promptAction 调用正确（通常直接使用导入的对象）
      promptAction.showToast({
        message: '验证出错，请重试',
        duration: 2000
      });
      this.resetPattern();
    } finally {
      this.isVerifying = false;
    }
  }

  // 验证成功处理
  // GraphPassPage.ets

  async handleSuccess(): Promise<void> {
    console.info('图形验证成功，准备跳转...');

    // 1. 立即触发 UI 反馈（不再等待持久化结果）
    try {
      // 显示成功提示
      promptAction.showToast({
        message: '图案验证成功！',
        duration: 1000
      });

      // 如果有动画，可以执行动画（注意：动画也不建议阻塞跳转太久）
      this.playSuccessAnimation();
    } catch (uiError) {
      console.error('UI Feedback Error: ' + JSON.stringify(uiError));
    }

    // 2. 【核心修改】异步保存，不使用 await
    // 这样即使持久化报 100001 错误，也不会中断下方的跳转逻辑
    PasswordManager.setLastUsedAuth(getContext(this) as common.Context, this.authType)
      .then(() => {
        console.info('持久化数据保存完成');
      })
      .catch((err: Error) => {
        // 这里的错误会被捕获，不会进入外层的 catch 块
        console.error('持久化保存失败（非致命）: ' + err.message);
      });

    // 3. 强制跳转逻辑
    // 使用 setTimeout 确保跳转指令在 UI 任务队列的下一帧执行，规避 Context 丢失问题
    setTimeout(() => {
      console.info('执行跳转主页');
      RouterUtil.goToHome();
    }, 400); // 400ms 足够让用户看到 Toast 提示
  }

  // 验证失败处理
  async handleFailure() {
    this.attempts++;

    if (this.attempts >= this.maxAttempts) {
      // 达到最大尝试次数
      this.dialogFailure.open();
      /*.showDialog({
        title: '验证失败',
        message: `您已连续失败${this.maxAttempts}次，请稍后再试`,
        buttons: [
          {
            text: '返回',
            color: '#007DFF'
          }
        ]
      }).then(result => {
        if (result.index === 0) {
          router.back();
        }
      });*/

      // 重置状态
      this.resetPattern();
      this.attempts = 0;
      return;
    }

    // 显示失败提示
    const remaining = this.maxAttempts - this.attempts;
    this.promptAction.openToast({
      message: `图案错误，还剩${remaining}次尝试`,
      duration: 2000
    });

    // 播放错误动画
    await this.playErrorAnimation();

    // 重置图案
    this.resetPattern();
  }

  // 播放成功动画
  private async playSuccessAnimation() {
    // 让所有点闪烁绿色
    for (let i = 0; i < 3; i++) {
      for (const point of this.points) {
        if (point.isSelected) {
          point.isActive = i % 2 === 0;
        }
      }
      this.points = [...this.points];
      await new Promise<void>(resolve => setTimeout(resolve, 200));
    }
  }

  // 播放错误动画
  private async playErrorAnimation() {
    // 让选中的点闪烁红色
    for (let i = 0; i < 3; i++) {
      for (const point of this.points) {
        if (point.isSelected) {
          point.isActive = i % 2 === 0;
        }
      }
      this.points = [...this.points];
      await new Promise<void>(resolve => setTimeout(resolve, 150));
    }
  }

  // 重置图案
  resetPattern() {
    for (const point of this.points) {
      point.isActive = false;
      point.isSelected = false;
      point.isConnected = false;
    }
    this.selectedPoints = [];
    this.currentPath = [];
    this.points = [...this.points];
  }

  // 返回上一页
  goBack() {
    router.back();
  }

  // 忘记图案
  handleForgotPattern() {
    this.dialogControllerConfirm.open();
    /*promptAction.showDialog({
      title: '忘记图案？',
      message: '1. 请联系管理员重置图案\n2. 或使用其他认证方式登录',
      buttons: [
        {
          text: '使用其他方式',
          color: '#007DFF'
        },
        {
          text: '重新绘制',
          color: '#FF9500'
        },
        {
          text: '取消',
          color: '#999'
        }
      ]
    }).then(result => {
      if (result.index === 0) {
        // 跳转到密码选择页
        router.pushUrl({
          url: 'pages/PasswordEntry'
        });
      } else if (result.index === 1) {
        this.resetPattern();
      }
    });*/
  }

  build() {
    Column({ space: 0 }) {
      // 顶部导航栏
      this.buildHeader()

      // 图案绘制区域
      this.buildPatternArea()

      // 底部操作区域
      this.buildBottomActions()
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

      Text('图案密码验证')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .layoutWeight(1)
        .textAlign(TextAlign.Center)

      // 重置按钮
      Button('重置')
        .width(60)
        .height(30)
        .fontSize(14)
        .backgroundColor('#E8F4FF')
        .fontColor('#007DFF')
        .borderRadius(15)
        .onClick(() => this.resetPattern())
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

  // 构建图案绘制区域
  @Builder
  buildPatternArea() {
    Column({ space: 20 }) {
      // 标题和提示
      Text('请绘制解锁图案')
        .fontSize(18)
        .fontWeight(FontWeight.Medium)
        .margin({ top: 20 })

      Text('连接至少4个点')
        .fontSize(14)
        .fontColor('#666')
        .margin({ bottom: 20 })

      // 图案绘制容器
      Stack() {
        // 九宫格容器
        Column() {
          // 九宫格点
          ForEach(this.points, (point: PatternPoint) => {
            this.buildPatternPoint(point)
          })

          // 连接线
          Canvas(this.context)
            .width(this.gridSize)
            .height(this.gridSize)
            .onReady(() => {
              this.drawLines();
            })
        }
        .width(this.gridSize)
        .height(this.gridSize)
        .backgroundColor('#FFFFFF')
        .border({ width: 2, color: '#E0E0E0' })
        .borderRadius(20)
        .shadow({
          radius: 10,
          color: '#00000010',
          offsetX: 0,
          offsetY: 5
        })

        // 触摸绘制层
        Column()
          .width(this.gridSize)
          .height(this.gridSize)
          .onTouch((event: TouchEvent) => {
            if (event.type === TouchType.Down) {
              this.handleTouchStart(event);
            } else if (event.type === TouchType.Move) {
              this.handleTouchMove(event);
            } else if (event.type === TouchType.Up) {
              this.handleTouchEnd();
            }
          })
      }
      .margin({ top: 10, bottom: 20 })

      // 状态提示
      if (this.isVerifying) {
        Text('验证中...')
          .fontSize(16)
          .fontColor('#FFA500')
          .margin({ top: 10 })
      }

      // 已选点序列显示（调试用，正式版可隐藏）
      if (this.selectedPoints.length > 0) {
        Text(`已选点: ${this.selectedPoints.join(' → ')}`)
          .fontSize(12)
          .fontColor('#999')
          .margin({ top: 10 })
      }
    }
    .width('100%')
    .padding(20)
    .alignItems(HorizontalAlign.Center)
  }

  // 构建单个图案点
  @Builder
  buildPatternPoint(point: PatternPoint) {
    Stack() {
      // 外圈
      Circle({ width: 40, height: 40 })
        .fill('#F0F0F0')
        .opacity(point.isSelected ? 0.5 : 1)

      // 内圈（激活状态显示）
      if (point.isActive) {
        Circle({ width: 30, height: 30 })
          .fill(point.isConnected ? '#4CD964' : '#007DFF')
          .shadow({
            radius: 3,
            color: '#00000030',
            offsetX: 0,
            offsetY: 2
          })
      }

      // 点编号（可选显示）
      Text(point.id.toString())
        .fontSize(12)
        .fontColor(point.isActive ? Color.White : '#999')
    }
    .position({ x: point.x - 20, y: point.y - 20 })
  }

  // Canvas上下文
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D();

  // 绘制连接线
  private drawLines() {
    const ctx = this.context;
    ctx.clearRect(0, 0, this.gridSize, this.gridSize);

    if (this.selectedPoints.length < 2) return;

    ctx.beginPath();
    ctx.lineWidth = 4;
    ctx.lineCap = 'round';
    ctx.strokeStyle = this.isVerifying ? '#FFA500' : '#007DFF';

    // 绘制连接线
    for (let i = 0; i < this.selectedPoints.length - 1; i++) {
      const fromId = this.selectedPoints[i];
      const toId = this.selectedPoints[i + 1];

      const fromPoint = this.points.find(p => p.id === fromId);
      const toPoint = this.points.find(p => p.id === toId);

      if (fromPoint && toPoint) {
        if (i === 0) {
          ctx.moveTo(fromPoint.x, fromPoint.y);
        }
        ctx.lineTo(toPoint.x, toPoint.y);
      }
    }

    ctx.stroke();
  }

  // 构建底部操作区域
  @Builder
  buildBottomActions() {
    Column({ space: 15 }) {
      // 验证按钮
      Button(this.isVerifying ? '验证中...' : '验证图案')
        .width('80%')
        .height(50)
        .backgroundColor(this.selectedPoints.length >= 4 ? '#007DFF' : '#CCCCCC')
        .fontSize(18)
        .fontColor(Color.White)
        .borderRadius(25)
        .enabled(this.selectedPoints.length >= 4 && !this.isVerifying)
        .onClick(() => this.verifyPattern())

      // 忘记图案链接
      Button('忘记图案？')
        .width('80%')
        .height(40)
        .backgroundColor(Color.Transparent)
        .fontSize(14)
        .fontColor('#666')
        .onClick(() => this.handleForgotPattern())

      // 尝试次数提示
      if (this.attempts > 0) {
        Text(`已尝试 ${this.attempts}/${this.maxAttempts} 次`)
          .fontSize(12)
          .fontColor(this.attempts >= 3 ? '#FF3B30' : '#FF9500')
          .margin({ top: 10 })
      }

      // 绘制提示
      Text('提示：从一个点拖动到另一个点')
        .fontSize(12)
        .fontColor('#999')
        .margin({ top: 20 })
    }
    .width('100%')
    .padding(20)
    .margin({ top: 20 })
    .alignItems(HorizontalAlign.Center)
  }
}