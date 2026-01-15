/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import Logger from '../util/Logger';
import picker from '@ohos.file.picker';
import { CryptoOperation } from '../cryptoframework/CryptoOperation';
import TextFileManager from '../textfilemanager/TextFileManager';
import common from '@ohos.app.ability.common';
import window from '@ohos.window';
import { util } from '@kit.ArkTS';
import { MetadataHelper } from '../util/MetadataHelper';

const TAG: string = '[Crypto_Framework]';

// 定义加密结果接口
interface EncryptOperationResult {
  success: boolean;
  data?: ArrayBuffer;
  error?: string;
  tag?: ArrayBuffer;
}

@Component
export struct Encrypt {
  // 状态变量
  fileHash: string = '';
  originalExtension: string = '';
  isProcessing: boolean = false;

  @State keyFileName: string = '';
  @State keyFileUri: string = '';
  @State textFileUri: string = '';
  @State textFileName: string = '';
  @State keyString: string = '';
  @State cipherText: string = '';
  @State plainText: string = '';
  @State message: string = '';
  @State createKeyUri: string = '';
  @State encryptedFileUri: string = '';

  private CryptoOperation: CryptoOperation = new CryptoOperation();

  build() {
    Stack({ alignContent: Alignment.TopStart }) {
      // 背景渐变装饰层
      Column()
        .width('100%')
        .height('100%')
        .linearGradient({
          angle: 180,
          colors: [[0xF5F7FA, 0.0], [0xE8ECF1, 1.0]]
        })

      // 主内容区域
      Scroll() {
        Column({ space: 16 }) {
          // 顶部标题区域
          Row() {
            Column() {
              Text('文件加密')
                .fontSize(24)
                .fontWeight(600)
                .fontColor($r('sys.color.font_primary'))
                .lineHeight(32)
              
              Text('安全可靠的加密文件保护工具')
                .fontSize(14)
                .fontWeight(400)
                .fontColor($r('sys.color.font_secondary'))
                .margin({ top: 4 })
                .opacity(0.8)
            }
            .alignItems(HorizontalAlign.Center)
            .width('100%')
          }
          .width('100%')
          .padding({ left: 24, right: 24, top: 16, bottom: 8 })

          // 文件选择卡片区域
          GridRow() {
            GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
              Column() {
                // 卡片标题
                Row() {
                  Column() {
                    Text('文件选择')
                      .fontSize(18)
                      .fontWeight(600)
                      .fontColor($r('sys.color.font_primary'))
                      .lineHeight(24)
                  }
                  .alignItems(HorizontalAlign.Start)
                  .width('100%')
                }
                .width('100%')
                .padding({ left: 20, right: 20, top: 16, bottom: 12 })

                // 分隔线
                Divider()
                  .color(0xE5E5E5)
                  .strokeWidth(1)
                  .margin({ left: 20, right: 20 })

                // 文件选择列表
                Column({ space: 12 }) {
                  // 待加密文件选择项
                  Row() {
                    Column() {
                      Row() {
                        // 图标区域
                        Row() {
                          Text('📄')
                            .fontSize(20)
                            .fontWeight(400)
                        }
                        .width(40)
                        .height(40)
                        .borderRadius(8)
                        .backgroundColor(0xF0F4F8)
                        .justifyContent(FlexAlign.Center)

                        // 文本区域
                        Column({ space: 4 }) {
                          Text($r('app.string.open_file'))
                            .fontSize(16)
                            .fontWeight(500)
                            .fontColor($r('sys.color.font_primary'))
                            .textAlign(TextAlign.Start)
                            .lineHeight(22)

                          Text(this.textFileName === '' ? $r('app.string.please_choose') : this.textFileName)
                            .fontSize(13)
                            .fontWeight(400)
                            .fontColor(this.textFileName === '' ? $r('sys.color.font_secondary') : $r('sys.color.font_primary'))
                            .textAlign(TextAlign.Start)
                            .lineHeight(18)
                            .maxLines(1)
                            .textOverflow({ overflow: TextOverflow.Ellipsis })
                            .opacity(this.textFileName === '' ? 0.6 : 0.9)
                        }
                        .alignItems(HorizontalAlign.Start)
                        .layoutWeight(1)
                        .margin({ left: 12 })

                        // 箭头图标
                        Image($r('app.media.right_arrow'))
                          .height(20)
                          .width(20)
                          .margin({ left: 8 })
                          .opacity(0.5)
                      }
                      .width('100%')
                      .height('100%')
                    }
                    .width('100%')
                    .height('100%')
                  }
                  .width('100%')
                  .height(72)
                  .padding({ left: 20, right: 20, top: 12, bottom: 12 })
                  .backgroundColor(0xFFFFFF)
                  .borderRadius(12)
                  .shadow({
                    radius: 8,
                    color: 0x1A000000,
                    offsetX: 0,
                    offsetY: 2
                  })
                  .onClick(() => {
                    if (!this.isProcessing) {
                      this.selectTextFileAndRead();
                    }
                  })

                  // 密钥文件选择项
                  Row() {
                    Column() {
                      Row() {
                        // 图标区域
                        Row() {
                          Text('🔑')
                            .fontSize(20)
                            .fontWeight(400)
                        }
                        .width(40)
                        .height(40)
                        .borderRadius(8)
                        .backgroundColor(0xFFF8E1)
                        .justifyContent(FlexAlign.Center)

                        // 文本区域
                        Column({ space: 4 }) {
                          Text($r('app.string.select_key_file'))
                            .fontSize(16)
                            .fontWeight(500)
                            .fontColor($r('sys.color.font_primary'))
                            .textAlign(TextAlign.Start)
                            .lineHeight(22)

                          Text(this.keyFileName === '' ? $r('app.string.please_choose') : this.keyFileName)
                            .fontSize(13)
                            .fontWeight(400)
                            .fontColor(this.keyFileName === '' ? $r('sys.color.font_secondary') : $r('sys.color.font_primary'))
                            .textAlign(TextAlign.Start)
                            .lineHeight(18)
                            .maxLines(1)
                            .textOverflow({ overflow: TextOverflow.Ellipsis })
                            .opacity(this.keyFileName === '' ? 0.6 : 0.9)
                        }
                        .alignItems(HorizontalAlign.Start)
                        .layoutWeight(1)
                        .margin({ left: 12 })

                        // 箭头图标
                        Image($r('app.media.right_arrow'))
                          .height(20)
                          .width(20)
                          .margin({ left: 8 })
                          .opacity(0.5)
                      }
                      .width('100%')
                      .height('100%')
                    }
                    .width('100%')
                    .height('100%')
                  }
                  .width('100%')
                  .height(72)
                  .padding({ left: 20, right: 20, top: 12, bottom: 12 })
                  .backgroundColor(0xFFFFFF)
                  .borderRadius(12)
                  .shadow({
                    radius: 8,
                    color: 0x1A000000,
                    offsetX: 0,
                    offsetY: 2
                  })
                  .onClick(() => {
                    if (!this.isProcessing) {
                      this.selectAesKeyFileAndRead();
                    }
                  })
                }
                .width('100%')
                .padding({ top: 8, bottom: 16 })
              }
              .width('100%')
              .backgroundColor(0xFFFFFF)
              .borderRadius(20)
              .padding({ top: 0, bottom: 0 })
              .shadow({
                radius: 12,
                color: 0x1F000000,
                offsetX: 0,
                offsetY: 4
              })
            }
          }
          .width('100%')
          .margin({ left: 16, right: 16, top: 8, bottom: 8 })

          // 加密结果显示区域
          GridRow() {
            GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
              Column() {
                // 标题栏
                Row() {
                  Row() {
                    Text('🔒')
                      .fontSize(18)
                      .margin({ right: 8 })
                    Text($r('app.string.encrypted_context'))
                      .fontSize(18)
                      .fontWeight(600)
                      .fontColor($r('sys.color.font_primary'))
                      .lineHeight(24)
                  }
                  .alignItems(VerticalAlign.Center)
                  .width('100%')
                }
                .width('100%')
                .height(56)
                .padding({ left: 20, right: 20 })
                .justifyContent(FlexAlign.Start)

                // 分隔线
                Divider()
                  .color(0xE5E5E5)
                  .strokeWidth(1)
                  .margin({ left: 20, right: 20 })

                // 内容区域
                Column({ space: 12 }) {
                  Row() {
                    Scroll() {
                      Column({ space: 10 }) {
                        Text() {
                          Span(this.cipherText || '暂无加密数据')
                            .fontSize(15)
                            .fontWeight(400)
                            .fontColor($r('sys.color.font_primary'))
                        }
                        .textAlign(TextAlign.Start)
                        .lineHeight(22)
                        .maxLines(10)
                        .textOverflow({ overflow: TextOverflow.Ellipsis })
                      }
                      .width('100%')
                    }
                    .width('100%')
                    .height(140)
                    .scrollBar(BarState.Auto)
                  }
                  .width('100%')
                  .padding({ left: 20, right: 20, top: 8, bottom: 16 })
                }
                .width('100%')
              }
              .width('100%')
              .backgroundColor(0xFFFFFF)
              .borderRadius(20)
              .padding({ top: 0, bottom: 0 })
              .shadow({
                radius: 12,
                color: 0x1F000000,
                offsetX: 0,
                offsetY: 4
              })
            }
          }
          .width('100%')
          .margin({ left: 16, right: 16, top: 8, bottom: 8 })

          // 文件信息显示区域
          GridRow() {
            GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
              Column() {
                // 标题栏
                Row() {
                  Row() {
                    Text('ℹ️')
                      .fontSize(18)
                      .margin({ right: 8 })
                    Text($r('app.string.file_info'))
                      .fontSize(18)
                      .fontWeight(600)
                      .fontColor($r('sys.color.font_primary'))
                      .lineHeight(24)
                  }
                  .alignItems(VerticalAlign.Center)
                  .width('100%')
                }
                .width('100%')
                .height(56)
                .padding({ left: 20, right: 20 })
                .justifyContent(FlexAlign.Start)

                // 分隔线
                Divider()
                  .color(0xE5E5E5)
                  .strokeWidth(1)
                  .margin({ left: 20, right: 20 })

                // 信息项列表
                Column({ space: 16 }) {
                  // 原始格式显示
                  Row() {
                    Row() {
                      Text('📋')
                        .fontSize(16)
                        .margin({ right: 12 })
                      Column({ space: 2 }) {
                        Text('原始格式')
                          .fontSize(13)
                          .fontWeight(400)
                          .fontColor($r('sys.color.font_secondary'))
                          .opacity(0.7)
                        Text(this.originalExtension || '未知')
                          .fontSize(15)
                          .fontWeight(500)
                          .fontColor($r('sys.color.font_primary'))
                      }
                      .alignItems(HorizontalAlign.Start)
                    }
                    .width('100%')
                    .justifyContent(FlexAlign.Start)
                  }
                  .width('100%')
                  .height(48)
                  .padding({ left: 20, right: 20, bottom: 16 })
                }
                .width('100%')
                .padding({ top: 8 })
              }
              .width('100%')
              .backgroundColor(0xFFFFFF)
              .borderRadius(20)
              .padding({ top: 0, bottom: 0 })
              .shadow({
                radius: 12,
                color: 0x1F000000,
                offsetX: 0,
                offsetY: 4
              })
            }
          }
          .width('100%')
          .margin({ left: 16, right: 16, top: 8, bottom: 8 })

          // 操作按钮区域
          Column({ space: 16 }) {
            // 生成AES密钥按钮
            Button() {
              Row() {
                Text('🔑')
                  .fontSize(18)
                  .margin({ right: 8 })
                Text($r('app.string.generate_aes_key_randomly'))
                  .fontSize(16)
                  .fontWeight(600)
                  .lineHeight(22)
                  .fontColor($r('sys.color.comp_background_list_card'))
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
            }
            .id('encryptAesGenKey')
            .borderRadius(24)
            .type(ButtonType.Capsule)
            .width('100%')
            .height(52)
            .backgroundColor($r('sys.color.brand'))
            .enabled(!this.isProcessing)
            .opacity(!this.isProcessing ? 1.0 : 0.5)
            .shadow({
              radius: 8,
              color: 0x33000000,
              offsetX: 0,
              offsetY: 2
            })
            .onClick(() => {
              this.genAesKey();
            })

            // 加密按钮
            Button() {
              Row() {
                if (this.isProcessing) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('sys.color.comp_background_list_card'))
                    .margin({ right: 8 })
                } else {
                  Text('🔒')
                    .fontSize(18)
                    .margin({ right: 8 })
                }
                Text($r('app.string.encrypt'))
                  .fontSize(16)
                  .fontWeight(600)
                  .lineHeight(22)
                  .fontColor($r('sys.color.comp_background_list_card'))
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
            }
            .borderRadius(24)
            .id('encryptionBtn')
            .type(ButtonType.Capsule)
            .width('100%')
            .height(52)
            .backgroundColor($r('sys.color.brand'))
            .shadow({
              radius: 8,
              color: 0x33000000,
              offsetX: 0,
              offsetY: 2
            })
            .onClick(() => {
              if (this.textFileUri === '' || this.keyString === '') {
                this.getUIContext().getPromptAction().showToast({
                  message: $r('app.string.null_message')
                });
              } else {
                this.encryptFunc();
              }
            })

            // 处理状态指示器
            if (this.isProcessing) {
              Row() {
                LoadingProgress()
                  .width(24)
                  .height(24)
                  .color($r('sys.color.brand'))
                Text('正在处理中...')
                  .fontSize(14)
                  .fontWeight(400)
                  .fontColor($r('sys.color.font_secondary'))
                  .margin({ left: 12 })
                  .opacity(0.8)
              }
              .width('100%')
              .height(40)
              .justifyContent(FlexAlign.Center)
              .margin({ top: 8 })
            }
          }
          .width('100%')
          .padding({ left: 16, right: 16, top: 8, bottom: 24 })
        }
        .width('100%')
        .padding({ top: 8, bottom: 16 })
      }
      .width('100%')
      .height('100%')
      .scrollBar(BarState.Auto)
      .scrollable(ScrollDirection.Vertical)
    }
    .width('100%')
    .height('100%')
  }

  /**
   * 选择并读取密钥文件
   */
  async selectAesKeyFileAndRead() {
    this.isProcessing = true;
    try {
      let context = this.getUIContext().getHostContext() as common.UIAbilityContext;
      let documentPicker = new picker.DocumentViewPicker(context);
      let documentSelectOptions = new picker.DocumentSelectOptions();

      let uris = await documentPicker.select(documentSelectOptions);
      const windowClass = await window.getLastWindow(context);

      if (uris && uris.length > 0) {
        this.keyFileUri = uris[0];
        this.keyFileName = this.getFileNameFromUri(this.keyFileUri);

        // 使用TextFileManager读取密钥文件
        const readResult = await TextFileManager.readTextFile(this.keyFileUri);
        if (readResult.success) {
          try {
            const decoder = new util.TextDecoder('utf-8');
            const uint8 = new Uint8Array(readResult.data as ArrayBuffer);
            const dataSlice = uint8.slice(0, 1000);
            this.keyString = decoder.decode(dataSlice);
          } catch (decodeError) {
            Logger.error(TAG, `Binary decode error: ${decodeError.code}`);
            this.keyString = "密钥内容预览不可用";
          }
          windowClass.getUIContext().getPromptAction().showToast({
            message: '密钥文件读取成功'
          });
        } else {
          windowClass.getUIContext().getPromptAction().showToast({
            message: '密钥文件读取失败'
          });
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '未选择文件'
        });
      }
    } catch (error) {
      Logger.error(TAG, `selectAesKeyFileAndRead failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({
        message: '选择文件失败，请重试'
      });
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 选择并读取待加密的文件
   */
  async selectTextFileAndRead() {
    this.isProcessing = true;
    try {
      let context = this.getUIContext().getHostContext() as common.UIAbilityContext;
      let documentPicker = new picker.DocumentViewPicker(context);
      let documentSelectOptions = new picker.DocumentSelectOptions();

      let uris = await documentPicker.select(documentSelectOptions);
      const windowClass = await window.getLastWindow(context);

      if (uris && uris.length > 0) {
        this.textFileUri = uris[0];
        this.textFileName = this.getFileNameFromUri(this.textFileUri);
        this.originalExtension = MetadataHelper.getFileExtension(this.textFileName);

        // 使用TextFileManager读取文件
        const readResult = await TextFileManager.readTextFile(this.textFileUri);
        if (readResult.success) {
          try {
            const decoder = new util.TextDecoder('utf-8');
            const uint8 = new Uint8Array(readResult.data as ArrayBuffer);
            const dataSlice = uint8.slice(0, 1000);
            this.plainText = decoder.decode(dataSlice);
          } catch (decodeError) {
            Logger.error(TAG, `Binary decode error: ${decodeError.code}`);
            this.plainText = "二进制内容预览不可用";
          }
          windowClass.getUIContext().getPromptAction().showToast({
            message: '文件读取成功'
          });
        } else {
          windowClass.getUIContext().getPromptAction().showToast({
            message: '文件读取失败'
          });
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '未选择文件'
        });
      }
    } catch (error) {
      Logger.error(TAG, `selectTextFileAndRead failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({
        message: '选择文件失败，请重试'
      });
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 创建加密文件并写入内容（支持元数据头）
   */
  async createEncryptedFileAndWrite(encryptedData: ArrayBuffer, originalExtension?: string): Promise<boolean> {
    this.isProcessing = true;
    try {
      let context = this.getUIContext().getHostContext() as common.UIAbilityContext;
      let documentPicker = new picker.DocumentViewPicker(context);

      let documentSaveOptions = new picker.DocumentSaveOptions();

      // 使用 MetadataHelper 生成文件名
      const newFileName = MetadataHelper.generateMetadataFileName(
        this.textFileName,
        'encrypted',
        originalExtension || this.originalExtension
      );

      documentSaveOptions.newFileNames = [newFileName];

      let saveUris = await documentPicker.save(documentSaveOptions);
      const windowClass = await window.getLastWindow(context);

      if (saveUris && saveUris.length > 0) {
        this.encryptedFileUri = saveUris[0];

        // 使用TextFileManager写入二进制数据
        const writeResult = await TextFileManager.writeTextFile(this.encryptedFileUri, encryptedData);
        if (writeResult.success) {
          windowClass.getUIContext().getPromptAction().showToast({
            message: `加密文件保存成功，格式: ${originalExtension || '未知'}`
          });
          return true;
        } else {
          windowClass.getUIContext().getPromptAction().showToast({
            message: '文件保存失败'
          });
          return false;
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '文件保存取消'
        });
        return false;
      }
    } catch (error) {
      Logger.error(TAG, `createEncryptedFileAndWrite failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({
        message: '文件保存失败'
      });
      return false;
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 创建密钥文件并写入密钥内容
   */
  async createKeyFileAndWrite(): Promise<boolean> {
    this.isProcessing = true;
    try {
      let context = this.getUIContext().getHostContext() as common.UIAbilityContext;
      let documentPicker = new picker.DocumentViewPicker(context);

      let documentSaveOptions = new picker.DocumentSaveOptions();
      documentSaveOptions.newFileNames = ['aes_key.txt'];

      let saveUris = await documentPicker.save(documentSaveOptions);
      const windowClass = await window.getLastWindow(context);

      if (saveUris && saveUris.length > 0) {
        this.createKeyUri = saveUris[0];

        // 使用TextFileManager写入密钥
        const writeResult = await TextFileManager.writeTextFile(this.createKeyUri, this.keyString);
        if (writeResult.success) {
          windowClass.getUIContext().getPromptAction().showToast({
            message: $r('app.string.gen_key_success')
          });
          return true;
        } else {
          windowClass.getUIContext().getPromptAction().showToast({
            message: $r('app.string.gen_key_fail')
          });
          return false;
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '密钥保存取消'
        });
        return false;
      }
    } catch (error) {
      Logger.error(TAG, `createKeyFileAndWrite failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({
        message: $r('app.string.gen_key_fail')
      });
      return false;
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 从文件URI中提取文件名
   */
  private getFileNameFromUri(uri: string): string {
    if (!uri) return '未知文件';
    try {
      const segments = uri.split('/');
      let name = segments[segments.length - 1] || '未知文件';
      // 有些平台会对文件名进行 URL 编码，尝试解码
      try {
        name = decodeURIComponent(name);
      } catch (e) {
        try { name = decodeURI(name); } catch (_) { /* ignore */ }
      }
      // 如果包含 file:// 前缀，去掉路径部分
      if (name.startsWith('file://')) {
        const parts = name.split('/');
        name = parts[parts.length - 1] || name;
      }
      return name;
    } catch (e) {
      return uri;
    }
  }

  // 手动哈希计算功能已移除；加密/解密时自动计算并验证哈希

  /**
   * 加密功能主函数 - 增强版本，支持元数据头
   */
  async encryptFunc() {
    this.isProcessing = true;
    const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);

    if (this.textFileUri === '' || this.keyString === '') {
      windowClass.getUIContext().getPromptAction().showToast({
        message: $r('app.string.null_message')
      });
      this.isProcessing = false;
      return;
    }

    try {
      // 读取源文件数据
      const readResult = await TextFileManager.readTextFile(this.textFileUri);
      if (!readResult.success) {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '文件读取失败'
        });
        this.isProcessing = false;
        return;
      }
      const fileData: ArrayBuffer = readResult.data;

      // 自动计算原始文件哈希（SHA256）并在元数据中保存
      let fileHashBuf: ArrayBuffer = new ArrayBuffer(0);
      try {
        const hashRes = await this.CryptoOperation.calculateFileHash(fileData, 'SHA256');
        if (hashRes.success && hashRes.data) {
          fileHashBuf = hashRes.data as ArrayBuffer;
        }
      } catch (e) {
        Logger.warn(TAG, `计算文件哈希失败: ${e.message}`);
      }

      // 使用二进制加密接口
      const binaryResult = await this.CryptoOperation.aesConvertAndEncryptBinary(
        this.keyString, fileData
      );

      const encryptResult: EncryptOperationResult = {
        success: binaryResult.success,
        data: binaryResult.data,
        tag: binaryResult.tag,
        error: binaryResult.error
      } as EncryptOperationResult;

      // 2. 检查加密结果
      if (encryptResult.success && encryptResult.data) {
        // 3. 为加密后的数据创建元数据头
        const metadata = MetadataHelper.createDefaultMetadata(
          this.textFileName,
          'AES-GCM',
          fileData.byteLength,
          encryptResult.tag,
          fileHashBuf
        );

        // 确保有正确的原始扩展名
        if (this.originalExtension) {
          metadata.originalExtension = this.originalExtension;
        }

        // 4. 创建带元数据头的最终加密数据
        const finalEncryptedData = MetadataHelper.createDataWithMetadata(encryptResult.data, metadata);

        // 5. 显示加密信息
        this.cipherText = `加密成功！\n原始数据大小: ${metadata.fileSize} 字节\n加密后大小: ${finalEncryptedData.byteLength} 字节`;

        // 6. 保存带元数据头的加密文件
        const saveSuccess = await this.createEncryptedFileAndWrite(finalEncryptedData, this.originalExtension);

        if (saveSuccess) {
          windowClass.getUIContext().getPromptAction().showToast({
            message: $r('app.string.encrypt_success')
          });
          Logger.info(TAG, `文件加密成功，原始格式: ${this.originalExtension}, 最终文件大小: ${finalEncryptedData.byteLength} 字节`);
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({
          message: `加密失败: ${encryptResult.error || '未知错误'}`
        });
      }

    } catch (error) {
      Logger.error(TAG, `encrypt failed, ${error.code}, ${error.message}`);
      windowClass.getUIContext().getPromptAction().showToast({
        message: $r('app.string.encrypt_fail')
      });
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 生成AES密钥
   */
  async genAesKey() {
    this.isProcessing = true;
    const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);

    try {
      this.keyString = await this.CryptoOperation.generateAesKey();

      if (!this.keyString) {
        windowClass.getUIContext().getPromptAction().showToast({
          message: $r('app.string.gen_key_fail')
        });
        this.isProcessing = false;
        return;
      }

      // 保存密钥文件
      const saveSuccess = await this.createKeyFileAndWrite();
      if (saveSuccess) {
        // 显示密钥摘要（不显示完整密钥）
        const keyPreview = this.keyString.length > 16 ?
          this.keyString.substring(0, 16) + '...' : this.keyString;
        this.cipherText = `AES密钥生成成功，长度: ${this.keyString.length} 字符`;
        Logger.info(TAG, `AES密钥生成成功，长度: ${this.keyString.length} 字符`);
      }

    } catch (error) {
      Logger.error(TAG, `gen aes key failed, ${error.code}, ${error.message}`);
      windowClass.getUIContext().getPromptAction().showToast({
        message: $r('app.string.gen_key_fail')
      });
    } finally {
      this.isProcessing = false;
    }
  }
}