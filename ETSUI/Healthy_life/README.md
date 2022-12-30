# 健康生活（ArkTS）

## 介绍

本篇Codelab介绍了如何实现一个简单的健康生活应用，主要功能包括：

1. 用户可以创建最多6个健康生活任务（早起，喝水，吃苹果，每日微笑，刷牙，早睡），并设置任务目标、是否开启提醒、提醒时间、每周任务频率。
2. 用户可以在主页面对设置的健康生活任务进行打卡，其中早起、每日微笑、刷牙和早睡只需打卡一次即可完成任务，喝水、吃苹果需要根据任务目标量多次打卡完成。

3. 主页可显示当天的健康生活任务完成进度，当天所有任务都打卡完成后，进度为100%，并且用户的连续打卡天数加一。

4. 当用户连续打卡天数达到3、7、30、50、73、99天时，可以获得相应的成就。成就在获得时会以动画形式弹出，并可以在“成就”页面查看。

5. 用户可以查看以前的健康生活任务完成情况。

本应用的运行效果如下图所示：

![](figures/健康生活.gif)

### 相关概念

- [@Observed 和 @ObjectLink](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/arkts-state-mgmt-page-level.md#observed%E5%92%8Cobjectlink%E6%95%B0%E6%8D%AE%E7%AE%A1%E7%90%86)：@Observed适用于类，表示类中的数据变化由UI页面管理；@ObjectLink应用于被@Observed装饰类的对象。

- [@Consume 和 @Provide](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/arkts-state-mgmt-page-level.md#consume%E5%92%8Cprovide)：@Provide作为数据提供者，可以更新子节点的数据，触发页面渲染。@Consume检测到@Provide数据更新后，会发起当前视图的重新渲染。

- [Flex](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-flex.md)：一个功能强大的容器组件，支持横向布局，竖向布局，子组件均分和流式换行布局。

- [List](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-list.md)：List是很常用的滚动类容器组件之一，它按照水平或者竖直方向线性排列子组件， List的子组件必须是ListItem，它的宽度默认充满List的宽度。

- [TimePicker](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-basic-components-timepicker.md)：TimePicker是选择时间的滑动选择器组件，默认以00:00至23:59的时间区创建滑动选择器。

- [Toggle](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-basic-components-toggle.md)：组件提供勾选框样式、状态按钮样式及开关样式。

- [后台代理提醒](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-reminderAgentManager.md)：使用后台代理提醒能力后，应用可以被冻结或退出，计时和弹出提醒的功能将被后台系统服务代理。

- [关系型数据库（Relational Database，RDB）](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-data-rdb.md)：一种基于关系模型来管理数据的数据库。

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

    ![](figures/zh-cn_image_0000001405854998.png)

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
├─entry/src/main/ets                 // 代码区
│  ├─common
│  │  ├─bean
│  │  │  ├─DayInfo.ets               // 每日信息接口
│  │  │  ├─GlobalInfo.ets            // 全局信息接口
│  │  │  └─TaskInfo.ets              // 任务信息接口
│  │  ├─constants
│  │  │  └─CommonConstants.ets       // 公共常量
│  │  ├─database
│  │  │  ├─tables                    // 数据表
│  │  │  │  ├─DayInfoTable.ets
│  │  │  │  ├─GlobalInfoTable.ets
│  │  │  │  └─TaskInfoTable.ets
│  │  │  └─Rdb.ets                   // RDB数据库
│  │  ├─reminderagent
│  │  │  └─ReminderAgent.ets         // 后台代理提醒接口
│  │  └─utils
│  │     ├─log                       // 日志类
│  │     │  ├─ConsoleLogger.ets
│  │     │  ├─HiLogger.ets
│  │     │  ├─ILogger.ets
│  │     │  └─Logger.ets
│  │     ├─BroadCast.ets             // 通知
│  │     ├─HealthDataSrcMgr.ets      // 数据管理单例
│  │     └─Utils.ets                 // 工具类
│  ├─entryability
│  │  └─EntryAbility.ts              // 程序入口类
│  ├─model                           // model
│  │  ├─AchieveModel.ets
│  │  ├─DatabaseModel.ets            // 数据库model
│  │  ├─Mine.ets
│  │  ├─NavItemModel.ets             // 菜单栏model
│  │  ├─TaskInitList.ets
│  │  └─WeekCalendarModel.ets        // 日历model
│  ├─pages
│  │  ├─AdvertisingPage.ets          // 广告页
│  │  ├─MainPage.ets                 // 应用主页面
│  │  ├─MinePage.ets                 // 我的页面
│  │  ├─SplashPage.ets               // 启动页
│  │  ├─TaskEditPage.ets             // 任务编辑页面
│  │  └─TaskListPage.ets             // 任务列表页面
│  ├─view
│  │  ├─dialog                       // 弹窗组件
│  │  │  ├─AchievementDialog.ets     // 成就弹窗
│  │  │  ├─CustomDialogView.ets      // 自定义弹窗
│  │  │  ├─TaskDetailDialog.ets      // 打卡弹窗
│  │  │  ├─TaskSettingDialog.ets     // 任务编辑相关弹窗
│  │  │  └─UserPrivacyDialog.ets
│  │  ├─home                         // 主页面相关组件
│  │  │  ├─AddBtnComponent.ets       // 添加任务按钮组件
│  │  │  ├─HomeTopComponent.ets      // 首页顶部组件
│  │  │  ├─TaskCardComponent.ets     // 任务item组件件
│  │  │  └─WeekCalendarComponent.ets // 日历组件
│  │  ├─task                         // 任务相关组件
│  │  │  ├─TaskDetailComponent.ets   // 任务编辑详情组件
│  │  │  ├─TaskEditListItem.ets      // 任务编辑行内容
│  │  │  └─TaskListComponent.ets     // 任务列表组件
│  │  ├─AchievementComponent.ets     // 成就页面
│  │  ├─BadgeCardComponent.ets       // 勋章卡片组件
│  │  ├─BadgePanelComponent.ets      // 勋章面板组件
│  │  ├─HealthTextComponent.ets      // 自定义text组件
│  │  ├─HomeComponent.ets            // 首页页面
│  │  ├─ListInfo.ets                 // 用户信息列表
│  │  ├─TitleBarComponent.ets        // 成就标题组件
│  │  └─UserBaseInfo.ets             // 用户基本信息
│  └─viewmodel                       // viewmodel
│     ├─AchievementViewModel.ets     // 成就相关模块
│     ├─CalendarViewModel.ets        // 日历相关模块
│     ├─HomeViewModel.ets            // 首页相关模块
│     └─TaskViewModel.ets            // 任务设置相关模块
└─resources                          // 资源文件夹
```

## 应用架构分析

本应用的基本架构如下图所示，数据库为其他服务提供基础的用户数据，主要业务包括：用户可以查看和编辑自己的健康任务并进行打卡、查看成就。UI层提供了承载上述业务的UI界面。

![](figures/zh-cn_image_0000001459627721.png)

## 应用主页面

本节将介绍如何给应用添加一个启动页，设计应用的主界面，以及首页的界面开发和数据展示。

### 启动页

首先我们需要给应用添加一个启动页，启动页里我们需要用到一个定时器来实现启动页展示固定时间后跳转应用主页的功能，效果图如下：

![](figures/权限.gif)

打开应用时会进入此页面，具体实现逻辑是：

通过修改/entry/src/main/ets/entryability里的loadContent路径可以改变应用的入口文件，我们需要把入口文件改为我们写的SplashPage启动页面。

```typescript
windowStage.loadContent("pages/SplashPage", (err, data) => {    
  if (err.code) {...}    
  Logger.info('windowStage','Succeeded in loading the content. Data: ' + JSON.stringify(data))
});
```

在SplashPage启动页的文件里通过首选项来实现是否需要弹“权限管理”的弹窗，如果需要弹窗的情况下，用户点击同意权限后通过首选项对用户的操作做持久化保存。相关代码如下：

```typescript
// SplashIndex.ets
import data_preferences from '@ohos.data.preferences';

onConfirm() {
  let preferences = data_preferences.getPreferences(globalThis.abilityContext, H_STORE);
  preferences.then((res) => {
    res.put(IS_PRIVACY, true).then(() => {
      res.flush();
      Logger.info('TaskInfoTable',‘isPrivacy is put success’);
    }).catch((err) => {
      Logger.info('TaskInfoTable', ‘isPrivacy put failed. Cause:’ + err);
    });
  })
  this.jumpAdPage();
}

exitApp() {
  globalThis.abilityContext.terminateSelf();
}

jumpAdPage() {
  setTimeout(() => {
    router.replace({ url: 'pages/AdvertisingPage' });
  }, commonConst.LAUNCHER_DELAY_TIME);
}

aboutToAppear() {
  let preferences = data_preferences.getPreferences(globalThis.abilityContext, H_STORE);
  preferences.then((res) => {
    res.get(IS_PRIVACY, false).then((isPrivate) => {
      if (isPrivate === true) {
        this.jumpAdPage();
      } else {
        this.dialogController.open();
      }
    });
  });
}
```

### APP功能入口

我们需要给APP添加底部菜单栏，用于切换不同的应用模块，由于各个模块之间属于完全独立的情况，并且不需要每次切换都进行界面的刷新，所以我们用到了Tabs，TabContent组件。

![](figures/主页面.gif)

本应用一共有首页（HomeIndex），成就（AchievementIndex）和我的（MineIndex）三个模块，分别对应Tabs组件的三个子组件TabContent。

```typescript
Tabs({ barPosition: BarPosition.End, controller: this.tabController }) {  
  TabContent() {    
    HomeIndex({ editedTaskInfo: $editedTaskInfo, editedTaskID: $editedTaskID })  
  }
  .tabBar(this.TabBuilder(TabId.HOME)) 
  TabContent() {    
    AchievementIndex()  
  }
  .tabBar(this.TabBuilder(TabId.ACHIEVEMENT))  
  TabContent() {    
    MineIndex()  
  }
  .tabBar(this.TabBuilder(TabId.MINE))
}
```

### 首页

首页包含了任务信息的所有入口，包含任务列表的展示，任务的编辑和新增，上下滚动的过程中顶部导航栏的渐变，日期的切换以及随着日期切换界面任务列表跟着同步的功能，效果图如下：

![](figures/首页.gif)

具体代码实现我们将在下边分模块进行说明：

1. 导航栏背景渐变

   Scroll滚动的过程中，在它的onScroll方法里我们通过计算它Y轴的偏移量来改变当前界面的@State修饰的naviAlpha变量值，进而改变顶部标题的背景色，代码实现如下：

   ```typescript
   // 视图滚动的过程中处理导航栏的透明度
   onScrollAction() {  
     let yOffset = this.scroller.currentOffset().yOffset;  
     if (yOffset > commonConst.DEFAULT_56) {    
       this.naviAlpha = 1;  
     } else {    
       this.naviAlpha = yOffset / commonConst.DEFAULT_56;
     }
   }
   ```


2. 日历组件

   日历组件主要用到的是一个横向滑动的Scroll组件。

   ```typescript
   build() {    
     Row() {      
       Column() {        
         Row() {...}             
         Scroll(this.scroller) {          
           Row() {            
             ForEach(this.homeStore.dateArr, (item: WeekDateModel, index: number) => {              
               Column() {                
                 Text(item.weekTitle)                  
                   .fontColor(sameDate(item.date, this.homeStore.showDate) ? $r('app.color.blueColor') : $r('app.color.titleColor'))                                 
                 Divider()
                   .color(sameDate(item.date, this.homeStore.showDate) ? $r('app.color.blueColor') : $r('app.color.white'))                
                 Image(this.getProgressImg(item))                               
               } 
               .onClick(() => WeekCalendarMethods.calenderItemClickAction.call(this, item, index))            
             })          
            }       
           }               
           .onScrollEnd(() => WeekCalendarMethods.onScrollEndAction.call(this))        
           .onScrollEdge(() => WeekCalendarMethods.onScrollEdgeAction.call(this))      
         }...       
       }...    
     }
   }
   ```

   手动滑动页面时，我们通过在onScrollEnd方法里计算Scroll的偏移量来实现分页的效果，同时Scroll有提供scrollPage\(\)方法可供我们点击左右按钮的时候来进行页面切换。

   ```typescript
   import display from '@ohos.display';
   ...
   // scroll滚动停止时通过判断偏移量进行分页处理
   function onScrollEndAction() {  
     let scrollWidth = DEFAULT_SCROLL_WIDTH;  
   
     // 获取屏幕的宽度  
     display.getDefaultDisplay((err, data) => {    
       if (!err) {      
         scrollWidth = data.width * DEFAULT_SCROLL_PERCENT;    
       } else {...}  
     })  
   
     // 区分是否是手动滑动，点击左右箭头按钮导致Scroll滑动时不作处理，不然会引起死循环
     if (!this.isPageScroll) {    
       let page = Math.round(this.scroller.currentOffset().xOffset / scrollWidth);   
       page = this.isLoadMore ? page + 1 : page;    
       if (this.scroller.currentOffset().xOffset % scrollWidth != 0 || this.isLoadMore) {      
         let xOffset = page * scrollWidth;   
      
         // 滑动到指定位置
         this.scroller.scrollTo({ xOffset, yOffset: 0 });      
         this.isLoadMore = false;    
       }  
   
       // 处理当前界面展示的数据  
       ...
     }  
     this.isPageScroll = false;
   }
   ```

   我们在需要在Scroll滑动到左边边缘的时候去请求更多的历史数据以便Scroll能一直滑动，通过Scroll的onScrollEdge方法我们可以判断它是否已滑到边缘位置。

   ```typescript
   function onScrollEdgeAction(side: Edge) {
     if (side == Edge.Top && !this.isPageScroll) {
       Logger.info('HomeIndex', 'onScrollEdge: currentPage ' + this.currentPage);
       if ((this.currentPage + LAZY_DATA_PAGE) * WEEK_DAY_NUM >= this.homeStore.dateArr.length) {
         Logger.info('HomeIndex', 'onScrollEdge: load more data');
         let date: Date = new Date(this.homeStore.showDate);
         date.setDate(date.getDate() - WEEK_DAY_NUM);
         this.homeStore.getPreWeekData(date);
         this.isLoadMore = true;
       }
     }
   }
   ```

   homeStore主要是请求数据库的数据并对数据进行处理进而渲染到界面上。

   ```typescript
   public getPreWeekData(date: Date) {
     let [initArr, dateArr] = getPreviousWeek(date);
       
     // 请求数据库数据
     this.dayInfoApi.queryList(dateArr, (res: DayInfo[]) => {
   
       // 数据处理
       ...  
       this.dateArr = initArr.concat(...this.dateArr);
     })
   }
   ```

   同时我们还需要知道怎么根据当天的日期计算出本周内的所有日期数据。

   ```typescript
   export function getPreviousWeek(showDate: Date): [Array<WeekDateModel>, Array<string>] {
     let arr: Array<WeekDateModel> = [];
     let strArr: Array<string> = []; 
   
     // 由于date的getDay()方法返回的是0-6代表周日到周六，我们界面上展示的周一-周日为一周，所以这里要将getDay()数据偏移一天
     let currentDay = showDate.getDay() - 1;
     if (showDate.getDay() == 0) {
       currentDay = 6;
     }
   
     // 将日期设置为当前周第一天的数据（周一）
     showDate.setDate(showDate.getDate() - currentDay);
     for (let index = WEEK_DAY_NUM; index > 0; index--) {
       let tempDate = new Date(showDate);
       tempDate.setDate(showDate.getDate() - index);
       let dateStr = dateToStr(tempDate);
       strArr.push(dateStr);
       arr.push(new WeekDateModel(WEEK_TITLES[tempDate.getDay()], dateStr, tempDate));
     }
     return [arr, strArr];
   }
   ```


3. 悬浮按钮

   由于首页右下角有一个悬浮按钮，所以首页整体我们用了一个Stack组件，将右下角的悬浮按钮和顶部的title放在滚动组件层的上边。

   ```typescript
   build() {  
     Stack() {    
       Scroll(this.scroller) {      
         Column() {     
           ...   // 上部界面组件
           Column() {          
             ForEach(TaskList, (item: TaskInfo) => {            
               TaskCard(...,
                 clickAction: (isClick) => this.taskItemAction(item, isClick)) 
             }, item => item.date + item.taskID)} 
           }   
         }
       }
       .onScroll(this.onScrollAction.bind(this))
        // 悬浮按钮
        AddBtn()   
        // 顶部title 
        Row() {       
          Text($r('app.string.MainAbility_label'))
        }
        .position({ x: 0, y: 0 })    
        .backgroundColor(`rgba(${WHITE_COLOR_0X},${WHITE_COLOR_0X},${WHITE_COLOR_0X},${this.naviAlpha})`)    
        CustomDialogView()  
     }  
     .allSize() 
     .backgroundColor($r('app.color.primaryBgColor'))
   }
   ```


4. 界面跳转及传参

   首页任务列表长按时需要跳转到对应的任务编辑界面，同时点击悬浮按钮时需要跳转到任务列表页面。

   页面跳转需要在头部引入router。

   ```typescript
   import router from '@ohos.router';
   ```

   任务item的点击事件代码如下

   ```typescript
   taskItemAction(item: TaskInfo, isClick: boolean) {  
     if (isClick) {  
       // 点击任务打卡    
       let callback: CustomDialogCallback = { confirmCallback: this.onConfirm.bind(this), cancelCallback: null };    
       this.broadCast.emit(BroadCastType.SHOW_TASK_DETAIL_DIALOG, [item, callback]);  
     } else {   
       // 长按编辑任务    
       const editTask: ITaskItem = {...};   
       router.push({ url: 'pages/task/TaskEdit', params: { params: JSON.stringify(editTask) } });  
     }
   }
   ```


## 任务创建与编辑

本节将介绍如何创建和编辑健康生活任务。

### 功能概述

用户点击悬浮按钮进入任务列表页，点击任务列表可进入对应任务编辑的页面中，对任务进行详细的设置，之后点击完成按钮编辑任务后将返回首页。实现效果如下图：

![](figures/任务.gif)


### 任务列表与编辑任务

这里主要为大家介绍添加任务列表页的实现、任务编辑的实现、以及具体弹窗设置和编辑完成功能的逻辑实现。

#### 任务列表页

任务列表页由包括上部分的标题、返回按钮以及正中间的任务列表组成。实现效果如图：

![](figures/任务列表---副本.png)

使用Navigation以及List组件构成元素，ForEach遍历生成具体列表。这里是Navigation构成页面导航：

```typescript
// TaskListPage.ets
Navigation() {
  Column() {
    // 页面中间的列表
    TaskList() 
  }
  .width(THOUSANDTH_1000)
  .justifyContent(FlexAlign.Center)
}
.size({ width: THOUSANDTH_1000, height: THOUSANDTH_1000 })
.title(ADD_TASK_TITLE)
```

列表右侧有一个判断是否开启的文字标识，点击某个列表需要跳转到对应的任务编辑页里。具体的列表实现如下：

```typescript
// TaskListComponent.ets
List({ space: commonConst.LIST_ITEM_SPACE }) {
  ForEach(this.taskList, (item) => {
    ListItem() {
      Row() {
        Row() {
          Image(item?.icon)
          Text(item?.taskName)
            ...
        }
        .width(commonConst.THOUSANDTH_500)

        // 状态显示
        if (item?.isOpen) {
          Text($r('app.string.already_open'))
        }
        Image($rawfile('task/right_grey.png'))
          .width(commonConst.DEFAULT_8)
          .height(commonConst.DEFAULT_16)
      }
      ...
    }
    ...

    // 路由跳转到任务编辑页
    .onClick(() => {
      router.push({
        url: 'pages/task/TaskEdit',
        params: {
          params: formatParams(item),
        }
      })
    })
    ...
  })
}
```

#### 任务编辑页

任务编辑页由上方的“编辑任务”标题以及返回按钮，主体内容的List配置项和下方的完成按钮组成，实现效果如图：

![](figures/任务编辑---副本.png)

由于每一个配置项功能不相同，且逻辑复杂，故将其拆分为五个独立的组件。

这是任务编辑页面，由Navigation和一个自定义组件TaskDetail构成：

```typescript
// TaskEditPage.ets
Navigation() {
  Column() {
    TaskDetail()
  }
  .width(THOUSANDTH_1000)
  .height(THOUSANDTH_1000)
}
.size({ width: THOUSANDTH_1000, height: THOUSANDTH_1000 })
.title(EDIT_TASK_TITLE)
```

自定义组件由List以及其子组件ListItem构成：

```typescript
// TaskDetailComponent.ets
List({ space: commonConst.LIST_ITEM_SPACE }) {
  ListItem() {
    TaskChooseItem()
  }
  ...
  ListItem() {
    TargetSetItem()
  }
  ...
  ListItem() {
    OpenRemindItem()
  }
  ...
  ListItem() {
    RemindTimeItem()
  }
  ...
  ListItem() {
    FrequencyItem()
  }
  ...
}
.width(commonConst.THOUSANDTH_940)
.margin({ bottom: commonConst.THOUSANDTH_400 })
```

其中做了禁用判断，需要任务打开才可以点击编辑：

```typescript
.enabled(this.settingParams?.isOpen)
```

一些特殊情况的禁用，如每日微笑、每日刷牙的目标设置不可编辑：

```typescript
.enabled(
  this.settingParams?.isOpen
  && (this.settingParams?.taskID !== taskType.smile)
  && (this.settingParams?.taskID !== taskType.brushTeeth)
)
```

提醒时间在开启提醒打开之后才可以编辑：

```typescript
.enabled(this.settingParams?.isOpen && this.settingParams?.isAlarm)
```

设置完成之后，点击完成按钮，会向数据库更新现在进行改变的状态信息，并执行之后的逻辑判断：

```typescript
// TaskDetailComponent.ets
addTask({

  // 相关参数
  ...
})
.then(res => {

  // 成功的状态，成功后跳转首页
  router.back({
    url: 'pages/index',
    params: {
      editTask: this.isOpen === this.settingParams.isOpen 
                ? {} 
                : formatParams({
                    ...this.settingParams,
                    isDone: true,
                    finValue: this.settingParams?.targetValue,
                  }),
    }
  })
  Logger.info('addTaskFinshed', resasstring);
})
.catch(res => {

  // 失败的状态，失败后弹出提示，并打印错误日志
  prompt.showToast({
    message: commonConst.SETTING_FINISH_FAILED_MESSAGE
  })
  Logger.error('addTaskFailed', res as string);
})
```

#### 任务编辑弹窗

弹窗由封装的自定义组件CustomDialogView注册事件，并在点击对应的编辑项时进行触发，从而打开弹窗。

CustomDialogView引入实例并注册事件：

```typescript
// TaskSettingDialog.ets
targetSettingDialog = new CustomDialogController({
  builder: TargetSettingDialog(),
  autoCancel: true,
  alignment: DialogAlignment.Bottom,
  offset: { dx: ZERO, dy: MINUS_20 }
})
...

