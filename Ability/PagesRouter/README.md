# Ability内页面间的跳转（ArkTS）

## 介绍
本篇Codelab基于Stage模型下的Ability开发，实现Ability内页面间的跳转和数据传递。

最终效果图如下：

![](figures/pageRouter.gif)

### 相关概念

-   [页面路由](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-arkui/apis/js-apis-router.md)：提供通过不同的url访问不同的页面，包括跳转到应用内的指定页面、用应用内的某个页面替换当前页面、返回上一页面或指定的页面等。

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

    ![](figures/systemVersion.png)

2.  搭建烧录环境。
    1.  [完成DevEco Device Tool的安装](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
    2.  [完成RK3568开发板的烧录](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3568-burn.md)

3.  搭建开发环境。
    1.  开始前请参考[工具准备](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#创建ets工程)创建工程（模板选择“Empty Ability”）。
    3.  工程创建完成后，选择使用[真机进行调测](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#使用真机运行应用)。

## 代码结构解读

本篇Codelab只对核心代码进行讲解，完整代码可以直接从gitee获取。

```
├──entry/src/main/ets                // 代码区
│  ├──common
│  │  ├──constants
│  │  │  └──CommonConstants.ets      // 公共常量类
│  │  └──utils
│  │    └──Logger.ets                // 日志类
│  ├──entryability
│  │  └──EntryAbility.ets            // 程序入口类
│  └──pages
│     ├──IndexPage.ets               // 入口页面
│     └──SecondPage.ets              // 跳转页
└──entry/src/main/resources          // 资源文件目录
```


## 页面跳转

1.  在工程pages目录中，选中Index.ets，点击鼠标右键 \> Refactor \> Rename，改名为IndexPage.ets。改名后，修改工程entryability目录下EntryAbility.ets文件中windowStage.loadContent方法第一个参数为pages/IndexPage。

    ```typescript
    // EntryAbility.ets
    onWindowStageCreate(windowStage: Window.WindowStage): void {
      ...
      windowStage.loadContent('pages/IndexPage', (err, data) => {
        ...
      });
    }
    ```

2.  在工程pages目录中，点击鼠标右键 \> New \> Page，新建命名为SecondPage的page页。
3.  从IndexPage页面跳转到SecondPage页面，并进行数据传递，需要如下几个步骤：

    -   给两个页面导入router路由模块。
    -   在IndexPage页面中给Button组件添加点击事件，使用router.pushUrl\(\)方法将SecondPage页面路径添加到url中，params为自定义参数。
    -   SecondPage页面通过router.getParams\(\)方法获取IndexPage页面传递过来的自定义参数。

    IndexPage页面有一个Text文本和Button按钮，点击按钮跳转到下一个页面，并传递数据。IndexPage.ets代码如下：

    ```typescript
    // IndexPage.ets
    import router from '@ohos.router';
    import CommonConstants from '../common/constants/CommonConstants';
    
    @Entry
    @Component
    struct IndexPage {
      @State message: string = CommonConstants.INDEX_MESSAGE;
    
      build() {
        Row() {
          Column() {
            Text(this.message)
              ...
            Blank()
            Button($r('app.string.next'))
              ...
              .onClick(() => {
                router.pushUrl({
                  url: CommonConstants.SECOND_URL,
                  params: {
                    src: CommonConstants.SECOND_SRC_MSG
                  }
               }).catch((error: Error) => {
                 Logger.info(TAG, 'IndexPage push error' + JSON.stringify(error));
               });
            })
          }
          ...
        }
        ...
      }
    }
    ```

    SecondPage页面有两个Text文本，其中一个文本展示从IndexPage页面传递过来的数据。SecondPage.ets代码如下：

    ```typescript
    // SecondPage.ets
    import router from '@ohos.router';
    import CommonConstants from '../common/constants/CommonConstants';
    
    @Entry
    @Component
    struct Second {
      @State message: string = CommonConstants.SECOND_MESSAGE;
      @State src: string = (router.getParams() as Record<string, string>)[CommonConstants.SECOND_SRC_PARAM];
    
      build() {
        Row() {
          Column() {
            Text(this.message)
              ...
            Text(this.src)
              ...
          }
          ...
        }
        ...
      }
    }
    ```
## 页面返回

在SecondPage页面中，Button按钮添加onClick()事件。调用router.back()方法，实现返回上一页面的功能。

```typescript
// SecondPage.ets
Button($r('app.string.back'))
  ...
  .onClick(() => {
    router.back();
  })
```
## 总结

您已经完成了本次Codelab的学习，并了解到以下知识点：

1. 使用页面路由实现应用内页面跳转。

![](figures/summarize.gif)