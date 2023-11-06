# 1 卡片介绍<a name="ZH-CN_TOPIC_0000001512954284"></a>

使用ArkTS语言，实现一个导航与内容二级联动的效果。

# 2 标题<a name="ZH-CN_TOPIC_0000001564074353"></a>

二级联动（ArkTS）

# 3 介绍<a name="ZH-CN_TOPIC_0000001563834249"></a>


本篇Codelab是主要介绍了如何基于List组件实现一个导航和内容的二级联动效果。样例主要包含以下功能：

1.  切换左侧导航，右侧滚动到对应的内容。
2.  滚动右侧的内容，左侧会切换对应的导航。

效果如图所示：

![](figures/twolevel_320.gif)

## 相关概念<a name="section16161125162119"></a>

-   [List](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-list.md)：列表包含一系列相同宽度的列表项。适合连续、多行呈现同类数据，例如图片和文本。

-   [ListItemGroup](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-listitemgroup.md)：该组件用来展示列表item分组，宽度默认充满List组件，必须配合List组件来使用。

# 4 环境搭建<a name="ZH-CN_TOPIC_0000001563715325"></a>

## 软件要求<a name="zh-cn_topic_0000001353166866_section17200112092018"></a>

-   [DevEco Studio](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)版本：DevEco Studio 3.1 Release。
-   OpenHarmony SDK版本：API version 9。

## 硬件要求<a name="zh-cn_topic_0000001353166866_section820192019206"></a>

-   开发板类型：[润和RK3568开发板](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-appendix-rk3568.md)。
-   OpenHarmony系统：3.2 Release。

## 环境搭建<a name="zh-cn_topic_0000001353166866_section1020132022018"></a>

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**RK3568**开发板为例，参照以下步骤进行：

1.  [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。以3.2 Release版本为例：

    ![](figures/zh-cn_image_0000001681848240.png)

2.  搭建烧录环境。
    1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
    2.  [完成RK3568开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3568-burn.md)

3.  搭建开发环境。
    1.  开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#创建arkts工程)创建工程（模板选择“Empty Ability”）。
    3.  工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#使用真机运行应用)。

# 5 代码结构解读<a name="ZH-CN_TOPIC_0000001563954505"></a>

本篇Codelab只对核心代码进行讲解，对于完整代码，我们会在gitee中提供。

```
├──entry/src/main/ets                // 代码区
│  ├──common
│  │  └──constants
│  │     └──Constants.ets            // 常量类
│  ├──entryability
│  │  └──EntryAbility.ts             // 程序入口类
│  ├──pages
│  │  └──IndexPage.ets               // 二级联动页面入口
│  ├──view
│  │  ├──ClassifyItem.ets            // 课程分类组件
│  │  └──CourseItem.ets              // 课程信息组件
│  └──viewmodel                          
│     ├──ClassifyModel.ets           // 导航Model
│     ├──ClassifyViewModel.ets       // 导航ViewModel
│     ├──CourseModel.ets             // 课程内容Model
│     └──LinkDataModel.ets           // 数据源Model
└──entry/src/main/resources          // 资源文件
```

# 6 二级联动实现<a name="ZH-CN_TOPIC_0000001513074252"></a>

界面整体使用Row组件实现横向布局，分为左右两部分。均使用List组件实现对导航和内容的数据展示，导航部分固定宽度，内容部分自适应屏幕剩余宽度并用ListItemGroup完成每个导航下的内容布局。

![](figures/twolevel_320-0.png)

```
Row() {
  List({ scroller: this.classifyScroller }) {
    ForEach(this.classifyList, (item: ClassifyModel, index: number) => {
      ListItem() {
        ClassifyItem({
          classifyName: item.classifyName,
          isSelected: this.currentClassify === index,
          onClickAction: () => this.classifyChangeAction(index, true)
        })
      }
    }, (item: ClassifyModel) => item.classifyName + this.currentClassify)
  }

  List({ scroller: this.scroller }) {
    ForEach(this.classifyList, (classifyItem: ClassifyModel) => {
      ListItemGroup({
        header: this.ClassifyHeader(classifyItem.classifyName),
        space: Constants.COURSE_ITEM_PADDING
      }) {
        ForEach(classifyItem.courseList, (courseItem: CourseModel) => {
          ListItem() {
            CourseItem({ itemStr: JSON.stringify(courseItem) })
          }
        }, courseItem => courseItem.courseId)
      }
    }, item => item.classifyId)
  }
  .sticky(StickyStyle.Header)
  .layoutWeight(1)
  .edgeEffect(EdgeEffect.None)
  .onScrollIndex((start: number) => this.classifyChangeAction(start, false))
}
```

点击左侧导航时，右侧内容区域通过scrollToIndex方法跳转到对应的内容页面，并改变导航的选中状态。同理在滚动右侧内容的过程中，如果当前展示的ListItemGroup发生改变时，修改左侧导航的选中状态，并滚到到对应的导航item。

```
classifyChangeAction(index: number, isClassify: boolean) {
  if (this.currentClassify !== index) {
    // change the classify status
    this.currentClassify = index;
    if (isClassify) {
      // scroll the course scroll
      this.scroller.scrollToIndex(index);
    } else {
      // scroll the classify scroll
      this.classifyScroller.scrollToIndex(index);
    }
  }
}
```

# 7 总结<a name="ZH-CN_TOPIC_0000001563714621"></a>

您已经完成了本次Codelab的学习，并了解到以下知识点：

1.  使用List组件实现导航与内容联动的效果。
2.  ListItemGroup组件的使用方法。

![](figures/彩带动效.gif)