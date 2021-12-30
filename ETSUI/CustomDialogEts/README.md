# 1.介绍

本文档将介绍警告弹窗、自定义弹窗的实现。效果图如下：

**图 1**  主页效果<a name="fig36936213910"></a>  


![](figures/IMG_20211214_194906.jpg)

**图 2**  弹窗效果<a name="fig20465286404"></a>  


![](figures/IMG_20211214_194929.jpg)![](figures/IMG_20211214_194949.jpg)![](figures/IMG_20211214_195001.jpg)

# 2.相关概念

1.[Button组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-button.md)

2.[警告弹窗](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-basic-components-button.md)

3.[自定义弹窗](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-methods-custom-dialog-box.md)

# 3.搭建OpenHarmony环境

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**Hi3516DV300**开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）

   以3.0版本为例：

   ![](figures/取版本.png)

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

   ![](figures/截图.png)

# 4.添加按钮Button组件

在这个任务中，我们需要设置Flex布局、并添加Button组件。

1.Flex页面布局

```
@Entry
@Component
struct Index {
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
    }
    .width('100%')
    .height('100%')
  }
}
```

2.添加Button组件

```
@Entry
@Component
struct Index {
  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Button('one button dialog')
       .backgroundColor(0x317aff)
       .width(300)
      Button('two button dialog')
       .backgroundColor(0x317aff)
       .margin({ top: 60 }).width(300)
      Button('Customization dialog')
       .backgroundColor(0x317aff)
       .margin({ top: 60 }).width(300)
    }
    .width('100%')
    .height('100%')
  }
}
```

# 5.添加点击弹窗事件

在这个任务中，我们要分别完成包含单个按钮、两个按钮的弹窗效果设计并为分别为其绑定按钮点击事件。

1.包含单个按钮的弹窗效果实现

![](figures/IMG_20211214_194929-0.jpg)

```
 Button('one button dialog').onClick(() => {
  AlertDialog.show(
	{
	  title: 'title',
	  message: 'text',
	  confirm: {
		value: 'button',
		action: () => {
		  console.info('Button-clicking callback')
		}
	  },
	  cancel: () => {
		console.info('Closed callbacks')
	  }
	}
  )
}).backgroundColor(0x317aff)
.width(300)
```

2.包含两个按钮的弹窗效果实现

![](figures/IMG_20211214_194949-1.jpg)

```
Button('two button dialog').onClick(() => {
  AlertDialog.show(
	{
	  title: 'title',
	  message: 'text',
	  primaryButton: {
		value: 'cancel',
		action: () => {
		  console.info('Callback when the first button is clicked')
		}
	  },
	  secondaryButton: {
		value: 'ok',
		action: () => {
		  console.info('Callback when the second button is clicked')
		}
	  },
	  cancel: () => {
		console.info('Closed callbacks')
	  }
	}
  )
}).backgroundColor(0x317aff)
.margin({ top: 60 }).width(300)
```

# 6.自定义弹窗效果实现

在这个任务中，我们要完成自定义内容的弹窗界面设计、并绑定相应的事件去打开或者关闭自定义弹窗。

![](figures/IMG_20211214_195001-2.jpg)

1.自定义内容的弹窗界面设计，并绑定关闭弹窗的点击事件。

-   通过CustomDialogController类显示自定义弹窗

```
@CustomDialog
struct CustomDialogExample {
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  build() {
    Column() {
      Text('Software uninstall').width('70%').fontSize(20).margin({ top: 10, bottom: 10 })
      Image($r('app.media.icon')).width(80).height(80)
      Text('Whether to uninstall a software?').fontSize(16).margin({ bottom: 10 })
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Button('cancel')
          .onClick(() => {
            this.controller.close()
            this.cancel()
          }).backgroundColor(0xffffff).fontColor(Color.Black)
        Button('confirm')
          .onClick(() => {
            this.controller.close()
            this.confirm()
          }).backgroundColor(0xffffff).fontColor(Color.Red)
      }.margin({ bottom: 10 })
    }
  }
}
```

2.引入自定义弹窗组件，并设置打开自定义弹窗的事件。

-   引入自定义弹窗组件

```
@Entry
@Component
struct Index {
  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExample({ cancel: this.onCancel, confirm: this.onAccept }),
    cancel: this.existApp,
    autoCancel: true
  })
  onCancel() {
    console.info('Callback when the first button is clicked')
  }
  onAccept() {
    console.info('Callback when the second button is clicked')
  }
  existApp() {
    console.info('Click the callback in the blank area')
  }
}

```

-   设置打开自定义弹窗的事件

```
Button('Customization dialog').onClick(() => {
  this.dialogController.open()
}).backgroundColor(0x317aff)
.margin({ top: 60 }).width(300)
```

# 7.恭喜你

-   AlertDialog可以实现警告弹窗效果。
-   可以通过CustomDialogController类显示自定义弹窗。

# 8.参考

[gitee地址](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/CustomDialogEts)