// 注册事件
this.broadCast.on(
  BroadCastType.SHOW_TARGETSETTING_DIALOG,
  function () {
    self.targetSettingDialog.open();
  })
```

点击对应的编辑项进行触发：

```typescript
.onClick(() => {
  this.broadCast.emit(BroadCastType.SHOW_TARGETSETTING_DIALOG);
})
```

自定义弹窗的实现：

任务目标设置的弹窗较为特殊，故单独拿出来说明。

因为任务目标设置有三种类型：

-   早睡早起的时间
-   喝水的量度
-   吃苹果的个数

如下图所示：

![](figures/弹窗---副本.png)

故根据任务的ID进行区分，将同一弹窗复用：

```typescript
// TaskSettingDialog.ets
if ([taskType.getup, taskType.sleepEarly].indexOf(this.settingParams?.taskID) 
    > commonConst.HAS_NO_INDEX) {
  TimePicker({
    selected: commonConst.DEFAULT_SELECTED_TIME,
  })
  ...
} else {
  TextPicker({ range: this.settingParams?.taskID === taskType.drinkWater 
    ? this.drinkRange 
    : this.appleRange })
  ...
}
```

弹窗确认的时候将修改好的值赋予该项设置，如不符合规则，将弹出提示：

```typescript
// TaskSettingDialog.ets

