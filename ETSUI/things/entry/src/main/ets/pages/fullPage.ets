import router from "@ohos.router"

@Entry
@Component
export struct FullScreenImagePage {
  // 接收传入的图片链接
  private imageUrl: string = ""
  // 暗黑模式开关（与主页面保持一致）
  private isDarkMode: boolean = false
  // 图片加载状态
  private isImageLoaded: boolean = false
  // 图片加载失败标识
  private isImageLoadFailed: boolean = false


  aboutToAppear() {
    this.getRouteParams()
  }

  private getRouteParams(): void {
    try {
      const params = router.getParams()
      // 步骤1：获取params自身的所有属性名（含不可枚举）；步骤2：判断是否包含imageUrl
      if (params && Object.getOwnPropertyNames(params).includes("imageUrl")) {
        this.imageUrl = (Object(params)["imageUrl"] as string) || ""
      }
    } catch (e) {
      console.error("获取图片参数失败：", e)
      this.isImageLoadFailed = true
    }
  }

  /**
   * 图片加载成功回调
   */
  private onImageLoadSuccess(): void {
    this.isImageLoaded = true
    this.isImageLoadFailed = false
  }

  /**
   * 图片加载失败回调
   */
  private onImageLoadFailed(): void {
    this.isImageLoaded = false
    this.isImageLoadFailed = true
  }

  /**
   * 点击页面关闭全屏预览，返回上一级
   */
  private closeFullScreenPage(): void {
    router.back()
  }

  /**
   * 获取页面背景色（兼容暗黑模式）
   */
  private getPageBackgroundColor(): string {
    return this.isDarkMode ? "#000000" : "#000000" // 全屏预览默认黑色背景，无论明暗模式
  }

  /**
   * 构建图片加载失败兜底视图
   */
  private buildImageLoadFailedView(): void {
    // UI组件需要在build方法中构建，这里只设置状态
  }

  build() {
    // 全屏容器，点击任意位置关闭页面
    Stack({ alignContent: Alignment.Center }) {
      // 页面背景
      Column() {
      }
      .width('100%')
      .height('100%')
      .backgroundColor(this.getPageBackgroundColor())

      // 图片展示区域（全屏自适应）
      if (!this.isImageLoadFailed) {
        Image($r(this.imageUrl))
          .width('100%')
          .height('100%')
          .objectFit(ImageFit.Contain) // 保持图片比例，完整显示在容器内
          .onComplete(() => {
            this.onImageLoadSuccess()
            console.info("finish")
          })
          .onError(() => {
            this.onImageLoadFailed()
          })
      } else {
        // 图片加载失败兜底视图
        Column({ space: 15 }) {
          Image($r("app.media.FailPic"))
            .width(80)
            .height(80)

          Text("图片加载失败，请稍后重试")
            .fontSize(14)
            .fontColor("#aaaaaa")
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
      }

      // 关闭提示（可选，增强用户体验）
      Text("点击任意位置关闭全屏预览")
        .fontSize(14)
        .fontColor("#ffffff")
        .opacity(0.7)
        .position({ y: '95%',x:"30%"})

    }
    .width('100%')
    .height('100%')
    .onClick(() => {
      this.closeFullScreenPage()
    })
  }
}