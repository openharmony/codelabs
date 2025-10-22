# 自定义下拉刷新动画（ArkTS）

## 介绍

本篇Codelab主要介绍组件动画animation属性设置。当组件的某些通用属性变化时，可以通过属性动画实现渐变效果，提升用户体验。效果如图所示：

![](figures/AnimateRefresh.gif)

> ![](public_sys-resources/icon-note.gif) **说明：**
> 本Codelab使用的display接口处于mock阶段，在预览器上使用会显示白屏现象，可选择在真机或模拟器上运行。

### 相关概念

- [属性动画](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-animatorproperty.md)：组件的某些通用属性变化时，可以通过属性动画实现渐变效果，提升用户体验。支持的属性包括width、height、backgroundColor、opacity、scale、rotate、translate等。案例中自定义头部组件的属性动画设置主要涉及duration\(动画时长\)、tempo\(动画速率\)、delay\(动画延时\)、curve\(动画曲线\)、palyMode\(动画模式\)和iterations（动画播放次数）。

## 环境搭建

### 软件要求

- [DevEco Studio](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)版本：DevEco Studio 3.1 Release。
- OpenHarmony SDK版本：API version 9。

### 硬件要求

- 开发板类型：[润和RK3568开发板](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-appendix-rk3568.md)。
- OpenHarmony系统：3.2 Release。

### 环境搭建

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**RK3568**开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。以3.2 Release版本为例：

   ![](figures/zh-cn_image_0000001405854998.png)

