# InputApplication
# 介绍<a name="ZH-CN_TOPIC_0000001231762989"></a>

-   [应用场景]

OpenHarmony ArkUI（基于JS扩展的类Web开发范式）框架提供了常用的接口和组件，开发者可以根据实际场景和开发需求，选用不同的组件和接口。本篇Codelab，我们将一起开启ArkUI（基于JS扩展的类Web开发范式）基础组件的学习之路。本教程是基础组件之input组件的使用。

在本教程中，我们将会通过一个简单的样例，使用input的text、email、date、number、password、button、checkbox、radio等类型实现一个form表单的提交效果。效果如下图所示，开发者还可以根据自己的需求添加不同的效果。

![](figures/IMG_20211213_141801.jpg)

## 应用场景<a name="section225718574575"></a>

-   社交、金融类应用涉及到用户注册等需要用户提交表单的场景。

# 相关概念<a name="ZH-CN_TOPIC_0000001231763361"></a>

[input组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-js/js-components-basic-input.md) ：交互式组件，包括单选框、多选框、按钮和单行文本输入框。

# 搭建OpenHarmony环境<a name="ZH-CN_TOPIC_0000001238774623"></a>

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**Hi3516DV300**开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96) ：标准系统解决方案（二进制）

   以3.0版本为例：

   ![](figures/取版本.png)

2. 搭建烧录环境

   1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
   2.  [完成Hi3516开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-3516-burn.md)
3. 搭建开发环境

    1.  开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87) ，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-js-fa.md#%E5%88%9B%E5%BB%BAjs%E5%B7%A5%E7%A8%8B) 创建工程（模板选择“Empty Ability”），选择JS或者eTS语言开发。
    3.  工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-js-fa.md#%E4%BD%BF%E7%94%A8%E7%9C%9F%E6%9C%BA%E8%BF%90%E8%A1%8C%E5%BA%94%E7%94%A8) 。
    
# 将组件添加到布局文件中<a name="ZH-CN_TOPIC_0000001186403694"></a>

-   [input组件]

在这个任务中，我们需要完成程序页面的新建和设计，并将程序使用到的input组件添加到布局文件index.hml中。在完成任务一新建项目后生成的默认目录pages.index下，打开index.hml文件，开始进行页面设计。

打开index.hml文件，默认代码如下：

```
<div class="container">
    <text class="title">
        {{ $t('strings.hello') }} {{ title }}
    </text>
</div>
```

