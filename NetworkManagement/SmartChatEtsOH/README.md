# 项目介绍<a name="ZH-CN_TOPIC_0000001252682182"></a>

本篇Codelab使用基于HTTP协议和服务端建立连接，实现了一个和智能机器人进行通信对话的聊天案例。在这个案例中，客户端可以给服务端发送聊天信息，服务端接收到请求后会对客户端进行自动回复。案例效果如下所示：

![](figures/20220520-153245(WeLinkPC).jpg)
# 搭建OpenHarmony环境<a name="ZH-CN_TOPIC_0000001299043153"></a>

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**RK3568**开发板为例，参照以下步骤进行：

1.  [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。

    以3.1版本为例：

    ![](figures/zh-cn_image_0000001306366081.png)

2.  搭建烧录环境。
    1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-standard-env-setup.md)
    2.  [完成RK3568开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-standard-running-rk3568-burning.md)

3.  搭建开发环境。
    1.  开始前请参考[工具准备](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#%E5%88%9B%E5%BB%BAets%E5%B7%A5%E7%A8%8B)创建工程（模板选择“Empty Ability”），选择JS或者eTS语言开发。
    3.  工程创建完成后，选择使用[真机进行调测](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-stage.md#%E4%BD%BF%E7%94%A8%E7%9C%9F%E6%9C%BA%E8%BF%90%E8%A1%8C%E5%BA%94%E7%94%A8)。
# 数据请求API介绍<a name="ZH-CN_TOPIC_0000001299083729"></a>

<a name="table15553881214"></a>
<table><thead align="left"><tr id="row55738111213"><th class="cellrowborder" valign="top" width="20.02%" id="mcps1.1.3.1.1"><p id="p1661038121219"><a name="p1661038121219"></a><a name="p1661038121219"></a>方法</p>
</th>
<th class="cellrowborder" valign="top" width="79.97999999999999%" id="mcps1.1.3.1.2"><p id="p3498151051317"><a name="p3498151051317"></a><a name="p3498151051317"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="row36738171213"><td class="cellrowborder" valign="top" width="20.02%" headers="mcps1.1.3.1.1 "><p id="p96338151220"><a name="p96338151220"></a><a name="p96338151220"></a>creatHttp</p>
</td>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.2 "><p id="p472214518119"><a name="p472214518119"></a><a name="p472214518119"></a>创建一个HTTP网络请求，包括发起请求、中断请求、订阅/取消订阅HTTP Response Header 事件。</p>
</td>
</tr>
<tr id="row96938101213"><td class="cellrowborder" valign="top" width="20.02%" headers="mcps1.1.3.1.1 "><p id="p13623881219"><a name="p13623881219"></a><a name="p13623881219"></a>request</p>
</td>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.2 "><p id="p106133810129"><a name="p106133810129"></a><a name="p106133810129"></a>发送请求。</p>
</td>
</tr>
<tr id="row16163818122"><td class="cellrowborder" valign="top" width="20.02%" headers="mcps1.1.3.1.1 "><p id="p933315114215"><a name="p933315114215"></a><a name="p933315114215"></a>destroy</p>
</td>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.2 "><p id="p66163891213"><a name="p66163891213"></a><a name="p66163891213"></a>中断请求。</p>
</td>
</tr>
<tr id="row1411636182015"><td class="cellrowborder" valign="top" width="20.02%" headers="mcps1.1.3.1.1 "><p id="p12153612202"><a name="p12153612202"></a><a name="p12153612202"></a>on(headerReceive)</p>
</td>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.2 "><p id="p20283612208"><a name="p20283612208"></a><a name="p20283612208"></a>订阅HTTP Response Header 事件。</p>
</td>
</tr>
<tr id="row1143013352110"><td class="cellrowborder" valign="top" width="20.02%" headers="mcps1.1.3.1.1 "><p id="p643011317214"><a name="p643011317214"></a><a name="p643011317214"></a>off(headerReceive)</p>
</td>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.2 "><p id="p13329218102113"><a name="p13329218102113"></a><a name="p13329218102113"></a>取消订阅HTTP Response Header 事件。</p>
</td>
</tr>
<tr id="row20394187142113"><td class="cellrowborder" valign="top" width="20.02%" headers="mcps1.1.3.1.1 "><p id="p939410712113"><a name="p939410712113"></a><a name="p939410712113"></a>once(headerReceive)</p>
</td>
<td class="cellrowborder" valign="top" width="79.97999999999999%" headers="mcps1.1.3.1.2 "><p id="p6401134920210"><a name="p6401134920210"></a><a name="p6401134920210"></a>订阅HTTP Response Header 事件，但只触发一次。</p>
</td>
</tr>
</tbody>
</table>

>![](public_sys-resources/icon-note.gif) **说明：**
>相关API可以参考：[数据请求](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis/js-apis-http.md)
# 代码结构解读<a name="ZH-CN_TOPIC_0000001252683130"></a>
```
├─AppScope		        # app全局目录
│  └─resources
│      └─base
│          ├─element
│          └─media
└─entry				# entry目录
    └─src
        └─main
            ├─ets		# ets代码
            │  ├─Application
            │  ├─MainAbility
            │  └─pages
            └─resources		# 资源目录
                └─base
                    ├─element
                    ├─media
                    └─profile
```

-   AppScope：App作用域目录。
-   entry/src/main/ets：程序目录。
    -   Application：stage模型目录。
        -   AbilityStage.ts：stage模型文件。

    -   MainAbility：程序入口目录。
        -   MainAbility.ts：程序入口类。

    -   pages：存放应用页面。
        -   ChatPage.ets：聊天界面。


-   entry/src/main/resources：资源文件目录。
-   module.json5：应用配置文件，包含网络权限配置。
# 相关权限<a name="ZH-CN_TOPIC_0000001252523878"></a>

本篇Codelab需要在module.json5中配置如下权限：

```
"reqPermissions": [
  {
    "name": "ohos.permission.INTERNET"
  }
]
```
# 聊天界面实现<a name="ZH-CN_TOPIC_0000001299083929"></a>

本节将介绍聊天界面布局的实现，效果图如下：

![](figures/20220520-153245(WeLinkPC)-0.jpg)

界面由三部分组成：

1.  顶部的Text组件展示标题。

    ```
    @Component
    struct Title {
      private title: string
      build() {
        Flex({ alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
          Text(this.title).fontSize(30).fontColor("#fdfdfd")
        }.height(80).backgroundColor("#333534")
      }
    }
    ```

2.  中间的Image组件展示用户头像；Text组件展示聊天内容。

    ```
    // 左侧机器人头像和聊天信息
    @Component
    struct LeftDialogBox {
      private imageSrc: string
      private chatMsg: string
      build() {
        Row() {
          Image($rawfile(this.imageSrc)).width(80).height(80).margin({ left: 10, right: 10 })
          Text(this.chatMsg)
            .fontSize(25)
            .backgroundColor("#d7f6f3f3")
            .padding(10)
            .borderRadius(10)
        }
        .width('100%')
        .padding({ top: 20, bottom: 20 , right: 280})
        .alignItems(VerticalAlign.Top)
      }
    }
    // 右侧本人头像和聊天信息
    @Component
    struct RightDialogBox {
      private imageSrc: string
      private chatMsg: string
      build() {
        Row() {
          Image($rawfile(this.imageSrc)).width(80).height(80).margin({ left: 10, right: 10 })
          Text(this.chatMsg)
            .fontSize(25)
            .backgroundColor("#d7f6f3f3")
            .padding(10)
            .borderRadius(10)
        }
        .width('100%')
        .padding({ top: 20, bottom: 20 , left: 280})
        .alignItems(VerticalAlign.Top)
        .direction(Direction.Rtl)
      }
    }
    ```

3.  底部左侧的TextInput组件用于输入聊天信息，底部右侧的Button组件发送聊天信息。

    ```
    Row() {
      TextInput({ placeholder: '', text: '' })
        .height(60)
        .fontSize(30)
        .margin(10)
        .width('80%')
        .onChange((value: string) => {
          this.sendMsg = value
        })
      Button("发送").height(50)
        .width(100)
        .margin(10)
        .onClick(() => {
          ...
        })
    }
    ```
# 使用HTTP实现和服务端的通信<a name="ZH-CN_TOPIC_0000001309998149"></a>

本节将介绍在聊天界面中，如何使用HTTP和服务端进行通信。

1.  导入http模块，创建HTTP请求，用于发起请求、中断请求、订阅/取消订阅HTTP Response Header 事件。

    ```
    // 导入http模块
    import http from '@ohos.net.http';
    // 创建HTTP请求
    let httpRequest = http.createHttp();
    ```

2.  给“发送”按钮绑定点击事件，调用智能机器人API接口实现发送消息功能。

    ```
    Button("发送").height(50)
      .width(100)
      .margin(10)
      .onClick(() => {
        let sendMsg = "http://******" + this.sendMsg // 用户可根据需要，选择合适的智能机器人聊天API进行测试
        this.sendMsg = ""
        ...
        // 发起HTTP网络请求
        httpRequest.request(sendMsg,
            {
            method: http.RequestMethod.GET,
            header: {
                'Content-Type': 'application/json'
            },
            readTimeout: 60000,
            connectTimeout: 60000
            }, (err, data) => {
                if (!err) {
                    console.info('httpRequest:' + data.result);
                    // 请求响应
                    this.msgArr.push("1-" + JSON.parse(data.result)
                    .content)
                    this.scroller.scrollPage({ next: true })
                } else {
                    console.info('httpRequest:' + err.data);
                }
            });
      }
    
    ```
# 总结与回顾<a name="ZH-CN_TOPIC_0000001252364114"></a>

本篇Codelab介绍如何基于HTTP协议及相关API实现一个简单的智能聊天机器人案例。
# 恭喜您<a name="ZH-CN_TOPIC_0000001299203385"></a>

通过本篇Codelab的学习，您已经学会了：

基于HTTP协议发起网络请求相关API的使用。