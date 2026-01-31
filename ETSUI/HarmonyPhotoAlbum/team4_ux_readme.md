## 团队成员4 开发说明与任务总结（用户体验优化）

> 角色定位：专注于**图片加载体验、手势体验、错误提示与整体交互动效**，在不改变业务流程的前提下，让电子相册更加顺滑、稳定、友好。

---

## 一、整体目标

1. 为所有图片展示点提供统一的**懒加载 + 缓存 + 占位/错误兜底**能力。
2. 优化大图浏览（Detail 页）的**缩放/拖拽/翻页**体验，避免“无限缩放”“误触乱跳”。
3. 建立统一的**日志 + 错误提示**机制，让用户在出错时有清晰反馈。
4. 为后续复杂**页面过渡动画/共享元素动效**预留常量和基础结构。

---

## 二、代码改动总览

### 1. 新增文件

- `entry/src/main/ets/common/utils/ImageCache.ets`
  - 内存图片缓存工具。
- `entry/src/main/ets/components/LazyImage.ets`
  - 懒加载图片组件。
- `team4_ux_readme.md`（本文）
  - 团队成员4的开发说明与任务总结。

### 2. 修改的主要文件

- 常量与工具
  - `entry/src/main/ets/common/constants/Constants.ets`
  - `entry/src/main/ets/common/utils/Logger.ets`
- 图片展示相关页面/组件
  - `entry/src/main/ets/pages/IndexPage.ets`
  - `entry/src/main/ets/pages/ListPage.ets`
  - `entry/src/main/ets/pages/DetailListPage.ets`
  - `entry/src/main/ets/pages/DetailPage.ets`
  - `entry/src/main/ets/view/PhotoItem.ets`
  - `entry/src/main/ets/components/ImagePicker.ets`

---

## 三、LazyImage 懒加载组件

### 3.1 组件位置与接口

- 文件：`entry/src/main/ets/components/LazyImage.ets`
- 组件声明：

```ts
@Component
export struct LazyImage {
  src: ImageSourceType;                 // string | Resource
  width: Length | Resource = '100%';
  height: Length | Resource = '100%';
  objectFit: ImageFit = ImageFit.Cover;
  cornerRadius: number | Resource | Length = 0;
  placeholder?: Resource;              // 加载中占位图
  errorHolder?: Resource;              // 加载失败占位图
  enableCache: boolean = true;         // 是否启用 ImageCache
}
```

内部使用状态：

- `realSrc`：实际展示的图片资源（从缓存或原始 src 得到）。
- `isLoading`：是否正在加载（逻辑上用于防抖）。
- `hasError`：是否加载失败。
- `isAppeared`：组件是否已进入可见生命周期。

### 3.2 生命周期与加载逻辑

- `aboutToAppear()`：
  - 标记 `isAppeared = true`，调用 `tryLoad()`。
- `aboutToDisappear()`：
  - 标记 `isAppeared = false`，避免不必要的重复加载。

`tryLoad()` 核心逻辑：

1. 若组件未出现 / 正在加载 / 已有 `realSrc`，则直接返回（防抖）。
2. 若开启缓存（`enableCache = true`）：
   - 调用 `imageCache.has(src)` → 命中则 `realSrc = imageCache.get(src)`。
3. 未命中缓存：
   - 直接使用 `src`，并在成功后写入缓存：`imageCache.set(src)`。
4. 发生异常时：
   - 用 `Logger.error('[LazyImage] load error', ...)` 打日志；
   - 标记 `hasError = true`，展示错误占位图。

### 3.3 视图渲染逻辑

在 `build()` 中：

1. **加载失败且有 `errorHolder`**：
   - 用错误占位图 `Image(errorHolder)` 填充。
2. **尚未加载完成（`!realSrc`）**：
   - 若有 `placeholder` → 使用 `Image(placeholder)`；
   - 否则 → 使用灰色 `Rect()` 作为通用占位。
3. **正常展示**：
   - 使用 `Image(realSrc)`，应用宽高、`objectFit`、圆角等属性。

