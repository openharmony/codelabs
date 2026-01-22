# 课程表 (SchoolTimeTable)

## 简介
这是一个基于 OpenHarmony 开发的课程表应用，旨在为学生提供便捷的课程管理和日程查看功能。支持多周课程管理、自定义节次时间、导入导出数据以及深色模式等特性。

## 主要功能
- **课程管理**：支持添加、编辑、删除课程，设置课程名称、教师、地点、周次和节次。
- **日程查看**：提供按周查看和按日查看两种模式，清晰展示每日课程安排。
- **自定义设置**：支持自定义每节课的具体时间段（开始时间与结束时间）。
- **数据导入导出**：支持 csv 格式的课程数据导入与导出，方便数据备份与迁移。
- **搜索功能**：支持按课程名称、教师或地点快速搜索课程。
- **统计分析**：提供简单的课程统计功能（如每周课时数）。
- **个性化**：支持设置当前周次、学期开始日期等。

## 演示视频

[OpenHarmony课程管理器演示视频](./OpenHarmony课程管理器-演示视频.mp4)

## 开发环境
- **IDE**: DevEco Studio
- **OpenHarmony SDK**: API 12 (5.0.0 Release)
- **编程语言**: ArkTS

## 快速开始

1. **克隆仓库**
   ```bash
   git clone https://gitcode.com/m0_62639958/codelabs_SchoolTimeTable.git
   ```

2. **导入项目**
   - 打开 DevEco Studio。
   - 选择 `File` -> `Open`，导航到 `UI/SchoolTimeTable` 目录并打开。

3. **运行应用**
   - 连接 OpenHarmony 设备或启动模拟器。
   - 点击 IDE 上方的 `Run` 按钮运行项目。

## 目录结构
```
UI/SchoolTimeTable
├── AppScope       # 应用范围内的资源
├── entry          # 主入口模块
│   ├── src
│   │   ├── main
│   │   │   ├── ets
│   │   │   │   ├── components  # 自定义组件 (弹窗, 列表项等)
│   │   │   │   ├── model       # 数据模型 (Course, Settings)
│   │   │   │   ├── pages       # 应用页面 (首页, 设置, 详情等)
│   │   │   │   ├── service     # 业务逻辑服务 (数据持久化, 状态管理)
│   │   │   │   └── utils       # 工具类
│   │   │   └── resources       # 应用资源 (图片, 字符串, 颜色)
│   │   └── ohosTest            # 测试代码
├── build-profile.json5  # 构建配置文件
└── hvigorfile.ts        # Hvigor 构建脚本
```

## 贡献指南
欢迎提交 Issue 和 Pull Request 来改进此项目。

## 许可证
本项目遵循 Apache License 2.0 协议。
