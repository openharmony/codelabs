# 隐私安全电子相册（HarmonyPhotoAlbum）

基于 OpenHarmony 的隐私安全电子相册 Demo，演示「应用沙箱 + PIN 鉴权 + 沉浸式电子相框 + 预置图片库」的完整体验。

> 本项目专注于沙箱内图片的管理与展示，所有照片数据均保存在应用私有目录，系统图库无法访问。

---

## ✨ 核心功能

### 1. 🔒 隐私安全保险箱

**PIN 码鉴权机制**
- 打开应用时，需要设置 **4 位数字 PIN 码**
- 再次进入应用时，必须输入正确 PIN 才能解锁相册
- PIN 使用 `@ohos.data.preferences` 持久化存储在应用沙箱内

**沙箱隔离**
- 所有相册图片文件与数据库均保存在 `UIAbilityContext.filesDir` 沙箱目录
- 系统图库无法扫描这些图片，实现物理隔离
- 确保隐私安全

**相关代码**
- `entry/src/main/ets/service/AuthService.ets` - PIN 鉴权服务
- `entry/src/main/ets/pages/IndexPage.ets` - 鉴权界面与流程

### 2. 🖼️ 智能相册首页

**双层展示布局**
- **顶部轮播区**：自动播放精选照片（前 4-8 张）
  - 支持用户照片优先展示
  - 数据库为空时显示预置示例图片
  - 自动轮播，支持点击查看大图
  
- **底部网格区**：两栏瀑布流展示所有照片
  - 显示除轮播外的照片
  - 确保上下不重复显示同一张图片
  - 点击可进入详情页浏览

**预置图片库**
- 内置精美示例图片（风景、生活、美食、人物等分类）
- 新用户体验无需添加照片即可浏览
- 来源：`entry/src/main/ets/model/ResourceGallery.ets`

### 3. 🎨 沉浸式电子相框

**分类播放模式**
- 点击首页右上角「电子相框」按钮
- 进入分类选择页，可选择：
  - 全部照片
  - 风景
  - 生活
  - 美食
  - 人物

**播放体验**
- 全屏沉浸式幻灯片播放
- 可调节轮播间隔（2秒 ~ 15秒）
- 点击屏幕显示/隐藏控制面板
- 自动循环播放，适合作为桌面电子相框

**相关代码**
- `entry/src/main/ets/pages/FrameCategoryPage.ets` - 分类选择页
- `entry/src/main/ets/pages/FramePage.ets` - 全屏播放页

### 4. 🔍 搜索与分类

**智能搜索**
- 支持按关键词搜索（名称、标签、分类）
- 分类筛选功能（风景、人物、动物、建筑、美食、生活等）
- 实时过滤，即时显示结果

**统一资源管理**
- 用户照片 + 预置图片统一展示
- 搜索和分类对两者同时生效
- 无缝浏览体验

**相关代码**
- `entry/src/main/ets/pages/ListPage.ets` - 搜索列表页
- `entry/src/main/ets/components/SearchBar.ets` - 搜索组件
- `entry/src/main/ets/components/CategoryFilter.ets` - 分类筛选组件

### 5. 📱 大图浏览

**横向滑动浏览**
- 左右滑动切换照片
- 支持双击/捏合缩放
- 显示照片名称、分类、标签等信息

**详情页切换**
- 从列表页进入，保持上下文
- 支持横屏全屏浏览
- 流畅的切换动画

**相关代码**
- `entry/src/main/ets/pages/DetailListPage.ets` - 横向浏览页
- `entry/src/main/ets/pages/DetailPage.ets` - 单图详情页

---

## 🏗️ 技术架构

### 1. 应用沙箱隔离

所有业务数据保存在应用私有目录：

```
/data/storage/el2/base/files/
├── photos/              # 用户照片文件
├── PhotoAlbum.db        # 关系型数据库
└── preferences/         # PIN 码等配置
```

**特点**：
- 系统图库无法扫描
- 卸载应用后自动删除

### 2. PIN 鉴权与持久化

使用 `@ohos.data.preferences` 实现：

```typescript

PREF_NAME = 'privacy_safe_box_pref'
KEY_PIN = 'pin_code'

// 鉴权流程
1. 启动 → 设置 PIN → 保存到 Preferences
2. 再次启动 → 输入 PIN → 校验 → 通过后显示相册
```