### 3.4 替换点

所有原本直接 `Image('file://' + path)` 的地方，现在统一改为 `LazyImage`：

- `IndexPage.ets` 首页网格中的图片。
- `ListPage.ets` 搜索/分类结果中的图片项。
- `DetailListPage.ets` 中的大图列表与缩略图列表。
- `DetailPage.ets` 中的大图浏览视图和前景单图。
- `PhotoItem.ets` 中栈叠展示的图片。

示例（以 ListPage 为例）：

```ts
LazyImage({
  src: 'file://' + photo.path,
  width: '100%',
  height: '100%',
  objectFit: ImageFit.Cover,
  placeholder: Constants.PLACEHOLDER_IMAGE,
  errorHolder: Constants.ERROR_IMAGE,
  cornerRadius: 8
})
```

---

## 四、ImageCache 图片缓存工具

### 4.1 工具位置与接口

- 文件：`entry/src/main/ets/common/utils/ImageCache.ets`
- 类型与结构：

```ts
export type ImageSourceType = string | Resource;

interface CacheEntry {
  src: ImageSourceType;
  time: number; // 写入时间，用于淘汰策略
}

class ImageCache {
  private cache: Map<string, CacheEntry> = new Map();
  private maxSize: number = Constants.IMAGE_CACHE_MAX_SIZE;

  setMaxSize(size: number) { ... }
  get(src: ImageSourceType): ImageSourceType | null { ... }
  set(src: ImageSourceType): void { ... }
  has(src: ImageSourceType): boolean { ... }
  delete(src: ImageSourceType): void { ... }
  clear(): void { ... }
}

const imageCache = new ImageCache();
export default imageCache;
```

### 4.2 淘汰策略

- 当 `cache.size > maxSize` 时：
  - 将 `Map` 转成数组，按 `time` 正序排序；
  - 删除最旧的 `cache.size - maxSize` 条记录。

> 目前只做内存级缓存，适合本地 file:// 图片和资源图片，简单好用，不引入复杂 IO。

---

## 五、DetailPage 手势与缩放优化

文件：`entry/src/main/ets/pages/DetailPage.ets`

### 5.1 缩放边界控制

- 常量：
  - `Constants.MIN_ZOOM_SCALE = 1`
  - `Constants.MAX_ZOOM_SCALE = 3`

- `resetImg()` 调整：

```ts
resetImg(): void {
  this.imgScale = Constants.MIN_ZOOM_SCALE;
  this.currentScale = Constants.MIN_ZOOM_SCALE;
  this.preOffsetX = 0;
  this.preOffsetY = 0;
}
```

- `PinchGesture.onActionUpdate` 中限制缩放范围：

```ts
this.imgScale = this.currentScale * event.scale;
if (this.imgScale < Constants.MIN_ZOOM_SCALE) {
  this.imgScale = Constants.MIN_ZOOM_SCALE;
} else if (this.imgScale > Constants.MAX_ZOOM_SCALE) {
  this.imgScale = Constants.MAX_ZOOM_SCALE;
}
```

- `onActionEnd`：
  - 小于最小值 → 重置；
  - 大于最大值 → 回落到 MAX，并更新 `currentScale`。

### 5.2 左右拖拽切图逻辑

- 常量：`Constants.SWIPE_THRESHOLD_RATIO = 0.25`。
- 逻辑：

```ts
handlePanEnd(): void {
  const threshold = this.imageWidth * Constants.SWIPE_THRESHOLD_RATIO;
  if (Math.abs(this.imgOffSetX) > threshold) {
    if (this.imgOffSetX > 0 && this.selectedIndex > 0) {
      this.selectedIndex -= 1; // 向右拖 -> 上一张
    } else if (this.imgOffSetX < 0 && this.selectedIndex < (this.photoArr.length - 1)) {
      this.selectedIndex += 1; // 向左拖 -> 下一张
    }
    this.isScaling = false;
    this.resetImg();
    this.scroller.scrollTo({ xOffset: this.selectedIndex * this.imageWidth, yOffset: 0 });
  }
}
```

