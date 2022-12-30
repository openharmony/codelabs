# 1.介绍

OpenHarmony的ArkUI（基于JS扩展的类Web开发范式）提供了常用的接口和组件，开发者可以根据实际场景和开发需求，选用不同的组件和接口。本篇Codelab我们将一起开启ArkUI（基于JS扩展的类Web开发范式）基础组件的学习之路。本教程是基础组件之slider以及image的使用。

在本教程中，我们将会通过一个简单的样例，将slider组件和image组件进行结合，实现一个风车转动的效果，用户可以利用slider组件调节风车的大小和转动速度。效果如下图所示，开发者还可以根据自己的需求添加不同的效果。

![](figures/VID_20211228_154514-00_00_00-00_00_30.gif)

## 应用场景

-   视频、音乐类应用调节音量等场景。

-   视频应用、系统应用调节屏幕亮度等场景。

# 2.相关概念

[slider组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-slider.md)
：滑动条组件，用来快速调节设置值，如音量、亮度等。

[image组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-image.md)
：图片组件，用来渲染展示图片。

[div组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-container-div.md)
：基础容器组件，用作页面结构的根节点或将内容进行分组。

[text组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-text.md)
：文本组件，用于呈现一段信息。

[animation-duration](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-common-animation.md)
：动画样式。

# 3.搭建OpenHarmony环境

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**Hi3516DV300**开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96) ：标准系统解决方案（二进制）

   以3.0版本为例：

   ![](figures/取版本.png)

2. 搭建烧录环境

   1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
   2.  [完成Hi3516开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3516-burn.md)
3. 搭建开发环境

   1. 开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87) ，完成DevEco Studio的安装和开发环境配置。
   2. 开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-js-fa.md#%E5%88%9B%E5%BB%BAjs%E5%B7%A5%E7%A8%8B) 创建工程（模板选择“Empty Ability”），选择JS或者eTS语言开发。
   3. 工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-js-fa.md#%E4%BD%BF%E7%94%A8%E7%9C%9F%E6%9C%BA%E8%BF%90%E8%A1%8C%E5%BA%94%E7%94%A8) 。


# 4.将组件添加到布局文件中

在这个任务中，我们需要完成程序页面的新建和设计，并将程序使用到的slider组件、image组件添加到布局文件index.hml中。在完成新建项目后，我们看到系统自动创建了pages.index目录，在这个目录下，我们找到index.hml文件，开始进行页面设计。

打开index.hml文件，默认代码使用div组件和text组件来共同呈现文本显示的效果，具体代码如下：

```
<div class="container">
    <text class="title">
        {{ $t('strings.hello') }} {{ title }}
    </text>
</div>
```

## image组件

开发者可以删除默认代码跟着接下来的步骤一起开发，整个布局文件使用div组件将页面进行划分，分别是上方的动画展示，和下方的操作控制。动画展示部分使用到image组件：

![](figures/windfill1.jpg)

```
<div class="image-block">
	<image id="image" src="{{imgUrl}}" style="transform: scale({{imageSize}}); animation-duration: {{animationDuration}};"/>
</div>
```

其中src属性用来表示风车图片的路径；style属性用来定义图片的大小，[animation-duration动画样式](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-common-animation.md) 用来定义图形旋转一周所用的时间。三个属性具体的值定义在index.js中。

## slider组件

页面的下半部分由容器组件div、基础组件[text](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-text.md) 、[slider](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-slider.md) 共同组成。text组件用来显示“速度”、“缩放比例”文本，slider用来实现控制风车的转动速度以及风车缩放的大小：

![](figures/1112131.jpg)

```
<div class="slider-block">
	<text>
		速度：{{speed}}
	</text>
	<slider class="slider" min="{{minSpeed}}" max="{{maxSpeed}}" value="{{speed}}" onchange="changeValue"></slider>
	<text>
		缩放比例：{{imageSize}}
	</text>
	<slider class="slider" min="{{minSize}}" max="{{maxSize}}" value="{{size}}" onchange="changeSize"></slider>
</div>
```

slider组件的相关属性说明可参考下表：

