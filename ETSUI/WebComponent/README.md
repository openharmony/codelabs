# Web组件加载本地H5小程序

# 介绍

本Codelab是一个OpenHarmony应用，使用具有网页显示能力的web组件，加载一个本地H5界面。所加载的界面是一个由HTML+CSS+JavaScript实现的完整小应用。本案例旨在向开发者展示OpenHarmony如何通过web组件加载完成的H5小程序，以及如何通过run JavaScript直接调用H5界面的JavaScript函数，实现eTS和H5的简单交互，简化OpenHarmony集成H5小程序的过程。

**运行效果：**

![](figures/GIF-2022-5-6-17-53-58.gif)

# 相关概念

[web组件](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/arkui-ts/ts-basic-components-web.md#loadurl)：提供具有网页显示能力的Web组件。

# 开发环境

-   IDE版本：3.0.0.900
-   项目类型：eTS语言、API9、stage模型
-   调试设备：RK3568

# 相关权限

加载web资源需要在config.json中配置如下权限：

```
"requestPermissions": [
  {
     name: "ohos.permission.INTERNET"
  }
]
```

# H5小程序

抽奖小程序由HTML+CSS+JS实现，主要的代码展示：

1.  抽奖盘html

    利用无序列表元素组合一个九宫格，中间一格为抽奖格，外围八格为奖品格。奖品格的图片src路径后续通过JS动态添加。

    ```
    <div class="luckyDraw">
        <h3 class="finishText" style="text-align: center;display: none;">恭喜你完成抽奖活动</h3>
        <!-- 使用无序列表实现抽奖盘 -->
        <ul class="prizes">
            <li class="prizes-li active"><img class="pic" src="#"></li>
            <li class="prizes-li"><img class="pic" src="#"></li>
            <li class="prizes-li"><img class="pic" src="#"></li>
            <li class="prizes-li"><img class="pic" src="#"></li>
            <li class="prizes-li"><img class="pic" src="#"></li>
            <li class="prizes-li"><img class="pic" src="#"></li>
            <li class="prizes-li"><img class="pic" src="#"></li>
            <li class="prizes-li"><img class="pic" src="#"></li>
            <li class="prizes-li"><span class="trigger" onclick="start()">点击抽奖</span></li>
            <!-- 奖品名称显示区 -->
            <li><span id="prizeText">奖品</span></li>
        </ul>
    </div>
    ```

2.  抽奖盘CSS

    通过绝对定位的方式，可以快速的将一个无序列表变为九宫格效果。

    ```
    /* 抽奖div */
    .luckyDraw {
        width: 480px;
        margin: 0 auto;
    }
    
    /* 抽奖列表 */
    .prizes {
        width: 480px;
        margin: 30px auto;
        position: relative;
    }
    
    /* 奖品格子 */
    .prizes li {
        width: 160px;
        height: 160px;
        box-sizing: border-box;
        text-align: center;
        line-height: 160px;
        position: absolute;
        border-radius: 25px;
        padding: 10px,10px;
    }
    
    /* 奖品格子位置 */
    .prizes li:nth-of-type(1) {
        left: 0;
        top: 0;
    }
    ....
    ```

3.  抽奖盘JS

    预先随机好结果，通过可变速的旋转动作，旋转指定的圈数后，最后落在指定位置。运行时的旋转效果通过给元素动态添加CSS效果来实现。

    ```
    let i = 0; //转到哪个位置
    let count = 0; //转圈初始值
    let totalCount = 8; //至少转动的总圈数
    let speed = 500; //转圈速度，值越大越慢
    let index = 3; //转到哪个奖品
    
    // 旋转函数，预先随机好结果，通过可变速的旋转动作，到达指定的圈数后，最后落在指定位置
    function roll() {
        //速度衰减
        speed -= 50;
        if (speed <= 10) {
            speed = 10;
        }
        //每次调用都去掉全部active类名
        for (var j = 0;j < ali.length; j++) {
            ali[j].classList.remove('active');
        }
        i++;
        //计算转圈次数
        if (i >= ali.length - 1) {
            i = 0;
            count++;
        }
        prizeText.innerHTML = arr[i]; //显示当前奖品名称
        ali[i].classList.add('active'); //添加激活类名，给奖品加样式
        //满足转圈数和指定位置就停止
        if (count >= totalCount && (i + 1) == index) {
            clearTimeout(timer);
            isClick = true;
            speed = initSpeed;
            dialogText.innerHTML = "恭喜获得 " + arr[i] + " 请填写收件地址";
            dialogImg.src = arrImg[i];
            timer = setTimeout(openDialog, 1000); // 等待1s打开弹窗
        } else {
            timer = setTimeout(roll, speed); //不满足条件时调用定时器
            //最后一圈减速
            if (count >= totalCount - 1 || speed <= 50) {
                speed += 100;
            }
        }
    }
    // 抽奖开始函数
    function start() {
        finishText.style.display = "none";
        // 防止抽奖多次触发
        if (isClick) {
            count = 0;
            //随机产生中奖位置
            index = Math.floor(Math.random() * arr.length + 1);
            roll();
            isClick = false;
        }
    }
    ```

# web组件

通过web组件不仅可以在OpenHarmony设备上展示H5小程序的界面，也可以通过runJavaScript\(\)执行小程序的JS函数，来实现eTS与H5小程序的交互功能。

web组件代码：

```
@Entry
@Component
struct Index {
  // web组件控制器可以控制Web组件各种行为
  webController: WebController = new WebController()

  build() {
    Row() {
      Column() {
        Text("Web组件外")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        Button("点击抽奖")
          .fontSize(40)
          .height("8%")
          .width("30%")
          .onClick(() => {
            this.webController.runJavaScript({ script: 'start()' }); // 运行web加载的本地JS函数
            console.log("点击抽奖按钮")
          })
        Text("-----------------------------")
          .fontSize(50)
        Text("Web组件内")
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        // web组件加载本地H5
        Web({ src: $rawfile('index.html'), controller: this.webController })
          // 设置是否开启通过$rawfile(filepath/filename)访问应用中rawfile路径的文件， 默认启用。
          .fileAccess(true)
          // 设置是否允许自动加载图片资源，默认允许。
          .imageAccess(true)
          // 设置是否允许执行JavaScript脚本，默认允许执行。
          .javaScriptAccess(true)
        .backgroundColor(Color.Pink)
      }
      .width('100%')
    }
    .height('100%')
  }
}
```

# 恭喜您

通过本篇Codelab，您可以学到：

1、Web组件加载本地H5小程序。

2、Web组件运行本地H5小程序的JS函数。