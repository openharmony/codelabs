# 目标管理（ArkTS）
## 介绍
本篇Codelab将介绍如何使用@State、@Prop、@Link、@Watch、@Provide、@Consume管理页面级变量的状态，实现对页面数据的增加、删除、修改。要求完成以下功能：

1.  实现一个自定义弹窗，完成添加子目标的功能。
2.  实现一个可编辑列表，可点击指定行展开调节工作目标进度，可多选、全选删除指定行。

![](figures/zh-cn_image_0000001546091404.gif)
### 相关概念

-   [页面状态管理](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/arkts-state-management-overview.md)：用于管理页面级变量的状态。
-   [自定义弹窗](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-methods-custom-dialog-box.md)： 通过CustomDialogController类显示自定义弹窗。
-   [List列表](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-container-list.md)：列表包含一系列相同宽度的列表项。

## 环境搭建
### 软件要求

-   [DevEco Studio](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)版本：DevEco Studio 3.1 Release。
-   OpenHarmony SDK版本：API version 9。

### 硬件要求

-   开发板类型：[润和RK3568开发板](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-appendix-rk3568.md)。
-   OpenHarmony系统：3.2 Release。

### 环境搭建

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**RK3568**开发板为例，参照以下步骤进行：

1.  [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。以3.2 Release版本为例：

    ![](figures/zh-cn_image_0000001569303293.png)

2.  搭建烧录环境。
    1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
    2.  [完成RK3568开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3568-burn.md)

3.  搭建开发环境。
    1.  开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#创建ets工程)创建工程（模板选择“Empty Ability”）。
    3.  工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#使用真机运行应用)。

## 代码结构解读

本篇Codelab只对核心代码进行讲解，对于完整代码，我们会在gitee中提供。

```
├──entry/src/main/ets                   // ArkTS代码区
│  ├──common
│  │  ├──constants
│  │  │  └──CommonConstants.ets         // 公共常量类
│  │  └──utils
│  │     ├──DateUtil.ets                // 获取格式化日期工具
│  │     └──Logger.ets                  // 日志打印工具类
│  ├──entryability
│  │  └──EntryAbility.ts                // 程序入口类
│  ├──pages
│  │  └──MainPage.ets                   // 主页面
│  ├──view
│  │  ├──TargetInformation.ets          // 整体目标详情自定义组件
│  │  ├──AddTargetDialog.ets            // 自定义弹窗
│  │  ├──ProgressEditPanel.ets          // 进展调节自定义组件
│  │  ├──TargetList.ets                 // 工作目标列表
│  │  └──TargetListItem.ets             // 工作目标列表子项
│  └──viewmodel
│     ├──DataModel.ets                  // 工作目标数据操作类
│     └──TaskItemViewModel.ets          // 任务进展实体类
└──entry/src/main/resources             // 资源文件目录
```
## 构建主界面

MainPage作为本应用的主界面，从上至下由三个自定义组件组成。

1.  标题titleBar。
2.  目标整体进展详情TargetInformation。
3.  子目标列表TargetList。

![](figures/zh-cn_image_0000001596850885.png)

MainPage主要维护五个参数：子目标数组targetData、子目标总数totalTasksNumber、已完成子目标数completedTasksNumber、最近更新时间latestUpdateDate、监听数据变化的参数overAllProgressChanged。具体作用有以下三个方面：

1.  子组件TargetInformation接收三个参数totalTasksNumber、completedTasksNumber、latestUpdateDate，渲染整体目标详情。
2.  子组件TargetList接收参数targetData渲染列表。
3.  使用@Watch监听overAllProgressChanged的变化。当overAllProgressChanged改变时，回调onProgressChanged方法，刷新整体进展TargetInformation。

```typescript
// MainPage.ets
@Entry
@Component
struct MainPage {
  // 子目标数组
  @State targetData: Array<TaskItemViewModel> = DataModel.getData();
  // 子目标总数
  @State totalTasksNumber: number = 0;
  // 已完成子目标数
  @State completedTasksNumber: number = 0;
  // 最近更新时间
  @State latestUpdateDate: string = CommonConstants.DEFAULT_PROGRESS_VALUE;
  // 监听数据变化的参数
  @Provide @Watch('onProgressChanged') overAllProgressChanged: boolean = false;	
  ...

  /**
   * overAllProgressChanged改变时的回调
   */
  onProgressChanged() {
    this.totalTasksNumber = this.targetData.length;
    this.completedTasksNumber = this.targetData.filter((item) => {
      return item.progressValue === CommonConstants.SLIDER_MAX_VALUE;
    }).length;
    this.latestUpdateDate = getCurrentTime();
  }

  build() {
    Column() {
      // 标题
      this.titleBar()
      // 目标整体进展详情
      TargetInformation({
        latestUpdateDate: this.latestUpdateDate,
        totalTasksNumber: this.totalTasksNumber,
        completedTasksNumber: this.completedTasksNumber
      })
      // 子目标列表
      TargetList({
        targetData: $targetData,
        onAddClick: () :void  => this.dialogController.open()
      })
        ...
    }
    ...
  }

  @Builder
  titleBar() {
    Text($r('app.string.title'))
      ...
  }
}
```
## 添加任务子目标

本章节主要介绍如何实现一个自定义弹窗，完成添加子目标的功能。效果如图所示：

![](figures/zh-cn_image_0000001596731141.png)

在MainPage.ets中，创建dialogController对象控制弹窗隐显，传入自定义组件AddTargetDialog和点击确定的回调方法saveTask。

```typescript
// MainPage.ets
@Entry
@Component
struct MainPage {
  dialogController: CustomDialogController = new CustomDialogController({
    builder: AddTargetDialog({
      onClickOk: (value: string): void => this.saveTask(value)
    }),
    alignment: DialogAlignment.Bottom,
    offset: {
      dx: CommonConstants.DIALOG_OFFSET_X,
      dy: $r('app.float.dialog_offset_y')
    },
    customStyle: true,
    autoCancel: false
  });
}
```

在AddTargetDialog.ets中，参数onClickOk为function类型，接收MainPage传入的saveTask方法。点击确定，调用onClickOk执行saveTask方法，关闭弹窗。

```typescript
// AddTargetDialog .ets
@CustomDialog
export default struct AddTargetDialog {
  ...
  private controller?: CustomDialogController;
  onClickOk?: (value: string) => void;

  build() {
    Column() {
      ...
      Text($r('app.string.add_task_dialog'))
      ...
      TextInput({ placeholder: $r('app.string.input_target_name')})
      ...
      .onChange((value: string) => {
        this.subtaskName = value;
      })
      Blank()
      Row() {
        ...
        Button($r('app.string.confirm_button'))
          .dialogButtonStyle()
          .onClick(() => {
            if (this.onClickOk !== undefined) {
              this.onClickOk(this.subtaskName);
            }
          })
      }
      ...
    }
    ...
  }
}
```

在MainPage.ets中，实现saveTask方法：保存数据至DataModel中，并更新targetData的值，完成添加子目标功能。

```typescript
// MainPage.ets
saveTask(taskName: string) {
  if (taskName === '') {
    promptAction.showToast({
      message: $r('app.string.cannot_input_empty'),
      duration: CommonConstants.TOAST_TIME,
      bottom: CommonConstants.TOAST_MARGIN_BOTTOM
    });
    return;
  }
  DataModel.addData(new TaskItemViewModel(taskName, 0, getCurrentTime()));
  this.targetData = DataModel.getData();
  this.overAllProgressChanged = !this.overAllProgressChanged;
  this.dialogController.close();
}
```
## 实现可编辑列表

本章节主要介绍子目标列表TargetList的实现，包括以下功能：

-   列表项展开。
-   列表子项点击下拉，滑动滑块更新进展。
-   列表进入编辑状态，单选、多选、全选、删除子项。

![](figures/zh-cn_image_0000001546411104.gif)


### 实现列表项展开

实现以下步骤完成点击列表项展开功能：

1.  使用@State 管理参数isExpanded，表示当前项是否展开，具体表现为自定义组件ProgressEditPanel的显示或隐藏。
2.  使用@Link和@Watch管理参数clickIndex，表示当前点击ListItem的Index索引。clickIndex值的改变将会传递至所有的ListItem。
3.  完成onClick点击事件，将isExpanded 值置反，修改clickIndex值为当前点击的索引。

```typescript
// TargetListItem.ets
@Component
export default struct TargetListItem {
  @State latestProgress?: number = 0;
  @Link @Watch('onClickIndexChanged') clickIndex: number;
  @State isExpanded: boolean = false;
  ...
  // clickIndex改变的回调方法
  onClickIndexChanged() {
    if (this.clickIndex !== this.index) {
      this.isExpanded = false;
    }
}

  build() {
    ...
    Column() {
      this.TargetItem()
      if (this.isExpanded) {
        Blank()
        // 自定义组件：编辑面板
        ProgressEditPanel({
          slidingProgress: this.latestProgress,
          onCancel: () => this.isExpanded = false,
          onClickOK: (progress: number): void => {
              this.latestProgress = progress;
              this.updateDate = getCurrentTime();
              let result = DataModel.updateProgress(this.index, this.latestProgress, this.updateDate);
              if (result) {
                this.overAllProgressChanged = !this.overAllProgressChanged;
              }
              this.isExpanded = false;
            },
          sliderMode: $sliderMode
        })
        ...
      }  
    }
    ...
    .onClick(() => {
      ...
      if (!this.isEditMode) {
        animateTo({ duration: CommonConstants.DURATION }, () => {
          this.isExpanded = !this.isExpanded;
        })
        this.clickIndex = this.index;
      }
    })
  }
  ...
}
```
### 实现更新进展

列表某项被展开后，实现以下步骤完成更新进展功能：

1.  Slider实现滑动条，滑动滑块调节进展，使用slidingProgress保存滑动值。
2.  点击确定调用onClickOK方法，将数据slidingProgress回调至TargetListItem。
3.  在TargetListItem中获取回调的数据并刷新页面。

```typescript
// ProgressEditPanel.ets
@Component
export default struct ProgressEditPanel {
  @Link sliderMode: number;
  @Prop slidingProgress: number = 0;
  onCancel?: () => void;
  onClickOK?: (progress: number) => void;

  build() {
    Column() {
      Slider({...})
      Row() {
        CustomButton({
          buttonText: $r('app.string.cancel_button')
        })
          .onClick(() => {
            if (this.onCancel !== undefined) {
              this.onCancel();
            }
          })
       CustomButton({
          buttonText: $r('app.string.cancel_button')
       })
          .onClick(() => {
            if (this.onClickOK !== undefined) {
              this.onClickOK(this.slidingProgress);
            }
          })
      }
    }
  }
}
```

在DataModel.ets中，编写updateProgress方法。该方法根据索引和进度值以及更新日期更新数据。

```typescript
// DataModel.ets
updateProgress(index: number, updateValue: number, updateDate: string): boolean {
  if (!this.targetData[index]) {
    return false;
  }
  this.targetData[index].progressValue = updateValue;
  this.targetData[index].updateDate = updateDate;
  return true;
}
```
### 实现列表多选

列表进入编辑模式才可单选、多选。实现以下步骤完成列表多选功能：

1.  维护一个boolean类型的数组selectArray，其长度始终与数据列表的长度相等，且初始值均为false。表示进入编辑状态时列表均未选中。
2.  定义一个boolean类型的值isEditMode，表示是否进入了编辑模式。
3.  TargetListItem选中状态的初始化和点击Checkbox改变TargetListItem的选中状态。

```typescript
// TargetList.ets
export default struct TargetList {
  ...
  @State isEditMode: boolean = false;
  @State selectArray: Array<boolean> = [];
  ...

  build() {
    Column() {
      ...
      if (this.isEditMode) {
        // 取消按钮
        Text($r('app.string.cancel_button'))
          ...
          .onClick(() => {
             this.selectAll = false;
             this.isEditMode = false;
             this.selectAllOrCancel(false);
          })
        ...
        // 全选按钮
        Checkbox()
          ...
          .onClick(() => {
            ...
            this.selectAllOrCancel(this.selectAll);
          })
      } else {
        // 编辑按钮
        Text($r('app.string.edit_button'))
          ...
          .onClick(() => {
            this.isEditMode = true;
            this.selectAllOrCancel(false);
          })
      }
      ...
    }
  }
}
```

点击全选Checkbox，将selectArray数组的值全赋值true或false，重新渲染列表为全选或者取消全选状态。

```typescript
// TargetList.ets
selectAllOrCancel(selectStatus: boolean) {
  let newSelectArray: Array<boolean> = [];
  this.targetData.forEach(() => {
    newSelectArray.push(selectStatus);
  });
  this.selectArray = newSelectArray;
}
```

在TargetListItem中，实现以下步骤改变ListItem的选中状态：

1.  使用@Link定义selectArr数组接收TargetList传入的selectArray。
2.  在TargetListItem渲染时，使用this.selectArr\[this.index\]获取初始选中状态。
3.  点击Checkbox时，按照当前ListItem的索引，将选中状态保存至selectArr，重新渲染列表完成单选和多选功能。

```typescript
// TargetListItem.ets
export default struct TargetListItem {

  ...
  @Link selectArr: Array<boolean>;
  public index: number = 0;
  
  build() {
    Stack({ alignContent: Alignment.Start }) {
      ...
      this.TargetItem()
      ...
      Checkbox()
        // 获取初始选中状态
        .select(this.selectArr[this.index])
        ...
        .onChange((isCheck: boolean) => {
          // 改变被点击项的选中状态
          this.selectArr[this.index] = isCheck;
        })
      ...
    ...
    }
  }
}
```
### 实现删除选中列表项

当点击“删除”时，TargetList.ets的deleteSelected方法，实现以下步骤完成列表项删除功能：

1.  调用DataModel的deleteData方法删除数据。
2.  更新targetData的数据重新渲染列表。
3.  修改overAllProgressChanged的值，通知主页刷新整体进展详情TargetInformation。

```typescript
// TargetList.ets
deleteSelected() {
  DataModel.deleteData(this.selectArray);
  this.targetData = DataModel.getData();
  this.overAllProgressChanged = !this.overAllProgressChanged;
  this.isEditMode = false;
}
```

在DataModel.ets中，遍历数据列表，删除被选中的数据项。

```typescript
// DataModel.ets
export class DataModel {
  ...
  deleteData(selectArr: Array<boolean>) {
    if (!selectArr) {
      Logger.error(TAG, 'Failed to delete data because selectArr is ' + selectArr);
    }
    let dataLen = this.targetData.length - CommonConstants.ONE_TASK;
    for (let i = dataLen; i >= 0; i--) {
      if (selectArr[i]) {
        this.targetData.splice(i, CommonConstants.ONE_TASK);
      }
    }
  }
  getData(): Array<TaskItemViewModel> {
    return this.targetData;
  }
  ...
}
```

## 总结

您已经完成了本次Codelab的学习，并了解到以下知识点：

1.  @State、@Prop、@Link、@Watch、@Provide、@Consume的使用。
2.  List组件的使用。
3.  自定义弹窗的使用。
4.  Slider组件的使用。

![](figures/zh-cn_image_0000001546251148.gif)