// 校验规则
compareTime(startTime: string, endTime: string) {
  if (returnTimeStamp(this.currentTime) < returnTimeStamp(startTime) 
      || returnTimeStamp(this.currentTime) > returnTimeStamp(endTime)) {

    // 弹出提示
    prompt.showToast({
      message: commonConst.CHOOSE_TIME_OUT_RANGE
    })
    return false;
  }
  return true;
}

// 设置修改项
if (this.settingParams?.taskID === taskType.sleepEarly) {
  if (!this.compareTime(commonConst.SLEEP_EARLY_TIME, commonConst.SLEEP_LATE_TIME)) {
    return;
  }
  this.settingParams.targetValue = this.currentTime;
  return;
}
this.settingParams.targetValue = this.currentValue;
```

其余弹窗实现基本类似，这里不再赘述。

## 后台代理提醒

健康生活App中提供了任务提醒功能，我们用系统提供的后台代理提醒reminderAgent接口完成相关的开发。

>![](public_sys-resources/icon-note.gif) **说明：** 
>后台代理提醒接口需要在module.json5中申请ohos.permission.PUBLISH\_AGENT\_REMINDER权限，代码如下：

```typescript
"requestPermissions": [
  {
   "name": "ohos.permission.PUBLISH_AGENT_REMINDER"
  }
]
```

后台代理提醒entry\\src\\main\\ets\\service\\ReminderAgent.ts文件中提供了发布提醒任务、查询提醒任务、删除提醒任务三个接口供任务编辑页面调用，跟随任务提醒的开关增加、更改、删除相关后台代理提醒，代码如下：

```typescript
import reminderAgentManager from'@ohos.reminderAgentManager';
import Notification from '@ohos.notification';
import { Logger } from '../utils/log/Logger';

