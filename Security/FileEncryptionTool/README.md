# **File Encryption Tool**

本项目为一个基于 OpenHarmony 的文件加密 / 解密、签名 / 验签示例工具。主要界面和功能代码位于 `entry/src/main/ets/tab`，包括四个主要页面：`Encrypt`、`Decrypt`、`Sign`、`Verify`。

**Prerequisites**
- **OpenHarmony / DevEco Studio**: 项目基于 OpenHarmony 应用框架开发，建议使用 DevEco Studio 打开并运行。
- **项目版本信息**: 模型版本 `6.0.0`（见 `oh-package.json5` 中 `modelVersion`），包版本 `1.0.0`。
- **依赖**: 使用 `@ohos/hypium`（在 `oh-package.json5` 中声明为 `1.0.6`）。
- **权限**: 应用请求了媒体读取/写入权限（`ohos.permission.READ_MEDIA` / `ohos.permission.WRITE_MEDIA`），运行时需授予以访问文件。

**快速开始**
- 在 DevEco Studio 中打开工程目录 `FileEncryptionTool`。
- 将项目部署到支持的 OpenHarmony 设备或模拟器上。
- 允许应用请求的文件读写权限后，打开应用即可看到主界面（四个标签页）。

**四个页面（用法说明）**
- **Encrypt（文件加密）:**
  - 选择“文件选择”项来挑选要加密的文件。
  - 选择或加载 AES 密钥文件（或使用“生成 AES 密钥”生成并保存密钥）。
  - 点击“加密”按钮，选择保存位置并保存加密文件。界面会显示加密结果摘要与文件信息。

- **Decrypt（文件解密）:**
  - 选择需要解密的加密文件（支持本项目生成的带元数据头的加密文件）。
  - 选择对应的 AES 密钥文件。
  - 点击“解密”，选择保存位置，完成后界面会显示解密结果、元数据信息和校验状态。

- **Sign（文件签名）:**
  - 选择要签名的文件并选择或加载 RSA 私钥（PEM 或明文文件，按项目中密钥读取方式提供）。
  - 点击“签名”，保存签名文件（通常为独立的签名文件），界面会显示签名十六进制摘要与签名结果。

- **Verify（文件验签）:**
  - 选择待验签的文件、RSA 公钥以及对应的签名文件。
  - 点击“验证”，界面会显示验签是否通过、算法信息与验证详情（用于确认文件完整性与来源）。

**文件与路径说明（项目内重要路径）**
- 主要代码: `entry/src/main/ets`（页面代码位于 `entry/src/main/ets/tab`）。
- 模块声明: `entry/src/main/module.json5`（包含权限与 ability 配置）。
- 项目根描述: `oh-package.json5`（包含 modelVersion 与依赖信息）。

**截图预览（位于 `screenshots/device`）**

- 主页面（加密）:
  ![FileEncrypt](screenshots/device/FileEncrypt.png)

- 主页面（解密）：
  ![FileEncrypt](screenshots/device/FileDecrypt.png)

- 主页面（签名）：
  ![FileSign](screenshots/device/FileSign.png)

- 主页面（验签）：
  ![FileVerify](screenshots/device/FileVerify.png)

- 加密成功:
  ![EncryptSuccess](screenshots/device/EncryptSucess.png)

- 解密成功:
  ![DecryptSuccess](screenshots/device/DecryptSucess.png)

- 签名成功:
  ![SignSuccess](screenshots/device/SignSucess.png)

- 验签成功:
  ![VerifySuccess](screenshots/device/VerifySucess.png)

- PNG 文件加密/解密对比证明（用于展示原图与解密后图像一致）:
  - 加密前效果图:               ![PNG Encrypt Result](screenshots/device/PNGUnEncryptPic.png)
  - 解密并还原后的效果图: ![PNG Decrypt Result](screenshots/device/PNGEncryptPic.png)
  
  上述两张图分别展示了对 PNG 文件进行加密与解密后的结果对比