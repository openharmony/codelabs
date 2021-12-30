# SliderApplicationEtsOpenH
# 介绍<a name="ZH-CN_TOPIC_0000001237044819"></a>

-   [应用场景](#section225718574575)

## 应用场景<a name="section225718574575"></a>

OpenHarmony eTS提供了丰富的动画组件和接口，开发者可以根据实际场景和开发需求，选用不同的组件和接口来实现不同的动画效果。

在本教程中，我们将会通过一个简单的样例，学习如何使用eTS开发框架的转场动画。其中包含页面间转场、组件内转场以及共享元素转场，完成效果如下图所示：

![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/figures/VID_20211221_194349-00_00_00-00_00_30-2.gif)

# 相关概念<a name="ZH-CN_TOPIC_0000001189884870"></a>

-   [eTS工程目录](#section116501561575)

**页面间转场：**在两个页面之间切换时执行过渡动效。

**组件内转场：**在组件添加和移除时执行过渡动效，主要用于容器组件子组件添加和移除时提升用户体验。

**共享元素转场：**元素在不同页面之间过渡动效。例如，如果两个页面使用相同的图片（但位置和大小不同），图片就会在这两个页面之间流畅地平移和缩放。

## eTS工程目录<a name="section116501561575"></a>

新建工程的eTS目录如下图所示。

![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/figures/zh-cn_image_0000001236728127.png)

**各个文件夹和文件的作用：**

-   **index.ets：**用于描述UI布局、样式、事件交互和页面逻辑。

-   **app.ets**：用于全局应用逻辑和应用生命周期管理。
-   **pages**：用于存放所有组件页面。
-   **resources：**用于存放资源配置文件。

# 任务一：构建主界面<a name="ZH-CN_TOPIC_0000001189644904"></a>

在这个任务中，我们将完成示例主界面的设计，效果图如下：

![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/figures/zh-cn_other_0000001191964634.jpeg)

从上面效果图可以看出，主界面主要由5个相同样式的功能菜单组成，我们可以将这些菜单抽取成一个子组件Item。

1. 将所需要的图片添加到resources -\> base -\> media 目录下。

   ![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/figures/zh-cn_image_0000001237187351.png)

2. 在index.ets中新建名为Item的子组件，声明子组件Item的UI布局并添加样式。创建Stack组件，包含图片和文本，并在已创建的Item组件中添加文本信息和页面跳转事件，定义变量text和uri。其中text用于给Text组件设置文本信息，uri用于设置页面路由的地址。示例代码如下：

   ```
   @Component
   struct Item {
   // 文本信息
     private text: string
   
   // 页面跳转uri
     private uri: string
   
     build() {
       Stack({ alignContent: Alignment.Center }) {
         Image($r('app.media.image3'))
           .objectFit(ImageFit.Cover)
           .width('100%')
           .height(100)
           .borderRadius(15)
   
         Text(this.text)
           .fontSize(20)
           .fontWeight(FontWeight.Bold)
           .fontColor(Color.Black)
       }
       .onClick(() => {
         router.push({ uri: this.uri })
       })
       .height(120)
       .borderRadius(15)
       .width('80%')
       .margin({ bottom: 20 })
     }
   }
   ```

3. 将Item组件添加到Index组件中，并给Item传入参数text和uri，由于还未创建要跳转的页面，所以这里uri暂时传空字符串。

   ```
   @Entry
   @Component
   struct Index {
     build() {
       Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
         Item({ text: '页面间转场：底部滑入', uri: '' })
         Item({ text: '页面间转场：自定义1', uri: '' })
         Item({ text: '页面间转场：自定义2', uri: '' })
         Item({ text: '组件内转场', uri: '' })
         Item({ text: '共享元素转场', uri: '' })
       }
       .width('100%')
       .height('100%')
       .backgroundColor('#FFECECEC')
     }
   }
   
   @Component
   struct Item {
    ......
   }
   ```

# 任务二：实现页面间转场<a name="ZH-CN_TOPIC_0000001237044821"></a>

-   新建页面

1. 首先在pages目录下新建名为page的package，然后在page目录下，点击鼠标右键分别新建名为BottomTransition、CustomTransition和FullCustomTransition的三个ets文件。其中BottomTransition用于实现“页面间转场：底部滑入”动效；CustomTransition用于实现“页面间转场：自定义1”动效；FullCustomTransition用于实现“页面间转场：自定义2”动效

   ![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/figures/zh-cn_image_0000001192120584.png)

   >![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/public_sys-resources/icon-note.gif) **说明：** 
   >
   >-   页面文件名不能使用组件名称，比如：Text.ets、Button.ets等。
   >-   每个页面文件中必须包含入口组件。

2. 在config.json文件的pages标签下分别添加BottomTransition、CustomTransition和FullCustomTransition的路由地址。

   ```
   "pages": [
     "pages/Index",
     "pages/page/BottomTransition",
     "pages/page/CustomTransition",
     "pages/page/FullCustomTransition"
   ]
   ```

   >![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/public_sys-resources/icon-note.gif) **说明：** 
   >pages列表中第一个页面为应用的首页入口。

3. 在主界面index.ets的Index组件中，将BottomTransition、CustomTransition和FullCustomTransition的路由地址赋值给对应Item的uri。

   ```
   @Entry
   @Component
   struct Index {
     build() {
       Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
         Item({ text: '页面间转场：底部滑入', uri: 'pages/page/BottomTransition' })
         Item({ text: '页面间转场：自定义1', uri: 'pages/page/CustomTransition' })
         Item({ text: '页面间转场：自定义2', uri: 'pages/page/FullCustomTransition' })
         Item({ text: '组件内转场', uri: '' })
         Item({ text: '共享元素转场', uri: '' })
       }
       .width('100%')
       .height('100%')
       .backgroundColor('#FFECECEC')
     }
   }
   ```


-   实现“底部滑入”动效

在BottomTransition声明pageTransition方法配置转场参数，其中PageTransitionEnter用于自定义当前页面的入场效果，PageTransitionExit用于自定义当前页面的退场效果。

在这里我们设置PageTransitionEnter和PageTransitionExit的slide属性为SlideEffect.Bottom，来实现BottomTransition入场时从底部滑入，退场时从底部滑出。

```
@Entry
@Component
struct BottomTransition{
  build() {
    Stack() {
      Image($r('app.media.image1'))
        .objectFit(ImageFit.Cover)
        .width('100%')
        .height('100%')
    }
    .width('100%')
    .height('100%')
  }

// 页面转场通过全局transition方法配置转场参数
  pageTransition() {

    // 页面入场组件：SlideEffect.Bottom 设置到入场时表示从下边滑入，出场时表示滑出到下边。
    PageTransitionEnter({ duration: 600, curve: Curve.Smooth })
      .slide(SlideEffect.Bottom)

    // 页面退场组件：SlideEffect.Bottom 设置到入场时表示从下边滑入，出场时表示滑出到下边。
    PageTransitionExit({ duration: 600, curve: Curve.Smooth })
      .slide(SlideEffect.Bottom)
  }
}
```

# 任务三：实现组件内转场<a name="ZH-CN_TOPIC_0000001189644902"></a>

本节实现组件内转场动效，通过一个按钮来控制组件的添加和移除，呈现容器组件子组件添加和移除时的动效。

组件转场主要通过transition属性方法配置转场参数，在组件添加和移除时会执行过渡动效，需要配合animateTo才能生效。动效时长、曲线、延时跟随animateTo中的配置。

1. 在pages目录下，新建名为ComponentTransition的ets文件，然后在config.json文件下的pages标签下添加ComponentTransition的路由地址。

   ```
   "pages": [
     "pages/Index",
     "pages/page/BottomTransition",
     "pages/page/CustomTransition",
     "pages/page/FullCustomTransition",
     "pages/ComponentTransition"
   ]
   ```

2. 在ComponentTransition.ets文件中，新建ComponentItem子组件，添加Stack组件和Image组件。给Stack添加两个transition属性，分别用于定义组件的添加动效和移除动效。

   ```
   @Component
   struct ComponentItem {
     build() {
       Stack({ alignContent: Alignment.Center }) {
         Image($r('app.media.image3'))
           .objectFit(ImageFit.Cover)
           .width('100%')
           .height(120)
           .borderRadius(15)
       }
       .height(120)
       .borderRadius(15)
       .width('80%')
       .padding({ top: 20 })
       // 组件添加时x、y轴缩放从0.5变化到1，透明度从0到1
       .transition({ type: TransitionType.Insert, scale: { x: 0.5, y: 0.5 }, opacity: 0 })
       // 组件移除时沿y轴旋转360度
       .transition({ type: TransitionType.Delete, rotate: { x: 0, y: 1, z: 0, angle: 360 }, scale: { x: 0, y: 0 } })
     }
   }
   
   @Entry
   @Component
   struct ComponentTransition {
     ...
   }
   ```

3. 在ComponentTransition组件定义一个变量，用于控制ComponentItem的添加和移除，在Button组件的onClick事件中添加animateTo方法，来使ComponentItem子组件动效生效。

   ```
   @Component
   struct ComponentItem {
     ...
   }
   
   
   @Entry
   @Component
   struct ComponentTransition {
     @State private isShow: boolean= false
   
     build() {
       Column() {
         if (this.isShow) {
           ComponentItem()
         }
   
         ComponentItem()
   
         Button("Toggle")
           .onClick(() => {
             //执行动效，动效时长600ms
             animateTo({ duration: 600 }, () => {
               this.isShow = !this.isShow;
             })
           })
           .height(45)
           .width(200)
           .fontColor(Color.Black)
           .backgroundColor('rgb(181,222,224)')
           .margin({ top: 20 })
       }
       .padding({ left: 20, right: 20 })
       .backgroundColor('#FFECECEC')
       .height('100%')
       .width('100%')
     }
   }
   ```

# 任务四：实现元素共享转场<a name="ZH-CN_TOPIC_0000001236761375"></a>

共享元素转场通过给组件设置sharedTransition属性来实现，两个页面的组件配置为同一个id，则转场过程中会执行共享元素转场。sharedTransition可以设置动效的时长、动画曲线和延时，实现步骤如下：

1. 首先在pages目录下新建名为share的包，然后在share目录下分别新建名为ShareItem和SharePage的ets文件。其中ShareItem.ets用于展示小图，SharePage.ets用于展示大图。

   ![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/figures/zh-cn_image_0000001192441454.png)

2. 在config.json文件下的pages标签下分别添加ShareItem和SharePage的路由地址。

   ```
   "pages": [  "pages/Index",  "pages/page/BottomTransition",  "pages/page/CustomTransition",  "pages/page/FullCustomTransition",  "pages/ComponentTransition",  "pages/share/ShareItem",  "pages/share/SharePage"]
   ```

3. 在ShareItem.ets中给Image组件设置sharedTransition属性，组件转场id设置为“imageId”。

   ```
   import router from '@system.router'
   
   @Entry
   @Component
   struct ShareItem{
     build() {
       Flex() {
         Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
           Stack() {
             Image($r('app.media.image2'))
               // 设置共享元素转场属性
               .sharedTransition('imageId', { duration: 600, curve: Curve.Smooth, delay: 100 })
               .onClick(() => {
               router.push({ uri: 'pages/share/SharePage' })
             })
               .objectFit(ImageFit.Cover)
               .height('100%')
               .width('100%')
               .borderRadius(15)
           }
           .height('100%')
           .width('100%')
   
           Text('点击查看共享元素转场动效')
             .fontSize(20)
             .fontColor(Color.Black)
             .fontWeight(FontWeight.Regular)
             .margin({ left: 10, right: 10 })
   
         }
         .height(120)
         .backgroundColor('rgb(181,222,224)')
         .borderRadius(15)
         .margin({ top: 20 })
       }
       .width('100%')
       .padding({ left: 16, right: 16 })
       .backgroundColor('#FFECECEC')
     }
   }
   ```

   >![](D:/openHarmonyAdapt/OpenHarmonyMD-File/eTS转场动画的使用_1640595367382/eTS转场动画的使用/public_sys-resources/icon-note.gif) **说明：** 
   >两个页面的组件配置为同一个id，则转场过程中会执行共享元素转场，配置为空字符串时不会有共享元素转场效果。

4. 在SharePage.ets中给Image组件设置sharedTransition属性，组件转场id设置为“imageId”。

   ```
   @Entry
   @Component
   struct SharePage{
     build() {
       Stack() {
         Image($r('app.media.image2'))
           // 设置共享元素转场属性
           .sharedTransition('imageId', { duration: 1000, curve: Curve.Smooth, delay: 100 })
           .objectFit(ImageFit.Cover)
           .width('100%')
           .height('100%')
       }
       .width('100%')
       .height('100%')
     }
   }
   ```

# 恭喜你<a name="ZH-CN_TOPIC_0000001236724851"></a>

在本篇Codelab中，我们主要用到了以下知识点：

-   转场动画
-   路由

本示例代码从布局、样式、响应事件三个层面，逐步实现了页面间转场、组件内转场以及共享元素转场。希望通过本教程，各位开发者可以对转场动画具有更深刻的认识。

# 参考<a name="ZH-CN_TOPIC_0000001236884841"></a>

[gitee地址](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/TransitionAnimtaionEts)

