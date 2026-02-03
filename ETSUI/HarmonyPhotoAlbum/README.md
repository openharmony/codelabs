## 电子相册（HarmonyPhotoAlbum, ArkTS）

一个基于 **OpenHarmony** 与 **ArkTS** 实现的电子相册应用示例，支持从系统相册/相机导入图片、本地数据库管理、分类与搜索、懒加载与缩放浏览等功能，适合作为 OpenHarmony UI 与数据持久化实践项目。

---

## 1. 项目概览

### 1.1 技术栈

- OpenHarmony Stage 模型
- ArkTS 声明式 UI
- `@ohos.data.relationalStore` 本地关系型数据库（RDB）
- `@ohos.multimedia.mediaLibrary` 媒体库（图片选择）
- `@ohos.multimedia.camera` 相机能力（拍照）

### 1.2 核心功能

- 首页精选轮播图展示（`IndexPage`）
- 图片网格瀑布流展示
- 从系统相册/相机导入图片
- 图片信息录入（名称、分类、标签）并持久化到本地数据库
- 图片分类筛选、关键词搜索（名称/分类/标签）
- 图片详情大图浏览与缩放拖拽查看
- LazyImage 懒加载与图片缓存

---

## 2. 目录结构

项目根目录：`HarmonyPhotoAlbum/`

```bash
HarmonyPhotoAlbum/
├── entry/src/main/ets/              # 应用开发主目录
│   ├── entryability/
│   │   └── EntryAbility.ts          # 应用入口
│   ├── pages/                       # 页面
│   │   ├── IndexPage.ets            # 首页：轮播 + 网格 + Tab
│   │   ├── ListPage.ets             # 搜索与分类列表页
│   │   ├── DetailListPage.ets       # 详情列表页
│   │   └── DetailPage.ets           # 单张图片大图浏览页
│   ├── components/                  # 复用组件
│   │   ├── ImagePicker.ets          # 图片选择器（相册/相机）
│   │   ├── SearchBar.ets            # 搜索框
│   │   ├── CategoryFilter.ets       # 分类筛选
│   │   └── LazyImage.ets            # 图片懒加载组件
│   ├── model/
│   │   └── PhotoModel.ets           # 图片数据模型
│   ├── service/
│   │   ├── PhotoService.ets         # 图片数据库访问、搜索、分类
│   │   └── FileService.ets          # 文件/媒体相关操作
│   ├── view/
│   │   └── PhotoItem.ets            # 通用图片 Item 视图
│   └── common/
│       ├── constants/
│       │   └── Constants.ets        # 常量定义（RDB 配置、路由、UI 常量等）
│       └── utils/
│           ├── ImageCache.ets       # 图片缓存工具
│           └── Logger.ets           # 日志与错误提示封装
├── entry/src/main/resources/        # 资源文件
│   ├── base/element/                # 字符串、颜色、浮点等资源
│   ├── base/media/                  # 图片资源（图标、示例图片）
│   └── base/profile/main_pages.json # 页面路由配置
└── AppScope/                        # 应用范围配置
    └── app.json5
```

---

## 3. 功能说明

### 3.1 首页（IndexPage）

- 初始化本地数据库：`PhotoService.initDB`，并加载所有图片数据
- 顶部 Swiper 精选轮播：使用数据库中前 N 张图片，支持点击进入详情
- 中部 Grid 图片网格：按时间倒序展示所有图片
- 悬浮「+」按钮：弹出图片选择底部弹窗
  - 通过 `ImagePicker` 选择系统相册图片或拍照
  - 选择后弹出 `PhotoInfoDialog` 录入名称、分类、标签
  - 使用 `PhotoService.insert` 将图片信息写入 RDB

### 3.2 列表页 & 搜索（ListPage）

- 作为「搜索」Tab 对应页面，通过 `router.pushUrl` 从首页跳转
- 顶部始终展示：
  - `SearchBar`：输入关键词回车/点击搜索触发
  - `CategoryFilter`：从数据库分类列表中生成筛选标签