<table><thead align="left"><tr id="row9589959475"><th class="cellrowborder" valign="top" width="25%" id="mcps1.1.5.1.1"><p id="p1659012594718"><a name="p1659012594718"></a><a name="p1659012594718"></a>名称</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.1.5.1.2"><p id="p559016591373"><a name="p559016591373"></a><a name="p559016591373"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.1.5.1.3"><p id="p1759011591571"><a name="p1759011591571"></a><a name="p1759011591571"></a>默认值</p>
</th>
<th class="cellrowborder" valign="top" width="25%" id="mcps1.1.5.1.4"><p id="p1859065918710"><a name="p1859065918710"></a><a name="p1859065918710"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row45901599717"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.1 "><p id="p41715199815"><a name="p41715199815"></a><a name="p41715199815"></a>min</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.2 "><p id="p51751911814"><a name="p51751911814"></a><a name="p51751911814"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.3 "><p id="p15901359972"><a name="p15901359972"></a><a name="p15901359972"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.4 "><p id="p1785572517812"><a name="p1785572517812"></a><a name="p1785572517812"></a>滑动选择器的最小值。</p>
</td>
</tr>
<tr id="row159025916712"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.1 "><p id="p175901459773"><a name="p175901459773"></a><a name="p175901459773"></a>max</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.2 "><p id="p759015595717"><a name="p759015595717"></a><a name="p759015595717"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.3 "><p id="p1459055914716"><a name="p1459055914716"></a><a name="p1459055914716"></a>100</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.4 "><p id="p994174313811"><a name="p994174313811"></a><a name="p994174313811"></a>滑动选择器的最大值。</p>
</td>
</tr>
<tr id="row159015591071"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.1 "><p id="p0590059372"><a name="p0590059372"></a><a name="p0590059372"></a>value</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.2 "><p id="p359013591671"><a name="p359013591671"></a><a name="p359013591671"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.3 "><p id="p1359025910710"><a name="p1359025910710"></a><a name="p1359025910710"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="25%" headers="mcps1.1.5.1.4 "><p id="p1621125813101"><a name="p1621125813101"></a><a name="p1621125813101"></a>滑动选择器的初始值。</p>
</td>
</tr>
</tbody>
</table>


onchange事件在滑动slider组件时候执行，具体的属性值以及相应事件的定义，会在任务四中具体介绍。下面是该部分的完整代码：

```
<div class="container">
    <div class="image-block">
        <image id="image" src="{{imgUrl}}" style="transform: scale({{imageSize}}); animation-duration: {{animationDuration}};"/>
    </div>
    <div class="slider-block">
        <text>
            速度：{{speed}}
        </text>
        <slider class="slider" min="{{minSpeed}}" max="{{maxSpeed}}" value="{{speed}}" onchange="changeValue"></slider>
        <text>
            缩放比例：{{imageSize}}
        </text>
        <slider class="slider" min="{{minSize}}" max="{{maxSize}}" value="{{size}}" onchange="changeSize"></slider>
    </div>
</div>
```

# 5.为页面设计样式

