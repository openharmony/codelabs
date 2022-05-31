# 1.介绍

OpenHarmony的ArkUI（基于JS扩展的类Web开发范式）提供了常用的接口和组件，开发者可以根据实际场景和开发需求，选用不同的组件和接口。本篇Codelab，我们将一起开启ArkUI（基于JS扩展的类Web开发范式）基础组件的学习之路。本教程是基础组件rating的使用。

在本教程中，我们将会通过一个简单的样例，实现一个星级打分的效果，开发者可以使用rating组件调节星级评分的大小。效果如下图所示，开发者还可以根据自己的需求添加不同的效果。

![](figures/unnaming.png)![](figures/unnaming-(1).png)![](figures/unnaming-(2).png)

## 应用场景

-   购物、团购类应用打分评价场景。

# 2.相关概念

[rating组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-rating.md) ：评分条，表示用户使用感受的衡量标准条。

[div组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-container-div.md) ：基础容器组件，用作页面结构的根节点或将内容进行分组。

[text组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-text.md) ：文本组件，用于呈现一段信息。

# 3.搭建OpenHarmony环境

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**Hi3516DV300**开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96) ：标准系统解决方案（二进制）

   以3.0版本为例：

   ![](figures/取版本.png)

2. 搭建烧录环境

   1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-standard-env-setup.md)

   2.  [完成Hi3516开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-steps-hi3516-burn.md)

3. 搭建开发环境

   1.  开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87) ，完成DevEco Studio的安装和开发环境配置。
   2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-js.md#%E5%88%9B%E5%BB%BAjs%E5%B7%A5%E7%A8%8B) 创建工程（模板选择“Empty Ability”），选择JS或者eTS语言开发。
   3.  工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets.md#%E4%BD%BF%E7%94%A8%E7%9C%9F%E6%9C%BA%E8%BF%90%E8%A1%8C%E5%BA%94%E7%94%A8) 。
   
# 4.将组件添加到布局文件中

在这个任务中，我们需要完成程序页面的新建和设计，并将程序使用到的rating组件添加到布局文件index.hml中。在完成新建项目后，我们看到系统自动创建了pages.index目录，在这个目录下，我们找到index.hml文件，开始进行页面设计。

打开index.hml文件，默认代码使用div组件和text组件来共同呈现文本显示的效果，具体代码如下：

```
<div class="container">
    <text class="title">
        {{ $t('strings.hello') }} {{ title }}
    </text>
</div>
```

## rating组件

开发者可以删除默认代码跟着接下来的步骤一起开发，整个布局文件使用div组件将页面进行划分，分别是上方的自定义星级打分和下方显示平均分星级。

```
<div class="container">
        <text  class="avgText">自定义星级打分</text>
        <rating onchange="rateBlue" class="rateBlue" rating ="3"></rating>
        <rating onchange="rateRed" class="rateRed"  rating ="2.5"></rating>
        <rating onchange="rateGreen" class="rateGreen"rating ="1.5" ></rating>
        <text  class="avgText"  maxlength="20">平均分等级</text>
        <rating class="rate" indicator="true" stepsize="0.1" rating ="{{avg}}"></rating>
</div>
```

rating组件的属性说明可参考下表：

