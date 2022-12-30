# 新闻数据加载（ArkTS）
## 介绍
本篇Codelab是基于ArkTS的声明式开发范式的样例，主要介绍了数据请求和touch事件的使用。包含以下功能：

1.  数据请求。
2.  列表下拉刷新。
3.  列表上拉加载。

最终效果图如下：

![](figures/news.gif)

### 相关概念

-   [List组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-list.md)：列表包含一系列相同宽度的列表项。
-   [Tabs组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-tabs.md)：通过页签进行内容视图切换。
-   [TabContent组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-tabcontent.md)：仅在Tabs中使用，对应一个切换页签的内容视图。
-   [数据请求](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-http.md)：提供发起请求、中断请求、订阅/取消订阅HTTP Response Header 等方法。
-   [触摸事件onTouch](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-universal-events-touch.md)：触摸动作触发调用该方法。

### 相关权限

网络数据请求需要权限：ohos.permission.INTERNET

## 环境搭建

### 软件要求

-   [DevEco Studio](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)版本：DevEco Studio 3.1 Canary1及以上版本。
-   OpenHarmony SDK版本：API version 9及以上版本。

### 硬件要求

-   开发板类型：[润和RK3568开发板](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-appendix-rk3568.md)。
-   OpenHarmony系统：3.2 Beta3及以上版本。

### 环境搭建

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**RK3568**开发板为例，参照以下步骤进行：

