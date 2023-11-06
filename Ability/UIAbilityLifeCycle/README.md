# UIAbility和自定义组件生命周期（ArkTS）

## 介绍

本文档主要描述了应用运行过程中UIAbility和自定义组件的生命周期。对于UIAbility，描述了Create、Foreground、Background、Destroy四种生命周期。对于页面和自定义组件描述了aboutToAppear、onPageShow、onPageHide、onBackPress、aboutToDisappear五种生命周期。

### 相关概念

-   [UIAbility](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-app-ability-uiAbility.md)：UIAbility是包含UI界面的应用组件，提供组件创建、销毁、前后台切换等生命周期回调，同时也具备组件协同的能力。
-   [自定义组件的生命周期](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-custom-component-lifecycle.md#%E8%87%AA%E5%AE%9A%E4%B9%89%E7%BB%84%E4%BB%B6%E7%9A%84%E7%94%9F%E5%91%BD%E5%91%A8%E6%9C%9F)：自定义组件的生命周期回调函数用于通知用户该自定义组件的生命周期，这些回调函数是私有的，在运行时由开发框架在特定的时间进行调用，不能从应用程序中手动调用这些回调函数。
-   [窗口开发指导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/windowmanager/window-overview.md)：窗口模块用于在同一块物理屏幕上，提供多个应用界面显示、交互的机制。

## 环境搭建

### 软件要求

-   [DevEco Studio](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)版本：DevEco Studio 3.1 Release。
-   OpenHarmony SDK版本：API version 9。

### 硬件要求

-   开发板类型：[润和RK3568开发板](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-appendix-rk3568.md)。
-   OpenHarmony系统：3.2 Release。

### 环境搭建

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**RK3568**开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。以3.2 Release版本为例：

   ![](figures/zh-cn_image_0000001554588725.png)

2. 搭建烧录环境。

   1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
   2.  [完成RK3568开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3568-burn.md)

3. 搭建开发环境。

   1.  开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
   2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#创建ets工程)创建工程（模板选择“Empty Ability”）。
   3.  工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#使用真机运行应用)。

## 代码结构解读

本篇Codelab只对核心代码进行讲解，对于完整代码，我们会在gitee中提供。

```
├──entry/src/main/ets                // 代码区
│  ├──common
│  │  ├──constants
│  │  │  └──CommonConstants.ets      // 公共常量类
│  │  └──util
│  │     └──Logger.ets               // 日志管理工具类
│  ├──entryability
│  │  └──EntryAbility.ts             // 程序入口类
│  └──pages
│     └──LifeCyclePage.ets           // 跳转页
└──entry/src/main/resources          // 资源文件目录
```

## UIAbility生命周期

当用户打开、切换和返回到应用时，UIAbility实例会在其生命周期的不同状态之间转换。UIAbility类提供了一系列回调，通过这些回调可以知道当前UIAbility实例的某个状态发生改变，UIAbility实例的创建和销毁，或者UIAbility实例发生了前后台的状态切换。Create、Foreground、Background、Destroy四个状态，如图所示：

![](/figures/UIAbility全生命周期.png)

**Create状态**：在应用首次创建UIAbility实例时触发，此时系统会调用onCreate\(\)回调。可以在该回调中进行应用初始化操作，例如变量定义资源加载等，用于后续的UI界面展示。

```typescript
// EntryAbility.ts
import UIAbility from '@ohos.app.ability.UIAbility';
import Window from '@ohos.window';

export default class EntryAbility extends UIAbility {
  ...
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    // 初始化应用
    ...
  }
  ...
}
```

**WindowStageCreate和WindowStageDestory状态**：UIAbility实例创建完成之后，在进入Foreground之前，系统会创建一个WindowStage。WindowStage创建完成后会进入onWindowStageCreate\(\)回调，可以在该回调中设置UI界面加载、设置WindowStage的事件订阅。

![](/figures/WindowStage状态.png)

在onWindowStageCreate\(\)回调中通过loadContent\(\)方法设置应用要加载的页面，并根据需要调用on\('windowStageEvent'\)方法订阅WindowStage的事件（获焦/失焦、可见/不可见）。

```typescript
// EntryAbility.ts
import UIAbility from '@ohos.app.ability.UIAbility';
import window from '@ohos.window';

export default class EntryAbility extends UIAbility {
  ...
  onWindowStageCreate(windowStage: window.WindowStage): void {
    this.windowStage = windowStage;
    // 设置WindowStage事件订阅（获取/失焦、可见/不可见）
    try {
      windowStage.on('windowStageEvent', (data) => {
        hilog.info(
          this.domain,
          'Succeeded in enabling the listener for window stage event changes. Data: %{public}',
          JSON.stringify(data) ?? ''
        );
      });
    } catch (exception) {
      hilog.error(
        this.domain,
        'Failed to enable the listener for window stage event changes. Cause: %{public}',
        JSON.stringify(exception) ?? ''
      );
    }
    // 设置UI加载
    windowStage.loadContent('pages/LifeCyclePage', (err, data) => {
      ...
    });
  }
}
```

**Foreground和Background状态**

Foreground和Background状态分别在UIAbility实例切换至前台和切换至后台时触发，对应于onForeground\(\)回调和onBackground\(\)回调。

-   onForeground\(\)回调，在UIAbility的UI界面可见之前，如UIAbility切换至前台时触发。可以在onForeground\(\)回调中申请系统需要的资源，或者重新申请在onBackground\(\)中释放的资源。
-   onBackground\(\)回调，在UIAbility的UI界面完全不可见之后，如UIAbility切换至后台时触发。可以在onBackground\(\)回调中释放UI界面不可见时无用的资源，或者在此回调中执行较为耗时的操作，例如状态保存等。

例如应用在使用过程中需要使用用户定位时，假设应用已获得用户的定位权限授权。在UI界面显示之前，可以在onForeground\(\)回调中开启定位功能，从而获取到当前的位置信息。

当应用切换到后台状态，可以在onBackground\(\)回调中停止定位功能，以节省系统的资源消耗。

```typescript
// EntryAbility.ts
import UIAbility from '@ohos.app.ability.UIAbility';

export default class EntryAbility extends UIAbility {
  ...
  onForeground(): void {
    // 申请系统所需的资源或在onBackground中申请释放的资源
  }

  onBackground(): void {
    // UI不可见时释放无用资源，或在此回调中执行耗时操作
    // 例如，状态保存
  }
}
```

**Destory状态**

Destroy状态在UIAbility实例销毁时触发。可以在onDestroy\(\)回调中进行系统资源的释放、数据的保存等操作。

例如调用terminateSelf\(\)方法停止当前UIAbility实例，从而完成UIAbility实例的销毁；或者用户使用最近任务列表关闭该UIAbility实例，完成UIAbility的销毁。

```typescript
// EntryAbility.ts
import UIAbility from '@ohos.app.ability.UIAbility';
import Window from '@ohos.window';

export default class EntryAbility extends UIAbility {
  ...
  onDestroy(): void | Promise<void> {
    // 释放系统资源，保存数据
  }
}
```

## 自定义组件生命周期

自定义组件的生命周期回调函数用于通知用户该自定义组件的生命周期，这些回调函数是私有的，在运行时由开发框架在特定的时间进行调用，不能从应用程序中手动调用这些回调函数。

>![](/public_sys-resources/icon-note.gif) **说明：** 
>
>-   允许在生命周期函数中使用Promise和异步回调函数，比如网络资源获取，定时器设置等。

自定义组件的生命周期包括aboutToAppear、onPageShow、onPageHide、onBackPress、aboutToDisappear五种状态，如图所示：

![](/figures/zh-cn_image_0000001547389382.png)

- **aboutToAppear**

  aboutToAppear?\(\): void

  aboutToAppear函数在创建自定义组件的新实例后，在执行其build\(\)函数之前执行。允许在aboutToAppear函数中改变状态变量，更改将在后续执行build\(\)函数中生效。


- **aboutToDisappear**

  aboutToDisappear?\(\): void

  aboutToDisappear函数在自定义组件析构销毁之前执行。不允许在aboutToDisappear函数中改变状态变量，特别是@Link变量的修改可能会导致应用程序行为不稳定。


- **onPageShow**

  onPageShow?\(\): void

  页面每次显示时触发一次，包括路由过程、应用进入前后台等场景，仅**@Entry**修饰的自定义组件生效。


- **onPageHide**

  onPageHide?\(\): void

  页面每次隐藏时触发一次，包括路由过程、应用进入前后台等场景，仅**@Entry**修饰的自定义组件生效。


- **onBackPress**

  onBackPress?\(\): void

  当用户点击返回按钮时触发，仅**@Entry**装饰的自定义组件生效。

  ```typescript
  // LifeCyclePage.ets
  @Entry
  @Component
  struct LifeCyclePage {
    @State textColor: Color = Color.Black;
  
    aboutToAppear() {
      this.textColor = Color.Blue;
      Logger.info('[LifeCyclePage]  LifeCyclePage aboutToAppear');
    }
  
    onPageShow() {
      this.textColor = Color.Brown;
      Logger.info('[LifeCyclePage]  LifeCyclePage onPageShow');
    }
  
    onPageHide() {
      Logger.info('[LifeCyclePage]  LifeCyclePage onPageHide');
    }
  
    onBackPress() {
      this.textColor = Color.Red;
      Logger.info('[LifeCyclePage]  LifeCyclePage onBackPress');
      return false;
    }
  
    aboutToDisappear() {
      Logger.info('[LifeCyclePage]  LifeCyclePage aboutToDisappear');
    }
  
    build() {
      Column() {
        Text($r('app.string.hello_message'))
          .fontSize(CommonConstants.DEFAULT_FONT_SIZE)
          .fontColor(this.textColor)
          .margin(CommonConstants.DEFAULT_MARGIN)
          .fontWeight(FontWeight.Bold)
      }
      .width(CommonConstants.FULL_WIDTH)
    }
  }
  ```

## 总结

您已经完成了本次Codelab的学习，并了解到以下知识点：

1. UIAbility生命周期。

2. 自定义组件生命周期。

![](figures/zh-cn_image_0000001509916136.gif)