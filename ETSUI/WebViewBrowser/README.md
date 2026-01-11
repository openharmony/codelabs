# WebViewBrowser

Web组件示例，使用内置 Web 组件实现轻量级多标签浏览器。核心逻辑位于 entry 模块。

## 功能特点
- 多标签浏览，标签列表弹窗支持快速切换与隐私模式开关。
- 工具栏：前进、后退、刷新；地址栏支持自动补全 https://，提供主页快捷按钮。
- Web 配置：深色模式与强制深色、JavaScript 与图片开关、DOM Storage 与地理定位访问、文本缩放、缓存/ Cookie 清理。
- 设置持久化：通过 preferences 保存，运行时监听变更并即时应用。
- 加载与导航状态日志，便于排查页面加载错误。

## 目录结构（关键部分）
```
AppScope/            # 应用配置与资源
entry/               # 主入口模块
  src/main/ets/      # ArkTS 代码
    entryability/    # Ability 启动入口
    pages/           # UI 页面（Index 主页面）
  src/main/resources # 资源文件
hvigor/              # 构建配置
```

## 重要文件
- [entry/src/main/ets/pages/Index.ets](entry/src/main/ets/pages/Index.ets)：主浏览器 UI、标签管理与 Web 设置。
- [entry/src/main/module.json5](entry/src/main/module.json5)：模块元数据与权限（含 INTERNET）。
- [entry/hvigorfile.ts](entry/hvigorfile.ts)、[entry/build-profile.json5](entry/build-profile.json5)：模块构建配置。

## 环境要求
- DevEco Studio（Stage 模型，ArkTS）及 API 20。



## 使用说明
- 启动时从 preferences 读取主页、深色模式、强制深色、JS/图片开关、文本缩放、DOM Storage、地理定位、退出清缓存等设置，并在运行时监听变更。
- 在标签列表对话框切换隐私模式，仅对新建标签生效。
- 如启用“退出清缓存”偏好，关闭页面时自动调用缓存清理；Cookies 可按需手动清理。

## 测试
当前未提供自动化测试。已引入 Hypium 与 Hamock 作为开发依赖，可在 `entry/src/ohosTest` 或 `entry/src/test` 下添加用例。

## 常见问题
- 页面无法加载：检查设备/模拟器网络，确认 [entry/src/main/module.json5](entry/src/main/module.json5) 中仍声明 `ohos.permission.INTERNET`。
- 深色或其他设置未生效：确认 `browser_settings` 偏好文件可写，并确保 StorageLink 值在监听中更新。
# MyApplication

HarmonyOS ArkTS sample that implements a lightweight multi-tab browser using the built-in Web component. The main UI and logic live in the entry module.

## Features
- Multi-tab browsing with quick switcher dialog and incognito toggle.
- Toolbar navigation (back/forward/refresh), address bar with auto https:// prefix, and home shortcut.
- Per-tab Web settings: dark mode toggle with optional force-dark, JavaScript and image enable switches, DOM storage and geolocation access, text zoom ratio, and cache/cookie clearing.
- Persistent settings via preferences with runtime watchers to reflect user changes without restart.
- Basic error logging for page load failures and navigation state updates.

## Project Structure (key parts)
```
AppScope/            # App profile and resources
entry/               # Main entry module
  src/main/ets/      # ArkTS code
    entryability/    # Ability bootstrap
    pages/           # UI pages (Index main page)
  src/main/resources # App resources
hvigor/              # Build configuration
```

## Important Files
- [entry/src/main/ets/pages/Index.ets](entry/src/main/ets/pages/Index.ets): Main browser UI and tab management.
- [entry/src/main/module.json5](entry/src/main/module.json5): Module metadata and permissions (includes INTERNET).
- [entry/hvigorfile.ts](entry/hvigorfile.ts) and [entry/build-profile.json5](entry/build-profile.json5): Module build settings.

## Requirements
- DevEco Studio with HarmonyOS SDK (ArkTS/Stage model).
- Node.js environment for `ohpm` (installed with DevEco Studio).

## Setup
1) Install dependencies at the repo root:
```
cd MyApplication
ohpm install
```
2) (Optional) Install entry-specific dependencies if added later:
```
cd entry
ohpm install
```

## Build and Run
- From DevEco Studio: Open the project folder, select the `entry` module, and Run on a phone/emulator.
- From CLI (root directory):
```
./hvigorw clean
./hvigorw assembleHap --mode module --module entry
```
Outputs appear under `entry/build/default/outputs`.

## Usage Notes
- Home URL, dark mode, force-dark, JS/image enablement, text zoom, DOM storage, geolocation, and cache/cookie clearing are read from preferences when the app starts and watched at runtime.
- Toggling incognito on the tab list dialog applies to newly created tabs.
- Cache can be cleared on exit via the stored preference; cookies can be cleared on demand.

## Testing
No automated tests are currently defined. Hypium and Hamock are available as dev dependencies; add cases under `entry/src/ohosTest` or `entry/src/test` as needed.

## Troubleshooting
- If Web pages fail to load, confirm the device/emulator has network access and that `ohos.permission.INTERNET` remains declared in [entry/src/main/module.json5](entry/src/main/module.json5).
- If dark mode or other settings do not reflect, ensure the `browser_settings` preferences file is writable and the storage links are updated.
