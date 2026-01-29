# 📦 模块四：数据安全模块（Security Module）

本模块为 **DeviceHealth 项目** 提供统一的数据加解密能力，用于敏感日志与配置数据的安全存储。

> 模块目标：  
> **其他模块无需关注加密实现细节，只需调用接口即可完成加密 / 解密。**

---

## 一、模块职责

- 提供统一的 **文本加解密接口**
- 提供 **日志字段级加密 / 解密接口**
- 支持敏感字段（如日志 `detail / extra`）加密存储
- 与 UI、数据库模块解耦，作为底层安全能力模块

---

## 二、代码结构

```text
entry/src/main/ets/
├─ security/
│  ├─ CryptoUtil.ets        # 底层加解密工具（AES-GCM）
│  └─ SecurityService.ets   # 对外统一安全服务
│
├─ model/
│  └─ EncryptedData.ts      # 加密数据模型
│
├─ test/
│  ├─ CryptoUtilTest.ets
│  ├─ SecurityServiceTest.ets
│  └─ EncryptedDBTest.ets
```



## 三、核心接口说明（对外使用）
1️⃣ 文本加解密接口

适用于：配置数据、敏感字符串等。

```
SecurityService.encryptText(plainText: string): EncryptedData
SecurityService.decryptText(data: EncryptedData): string
```

2️⃣ 日志字段加解密接口（推荐使用）
```
日志明文结构
interface PlainEventLog {
  type: string
  level: number
  timestamp: number
  detail?: string
  extra?: string
}

加密日志字段（写数据库前）
SecurityService.encryptLogFields(log: PlainEventLog): EncryptedEventLogRow


仅加密 detail / extra

type / level / timestamp 保持明文，便于查询和统计

解密日志字段（从数据库读取后）
SecurityService.decryptLogFields(row: EncryptedEventLogRow): PlainEventLog
```
### 四、各模块使用建议
#### 模块 1：系统监控模块
```const log: PlainEventLog = {
  type: 'BATTERY',
  level: 1,
  timestamp: Date.now(),
  detail: 'Battery level low',
  extra: 'Current level: 15%'
}

// 写库前调用
const encryptedRow = SecurityService.encryptLogFields(log)
EventDB.insert(encryptedRow)
```
#### 模块 2：配置管理模块

```模块四 不管理开关

是否启用加密由配置项（如 isLogEncryptionOn）控制

if (settings.isLogEncryptionOn) {
  encryptedRow = SecurityService.encryptLogFields(log)
}
```

#### 模块 3：UI / 通知模块

```展示数据库日志前调用解密接口

const plainLog = SecurityService.decryptLogFields(dbRow)
```

### 模块 5 / 6：集成与测试

模块四已提供单元测试：

CryptoUtil

SecurityService

加密数据库流程

在 Previewer 环境下：

加密与编码接口为 mock 实现

测试采用 结构 + 流程验证

真机环境支持完整加解密能力
