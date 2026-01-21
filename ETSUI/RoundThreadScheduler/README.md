# ArkTS 多线程轮次安全调度示例

## 一、示例简介

本示例基于 OpenHarmony ArkTS，演示如何在多线程环境下安全地对共享数据进行调度。  
示例通过 **Worker + SharedArrayBuffer + Atomics + Barrier + Semaphore** 的组合，实现了一个 **按轮次顺序执行的多线程调度模型**。

该示例适合 ArkTS 并发编程学习和教学，可用于理解线程安全、轮次同步以及并发限制。

---

## 二、实验功能

1. 创建 **10 个 Worker 线程**。
2. 每个线程对共享数字执行 **+1** 操作。
3. 执行 **10 轮**，保证每轮全部线程完成后再进入下一轮。
4. **同时最多 5 个线程访问共享变量**（Semaphore 限制）。
5. 程序结束后，最终共享数字为 **100**。
6. 提供 **UI 页面** 显示每轮轮次、线程执行状态以及日志信息。

---

## 三、技术要点

### 1. Worker 多线程模型
- 使用 ArkTS `@ohos.worker` 创建独立线程。
- 主线程负责调度任务，Worker 负责执行增量操作。

### 2. SharedArrayBuffer + Atomics
- 共享内存实现主线程和 Worker 之间的数据同步。
- 使用 `Atomics.add` 保证线程安全的增量操作。
- 使用 `Atomics.wait` 和 `Atomics.notify` 实现轮次同步。

### 3. Semaphore 信号量
- 限制同一时间访问共享变量的线程数量。
- 保证不会出现超过 5 个线程同时操作共享数据的情况。

### 4. Round Barrier 轮次屏障
- 每轮线程完成后，Barrier 通知主线程进入下一轮。
- 确保每轮执行的顺序性和完整性。

### 5. 日志系统
- 每个线程执行状态通过 Logger 记录。
- 日志同时输出到控制台和页面，便于教学演示。

---

## 四、工程结构

RoundThreadScheduler/
├── README.md                         
├── module.json5
├── build-profile.json5
└── entry/
    └── src/main/ets/
        ├── entryability/
        │   └── EntryAbility.ets     
        ├── pages/
        │   └── Index.ets             
        ├── constants/
        │   └── Config.ets            
        ├── model/
        │   ├── SharedState.ets       
        │   ├── RoundState.ets        
        │   └── ThreadLog.ets         
        ├── scheduler/
        │   ├── RoundBarrier.ets      
        │   ├── ThreadManager.ets     
        │   └── RoundScheduler.ets   
        ├── workers/
        │   └── IncrementWorker.ets  
        └── utils/
            └── Logger.ets            

---

## 五、运行方式

1. 使用 **DevEco Studio** 打开本示例工程。
2. 编译并运行到模拟器或真机。
3. 打开主页面 **Index.ets**，点击 **启动调度** 按钮。
4. 页面展示每轮线程执行状态、日志和轮次统计。
5. 验证最终共享数字为 **100**。

---

## 六、适用场景

- ArkTS 并发编程学习。
- 多线程同步机制教学。
- OpenHarmony 应用开发示例参考。
- 演示 Barrier 和 Semaphore 在 ArkTS 的实现方法。

---

## 七、许可说明

本示例遵循 **Apache License 2.0**，可以自由参考和修改。
