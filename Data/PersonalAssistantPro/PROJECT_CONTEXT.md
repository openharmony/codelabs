# Project Handover Protocol (v1.0 - Feature Complete)

## 1. 项目概况

* **项目名称**: PersonalAssistantPro (个人事务助理专业版)
* **目标**: 构建一个代码量 > 5000 行的 OpenHarmony 综合示例应用，展示数据持久化、UI 交互、后台服务及安全机制。
* **技术栈**: OpenHarmony API 20 (6.0), ArkTS, ArkUI, RDB, Preferences, UserIAM, CryptoFramework.
* **适配设备**: Phone (直板/折叠), Tablet, 2in1 (PC 模式), Car (车机), Watch (部分精简功能).
* **当前版本**: v1.0.0

## 2. 核心功能与进度追踪

| ID | 功能点 | 对应需求 | 状态 | 备注 |
| --- | --- | --- | --- | --- |
| **F1** | **搜索功能** | 1 | [x] **已完成** | 支持联系人/日程模糊查询，UI 集成搜索栏 |
| **F2** | 数据库存储 (RDB) | 2 | [x] **已完成** | 核心库封装，支持动态建表，CRUD 接口完备 |
| **F3** | **模拟云同步** | 3 | [x] **已完成** | 实现 JSON 格式导出(备份)/导入(恢复)至沙箱 |
| **F4** | 通知提醒 | 4 | [x] **已完成** | 集成 `reminderAgentManager`，支持后台定时提醒 |
| **F5** | 联系人管理 | 5 | [x] **已完成** | 列表/详情/增删，集成隐私保护锁 |
| **F6** | 隐私加密 | 8 | [x] **已完成** | 敏感字段 (Phone) AES-256 加密存储 |
| **F9** | 全局状态管理 | 23 | [x] **已完成** | 使用 `AppStorage` 实现跨页面列表自动刷新 |
| **F10** | 生物认证 | 30 | [x] **已完成** | 指纹解锁查看隐私详情，设置页开关控制 |
| **F11** | 日程管理 (Event) | 2, 19 | [x] **已完成** | 增删改查，列表侧滑删除，关联通知提醒 |

## 3. 更新记录 (Changelog)

> 记录原则：倒序排列，包含关键代码变动与功能新增。

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
* **生物认证 (F10)**: 新增 `MinePage`，并在联系人详情页实现指纹解锁闭环。


* **[2025-12-31] v0.4.0 {安全增强} - 隐私加密**
* **加密 (F6)**: 集成 `AesCryptoUtils`，实现联系人电话号码加密存储。



## 4. 技术规范与设计指南 (Design Specifications)

**⚠️ 强制规范：违反以下条款将被视为 Critical Defect。**

### 4.1 ArkTS 严格模式规范 (Strict Mode)

* **错误处理**: `catch(err)` 禁止添加类型注解（如 `: any` 或 `: ESObject`），必须在块内使用类型断言 (`as BusinessError`) 或 `JSON.stringify(err)` 处理。
* **异常抛出**: 禁止 `throw` 任意类型，必须抛出 `Error` 或其子类实例 (e.g., `throw new Error(msg)`).
* **类型断言**: SDK 类型定义缺失时，必须使用 `as ESObject` 规避编译器检查。
* **静态方法**: 在类的方法（尤其是 async）中，禁止在静态上下文使用 `this` 调用其他静态方法，必须使用**类名**调用。

### 4.2 路由架构规范 (Navigation Architecture)

* **单一入口**: 仅 `MainPage` 使用 `@Entry`。
* **单一容器**: 全局仅维护一个 `Navigation` 组件，位于 `MainPage`。
* **路由栈**: 通过 `@Provide('pageStack')` 和 `@Consume('pageStack')` 在组件树中共享 `NavPathStack`。

### 4.3 数据库与安全规范

* **初始化**: RDB 和 Crypto Key 的初始化必须在 `EntryAbility` 或 `SplashPage` 确保完成，特别是涉及 Debug Case 运行时，需手动提前初始化。
* **隐私字段**: 联系人 `phone` 字段入库前必须加密，出库后解密；搜索时受限于加密，目前仅支持姓名/邮箱/关系字段的模糊搜索。

