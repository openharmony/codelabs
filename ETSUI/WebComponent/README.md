# Web组件抽奖案例（ArkTS）

## 介绍

本篇Codelab是基于ArkTS的声明式开发范式的样例，主要介绍了Web组件如何加载本地和云端H5小程序。所加载的页面是一个由HTML+CSS+JavaScript实现的完整小应用。样例主要包含以下功能：

1.  web组件加载H5页面。
2.  ArkTS和H5页面交互。

![image](figures/web_oh.gif)

### 相关概念

-   [Web](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-basic-components-web.md)：提供具有网页显示能力的Web组件。

-   [runJavaScript](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkui/apis/js-apis-webview.md#runjavascript)：异步执行JavaScript脚本，并通过回调方式返回脚本执行的结果。

### 相关权限

本篇Codelab使用了在线网页，需要在配置文件module.json5文件里添加网络权限：ohos.permission.INTERNET。

```json
{
  "module": {
    "name": "entry",
    ...
    "requestPermissions": [
      {
        "name": "ohos.permission.INTERNET"
      }
    ]
  }
}
```
### 约束与限制

本篇Codelab需要搭建服务端环境，服务端搭建流程如下：

1. 搭建nodejs环境：本篇Codelab的服务端是基于nodejs实现的，需要安装nodejs，如果您本地已有nodejs环境可以跳过此步骤。

   1. 检查本地是否安装nodejs：打开命令行工具（如Windows系统的cmd和Mac电脑的Terminal，这里以Windows为例），输入node -v，如果可以看到版本信息，说明已经安装nodejs。

      ![](figures/node.PNG)

   2. 如果本地没有nodejs环境，您可以去nodejs官网上下载所需版本进行安装配置。

   3. 配置完环境变量后，重新打开命令行工具，输入node -v，如果可以看到版本信息，说明已安装成功。

2. 运行服务端代码：在本项目的HttpServerOfWeb目录下打开命令行工具，输入npm install 安装服务端依赖包，安装成功后输入npm start点击回车。看到“服务器启动成功！"则表示服务端已经在正常运行。

   ![](figures/npm.PNG)

3. 构建局域网环境：测试本Codelab时要确保运行服务端代码的电脑和测试机连接的是同一局域网下的网络，您可以用您的手机开一个个人热点，然后将测试机和运行服务端代码的电脑都连接您的手机热点进行测试。

4. 连接服务器地址：打开命令行工具，输入ipconfig命令查看本地ip，将本地ip地址复制到entry/src/main/ets/common/constants/Constants.ets文件下的23行，注意只替换ip地址部分，不要修改端口号，保存好ip之后即可运行Codelab进行测试。

## 环境搭建

### 软件要求

-   [DevEco Studio](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)版本：DevEco Studio 3.1 Release。
-   OpenHarmony SDK版本：API version 9。

### 硬件要求

-   开发板类型：[润和RK3568开发板](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-appendix-rk3568.md)。
-   OpenHarmony系统：3.2 Release。

### 环境搭建

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**RK3568**开发板为例，参照以下步骤进行：

1.  [获取OpenHarmony系统版本](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。以3.2 Release版本为例：

    ![](figures/zh-cn_image_0000001405854998.png)

2.  搭建烧录环境。
    1.  [完成DevEco Device Tool的安装](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
    2.  [完成RK3568开发板的烧录](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3568-burn.md)

3.  搭建开发环境。
    1.  开始前请参考[工具准备](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#创建ets工程)创建工程（模板选择“Empty Ability”）。
    3.  工程创建完成后，选择使用[真机进行调测](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#使用真机运行应用)。

## 代码结构解读

本篇Codelab只对核心代码进行讲解，对于完整代码，我们会在gitee中提供。

```
├──entry/src/main/ets        // 代码区
│  ├──common
│  │  └──Constant.ets        // 常量类
│  ├──entryability            
│  │  └──EntryAbility.ts     // 程序入口类
│  └──pages
│     ├──MainPage.ets        // 主页入口文件
│     └──WebPage.ets         // 抽奖页入口文件
├──entry/src/main/resources  
│  ├──base
│  │  ├──element             // 尺寸、颜色、文字等资源文件存放位置
│  │  ├──media               // 媒体资源存放位置
│  │  └──profile             // 页面配置文件存放位置
│  ├──en_US                  // 国际化英文
│  ├──rawfile                // 本地html代码存放位置 
│  └──zh_CN                  // 国际化中文
└──HttpServerOfWeb           // 服务端代码
```
## H5小程序

抽奖小程序由HTML+CSS+JS实现，HTML代码使用无序列表实现抽奖盘页面布局：

```html
<!-- index.html -->
<div class="luckyDraw">
    <!-- 使用无序列表实现抽奖盘 -->
    <ul id="prize" class="prizes">
        ...
        <li class="prizes-li"><img class="pic" onclick="startDraw()" src="#"></li>
    </ul>
</div>
```
CSS代码设置抽奖盘的样式：

```css
/* css/index.css */
/* 抽奖列表 */
.prizes {
    width: 96.5%;
    height: 96.7%;
    position: absolute;
}
...
/* 点击抽奖 */
.prizes li:nth-of-type(9) {
    width: 34.9%;
    height: 34.6%;
    ...
}
...
```

JS代码实现抽奖的业务逻辑，并返回抽奖结果：

```javascript
// js/index.js
function roll() {
  ...
  // 满足转圈数和指定位置就停止
  if (count >= totalCount && (prizesPosition + 1) === index) {
    clearTimeout(timer);
    isClick = true;
    speed = initSpeed;
    // 等待1s打开弹窗
    timer = setTimeout(openDialog, 1000); 
  }
  ...
}

function startDraw() {
  ...
  if (isClick) {
    ...
    roll();
    isClick = false;
  }
}

function openDialog() {
  // confirm返回抽奖结果
  confirm(prizesArr[prizesPosition]);
}
```

## Web组件

启动应用进入首页，页面提供两个按钮，分别对应加载本地H5和加载云端H5，点击按钮跳转到抽奖页面。

![](figures/2022092300914.png)

```typescript
// MainPage.ets
Column() {
  ...
  Navigator({ target: WEB_PAGE_URI, type: NavigationType.Push }) {
    Button($r('app.string.loadLocalH5'))
      ...
  }
  .params({ path: LOCAL_PATH, tips: $r('app.string.local') })

  Navigator({ target: WEB_PAGE_URI, type: NavigationType.Push }) {
    Button($r('app.string.loadCloudH5'))
      ...
  }
  .params({ path: CLOUD_PATH, tips: $r('app.string.online') })
}
```

>![](public_sys-resources/icon-note.gif) **说明：**
>H5页面本地存放在resources/rawfile目录下，通过$rawfile()访问；云端则存放在HttpServerOfWeb服务器上，开发者可以根据约束与限制章节服务端搭建流程进行服务器搭建。

抽奖页面主要是由“点击抽奖”按钮和Web组件构成。给“点击抽奖”按钮绑定点击事件，实现点击按钮调用H5页面的JavaScript函数，并且通过onConfirm回调返回抽奖结果，在原生页面弹窗显示，完成ArkTS和H5的双向交互。

![](figures/图片2.png)

```typescript
// WebPage.ets
Column() {
  ...
  Web({ src: this.params['path'], controller: this.webController })
    ...
    // 网页调用confirm()告警时触发此回调
    .onConfirm((event) => {
      // 弹窗显示抽奖结果
      AlertDialog.show({
        message: WebConstant.WEB_ALERT_DIALOG_TEXT_VALUE + event.message,
        ...
      })
      return true;
    })
    ...
  Column() {
    Text($r('app.string.textValue'))
      ...
    Text(this.params['tips'])
      ...
  }
  Button($r('app.string.btnValue'))
    ...
    .onClick(() => {
      // 异步执行JavaScript脚本
      this.webController.runJavaScript('startDraw()');
    })
}
```

## 总结

您已经完成了本次Codelab的学习，并了解到以下知识点：

1. 使用Web组件加载H5页面。
2. 使用ArkTS与H5页面进行交互。

![](figures/彩带动效.gif)