<table><thead align="left"><tr id="row9589959475"><th class="cellrowborder" valign="top" width="20%" id="mcps1.1.5.1.1"><p id="p1659012594718"><a name="p1659012594718"></a><a name="p1659012594718"></a>名称</p>
</th>
<th class="cellrowborder" valign="top" width="20%" id="mcps1.1.5.1.2"><p id="p559016591373"><a name="p559016591373"></a><a name="p559016591373"></a>类型</p>
</th>
<th class="cellrowborder" valign="top" width="20%" id="mcps1.1.5.1.3"><p id="p1759011591571"><a name="p1759011591571"></a><a name="p1759011591571"></a>默认值</p>
</th>
<th class="cellrowborder" valign="top" width="40%" id="mcps1.1.5.1.4"><p id="p1859065918710"><a name="p1859065918710"></a><a name="p1859065918710"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row45901599717"><td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.1 "><p id="p41715199815"><a name="p41715199815"></a><a name="p41715199815"></a>indicator</p>
</td>
<td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.2 "><p id="p51751911814"><a name="p51751911814"></a><a name="p51751911814"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.3 "><p id="p15901359972"><a name="p15901359972"></a><a name="p15901359972"></a>false</p>
</td>
<td class="cellrowborder" valign="top" width="40%" headers="mcps1.1.5.1.4 "><p id="p12829183319245"><a name="p12829183319245"></a><a name="p12829183319245"></a>设置评分条是否作为一个指示器，此时用户不可操作。</p>
</td>
</tr>
<tr id="row159025916712"><td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.1 "><p id="p99319403224"><a name="p99319403224"></a><a name="p99319403224"></a>stepsize</p>
</td>
<td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.2 "><p id="p759015595717"><a name="p759015595717"></a><a name="p759015595717"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.3 "><p id="p541415236562"><a name="p541415236562"></a><a name="p541415236562"></a>0.5</p>
</td>
<td class="cellrowborder" valign="top" width="40%" headers="mcps1.1.5.1.4 "><p id="p1697652814567"><a name="p1697652814567"></a><a name="p1697652814567"></a>设置评分条的评星步长。</p>
</td>
</tr>
<tr id="row159015591071"><td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.1 "><p id="p19500546122212"><a name="p19500546122212"></a><a name="p19500546122212"></a>rating</p>
</td>
<td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.2 "><p id="p359013591671"><a name="p359013591671"></a><a name="p359013591671"></a>number</p>
</td>
<td class="cellrowborder" valign="top" width="20%" headers="mcps1.1.5.1.3 "><p id="p1359025910710"><a name="p1359025910710"></a><a name="p1359025910710"></a>0</p>
</td>
<td class="cellrowborder" valign="top" width="40%" headers="mcps1.1.5.1.4 "><p id="p15172165410566"><a name="p15172165410566"></a><a name="p15172165410566"></a>设置评分条当前评星数。</p>
</td>
</tr>
</tbody>
</table>

# 5.为页面设计样式

在这个任务中，我们将一起为写好的页面添加样式，上面所有的组件，我们都定义了class属性，对应的样式都定义在index.css中，有关css更多的知识可以参考[CSS语法参考](https://gitee.com/openharmony/docs/blob/OpenHarmony-3.0-LTS/zh-cn/application-dev/js-reference/js-framework-syntax-css.md)。

## rating 组件样式

删掉默认代码后，在index.css中先添加如下代码：

```
/* 页面整体纵向布局 */
.container {
    flex-direction: column;
/*    justify-content: center;*/
    align-items: center;
    padding-top: 30px;

}
/* 平均分等级样式 */
.rate{
    width:200px;
    height:45px;
    margin-top: 20px;
}
/* 字体样式 */
.avgText{
    text-align:center;
    font-size:20px;
    width: 200px;
    color: #5A554D;
    margin-top: 80px;
    font-weight:900
}
/*自定义星级布局 */
.rateRed{
    star-background:url("/common/asserts/zdy1.png");
    star-foreground:url("/common/asserts/zdy2.png");
    star-secondary:url("/common/asserts/zdy3.png");
    width:200px;
    height:45px;
}

.rateBlue{
    star-background:url("/common/asserts/zdy1.png");
    star-foreground:url("/common/asserts/zdy2.png");
    star-secondary:url("/common/asserts/zdy3.png");
    width:200px;
    height:45px;
    margin-top: 10px;
}
.rateGreen{
    star-background:url("/common/asserts/zdy1.png");
    star-foreground:url("/common/asserts/zdy2.png");
    star-secondary:url("/common/asserts/zdy3.png");
    width:200px;
    height:45px;
}
```

# 6.恭喜你

在本篇Codelab中，我们主要为大家讲解了如下JS基础组件

-   rating

通过一个代码示例，实现自定义星级打分以及显示星级平均分，从布局、样式、响应事件三个层面，逐步为大家进行代码讲解。希望通过本教程，各位开发者可以对以上基础组件具有更深刻的认识。

# 7.参考

[gitee地址](https://gitee.com/openharmony/codelabs/tree/master/JSUI/RatingApplication)