1.  [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。以3.2 Beta3版本为例：

    ![](figures/systemVersion.png)

2.  搭建烧录环境。
    1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
    2.  [完成RK3568开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3568-burn.md)

3.  搭建开发环境。
    1.  开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#创建ets工程)创建工程（模板选择“Empty Ability”），选择JS或者eTS语言开发。
    3.  工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#使用真机运行应用)。
    
## 代码结构解读

本篇Codelab只对核心代码进行讲解，完整代码可以直接从gitee获取。

```
├──entry/src/main/ets                   // ArkTS代码区
│  ├──common
│  │  ├──bean
│  │  │  └──NewsData.ets                // 新闻数据实体类
│  │  ├──constants
│  │  │  └──CommonConstants.ets         // 公共常量类
│  │  └──utils
│  │     ├──DataUtils.ets               // 获取新闻数据、类型等方法
│  │     ├──HttpUtil.ets                // 网络请求方法
│  │     ├──PullDownRefresh.ets         // 下拉刷新方法
│  │     └──PullUpLoadMore.ets          // 上拉加载更多方法
│  ├──entryability
│  │  └──EntryAbility.ts                // 程序入口类
│  ├──pages
│  │  └──Index.ets                      // 入口文件
│  └──view
│     ├──CustomRefreshLoadLayout.ets	// 下拉刷新、上拉加载布局文件
│     ├──LoadMoreLayout.ets             // 上拉加载布局封装
│     ├──NewsItem.ets                   // 新闻数据
│     ├──NewsList.ets                   // 新闻列表
│     ├──RefreshLayout.ets              // 下拉刷新布局封装
│     └──TabBar.ets                     // 新闻类型页签
└──entry/src/main/resources             // 资源文件目录
```

## 构建主界面

本章节将介绍新闻列表页面的实现，用tabBar展示新闻分类，tabContent展示新闻列表，效果图如下：

![](figures/mainPage.png)

在TabBar.ets文件中的aboutToAppear()方法里获取新闻分类。示例代码如下：

```typescript
aboutToAppear() {
  // 新闻类别
  loadNewsTypes({
    onSuccess: (value) => this.tabBarArray = value,
    onFail: () => this.tabBarArray = TabBars.DEFAULT_NEWS_TYPES
  });
}
```

在NewsList.ets文件中的aboutToAppear()方法里获取新闻数据，将数据加载到新闻列表页面ListLayout布局中。示例代码如下：

```typescript
// NewsList.ets
aboutToAppear() {
  // 请求新闻数据
  getTabData(this.currentPage - 1, this.pageSize, this.currentIndex, {
    onSuccess: (value) => {
      this.pageState = PageState.Success;
      this.newsData = value.newsPageData;
    },
    onFail: () => this.pageState = PageState.Fail
  });
}
...
@Builder ListLayout() {
  List() {
    // 下拉刷新布局
    ListItem() {
      RefreshLayout({
        refreshLayoutClass: new CustomRefreshLoadLayoutClass(this.isVisiblePullDown, this.pullDownRefreshImage, this.pullDownRefreshText, 
        this. pullDownRefreshHeight)
      });
    }
    ForEach(this.newsData, item => {
      // 新闻数据
      ListItem() {
        NewsItem({ newsData: item });
      }
      ...
    }, item => item.id)
    // 上拉加载布局
    ...
  }
}
```

## 数据请求

这一章节，将基于新闻数据请求（getTabData方法）来介绍如何从服务端请求数据。

在module.json5文件中配置如下权限：

```typescript
"requestPermissions": [
  {
    "name": "ohos.permission.INTERNET"
  }
]
```

导入http模块，封装httpRequestGet方法，调用者传入url地址发起网络数据请求。示例代码如下：

```typescript
import http from '@ohos.net.http';
...
export function httpRequestGet(url: string, params: Array<any>) {
  ...
  let httpRequest = http.createHttp();
  // 发送数据请求
  let responseResult = httpRequest.request(url, {
    method: http.RequestMethod.GET,
    readTimeout: HTTP_READ_TIMEOUT,
    connectTimeout: HTTP_READ_TIMEOUT
  });
  let serverData: {
    code,
    data: any,
    msg: string | Resource
  } = { code: 0, data: '', msg: '' };
  // 处理数据，并返回
  return responseResult.then((value) => {
    if (value.responseCode === HTTP_CODE_200) {
      // 获取返回数据
      let result = `${value.result}`;
      let resultJson = JSON.parse(result);
      if (resultJson.code === SERVER_CODE_SUCCESS) {
        serverData.data = resultJson.data;
      }
      serverData.code = resultJson.code;
      serverData.msg = resultJson.msg;
    } else {
      serverData.code = value.responseCode;
      serverData.msg = `${$r('app.string.http_error_message')}&${value.responseCode}`;
    }
    return serverData;
  }).catch(() => {
    serverData.msg = $r('app.string.http_error_message')
    return serverData;
  })
}
```

在DataUtils.ets文件中封装getTabData方法，调用httpRequestGet方法统一发起网络数据请求，callback回调方法处理请求到的数据。示例代码如下：

```typescript
// DataUtils.ets
export async function getTabData(currentPage: number, pageSize: number, currentIndex: number, callback) {
  ...
  let data = await httpRequestGet(url, params);
  if (data.code === SERVER_CODE_SUCCESS) {
    newsPageData = data.data.pageData;
    callback.onSuccess({ currentPage, newsPageData });
  } else {
    callback.onFail(data.msg);
  }
}
```

## 下拉刷新

本章节将以下拉刷新的功能效果来介绍touch事件的使用。效果图如下：

![](figures/pullDownRefresh.png)

创建一个下拉刷新布局CustomLayout，动态传入刷新图片和刷新文字描述。示例代码如下：

```typescript
@Component
export default struct CustomLayout {
  Row() {
    // 下拉刷新图片
    Image(this.customClass.imageSrc)
      ...
    // 下拉刷新文字
    Text(this.customClass.textValue)
      ...
  }
  ...
}
```

将下拉刷新的布局添加到NewsList.ets文件中新闻列表布局ListLayout里面，监听ListLayout组件的onTouch事件实现下拉刷新。示例代码如下：

```typescript
build() {
  Column() {
  ...
  this.ListLayout();
  ...
  }
  ...
  .onTouch((event) => {
    if (this.pageState === PageState.Success) {
      listTouchEvent.call(this, event);
    }
  })
}
...
@Builder ListLayout() {
  List() {
    // 下拉刷新布局
    ListItem() {
      RefreshLayout({
        refreshLayoutClass: new CustomRefreshLoadLayoutClass(this.isVisiblePullDown, 
        this.pullDownRefreshImage, this.pullDownRefreshText, this.pullDownRefreshHeight)
      });
      ...
    }
  }
  ...
}
```

在onTouch事件中，listTouchEvent方法判断触摸事件是否满足下拉条件。示例代码如下：

```typescript
export function listTouchEvent(event: TouchEvent) {
  switch (event.type) {
    ...
    case TouchType.Move:
      let isDownPull = event.touches[0].y - this.lastMoveY > 0
      if ((isDownPull || this.isPullRefreshOperation) && !this.isCanLoadMore) {
         // 手指移动，处理下拉刷新
         touchMovePullRefresh.call(this,event);
      }
      ...
      break;
  }
}
```

在touchMovePullRefresh方法中，我们将对下拉的偏移量与下拉刷新布局的高度进行对比，如果大于布局高度并且在新闻列表的顶部，则表示达到刷新条件。

示例代码如下：

```typescript
export function touchMovePullRefresh(event: TouchEvent) {
  if (this.startIndex === 0) {
    this.isPullRefreshOperation = true;
    let height = vp2px(this.pullDownRefreshHeight);
    this.offsetY = event.touches[0].y - this.downY;
    // 滑动的偏移量大于下拉刷新布局高度，达到刷新条件
    if (this.offsetY >= height) {
      pullRefreshState.call(this, RefreshState.Release);
      ...
    } else {
      pullRefreshState.call(this, RefreshState.DropDown);
    }
  ...
  }
}
```

在pullRefreshState方法中我们会对下拉刷新布局中的状态图片和描述进行改变，此处不做详细介绍。

当手指松开，才执行刷新操作。示例代码如下：

```typescript
export function pullRefreshState(state: number) {
  switch (state) {
    ...
    case TouchType.Up:
      if (this.isRefreshing || this.isLoading) {
        return;
      }
      if (this.isPullRefreshOperation) {
        // 手指抬起，处理下拉刷新
        touchUpPullRefresh.call(this);
      } 
      ...
      break;
    ...
  }
}
```

上拉加载也是通过touch事件来实现的，此处不再赘叙，有兴趣的同学可参考代码。

## 总结

您已经完成了本次Codelab的学习，并了解到以下知识点：

1.  使用List组件实现数据列表。
2.  使用Tabs、TabContent组件实现内容视图切换。
3.  网络数据请求。
4.  触摸事件onTouch的用法。

![](figures/summarize.gif)