在这个任务中，我们将一起为写好页面添加样式，上面所有的组件，我们都定义了class属性，它对应的样式都定义在index.css中，有关css更多的知识可以参考[CSS语法参考](https://gitee.com/openharmony/docs/blob/OpenHarmony-3.0-LTS/zh-cn/application-dev/js-reference/js-framework-syntax-css.md)。

## image组件部分样式

这部分定义了image-block容器的样式，以及image组件的样式。删掉默认代码后，在index.css中先添加如下代码：

```
/* 页面整体纵向布局 */
.container {
    flex-direction: column;
}
/* 图片区域布局 */
.image-block {
    height: 50%;
    justify-content: center; /* 水平居中 */
    align-items: center; /* 垂直居中 */
}
/* 图片布局 */
image {
    width: 150px;
    height: 150px;
    animation-name: Go;
    animation-iteration-count: infinite; /* 图片不限旋转次数 */
    animation-timing-function: linear; /* 动画匀速转动 */
}
/* 图片旋转角度：0到360° */
@keyframes Go
{
    from {
        transform:rotate(0deg);
    }
    to {
        transform:rotate(360deg);
    }
}
```

每个样式的具体含义，在注释中有较为详细的说明，值得注意的是@keyframes Go定义了动画的旋转角度为从0度旋转到360度。

## slider组件部分样式

这部分定义了slider-block容器、slider组件和text组件的样式，继续在index.css中添加如下代码：

```
/* 进度条区域布局 */
.slider-block {
    height: 50%;
    justify-content: center; /* 水平居中 */
    align-items: center; /* 垂直居中 */
    flex-direction: column;
}
/* 进度条布局 */
slider {
    margin-bottom: 20px;
}
/* 文字布局 */
text {
    margin-bottom: 10px;
    font-size: 20px;
}
```

# 6.为组件添加响应事件

接下来，我们一起来完成整个样例的最后一步：定义slider的响应事件和一些初始值，完成最后一个任务，风车才可以随意调节大小并且转动。

## 数据定义

我们已经已经提到了布局中组件初始值的定义，在index.js中首先添加如下代码：

```
export default {
    data: {
        title: '',
        imgUrl: '/common/images/windmill.png',
        animationDuration: '10000ms',
        animationDurationNum: 10000,
        speed: 50,
        minSpeed: 0,
        maxSpeed: 100,
        imageSize: 1,
        size: 0,
        minSize: 0,
        maxSize: 100
    },
```

这些值的具体含义，可以参考如下表格：

<table><thead align="left"><tr id="row7799183932719"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p13799163942713"><a name="p13799163942713"></a><a name="p13799163942713"></a>key</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p1979920394275"><a name="p1979920394275"></a><a name="p1979920394275"></a>description</p>
</th>
</tr>
</thead>
<tbody><tr id="row167993390271"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p3800539112717"><a name="p3800539112717"></a><a name="p3800539112717"></a>imgUrl</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p16800139132713"><a name="p16800139132713"></a><a name="p16800139132713"></a>图片存放的路径。</p>
</td>
</tr>
<tr id="row7800173942712"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p16800639142716"><a name="p16800639142716"></a><a name="p16800639142716"></a>animationDuration</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p180003913276"><a name="p180003913276"></a><a name="p180003913276"></a>图片转360度所用时间。</p>
</td>
</tr>
<tr id="row3800163922714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p88001339112719"><a name="p88001339112719"></a><a name="p88001339112719"></a>animationDurationNum</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p16800103917279"><a name="p16800103917279"></a><a name="p16800103917279"></a>改变图片转速响应事件的速度初始值。</p>
</td>
</tr>
<tr id="row880093915275"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1880017395270"><a name="p1880017395270"></a><a name="p1880017395270"></a>speed</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p28001739102710"><a name="p28001739102710"></a><a name="p28001739102710"></a>图片转速初始值。</p>
</td>
</tr>
<tr id="row280083914277"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p180016398274"><a name="p180016398274"></a><a name="p180016398274"></a>minSpeed</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p5800639122712"><a name="p5800639122712"></a><a name="p5800639122712"></a>slider组件控制速度的最小值。</p>
</td>
</tr>
<tr id="row68001939122713"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p138001639162715"><a name="p138001639162715"></a><a name="p138001639162715"></a>maxSpeed</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p15619149183313"><a name="p15619149183313"></a><a name="p15619149183313"></a>slider组件控制速度的最大值。</p>
</td>
</tr>
<tr id="row6800539192714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1280093992711"><a name="p1280093992711"></a><a name="p1280093992711"></a>imageSize</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p380083911278"><a name="p380083911278"></a><a name="p380083911278"></a>图像大小。</p>
</td>
</tr>
<tr id="row18006395275"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p188004396279"><a name="p188004396279"></a><a name="p188004396279"></a>size</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p68001539162711"><a name="p68001539162711"></a><a name="p68001539162711"></a>改变图像大小响应事件的图像初始值。</p>
</td>
</tr>
<tr id="row380073992718"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p4800203916275"><a name="p4800203916275"></a><a name="p4800203916275"></a>minSize</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p168001239142710"><a name="p168001239142710"></a><a name="p168001239142710"></a>slider组件控制缩放比例的最小值。</p>
</td>
</tr>
<tr id="row7800339142718"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p78001839172716"><a name="p78001839172716"></a><a name="p78001839172716"></a>maxSize</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p080018397276"><a name="p080018397276"></a><a name="p080018397276"></a>slider组件控制缩放比例的最大值。</p>
</td>
</tr>
</tbody>
</table>


## 事件

继续添加如下代码，实现改变风车的转速和大小：

```
    // 改变转速
    changeValue(e) {
        if (e.mode == 'end' || e.mode == 'click') {
            this.speed = e.value;
            this.animationDurationNum = 10000 - e.value * 95 ;
            this.animationDuration = this.animationDurationNum + 'ms';
        }
    },
    // 改变大小
    changeSize(e) {
        if (e.mode == 'end' || e.mode == 'click') {
            this.size = e.value;
            this.imageSize = this.size / 40 < 1 ? 1 : this.size / 40;

        }
    }
}
```

到此我们已经完成了所有的任务和代码的编写。

>![](public_sys-resources/icon-note.gif) **说明：** 
>样例中我们使用了一张风车图片，它放在js/default/common.images目录下，命名为windmill.png

# 7.恭喜您

在本篇Codelab中，我们主要为大家讲解了如下JS基础组件

-   slider
-   image

通过一个代码示例，实现风车转速和大小的调节，从布局、样式、响应事件三个层面，逐步为大家进行代码讲解。希望通过本教程，各位开发者可以对以上基础组件具有更深刻的认识。