> 拖拽是否切图由“偏移量是否超过图片宽度的一定比例”决定，避免轻微拖动就误触翻页。

### 5.3 与 LazyImage 的配合

- 所有展示图片的地方改为 LazyImage：
  - 横向列表中的大图。
  - 前景跟随手势缩放的单张图片。
- 这样 DetailPage 既有更好的手势体验，又继承了统一的懒加载/缓存能力。

---

## 六、全局 Logger 与错误提示

文件：`entry/src/main/ets/common/utils/Logger.ets`

### 6.1 新增能力

- 新增依赖：`import prompt from '@ohos.promptAction';`
- 新增方法：

```ts
showError(message: string) {
  prompt.showToast({
    message: message,
    duration: 3000
  });
  this.error(message);
}

showToast(message: string) {
  prompt.showToast({
    message: message,
    duration: 2000
  });
}
```

### 6.2 实际接入点

1. `IndexPage.ets`
   - DB 初始化失败：`Logger.showError('相册初始化失败，请稍后重试');`
   - 插入数据失败：`Logger.showError('保存失败，数据库写入错误');`

2. `ListPage.ets`
   - 加载所有图片失败：`图片加载失败，请稍后重试`。
   - 搜索失败：`搜索失败，请稍后重试`。
   - 切换分类失败：`切换分类失败，请稍后重试`。

3. `ImagePicker.ets`
   - 相册选择失败：`从相册选择图片失败，请稍后重试`。
   - 拍照失败：`拍照失败，请检查相机权限或稍后重试`。

> 统一通过 Logger 做 **日志记录 + 用户提示**，以后若要切换成对话框或其他提示形态，只需改 Logger 内部实现。

---

## 七、动画相关预留

- 文件：`entry/src/main/ets/common/constants/Constants.ets`
  - 新增通用动画时长：
    - `TRANSITION_DURATION_SHORT = 150`
    - `TRANSITION_DURATION_MEDIUM = 300`
    - `TRANSITION_DURATION_LONG = 500`
- 现在已有使用示例：
  - `IndexPage.ets` 中的 `Tabs.animationDuration(300)` 可与常量对齐。

后续可以在以下场景中使用这些常量：

1. 路由跳转动画（如支持在 `router.pushUrl` 中配置 animation）。
2. Tab 切换时图标/文字的缩放与颜色渐变。
3. FAB（右下角 + 按钮）进入页面时的小弹跳动画。
4. List → DetailList → Detail 的共享元素动画等。

---

## 八、开发与调试建议

1. **本地运行与真机调试**：
   - 建议使用真机或高分辨率模拟器，重点体验：
     - 首页加载速度与占位表现。
     - 搜索/分类下的图片懒加载情况。
     - DetailPage 多次缩放/拖拽后的稳定性。
2. **日志观察**：
   - 通过 `hilog` 查看 `Electronic Album` 域的日志，关注：
     - LazyImage 的加载错误日志。
     - 数据库/文件操作相关错误信息。
3. **扩展方向**：
   - 对 LazyImage 增加“进入可视区域再加载”的更精细逻辑（基于 `onVisibleAreaChange`）。
   - 在 ImageCache 中增加简单的过期时间或命中统计。
   - 为 DetailPage 的拖拽结束添加 `animateTo` 实现平滑回弹。

---

## 九、小结

作为团队成员4，你完成的是 **“让一切看起来更顺、用起来更安心”** 的工作，而不是改业务功能：

- LazyImage + ImageCache：解决统一的图片加载体验问题。
- DetailPage 手势优化：解决用户看图过程中的核心体验问题。
- Logger + 统一错误提示：解决“出问题时用户不知道发生了什么”的问题。
- 动画参数预留：为未来的视觉升级留好扩展点。

后续若要进一步迭代动画或优化缓存策略，可以继续在此文档下追加新章节，保持团队成员4的工作范围清晰可追踪。