代码使用div组件和text组件来共同呈现文本显示的效果，其中div属于[基础容器](https://developer.harmonyos.com/cn/docs/documentation/doc-references/js-components-container-div-0000000000611484) ，用作页面结构的根节点或将内容进行分组；text是[文本组件](https://developer.harmonyos.com/cn/docs/documentation/doc-references/js-components-basic-text-0000000000611633) ，用于呈现一段信息。有关这两个组件更多的知识，我们会在其他Codelab中为大家讲解。

## input组件<a name="section1648574312326"></a>

开发者可以删除index.hml文件中的默认代码，根据下述步骤一起进行开发，整个布局文件使用div组件将页面进行划分，页面从上到下包括：text、password、email、date、number、radio、checkbox、button类型，页面如下：

![](figures/IMG_20211213_141801-0.jpg)

上图的页面hml示例代码如下：

```
<div class="container">
    <div class="input-form">
        <text class="form-text">input 表单</text>
    </div>
    <div class="row">
        <label class="label">用户名：</label>
        <div class="input-block">
            <input id="username" class="input" type="text" maxlength="10" placeholder="请输入内容用户名" headericon="{{require}}" onchange="change" ontranslate="translate"/>
        </div>
    </div>
    <div class="row">
        <label class="label">请设置密码：</label>
        <div class="input-block">
            <input id="password" class="input" type="password" maxlength="10" placeholder="请输入密码" headericon="{{require}}" onchange="change"/>
        </div>
    </div>
    <div class="row">
        <label class="label">请确认密码：</label>
        <div class="input-block">
            <input id="password2"class="input" type="password" maxlength="10" placeholder="请再次确认密码" headericon="{{require}}" onchange="change"/>
        </div>
    </div>
    <div class="row">
        <label class="label">电子邮箱：</label>
        <div class="input-block">
            <input id="email" class="input" type="email" maxlength="20" placeholder="请输入电子邮箱" headericon="{{require}}" onchange="change" onshare="share"/>
        </div>
    </div>
    <div class="row">
        <label class="label">出生日期：</label>
        <div class="input-block">
            <input id="date" class="input" type="date" maxlength="20" placeholder="请选输入生年月" onchange="change" onsearch="search"/>
        </div>
    </div>
    <div class="row">
        <label class="label">身高（cm）：</label>
        <div class="input-block">
            <input id="height" class="input" type="number" maxlength="3" placeholder="请选输入身高" onchange="change" menuoptions="{{list}}" onoptionselect="optionselect"/>
        </div>
    </div>
    <div class="row">
        <label class="label">性别：</label>
        <div class="input-block">
            <input name="radio" type="radio" checked='true' value="0"></input>
            <text class="sex">男</text>
            <input name="radio" type="radio" value="1">女</input>
            <text class="sex">女</text>
        </div>
    </div>
    <div class="row">
        <label class="label">爱好：</label>
        <div class="input-block">
            <input onchange="checkboxOnChange" checked="false" type="checkbox" value="0"></input>
            <text class="favorite">游泳</text>
            <input onchange="checkboxOnChange" checked="false" type="checkbox" value="1"></input>
            <text class="favorite">健身</text>
            <input onchange="checkboxOnChange" checked="false" type="checkbox" value="2"></input>
            <text class="favorite">足球</text>
            <input onchange="checkboxOnChange" checked="false" type="checkbox" value="3"></input>
            <text class="favorite">篮球</text>
        </div>
    </div>

    <input class="button" type="button" value="提交" onclick="buttonClick" />

</div>
```

# 为页面设计样式<a name="ZH-CN_TOPIC_0000001186562260"></a>

-   [input组件部分样式]

在这个任务中，我们将一起为写好的页面添加样式，上面所有的组件，我们都定义了class属性，它对应的样式都定义在index.css中，有关css更多的知识可以参考[CSS语法参考](https://gitee.com/openharmony/docs/blob/OpenHarmony-3.0-LTS/zh-cn/application-dev/js-reference/js-framework-syntax-css.md)。

## input组件部分样式<a name="section165281839182814"></a>

这部分定义了输入框、单选、多选组件的样式。删掉默认代码后，在index.css中先添加如下代码：

```
.container {
    flex-direction: column;
    align-items: center;
}
.input-form {
    width: 100%;
    justify-content: center;
    margin-top: 10px;
    margin-bottom: 10px;
}
.form-text {
    font-size: 20px;
}
.row {
    width: 98%;
    margin-top: 5px;
    flex-direction: row;
}
.label {
    font-size: 13px;
    width: 120px;
    text-align: right;
}
.input-block {
    width: 100%;
}
.input {
    width: 90%;
    font-size: 12px;
}
.password-tip {
    font-size: 10px;
}
.sex {
    font-size: 10px;
    margin-right: 10px;
}
.favorite {
    font-size: 10px;
}
.button {
    margin-top: 70px;
    width: 150px;
    background-color: #17A98E;
}
```

# 为组件添加响应事件<a name="ZH-CN_TOPIC_0000001232001867"></a>

-   [数据定义]
-   [事件]

## 数据定义<a name="section191691020144313"></a>

在前面的步骤中，我们已经提到了布局中组件初始值的定义，在index.js中首先添加如下代码：

```
import prompt from '@system.prompt';

export default {
  data: {
    require: '/common/images/require.png',
    username: '',
    password: '',
    password2: '',
    email: '',
    date: '',
    height: '',
    favorite: [],
    list: [{icon: '/common/images/require.png', content: '选项0'},
      {icon: '/common/images/require.png', content: '选项1'}]
  },
}  
```

这些值的具体含义，可以参考如下表格：

<a name="table10799439202717"></a>

<table><thead align="left"><tr id="row7799183932719"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p13799163942713"><a name="p13799163942713"></a><a name="p13799163942713"></a>key</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p1979920394275"><a name="p1979920394275"></a><a name="p1979920394275"></a>description</p>
</th>
</tr>
</thead>
<tbody><tr id="row167993390271"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1456225735310"><a name="p1456225735310"></a><a name="p1456225735310"></a>require</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1462164155910"><a name="p1462164155910"></a><a name="p1462164155910"></a>表示必填项的红色标识</p>
</td>
</tr>
<tr id="row7800173942712"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p05141039543"><a name="p05141039543"></a><a name="p05141039543"></a>username</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p180003913276"><a name="p180003913276"></a><a name="p180003913276"></a>用户名</p>
</td>
</tr>
<tr id="row3800163922714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p6482108105419"><a name="p6482108105419"></a><a name="p6482108105419"></a>password</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p16800103917279"><a name="p16800103917279"></a><a name="p16800103917279"></a>密码</p>
</td>
</tr>
<tr id="row880093915275"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p121836168415"><a name="p121836168415"></a><a name="p121836168415"></a>passwordConfirm</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p813625117550"><a name="p813625117550"></a><a name="p813625117550"></a>确认密码</p>
</td>
</tr>
<tr id="row280083914277"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1238213216546"><a name="p1238213216546"></a><a name="p1238213216546"></a>email</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p5800639122712"><a name="p5800639122712"></a><a name="p5800639122712"></a>电子邮箱</p>
</td>
</tr>
<tr id="row68001939122713"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p15315102725416"><a name="p15315102725416"></a><a name="p15315102725416"></a>date</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1969117263561"><a name="p1969117263561"></a><a name="p1969117263561"></a>出生日期</p>
</td>
</tr>
<tr id="row6800539192714"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p2090310341549"><a name="p2090310341549"></a><a name="p2090310341549"></a>height</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p380083911278"><a name="p380083911278"></a><a name="p380083911278"></a>身高</p>
</td>
</tr>
<tr id="row18006395275"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p19791124318548"><a name="p19791124318548"></a><a name="p19791124318548"></a>hobby</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p17511740125615"><a name="p17511740125615"></a><a name="p17511740125615"></a>爱好</p>
</td>
</tr>
<tr id="row380073992718"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1121565035417"><a name="p1121565035417"></a><a name="p1121565035417"></a>list</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p168001239142710"><a name="p168001239142710"></a><a name="p168001239142710"></a>身高input输入框中menuoptions事件的值</p>
</td>
</tr>
</tbody>
</table>



## 事件<a name="section2363155714436"></a>

接下来在export default的data对象后面继续添加事件代码，当文本框内容改变、长按文本框时触发如下事件：

```
// 文本框内容发生变化
change(e) {
	var idName = e.target.id;
	if (idName === "username") {
		this.username = e.value;
	} else if (idName === "password") {
		this.password = e.value;
	} else if (idName === "password2") {
		this.password2 = e.value;
	} else if (idName === "email") {
		this.email = e.value;
	} else if (idName === "date") {
		this.date = e.value;
	} else if (idName === "height") {
		this.height = e.value;
	}
},
// 复选框修改
checkboxOnChange(e) {
	var value = e.target.attr.value
	if (e.checked) {
		this.favorite.push(value);
	} else {
		this.favorite.splice(this.favorite.findIndex(e => e === value), 1);
	}
},
// 提交
buttonClick() {
	if (this.username === "") {
		this.showPrompt('用户名不能为空');
	} else if (this.password === "") {
		this.showPrompt('密码不能为空');
	} else if (this.password !== "" && this.password !== this.password2) {
		this.showPrompt('两次密码输入不一致');
	} else if (this.email === "") {
		this.showPrompt('邮箱不能为空');
	} else if (this.favorite.length === 0) {
		this.showPrompt('请至少选择一个爱好');
	} else {
		this.showPrompt('提交成功');
	}
},
//弹框
showPrompt(msg) {
	prompt.showToast({
		message: msg,
		duration: 3000,
	});
},
// 进行文本选择操作后文本选择弹窗会出现翻译按钮
translate(e) {
	this.showPrompt(e.value);
},
//进行文本选择操作后文本选择弹窗会出现分享按钮
share(e) {
	this.showPrompt(e.value);
},
// 进行文本选择操作后文本选择弹窗会出现查询按钮
search(e) {
	this.showPrompt(e.value);
},
// 用户在文本选择操作后，点击菜单项后触发该回调
optionselect(e) {
	this.showPrompt("选项" + e.index + ": " + e.value);
}
```

到此我们已经完成了所有的任务和代码的编写。

>![](public_sys-resources/icon-note.gif) **说明：** 
>样例中我们使用了一张input输入框必填标识图片，它放在js/default/common.images目录下，命名为require.png

# 恭喜您<a name="ZH-CN_TOPIC_0000001186722184"></a>

在本篇Codelab中，我们主要为大家讲解了如下ArkUI（基于JS扩展的类Web开发范式）基础组件

-   input

通过一个代码示例，实现input组件表单，从布局、样式、响应事件三个层面，逐步为大家进行代码讲解。希望通过本教程，各位开发者可以对以上基础组件具有更深刻的认识。