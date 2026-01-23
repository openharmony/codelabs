# Project Handover Protocol (v1.1 - Release Candidate)

## 1. 项目概况

* **项目名称**: **PersonalAssistantPro (个人助手Pro)**
* **定位**: 企业级 OpenHarmony 综合事务管理解决方案。
* **规模**: 代码总量 **> 6,300 行** (TypeScript/ArkTS)，采用高内聚低耦合的分层架构。
* **目标**: 构建一个代码量达标的 OpenHarmony 综合示例应用，展示数据持久化、UI 交互、后台服务及安全机制。
* **技术栈**: OpenHarmony API 20 (6.0), ArkTS, ArkUI, RDB, Preferences, UserIAM, CryptoFramework, ReminderAgent.
* **适配设备**: Phone (直板/折叠), Tablet, 2in1 (PC 模式), Car (车机).
* **当前版本**: v1.1.0 (RC)

## 2. 核心功能与架构亮点

本项目不仅实现了基础业务，更为了满足企业级健壮性（以及代码量要求），构建了完整的**基础设施**与**质量保障体系**。

| ID | 模块域 | 功能特性 | 技术实现与价值 |
| --- | --- | --- | --- |
| **F1** | **交互体验** | **搜索功能 & 冷启动** | 支持联系人/日程模糊查询；**[新增]** 采用 Layered Icon (分层图标) 技术，实现“蓝底白标”的沉浸式无缝启动体验。 |
| **F2** | **数据存储** | **数据库存储 (RDB)** | 核心库封装 `RdbHelper`，支持动态建表，CRUD 接口完备，支持多字段模糊匹配。 |
| **F3** | **数据服务** | **多格式报表导出** | **[新增]** 不仅支持 JSON 备份，更实现了 **HTML (含CSS)/XML/CSV** 格式的业务报表生成引擎 (`DataExportService`)。 |
| **F4** | **通知提醒** | **通知服务** | 集成 `reminderAgentManager`，支持后台定时提醒；日程编辑页集成通知开关，实现自动更新逻辑。 |
| **F5** | **隐私安全** | **联系人管理 & 加密** | 列表/详情/增删；敏感字段 (Phone) 采用 **AES-256 加密**落库；结合 **生物识别 (UserIAM)** 实现指纹解锁查看隐私详情。
| **F6** | **视图组件** | **高性能日历组件** | **[新增]** 抛弃原生简单组件，自主封装 `MonthView`/`WeekView`，配合 `ViewModel` 层实现复杂的数据驱动渲染。 |
| **F7** | **质量保障** | **自动化压力测试** | **[新增]** 研发独立的 `DebugRunner` 框架，包含针对数据库、加密算法及日期计算的 **1000+ 行** 压力测试用例。 |
| **F8** | **后端模拟** | **云同步仿真** | 实现 `MockNetworkService`，模拟真实网络延迟与数据分片上传，为未来对接云端解耦。 |
| **F9** | **全局状态** | **状态管理** | 使用 `AppStorage` 实现跨页面列表自动刷新，`MainPage` 作为唯一根容器管理路由栈。 |

## 3. 更新记录 (Changelog)

> 记录原则：倒序排列，包含关键代码变动与功能新增。

* **[2026-01-07] v1.1.0 {Release Candidate} - 品牌重塑与架构封板**
* **品牌升级**: App 正式更名为“个人助手Pro”；图标重构为**系统分层图标** (`layered_image`)，配置 `startWindow` 实现无缝启动。
* **架构定型**: 确立五大核心领域分工（Pages, View, Services, Common, Debug），通过服务化拆分（如 `DataExportService`）大幅提升代码可维护性。
* **代码合规**: 完成 6300+ 行代码的最终审计，确保所有模块（包括测试用例）均符合 ArkTS 严格模式。


* **[2026-01-06] v1.0.5 {数据增强} - 报表与服务层扩展**
* **数据导出**: 新增 `DataExportService`，支持生成带有内联 CSS 样式的 HTML 统计报表及标准 XML 数据交换格式。
* **模型增强**: `EventModel` 扩展 `queryAll` 及 `queryRange` 接口，支持复杂时间段的内存级筛选。
* **样式重构**: 引入 `StyleConstants` 语义化颜色变量，修复 UI 硬编码问题。


* **[2026-01-05] v1.0.0 {功能完备} - 搜索、备份与修复**
* **搜索功能 (F1)**:
* 后端：`RdbHelper` 新增 `queryByLike` 接口，支持多字段模糊匹配。
* 前端：联系人与日程列表集成 `Search` 组件，实现了实时/回车搜索逻辑。


* **模拟云同步 (F3)**:
* 新增 `BackupService`，支持将数据库全量数据导出为 JSON 文件至应用沙箱。
* `MinePage` 新增数据管理入口，实现了“备份”与“恢复”的完整闭环。


