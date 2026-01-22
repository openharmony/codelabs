# SimpleApp- OpenHarmony 基础APP示例应用

## 📱 介绍

SimpleApp 是一个基于 OpenHarmony ArkTS 开发的多功能应用演示项目。该项目展示了如何使用 OpenHarmony 的 ArkTS 语言和 ArkUI 框架构建一个功能丰富的移动应用，包含常用系统应用启动、搜索功能、数据展示、交互组件等多种功能模块。
![](./image/output.gif)

### ✨ 主要功能

- **快速启动系统应用**：通讯、相机、音乐、照片、购物、设置等常用应用
- **智能搜索**：集成百度搜索，支持关键词搜索
- **数据统计展示**：步数、心率、睡眠等健康数据可视化
- **交互组件**：开关、滑块等交互控件演示
- **历史记录管理**：应用使用记录追踪和管理
- **响应式UI设计**：适配不同屏幕尺寸的现代化界面

## 🛠️ 技术特性

### 核心技术栈

- **开发语言**：ArkTS (TypeScript-based)
- **UI框架**：ArkUI
- **开发工具**：DevEco Studio
- **目标平台**：OpenHarmony

### 主要组件使用

- **@Component**：自定义组件装饰器
- **@State**：状态管理
- **@Builder**：构建器方法
- **@CustomDialog**：自定义弹窗
- **Grid/List**：网格和列表布局
- **Scroll**：滚动容器
- **TextInput/Toggle/Slider**：交互组件

## 📁 项目结构

```
MyApplication/
├── AppScope/                          # 应用全局配置
│   ├── app.json5                      # 应用基本信息
│   └── resources/                     # 全局资源文件
│       ├── base/
│       │   ├── element/               # 颜色、字符串资源
│       │   ├── media/                 # 图片资源
│       │   └── profile/               # 页面配置
│       └── en_US/zh_CN/               # 多语言支持
├── entry/                             # 主模块
│   └── src/main/
│       ├── ets/                       # ArkTS 源代码
│       │   ├── entryability/          # 应用入口
│       │   │   └── EntryAbility.ets   # 应用生命周期管理
│       │   ├── pages/                 # 页面文件
│       │   │   └── Index.ets          # 主页面
│       │   └── utils/                 # 工具类
│       │       ├── SystemAppLauncher.ts  # 系统应用启动工具
│       │       └── HistoryManager.ts     # 历史记录管理
│       └── resources/                 # 模块资源文件
└── build-profile.json5                # 构建配置
```

## 🚀 快速开始

### 环境要求

- **DevEco Studio**:  6.0.1 或更高版本
- **OpenHarmony SDK**: API 版本 20
- **Node.js**: 18+ (用于包管理)

### 开发环境搭建

1. **安装 DevEco Studio**

   - 获取并安装 [DevEco Studio](https://developeros.com/cn/develop/deveco-studio) 开发工具
   - 配置开发所需的 SDK
2. **导入项目**

   ```bash
   # 克隆或下载项目代码
   git clone <repository-url>

   # 在 DevEco Studio 中打开项目
   File → Open → 选择项目根目录
   ```
3. **配置设备**

   - 连接真机设备或启动模拟器
   - 确保设备已开启开发者模式
4. **构建运行**

   ```bash
   # 使用 DevEco Studio 构建
   Build → Build HAP(s)

   # 或使用命令行
   hvigorw assembleHap --mode module -p product=default
   ```

## 📖 核心功能详解

### 1. 系统应用启动

项目集成了多种系统应用的启动功能，通过 `SystemAppLauncher` 工具类实现：

```typescript
// 启动通讯录
SystemAppLauncher.launchContacts(context);

// 启动相机
SystemAppLauncher.launchCamera(context);

// 启动浏览器搜索
SystemAppLauncher.launchBrowser(context, 'https://www.baidu.com/s?wd=关键词');
```

### 2. 历史记录管理

使用 `HistoryManager` 类记录和管理应用使用历史：

```typescript
// 添加使用记录
const record: AppUsageRecord = {
  appName: '通讯',
  timestamp: Date.now(),
  packageName: 'com.ohos.contacts'
};
await HistoryManager.addRecord(context, record);

// 获取历史记录
const records = await HistoryManager.getRecords(context);
```

### 3. 自定义弹窗

实现历史记录查看弹窗，使用 `@CustomDialog` 装饰器：

```typescript
@CustomDialog
struct HistoryDialog {
  controller: CustomDialogController;
  @State historyList: AppUsageRecord[] = [];
  
  build() {
    // 弹窗UI实现
  }
}
```

### 4. 响应式布局

使用 Grid 和 Flex 布局实现自适应界面：

```typescript
Grid() {
  ForEach(this.featureItems, (item: FeatureItem) => {
    GridItem() {
      // 功能卡片布局
    }
  })
}
.columnsTemplate('1fr 1fr')
.rowsTemplate('1fr 1fr 1fr')
```

## 🎨 UI 组件展示

### 顶部导航栏

- 用户欢迎信息
- 四叶草按钮（最近使用记录）

### 搜索区域

- 文本输入框
- 搜索按钮（调用百度搜索）

### 功能卡片

- 6个常用功能入口
- 网格布局，支持点击启动

### 数据统计

- 健康数据展示（步数、心率、睡眠）
- 进度条可视化

### 交互组件

- 开关控件（通知开关）
- 滑块控件（音量调节）

## 🔧 开发指南

### 添加新功能

1. **在 SystemAppLauncher 中添加新应用启动方法**

```typescript
static launchNewApp(context: common.UIAbilityContext): void {
  const want = {
    bundleName: 'com.example.app',
    abilityName: 'com.example.app.MainAbility'
  };
  context.startAbility(want);
}
```

2. **在 Index.ets 中添加功能卡片**

```typescript
// 在 featureItems 数组中添加新项
new FeatureItem('🎮', '新功能', '#FF6B6B')
```

### 自定义样式

修改颜色主题：

```typescript
// 在 Index.ets 中更新颜色配置
const themeColors = {
  primary: '#007AFF',
  secondary: '#FF9500',
  success: '#34C759'
};
```

## 📱 兼容性

- **设备类型**：手机、平板
- **OpenHarmony版本**：5.1.1及以上版本
- **API版本**：19及以上

## 📄 许可证

本项目采用 Apache License, Version 2.0 许可证 - 查看 [LICENSE](http://www.apache.org/licenses/LICENSE-2.0) 文件了解详情。

**注意**: 本项目为演示用途，实际部署时请根据具体需求进行适当修改和安全加固。
