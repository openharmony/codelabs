import { getResourcesList, Resources } from '../model/resourceData'
import screenshot from '@ohos.screenshot'
import display from '@ohos.display';
import image from '@ohos.multimedia.image'

const clippingEnum = {
  LEFT: 0,
  RIGHT: 1,
  UP: 2,
  DOWN: 3,
  leftUp: 4,
  rightUp: 5,
  leftDown: 6,
  rightDown: 7,
}

@CustomDialog
struct ScreenshotDialog {
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void
  private pixelMap: PixelMap = null

  build() {
    Column() {
      Text('截屏成功').width('70%').fontSize(20).margin({ top: 10, bottom: 10 })
      Image(this.pixelMap)
        .width('95%')
        .height('50%')
        .margin({ top: 10 })
        .objectFit(ImageFit.Contain)
      Flex({ justifyContent: FlexAlign.End }) {
        Button('confirm')
          .onClick(() => {
            this.controller.close()
            this.confirm()
          }).backgroundColor(0xffffff).fontColor(Color.Red)
      }.margin({ bottom: 10 })
    }
  }
}

@Entry
@Component
struct Index {
  @State pixelMap: image.PixelMap = undefined
  dialogController: CustomDialogController = new CustomDialogController({
    builder: ScreenshotDialog({ cancel: this.onCancel, confirm: this.onAccept, pixelMap: this.pixelMap }),
    cancel: this.existApp,
    autoCancel: true
  })
  private swiperController: SwiperController = new SwiperController()
  @State swiperArray: Resources[] = getResourcesList()
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  private offContext: OffscreenCanvasRenderingContext2D = new OffscreenCanvasRenderingContext2D(600, 600, this.settings)
  private offsetX1: number = 50
  private offsetX2: number = 300
  private offsetY1: number = 200
  private offsetY2: number = 450
  private screenWidth: number = 0
  private screenHeight: number = 0
  private isClipping: boolean = false
  private clippingType: number
  @State showScreen: boolean = false

  onCancel() {
    console.info('Callback when the first button is clicked')
  }

  onAccept() {
    console.info('Callback when the second button is clicked')
  }

  existApp() {
    console.info('Click the callback in the blank area')
  }

  private draw(x1, x2, y1, y2) {
    this.context.clearRect(0, 0, this.screenWidth, this.screenHeight)
    this.context.beginPath()

    this.context.moveTo(0, 0)
    this.context.lineTo(this.screenWidth, 0)
    this.context.lineTo(this.screenWidth, this.screenHeight)
    this.context.lineTo(0, this.screenHeight)
    this.context.globalAlpha = 0
    this.context.closePath()
    this.context.fill()


    this.context.moveTo(x2, y1)
    this.context.lineTo(x1, y1)
    this.context.lineTo(x1, y2)
    this.context.lineTo(x2, y2)

    this.context.globalAlpha = 0.9
    this.context.fillStyle = '#ff000000'
    this.context.closePath()
    this.context.fill()
  }

  private fetchType(x, y) {
    let left, right, up, down
    if (y > this.offsetY1 - 15 && y < this.offsetY2 + 15 && x > this.offsetX1 - 15 && x < this.offsetX1 + 15) {
      left = true
    }
    if (y > this.offsetY1 - 15 && y < this.offsetY2 + 15 && x > this.offsetX2 - 15 && x < this.offsetX2 + 15) {
      right = true
    }
    if (x > this.offsetX1 - 15 && x < this.offsetX2 + 15 && y > this.offsetY1 - 15 && y < this.offsetY1 + 15) {
      up = true
    }
    if (x > this.offsetX1 - 15 && x < this.offsetX2 + 15 && y > this.offsetY2 - 15 && y < this.offsetY2 + 15) {
      down = true
    }

    if (left && up) return clippingEnum.leftUp
    if (left && down) return clippingEnum.leftDown
    if (right && up) return clippingEnum.rightUp
    if (right && down) return clippingEnum.rightDown
    if (left) return clippingEnum.LEFT
    if (right) return clippingEnum.RIGHT
    if (up) return clippingEnum.UP
    if (down) return clippingEnum.DOWN

    return -1
  }