- 底部使用 Grid 展示 `displayPhotos`：
  - 初次进入与页面返回时调用 `PhotoService.queryAll()`
  - 搜索与分类使用 `PhotoService.searchPhotosByCategoryAndKeyword`
- 空状态时提供「返回首页上传」按钮

### 3.3 数据存储与搜索（PhotoService & PhotoModel）

- 使用 `@ohos.data.relationalStore` 创建本地 RDB：
  - 表字段：`id, name, path, category, createTime, tags`
  - 常量在 `Constants.ets` 中维护
- 提供能力：
  - `initDB(context)`：初始化数据库、建表
  - `insert(photo: PhotoModel)`：插入图片记录
  - `queryAll()`：按 `createTime` 倒序查询所有图片
  - `queryByCategory(category)`：按分类查询
  - `searchPhotos(keyword)`：按名称/分类/标签模糊搜索
  - `searchPhotosByCategoryAndKeyword(category, keyword)`：组合搜索
  - `getCategories()`：去重后的分类列表

### 3.4 图片选择、缓存与懒加载

- `ImagePicker` 组件：
  - 集成媒体库与相机能力
  - 提供回调 `onImageSelected(path: string)` 返回本地文件路径
- `LazyImage` 组件：
  - 支持占位图、错误图
  - 结合 `ImageCache` 工具实现简单缓存与懒加载

### 3.5 图片详情与手势

- `DetailListPage`：接收图片数组参数 `photoArr`，负责图片集合浏览
- `DetailPage`：
  - 支持捏合缩放、拖拽查看细节
  - 支持左右滑动切换图片

---

## 4. 运行与调试


### 4.1 导入项目

1. 在 DevEco Studio 中选择 **Open Project**
2. 选择本仓库根目录 `HarmonyPhotoAlbum`
3. 等待依赖同步完成

### 4.2 安装 & 运行

1. 连接 OpenHarmony 真机或启动模拟器
2. 在 DevEco Studio 中点击 **Run** 运行 `entry` 模块
3. 应用安装完成后即可在设备上体验电子相册功能

> 注：由于依赖系统媒体库和相机能力，**真机体验效果更佳**。

---

## 5. 使用指南

1. 打开应用后，在首页点击右下角 **「+」**：
   - 选择从系统相册导入或调用相机拍照
2. 选择图片后，在弹出的对话框中填写：
   - 图片名称
   - 分类（如：生活 / 风景 / 美食）
   - 标签（支持多标签，如：`#旅行 #家庭`）
3. 保存后，图片会出现在：
   - 首页轮播区与下方网格
   - 搜索/分类列表页
4. 在 **搜索页（ListPage）**：
   - 通过顶部搜索框按名称/标签/分类搜索
   - 通过分类筛选快速查看某一类图片
5. 点击任意图片进入详情，可：
   - 捏合放大/缩小
   - 拖拽移动查看细节
   - 左右滑动切换图片

---

## 6. 设计与实践要点

- **数据持久化**：通过 RDB 将原本硬编码在 `Constants.ets` 中的图片信息迁移到本地数据库，提升可维护性与扩展性。
- **分层设计**：
  - UI 层：`pages/`、`components/`、`view/`
  - 业务服务层：`service/`
  - 数据模型层：`model/`
  - 公共基础能力：`common/constants`、`common/utils`
- **搜索与分类**：在 `PhotoService` 中统一封装搜索条件，前端只需传入分类与关键词。
- **性能与体验**：
  - 使用 `LazyImage` 与 `ImageCache` 实现图片懒加载与缓存
  - 页面空态、加载态与错误提示统一使用 `Logger` 等工具封装

---

## 7. 后续可扩展方向

- 图片信息编辑与删除
- 幻灯片自动播放模式
- 云端同步与备份
- 多端协同（平板/大屏适配）
- 用户多相册/多账号管理

---

## 8. 许可协议

本项目基于 **Apache License 2.0** 开源，详情见仓库中的 `LICENSE` 文件。
