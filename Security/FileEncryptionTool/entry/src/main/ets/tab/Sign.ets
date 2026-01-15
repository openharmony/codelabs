/*
 * Copyright (c) 2026 Beijing Institute of Technology
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

// 定义元数据接口
interface FileMetadata {
  originalExtension: string;
  timestamp: string;
  version: string;
  fileSize?: number;
  signatureAlgorithm?: string;
}

@Component
export struct Sign {
  // 状态变量
  @State fileHash: string = '';
  @State originalExtension: string = '';
  @State isProcessing: boolean = false;
  @State signResult: string = '';
  @State signatureHex: string = ''; // 签名十六进制显示

  @State keyFileName: string = '';
  @State keyFileUri: string = '';
  @State textFileUri: string = '';
  @State textFileName: string = '';
  @State keyString: string = '';
  @State cipherText: string = '';
  @State plainText: string = '';
  @State message: string = '';
  @State signFileUri: string = '';
  @State createKeyUri: string = '';
  @State fileSize: number = 0; // 文件大小

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
              Text('文件签名')
                .fontSize(24)
                .fontWeight(600)
                .fontColor($r('sys.color.font_primary'))
                .lineHeight(32)
              
              Text('安全可靠的数字签名工具')
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
                  // 待签名文件选择项
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

                  // RSA密钥文件选择项
                  Row() {
                    Column() {
                      Row() {
                        // 图标区域
                        Row() {
                          Text('🔐')
                            .fontSize(20)
                            .fontWeight(400)
                        }
                        .width(40)
                        .height(40)
                        .borderRadius(8)
                        .backgroundColor(0xFFE8F5E9)
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
                      this.selectRsaKeyFileAndRead();
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

          // 签名结果显示区域
          GridRow() {
            GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
              Column() {
                // 标题栏
                Row() {
                  Row() {
                    Text('✍️')
                      .fontSize(18)
                      .margin({ right: 8 })
                    Text($r('app.string.signature_result'))
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
                        Text(this.signResult || '暂无签名结果')
                          .fontSize(15)
                          .fontWeight(400)
                          .fontColor($r('sys.color.font_primary'))
                          .textAlign(TextAlign.Start)
                          .lineHeight(22)

                        if (this.signatureHex) {
                          Divider()
                            .color(0xF0F0F0)
                            .strokeWidth(1)
                            .margin({ top: 4, bottom: 4 })

                          Text('签名值: ' + this.signatureHex.substring(0, 64) + '...')
                            .fontSize(13)
                            .fontWeight(400)
                            .fontColor($r('sys.color.font_secondary'))
                            .textAlign(TextAlign.Start)
                            .lineHeight(20)
                            .opacity(0.85)
                        }
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
                  // 文件大小显示
                  Row() {
                    Row() {
                      Text('📏')
                        .fontSize(16)
                        .margin({ right: 12 })
                      Column({ space: 2 }) {
                        Text('文件大小')
                          .fontSize(13)
                          .fontWeight(400)
                          .fontColor($r('sys.color.font_secondary'))
                          .opacity(0.7)
                        Text(this.fileSize ? this.formatFileSize(this.fileSize) : '未知')
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
                  .padding({ left: 20, right: 20 })

                  // 分隔线
                  Divider()
                    .color(0xF5F5F5)
                    .strokeWidth(1)
                    .margin({ left: 20, right: 20 })

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
            // 生成RSA密钥按钮
            Button() {
              Row() {
                Text('🔐')
                  .fontSize(18)
                  .margin({ right: 8 })
                Text($r('app.string.generate_rsa_key_randomly'))
                  .fontSize(16)
                  .fontWeight(600)
                  .lineHeight(22)
                  .fontColor($r('sys.color.comp_background_list_card'))
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
            }
            .id('encryptRsaGenKey')
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
              this.genRsaKey();
            })

            // 签名按钮
            Button() {
              Row() {
                if (this.isProcessing) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('sys.color.comp_background_list_card'))
                    .margin({ right: 8 })
                } else {
                  Text('✍️')
                    .fontSize(18)
                    .margin({ right: 8 })
                }
                Text($r('app.string.sign'))
                  .fontSize(16)
                  .fontWeight(600)
                  .lineHeight(22)
                  .fontColor($r('sys.color.comp_background_list_card'))
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
            }
            .borderRadius(24)
            .id('signBtn')
            .type(ButtonType.Capsule)
            .width('100%')
            .height(52)
            .backgroundColor($r('sys.color.brand'))
            .enabled(!this.isProcessing && !!this.textFileUri && !!this.keyString)
            .opacity((!this.isProcessing && !!this.textFileUri && !!this.keyString) ? 1.0 : 0.5)
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
                this.signFunc();
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
   * 选择并读取RSA密钥文件
   */
  async selectRsaKeyFileAndRead() {
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
            // 需要完整密钥内容用于解析与签名，不应仅使用前 1000 字节
            this.keyString = decoder.decode(uint8);
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
      Logger.error(TAG, `selectRsaKeyFileAndRead failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({
        message: '选择文件失败，请重试'
      });
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 选择并读取待签名的文件
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
            this.fileSize = (readResult.data as ArrayBuffer).byteLength;
          } catch (decodeError) {
            Logger.error(TAG, `Binary decode error: ${decodeError.code}`);
            this.plainText = "二进制内容预览不可用";
            this.fileSize = (readResult.data as ArrayBuffer).byteLength;
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
   * 创建带元数据头的签名数据
   */
  private createSignatureWithMetadata(signatureData: ArrayBuffer, originalExtension: string): ArrayBuffer {
    // 创建元数据
    const metadata = MetadataHelper.createDefaultMetadata(
      this.textFileName,
      'RSA-PKCS1-SHA256',
      this.fileSize
    );

    // 确保有正确的扩展名
    if (originalExtension) {
      metadata.originalExtension = originalExtension;
    }

    // 使用 MetadataHelper 创建带元数据头的数据
    return MetadataHelper.createDataWithMetadata(signatureData, metadata);
  }

  /**
   * 创建签名文件并写入内容（支持元数据头）
   */
  async createSignedFileAndWrite(signatureData: ArrayBuffer, originalExtension?: string): Promise<boolean> {
    this.isProcessing = true;
    try {
      let context = this.getUIContext().getHostContext() as common.UIAbilityContext;
      let documentPicker = new picker.DocumentViewPicker(context);

      let documentSaveOptions = new picker.DocumentSaveOptions();

      // 智能生成文件名
      let newFileName: string;
      if (originalExtension) {
        const baseName = MetadataHelper.getFileBaseName(this.textFileName);
        newFileName = `${baseName}.${originalExtension}.signature`;
      } else {
        newFileName = this.textFileName + '.signature';
      }

      documentSaveOptions.newFileNames = [newFileName];

      let saveUris = await documentPicker.save(documentSaveOptions);
      const windowClass = await window.getLastWindow(context);

      if (saveUris && saveUris.length > 0) {
        this.signFileUri = saveUris[0];

        // 创建带元数据头的签名数据
        const signatureWithMetadata = this.createSignatureWithMetadata(signatureData, originalExtension || this.originalExtension);

        // 使用TextFileManager写入数据
        const writeResult = await TextFileManager.writeTextFile(this.signFileUri, signatureWithMetadata);
        if (writeResult.success) {
          windowClass.getUIContext().getPromptAction().showToast({
            message: `签名文件保存成功，格式: ${originalExtension || '未知'}`
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
      Logger.error(TAG, `createSignedFileAndWrite failed, ${error.code}, ${error.message}`);
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
      documentSaveOptions.newFileNames = ['rsa_key_pair.json'];

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
      try {
        name = decodeURIComponent(name);
      } catch (e) {
        try { name = decodeURI(name); } catch (_) { }
      }
      if (name.startsWith('file://')) {
        const parts = name.split('/');
        name = parts[parts.length - 1] || name;
      }
      return name;
    } catch (e) {
      return uri;
    }
  }

  /**
   * 格式化文件大小显示
   */
  private formatFileSize(bytes: number): string {
    if (bytes === 0) return '0 B';
    const k = 1024;
    const sizes = ['B', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
  }

  /**
   * 计算文件哈希值
   */
  async calculateFileHash() {
    this.isProcessing = true;
    try {
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);

      // 读取文件数据
      const readResult = await TextFileManager.readTextFile(this.textFileUri);
      if (!readResult.success) {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '文件读取失败'
        });
        return;
      }

      const fileData = readResult.data as ArrayBuffer;

      // 手动哈希计算已移除；加解密/签名流程会自动处理哈希
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 签名功能主函数 - 增强版本，支持元数据头
   */
  async signFunc() {
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
      // 读取文件数据
      const readResult = await TextFileManager.readTextFile(this.textFileUri);
      if (!readResult.success) {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '文件读取失败'
        });
        this.isProcessing = false;
        return;
      }

      const fileDataBuf = readResult.data as ArrayBuffer;
      let signResult: ArrayBuffer | null = null;

      // 使用二进制签名方法
      const binarySignResult = await this.CryptoOperation.rsaConvertAndSignBinary(this.keyString, fileDataBuf);
      if (binarySignResult.success && binarySignResult.data) {
        signResult = binarySignResult.data;
      }

      if (signResult) {
        // 生成签名十六进制显示
        const signatureArray = new Uint8Array(signResult);
        this.signatureHex = Array.from(signatureArray).map(b => b.toString(16).padStart(2, '0')).join('');

        // 显示签名信息
        this.signResult = `签名成功！\n签名算法: RSA-PKCS1-SHA256\n签名时间: ${new Date().toLocaleString()}\n文件大小: ${this.formatFileSize(this.fileSize)}\n签名长度: ${signResult.byteLength} 字节`;

        // 保存签名文件
        const saveSuccess = await this.createSignedFileAndWrite(signResult, this.originalExtension);
        if (saveSuccess) {
          windowClass.getUIContext().getPromptAction().showToast({
            message: $r('app.string.sign_success')
          });
          Logger.info(TAG, `文件签名成功，原始格式: ${this.originalExtension}, 签名大小: ${signResult.byteLength} 字节`);
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({
          message: $r('app.string.sign_fail')
        });
      }

    } catch (error) {
      Logger.error(TAG, `sign failed, ${error.code}, ${error.message}`);
      windowClass.getUIContext().getPromptAction().showToast({
        message: $r('app.string.sign_fail')
      });
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 生成RSA密钥
   */
  async genRsaKey() {
    this.isProcessing = true;
    const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);

    // 显示生成密钥的提示
    windowClass.getUIContext().getPromptAction().showToast({
      message: $r('app.string.slow_rsa_key_gen')
    });

    try {
      this.keyString = await this.CryptoOperation.generateRsaKey();

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
        // 显示密钥摘要
        const keyPreview = this.keyString.length > 50 ?
          this.keyString.substring(0, 50) + '...' : this.keyString;
        this.signResult = `RSA密钥生成成功，长度: ${this.keyString.length} 字符`;
        Logger.info(TAG, `RSA密钥生成成功，长度: ${this.keyString.length} 字符`);
      }

    } catch (error) {
      Logger.error(TAG, `gen rsa key failed, ${error.code}, ${error.message}`);
      windowClass.getUIContext().getPromptAction().showToast({
        message: $r('app.string.gen_key_fail')
      });
    } finally {
      this.isProcessing = false;
    }
  }
}