// publishReminder
function publishReminder(params) {
    if(!params) return;
    let timer = fetchData(params);
    reminderAgentManager.publishReminder(timer).then((reminderId) => {
        Logger.info("reminderAgent", `promise, reminderId: ${reminderId}`);
    }).catch((err) => {
        Logger.error("reminderAgent", `publishReminder err: ${err}`);
    })
}

// cancelReminder
function cancelReminder(reminderId) {
    if(!reminderId) return;
    reminderAgentManager.cancelReminder(reminderId).then((data) => {
        Logger.info("reminderAgent", `cancelReminder promise: ${data}`);
    }).catch((err) => {
        Logger.error("reminderAgent", `cancelReminder err: ${err}`);
    })
}

// fetchData
function fetchData(params): reminderAgentManager.ReminderRequestAlarm {
    return {
        reminderType: reminderAgentManager.ReminderType.REMINDER_TYPE_ALARM,
        hour: params.hour || 0,
        minute: params.minute || 0,
        daysOfWeek: params.daysOfWeek || [],
        wantAgent: {
            pkgName: "com.example.exercisehealth",
            abilityName: "MainAbility"
        },
        title: params.title || '',
        content: params.content || '',
        notificationId: params.notificationId || -1,
        slotType: Notification.SlotType.SOCIAL_COMMUNICATION
    }
}

// hasNotificationId
async function hasNotificationId(params: number) {
    if(!params) return;
    await reminderAgentManager.getValidReminders().then((reminders) => {
        if (!reminders.length) {
            return false;
        }
        let notificationIdList = [];
        for (let i = 0; i < reminders.length; i++) {
            notificationIdList.push(reminders[i].notificationId)
        }
        const flag = notificationIdList.indexOf(params);

        return flag === -1 ? false : true;
    })
}

const reminder = {
    publishReminder,
    cancelReminder,
    hasNotificationId
}

