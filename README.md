# OpenHarmony Codelabs<a name="ZH-CN_TOPIC_0000001228194435"></a>

-   [概要简介](#section117915431558)
-   [目录](#sectionMenu)
-   [使用说明](#section1954919258619)
-   [约束与限制](#section682025019613)
-   [相关仓](#section01752910717)

## 概要简介<a name="section117915431558"></a>

为帮助开发者快速熟悉OpenHarmony的能力以及相关的应用开发流程，我们提供了一系列的基于趣味场景的应用示例，即Codelabs，开发者可以根据我们的文档一步步的学习和完成简单项目的开发。

## 目录<a name="sectionMenu"></a>

- Ability
  - [Page内和Page间导航跳转](https://gitee.com/openharmony/codelabs/tree/master/Ability/PageAbility)（API8+）
- ArkUI
  - [极简声明式UI范式（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/SimpleGalleryEts)（API8+）
  - [购物应用（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/ShoppingEts)（API8+）
  - [购物应用（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/ShoppingOpenHarmony)（API8+）
  - [自定义组件（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/JSCanvasComponet)（API8+）
  - [转场动画的使用（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/TransitionAnimtaionEts)（API8+）
  - [基础组件Slider的使用（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/SliderApplicationEts)（API8+）
  - [image、image-animator（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/ClickableJsDemo)（API8+）
  - [动画样式（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/AnimationDemo)（API8+）
  - [dialog（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/DialogDemo)（API8+）
  - [input、label（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/InputApplication)（API8+）
  - [rating（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/RatingApplication)（API8+）
  - [slider（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/SliderApplication)（API8+）
  - [switch、chart（JS）](https://gitee.com/openharmony/codelabs/tree/master/JSUI/SwitchApplication)（API8+）
  - [流式布局（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/FlowLayoutEts)（API8+）
  - [弹窗（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/CustomDialogEts)（API8+）
  - [一次开发多端部署（eTS）](https://gitee.com/openharmony/codelabs/tree/master/ETSUI/MultiDeploymentEts)（API8+）
- 分布式
  - [分布式调度启动远程FA](https://gitee.com/openharmony/codelabs/tree/master/Distributed/RemoteStartFA)（API8+）
  - [分布式新闻客户端](https://gitee.com/openharmony/codelabs/tree/master/Distributed/NewsDemo)（API8+）
  - [分布式手写板（eTS）](https://gitee.com/openharmony/codelabs/tree/master/Distributed/DistributeDatabaseDrawEts)（API8+）
  - [分布式鉴权（JS）](https://gitee.com/openharmony/codelabs/tree/master/Distributed/GameAuthOpenH)（API8+）
  - [分布式游戏手柄（eTS）](https://gitee.com/openharmony/codelabs/tree/master/Distributed/HandleGameApplication)（API8+）
  - [分布式邮件（eTS）](https://gitee.com/openharmony/codelabs/tree/master/Distributed/OHMailETS)（API8+）
  - [分布式亲子早教系统（eTS）](https://gitee.com/openharmony/codelabs/tree/master/Distributed/OpenHarmonyPictureGame)（API8+）
  - [分布式遥控器（eTS）](https://gitee.com/openharmony/codelabs/tree/master/Distributed/RemoteControllerETS)（API8+）
- 媒体
  - [图片编辑模板](https://gitee.com/openharmony/codelabs/tree/master/Media/ImageEditorTemplate)（API8+）
  - [图片常见操作](https://gitee.com/openharmony/codelabs/tree/master/Media/ImageJsDemo)（API8+）
  - [简易视频播放器](https://gitee.com/openharmony/codelabs/tree/master/Media/VideoOpenHarmony)（API8+）
  - [音频播放器](https://gitee.com/openharmony/codelabs/tree/master/Media/Audio_OH_ETS)（API9+）
- 数据库
  - [分布式数据库](https://gitee.com/openharmony/codelabs/tree/master/Data/JsDistributedData)（API8+）
  - [关系型数据库](https://gitee.com/openharmony/codelabs/tree/master/Data/JSRelationshipData)（API8+）
  - [轻量级偏好数据库](https://gitee.com/openharmony/codelabs/tree/master/Data/Database)（API8+）
  - [备忘录（eTS）](https://gitee.com/openharmony/codelabs/tree/master/Data/NotePad_OH_ETS)（API8+）
- 设备开发
  - [轻量系统环境搭建指导](https://gitee.com/openharmony/codelabs/tree/master/Device/DeviceEnvironmentSetupGuide)（指导文档）
  - [日志、线程、定时器](https://gitee.com/openharmony/codelabs/tree/master/Device/%E6%97%A5%E5%BF%97%E3%80%81%E7%BA%BF%E7%A8%8B%E3%80%81%E5%AE%9A%E6%97%B6%E5%99%A8)（开发板类型：Hi3861，OpenHarmony系统：OpenHarmony 3.0 LTS）
  - [利用PWM方波控制呼吸灯和蜂鸣器](https://gitee.com/openharmony/codelabs/tree/master/Device/%E5%88%A9%E7%94%A8PWM%E6%96%B9%E6%B3%A2%E6%8E%A7%E5%88%B6%E5%91%BC%E5%90%B8%E7%81%AF%E5%92%8C%E8%9C%82%E9%B8%A3%E5%99%A8)（开发板类型：Hi3861，OpenHarmony系统：OpenHarmony 3.0 LTS）
  - [利用GPIO点亮小灯泡、红绿灯](https://gitee.com/openharmony/codelabs/tree/master/Device/%E5%88%A9%E7%94%A8GPIO%E7%82%B9%E4%BA%AE%E5%B0%8F%E7%81%AF%E6%B3%A1%E3%80%81%E7%BA%A2%E7%BB%BF%E7%81%AF)（开发板类型：Hi3861，OpenHarmony系统：OpenHarmony 3.0 LTS）
  - [远程台灯](https://gitee.com/openharmony/codelabs/tree/master/Device/RemoteLamp)（开发板类型：Hi3861，OpenHarmony系统：OpenHarmony 3.0 LTS）
  - [智能猫眼](https://gitee.com/openharmony/codelabs/tree/master/Device/smart_cat_eye)（开发板类型：Hi3516DV300，OpenHarmony系统：OpenHarmony 3.0.2 LTS）
  - [OpenHarmony最小系统移植](https://gitee.com/openharmony/codelabs/tree/master/Device/PortingOpenHarmony)（开发板类型：RK3568，OpenHarmony系统：Rk3568 3.1 Release）
  - [HDF驱动模型Wi-Fi驱动](https://gitee.com/openharmony/codelabs/tree/master/Device/WifiDemo)（开发板类型：Hi3516DV300，OpenHarmony系统：OpenHarmony 3.1 Release）
- 三方库
  - [VCard](https://gitee.com/openharmony/codelabs/tree/master/ThirdPartyComponents/VCardDemo)（API8+）

## 使用说明<a name="section1954919258619"></a>

1.  将独立的应用示例工程导入DevEco Studio进行编译构建及运行调试。
2.  部分应用示例中含有多个模块，开发者可以选择对单个模块进行编译构建，生成一个HAP应用安装包，也可以对整个工程进行编译构建，生成多个HAP应用安装包。
3.  安装运行后，即可在设备上查看应用示例运行效果，以及进行相关调试。

## 约束与限制<a name="section682025019613"></a>

1.  安装应用示例之前，请先查看"README_zh.md"文件来确认应用示例是否为stage模型，若为stage模型需要查看entry/src/main路径下的module.json5文件中的"deviceType"字段来确认该应用支持的设备类型；否则为FA模型，查看entry/src/main路径下的config.json文件中的"deviceType"字段来确认该应用示例支持的设备类型，两种模型都可尝试通过修改该字段使其可以在相应类型的设备上运行。
2.  IDE版本及配套SDK问题，可前往[DevEco Studio版本说明](https://developer.harmonyos.com/cn/docs/documentation/doc-releases/release_notes-0000001057597449)来查看详细的IDE、SDK、插件及Gradle配套版本。