### 3. 关系型数据库

使用 `@ohos.data.relationalStore`：

**表结构**：
```sql
CREATE TABLE photo_table (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  path TEXT NOT NULL,
  category TEXT,
  createTime INTEGER,
  tags TEXT
)
```

**数据服务**：
- `PhotoService.initDB()` - 初始化数据库
- `PhotoService.insert()` - 插入照片记录
- `PhotoService.queryAll()` - 查询所有照片
- `PhotoService.searchPhotosByCategoryAndKeyword()` - 组合搜索

### 4. 图片懒加载与缓存

**LazyImage 组件**：
- 内存缓存机制（最多缓存 100 张）
- 占位图与错误图支持
- 圆角、裁剪等样式定制

**相关代码**：
- `entry/src/main/ets/components/LazyImage.ets`
- `entry/src/main/ets/common/utils/ImageCache.ets`

---

## 📁 项目结构

```
entry/src/main/ets/
├── common/
│   ├── constants/
│   │   └── Constants.ets           # 全局常量（路由、数据库、UI 参数）
│   └── utils/
│       ├── ImageCache.ets          # 图片缓存工具
│       └── Logger.ets              # 日志工具
│
├── components/
│   ├── LazyImage.ets               # 图片懒加载组件
│   ├── SearchBar.ets               # 搜索栏组件
│   ├── CategoryFilter.ets          # 分类筛选组件
│   └── ImagePicker.ets             # 图片选择器（预留）
│
├── entryability/
│   └── EntryAbility.ts             # Ability 入口
│
├── model/
│   ├── PhotoModel.ets              # 照片数据模型
│   └── ResourceGallery.ets         # 预置图片资源库
│
├── pages/
│   ├── IndexPage.ets               # 首页 + PIN 鉴权
│   ├── ListPage.ets                # 搜索列表页
│   ├── DetailListPage.ets          # 大图横向浏览页
│   ├── DetailPage.ets              # 单图详情页
│   ├── FrameCategoryPage.ets       # 电子相框分类选择页
│   └── FramePage.ets               # 沉浸式电子相框播放页
│
├── service/
│   ├── PhotoService.ets            # 照片数据服务（RDB 封装）
│   ├── FileService.ets             # 文件服务（沙箱文件操作）
│   └── AuthService.ets             # PIN 鉴权服务
│
└── view/
    └── PhotoItem.ets               # 照片列表项视图

entry/src/main/resources/
├── base/
│   ├── element/
│   │   ├── string.json             # 英文字符串资源
│   │   ├── color.json              # 颜色资源
│   │   └── float.json              # 尺寸资源
│   ├── media/                      # 预置图片资源
│   │   ├── ic_scene_0.png          # 风景类
│   │   ├── ic_life_0.png           # 生活类
│   │   ├── ic_food_0.png           # 美食类
│   │   ├── ic_men_0.png            # 人物类
│   │   └── icon.png                # 应用图标
│   └── profile/
│       └── main_pages.json         # 页面路由配置
│
└── zh_CN/
    └── element/
        └── string.json             # 中文字符串资源
```

---

## 🚀 运行指南

### 环境要求

- **DevEco Studio** 4.0+ （或支持 OpenHarmony 的开发环境）
- ** OpenHarmony 模拟器**（建议带屏设备/平板）
- **API Level**: 9+

### 运行步骤

1. **打开项目**
   ```bash
   # 克隆仓库（如适用）
   cd HarmonyPhotoAlbum_v10
   
   # 在 DevEco Studio 中打开
   ```

2. **配置签名**
   - 确保 `build-profile.json5` 中的签名配置有效
   - 默认使用本机调试证书

3. **安装运行**
   - 连接 设备或启动模拟器
   - 点击 DevEco Studio 的 **Run** 按钮
   - 等待应用安装完成

### 功能体验流程

#### 第一步：设置 PIN 码

1. 打开应用，看到「隐私安全保险箱」界面
2. 输入 4 位数字 PIN（例如：`1234`）
3. 再次输入确认
4. 点击「保存并进入相册」

#### 第二步：浏览相册