  aboutToAppear() {
    display.getDefaultDisplay((err, data) => {
      if (err.code === 0) {
        console.info('Failed to obtain the default display object. Code:  ' + JSON.stringify(err))
      }
      console.info('Failed to obtain the default display object. Code:  ' + JSON.stringify(data))
      this.screenWidth = data.width
      this.screenHeight = data.height
    });
  }

  build() {
    Stack() {
      Column() {
        // 图片轮播图
        Column() {
          Swiper(this.swiperController) {
            ForEach(this.swiperArray.map((item, index) => {
              return { i: index, data: item };
            }), (item) => {
              Column() {
                Image(item.data.resourceSrc)
                  .objectFit(ImageFit.Cover)
                  .width('100%')
                  .height('100%')
              }
              .width('100%')
              .height('100%')
              .alignItems(HorizontalAlign.Center)
            })
          }
          .index(0)
          .autoPlay(true)
          .interval(3000)
          .indicator(false)
          .loop(true)
          .indicatorStyle({ color: '#cccccc', selectedColor: '#ffffff' })
          .height('100%')
        }
        .width('100%')
        .height('50%')

        // 文字信息
        Column() {
          Column({ space: 20 }) {
            Text('微澜止水    ')
              .fontSize(20)
              .padding({ bottom: 10 })
            Text('平地微澜终吹皱，')
              .fontSize(20)
            Text('顾盼逡巡为哪般？')
              .fontSize(20)
            Text('杨柳青烟红暮里，')
              .fontSize(20)
            Text('斌驳墙下故人还。')
              .fontSize(20)
          }
          .border({ width: 2, radius: 10, style: BorderStyle.Solid, color: Color.White })
          .align(Alignment.Top)
          .padding({ top: 28, bottom: 8 })
          .margin({ left: 8, right: 8, top: 50 })
          .width('100%')
        }
        .padding({ left: 8, right: 8 })
        .height('50%')
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#ffd6d6d6')
      .gesture(
        // 触发手指数：fingers 触发方向：direction 触发滑动距离：distance
      PanGesture({ fingers: 1, direction: PanDirection.Down, distance: 50 })
        // 触发开始回调
        .onActionStart((event: GestureEvent) => {
          console.info('Pan update' + JSON.stringify(event))
          // 截屏参数及API
          var ScreenshotOptions = {
            rotation: 0
          };
          screenshot.save(ScreenshotOptions, (err, data: image.PixelMap) => {
            if (err) {
              console.error(`Failed to save the screenshot. Error:${JSON.stringify(err)}`)
            }
            if (this.pixelMap !== undefined) {
              this.pixelMap.release()
            }
            this.pixelMap = data
            this.dialogController.open()
            console.log(`Success to save the screenshot. data:${JSON.stringify(this.pixelMap)}`)
          })
        })
          // 触发更新回调
        .onActionUpdate((event: GestureEvent) => {
          console.info('Pan update' + JSON.stringify(event))
        })
          // 触发结束回调
        .onActionEnd(() => {
          console.info('Pan end')
        })
      )

      if (this.showScreen) {
        Stack() {
          Flex({ justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
            Button({ type: ButtonType.Capsule, stateEffect: true }) {
              Text('取消')
                .fontSize(24)
                .fontWeight(FontWeight.Bolder)
                .fontColor('#FFFFFF')
            }
            .onClick(() => {
              this.showScreen = false
              this.offsetX1 = 50
              this.offsetX2 = 300
              this.offsetY1 = 200
              this.offsetY2 = 450
            })
            .backgroundColor('#00ffffff')
            .width('25%')
            .height(50)

            Button({ type: ButtonType.Capsule, stateEffect: true }) {
              Text('确定')
                .fontSize(24)
                .fontWeight(FontWeight.Bolder)
                .fontColor('#FFFFFF')
            }
            .onClick(() => {
              this.showScreen = false
              var ScreenshotOptions = {
                screenRect: {
                  left: this.offsetX1,
                  top: this.offsetY1,
                  width: this.offsetX2 - this.offsetX1,
                  height: this.offsetY2 - this.offsetY1 },
                imageSize: { width: this.offsetX2 - this.offsetX1, height: this.offsetY2 - this.offsetY1 },
                rotation: 0,
                displayId: 0
              };
              screenshot.save(ScreenshotOptions, (err, data: image.PixelMap) => {
                if (err) {
                  console.error(`Failed to save the screenshot. Error:${JSON.stringify(err)}`)
                }
                if (this.pixelMap !== undefined) {
                  this.pixelMap.release()
                }
                this.pixelMap = data
                this.dialogController.open()
                console.log(`Success to save the screenshot. data:${JSON.stringify(this.pixelMap)}`)
              })
              this.offsetX1 = 50
              this.offsetX2 = 300
              this.offsetY1 = 200
              this.offsetY2 = 450
            })
            .backgroundColor('#00ffffff')
            .width('25%')
            .height(50)
          }
          .zIndex(1)
          .position({ x: 0, y: 0 })
          .margin({top: 20})

          Canvas(this.context)
            .height('100%')
            .onReady(() => {
              this.draw(this.offsetX1, this.offsetX2, this.offsetY1, this.offsetY2)
            })

        }
        .width('100%')
        .height('100%')
        .onTouch((event: TouchEvent) => {
          if (event.type === TouchType.Down) {
            this.clippingType = this.fetchType(event.touches[0].screenX, event.touches[0].screenY)
          }
          if (event.type === TouchType.Move) {
            if (this.clippingType !== -1) {
              if (this.clippingType == clippingEnum.LEFT) {
                this.draw(event.touches[0].screenX, this.offsetX2, this.offsetY1, this.offsetY2)
                this.offsetX1 = event.touches[0].screenX
              }
              if (this.clippingType === clippingEnum.RIGHT) {
                this.draw(this.offsetX1, event.touches[0].screenX, this.offsetY1, this.offsetY2)
                this.offsetX2 = event.touches[0].screenX
              }
              if (this.clippingType === clippingEnum.UP) {
                this.draw(this.offsetX1, this.offsetX2, event.touches[0].screenY, this.offsetY2)
                this.offsetY1 = event.touches[0].screenY
              }
              if (this.clippingType === clippingEnum.DOWN) {
                this.draw(this.offsetX1, this.offsetX2, this.offsetY1, event.touches[0].screenY)
                this.offsetY2 = event.touches[0].screenY
              }
              if (this.clippingType === clippingEnum.leftUp) {
                this.draw(event.touches[0].screenX, this.offsetX2, event.touches[0].screenY, this.offsetY2)
                this.offsetX1 = event.touches[0].screenX
                this.offsetY1 = event.touches[0].screenY
              }
              if (this.clippingType === clippingEnum.leftDown) {
                this.draw(event.touches[0].screenX, this.offsetX2, this.offsetY1, event.touches[0].screenY)
                this.offsetX1 = event.touches[0].screenX
                this.offsetY2 = event.touches[0].screenY
              }
              if (this.clippingType === clippingEnum.rightUp) {
                this.draw(this.offsetX1, event.touches[0].screenX, event.touches[0].screenY, this.offsetY2)
                this.offsetX2 = event.touches[0].screenX
                this.offsetY1 = event.touches[0].screenY
              }
              if (this.clippingType === clippingEnum.rightDown) {
                this.draw(this.offsetX1, event.touches[0].screenX, this.offsetY1, event.touches[0].screenY)
                this.offsetX2 = event.touches[0].screenX
                this.offsetY2 = event.touches[0].screenY
              }
            }
          }
          if (event.type === TouchType.Up) {
          }
        })
      }

    }.gesture(
    TapGesture({ count: 2 })
      .onAction(() => {
        console.info('TapGesture')
        this.showScreen = true
      })
    )
  }
}