# ChatAppDemo 项目说明文档

## 项目概述

ChatAppDemo 是一个基于 OpenHarmony 系统开发的模拟即时通讯应用，使用 ETS (eTS) 语言和 ArkUI 框架实现。应用提供了完整的聊天功能，包括用户注册登录、消息发送接收、联系人管理、动态发布等核心功能，是一个功能齐全的社交应用示例。

## 项目结构

项目采用模块化架构设计，代码组织清晰，主要目录结构如下：

```
entry/src/main/ets/
├── common/              # 公共资源和工具类
│   └── I18nManager.ets  # 国际化管理
├── components/          # 可复用组件
│   ├── ChatList.ets     # 聊天列表组件
│   ├── Contact.ets      # 联系人组件
│   ├── ContactGroup.ets # 联系人分组组件
│   ├── LocationPickerDialog.ets # 位置选择对话框
│   ├── NavTitle.ets     # 导航标题组件
│   ├── Post.ets         # 动态发布组件
│   └── Profile.ets      # 个人资料组件
├── entryability/        # 应用入口能力
│   ├── EntryAbility.ets # 主入口能力
│   └── EntryBackupAbility.ets # 备份入口能力
├── pages/               # 页面文件
│   ├── AboutUs.ets      # 关于页面
│   ├── AddFriends.ets   # 添加好友页面
│   ├── AddPost.ets      # 发布动态页面
│   ├── ChangePassword.ets # 修改密码页面
│   ├── Chat.ets         # 聊天页面
│   ├── EditProfile.ets  # 编辑个人资料页面
│   ├── FriendDetail.ets # 好友详情页面
│   ├── Index.ets        # 启动页面
│   ├── LanguageSettings.ets # 语言设置页面
│   ├── LicenseAgreement.ets # 许可证协议页面
│   ├── Login.ets        # 登录页面
│   ├── Main.ets         # 主页面
│   ├── NewFriends.ets   # 新好友页面
│   ├── Register.ets     # 注册页面
│   └── Settings.ets     # 设置页面
├── utils/               # 工具类
│   ├── ChatHandler.ets  # 聊天相关操作
│   ├── DatabaseManager.ets # 数据库管理
│   ├── FriendHandler.ets # 好友相关操作
│   ├── LocationData.ets # 位置数据
│   ├── PinyinData.ets   # 拼音数据
│   ├── PostHandler.ets  # 动态相关操作
│   └── UserHandler.ets  # 用户相关操作
└── viewmodel/           # 数据模型
    ├── ContactModel.ets # 联系人模型
    ├── FriendRequestModel.ets # 好友请求模型
    ├── Message.ets      # 消息模型
    ├── PostModel.ets    # 动态模型
    └── UserModel.ets    # 用户模型
```

## 核心功能

### 1. 用户管理
- **注册登录**：支持手机号注册和登录
- **个人资料**：查看和编辑个人资料，包括昵称、头像等
- **密码修改**：支持修改登录密码

### 2. 聊天功能
- **消息列表**：显示最近的聊天会话
- **单聊**：与好友进行一对一聊天
- **消息历史**：查看聊天历史记录
- **好友验证**：只有好友才能发送消息

### 3. 联系人管理
- **联系人列表**：按字母顺序显示联系人
- **添加好友**：通过手机号添加好友
- **好友请求**：接收和处理好友请求
- **好友详情**：查看好友详细信息

### 4. 动态发布
- **发布动态**：支持发布文字和图片动态
- **动态列表**：查看所有好友的动态
- **点赞评论**：支持对动态进行点赞和评论

### 5. 其余功能
- **国际化**：支持多语言切换
- **设置管理**：应用相关设置
- **关于**：应用信息和许可证协议

## 技术架构

### 1. 前端框架
- **ArkUI**：OpenHarmony 原生 UI 框架
- **ETS**：eTS 开发语言，基于 TypeScript 扩展

### 2. 数据管理
- **DatabaseManager**：数据库管理类，负责初始化和管理数据库连接
- **关系型数据库**：使用 OpenHarmony 提供的 relationalStore 进行本地数据存储
- **数据模型**：使用 viewmodel 目录下的模型类封装数据结构

### 3. 核心模块
- **UserHandler**：用户相关数据库操作
- **FriendHandler**：好友关系相关操作
- **ChatHandler**：聊天消息相关操作
- **PostHandler**：动态发布相关操作

### 4. 通信机制
- **Emitter**：使用 emitter 进行组件间通信，实现消息刷新等功能
- **AppStorage**：使用全局状态管理，实现跨页面数据共享
- **PersistentStorage**：实现数据持久化，保存用户登录状态

### 5. 国际化
- **I18nManager**：国际化管理类，支持多语言切换
- **资源文件**：使用 Resource 类型管理多语言字符串

## 关键实现

### 1. 登录流程

1. 用户在登录页面输入手机号和密码
2. 系统通过 `DatabaseManager` 查询用户信息
3. 验证密码是否正确
4. 登录成功后，使用 `@StorageLink` 存储当前用户信息
5. 通过 `PersistentStorage` 持久化用户登录状态

### 2. 聊天功能实现

