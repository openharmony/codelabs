# SimpleApp- OpenHarmony 多模块化演示应用

## 📱 介绍

SimpleApp 是一个基于 OpenHarmony ArkTS 开发的多模块化演示应用。该项目展示了如何使用 OpenHarmony 的 ArkTS 语言和 ArkUI 框架构建现代化的模块化应用架构，包含系统应用启动、模块化UI组件、历史记录管理等功能。
![](./image/output.gif)
### ✨ 主要功能

- **系统应用快速启动**：通讯、相机、备忘录、照片、时钟、设置等6个常用系统应用
- **多模块化UI架构**：独立的模块组件设计，支持灵活扩展
- **历史记录管理**：应用使用记录追踪和管理
- **响应式断点设计**：适配不同屏幕尺寸的现代化界面
- **性能优化**：使用 LazyForEach 实现高效列表渲染

## 🛠️ 技术特性

### 核心技术栈

- **开发语言**：ArkTS (TypeScript-based)
- **UI框架**：ArkUI
- **开发工具**：DevEco Studio
- **目标平台**：OpenHarmony
- **架构模式**：模块化组件设计

### 主要技术特性

- **@Component**：自定义组件装饰器
- **@State/@Prop/@Link**：状态管理
- **@Builder**：构建器方法
- **@Provide/@Consume**：跨组件数据传递
- **LazyForEach**：高性能列表渲染
- **Grid/Scroll**：网格和滚动布局
- **响应式断点**：自适应屏幕尺寸

## 📁 项目结构

```
SimpleApp/
├── AppScope/                          # 应用全局配置
│   ├── app.json5                      # 应用基本信息
│   └── resources/                     # 全局资源文件
├── entry/                             # 主模块
│   └── src/main/
│       ├── ets/                       # ArkTS 源代码
│       │   ├── entryability/          # 应用入口
│       │   │   └── EntryAbility.ets   # 应用生命周期管理
│       │   ├── pages/                 # 页面文件
│       │   │   └── Index.ets          # 主页面（完整功能实现）
│       │   ├── generated/             # 生成的模块化代码
│       │   │   ├── pages/
│       │   │   │   └── HomePage.ets   # 模块化首页
│       │   │   └── view/              # UI模块组件
│       │   │       ├── MultiFunctionEntryModule.ets      # 常用功能模块
│       │   │       ├── PromotionalBannerModule.ets       # 促销横幅模块
│       │   │       ├── ProductRecommendationModule.ets   # 产品推荐模块
│       │   │       ├── PersonalizedRecommendationModule.ets # 个性化推荐模块
│       │   │       └── SearchNotificationModule.ets      # 搜索通知模块（UI组件）
│       │   └── utils/                 # 工具类
│       │       ├── SystemAppLauncher.ts  # 系统应用启动工具
│       │       └── HistoryManager.ts     # 历史记录管理
│       └── resources/                 # 模块资源文件
├── build-profile.json5                # 构建配置
└── module.json5                       # 模块配置（权限声明）
```

## 🚀 快速开始

### 环境要求

- **DevEco Studio**: 6.0.1 或更高版本
- **OpenHarmony SDK**: API 版本 20
- **Node.js**: 18+ (用于包管理)

### 开发环境搭建

1. **安装 DevEco Studio**

   - 获取并安装 [DevEco Studio](https://developeros.com/cn/develop/deveco-studio) 开发工具
   - 配置开发所需的 SDK

2. **导入项目**

   ```bash
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

项目集成了6个常用系统应用的启动功能，通过 `SystemAppLauncher` 工具类实现：

```typescript
// 启动通讯录
SystemAppLauncher.launchContacts(context);

// 启动相机
SystemAppLauncher.launchCamera(context);

// 启动备忘录
SystemAppLauncher.launchMemo(context);

// 启动图库
SystemAppLauncher.launchGallery(context);

// 启动时钟
SystemAppLauncher.launchClock(context);

// 启动设置
SystemAppLauncher.launchSettings(context);
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

// 清除历史记录
await HistoryManager.clearRecords(context);
```

### 3. 模块化组件架构

项目采用模块化设计，每个功能模块独立实现：

```typescript
// HomePage 集成所有模块
@Component
export struct HomePage {
  build() {
    Scroll(this.scroller) {
      Column() {
        MultiFunctionEntryModule()     // 常用功能入口
        PromotionalBannerModule()      // 促销横幅
        ProductRecommendationModule()  // 产品推荐
        PersonalizedRecommendationModule() // 个性化推荐
      }
    }
  }
}
```

### 4. 响应式断点设计

使用断点系统实现自适应布局：

```typescript
// 响应式布局示例
@Builder
buildResponsiveLayout() {
  if (this.currentBreakpoint === BreakpointType.SM) {
    // 小屏幕布局
    this.buildMobileLayout();
  } else {
    // 大屏幕布局
    this.buildTabletLayout();
  }
}
```

## 🎨 UI 模块展示

### MultiFunctionEntryModule - 常用功能模块

- 6个系统应用入口的网格布局
- 使用 Grid 和 LazyForEach 优化性能
- 支持点击启动相应应用

### PromotionalBannerModule - 促销横幅模块

- 轮播图展示热门促销信息
- 自动滚动和手动切换功能
- 视觉吸引力的横幅设计

### ProductRecommendationModule - 产品推荐模块

- 瀑布流产品展示
- 分类筛选和排序功能
- 产品详情查看

### PersonalizedRecommendationModule - 个性化推荐模块

- 基于用户行为的个性化推荐
- 智能算法优化展示内容
- 用户偏好学习

## 🔧 开发指南

### 添加新功能模块

1. **创建新的模块组件**

```typescript
// 在 generated/view/ 目录下创建新模块
@Component
export struct NewFeatureIntroductionModule {
  @Consume('appPathStack') appPathStack: NavPathStack;
  
  build() {
    // 新模块的UI实现
  }
}
```

2. **在 HomePage 中集成新模块**

```typescript
// 更新 HomePage.ets
@Component
export struct HomePage {
  build() {
    Scroll(this.scroller) {
      Column() {
        MultiFunctionEntryModule()
        NewFeatureIntroductionModule()           // 添加新模块
        PromotionalBannerModule()
      }
    }
  }
}
```

### 自定义样式主题

修改颜色和样式配置：

```typescript
// 在资源文件中定义主题色
const themeConfig = {
  primaryColor: '#007AFF',
  secondaryColor: '#FF9500',
  backgroundColor: '#F5F5F5'
};
```


## 📱 兼容性

- **设备类型**：手机、平板
- **OpenHarmony版本**：5.1.1及以上版本
- **API版本**：19及以上
- **权限要求**：相机权限、网络权限（如需要）

## 🔒 权限说明

应用在 module.json5 中声明了必要的权限：

```json
{
  "requestPermissions": [
    {
      "name": "ohos.permission.CAMERA",
      "reason": "启动相机应用需要相机权限"
    },
    {
      "name": "ohos.permission.INTERNET", 
      "reason": "网络功能需要网络权限"
    }
  ]
}
```

## 📄 许可证

本项目采用 Apache License, Version 2.0 许可证 - 查看 [LICENSE](http://www.apache.org/licenses/LICENSE-2.0) 文件了解详情。


---

**注意**: 本项目为 OpenHarmony 开发学习演示用途，展示了模块化应用架构的展示。实际部署时请根据具体需求进行调整和加强安全措施。