* **Bug 修复 (Critical)**:
* 修复了 `EntryAbility` 中的**竞态条件 (Race Condition)**，确保在运行 Debug Case 前等待 RDB 和 Crypto 初始化完成。
* 修复了 `BackupService` 和 `MinePage` 在 ArkTS 严格模式下的类型报错 (`throw unknown`, `catch type annotation`).




* **[2026-01-04] v0.6.0 {服务集成} - 后台通知与数据库升级**
* **通知服务 (F4)**: 新增 `NotificationService`，基于 `reminderAgentManager` 实现日历提醒；解决 Strict Mode 类型定义缺失问题。
* **数据库**: `event` 表新增 `reminder_id` 字段。
* **业务**: 日程编辑页集成通知开关，实现“编辑旧日程自动更新提醒”逻辑。


* **[2026-01-01] v0.5.0 {架构修复} - Navigation 重构**
* **架构**: 确立 `MainPage` 为唯一根容器，修复多层嵌套导致的闪退。
* **生物识别 (F10)**: 新增 `MinePage`，并在联系人详情页实现指纹解锁闭环。


* **[2025-12-31] v0.4.0 {安全增强} - 隐私加密**
* **加密 (F6)**: 集成 `AesCryptoUtils`，实现联系人电话号码加密存储。



## 4. 技术规范与设计指南 (Design Specifications)

**⚠️ 强制规范：违反以下条款将被视为 Critical Defect。**

### 4.1 ArkTS 严格模式规范 (Strict Mode)

* **错误处理**: `catch(err)` 禁止添加类型注解（如 `: any` 或 `: ESObject`），必须在块内使用类型断言 (`as BusinessError` 或 `as object`) 或 `JSON.stringify(err)` 处理。
* **异常抛出**: 禁止 `throw` 任意类型，必须抛出 `Error` 或其子类实例。
* **类型断言**: SDK 类型定义缺失时，必须使用 `as ESObject` 规避编译器检查。
* **静态方法**: 在类的方法（尤其是 async）中，禁止在静态上下文使用 `this` 调用**其余**静态方法，必须使用**类名**调用（如 `EventModel.queryAll()`）。

### 4.2 架构与分层规范 (Architecture)

* **路由架构**:
* **单一入口**: 仅 `MainPage` 使用 `@Entry`。
* **单一容器**: 全局仅维护一个 `Navigation` 组件，位于 `MainPage`。
* **路由栈**: 通过 `@Provide('pageStack')` 和 `@Consume('pageStack')` 在组件树中共享 `NavPathStack`。


* **关注点分离**:
* **Pages**: 仅负责路由与容器组装，不包含复杂逻辑。
* **View/ViewModel**: 负责 UI 绘制与状态流转。
* **Services/Model**: 负责数据持久化、复杂算法及对外服务。
* **Common**: 提供纯函数工具链。



### 4.3 数据库与安全规范

* **初始化**: RDB 和 Crypto Key 的初始化必须在 `EntryAbility` 或 `SplashPage` 确保完成。
* **隐私字段**: 联系人 `phone` 字段入库前必须加密，出库后解密；搜索时受限于加密，目前仅支持姓名/邮箱/关系字段的模糊搜索。

## 5. 遗留问题与未来规划 (Backlog)

1. **[Infrastructure] 真实云端对接**:
* 当前 F3/F8 仅为本地文件模拟和网络层仿真 (`MockNetworkService`)，未来计划对接华为云对象存储或自建 RESTful API。


2. **[Low] 通知交互体验**:
* 目前点击系统通知栏可能无法精确跳转到对应的详情页（受限于不同 API 版本的 `WantAgent` 参数差异），维持基本提醒功能。


3. **[UI/UX] 搜索高亮**:
* 当前搜索结果未对匹配的关键词进行高亮显示，可作为后续体验优化点。


4. **[Performance] 列表虚拟滚动**:
* 在压力测试生成 10,000 条数据后，长列表渲染性能有优化空间，计划引入 `LazyForEach`。


## 6. 当前文件架构 (File Structure)

