应用启动框架FunctionFlowRuntime
介绍

本demo使用c++/c语言封装了鸿蒙ffrt调度机制，并在接口层面实现了串行/并行调度选项，从而以demo方式，展示了并发编程和任务调度如何使用ffrt调度接口

目录结构展示：
young@yy:~/develop/FunctionFlowRuntime/entry$ tree
.
├── build-profile.json5
├── hvigorfile.ts
├── obfuscation-rules.txt
├── oh-package.json5
├── oh-package-lock.json5
└── src
    ├── main
    │   ├── cpp
    │   │   ├── BankQueueSystem.cpp
    │   │   ├── BankQueueSystem.h
    │   │   ├── CMakeLists.txt
    │   │   ├── common_ffrt.c
    │   │   ├── common_ffrt.h
    │   │   ├── compute_class.c
    │   │   ├── compute_class.h
    │   │   ├── function_class.c
    │   │   ├── FunctionClass.cpp
    │   │   ├── function_class.h
    │   │   ├── FunctionClass.h
    │   │   ├── napi_init.cpp
    │   │   ├── native_log_wrapper.h
    │   │   └── types
    │   │       └── libentry
    │   │           ├── Index.d.ts
    │   │           └── oh-package.json5
    │   ├── ets
    │   │   ├── entryability
    │   │   │   └── EntryAbility.ets
    │   │   ├── entrybackupability
    │   │   │   └── EntryBackupAbility.ets
    │   │   └── pages
    │   │       └── Index.ets
    │   ├── module.json5
    │   └── resources
    │       ├── base
    │       │   ├── element
    │       │   │   ├── color.json
    │       │   │   ├── float.json
    │       │   │   └── string.json
    │       │   ├── media
    │       │   │   ├── background.png
    │       │   │   ├── foreground.png
    │       │   │   ├── layered_image.json
    │       │   │   └── startIcon.png
    │       │   └── profile
    │       │       ├── backup_config.json
    │       │       └── main_pages.json
    │       └── dark
    │           └── element
    │               └── color.json
    ├── mock
    │   ├── Libentry.mock.ets
    │   └── mock-config.json5
    ├── ohosTest
    │   ├── ets
    │   │   └── test
    │   │       ├── Ability.test.ets
    │   │       └── List.test.ets
    │   └── module.json5
    └── test
        ├── List.test.ets
        └── LocalUnit.test.ets


compute_class.c展示了如何进行加减任务并行调度
function_class.c展示了如何进行睡眠任务串行调度
FunctionClass.cpp展示了如何进行睡眠任务并行和串行调度

