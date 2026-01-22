应用账号密码管理APP

## 介绍

在MyAccountApp 中，我们构建了一个具有用户注册、登录、账号密码增删改查以及密码安全检测功能的简易APP。

- 图1：添加账号密码

  ![添加账号](./photo/1.gif)

- 图2：编辑账号

  ![账号](./photo/3.gif)

## 搭建 OpenHarmony 环境

### 软件要求

- [DevEco Studio](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87) 版本：DevEco Studio 6.0.0
- OpenHarmony SDK 版本：API version 20

### 硬件要求

- 开发板类型：[润和 DAYU200 开发板](https://gitcode.com/hihope/DAYU200/blob/main/README.md)
- OpenHarmony 系统：6.0

## 代码结构解读

本篇 Codelab 只对核心代码进行讲解，对于完整代码，我们会在 gitcode 中提供。

```typescript
├──entry/src/main/ets                     // 代码区
│  ├──entryability
│  │  └──EntryAbility.ets                // 程序入口类
│  ├──model
│  │  ├──User.ets                        // 用户实体类
│  │  └──AppAccount.ets                  // 应用账号实体类
│  ├──database
│  │  └──RdbHelper.ets                   // 关系型数据库帮助类
│  ├──utils
│  │  └──PreferenceUtil.ets              // 自动登录
│  ├──pages
│  │  ├──LoginPage.ets                   // 登录页面
│  │  ├──RegisterPage.ets                // 注册页面
│  │  ├──Index.ets                       // 主页面（密码列表）
│  │  ├──AccountInputPage.ets            // 添加/编辑账号页面
│  │  ├──SettingsPage.ets                // 设置页面
│  │  └──ForgotPasswordPage                 // 忘记密码页面
```

## 用户登录页面

用户登录页面主要由三部分组成：标题区域、输入区域和操作按钮区域。

### 标题区域

使用 Text 组件显示应用名称 "Account Manager"，设置字体大小为 24，加粗显示，并设置顶部和底部边距。

```typescript
Text('Account Manager')
  .fontSize(24)
  .fontWeight(FontWeight.Bold)
  .fontColor(0x333333)
  .margin({ top: 80, bottom: 50 })
```

### 输入区域

包含用户名和密码两个输入框，使用 TextInput 组件实现。密码输入框通过设置 `type(InputType.Password)` 实现密码隐藏显示。输入框使用圆角矩形样式，背景色为浅灰色。

```typescript
Column({ space: 15 }) {
  // 用户名
  TextInput({ placeholder: '请输入用户名' })
    .width('100%')
    .height(50)
    .backgroundColor(0xF5F5F5)
    .borderRadius(25)
    .padding({ left: 20 })
    .onChange((value: string) => {
      this.usernameStr = value;
    })

  // 密码
  TextInput({ placeholder: '请输入密码' })
    .width('100%')
    .height(50)
    .backgroundColor(0xF5F5F5)
    .borderRadius(25)
    .padding({ left: 20 })
    .type(InputType.Password)
    .onChange((value: string) => {
      this.passwordStr = value;
    })
}
.width('85%')
```

### 自动登录勾选

使用 Checkbox 组件实现自动登录功能，勾选后下次启动应用将自动登录。登录成功后，用户信息会保存到 Preferences 中。

```typescript
Row() {
  Row() {
    Checkbox({ name: 'remember', group: 'login' })
      .select(this.isRemember)
      .selectedColor(0x007DFF)
      .onChange((value: boolean) => {
        this.isRemember = value;
      })
    Text('下次自动登录')
      .fontSize(14)
      .fontColor(Color.Gray)
      .onClick(() => {
        this.isRemember = !this.isRemember;
      })
  }

  Blank()

  Text('忘记密码？')
    .fontSize(14)
    .fontColor(0x007DFF)
}
.width('80%')
.margin({ top: 15, bottom: 30 })
```

### 登录逻辑

点击登录按钮后，首先校验用户名和密码是否为空，然后查询数据库验证用户身份。若勾选了"自动登录"，则将用户信息保存到 Preferences。登录成功后跳转到主页并传递用户信息。

```typescript
handleLogin() {
  if (this.usernameStr === '' || this.passwordStr === '') {
    this.showToast('请输入用户名和密码');
    return;
  }

  if (!this.rdbHelper) {
    this.showToast('系统初始化中...');
    return;
  }

  // 查询数据库
  this.rdbHelper.queryUser(this.usernameStr).then((user: User | null) => {
    if (user) {
      // 用户存在，校验密码
      if (user.getPassword() === this.passwordStr) {
        this.showToast('登录成功！欢迎回来');

        // 保存自动登录信息
        if (this.isRemember) {
          PreferenceUtil.saveLoginInfo(user.getId(), user.getUsername());
        }

        // 登录成功，跳转到主页
        setTimeout(() => {
          router.replaceUrl({
            url: 'pages/Index',
            params: {
              username: user.getUsername(),
              userId: user.getId()
            }
          });
        }, 1000);

      } else {
        this.showToast('密码错误，请重试');
      }
    } else {
      this.showToast('用户不存在，请先注册');
    }
  });
}
```

## 用户注册页面

注册页面采用纵向 Column 布局，包含页面标题和表单输入区域。

### 表单设计

表单包含用户名、手机号、密码、确认密码四个输入项，使用 TextInput 组件实现，手机号输入框设置 `type(InputType.PhoneNumber)`，密码输入框设置 `type(InputType.Password)`。

```typescript
Column() {
  // 页面标题
  Text('欢迎注册')
    .fontSize(30)
    .fontWeight(FontWeight.Bold)
    .fontColor('#333333')
    .margin({ top: 50, bottom: 20 })
    .alignSelf(ItemAlign.Start)
    .padding({ left: 20 })

  Text('创建一个新账号以使用完整功能')
    .fontSize(16)
    .fontColor(Color.Gray)
    .margin({ bottom: 40 })
    .alignSelf(ItemAlign.Start)
    .padding({ left: 20 })

  // 用户名输入框
  TextInput({ placeholder: '请输入用户名' })
    .width('90%')
    .height(50)
    .backgroundColor('#F5F5F5')
    .borderRadius(10)
    .margin({ bottom: 15 })
    .padding({ left: 15 })
    .onChange((value: string) => {
      this.usernameStr = value;
    })

  // 手机号输入框
  TextInput({ placeholder: '请输入手机号' })
    .width('90%')
    .height(50)
    .backgroundColor('#F5F5F5')
    .borderRadius(10)
    .margin({ bottom: 15 })
    .padding({ left: 15 })
    .type(InputType.PhoneNumber)
    .onChange((value: string) => {
      this.phoneStr = value;
    })

  // 密码输入框
  TextInput({ placeholder: '请输入密码' })
    .width('90%')
    .height(50)
    .backgroundColor('#F5F5F5')
    .borderRadius(10)
    .margin({ bottom: 15 })
    .padding({ left: 15 })
    .type(InputType.Password)
    .onChange((value: string) => {
      this.passwordStr = value;
    })

  // 确认密码输入框
  TextInput({ placeholder: '请再次确认密码' })
    .width('90%')
    .height(50)
    .backgroundColor('#F5F5F5')
    .borderRadius(10)
    .margin({ bottom: 30 })
    .padding({ left: 15 })
    .type(InputType.Password)
    .onChange((value: string) => {
      this.confirmPasswordStr = value;
    })

  // 注册按钮
  Button('立即注册')
    .width('90%')
    .height(50)
    .backgroundColor(0x007DFF)
    .borderRadius(25)
    .fontSize(18)
    .onClick(() => {
      this.handleRegister();
    })
}
```

### 注册逻辑

注册时进行多重校验：用户名长度（至少2位）、手机号格式（11位）、两次密码一致性、用户名是否已存在。校验通过后创建 User 对象并插入数据库。

```typescript
handleRegister() {
  // 1. 校验用户名
  if (this.usernameStr === '' || this.usernameStr.length < 2) {
    this.showToast('请输入有效的用户名（至少2位）');
    return;
  }

  // 2. 校验手机号
  if (this.phoneStr.length !== 11) {
    this.showToast('请输入11位手机号码');
    return;
  }

  // 3. 校验密码
  if (this.passwordStr === '') {
    this.showToast('密码不能为空');
    return;
  }

  // 4. 校验确认密码
  if (this.passwordStr !== this.confirmPasswordStr) {
    this.showToast('两次输入的密码不一致');
    return;
  }

  // 5. 检查用户名是否存在
  this.rdbHelper.queryUser(this.usernameStr).then((existUser) => {
    if (existUser) {
      this.showToast('该用户名已被注册，请更换');
    } else {
      this.performInsert();
    }
  });
}

performInsert() {
  // 创建新用户对象
  let newUser = new User(this.usernameStr, this.passwordStr, this.phoneStr);

  // 调用数据库插入
  this.rdbHelper.insertUser(newUser).then((rowId) => {
    if (rowId > 0) {
      this.showToast('注册成功！请登录');
      setTimeout(() => {
        router.back();
      }, 1000);
    } else {
      this.showToast('注册失败，请重试');
    }
  });
}
```

## 密码管理主页面

主页面由三部分组成：顶部栏（含搜索框）、安全评分卡片和账号列表。

### 顶部栏

顶部栏使用蓝色背景，包含标题 "我的密码本"、欢迎语、设置按钮和搜索框。搜索框使用 Search 组件，支持实时过滤账号列表。

```typescript
Column() {
  Row() {
    Text('我的密码本').fontSize(22).fontWeight(FontWeight.Bold).fontColor(Color.White)
    Blank()

    // 设置按钮
    Row() {
      Text('⚙️ 设置')
        .fontSize(16).fontWeight(FontWeight.Bold).fontColor(Color.White)
    }
    .onClick(() => {
      router.pushUrl({ url: 'pages/SettingsPage' });
    })
  }.width('100%').margin({ bottom: 10 })

  Text(`欢迎你，${this.currentUsername}`)
    .fontSize(14).fontColor(Color.White).opacity(0.8).width('100%').margin({ bottom: 15 })

  Search({ value: this.searchText, placeholder: '搜索平台或账号...' })
    .width('100%').height(40).backgroundColor(Color.White).borderRadius(20)
    .onChange((value: string) => { this.searchText = value; this.filterData(); })
}
.width('100%').height(160).backgroundColor(0x007DFF).padding(20)
```

### 搜索过滤逻辑

搜索功能通过 `filterData()` 方法实现，支持按平台名称或账号名过滤，不区分大小写。

```typescript
filterData(): void {
  let keyword = this.searchText.trim().toLowerCase();
  if (keyword === '') {
    this.accountList = this.allAccounts;
  } else {
    this.accountList = this.allAccounts.filter((item) => {
      return item.getPlatformName().toLowerCase().includes(keyword) ||
      item.getAccountName().toLowerCase().includes(keyword);
    });
  }
}
```

### 密码健康评分卡片

系统自动分析所有密码的安全性，检测弱密码（长度<6 或纯数字/纯字母）和重复密码，计算安全评分。卡片颜色根据评分动态变化：绿色（≥90）、黄色（≥60）、红色（<60）。

```typescript
@Builder SecurityCard() {
  Column() {
    Row() {
      Column() {
        Text('安全评分')
          .fontSize(14).fontColor(Color.White).opacity(0.9)
        Text(`${this.securityScore}`)
          .fontSize(40).fontWeight(FontWeight.Bold).fontColor(Color.White)
          .margin({ top: 5 })
      }.alignItems(HorizontalAlign.Start)

      Blank()

      // 右侧状态
      Column({ space: 8 }) {
        Row() {
          Text('⚠️ 弱密码: ').fontSize(12).fontColor(Color.White).opacity(0.9)
          Text(`${this.weakCount}`).fontSize(14).fontWeight(FontWeight.Bold)
            .fontColor(this.weakCount > 0 ? 0xFFC107 : Color.White)
        }
        Row() {
          Text('♻️ 重复项: ').fontSize(12).fontColor(Color.White).opacity(0.9)
          Text(`${this.duplicateCount}`).fontSize(14).fontWeight(FontWeight.Bold)
            .fontColor(this.duplicateCount > 0 ? 0xFF5252 : Color.White)
        }
      }.alignItems(HorizontalAlign.End)
    }
    .width('100%')
  }
  .width('100%')
  .padding(20)
  .backgroundColor(this.securityScore >= 90 ? 0x07C160 : (this.securityScore >= 60 ? 0xFFC107 : 0xFF5252))
  .borderRadius(16)
  .shadow({ radius: 10, color: 0x33000000, offsetY: 5 })
  .margin({ bottom: 20 })
}
```

### 密码健康分析算法

分析算法检测弱密码和重复密码，并根据检测结果扣分。弱密码每个扣10分，重复密码每个扣15分。

```typescript
analyzeSecurity(list: Array<AppAccount>): void {
  let weak = 0;
  let score = 100;

  // 1. 检查重复密码
  const pwdMap = new Map<string, number>();
  let duplicates = 0;

  list.forEach(item => {
    const pwd = item.getPassword();

    // 统计重复
    if (pwdMap.has(pwd)) {
      let count = pwdMap.get(pwd);
      if (count !== undefined) {
        pwdMap.set(pwd, count + 1);
      }
    } else {
      pwdMap.set(pwd, 1);
    }

    // 检查弱密码 (长度<6 或 纯数字/纯字母)
    const isShort = pwd.length < 6;
    const isSimple = /^[0-9]+$/.test(pwd) || /^[a-zA-Z]+$/.test(pwd);
    if (isShort || isSimple) {
      weak++;
    }
  });

  // 计算重复数
  pwdMap.forEach((count) => {
    if (count > 1) duplicates += (count - 1);
  });

  // 扣分逻辑
  score -= (weak * 10);
  score -= (duplicates * 15);
  if (score < 0) score = 0;

  // 更新状态
  this.weakCount = weak;
  this.duplicateCount = duplicates;
  this.securityScore = score;
}
```

### 账号列表

使用 List 组件展示账号列表，每个列表项包含平台头像、平台名、账号和密码（支持显示/隐藏切换）。列表项支持左滑显示编辑和删除按钮。

```typescript
List({ space: 12 }) {
  // 添加按钮
  ListItem() {
    Button('+ 添加新账号')
      .width('100%').height(45).backgroundColor(Color.White).fontColor(0x007DFF)
      .border({ width: 1, color: 0x007DFF })
      .onClick(() => {
        router.pushUrl({ url: 'pages/AccountInputPage', params: { userId: this.currentUserId } });
      })
  }.margin({ top: 15, bottom: 5 })

  // 安全评分卡片
  if (this.allAccounts.length > 0) {
    ListItem() {
      this.SecurityCard()
    }
  }

  // 账号列表
  ForEach(this.accountList, (item: AppAccount) => {
    ListItem() {
      Row() {
        // 平台头像
        Stack({ alignContent: Alignment.Center }) {
          Circle({ width: 50, height: 50 }).fill(this.getAvatarColor(item.getPlatformName()))
          Text(this.getAvatarText(item.getPlatformName()))
            .fontSize(20).fontColor(Color.White).fontWeight(FontWeight.Bold)
        }.margin({ right: 15 })

        Column() {
          // 平台名
          Row() {
            Text(item.getPlatformName()).fontSize(18).fontWeight(FontWeight.Bold).fontColor(0x333333).layoutWeight(1)
            Text('🚀').fontSize(16).backgroundColor(0xF0F8FF).padding(8).borderRadius(20)
              .onClick(() => this.jumpToTargetApp(item.getPlatformName(), item.getPassword()))
          }.width('100%').margin({ bottom: 8 })

          // 账号
          Text(item.getAccountName()).fontSize(14).fontColor(Color.Gray).width('100%').margin({ bottom: 5 })

          // 密码（点击切换显示/隐藏）
          Row() {
            if (this.isVisibleMap[item.getId()]) {
              Text(item.getPassword()).fontSize(16).fontColor(0x007DFF).fontWeight(FontWeight.Bold)
            } else {
              Text('••••••').fontSize(16).fontColor(Color.Black).letterSpacing(2)
            }
            Blank()
            Text(this.isVisibleMap[item.getId()] ? '🙈' : '👁️').fontSize(18).padding(5)
              .onClick(() => this.toggleVisibility(item.getId()))
          }.width('100%')
        }.layoutWeight(1)
      }
      .padding(15).backgroundColor(Color.White).borderRadius(12)
      .shadow({ radius: 5, color: 0xDDDDDD, offsetX: 2, offsetY: 2 })
    }
    .swipeAction({ end: this.SwipeMenu(item) })
  })
}
```

## 增加账号

### 添加账号界面设计

添加账号页面包含顶部导航栏和表单区域（平台名、账号、密码），并实时显示密码强度指示条。

```typescript
Column() {
  // 标题栏
  Row() {
    Text('< 返回').fontSize(16).fontColor(Color.Gray).onClick(() => this.safeBack())
    Blank()
    Text(this.editAccountId > 0 ? '编辑账号' : '添加新账号').fontSize(18).fontWeight(FontWeight.Bold)
    Blank()
    Text('      ')
  }
  .width('100%').padding(15).backgroundColor(Color.White)

  Column({ space: 20 }) {
    // 平台
    Column({ space: 8 }) {
      Text('平台名称').fontSize(14).fontColor(Color.Gray).width('100%')
      TextInput({ text: this.platform, placeholder: '例如: 微信 / QQ' })
        .height(50).backgroundColor(0xF5F5F5).borderRadius(10)
        .onChange((val) => this.platform = val)
    }

    // 账号
    Column({ space: 8 }) {
      Text('账号 / 用户名').fontSize(14).fontColor(Color.Gray).width('100%')
      TextInput({ text: this.account, placeholder: '输入您的账号' })
        .height(50).backgroundColor(0xF5F5F5).borderRadius(10)
        .onChange((val) => this.account = val)
    }

    // 密码区域
    Column({ space: 8 }) {
      Text('密码').fontSize(14).fontColor(Color.Gray).width('100%')

      Row({ space: 10 }) {
        TextInput({ text: this.password, placeholder: '输入密码' })
          .layoutWeight(1).height(50).backgroundColor(0xF5F5F5).borderRadius(10)
          .onChange((val) => {
            this.password = val;
            this.checkStrength(val); // 实时检测密码强度
          })
        Button('🎲 生成')
          .height(50).fontSize(14).backgroundColor(0xF0F8FF).fontColor(0x007DFF).borderRadius(10)
          .onClick(() => {
            this.password = this.generateRandomPassword();
          })
      }.width('100%')

      // 密码强度指示条
      if (this.password.length > 0) {
        Row({ space: 5 }) {
          Row().height(4).layoutWeight(1).borderRadius(2)
            .backgroundColor(this.strengthLevel >= 1 ? this.strengthColor : 0xE0E0E0)
          Row().height(4).layoutWeight(1).borderRadius(2)
            .backgroundColor(this.strengthLevel >= 2 ? this.strengthColor : 0xE0E0E0)
          Row().height(4).layoutWeight(1).borderRadius(2)
            .backgroundColor(this.strengthLevel >= 3 ? this.strengthColor : 0xE0E0E0)
        }.width('100%').margin({ top: 5 })

        Text(this.strengthText)
          .fontSize(12)
          .fontColor(this.strengthColor)
          .width('100%')
          .textAlign(TextAlign.End)
      }
    }

    Button('保 存')
      .width('100%').height(50).backgroundColor(0x007DFF).borderRadius(25)
      .fontSize(18).margin({ top: 30 })
      .onClick(() => this.handleSave())
  }
  .padding(20).width('100%').layoutWeight(1).backgroundColor(Color.White)
}
```

### 密码强度检测算法

根据密码长度和字符复杂度计算强度等级（弱/中/强），并使用三段式进度条可视化展示。

```typescript
checkStrength(pwd: string) {
  if (pwd.length === 0) {
    this.strengthLevel = 0;
    this.strengthText = '';
    return;
  }

  let score = 0;
  // 1. 长度奖励
  if (pwd.length >= 6) score += 1;
  if (pwd.length >= 10) score += 1;

  // 2. 字符类型奖励
  const hasLower = /[a-z]/.test(pwd);
  const hasUpper = /[A-Z]/.test(pwd);
  const hasNum = /[0-9]/.test(pwd);
  const hasSpecial = /[^a-zA-Z0-9]/.test(pwd);

  if (hasLower) score += 1;
  if (hasUpper) score += 1;
  if (hasNum) score += 1;
  if (hasSpecial) score += 2; // 特殊符号权重高

  // 3. 评级逻辑
  if (score < 3) {
    this.strengthLevel = 1;
    this.strengthColor = Color.Red;
    this.strengthText = '弱 (建议加长或混合字符)';
  } else if (score < 5) {
    this.strengthLevel = 2;
    this.strengthColor = Color.Orange;
    this.strengthText = '中 (尚可)';
  } else {
    this.strengthLevel = 3;
    this.strengthColor = 0x00CC66;
    this.strengthText = '强 (非常安全)';
  }
}
```

### 随机密码生成

支持一键生成 12 位随机强密码，包含大小写字母、数字和特殊符号。

```typescript
generateRandomPassword(): string {
  const length = 12;
  const charset = 'ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnpqrstuvwxyz23456789!@#$%&*+?';
  let result = '';
  for (let i = 0; i < length; i++) {
    const randomIndex = Math.floor(Math.random() * charset.length);
    result += charset.charAt(randomIndex);
  }
  this.safeToast('已生成随机强密码');
  this.checkStrength(result);
  return result;
}
```

### 保存账号逻辑

根据是否存在 `editAccountId` 判断是新增还是编辑模式，调用对应的数据库方法。

```typescript
handleSave() {
  if (this.platform === '' || this.account === '' || this.password === '') {
    this.safeToast('请填写完整信息');
    return;
  }
  if (this.currentUserId === 0) {
    this.safeToast('用户状态异常，请重新登录');
    return;
  }

  let newAccount = new AppAccount(this.platform, this.account, this.password, this.currentUserId);

  if (this.rdbHelper) {
    if (this.editAccountId > 0) {
      // 编辑模式
      newAccount.setId(this.editAccountId);
      this.rdbHelper.updateAppAccount(newAccount).then((rows) => {
        if (rows >= 0) {
          this.safeToast('修改成功');
          setTimeout(() => this.safeBack(), 500);
        }
      });
    } else {
      // 新增模式
      this.rdbHelper.insertAppAccount(newAccount).then((rowId) => {
        if (rowId > 0) {
          this.safeToast('保存成功');
          setTimeout(() => this.safeBack(), 500);
        }
      });
    }
  }
}
```

## 修改账号信息

### 修改账号界面设计

编辑页面复用添加页面的布局，通过路由参数判断是编辑模式。页面初始化时从参数中获取账号信息并回填到表单。

```typescript
aboutToAppear() {
  try {
    let params = router.getParams() as Record<string, Object>;
    if (params) {
      if (params['userId']) this.currentUserId = params['userId'] as number;
      if (params['id']) {
        this.editAccountId = params['id'] as number;
        this.platform = params['platform'] as string;
        this.account = params['account'] as string;
        this.password = params['password'] as string;
        // 初始化时检测一次强度
        this.checkStrength(this.password);
      }
    }
  } catch (e) { console.error(`Get params failed: ${e}`); }

  // 初始化数据库
  let context = getContext(this) as common.UIAbilityContext;
  this.rdbHelper = new RdbHelper(context);
  this.rdbHelper.initRdbStore((isSuccess: boolean) => {});
}
```

### 滑动菜单

主页列表项支持左滑显示编辑和删除按钮，点击编辑按钮跳转到编辑页面并传递账号信息。

```typescript
@Builder SwipeMenu(item: AppAccount) {
  Row() {
    Button() {
      Column() { Text('编辑').fontColor(Color.White).fontSize(14) }.justifyContent(FlexAlign.Center)
    }
    .width(60).height('100%').backgroundColor(0x007DFF).type(ButtonType.Normal)
    .onClick(() => {
      router.pushUrl({
        url: 'pages/AccountInputPage',
        params: {
          userId: this.currentUserId,
          id: item.getId(),
          platform: item.getPlatformName(),
          account: item.getAccountName(),
          password: item.getPassword()
        }
      });
    })

    Button() { Text('删除').fontColor(Color.White).fontSize(14) }
    .width(60).height('100%').backgroundColor(Color.Red).type(ButtonType.Normal)
    .onClick(() => { this.handleDelete(item.getId()); })
  }
  .height('100%')
}
```

## 删除账号

### 删除确认对话框

使用 AlertDialog 实现二次确认，防止误删操作。确认删除后调用数据库删除方法并刷新列表。

```typescript
handleDelete(id: number): void {
  try {
    AlertDialog.show({
      title: '删除确认',
      message: '确定要删除这条记录吗？',
      primaryButton: { value: '取消', action: () => {} },
      secondaryButton: {
        value: '删除', fontColor: Color.Red,
        action: () => {
          this.rdbHelper?.deleteAppAccount(id).then((rows) => {
            if (rows > 0) {
              this.safeToast('已删除');
              this.loadData();
            }
          });
        }
      }
    });
  } catch (e) {}
}
```

## 数据模型设计

### 用户实体类 (User.ets)

用户实体类定义了用户的基本属性，包括 id、用户名、密码、手机号等，并提供 `toValuesBucket()` 方法将对象转换为数据库存储格式。

```typescript
export default class User {
  private id: number;
  private username: string;
  private password: string;
  private phone: string;
  private email: string;
  private avatar: string;
  private createTime: number;

  constructor(username: string, password: string, phone: string = '') {
    this.id = 0;
    this.username = username;
    this.password = password;
    this.phone = phone;
    this.email = '';
    this.avatar = '/common/images/default_avatar.png';
    this.createTime = new Date().getTime();
  }

  // Getter / Setter 方法...

  public toValuesBucket(): relationalStore.ValuesBucket {
    return {
      'username': this.username,
      'password': this.password,
      'phone': this.phone,
      'email': this.email,
      'avatar': this.avatar,
      'create_time': this.createTime
    };
  }
}
```

### 应用账号实体类 (AppAccount.ets)

应用账号实体类用于存储用户记录的第三方应用账号信息，通过 `ownerId` 字段关联用户表实现多用户数据隔离。

```typescript
export default class AppAccount {
  private id: number;
  private platformName: string; // 平台名称，如 "微信", "淘宝"
  private accountName: string;  // 账号/用户名
  private password: string;     // 密码
  private ownerId: number;      // 归属人ID (关联 User 表的 id)

  constructor(platformName: string, accountName: string, password: string, ownerId: number) {
    this.id = 0;
    this.platformName = platformName;
    this.accountName = accountName;
    this.password = password;
    this.ownerId = ownerId;
  }

  // Getter / Setter 方法...

  public toValuesBucket(): relationalStore.ValuesBucket {
    return {
      'platform_name': this.platformName,
      'account_name': this.accountName,
      'password': this.password,
      'owner_id': this.ownerId
    };
  }
}
```

## 数据库设计

使用关系型数据库 (RDB) 存储数据，数据库名为 `PasswordSafe.db`，包含两张表。

### 用户表 (USER_TABLE)

```sql
CREATE TABLE IF NOT EXISTS USER_TABLE (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT NOT NULL,
  password TEXT NOT NULL,
  phone TEXT,
  email TEXT,
  avatar TEXT,
  create_time INTEGER
)
```

### 账号记录表 (APP_ACCOUNT_TABLE)

```sql
CREATE TABLE IF NOT EXISTS APP_ACCOUNT_TABLE (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  platform_name TEXT,
  account_name TEXT,
  password TEXT,
  owner_id INTEGER
)
```

其中 `owner_id` 关联用户表的 `id`，实现多用户数据隔离，确保每个用户只能查看和管理自己的账号数据。
