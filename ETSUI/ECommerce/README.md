# ECommerce（ArkTS）

## 介绍

本篇 Codelab 介绍了如何实现一个基于 ArkTS 的简易电商 Demo 应用。其主要功能包括：

- 首页商品瀑布流浏览与关键词搜索（商品名/描述/分类）
- 商品详情：收藏、评论入口、加入购物车、立即购买（引导到购物车结算）
- 购物车：数量增减、单选/全选、管理模式批量删除、结算支付
- 订单：列表按状态筛选、订单详情
- 个人中心：代金券余额展示、收货地址、收藏列表、足迹列表
- 本地持久化：使用 RDB 保存用户/商品/购物车/地址/订单/收藏/足迹等数据

本应用的运行效果如下图所示：

![](demo-gif/function_display_1.gif)

![](demo-gif/function_display_2.gif)

# 搭建 OpenHarmony 环境

## 软件要求

- [DevEco Studio](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)：DevEco Studio 6.0.0 Release。
- OpenHarmony SDK 版本：API version 20。

## 硬件要求

- 开发板类型：[润和 DAYU200 开发板](https://gitcode.com/hihope/DAYU200/blob/main/README.md)
- OpenHarmony系统：6.0 Release

## 环境搭建

完成本篇 Codelab 先要完成开发环境的搭建，本示例以 DAYU200 开发板为例，参照以下步骤进行：

1. [获取OpenHarmony系统版本](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/get-code/sourcecode-acquire.md#%E8%8E%B7%E5%8F%96%E6%96%B9%E5%BC%8F3%E4%BB%8E%E9%95%9C%E5%83%8F%E7%AB%99%E7%82%B9%E8%8E%B7%E5%8F%96)：标准系统解决方案（二进制）。以 6.0 版本为例：
   
   ![](figures/version.png)

2. 搭建烧录环境
   
   1. [完成DevEco Device Tool的安装](https://gitcode.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-ide-env-win.md)
   2. [完成DAYU200开发板的烧录](https://gitcode.com/hihope/DAYU200/blob/main/%E7%83%A7%E5%86%99%E5%B7%A5%E5%85%B7%E5%8F%8A%E6%8C%87%E5%8D%97/windows/%E7%83%A7%E5%BD%95%E6%8C%87%E5%AF%BC%E6%96%87%E6%A1%A3.md)

3. 搭建开发环境
   
   1. 开始前请参考[工具准备](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-overview.md#%E5%B7%A5%E5%85%B7%E5%87%86%E5%A4%87)，完成DevEco Studio的安装和开发环境配置。
   2. 开发环境配置完成后，请参考[使用工程向导](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-fa.md#%E5%88%9B%E5%BB%BAets%E5%B7%A5%E7%A8%8B)创建工程（模板选择“Empty Ability”），选择 JS 或者 eTS 语言开发。
   3. 工程创建完成后，选择使用[真机进行调测](https://gitcode.com/openharmony/docs/blob/master/zh-cn/application-dev/quick-start/start-with-ets-fa.md#%E4%BD%BF%E7%94%A8%E7%9C%9F%E6%9C%BA%E8%BF%90%E8%A1%8C%E5%BA%94%E7%94%A8)。

## 代码结构解读

本篇 Codelab 只对核心代码进行讲解，完整代码可以直接从 gitcode 获取。

```typescript
├── entry
│  ├── src/main/ets
│  │  ├── common
│  │  │  └── Constants.ets                // 常量：DB 名、表名、Key 等
│  │  ├── entryability
│  │  │  └── EntryAbility.ets             // 程序入口类
│  │  ├── entrybackupability
│  │  │  └── EntryBackupAbility.ets       // 数据备份与恢复类
│  │  ├── model
│  │  │  └── DataModels.ets               // 数据模型：User/Product/Order 等
│  │  ├── pages
│  │  │  ├── AddressEdit.ets              // 地址编辑
│  │  │  ├── AddressManage.ets            // 地址管理
│  │  │  ├── CartView.ets                 // 购物车
│  │  │  ├── CommentList.ets              // 评论
│  │  │  ├── FavoriteList.ets             // 收藏列表
│  │  │  ├── FootprintList.ets            // 足迹列表
│  │  │  ├── Index.ets                    // 首页
│  │  │  ├── LoginPage.ets                // 登录注册
│  │  │  ├── MineView.ets                 // 个人中心：订单入口/地址/收藏/足迹
│  │  │  ├── OrderDetail.ets              // 订单详情
│  │  │  ├── OrderList.ets                // 订单列表
│  │  │  └── ProductDetail.ets            // 商品详情
│  │  └── utils
│  │     ├── CryptoUtil.ets               // AES 加解密
│  │     └── RdbUtil.ets                  // RDB 封装：用户/商品/购物车/订单等
│  ├── src/main/resources                 // 应用静态资源目录
│  └── src/main/module.json5              // 模块配置
```

## 首页（Index）

首页是应用的主容器页面，通过 Tabs 组织三个区域：

- Home：商品列表与搜索
- Cart：购物车（需要登录，未登录统一跳转登录页）
- Mine：个人中心

这一页主要做三件事：

- 页面出现时加载商品（含初始化时 mock 注入）
- 将搜索关键词映射为 `shownProductList`（实际渲染列表）
- 协调 Tab 切换（含从子页面返回后“跳到指定 Tab”）

### 1) 页面出现：处理目标 Tab + 加载商品

首页 `aboutToAppear()` 会先应用 `indexTargetTab`（用于“从详情返回切到购物车”），再拉取商品并把搜索条件应用到展示列表：

```typescript
async aboutToAppear() {
  this.applyTargetTab()
  await this.loadProducts()
}

async loadProducts() {
  let products = await RdbUtil.getAllProducts()
  const changed = await this.initMockData(products)
  if (changed) {
    products = await RdbUtil.getAllProducts()
  }
  this.productList = products.slice()
  this.applySearch(this.searchKeyword)
}
```

### 2) 初始化时 mock 注入（Demo 级幂等）

为避免每次启动重复插入，这里用“多字段组合唯一键”做幂等判重。

```typescript
async initMockData(existingProducts: Product[] = []): Promise<boolean> {
  const buildKey = (p: Product): string => {
    const name = (p.name || '').toString()
    const category = (p.category || '').toString()
    const price = Number(p.price)
    const desc = (p.description || '').toString()
    return `${name}||${category}||${price}||${desc}`
  }

  const existingKeySet = new Set(existingProducts.map(buildKey))
  let inserted = false

  for (const p of mockProducts) {
    const key = buildKey(p)
    if (existingKeySet.has(key)) continue
    await RdbUtil.insertProduct(p)
    inserted = true
    existingKeySet.add(key)
  }

  return inserted
}
```

### 3) 搜索：输入绑定 + 过滤/提示

搜索栏把输入同步到 `searchKeyword`，点击“搜索”触发 `doSearch()`；`applySearch()` 负责把源数据 `productList` 过滤成 `shownProductList`：

```typescript
TextInput({ placeholder: '搜索商品', text: this.searchKeyword })
  .onChange((v: string) => { this.searchKeyword = v })

Button('搜索').onClick(() => { this.doSearch() })

private applySearch(keyword: string): number {
  const kw = (keyword || '').trim().toLowerCase()
  if (!kw) {
    this.shownProductList = this.productList.slice()
    return this.shownProductList.length
  }
  const filtered = this.productList.filter((p) => {
    const name = (p.name || '').toString().toLowerCase()
    const desc = (p.description || '').toString().toLowerCase()
    const cat = (p.category || '').toString().toLowerCase()
    return name.includes(kw) || desc.includes(kw) || cat.includes(kw)
  })
  this.shownProductList = filtered.slice()
  return filtered.length
}

private doSearch() {
  const kw = (this.searchKeyword || '').trim()
  if (!kw) {
    this.applySearch('')
    promptAction.showToast({ message: '已清除搜索' })
    return
  }
  const count = this.applySearch(kw)
  if (count === 0) promptAction.showToast({ message: '未找到相关商品' })
}
```

### 4) Tab：登录拦截 + “返回后跳到指定 Tab”

- 主动点击 Tab：在 `TabBuilder` 中对购物车 Tab 做登录拦截。
- 从子页面返回：通过 `AppStorage(indexTargetTab)` 指定目标 Tab；首页可见时读取并切换，完成后将其重置为 `-1` 避免重复触发。

```typescript
private applyTargetTab() {
  const raw = AppStorage.get('indexTargetTab') as number | undefined
  const next = raw === undefined ? NaN : Number(raw)
  if (!Number.isFinite(next) || next < 0) return

  if (next === 1 && !this.currentUser) {
    promptAction.showToast({ message: '请先登录' })
    router.pushUrl({ url: 'pages/LoginPage' })
    AppStorage.setOrCreate('indexTargetTab', -1)
    return
  }

  this.currentIndex = next
  this.tabsController.changeIndex(next)
  AppStorage.setOrCreate('indexTargetTab', -1)
}

.onClick(() => {
  if (targetIndex === 1 && !this.currentUser) {
    promptAction.showToast({ message: '请先登录' })
    router.pushUrl({ url: 'pages/LoginPage' })
    return
  }
  this.currentIndex = targetIndex
  this.tabsController.changeIndex(this.currentIndex)
})

.onVisibleAreaChange([0.0, 1.0], (isVisible) => {
  if (isVisible) this.applyTargetTab()
})
```

## 商品详情（ProductDetail）

商品详情页由首页商品卡片点击进入，通过路由参数接收 `product`。页面显示时（`aboutToAppear()`）会完成取参，并做两类“进入即初始化”：

- 同步收藏状态：登录且商品 id 有效时调用 `RdbUtil.isFavorite(userId, productId)` 刷新 `isFavorite`
- 记录足迹：登录且商品 id 有效时调用 `RdbUtil.addFootprint(userId, productId)`

```typescript
aboutToAppear() {
  const params = router.getParams() as Record<string, Object>
  if (params && params['product']) {
    this.product = params['product'] as Product
  }
  this.refreshFavoriteWrapper()
  this.addFootprintWrapper()
}

async refreshFavorite() {
  if (!this.currentUser || !this.product || this.product.id <= 0) {
    this.isFavorite = false
    return
  }
  this.isFavorite = await RdbUtil.isFavorite(this.currentUser.id, this.product.id)
}

async addFootprint() {
  if (!this.currentUser || !this.product || this.product.id <= 0) return
  await RdbUtil.addFootprint(this.currentUser.id, this.product.id)
}
```

页面交互围绕商品展开：

- 评论：携带当前 `product` 跳转到 `pages/CommentList`
- 收藏：未登录会 toast 并跳转登录；登录后调用 `RdbUtil.toggleFavorite(userId, productId)` 切换状态，`isFavorite` 驱动“★/☆”与颜色
- 加购/立即购买：按钮会根据 `product.stock` 决定是否可用；两者都需要登录并写入购物车。区别是“立即购买”会设置 `indexTargetTab=1` 并返回，让首页自动切到购物车 Tab 继续结算

```typescript
router.pushUrl({ url: 'pages/CommentList', params: { product: this.product } })

async toggleFavorite() {
  if (!this.currentUser) {
    promptAction.showToast({ message: '请先登录' })
    router.pushUrl({ url: 'pages/LoginPage' })
    return
  }
  this.isFavorite = await RdbUtil.toggleFavorite(this.currentUser.id, this.product.id)
  promptAction.showToast({ message: this.isFavorite ? '已收藏' : '已取消收藏' })
}

Button('加入购物车')
  .enabled(this.product.stock > 0)
  .onClick(async () => {
    if (!this.currentUser) {
      promptAction.showToast({ message: '请先登录' })
      router.pushUrl({ url: 'pages/LoginPage' })
      return
    }
    await RdbUtil.addToCart(new CartItem(0, this.product.id, this.product.name, this.product.price, 1, true, this.product.image))
    promptAction.showToast({ message: '已加入购物车' })
  })

Button('立即购买')
  .enabled(this.product.stock > 0)
  .onClick(async () => {
    if (!this.currentUser) {
      promptAction.showToast({ message: '请先登录' })
      router.pushUrl({ url: 'pages/LoginPage' })
      return
    }
    await RdbUtil.addToCart(new CartItem(0, this.product.id, this.product.name, this.product.price, 1, true, this.product.image))
    AppStorage.setOrCreate('indexTargetTab', 1)
    router.back()
    promptAction.showToast({ message: '请去购物车结算' })
  })
```

另外，底部“购物车”快捷入口不做加购，它只设置 `indexTargetTab=1` 并返回，用来从详情页直接回到首页的购物车 Tab：

```typescript
AppStorage.setOrCreate('indexTargetTab', 1)
router.back()
```

## 购物车（CartView）

购物车页首先做登录态拦截：页面出现时如果没有 `currentUser`，会清空列表、toast 提示，并把 Tab 切回首页后跳转登录页；登录后再通过 `RdbUtil.getCartItems()` 拉取购物车数据。

```typescript
async aboutToAppear() {
  if (!this.currentUser) {
    this.cartList = []
    promptAction.showToast({ message: '请先登录' })
    this.onSwitchTab(0)
    router.pushUrl({ url: 'pages/LoginPage' })
    return
  }
  await this.refreshCart()
}

async refreshCart() {
  if (!this.currentUser) {
    this.cartList = []
    return
  }
  this.cartList = await RdbUtil.getCartItems()
}
```

核心交互包括：

- 单选：勾选某个商品后更新 `selected`，调用 `RdbUtil.updateCartItem(item)` 持久化，再刷新列表
- 数量修改：`-` 在数量为 1 时会直接删除该条目，否则递减并更新；`+` 递增并更新
- 滑动删除：非管理模式下启用 `swipeAction`，点击“删除”调用 `RdbUtil.deleteCartItem(id)`
- 全选：底部全选 Checkbox 会批量更新所有条目的 `selected`，并用“同值保护”避免重复触发

```typescript
Checkbox()
  .select(item.selected)
  .onChange(async (val) => {
    item.selected = val
    await RdbUtil.updateCartItem(item)
    await this.refreshCart()
  })

Checkbox({ name: 'all', group: 'cart_all' })
  .select(this.isAllSelected())
  .onChange(async (val) => {
    if (val === this.isAllSelected()) return
    for (let it of this.cartList) {
      it.selected = val
      await RdbUtil.updateCartItem(it)
    }
    await this.refreshCart()
  })
```

结算逻辑会读取已勾选商品，调用 `RdbUtil.checkout(selectedItems, userId)`：内部会校验代金券余额与库存，成功后生成订单与明细并清理已结算的购物车条目（RDB 模式下用事务保证原子性）。页面侧在成功后会重新拉取用户信息写回 `AppStorage(currentUser)`，并刷新购物车：

```typescript
const selectedItems = this.cartList.filter(i => i.selected)
const success = await RdbUtil.checkout(selectedItems, this.currentUser.id)
if (success) {
  const nextUser = await RdbUtil.getUserById(this.currentUser.id)
  if (nextUser) {
    AppStorage.setOrCreate('currentUser', nextUser)
  }
  await this.refreshCart()
}
```

另外，管理模式下底部按钮会切换为“删除(已选数量)”，通过循环调用 `RdbUtil.deleteCartItem()` 批量删除勾选项。

## 个人中心（MineView）与订单（OrderList / OrderDetail）

“个人中心”页负责聚合个人入口，并统一做登录态拦截：涉及订单/地址/收藏/足迹等功能时，如果 `currentUser` 为空会提示并跳转登录。已登录时会在头部展示代金券余额（结算会扣减该余额）。

订单入口通过路由参数 `tab` 指定默认筛选。`OrderList` 在 `aboutToAppear()` 读取该参数，将其映射为订单状态码过滤条件，并调用 `RdbUtil.getOrderDetails(userId, status)` 拉取“订单+明细”；点击订单卡片会携带 `orderId` 进入 `OrderDetail`。

`OrderDetail` 在展示时读取 `orderId` 并通过 `RdbUtil.getOrderDetail(orderId, userId)` 拉取详情。。

```typescript
Text(`代金券余额 ¥${this.currentUser.voucherBalance.toFixed(2)}`)
router.pushUrl({ url: 'pages/OrderList', params: { tab: 2 } })

const t = params && params['tab'] ? Number(params['tab']) : 0
const status = this.getStatusFilter()
this.orders = (await RdbUtil.getOrderDetails(this.currentUser.id, status)).slice()
router.pushUrl({ url: 'pages/OrderDetail', params: { orderId: o.order.id } })

const oid = params && params['orderId'] ? String(params['orderId']) : ''
this.detail = await RdbUtil.getOrderDetail(this.orderId, this.currentUser.id)
const ok = await RdbUtil.updateOrderStatus(this.detail.order.id, nextStatus)
```

## 评论（CommentList）

评论页通过路由参数接收 `product`，并使用 `AppStorage(commentStore)` 做本地存储。初始化时会为该商品注入种子评论，之后所有操作都会写回存储。

- 取参与加载：
  
```typescript
aboutToAppear() {
  const params = router.getParams() as Record<string, Object>
  if (params && params['product']) this.product = params['product'] as Product
  this.loadComments()
}

private loadComments() {
  const key = (this.product?.id ?? 0).toString()
  const store = (AppStorage.get('commentStore') as CommentStore | undefined) || {}
  if (!store[key] || store[key].length === 0) store[key] = this.buildSeedComments()
  AppStorage.setOrCreate('commentStore', store)
  this.comments = (store[key] || []).slice()
}
  ```
- 发表评论/回复与删除（需登录，且删除仅限用户本人）：
```typescript
private send() {
  if (!this.ensureLoggedIn()) return
  const text = (this.inputText || '').trim()
  if (!text) return
  const isReply = !!this.replyToId && !!this.replyToName
  const now = Date.now()
  const item: CommentItem = {
    id: `${now}`,
    userName: this.currentUser ? this.currentUser.username : '',
    content: isReply ? `回复@${this.replyToName}：${text}` : text,
    createTime: now,
    likeCount: 0,
    liked: false,
    replyToName: isReply ? this.replyToName : undefined,
    parentId: isReply ? this.replyToId : undefined
  }
  const next = this.comments.slice()
  if (isReply) {
    const idx = next.findIndex(c => c.id === this.replyToId)
    if (idx >= 0) next.splice(idx + 1, 0, item); else next.unshift(item)
  } else {
    next.unshift(item)
  }
  this.comments = next
  this.inputText = ''
  this.clearReplyTarget()
  this.persist()
}

private deleteById(id: string) {
  if (!this.ensureLoggedIn()) return
  const idx = this.comments.findIndex(c => c.id === id)
  if (idx < 0) return
  const target = this.comments[idx]
  if (!this.isMyComment(target)) { promptAction.showToast({ message: '只能删除自己的评论' }); return }
  this.comments = this.comments.filter(c => c.id !== id && c.parentId !== id)
  this.persist()
}
```

## 本地数据存储（RdbUtil）

项目使用 `@kit.ArkData` 的 `relationalStore` 做本地持久化，应用启动时由 `EntryAbility` 初始化数据库；`RdbUtil` 内部缓存 `rdbStore/context`，所有读写优先走 RDB，预览器/无库/异常场景则使用 `memUsers/memProducts/memCart/...` 作为内存兜底。

数据库表名集中在 `Constants.ets`，并由 `RdbUtil.createTables()` 通过 `CREATE TABLE IF NOT EXISTS` 初始化。

```typescript
await RdbUtil.initDb(this.context)

static async initDb(context: common.Context): Promise<void> {
  RdbUtil.context = context;
  if (RdbUtil.rdbStore) return;
  const config: relationalStore.StoreConfig = {
    name: Constants.DB_NAME,
    securityLevel: relationalStore.SecurityLevel.S1
  };
  RdbUtil.rdbStore = await relationalStore.getRdbStore(context, config);
  await RdbUtil.createTables();
}

private static async getStore(): Promise<relationalStore.RdbStore | null> {
  if (RdbUtil.rdbStore) return RdbUtil.rdbStore;
  if (RdbUtil.context) {
    await RdbUtil.initDb(RdbUtil.context);
    return RdbUtil.rdbStore;
  }
  return null;
}

static resolveImageSource(src: string | Resource): string | Resource {
  if (typeof src !== 'string') return src;
  const key = (src || '').trim();
  if (!key || key.indexOf('[object Object]') >= 0) return $r('app.media.startIcon');
  if (key.startsWith('app.media.')) return $r(key);
  return key;
}
```

## 页面路由

应用页面入口在 `entry/src/main/resources/base/profile/main_pages.json` 中配置：

- pages/Index
- pages/LoginPage
- pages/ProductDetail
- pages/CommentList
- pages/AddressManage
- pages/AddressEdit
- pages/FavoriteList
- pages/FootprintList
- pages/OrderList
- pages/OrderDetail
