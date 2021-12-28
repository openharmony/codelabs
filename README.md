# OpenHarmony Codelabs<a name="ZH-CN_TOPIC_0000001228194435"></a>

-   [概要简介](#section117915431558)
-   [目录](#sectionMenu)
-   [使用说明](#section1954919258619)
-   [约束与限制](#section682025019613)
-   [相关仓](#section01752910717)

## 概要简介<a name="section117915431558"></a>

为帮助开发者快速熟悉OpenHarmony的能力以及相关的应用开发流程，我们提供了一系列的基于趣味场景的应用示例，即Codelabs，开发者可以根据我们的文档一步步的学习和完成简单项目的开发。

## 项目列表<a name="sectionMenu"></a>
| 序号 | 项目名称                                                     | 简介                                                         |
| ---- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 1    | [分布式调度启动远程FA](https://gitee.com/openharmony/codelabs/tree/master/Distributed/RemoteStartFA) | 基于分布式调度的能力，实现远程FA的启动。                     |
| 2    | [分布式新闻客户端](https://gitee.com/openharmony/codelabs/tree/master/Distributed/NewsDemo) | 基于OpenHarmony Page Ability的使用，实现跨设备FA拉起。       |
| 3    | [分布式手写板（eTS）](https://gitee.com/openharmony/codelabs/tree/master/Distributed/DistributeDatabaseDrawEts) | 基于分布式能力，实现多设备同步书写互动。                     |
| 4    | [分布式数据库](https://gitee.com/openharmony/codelabs/tree/master/Data/JsDistributedData) | 基于分布式数据接口，实现多种设备上一致的数据访问体验。       |
| 5    | [关系型数据库](https://gitee.com/openharmony/codelabs/tree/master/Data/JSRelationshipData) | 基于关系型数据库和数据管理能力，实现数据库相关应用服务的快速开发。 |
| 6    | [轻量级偏好数据库](https://gitee.com/openharmony/codelabs/tree/master/Data/Database) | 基于轻量级偏好数据库，实现存储在本地应用数据的访问及操作。   |
| 7    | [图片编辑模板](https://gitee.com/openharmony/codelabs/tree/master/Media/ImageEditorTemplate) | 基于图片处理能力，实现一个图片编辑模板。                     |
| 8    | [图片常见操作](https://gitee.com/openharmony/codelabs/tree/master/Media/ImageJsDemo) | 基于图像编解码，实现图片的旋转、剪裁、缩放、镜像。           |
| 9    | [简易视频播放器](https://gitee.com/openharmony/codelabs/tree/master/Media/VideoOpenHarmony) | 基于OpenHarmony Video组件，实现视频文件的播放。              |
| 10   | [极简声明式UI范式（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/SimpleGalleryEts) | 基于OpenHarmony eTS UI，实现一个简单的图库应用。             |
| 11   | [一次开发多端部署（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/MultiDeploymentEts) | 基于OpenHarmony eTS UI，实现一次布局，多端部署。             |
| 12   | [购物应用（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/ShoppingEts) | 基于OpenHarmony eTS UI丰富的组件实现购物商城应用。           |
| 13   | [购物应用（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/ShoppingOpenHarmony) | 基于OpenHarmony JS UI丰富的组件实现购物商城应用。            |
| 14   | [自定义组件](https://gitee.com/openharmony/codelabs/tree/master/JSUI/JSCanvasComponet) | 通过一个圆形抽奖转盘演示OpenHarmony自定义组件的实现。        |
| 15   | [Page内和Page间导航跳转](https://gitee.com/openharmony/codelabs/tree/master/Ability/PageAbility) | 入门教程，学习如何完成Page内和Page间的页面导航跳转。         |
| 16   | [OpenHarmony最小系统移植](https://gitee.com/openharmony/codelabs/tree/master/Device/PortingOpenHarmony) | 介绍如何移植一个最小操作系统，包括开发板配置、产品配置等等。 |
| 17   | [HDF驱动模型Wi-Fi驱动](https://gitee.com/openharmony/codelabs/tree/master/Device/WifiDemo) | 基于HDF Wi-Fi模型，进行Wi-Fi的适配开发，实现了连接和交互功能。 |
| 18   | [转场动画的使用（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/TransitionAnimtaionEts) | 基于OpenHarmony eTS UI转场动画，实现了页面间转场、组件内转场以及共享元素转场。 |
| 19   | [基础组件Slider的使用（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/SliderApplictionEts) | 基于OpenHarmony eTS UI，使用slider组件，实现可调节风车大小和转速的动画效果。 |
| 20   | [image、image-animator（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/ClickableJsDemo) | 基于OpenHarmony JS UI，使用Image、image-animator，实现图片展示与界面交互的动态效果。 |
| 21   | [动画样式（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/AnimationDemo) | 基于OpenHarmony JS UI，使用动画样式实现平移、旋转、缩放以及透明度变化的效果。 |
| 22   | [dialog（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/DialogDemo) | 基于OpenHarmony JS UI，使用dialog组件实现几种常用的弹窗效果。 |
| 23   | [input、label（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/InputApplication) | 基于OpenHarmony JS UI，使用input组件，实现form表单的输入以及表单验证效果。 |
| 24   | [rating（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/RatingApplication) | 基于OpenHarmony JS UI，使用rating组件，实现星级打分的效果。  |
| 25   | [slider（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/SliderApplication) | 基于OpenHarmony JS UI，使用slider组件，实现可调节风车大小和转速的动画效果。 |
| 26   | [switch、chart（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/SwitchApplication) | 基于OpenHarmony JS UI，使用switch、chart组件，实现可以切换数据动静的展示效果。 |
| 27   | [流式布局（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/FlowLayoutEts) | 基于OpenHarmony eTS UI，实现流式布局效果。                   |
| 28   | [弹窗（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/CustomDialogEts) | 基于OpenHarmony eTS UI，实现警告弹窗和自定义弹窗。           |


## 使用说明<a name="section1954919258619"></a>

1.  将独立的应用示例工程导入DevEco Studio进行编译构建及运行调试。
2.  部分应用示例中含有多个模块，开发者可以选择对单个模块进行编译构建，生成一个HAP应用安装包，也可以对整个工程进行编译构建，生成多个HAP应用安装包。
3.  安装运行后，即可在设备上查看应用示例运行效果，以及进行相关调试。

## 约束与限制<a name="section682025019613"></a>

1.  安装运行应用示例之前，请先通过config.json文件中的"deviceType"字段来确认该应用示例支持的设备类型，可尝试通过修改该字段使其可以在相应类型的设备上运行（config.json文件一般在代码的entry/src/main路径下，不同的Codelabs可能会有不同）。
2.  配置开发环境时，如果您想让应用示例运行到HarmonyOS上，请参考[DevEco Studio使用说明](https://developer.harmonyos.com/cn/docs/documentation/doc-guides/tools_overview-0000001053582387)。如果您想让应用示例运行到OpenHarmony上，请参考[DevEco Studio（OpenHarmony）使用指南](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/Readme-CN.md)。
3.  IDE版本及配套SDK问题，可前往[DevEco Studio版本说明](https://developer.harmonyos.com/cn/docs/documentation/doc-releases/release_notes-0000001057597449)来查看详细的IDE、SDK、插件及Gradle配套版本。
4.  所有HarmonyOS相关Codelabs代码已被全部迁移至[Harmony组织](https://gitee.com/harmonyos)之下的[harmonyos\_codelabs](https://gitee.com/harmonyos/harmonyos_codelabs)仓中，本仓中不再体现。

## 相关仓<a name="section01752910717"></a>

1.  HarmonyOS Codelabs仓：[harmonyos\_codelabs](https://gitee.com/harmonyos/harmonyos_codelabs)
2.  HarmonyOS Codelabs官网：[HarmonyOS Codelabs](https://developer.harmonyos.com/cn/documentation/codelabs/)

