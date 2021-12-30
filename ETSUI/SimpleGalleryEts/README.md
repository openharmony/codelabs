# 极简声明式UI范式
# 介绍<a name="ZH-CN_TOPIC_0000001237205373"></a>

-   [应用场景](#section225718574575)

## 应用场景<a name="section225718574575"></a>

声明式UI开发框架，采用更接近自然语义的编程方式，让开发者可以直观地描述UI界面，不必关心框架如何实现UI绘制和渲染，从而实现极简高效的开发。从组件、动效和状态管理三个维度来提供UI能力，还提供了多端部署和系统能力接口，真正实现多端代码共享和系统能力的极简调用。

本篇Codelab使用极简声明式UI范式开发完成如下功能：

-   组合系统组件为自定义组件，完成顶部标签栏、推荐栏。
-   使用共享元素转场动画、显示动画完成大图浏览界面。
-   使用状态数据管理完成组件之间的数据传递。

为了让您快速了解本篇Codelab所实现的功能，我们先对极简声明式UI范式开发进行展示，效果如下：

![](D:/openHarmonyAdapt/OpenHarmonyMD-File/极简声明式UI范式_1640677876081/极简声明式UI范式/figures/VID_20211222_162936-00_00_00-00_00_30-1.gif)

# 相关概念<a name="ZH-CN_TOPIC_0000001192285406"></a>

# 搭建OpenHarmony环境<a name="ZH-CN_TOPIC_0000001237205393"></a>

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**Hi3516DV300**开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）

   以3.0版本为例：

   ![](D:\openHarmonyAdapt\OpenHarmonyMD-File\极简声明式UI范式_1640677876081\极简声明式UI范式\figures\取版本.png)

2. 搭建烧录环境

   1.  [完成DevEco Device Tool的安装](https://device.harmonyos.com/cn/docs/documentation/guide/install_windows-0000001050164976)

   2.  [完成Hi3516开发板的烧录](https://device.harmonyos.com/cn/docs/documentation/guide/hi3516_upload-0000001052148681)

3. 搭建开发环境

   1.  开始前请参考[下载与安装软件](https://developer.harmonyos.com/cn/docs/documentation/doc-guides/software_install-0000001053582415)、[配置开发环境](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/configuring-openharmony-sdk.md)，完成DevEco Studio的安装和开发环境配置。
   2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/use-wizard-to-create-project.md)创建工程，使用JS或者eTS语言开发、“Application”为例，模板选择“\[Standard\]Empty Ability”。
   3.  工程创建完成后，可参考下面章节进行代码编写，使用真机进行调测：

   -   [配置OpenHarmony应用签名信息](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/configuring-openharmony-app-signature.md)
   -   [hap包安装指导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/installing-openharmony-app.md)
   -   工程示例：

   ![](D:\openHarmonyAdapt\OpenHarmonyMD-File\极简声明式UI范式_1640677876081\极简声明式UI范式\figures\截图.png)

# 代码结构解读<a name="ZH-CN_TOPIC_0000001237577529"></a>

本篇Codelab只对核心代码进行讲解，以下介绍整个工程的代码结构。

![](D:\openHarmonyAdapt\OpenHarmonyMD-File\极简声明式UI范式_1640677876081\极简声明式UI范式\figures\036C0734-B95E-4D50-98E1-BEEC33160D0E.png)

-   common：自定义组件。
    -   bottomTabs.ets：底部页签容器。
    -   homeListItem.ets：首页推荐列表项。
    -   homeTabContent.ets：首页页签内容。
    -   topTabs.ets：顶部标签。

-   model：数据结构和数据初始化。
    -   homeListDataModel.ets：今日推荐、精选推荐、X月推荐数据。

-   pages：页面。
    -   home.ets：首页。
    -   image.ets：图片预览界面。

-   resources：项目资源存放路径，包括图片资源和国际化字符串等。
-   config.json：应用的配置文件。

# 页面布局与搭建<a name="ZH-CN_TOPIC_0000001192125426"></a>

-   Tabs组件

-   推荐List布局

-   大图浏览界面布局

-   页面跳转和数据传递

# Tabs组件<a name="ZH-CN_TOPIC_0000001237578193"></a>

在这一小节中，我们实现首页底部页签和顶部标签的布局。首先我们使用Flex组件，在Flex组件中使用Tabs组件。

1. 在common中创建文件bottomTabs.ets，自定义底部页签容器BottomTabs。使用ForEach语句，显示四个相同的页签，每个页签使用Column组件，使用justifyContent: FlexAlign.SpaceEvenly来使页签均分布局。

   ![](D:\openHarmonyAdapt\OpenHarmonyMD-File\极简声明式UI范式_1640677876081\极简声明式UI范式\figures\zh-cn_image_0000001237578411.png)

   ```
   import prompt from '@system.prompt'
   
   @Component
   export struct BottomTabs {
     private tabSrc: number[] = [0, 1, 2, 3]
     @Link bottomTabIndex: number
   
     build() {
       Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceEvenly }) { // FlexAlign.SpaceEvenly实现均分布局
         ForEach(this.tabSrc, item => { // 通过ForEach语句添加四个页签
           Column() {
             Image(getTabSrc(this.bottomTabIndex, item))
               .objectFit(ImageFit.Contain)
               ......
           }
         })
       }
       .width('100%')
       .height('8%')
     }
   }
   ```

2. Tabs组件中TabContent中的内容使用自定义组件完成，新建homeTabContent.ets文件，实现HomeTabComponent 组件。

   ![](D:\openHarmonyAdapt\OpenHarmonyMD-File\极简声明式UI范式_1640677876081\极简声明式UI范式\figures\zh-cn_image_0000001237778483.png)

   ```
   import { TopTabs } from '../common/topTabs'
   
   @Component
   export struct HomeTabComponent {
     private recommends: Resource[] = [$r('app.string.today_recommend'), 
                                       $r('app.string.featured_recommend'),
                                       $r('app.string.september_recommend'), 
                                       $r('app.string.august_recommend')]
     @Link showSettings: boolean
   
     build() {
       Column() {
         TopTabs({ showSettings: $showSettings }) // 自定义组件顶部标签
         ...... // 页面内容，此处省略
       }
     }
   }
   ```

3. 首页中引用自定义组件显示。

   ```
   import { HomeTabComponent } from '../common/homeTabContent' // import导入自定义组件
   
   @Entry
   @Component
   struct HomeComponent {
     build() {
       Stack({ alignContent: Alignment.BottomStart }) {
         Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.End, justifyContent: FlexAlign.End }) {
           Tabs({ barPosition: BarPosition.End, controller: this.controller }) {
             TabContent() {
               HomeTabComponent({ showSettings: $showSettings }) // TabContent的内容使用自定义组件HomeTabComponent
             }
             .padding({ left: 15, right: 15 })
           }
           .vertical(false)
           .barHeight(0) // 本篇Codelab中使用了自定义的底部页签，不使用Tabs的TabBar，所以BarHeight为0
           .width('100%')
           .scrollable(false)
         }
         BottomTabs({ controller: this.controller, bottomTabIndex: $bottomTabIndex }) // 底部页签容器组件
       }
       .width('100%')
       .height('100%')
     }
   }
   ```

# 推荐List布局<a name="ZH-CN_TOPIC_0000001237898151"></a>

首先，在首页中有今日推荐、精选推荐等推荐栏，在HomeTabComponent组件中，推荐栏放在List中作为List的ListItem组件显示。推荐列表使用ForEach语句，LisItem中使用自定义组件HomeListItem，在HomeListItem组件中会判断是否是今日推荐来获取不同的数据。

![](D:/openHarmonyAdapt/OpenHarmonyMD-File/极简声明式UI范式_1640677876081/极简声明式UI范式/figures/zh-cn_image_0000001192938616.png)

```
import {TopTabs} from '../common/topTabs'
import {HomeListItem} from '../common/homeListItem'

@Component
export struct HomeTabComponent {
  private recommends: Resource[] = [$r('app.string.today_recommend'), $r('app.string.featured_recommend'),
  $r('app.string.september_recommend'), $r('app.string.august_recommend')]

  build() {
    Column() {
      TopTabs()
      List() {
        ForEach(this.recommends, item => {
          ListItem() {
            HomeListItem({ titleIndex: this.recommends.indexOf(item) })
          }
        }, item => this.recommends.indexOf(item).toString())
      }
      .listDirection(Axis.Vertical)
      .width('100%')
      .layoutWeight(1)
    }
  }
}
```

然后，今日推荐、精选推荐等作为ListItem，使用自定义组件HomeListItem作为子组件，下面介绍自定义组件HomeListItem：

1. 在common中创建homeListItem.ets作为自定义组件。

   ![](D:/openHarmonyAdapt/OpenHarmonyMD-File/极简声明式UI范式_1640677876081/极简声明式UI范式/figures/zh-cn_image_0000001237898779.png)

2. HomeListItem中包括上面的title和下面图片列表两部分，使用ForEach语句显示图片列表。

   ```
   @Component
   export struct HomeListItem {
     private recommends: Resource[] = [$r('app.string.today_recommend'), 
                                       $r('app.string.featured_recommend'),
                                       $r('app.string.september_recommend'),
                                       $r('app.string.august_recommend')]
     private listItems: ImageData[] = initializeImageData()
     private titleIndex: number = 0
     private imageWidth: string = '120'
     private ratio: number = 1
   
     build() {
       Column() {
         Row() {
           Text(this.recommends[this.titleIndex]) // // 这里this.recommends[this.titleIndex]是推荐的title
             .textAlign(TextAlign.Start)
             .fontSize(16)
             .fontWeight(FontWeight.Bold)
             .layoutWeight(1)
             .borderColor(Color.Blue)
         }
         .width('100%')
   
         List({ initialIndex: 0 }) { // 图片列表
           ForEach(this.listItems, item => {
             ListItem() {
               Image(item.smallImg)
                 .width(this.imageWidth)
                 .aspectRatio(this.ratio)
                 .borderRadius(10)
                 .margin({ right: 15 })
             }
           })
         }
         .listDirection(Axis.Horizontal)
         .margin({ top: '2%', bottom: '2%' })
       }
     }
   }
   ```

   ![](D:/openHarmonyAdapt/OpenHarmonyMD-File/极简声明式UI范式_1640677876081/极简声明式UI范式/figures/IMG_20211222_162835.jpg)

# 大图浏览界面布局<a name="ZH-CN_TOPIC_0000001237738181"></a>

点击首页推荐栏中的图片，可以进入大图浏览界面，现在来实现大图界面的布局。

1. 首先在pages文件夹中,单击右键，然后新建eTS Page，命名为 image.ets。

   ![](D:/openHarmonyAdapt/OpenHarmonyMD-File/极简声明式UI范式_1640677876081/极简声明式UI范式/figures/zh-cn_image_0000001237738855.png)

2. 大图浏览界面从界面上分为顶部titleBar，底部toolBar，中间图片铺满屏幕。由于图片需要左右滑动切换，此处使用Swiper组件，Swiper中子组件使用Image。

   ```
   import { ImageData, initializeImageData, initializeTodayData } from '../model/HomeListDataModel'
   
   @Entry
   @Component
   struct ImageComponent{
     private imageSrc: ImageData[] = initializeImageData()
     private imageIndex: number = router.getParams().index
     private title: string = ''
     @State imageMargin: number = 56
     @State visibility: Visibility = Visibility.Visible
   
     build() {
       Flex({ direction: FlexDirection.Column,alignItems: ItemAlign.End, justifyContent: FlexAlign.End }) {
         Row() { 
         ...... // titleBar部分
         }
         .width('100%')
         .height(this.imageMargin)
         .backgroundColor('#F1F3F5')
         .visibility(this.visibility)
         Swiper(){ // 中间大图显示，使用Swiper组件
           ForEach(this.imageSrc, item => {
             Image(item.bigImg) // Image作为Swiper的子组件，显示图片
               .height('100%')
               .width('100%')
               .objectFit(ImageFit.Contain)
               },item => item.toString())
           }
         }
         .width('100%')
         .height('100%')
         Column() { 
         ...... // 底部工具栏部分
         }
         .width('100%')
       }
       .width('100%')
       .height('100%')
     }
   }
   ```

# 页面跳转和数据传递<a name="ZH-CN_TOPIC_0000001237778169"></a>

本篇Codelab中，页面跳转即首页推荐栏中的图片跳转到大图界面和大图界面点击返回图标回到首页，数据传递包括父组件向子组件传递参数，双向传递以及页面之间数据传递。

- 页面跳转及页面间数据传递。

  点击首页推荐栏中的图片跳转到大图界面，在homeListItem.ets中的图片列表中给Image添加点击事件，通过页面路由router完成页面跳转和参数传递。

  ```
  List({ initialIndex: 0 }) {
    ForEach(this.listItems, item => {
      ListItem() {
        Image(item.smallImg)
          .onClick(() => {
            let shareIdStr = this.titleIndex + item.id
            console.info('Item onClick' + shareIdStr)
            router.push({ // 通过页面路由router完成页面跳转
              uri: 'pages/image',
              params: { isToday: this.isToday, shareId: shareIdStr, index: item.id } // 参数传递
            })
          })
      }
    })
  }
  .listDirection(Axis.Horizontal)
  .margin({ top: '2%', bottom: '2%' })
  ```

  大图界面使用router.getParams\(\)获取参数，实现返回图片的onClick事件，执行router.back\(\)来返回首页。

  ```
  import router from '@system.router'
  @Entry
  @Component
  struct Index {
    // 获取参数
    private imageIndex: number = router.getParams().index
    private shareId: string = router.getParams().shareId
    private isToday: boolean = router.getParams().isToday
  
    build() {
      Flex({ direction: FlexDirection.Column,alignItems: ItemAlign.End, justifyContent: FlexAlign.End }) {
        Row() {
          Image('/common/arrow.png')
            .width('10%')
            .height('80%')
            .objectFit(ImageFit.Contain)
            .margin({ left: 10 })
            .onClick(() => {
              router.back() // 通过router.back()返回
            })
        }
      }
      .width('100%')
      .height('100%')
    }
  }
  ```

- 组件之间的数据传递。

  - 父组件向子组件传递参数。

    自定义组件作为子组件，父组件引用时直接传递参数，子组件中定义参数即可。

    ```
    //在homeTabContent.ets中引用自定义组件时直接传参数ForEach(this.recommends, item => {   ListItem() {     HomeListItem({ titleIndex: this.recommends.indexOf(item) })   }}, item => this.recommends.indexOf(item).toString())
    ```

    ```
    //在homeListItem.ets中定义的子组件中定义参数@Componentexport struct HomeListItem {  private titleIndex: number = 0  ......}
    ```

  - 父组件和子组件之间双向传递。

    父组件和子组件之间的双向传递使用状态数据管理，状态数据管理作为声明式UI的特色，通过功能不同的装饰器给开发者提供了清晰的页面更新渲染的流程和管道。状态管理包括UI组件状态和应用程序状态，两者协作可以使开发者完整地架构整个应用的数据更新和UI渲染。本篇Codelab中首页中的bottomTabIndex代表当前Tabs在第几页。双向传递父组件使用@State修饰的变量，子组件使用@Link修饰，且父组件需要使用$引用。

    ```
    @Entry@Componentstruct HomeComponent {  @State bottomTabIndex: number = 0 // 父组件需要时@State修饰  build() {    Stack({ alignContent: Alignment.BottomStart }) {      ......      BottomTabs({ controller: this.controller, bottomTabIndex: $bottomTabIndex }) // 使用$引用传递数据    }    .width('100%')    .height('100%')  }}
    ```

    ```
    @Componentexport struct BottomTabs {  private tabSrc: number[] = [0, 1, 2, 3]  private backgroundColor: string = '#F1F3F5'  private controller: TabsController = new TabsController()  @Link bottomTabIndex: number // 子组件中使用@Link修饰  ......}
    ```



# 动画<a name="ZH-CN_TOPIC_0000001236765407"></a>

- 添加共享元素转场动画。

  共享元素转场支持页面内的转场，如Row组件中的元素转场至List组件中；也支持页面间的转场，如当前页面的图片转场至下一页面中，只需要调用动画接口sharedTransition实现，使用非常简单。此处使用共享元素转场动画完成点击小图进入大图浏览界面和大图浏览界面返回的动画。两个页面的组件配置为同一个id，则转场过程中会进行共享元素转场，配置为空字符串时不会有共享元素转场效果。使用sharedTransition属性方法添加共享元素转场动画。

  ```
  // 在首页的HomeListItem.ets中的图片ListItem中添加共享元素转场动画
  List({ initialIndex: 0 }) {
    ForEach(this.listItems, item => {
      ListItem() {
        Image(item.smallImg)
          .width(this.imageWidth)
          .aspectRatio(this.ratio)
          .borderRadius(10)
          .margin({ right: 15 })
          .sharedTransition(this.titleIndex + item.id, { duration: 500, curve: Curve.Linear })
      }
    })
  }
  .listDirection(Axis.Horizontal)
  .margin({ top: '2%', bottom: '2%' })
  ```

  ```
  // 在image.ets加共享元素转场动画
  Swiper(){
    ForEach(this.imageSrc, item => {
      Image(item.bigImg)
        .height('100%')
        .width('100%')
    },item => item.toString())
  }
  .width('100%')
  .aspectRatio(this.ratio)
  .scale({ x: this.scale, y: this.scale })
  .index(this.imageIndex)
  .indicator(false)
  .loop(false)
  .sharedTransition(this.shareId, { duration: 500, curve: Curve.Linear })
  ```

# 恭喜你<a name="ZH-CN_TOPIC_0000001237085377"></a>

目前你已经成功完成了Codelab并且学到了：

-   如何通过极简声明式UI范式使用常用组件，包括Text、Image、 List、listItem、Tabs等。
-   如何通过极简声明式UI范式完成自定义弹窗。
-   如何通过极简声明式UI范式完成页面跳转及数据传递。
-   如何通过极简声明式UI范式完成动画添加。

# 参考<a name="ZH-CN_TOPIC_0000001192445396"></a>



[gitee地址](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/SimpleGalleryEts)

