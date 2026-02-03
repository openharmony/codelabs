
# 设备健康监测（DeviceHealth）

## 介绍

本篇 Codelab 基于 **OpenHarmony ArkTS Stage 模型**，实现了设备健康状态的实时监控与历史记录展示，涵盖以下功能：

* 电池状态监控与低电量预警
* 网络连接状态监控
* 存储空间监控与不足预警
* 屏幕状态变化监控
* 事件日志持久化存储与查询
* 用户配置管理（阈值、开关、通知等）

应用通过 **系统能力订阅 + 数据库存储 + UI 实时渲染** 的方式，实现设备状态的可视化监测。效果如下所示：

![](figures/DeviceHealth.gif)

---

### 相关概念

* **偏好（Preferences）**
  用于存储用户配置项，如监控开关、阈值、通知设置等，支持持久化保存。

* **关系型数据库（RDBStore）**
  使用 `@ohos.data.relationalStore` 实现事件日志的本地持久化存储，支持查询、分页与清理。

* **Stage 模型（UIAbility）**
  应用采用 Stage 模型架构，通过 UIAbility 管理生命周期与页面加载。

* **Router 路由机制**
  用于页面之间的跳转，如主页面进入各监控详情页。

* **系统能力 Provider**
  对系统能力（电池、网络、存储、屏幕）进行统一封装，向上提供订阅接口。

---

## 环境搭建

### 软件要求

* **DevEco Studio**：3.1 Release
* **OpenHarmony SDK**：API Version 9

### 硬件要求

* 设备类型：开发板或模拟器
* OpenHarmony 系统版本：3.2 Release 及以上

> 若仅进行 UI 预览，可使用 Previewer；完整功能验证需在模拟器或真机运行。

---

## 代码结构解读

```text
entry/src/main/ets
├── entryability
│   └── EntryAbility.ets          // 应用入口 Ability
├── pages
│   ├── MainPage.ets              // 主页面（监控入口聚合）
│   ├── SettingsPage.ets          // 设置页面
│   └── eventpages                // 监控详情页
│       ├── BatteryPage.ets
│       ├── NetworkPage.ets
│       ├── StoragePage.ets
│       └── ScreenPage.ets
├── services
│   ├── MonitorCoreService.ets    // 监控核心服务
│   ├── EventLogger.ets           // 事件日志记录器
│   └── monitors                  // 各类系统监控器
├── database
│   └── EventDB.ets               // 事件数据库封装
├── model
│   └── EventLog.ts               //  事件日志数据模型

├── providers
│   ├── BatteryProvider.ets
│   ├── NetworkProvider.ets
│   ├── StorageProvider.ets
│   └── ScreenProvider.ets
└── manager
    └── PreferenceManager.ets     // 偏好项管理器
```

---

## 应用初始化

应用启动后，由 `EntryAbility` 负责完成全局初始化工作，包括：

* 设置系统颜色模式
* 初始化 Preferences（用户配置）
* 初始化事件数据库
* 启动系统监控核心服务

```ts
// EntryAbility.ets
onCreate() {
  PreferenceManager.getInstance().init(this.context);
  EventHubManager.getInstance().init(this.context);
}
```

在应用进入前台时，通过 `MonitorCoreService.startAll()` 启动所有监控模块。

---

## 主页面（MainPage）

主页面作为应用入口，用于聚合展示各监控模块的入口卡片。

### 功能说明

* 展示电池 / 网络 / 存储 / 屏幕 / 设置入口
* 实时展示当前电池电量摘要
* 点击卡片跳转至对应详情页面

页面跳转通过 Router 实现：

```ts
this.getUIContext().getRouter().pushUrl({
  url: 'pages/eventpages/BatteryPage'
});
```

---

## 设置页面（SettingsPage）

设置页面用于统一管理用户配置项。

### 支持配置

* 电量预警阈值（Slider）
* 存储预警阈值（Slider）
* 网络监控开关
* 日志加密开关
* 通知提醒开关
* 恢复默认设置

### 技术实现

* 使用 `@StorageLink` 与 `AppStorage` 双向绑定
* 配置修改后通过 `PreferenceManager` 持久化到 Preferences
* 配置变化可被监控核心模块实时感知

---

## 电池监控（BatteryPage）

### 功能说明

* 展示当前设备电量百分比
* 低电量时（≤20%）高亮预警
* 展示最近 20 条电量变化日志

### 实现流程

1. 初始化 EventDB
2. 获取电量快照，避免首屏 0% 闪烁
3. 订阅电量变化
4. 每次变化后刷新历史日志列表

```ts
await this.batteryProvider.subscribe((soc: number) => {
  this.batteryLevel = soc;
  this.refreshHistory();
});
```

---

## 网络监控（NetworkPage）

### 功能说明

* 实时展示网络连接状态
* 展示网络状态切换历史记录
* 按事件级别（INFO / WARN）区分显示

### 数据来源

* NetworkProvider 获取系统网络状态
* NetworkMonitor 后台写入 EventDB
* 页面从数据库查询历史日志

---

## 存储监控（StoragePage）

### 功能说明

* 展示当前可用存储空间（GB）
* 使用环形进度条可视化存储占用情况
* 展示最近 15 条存储变更日志
* 提供手动刷新按钮

---

## 屏幕监控（ScreenPage）

### 功能说明

* 实时监听屏幕点亮 / 熄灭 / 锁定 / 解锁事件
* 顶部拟物化屏幕状态展示
* 时间线方式展示历史日志

---

## 事件日志持久化（EventDB）

所有监控事件统一封装为 EventLog 数据模型，并写入数据库。


### EventLog 数据结构


* type：事件类型（BATTERY / NETWORK / STORAGE / SCREEN）
* level：事件级别（INFO / WARN / ERROR）
* timestamp：事件时间戳
* message：事件描述
* payload：附加数据（JSON）

数据库支持：

* 自动建表与索引
* 按类型与时间查询
* 分页查询
* 历史日志清理

---

## 监控核心模块（MonitorCoreService）

`MonitorCoreService` 作为系统监控中枢，负责：

* 管理所有监控器生命周期
* 接收各监控器产生的事件
* 将事件写入数据库
* 触发全局告警事件（低电量 / 低存储）

并支持通过 `MonitorOptions` 注入 Provider 和存储实现，便于测试与扩展。