export default reminder
```

## 实现打卡功能

首页会展示当前用户已经开启的任务列表，每条任务会显示对应的任务名称以及任务目标、当前任务完成情况。用户只可对当天任务进行打卡操作，用户可以根据需要对任务列表中相应的任务进行点击打卡。如果任务列表中的每个任务都在当天完成则为连续打卡一天，连续打卡多天会获得成就徽章。打卡效果如下图所示：

![](figures/打卡.gif)

### 任务列表

使用List组件展示用户当前已经开启的任务，每条任务对应一个TaskCard组件，clickAction包装了点击和长按事件，用户点击任务卡时会触发弹起打卡弹窗，从而进行打卡操作；长按任务卡时会跳转至任务编辑界面，对相应的任务进行编辑处理。代码如下：

```typescript
// 任务列表
List({ space: 12 }) {
  ForEach(this.homeStore.getTaskListOfDay(), (item: TaskInfo) => {
    ListItem() {
      TaskCard({
        taskInfoStr: JSON.stringify(item),
        clickAction: (isClick) => this.taskItemAction(item, isClick)
      })
    }
  }, item => item.date + item.taskID)
}
...
CustomDialogView() // 自定义弹窗中间件
```

#### 自定义弹窗中间件CustomDialogView

在组件CustomDialogView的aboutToAppear生命周期中注册SHOW\_TASK\_DETAIL\_DIALOG的事件回调方法 ，当通过emit触发此事件时即触发回调方法执行。代码如下：

```typescript
@Component
export struct CustomDialogView {
  @Consume broadCast: BroadCast;
  @Provide currentTask: TaskInfo = TaskItem;
  @Provide dialogCallBack: CustomDialogCallback = { confirmCallback: null, cancelCallback: null };
  // 任务打卡弹窗
  taskDialog = new CustomDialogController({
    builder: TaskDetailDialog(),
    autoCancel: true,
    customStyle: true
  })
  aboutToAppear() {
    Logger.debug('CustomDialogView', 'aboutToAppear')
    let self = this;
    // 任务打卡弹窗  注册 “SHOW_TASK_DETAIL_DIALOG” 事件回调
    this.broadCast.on(BroadCastType.SHOW_TASK_DETAIL_DIALOG, function (currentTask: TaskInfo, dialogCallBack: CustomDialogCallback) {
      Logger.debug('CustomDialogView', 'SHOW_TASK_DETAIL_DIALOG')
      self.currentTask = currentTask; // 接收当前任务参数 以Provide Consume 方式向子组件透传
      self.dialogCallBack = dialogCallBack;  // 接收当前任务确认打卡回调 以Provide Consume 方式向子组件透传
      self.taskDialog.open(); // 弹出打卡弹窗
    })
  }
  ...
}
```

#### 点击任务卡片

点击任务卡片会emit触发 “SHOW\_TASK\_DETAIL\_DIALOG” 事件，同时把当前任务，以及确认打卡回调方法传递下去。代码如下：

```typescript
// 任务卡片事件
taskItemAction(item: TaskInfo, isClick: boolean) {
  ...
  if (isClick) {
    // 点击任务打卡
    let callback: CustomDialogCallback = { confirmCallback: this.onConfirm.bind(this), cancelCallback: null };
    // 触发弹出打卡弹窗事件  并透传当前任务参数（item） 以及确认打卡回调
    this.broadCast.emit(BroadCastType.SHOW_TASK_DETAIL_DIALOG, [item, callback]);
  } else {
    // 长按编辑任务
    ...
  }
}
// 确认打卡
onConfirm(task) {
  this.homeStore.taskClock(task).then((res) => {
    // 打卡成功后 根据连续打卡情况判断是否 弹出成就勋章  以及成就勋章级别
    if (res.showAchievement) {
      // 触发弹出成就勋章SHOW_ACHIEVEMENT_DIALOG 事件， 并透传勋章类型级别
      this.broadCast.emit(BroadCastType.SHOW_ACHIEVEMENT_DIALOG, [res.achievementLevel]);
    }
  })
}
```

### 打卡弹窗组件TaskDetailDialog

打卡弹窗组件根据当前任务的ID获取任务名称以及弹窗背景图片资源。

打卡弹窗组件由两个小组件构成，代码如下：

```typescript
Column() {
  // 展示任务的基本信息
  TaskBaseInfo({
    taskName: TaskMapById[this.currentTask?.taskID].taskName, // 根据当前任务ID获取任务名称
  });
  // 打卡功能组件 （任务打卡、关闭弹窗）
  TaskClock({
    confirm: () => {
      this.dialogCallBack.confirmCallback(this.currentTask); // 任务打卡确认回调执行 
      this.controller.close();
    },
    cancel: () => {
      this.controller.close();
    },
    showButton: this.showButton
  })
}
...
```

TaskBaseInfo组件代码如下：

```typescript
@Component
struct TaskBaseInfo {
  taskName: string | Resource;
  build() {
    Column({ space: DEFAULT_8 }) {
      Text(this.taskName)
        ...
    }
    ...
  }
}

```

TaskClock组件代码如下：

```typescript
@Component
struct TaskClock {
  confirm: () => void;
  cancel: () => void;
  showButton: boolean = false;
  build() {
    Column({ space: DEFAULT_12 }) {
      Button() {
        Text($r('app.string.clock_in')) // 打卡
          ...
      }
      ...
      .onClick(() => {
         this.confirm();
      })
      .visibility(!this.showButton ? Visibility.None : Visibility.Visible)
      Text($r('app.string.got_it')) // 知道了
        ...
        .onClick(() => {
           this.cancel();
        })
    }
  }
}
```

### 打卡接口调用

```typescript
public async taskClock(taskInfo: TaskInfo) {
  let taskItem = await this.updateTask(taskInfo);
  // 更新任务失败 
  if (!taskItem) {
    return Promise.resolve({
      achievementLevel: 0,
      showAchievement: false
    });
  }
  // 更新当前时间的任务列表
  this.selectedDayInfo.taskList = this.selectedDayInfo.taskList.map((item) => item.taskID == taskItem?.taskID ? taskItem : item);
  let achievementLevel;
  if(taskItem.isDone) {
    // 更新每日任务完成情况数据
    let dayInfo = await this.updateDayInfo();
    ...
    if(dayInfo) {
      // 当日任务完成数量等于总任务数量时 累计连续打卡一天
      if (dayInfo.finTaskNum === dayInfo?.targetTaskNum) {
        // 更新成就勋章数据 判断是否弹出获得勋章弹出及勋章类型
        achievementLevel = await this.updateAchievement(this.selectedDayInfo.dayInfo);
      }
    }
  }
  ...
  return Promise.resolve({
    achievementLevel: achievementLevel,
    showAchievement: ACHIEVEMENT_LEVEL_LIST.includes(achievementLevel)
  });
}
```

```typescript
// 更新当天任务列表
updateTask(task: TaskInfo): Promise<TaskInfo> {
  return new Promise((resolve, reject) => {
    let {taskID, targetValue, finValue} = task;
    let updateTask = Object.assign({}, task);
    let step = TaskMapById[taskID].step; // 任务步长
    if (step === 0) { // 任务步长为0 打卡一次即完成该任务
      updateTask.isDone = true; // 打卡一次即完成该任务
      updateTask.finValue = targetValue;
    } else {
      let value = Number(finValue) + step; // 任务步长非0 打卡一次 步长与上次打卡进度累加
      updateTask.isDone = value >= Number(targetValue); // 判断任务是否完成
      updateTask.finValue = updateTask.isDone ? targetValue : `${value}`;
    }
    TaskInfoTableApi.updateDataByDate(updateTask, (res) => { // 更新数据库
      if (!res) {
        Logger.error('taskClock-updateTask', res);
        reject(res);
      }
      resolve(updateTask);
    })
  })
}
```

## 获取成就

本节将介绍成就页面。

### 功能概述

成就页面展示用户可以获取的所有勋章，当用户满足一定的条件时，将点亮本页面对应的勋章，没有得到的成就勋章处于熄灭状态。共有六种勋章，当用户连续完成任务打卡3天、7天、30天、50天、73天、99天时，可以获得对应的 "连续xx天达成" 勋章。

### 页面布局与 ArkTS 代码对应关系

效果如图所示：

![](figures/成就---副本.png)

标题部分TitleBar是一个横向容器Row里包含一个子组件Text。

```typescript
// TitleBarComponent.ets
@Component
export struct TitleBar {
  build() {
    Row() {
      Text($r('app.string.achievement')) 
      ... // 省略属性设置  
    }.width(commonConst.FULL_WIDTH)
  }
}
```

每个勋章卡片BadgeCard是一个竖向容器Column一个图片子组件Image和一个文字子组件Text。

```typescript
// BadegeCardComponent.ets
@Component
export struct BadgeCard {
  @Prop content: string;
  @Prop imgSrc: string;
  build() { 
    Column({space: commonConst.DEFAULT_18}) {
      Image($rawfile(this.imgSrc))
      ... // 省略属性设置
      Text($r('app.string.task_achievement_level', Number(this.content))) 
      ... // 省略属性设置
    }
  }
}
```

整体的勋章面板使用Flex一个组件即可以实现均分和换行的功能。

```typescript
// BadgePanelComponent.ets
@Component
export struct BadgePanel {
  @StorageProp(ACHIEVEMENT_LEVEL_KEY) successiveDays: number = 0;
  aboutToAppear() {
    getAchievementLevel();
  } 
  build() {
    Flex({ direction: FlexDirection.Row, wrap: FlexWrap.Wrap }) { 
      ForEach(getBadgeCards(this.successiveDays), (item) => { 
        BadgeCard({ content: item[0], imgSrc: item[1]})
      })
    }.width(commonConst.FULL_WIDTH)
  }
}
```

### 获取数据

进入界面第一次获取数据在aboutToAppear\(\)声明周期中从数据库GlobalInfo表中获取存储的勋章数据,  通过@StorageProp装饰器刷新界面，其他的地方只要通过AppStorage更新勋章数据即可。

```typescript
// BadgePanelComponent.ets
aboutToAppear() {
  getAchievementLevel();
} 