1. **消息存储**：使用本地数据库存储聊天消息，每条消息包含发送者、接收者、内容、时间等信息
2. **消息发送**：用户发送消息后，同时存储两条记录（发送方和接收方各一条）
3. **消息刷新**：使用 `emitter` 机制实现聊天列表的实时刷新
4. **好友验证**：发送消息前检查双方是否为好友关系

### 3. 联系人管理

1. **联系人分组**：根据拼音首字母对联系人进行分组
2. **好友请求**：支持发送、接收和处理好友请求
3. **关系管理**：维护好友关系的建立和解除

### 4. 动态发布

1. **内容存储**：将动态内容存储到本地数据库
2. **点赞机制**：记录用户的点赞状态
3. **评论功能**：支持对动态进行评论

## 数据库设计

应用使用关系型数据库存储数据，主要表结构如下：

### 1. User 表
- 存储用户基本信息
- 字段：id, avatarPath, nickName, country, phoneNumber, password

### 2. FriendRequest 表
- 存储好友请求信息
- 字段：id, fromPhone, toPhone, fromName, message, status

### 3. Friend 表
- 存储好友关系
- 字段：id, myPhone, friendPhone

### 4. ChatHistory 表
- 存储聊天记录
- 字段：id, owner, targetPhone, nickName, content, time, isSelf, avatar

### 5. Post 表
- 存储动态信息
- 字段：id, owner, avatar, nickname, publishTime, content, image, likeCount, isLiked, comments

### 6. LikeRecord 表
- 存储点赞记录
- 字段：id, postId, userPhone

## 技术特点

1. **模块化设计**：采用组件化和模块化设计，代码复用率高
2. **响应式布局**：使用 ArkUI 的声明式 UI，实现自适应布局
3. **数据持久化**：使用关系型数据库和 PersistentStorage 实现数据持久化
4. **国际化支持**：内置国际化管理，支持多语言切换
5. **组件通信**：使用 emitter 和全局状态管理实现组件间通信
6. **类型安全**：使用 TypeScript 语法，提供类型检查

## 依赖和配置

### 核心依赖

- **@kit.AbilityKit**：能力管理
- **@kit.ArkUI**：UI 框架
- **@kit.ArkData**：数据存储
- **@kit.PerformanceAnalysisKit**：性能分析
- **@kit.BasicServicesKit**：基础服务

### 配置信息

- **数据库配置**：使用安全级别 S1，数据库名称为 ChatDatabase.db
- **存储配置**：使用 PersistentStorage 持久化用户登录状态
- **权限配置**：需要申请相关权限以支持完整功能

## 使用说明

### 1. 运行环境

- OpenHarmony 3.0 及以上版本
- DevEco Studio 3.0 及以上版本

### 2. 安装部署

1. 使用 DevEco Studio 打开项目
2. 连接 OpenHarmony 设备或模拟器
3. 编译并运行项目

### 3. 功能使用

#### 注册登录
- 初次使用需要注册账号，输入手机号和密码
- 注册成功后可以直接登录
- 登录后系统会记住登录状态

#### 聊天功能
- 在聊天列表页面可以查看最近的聊天记录
- 点击列表项进入聊天页面
- 在聊天页面可以发送和接收消息

#### 联系人管理
- 在联系人页面可以查看所有好友
- 点击添加好友按钮，输入手机号添加好友
- 在新好友页面处理好友请求

#### 动态发布
- 在动态页面可以查看所有好友的动态
- 点击发布按钮，输入内容发布动态
- 可以对动态进行点赞和评论

## 开发指南

### 1. 代码结构

- **页面开发**：在 pages 目录下创建新页面
- **组件开发**：在 components 目录下创建可复用组件
- **工具类**：在 utils 目录下创建工具类
- **数据模型**：在 viewmodel 目录下定义数据模型

### 2. 数据库操作

- 使用 DatabaseManager 进行数据库初始化
- 通过各 Handler 类进行具体的数据库操作
- 新增功能时，需要在 DatabaseManager 中添加相应的建表语句

### 3. 国际化

- 在 I18nManager 中管理语言设置
- 使用 Resource 类型引用字符串资源
- 在资源文件中添加多语言支持

### 4. 性能优化

- 合理使用 @State、@StorageLink 等状态管理
- 避免频繁的 UI 更新
- 优化数据库查询，减少不必要的操作

## 注意事项

1. **数据存储**：当前版本使用本地数据库存储数据，未实现服务器端同步
2. **权限申请**：部分功能需要申请相应权限，请在使用前确保权限已获取
3. **国际化**：需要在资源文件中添加相应的多语言支持
4. **兼容性**：建议在 OpenHarmony 3.0 及以上版本运行

## 总结

ChatAppDemo 是一个功能齐全的模拟即时通讯应用示例，展示了如何使用 OpenHarmony 和 ArkUI 开发复杂的移动应用。该项目结构清晰，代码组织合理，是学习 OpenHarmony 应用开发的示例。通过本项目，开发者可以了解到如何实现用户管理、聊天功能、联系人管理、动态发布等核心社交应用功能，以及如何使用 OpenHarmony 提供的各种能力和 API。

## 许可证

本项目采用 Apache License 2.0 许可证，详情请参考 LICENSE 文件。