## 5. 遗留问题与未来规划 (Backlog)

1. **[Low] 通知交互体验**:
* 目前点击系统通知栏可能无法精确跳转到对应的详情页（受限于不同 API 版本的 `WantAgent` 参数差异），当前已跳过此优化，维持基本提醒功能。


2. **[Enhancement] 真实云同步**:
* 当前 F3 仅为本地文件模拟，未来可对接华为云空间或自建服务器 API。


3. **[UI/UX] 搜索高亮**:
* 当前搜索结果未对匹配的关键词进行高亮显示，可作为后续体验优化点。


## 6. 当前文件架构 (File Structure)

```text
└── PersonalAssistantPro
    ├── entry
    │   ├── src
    │   │   └── main
    │   │       ├── ets
    │   │       │   ├── common
    │   │       │   │   ├── constants
    │   │       │   │   │   ├── CommonConstants.ets [EMPTY]
    │   │       │   │   │   └── StyleConstants.ets [EMPTY]
    │   │       │   │   ├── database
    │   │       │   │   │   ├── RdbHelper.ts
    │   │       │   │   │   └── Tables.ts
    │   │       │   │   └── utils
    │   │       │   │       ├── AesCryptoUtils.ts
    │   │       │   │       ├── DateUtils.ets [EMPTY]
    │   │       │   │       ├── Logger.ts
    │   │       │   │       ├── PermissionUtils.ets [EMPTY]
    │   │       │   │       └── ResourceUtils.ets [EMPTY]
    │   │       │   ├── debug
    │   │       │   │   ├── BiometricSettingCase.ets
    │   │       │   │   ├── ContactDatabaseCase.ts
    │   │       │   │   ├── DebugCase.ts
    │   │       │   │   ├── DebugRunner.ets
    │   │       │   │   ├── EncryptionPrivacyCase.ts
    │   │       │   │   └── ExampleDebugCase.ts
    │   │       │   ├── entryability
    │   │       │   │   └── EntryAbility.ets
    │   │       │   ├── model
    │   │       │   │   ├── ContactModel.ts
    │   │       │   │   ├── EventModel.ets
    │   │       │   │   └── SettingModel.ets
    │   │       │   ├── pages
    │   │       │   │   ├── ContactDetailPage.ets
    │   │       │   │   ├── ContactListPage.ets
    │   │       │   │   ├── EventDetailPage.ets [EMPTY]
    │   │       │   │   ├── EventEditPage.ets
    │   │       │   │   ├── EventListPage.ets
    │   │       │   │   ├── Index.ets
    │   │       │   │   ├── MainPage.ets
    │   │       │   │   ├── MinePage.ets
    │   │       │   │   ├── SettingsPage.ets
    │   │       │   │   └── SplashPage.ets
    │   │       │   ├── services
    │   │       │   │   ├── BiometricService.ets
    │   │       │   │   ├── JumpService.ets [EMPTY]
    │   │       │   │   ├── NotificationService.ets
    │   │       │   │   └── PreferenceService.ets
    │   │       │   ├── view
    │   │       │   │   ├── calendar
    │   │       │   │   │   ├── DayItemView.ets [EMPTY]
    │   │       │   │   │   ├── MonthView.ets [EMPTY]
    │   │       │   │   │   └── WeekView.ets [EMPTY]
    │   │       │   │   ├── component
    │   │       │   │   │   ├── CustomDialog.ets [EMPTY]
    │   │       │   │   │   └── TitleBar.ets [EMPTY]
    │   │       │   │   └── form
    │   │       │   │       ├── DatePickerItem.ets [EMPTY]
    │   │       │   │       └── TextInputItem.ets [EMPTY]
    │   │       │   └── viewmodel
    │   │       │       ├── CalendarViewModel.ets [EMPTY]
    │   │       │       ├── ContactViewModel.ts [EMPTY]
    │   │       │       └── EventViewModel.ets [EMPTY]
    │   │       └── module.json5
    │   ├── build-profile.json5
    │   └── oh-package.json5
    ├── hvigor
    │   └── hvigor-config.json5
    ├── build-profile.json5
    ├── oh-package.json5
    ├── PROJECT_CONTEXT.md
    └── tree.ts

```