1. 成功解锁后进入首页
2. **顶部轮播区**：自动播放预置示例图片
3. **底部网格区**：两栏显示所有图片
4. 点击任意图片可查看大图

#### 第三步：搜索与分类

1. 点击底部「搜索」Tab
2. 使用搜索栏输入关键词（如：`风景`、`美食`）
3. 或使用分类筛选按钮快速过滤
4. 点击图片查看详情

#### 第四步：电子相框播放

1. 返回首页，点击右上角「电子相框」按钮
2. 选择播放分类（全部/风景/生活/美食/人物）
3. 进入全屏播放模式：
   - 图片自动轮播切换
   - 点击屏幕显示/隐藏控制面板
   - 拖动滑块调节播放间隔
4. 点击左上角返回按钮退出

#### 第五步：再次启动

1. 关闭应用后重新打开
2. 需要输入之前设置的 PIN 码
3. 输入正确后才能访问相册

---

## 🎯 功能亮点

### 1. 安全性

- ✅ PIN 码鉴权保护
- ✅ 应用沙箱物理隔离
- ✅ 系统图库无法访问
-

### 2. 用户体验

- ✅ 首页双层布局，轮播+网格不重复
- ✅ 预置图片库，开箱即用
- ✅ 流畅的动画过渡
- ✅ 响应式交互设计

### 3. 电子相框

- ✅ 分类播放模式
- ✅ 可调节播放间隔
- ✅ 全屏沉浸式体验
- ✅ 一键隐藏控制面板

### 4. 技术实现

- ✅ 关系型数据库管理
- ✅ 图片懒加载与缓存
- ✅ 模块化代码架构
- ✅ TypeScript 类型安全

---

## 🔧 配置说明

### 修改轮播间隔

在 `Constants.ets` 中修改默认值：

```typescript
// 默认轮播间隔（毫秒）
static readonly DEFAULT_CAROUSEL_INTERVAL: number = 5000;
```

### 修改缓存大小

在 `Constants.ets` 中修改：

```typescript
// 最大缓存图片数量
static readonly IMAGE_CACHE_MAX_SIZE: number = 100;
```

### 添加预置图片

在 `ResourceGallery.ets` 中添加：

```typescript
export const GALLERY_ITEMS: Array<GalleryItem> = [
  {
    src: $r('app.media.your_new_image'),
    name: '图片名称',
    category: '分类',
    tags: '标签'
  },

];
```

---

## 📝 待扩展功能

### 短期计划

- [ ] 支持从相机拍摄并保存到沙箱
- [ ] 支持从系统图库导入（复制到沙箱）
- [ ] PIN 码重置/修改功能
- [ ] 照片编辑功能（裁剪、滤镜）

### 中期计划

- [ ] 相册分组管理
- [ ] 照片标签自动识别（AI）
- [ ] 云端备份与同步
- [ ] 多设备数据迁移



---

## 🐛 常见问题

### Q1: 忘记 PIN 码怎么办？

**A**: 目前需要卸载应用重新安装（会清空所有数据）。后续版本将支持安全问题或邮箱找回。

### Q2: 如何添加自己的照片？

**A**: 当前版本需要通过开发工具将图片推送到应用沙箱目录，然后通过代码入库。后续版本将支持相机拍摄和图库导入。

### Q3: 预置图片可以删除吗？

**A**: 预置图片仅用于展示，不会占用数据库空间。可以在 `ResourceGallery.ets` 中移除不需要的图片。

### Q4: 电子相框可以后台播放吗？

**A**: 目前仅支持前台播放。可以通过系统设置禁用锁屏来实现长时间播放。

### Q5: 编译报错怎么办？

**A**: 
1. 确保 DevEco Studio 版本符合要求
2. 清理构建缓存：`Build > Clean Project`
3. 重新构建：`Build > Rebuild Project`
4. 检查签名配置是否正确

---

## 📄 许可证

本项目基于 Apache License 2.0 开源协议。

---

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request！

在提交 PR 前，请确保：
1. 代码符合 TypeScript 规范
2. 所有功能经过测试
3. 更新相关文档

---

## 📧 联系方式

如有问题或建议，欢迎：
- 提交 GitHub Issue
- 发送邮件讨论
- 加入社区交流

---

**享受安全、私密的照片管理体验！** 📸✨