// AchieveModel.ets
export function getAchievementLevel() {
  let globalInfoTable: GlobalInfoTable = new GlobalInfoTable();
  globalInfoTable.query((res) => {
    ... // 省略数据验证
    if (achievements.length > 0) {
      AppStorage.Set<Number>(ACHIEVEMENT_LEVEL_KEY, Number(achievements[achievements.length - 1]));
    }
  }
}

// BadgePanelComponent.ets
@StorageProp(ACHIEVEMENT_LEVEL_KEY) successiveDays: number = 0;

ForEach(getBadgeCards(this.successiveDays), (item) => { 
  BadgeCard({ content: item[0], imgSrc: item[1]})
})

// AchievementViewModel.ets
export function getBadgeCardsItems(successiveDays: number):[string, string][] {
  let badgeMileStones = ACHIEVEMENT_LEVEL_LIST;
  let cardItems:[string, string][] = [];
  for (let i = 0; i < badgeMileStones.length; i++) { 
    ... // 省略数据拼装细节
    cardItems.push(oneItem);
  }    
  return cardItems;
}
```

## 搭建关系型数据库

本节将介绍如何调用关系型数据库接口在本地搭建数据库，并读写相应的用户数据。

### 创建数据库

要使用关系型数据库存储用户数据，首先要进行数据库的创建，并提供基本的增、删、查、改接口。

导入关系型数据库模块：

```typescript
import data_rdb from '@ohos.data.rdb';
```

关系型数据库提供以下两个基本功能：

![](figures/zh-cn_image_0000001459867377.png)

#### 获取RdbStore

首先要获取一个RdbStore来操作关系型数据库，代码如下：

```typescript
getRdbStore(callback) {
  // 如果已经获取到RdbStore则不做操作
  if (this.rdbStore != null) {
    Logger.info('Rdb', 'The rdbStore exists.');
    callback();
    return;
  }

  // 应用上下文，本Codelab使用API9 Stage模型的Context
  let context: Context = getContext(this) as Context;
  data_rdb.getRdbStore(context, STORE_CONFIG, 1, (err, rdb) => {
    if (err) {
      Logger.error('Rdb', 'gerRdbStore() failed, err: ' + err);
      return;
    }
    this.rdbStore = rdb;

    // 获取到RdbStore后，需使用executeSql接口初始化数据库表结构和相关数据
    this.rdbStore.executeSql(this.sqlCreateTable);          
    Logger.info('Rdb', 'getRdbStore() finished.');
    callback();
  });
}
```

#### 封装增、删、改、查接口

关系型数据库接口提供的增、删、改、查操作均有callback和Promise两种异步回调方式，本Codelab使用了callback异步回调，其中插入数据使用了insert\(\)接口，实现代码如下：

```typescript
insertData(data, callback) {
  let resFlag: boolean = false;  // 用于记录插入是否成功的flag
  const valueBucket = data;  // 存储键值对的类型，表示要插入到表中的数据行
  this.rdbStore.insert(this.tableName, valueBucket, function (err, ret) {
    if (err) {
      Logger.error('Rdb', 'insertData() failed, err: ' + err);
      callback(resFlag);
      return;
    }
    callback(!resFlag);
  });
}
```

删除数据使用了delete\(\)接口，实现代码如下：

```typescript
deleteData(predicates, callback) {
  let resFlag: boolean = false;

  // predicates表示待删除数据的操作条件
  this.rdbStore.delete(predicates, function (err, ret) {
    if (err) {
      Logger.error('Rdb', 'deleteData() failed, err: ' + err);
      callback(resFlag);
      return;
    }
    callback(!resFlag);
  });
}
```

更新数据使用了update\(\)接口，实现代码如下：

```typescript
updateData(predicates, data, callback) {
  let resFlag: boolean = false;
  const valueBucket = data;
  this.rdbStore.update(valueBucket, predicates, function (err, ret) {
    if (err) {
      Logger.error('Rdb', 'updateData() failed, err: ' + err);
      callback(resFlag);
      return;
    }
    callback(!resFlag);
  });
}
```

查找数据使用了query\(\)接口，实现代码如下：

```typescript
query(predicates, callback){
  // columns表示要查询的列，如果为空则表示查询所有列
  this.rdbStore.query(predicates, this.columns, function (err, resultSet) {
    if (err) {
      Logger.error('Rdb', 'query() failed, err: ' + err);
      return;
    }
    callback(resultSet);  // 如果查找成功则返回resultSet结果集
    resultSet.close();  // 操作完成后关闭结果集
  });
}
```



### 数据库表结构

根据健康生活APP的使用场景和业务逻辑，定义了三个数据对象，并使用三张数据表来存储，分别是健康任务信息表、每日信息表和全局信息表。

#### 健康任务信息表

目前健康生活应用提供了6个基本的健康任务，分别是早起、喝水、吃苹果、每日微笑、睡前刷牙和早睡。用户可以选择开启或关闭某个任务，开启的任务可以选择是否开启提醒，在指定的时间段内提醒用户进行打卡。任务也可以选择开启的频率，如只在周一到周五开启等。需要记录每项任务的目标值和实际完成值，在用户打卡后判断任务是否已经完成，并记录在数据库中。因此，需要创建一张存储每天的健康任务信息的表，表头如下：

![](figures/zh-cn_image_0000001409507612.png)

#### 每日信息表

在主页面，用户可以查看当天健康任务的完成进度，需要创建一张表记录当天开启的任务个数和已经完成的任务个数，表头如下：

![](figures/zh-cn_image_0000001409347632.png)

#### 全局信息表

用户连续多日打卡完成所有创建的任务可以获得相应的成就，因此，需要有一张表记录连续打卡天数和已达成的成就项。另外，考虑应用多日未打开的情况，需要记录应用第一次打开的日期和最后一次打开的日期以向数据库回填数据，表头如下：

![](figures/zh-cn_image_0000001409187812.png)

### 创建数据表

根据6.2中设计的表结构，创建对应的数据表，实现对相应数据的读写操作。

#### 健康任务信息数据表

在获取RdbStore后，需要使用executeSql接口执行SQL语句来创建相应的表结构和初始化数据，SQL语句如下：

```sql
CREATE TABLE IF NOT EXISTS taskInfo(
  id INTEGER PRIMARY KEY AUTOINCREMENT, 
  date TEXT NOT NULL, 
  taskID INTEGER, 
  targetValue TEXT NOT NULL, 
  isAlarm BOOLEAN, 
  startTime TEXT NOT NULL, 
  endTime TEXT NOT NULL, 
  frequency TEXT NOT NULL, 
  isDone BOOLEAN, 
  finValue TEXT NOT NULL, 
  isOpen BOOLEAN
)
```

健康任务信息数据表需要提供插入数据的接口，以在用户当天第一次打开应用时创建当天的健康任务信息，实现代码如下：

```typescript
insertData(taskInfo: TaskInfo, callback) {
  // 根据输入数据创建待插入的数据行
  const valueBucket = generateBucket(taskInfo);
  this.taskInfoTable.insertData(valueBucket, callback);
  Logger.info('TaskInfoTable', `Insert taskInfo {${taskInfo.date}:${taskInfo.taskID}} finished.`);
}
```

其中generateBucket\(\)代码如下：

```typescript
function generateBucket(taskInfo: TaskInfo) {
  let obj = {};
  TASK_INFO.columns.forEach((item) => {
    obj[item] = taskInfo[item];
  });
  return obj;
}

// Const.ts
export const TASK_INFO = {
  tableName: 'taskInfo',
  sqlCreate: 'CREATE TABLE IF NOT EXISTS taskInfo(id INTEGER PRIMARY KEY AUTOINCREMENT, date TEXT NOT NULL, taskID INTEGER, targetValue ' + 
  'TEXT NOT NULL, isAlarm BOOLEAN, startTime TEXT NOT NULL, endTime TEXT NOT NULL, frequency TEXT NOT NULL, isDone BOOLEAN, finValue ' + 
  'TEXT NOT NULL, isOpen BOOLEAN)',
  columns: ['id', 'date', 'taskID', 'targetValue', 'isAlarm', 'startTime', 'endTime', 'frequency', 'isDone', 'finValue', 'isOpen']
}
```

用户开启和关闭任务，改变任务的目标值、提醒时间、频率等，用户打卡后修改任务的实际完成值都是通过更新数据接口来实现的，代码如下：

```typescript
updateDataByDate(taskInfo: TaskInfo, callback) {
  const valueBucket = generateBucket(taskInfo);
  let predicates = new data_rdb.RdbPredicates(TASK_INFO.tableName);

  // 根据date和taskID匹配要更新的数据行
  predicates.equalTo('date', taskInfo.date).and().equalTo('taskID', taskInfo.taskID);
  this.taskInfoTable.updateData(predicates, valueBucket, callback);
  Logger.info('TaskInfoTable', `Update data {${taskInfo.date}:${taskInfo.taskID}} finished.`);
}
```

用户可以查看当天和以前某日的健康任务信息，需要提供查找数据接口，实现代码如下：

```typescript
query(date: string, isOpen: boolean = true, callback) {
  let predicates = new data_rdb.RdbPredicates(TASK_INFO.tableName);
  predicates.equalTo('date', date);

  // 如果isOpen为true，则只查找开启的任务
  if (isOpen) {
    predicates.equalTo('isOpen', true);
  }
  predicates.orderByAsc('taskID');  // 查找结果按taskID排序
  this.taskInfoTable.query(predicates, function(resultSet) {
    let count = resultSet.rowCount;
        
    // 查找结果为空则返回空数组，否则返回查找结果数组
    if (count === 0 || typeof count === 'string') {
      Logger.info('TaskInfoTable',`${date} query no results!`);
      callback([]);
    } else {
      resultSet.goToFirstRow();
      const result = [];
      for (let i = 0; i < count; i++) {
        let tmp = new TaskInfo(0,'', 0, '', false, '', '', '', false, '');
        tmp.id = resultSet.getDouble(resultSet.getColumnIndex('id'));
        ...  // 省略赋值代码
        result[i] = tmp;
        resultSet.goToNextRow();
      }
      callback(result);
    }
    return;
  });
}
```

#### 每日信息数据表

创建每日信息数据表的SQL语句如下：

```sql
CREATE TABLE IF NOT EXISTS dayInfo(
  date TEXT NOT NULL PRIMARY KEY, 
  targetTaskNum INTEGER, 
  finTaskNum INTEGER
)
```

在当天第一次打开应用时需要初始化每日信息数据，页面需要根据用户编辑任务和打卡的情况来更新当天目标任务个数和完成任务个数，所以需要提供插入数据和更新数据的接口，写法与上一条中相应接口类似，不再赘述。

页面需要查找对应日期的目标任务个数和完成任务个数用以在页面显示任务进度，因此需要查找数据的接口。且页面在打开时需要显示当周每天任务的完成情况，因此需要允许一次调用查找一周的每日任务信息。实现代码如下：

```typescript
queryList(dates: string[], callback) {
  let predicates = new data_rdb.RdbPredicates(DAY_INFO.tableName);
  predicates.in('date', dates);  // 匹配日期数组内的所有日期
  this.dayInfoTable.query(predicates, function(resultSet) {
    let count = resultSet.rowCount;
    if (count === 0) {
      Logger.info('DayInfoTable','query no results.');
      callback([]);
    } else {
      resultSet.goToFirstRow();
      let result = [];
      for (let i = 0; i < count; i++) {
        let tmp = new DayInfo('', 0, 0);
        tmp.date = resultSet.getString(resultSet.getColumnIndex('date'));
        ...  // 省略赋值代码
        result[i] = tmp;
        resultSet.goToNextRow();
      }
      callback(result);
    }
    return;
  });
}
```

#### 全局信息数据表

创建全局信息数据表的SQL语句如下：

```sql
CREATE TABLE IF NOT EXISTS globalInfo(
  id INTEGER PRIMARY KEY, 
  firstDate TEXT NOT NULL, 
  lastDate TEXT NOT NULL, 
  checkInDays INTEGER, achievements TEXT NOT NULL
)
```

全局信息数据表同样需要提供插入数据、更新数据和查找数据的接口，写法与本节前两条中相应接口类似，不再赘述。

### 数据库初始化

应用首次打开时，数据库中没有数据，要做数据库的初始化，写入一组空数据。另外，如果用户连续几天没有打开APP，再次打开时需要将数据回写至数据库。因此需要实现一个数据库接口，在应用打开时调用，进行上述操作。代码如下：

```typescript
query(date: string, callback) {
  let result = [];
  let self = this;
  this.globalInfoTable.query(function(globalResult) {
    if (globalResult.length === 0) {  // 如果查不到全局信息，就写入全局信息
      ...  // 插入健康任务信息、每日信息和全局信息
      callback(result, dayInfo);
    } else {  // 如果查到全局信息，那么查询当日任务信息
      let newGlobalInfo = globalResult;
      let preDate = globalResult.lastDate;
      newGlobalInfo.lastDate = date;
      ...  // 更新全局信息

      // 查询当日任务信息
      self.taskInfoTable.query(date, false, (taskResult) => {
        // 如果查不到当日任务信息，就查询全局任务信息
        if (taskResult.length === 0) {
          ...
          self.taskInfoTable.query(GLOBAL_KEY, false, (globalTaskResult) => {
          ...  // 回写没打开应用的时间段的健康任务信息和每日信息
          })
        } else {
          let taskNum = 0;
          let finNum = 0;
          ...  // 计算当日健康任务的开启个数和完成数
          dayInfo.targetTaskNum = taskNum;
          dayInfo.finTaskNum = finNum;
        }
        callback(result, dayInfo);
      });
    }
  });
}
```

## 编写通用工具类

本节将介绍日志打印、时间换算等通用工具类的编写和使用，工具类可以简化应用代码编写和业务流程处理。

### 日志类

日志类Logger旨在提供一个全局的日志打印、日志管理的地方，既可以规范整个应用的日志打印，也方便日后对日志工具类进行修改，而不需要去改动代码中每一个调用日志的地方，如切换具体的日志实现类（比如不使用Console而是HiLog），将日志记录到本地文件等。

Logger对外的日志API全部使用静态方法，方便调用者使用，目前分verbose，debug，info，warn，error五个级别。

使用方法如下：

1. import Logger日志类：

   ```typescript
   import { Logger } from '../utils/log/Logger';
   ```

2. 调用对应级别的静态方法:

   ```typescript
   Logger.debug('MyAbilityStage', 'onCreate');
   ```

   Logger内部采用单例模式，全局所有日志共享一个实例：

   ```typescript
   private static logger: ILogger;
   private constructor() {}
   public static getInstance(): Logger {
     if (!Logger.logger) {
       ...    
     } 
     return Logger.logger;
   }
   ```

   Logger目前在打印日志时会拼装本应用的唯一标识，方便筛选日志和调试：

   ```typescript
   // Logger.ets  
   private static appIdentifier: string = 'EHAPP';
   
   // ConsoleLogger.ets 
   public debug(tag: string, msg: string): void {
     console.debug(`[${ Logger.appIdentifier }] [debug] tag:${ tag } msg:${ msg }`);
   }
   ```

   Logger在打印日志前会判断当前是debug版本还是release版本，在release版本中关闭日志，防止应用上线后日志泄露，影响应用安全：

   ```typescript
   private static isAppDebugMode: boolean = true;
   
   public static debug(tag: string, msg: string): void {
     if (Logger.isAppDebugMode) {
       Logger.getInstance().debug(tag, msg);    
     }
   }
   ```


### 时间工具

为全局提供时间工具，避免重复定义。

1. 常用时间相关常量：

   ```typescript
   const CHINESE_OF_WEEK: string[] = ['一', '二', '三', '四', '五', '六', '日'];
   const YEAR: string = '年';
   const MONTH: string = '月';
   const DAY: string = '日';
   const WEEK: string = '星期'; 
   ```

2. 时间函数示例（由时间常量衍生出星期一到星期日和数字 1-7 的字典映射）：

   ```typescript
   export const oneWeekDictFunc = () => {
     const oneWeekDict = {};
     CHINESE_OF_WEEK.forEach((item, index) => {
       oneWeekDict[index + 1] = `${ WEEK }${ CHINESE_OF_WEEK[index] }`;
     })
     return oneWeekDict;
   }
   ```

### 单位转换工具

把比例等分浮点数转换为百分比字符串。

例如成就页面，每一行平均分布三个徽章，可以先定义一个浮点数代表等分比例，再转换为百分比字符串。

```typescript
export function ratio2percent(ratio: number): string {
  return `${ ratio * 100 }%`;
}
```

使用方法如下：

1. import 工具方法：

   ```typescript
   import { ratio2percent } from '../common/utils/Utils'
   ```

2. 引用工具方法 \( 例如成就页面，每个徽章占据屏幕宽度的三分之一 \) ：

   ```typescript
   Column({space: commonConst.DEFAULT_18}) { 
     ...  // 省略徽章卡片的 UI 布局细节
   }.width(ratio2percent(achieveConst.ACHIEVE_SPLIT_RATIO)) // achieveConst.ACHIEVE_SPLIT_RATIO = 1 / 3
   ```

### 事件分发类

事件分发类提供应用全局的事件注册，分发，接受，可以实现组件之间的解耦。

事件分发类全局共享一个实例， 将事件处理统一管理（HealthDataSrcMgr是单例）:

获取事件分发实例：

```typescript
// HomeIndex.ets
@Provide broadCast: BroadCast = HealthDataSrcMgr.getInstance().getBroadCast();

// HealthDataSrcMgr.ets
public getBroadCast(): BroadCast {
  return this.broadCast;
}
```

事件注册：

```typescript
// CustomDialogView.ets
aboutToAppear() {
  ...
  this.broadCast.on(BroadCastType.SHOW_ACHIEVEMENT_DIALOG, function (achievementLevel: number) { 
    ... // 省略回调细节
  })
  ...
}
  
// BroadCast.ets
public on(event, callback) {
  (this.callBackArray[event] || (this.callBackArray[event] = [])).push(callback);
}
```

取消事件注册：

```typescript
// HomeIndex.ets
aboutToDisappear() {
  this.broadCast.off(null, null);
}

// BroadCast.ets
public off(event, callback) {
  ... // 省略入参检查
  let len = cbs.length;
  for (let i = 0; i < len; i++) {
    cb = cbs[i];
    if (cb === callback || cb.fn === callback) {
      cbs.splice(i, 1);
      break;
    }
  }
}
```

发送事件：

```typescript
// HomeIndex.ets
taskItemAction(item: TaskInfo, isClick: boolean) {
  ...
  if (isClick) {
    // 点击任务打卡 
    ...
    this.broadCast.emit(BroadCastType.SHOW_TASK_DETAIL_DIALOG, [item, callback]);
  }
  else {
    ...
  }
}

// BroadCast.ets
public emit(event, args?: any[]) { 
  ... // 省略入参检查
  let len = cbs.length;
  for (let i = 0; i < len; i++) {
    try {
      cbs[i].apply(_self, args);
    } catch (e) {
      new Error(e);
    }
  }
}
```

## 总结

通过本次Codelab的学习，您应该已经掌握了页面跳转、自定义弹窗等UI方法，并学会了操作关系型数据库读写数据。

![](figures/彩带动效.gif)