2. 搭建烧录环境。

    1. [完成DevEco Device Tool的安装](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
    2. [完成RK3568开发板的烧录](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3568-burn.md)

3. 搭建开发环境。

    1. 开始前请参考[工具准备](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2. 开发环境配置完成后，请参考[使用工程向导](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#创建ets工程)创建工程（模板选择“Empty Ability”）。
    3. 工程创建完成后，选择使用[真机进行调测](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#使用真机运行应用)。

## 代码结构解读

本篇Codelab只对核心代码进行讲解，完整代码可以直接从gitcode获取。

```
├──entry/src/main/ets                      // 代码区           
│  ├──common
│  │  ├──constants                     
│  │  │  ├──CommonConstants.ets            // 公共常量类
│  │  │  └──RefreshConstants.ets           // 下拉刷新常量类
│  │  └──utils                 
│  │     ├──DimensionUtil.ets              // 屏幕适配工具类
│  │     └──GlobalContext.ets              // 全局上下文工具类
│  ├──entryability
│  │  └──EntryAbility.ts                   // 程序入口类
│  ├──pages
│  │  ├──FileManagerIndex.ets              // 文件管理Tab页
│  │  └──TabIndex.ets                      // Tab管理页
│  ├──view
│  │  ├──RefreshAnimHeader.ets             // 动画刷新组件
│  │  ├──RefreshComponent.ets              // 下拉刷新组件
│  │  └──RefreshDefaultHeader.ets          // 默认刷新组件
│  └──viewmodel
│     ├──AnimationModel.ets                // 动画封装模型
│     └──CardModel.ets                     // 页签封装模型
└──entry/src/main/resources                // 资源文件目录
```

## 自定义下拉组件

自定义下拉刷新通过自定义List组件RefreshComponent实现。在List容器中添加自定义刷新头部组件和其它的需要刷新部件，RefreshComponent提供了头部样式设置，刷新部件样式设置和刷新回调方法设置。

```typescript
// FileManagerIndex.ets
RefreshComponent({
  headerStyle: RefreshHeaderStyle.CLOUD, // 头部样式设置
  itemLayout: () => this.ContentBody(), // 刷新部件样式
  displayHeight: (
    px2vp(this.deviceDisplay.height) - DimensionUtil.getVp($r('app.float.file_index_title_height'))),
  onRefresh: () => { // 刷新回调方法
    ......
  }
})
```

- 头部样式设置

  本Codelab提供了DEFAULT默认刷新样式和CLOUD云朵动画刷新样式设置，在RefreshComponent组件初始化时，判断当前刷新样式进行渲染。

  ```typescript
  // RefreshComponent.ets
  if (this.headerStyle === RefreshHeaderStyle.DEFAULT) {
    RefreshDefaultHeader().height(RefreshConstants.REFRESH_HEADER_HEIGHT)
  } else if (this.headerStyle === RefreshHeaderStyle.CLOUD) {
    RefreshAnimHeader().height(RefreshConstants.REFRESH_HEADER_HEIGHT)
  }
  ```

- 刷新部件样式

  刷新部件样式itemLayout为嵌入RefreshComponent组件中的元素，通过@BuilderParam装饰符定义，可根据具体业务需求，当前为默认的Image组件样式。

  ```typescript
  // FileManagerIndex.ets
  @Builder ContentBody() {
    Image($r('app.media.bg_content'))
      .width(CommonConstants.FULL_LENGTH)
      .height(DimensionUtil.getVp($r('app.float.content_height')))
      .objectFit(ImageFit.Fill)
  }
  ```

- 刷新回调方法设置

  刷新回调方法是RefreshComponent提供的当下拉一定范围距离松手后，调用页面刷新的方法。实现步骤是：

  1.设置RefreshComponent刷新组件state状态的更新。

  ```typescript
  // RefreshComponent.ets
  @Consume(RefreshConstants.REFRESH_STATE_TAG) @Watch('onStateChanged') state: RefreshState;
  @BuilderParam itemLayout: () => void;
  
  private onStateChanged() {
    switch (this.state) {
      case RefreshState.REFRESHING:
        if (this.onRefresh) {
          this.onRefresh();
        }
        break;
      ......
    }
  }
  ```

  2.监听RefreshComponent组件的触摸事件，当手指松开且List组件下拉距离超出可刷新距离时，修改当前状态为REFRESHING，回调“onStateChanged”方法触发外部传入的onRefresh事件。

  ```typescript
  // RefreshComponent.ets
  case TouchType.Move:
    if (this.state === RefreshState.DRAGGING
    && this.listController.currentOffset().yOffset <= -RefreshConstants.REFRESH_EFFECTIVE_HEIGHT) {
      this.state = RefreshState.DRAGGING_REFRESHABLE;
    }
    break;
  case TouchType.Up:
    if (this.state === RefreshState.DRAGGING_REFRESHABLE) {
      this.headerOffset = 0;
      this.state = RefreshState.REFRESHING;
    }
    break;
  ```

  3.本Codelab中onRefresh事件没有做相关刷新动作，只做了模拟延时操作，开发者可以自行加入真实网络加载动作。

  ```typescript
  // RefreshComponent.ets
  onRefresh: () => {
    setTimeout(() => {
      this.state = RefreshState.COMPLETE;
    }, CommonConstants.REFRESH_DEFAULT_TIMEOUT);
  }
  ```

## 自定义刷新动画

本Codelab中自定义刷新是5个图片的组合动画效果。

1. 每个Image通过iconItem参数分别设置各自的x轴偏移量和延时播放的属性动画效果。

   ```typescript
   // RefreshAnimHeader.ets
   @Builder AttrAnimIcons(iconItem: ClassifyModel) {
     Image(iconItem.imgRes)
       .width(px2vp(DimensionUtil.adaptDimension(this.iconWidth)))
       .position({ x: iconItem.posX })
       .objectFit(ImageFit.Contain)
       .animation({
         duration: CommonConstants.REFRESH_HEADER_ITEM_ANIM_DURATION,
         tempo: CommonConstants.REFRESH_HEADER_ITEM_ANIM_TEMPO,
         delay: iconItem.delay,
         curve: Curve.Linear,
         playMode: PlayMode.Alternate,
         iterations: CommonConstants.REFRESH_HEADER_ITEM_ANIM_ITERATIONS
       })
   }
   ```

2. 监听RefreshComponent刷新组件state状态的变化，当前状态为REFRESHING状态时，启动动画效果。

   ```typescript
   // RefreshAnimHeader.ets
   @Consume(RefreshConstants.REFRESH_STATE_TAG) @Watch('onStateCheck') state: RefreshState;
   @State iconWidth: number = CommonConstants.REFRESH_HEADER_ITEM_DEFAULT_WIDTH;
   
   private onStateCheck() {
     if (this.state === RefreshState.REFRESHING) {
       this.iconWidth = CommonConstants.REFRESH_HEADER_ITEM_SCALE_WIDTH;
     } else {
       this.iconWidth = CommonConstants.REFRESH_HEADER_ITEM_DEFAULT_WIDTH;
     }
   }
   ```

## 总结

您已经完成了本次Codelab的学习，并了解到以下知识点：

1. 使用属性动画实现自定义下拉组件。
2. 使用属性动画实现自定义刷新组件。

![](figures/zh-cn_image_0000001409082962.gif)