```text
=== VISUAL TREE & LINE COUNTS ===
└── PersonalAssistantPro [DIR: 6427 lines]
    ├── entry [DIR: 6162 lines]
    │   ├── src [DIR: 6118 lines]
    │   │   └── main [DIR: 6118 lines]
    │   │       ├── ets [DIR: 6057 lines]
    │   │       │   ├── common [DIR: 1008 lines]
    │   │       │   │   ├── constants [DIR: 122 lines]
    │   │       │   │   │   ├── CommonConstants.ets [43 lines]
    │   │       │   │   │   └── StyleConstants.ets [79 lines]
    │   │       │   │   ├── database [DIR: 139 lines]
    │   │       │   │   │   ├── RdbHelper.ts [103 lines]
    │   │       │   │   │   └── Tables.ts [36 lines]
    │   │       │   │   └── utils [DIR: 747 lines]
    │   │       │   │       ├── AesCryptoUtils.ts [124 lines]
    │   │       │   │       ├── DateUtils.ets [111 lines]
    │   │       │   │       ├── Logger.ts [149 lines]
    │   │       │   │       ├── PermissionUtils.ets [81 lines]
    │   │       │   │       ├── ResourceUtils.ets [95 lines]
    │   │       │   │       ├── StringUtils.ts [108 lines]
    │   │       │   │       └── ValidationUtils.ts [79 lines]
    │   │       │   ├── debug [DIR: 1114 lines]
    │   │       │   │   ├── BackupRestoreDebugCase.ets [101 lines]
    │   │       │   │   ├── BiometricSettingCase.ets [60 lines]
    │   │       │   │   ├── ContactDatabaseCase.ts [63 lines]
    │   │       │   │   ├── ContactStressTestCase.ts [87 lines]
    │   │       │   │   ├── DateAndConfigStressTestCase.ets [124 lines]
    │   │       │   │   ├── DebugCase.ts [33 lines]
    │   │       │   │   ├── DebugRunner.ets [86 lines]
    │   │       │   │   ├── EncryptionPrivacyCase.ts [81 lines]
    │   │       │   │   ├── EventAlgoTestCase.ets [96 lines]
    │   │       │   │   ├── ExampleDebugCase.ts [43 lines]
    │   │       │   │   ├── SearchDebugCase.ets [72 lines]
    │   │       │   │   ├── SecurityStressTestCase.ts [124 lines]
    │   │       │   │   └── UserScenarioSimulationCase.ets [144 lines]
    │   │       │   ├── entryability [DIR: 74 lines]
    │   │       │   │   └── EntryAbility.ets [74 lines]
    │   │       │   ├── model [DIR: 347 lines]
    │   │       │   │   ├── ContactModel.ts [142 lines]
    │   │       │   │   ├── EventModel.ets [172 lines]
    │   │       │   │   └── SettingModel.ets [33 lines]
    │   │       │   ├── pages [DIR: 1653 lines]
    │   │       │   │   ├── ContactDetailPage.ets [192 lines]
    │   │       │   │   ├── ContactListPage.ets [273 lines]
    │   │       │   │   ├── EventDetailPage.ets [210 lines]
    │   │       │   │   ├── EventEditPage.ets [239 lines]
    │   │       │   │   ├── EventListPage.ets [220 lines]
    │   │       │   │   ├── Index.ets [23 lines]
    │   │       │   │   ├── MainPage.ets [87 lines]
    │   │       │   │   ├── MinePage.ets [211 lines]
    │   │       │   │   ├── SettingsPage.ets [98 lines]
    │   │       │   │   └── SplashPage.ets [100 lines]
    │   │       │   ├── services [DIR: 804 lines]
    │   │       │   │   ├── BackupService.ets [107 lines]
    │   │       │   │   ├── BiometricService.ets [87 lines]
    │   │       │   │   ├── DataExportService.ets [339 lines]
    │   │       │   │   ├── JumpService.ets [102 lines]
    │   │       │   │   ├── NotificationService.ets [98 lines]
    │   │       │   │   └── PreferenceService.ets [71 lines]
    │   │       │   ├── view [DIR: 678 lines]
    │   │       │   │   ├── calendar [DIR: 287 lines]
    │   │       │   │   │   ├── DayItemView.ets [72 lines]
    │   │       │   │   │   ├── MonthView.ets [105 lines]
    │   │       │   │   │   └── WeekView.ets [110 lines]
    │   │       │   │   ├── component [DIR: 193 lines]
    │   │       │   │   │   ├── CustomDialog.ets [126 lines]
    │   │       │   │   │   └── TitleBar.ets [67 lines]
    │   │       │   │   └── form [DIR: 198 lines]
    │   │       │   │       ├── DatePickerItem.ets [140 lines]
    │   │       │   │       └── TextInputItem.ets [58 lines]
    │   │       │   └── viewmodel [DIR: 379 lines]
    │   │       │       ├── CalendarViewModel.ets [135 lines]
    │   │       │       ├── ContactViewModel.ts [119 lines]
    │   │       │       └── EventViewModel.ets [125 lines]
    │   │       └── module.json5 [61 lines]
    │   ├── build-profile.json5 [33 lines]
    │   └── oh-package.json5 [11 lines]
    ├── hvigor [DIR: 24 lines]
    │   └── hvigor-config.json5 [24 lines]
    ├── build-profile.json5 [56 lines]
    ├── oh-package.json5 [11 lines]
    └── PROJECT_CONTEXT.md [174 lines]

```