# 介绍

我们备份手机联系人时，会导出到SD卡，在SD卡中生成一个vcf文件，用于保存联系人姓名，手机号码。vCard 规范容许公开交换个人数据交换 (Personal Data Interchange PDI)信息，在传统纸质商业名片可找到这些信息。规范定义电子名片（或叫vCard）的格式。实现VCard进行联系人备份需要引用第三方库、下面介绍VCard使用。

# 源码

[三方库源码](https://gitee.com/openharmony-tpc/VCard)

要想读懂源码，需要具备以下技能。

-   [熟悉基于TS扩展的声明式开发](https://developer.harmonyos.com/cn/docs/documentation/doc-references/reference-document-outline-0000001115016824)
-   熟悉ES6

# 搭建OpenHarmony环境

完成本篇Codelab我们首先要完成开发环境的搭建，本示例以**Hi3516DV300**开发板为例，参照以下步骤进行：

1.  [获取OpenHarmony系统版本](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。

    以3.0版本为例：

    ![](figures/取版本.png)

2.  搭建烧录环境。

    1.  [完成DevEco Device Tool的安装](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-standard-env-setup.md)

    2.  [完成Hi3516开发板的烧录](https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-lite-steps-hi3516-burn.md)

3.  搭建开发环境。

    1.  开始前请参考[下载与安装软件](https://developer.harmonyos.com/cn/docs/documentation/doc-guides/software_install-0000001053582415)、[配置开发环境](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/configuring-openharmony-sdk.md)，完成DevEco Studio的安装和开发环境配置。
    2.  开发环境配置完成后，请参考[使用工程向导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/use-wizard-to-create-project.md)创建工程，选择JS或者eTS语言开发、“Application”为例，模板选择“\[Standard\]Empty Ability”。
    3.  工程创建完成后，可参考下面章节进行代码编写，使用真机进行调测：

    -   [配置OpenHarmony应用签名信息](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/configuring-openharmony-app-signature.md)
    -   [hap包安装指导](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/installing-openharmony-app.md)
    -   工程示例：

    ![](figures/截图.png)

# 添加依赖

工程创建完成后，在项目根目录下存在 package.json文件，根目录下运行命令。

配置仓库：npm config set @ohos:registry=https://repo.harmonyos.com/npm/

安装依赖：npm install @ohos/vcard

# 具体用法

1.  导入类

    ```
    import router from "@system.router";
    import {contact} from "@ohos/vcard";
    import {VCardBuilder} from "@ohos/vcard";
    import {VCardConfig} from "@ohos/vcard";
    ```

2.  Vcard三方库可添加个个人信息，比如姓名、手机、邮箱、备注等等。根据个人信息生成规范定义电子名片，在此介绍下添加姓名，添加其他信息可参考[源码](源码.md)。

    ```
    let myCard= new  contact.Contact()
    let nickName = new contact.NickName;
    nickName.nickName="昵称小张"
    myCard.nickName = nickName
    let builder = new VCardBuilder(VCardConfig.VCARD_TYPE_V21_GENERIC,"UTF-8");
    builder.appendNickNames(myCard)
    let result = builder.toString()
    ```

3.  生成结果

    ```
    BEGIN:VCARD VERSION:2.1 X-ANDROID-CUSTOM;CHARSET=UTF-8;
    ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/nickname;
    =e6=98=b5=e7=a7=b0=e5=b0=8f=e5=bc=a0;;;;;;;;;;;;;; END:VCAR
    ```

# 恭喜你

通过本篇Codelab，您可以学到：

openHarmony三方库使用。

# 参考

gitee地址