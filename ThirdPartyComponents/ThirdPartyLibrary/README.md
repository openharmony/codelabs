# 库的调用（ArkTS）

## 介绍

本篇Codelab主要向开发者展示了在Stage模型中，如何调用已经上架到[三方库中心](https://gitee.com/openharmony-tpc/tpc_resource)的社区库和项目内创建的本地库。效果图如下：

![](figures/ThirdPartyLibrary.gif)

### 相关概念

- [Navigation](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-basic-components-navigation.md)：一般作为Page页面的根容器，通过属性设置来展示页面的标题、工具栏、菜单。

- [Tabs](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-tabs.md)：一种可以通过页签进行内容视图切换的容器组件，每个页签对应一个内容视图。

- [Canvas](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-components-canvas-canvas.md)：画布组件，用于自定义绘制图形。

- [OpenHarmony 共享包](https://gitee.com/openharmony-tpc/docs/blob/master/OpenHarmony_npm_usage.md)：OpenHarmony 共享包定义了特定的工程结构和配置文件，支持OpenHarmony页面组件相关API、资源的调用。

# 环境搭建

## 软件要求

- [DevEco Studio](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)版本：DevEco Studio 3.1 Release。
- OpenHarmony SDK版本：API version 9。

## 硬件要求

- 开发板类型：[润和RK3568开发板](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-appendix-rk3568.md)。
- OpenHarmony系统：3.2 Release。

## 环境搭建

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**RK3568**开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。以3.2 Release版本为例：

    ![](figures/zh-cn_image_0000001554588725.png)

2. 搭建烧录环境。
    1. [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
    2. [完成RK3568开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3568-burn.md)

3. 搭建开发环境。
    1. 开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2. 开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#创建ets工程)创建工程（模板选择“Empty Ability”）。
    3. 工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#使用真机运行应用)。

## 代码结构解读

本篇Codelab只对核心代码进行讲解，完整代码可以直接从gitee获取。

```
├──entry/src/main/ets                  // 代码区
│  ├──common
│  │  ├──constants                     // 常量文件
│  │  │  └──CommonConst.ets            // 通用常量
│  │  ├──lottie
│  │  │  └──data.json                  // 动画JSON文件
│  │  └──utils
│  │     └──log                        // 日志工具类
│  │        └──Logger.ets
│  ├──entryability
│  │  └──EntryAbility.ts               // 程序入口类
│  ├──pages
│  │  └──MainPage.ets                  // 主界面
│  ├──view
│  │  ├──InnerComponent.ets            // 本地库子页面
│  │  └──OuterComponent.ets            // 社区库子界面
│  └──viewmodel
│     ├──ButtonList.ets                // 按钮类
│     └──InnerViewModel.ets            // 本地库数据获取
├──entry/src/main/resources            // 资源文件
└──library/src/main/ets                // 本地库代码区       
   ├──components
   │  └──MainPage           
   │     └──Buttons.ets                // 本地库代码实现
   └──viewmodel
      └──ButtonsViewModel.ets          // 按钮数据类型
```

# 整体框架搭建

本篇Codelab由主页面、本地库组件页面、社区库组件页面三个页面组成，主页面由Navigation作为根组件实现全局标题，由Tabs组件实现本地库和社区库页面的切换，代码如下：

```typescript
// MainPage.ets
import { Outer } from '../view/OuterComponent';
import { Inner } from '../view/InnerComponent';
import { CommonConstants } from '../common/constants/CommonConst';

@Entry
@Component
struct Index {
  private controller: TabsController = new TabsController();
  @State currentIndex: number = 0;

  ...

  build() {
    Column() {
      Navigation() {
        Tabs({ barPosition: BarPosition.Start, controller: this.controller }) {
          TabContent() {
            Inner()
          }.tabBar(this.TabBuilder(CommonConstants.FIRST_TAB))

          TabContent() {
            Outer()
          }.tabBar(this.TabBuilder(CommonConstants.SECOND_TAB))
        }
        .barWidth(CommonConstants.BAR_WIDTH)
        .barHeight($r('app.float.default_56'))
        .onChange((index: number) => {
          this.currentIndex = index;
        })
      }
      .titleMode(NavigationTitleMode.Mini)
      .title(this.NavigationTitle)
      .hideBackButton(true)
    }
    .backgroundColor($r('app.color.app_bg'))
  }
}
```

在pages文件夹下新建components文件并在此文件夹下创建两个ArkTS文件，分别命名为inner和outer，至此整体框架搭建完毕。

# 本地库实现

本地库主要是指未上架到ohpm中心且在项目组内共享使用的库文件，这类库需要开发者在项目中创建并开发新的Library模块，创建步骤如下：

1. 通过如下两种方法，在OpenHarmony工程中添加OpenHarmony ohpm块。
    - 方法1：鼠标移到工程目录顶部，单击鼠标右键，选择New\>Module。
    - 方法2：在菜单栏选择File \> New \> Module。

2. 在Choose Your Ability Template界面中，选择Static Library，并单击Next。
3. 在Configure the New Module界面中，设置新添加的模块信息，设置完成后，单击Finish完成创建。
    - Module name：新增模块的名称。
    - Language：选择开发OpenHarmony ohpm包的语言。
    - Device type：选择OpenHarmony ohpm包支持的设备类型。
    - Enable Native：是否创建一个用于调用C++代码的OpenHarmony ohpm共享模块。

4. 创建完成后，会在工程目录中生成OpenHarmony ohpm共享模块及相关文件。

本Codelab在本地库中实现了对Button组件的简单封装，主要代码实现如下：

```typescript
// library/src/main/ets/components/MainPage/Buttons.ets
@Component
export struct Buttons {
  @Prop buttonText: string;
  @Prop stateEffect: boolean;
  @Prop buttonShape: string;
  @Prop buttonType: string;
  @Prop fontColor: string;

  build() {
    Row() {
      Column() {
        Button({ type: ButtonViewModel.fetchType(this.buttonShape), stateEffect: this.stateEffect }){
          Text(this.buttonText)
          .fontSize($r('app.float.default_16'))
          .fontColor(this.fontColor || $r('app.color.white'))
        }
        .width($r('app.float.default_90'))
        .height($r('app.float.default_35'))
        .backgroundColor(ButtonViewModel.fetchBackgroundColor(this.buttonType))
      }
    }
  }
}
```

如果想在Codelab的主工程代码中引用本地库，有如下两种方式：

方式一：在Terminal窗口中，执行如下命令进行安装，并会在package.json中自动添加依赖。

```
ohpm install ../library --save
```

方式二：在工程的oh\_package.json5中设置OpenHarmony ohpm三方包依赖，配置示例如下：

```json
"dependencies": {
  "@ohos/library": "file:../library"
}
```

依赖设置完成后，需要执行ohpm install命令安装依赖包，依赖包会存储在工程的oh\_modules目录下。

```
ohpm install
```

在完成上述步骤后，我们继续完成inner页面的开发，在inner页面中我们通过import的方式引入开发的本地库，并通过循环传入不同的参数展示不同的button，代码实现如下：

```typescript
// InnerComponent.ets
import { Buttons } from '@ohos/library';

@Component
export struct Inner {
  @State buttonList: ButtonList[] = InnerViewModel.getButtonListData();
  scroller: Scroller = new Scroller();

  build() {
    Scroll(this.scroller) {
      Column({ space: CommonConstants.SPACE_12 }) {
        ForEach(this.buttonList,  (item: ButtonList) => {
          Column() {
            Flex({
              direction: FlexDirection.Column,
              justifyContent: FlexAlign.SpaceBetween,
              alignItems: ItemAlign.Start
            }) {
              Column() {
                ...
              }
              .alignItems(HorizontalAlign.Start)

              Column() {
                Buttons({
                  buttonText: item.buttonText,
                  buttonShape: item.buttonShape,
                  buttonType: item.buttonType,
                  stateEffect: item.stateEffect,
                  fontColor: item.fontColor
                })
                  .alignSelf(ItemAlign.Center)
                  .margin({ bottom: $r('app.float.default_21') })
              }
              .width($r('app.float.default_260'))
              .height($r('app.float.default_90'))
              .backgroundImage($r('app.media.mobile'))
              .backgroundImageSize(ImageSize.Contain)
              .justifyContent(FlexAlign.End)
              .alignSelf(ItemAlign.Center)
              .align(Alignment.End)
            }
            .padding({
              bottom: $r('app.float.default_24')
            })
            .width(CommonConstants.CONTAINER_WIDTH)
            .height(CommonConstants.CONTAINER_HEIGHT)
          }
          .width(CommonConstants.CONTAINER_WIDTH)
          .aspectRatio(CommonConstants.ASPECT_RATIO_176)
          .padding({
            top: $r('app.float.default_12'),
            left: $r('app.float.default_8')
          })
          .backgroundColor($r('app.color.white'))
          .borderRadius($r('app.float.default_24'))
        })
      }
      .width(CommonConstants.CONTAINER_WIDTH)
      .padding({
        left: $r('app.float.default_12'),
        right: $r('app.float.default_12'),
        top: $r('app.float.default_12')
      })
    }
    .scrollable(ScrollDirection.Vertical)
    .scrollBar(BarState.Off)
    .margin({bottom: $r('app.float.default_24')})
  }
}
```

至此本地库的调用已完成。

# 社区库调用

社区库是指已经由贡献者上架到ohpm中心供其他开发者下载使用的库，调用这类库的方法如下：

然后通过如下两种方式设置OpenHarmony ohpm三方包依赖信息（下面步骤以@ohos/lottie三方库为例，其他库替换对应库的名字及版本号即可）：

- 方式一：在Terminal窗口中，执行如下命令安装OpenHarmony ohpm三方包，DevEco Studio会自动在工程的oh\_package.json中自动添加三方包依赖。

    ```
    ohpm install @ohos/lottie --save
    ```

- 方式二：在工程的oh\_package.json5中设置OpenHarmony ohpm三方包依赖，配置示例如下：

    ```json
    "dependencies": {
        "@ohos/lottie": "^2.0.0"
    }
    ```

    依赖设置完成后，需要执行ohpm install命令安装依赖包，依赖包会存储在工程的oh\_modules目录下。

    ```
    ohpm install
    ```

在完成上述步骤后，我们继续完成outer页面的开发，在outer页面中我们通过import的方式引入配置的社区库，并实现对社区库动画的调用，关键代码如下：

```typescript
// OuterComponent.ets
import lottie, { AnimationItem } from '@ohos/lottie';
import Logger from '../common/utils/log/logger';
import { CommonConstants } from '../common/constants/CommonConst';

@Component
export struct Outer {
  private renderingSettings: RenderingContextSettings = new RenderingContextSettings(true);
  private renderingContext: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.renderingSettings);
  private animateName: string = CommonConstants.ANIMATE_NAME;
  private animateItem: AnimationItem | null = null;
  @State canvasTitle: Resource | undefined = undefined;

  ...

  build() {
    Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.SpaceBetween }) {

      // Canvas area
      Column() {
        Canvas(this.renderingContext)
          .width(CommonConstants.CONTAINER_WIDTH)
          .aspectRatio(CommonConstants.ASPECT_RATIO_176)
          .backgroundImage($r('app.media.canvasBg'))
          .backgroundImageSize(ImageSize.Cover)
          .onDisAppear(() => {
            lottie.destroy(this.animateName);
          })
        ...
      }
      .margin({
        top: $r('app.float.default_10'),
        left: $r('app.float.default_10'),
        right: $r('app.float.default_10')
      })

      // Buttons area
      Column({ space: CommonConstants.SPACE_12 }) {
        Button() {
          ...
        }
        .width(CommonConstants.CONTAINER_WIDTH)
        .height($r('app.float.default_40'))
        .backgroundColor($r('app.color.outer_button_bg'))
        .onClick(() => {
          this.canvasTitle = $r('app.string.outer_button_load');
          this.animateItem = lottie.loadAnimation({
            container: this.renderingContext,
            renderer: 'canvas',
            loop: 10,
            autoplay: true,
            name: this.animateName,
            path: 'common/lottie/data.json'
          });
        })
        ...
        }
      }
      .padding({
        left: $r('app.float.default_23'),
        right: $r('app.float.default_23'),
        bottom: $r('app.float.default_41')
      })
    }
    .height(CommonConstants.CONTAINER_HEIGHT)
  }
}
```

至此本篇Codelab的开发已经完成。

## 总结

您已经完成了本次Codelab的学习，并了解到以下知识点：

1. 如何创建及调用本地三方库。
2. 如何调用社区三方库。

![](figures/zh-cn_image_0000001455125133.gif)
