# 环境配置

目前环境采用的是

* DevEco Studio 6.0.2 Beta1
* Node 16.20.2
* npm 8.19.4

## 安装步骤

### DevEco Studio

登录[最新版本 - 下载中心 - 华为开发者联盟](https://developer.huawei.com/consumer/cn/download/)下载第一个即可，注意在安装的时候（选择是否创建快捷方式阶段）勾选上加入环境变量。

<img src="C:\Users\19339\Desktop\Harmony\Readme_environment.assets\image-20260119201132851.png" alt="image-20260119201132851" style="zoom: 33%;" />

### Node & npm

首先配置nvm
在网站上登录[Releases · coreybutler/nvm-windows](https://github.com/coreybutler/nvm-windows/releases)，我下载的是1.1.7版本
直接解压安装。

之后将如下代码加入到 nvm 安装目录下的settings.txt文件中的内容后面。

```
node_mirror: https://npmmirror.com/mirrors/node/
npm_mirror: https://npmmirror.com/mirrors/npm/
```

采用如下命令开始安装（好像要加环境变量，不过我看着环境变量在安装结束之后直接加上了）：

```
nvm install 16.20.2
```

然后等待安装结束就行（我这边安装时间很长，如果在安装那一栏一段时间没动别怕）



## 编译器配置

安装并打开DevEco Studio的时候，就可以自动开始配置环境，等着下载就行。

下载结束后，在右上角的No Devices处开始配置设备

![image-20260119202050767](C:\Users\19339\Desktop\Harmony\Readme_environment.assets\image-20260119202050767.png)

点击弹出页面右下角 Agree 按钮之后选择手机安装就行

在Device Manager 处开始配置设备，我选择的是

![image-20260119202135388](C:\Users\19339\Desktop\Harmony\Readme_environment.assets\image-20260119202135388.png)

点击Actions下面那个运行按钮就可以开启设备，然后在代码页面点击运行就可以跑代码