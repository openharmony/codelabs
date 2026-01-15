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

import cryptoFramework from '@ohos.security.cryptoFramework';
import promptAction from '@ohos.promptAction';
import Logger from '../util/Logger';
import picker from '@ohos.file.picker';
import { CryptoOperation } from '../cryptoframework/CryptoOperation';
import TextFileManager from '../textfilemanager/TextFileManager';
import common from '@ohos.app.ability.common';
import window from '@ohos.window';
import { util } from '@kit.ArkTS';
import { MetadataHelper, MetadataParseResult } from '../util/MetadataHelper';

const TAG: string = '[Crypto_Framework]';

interface BinaryDecryptResult {
  success: boolean;
  data?: ArrayBuffer;
  error?: string;
}

@Component
export struct Decrypt {
  // 状态变量
  @State fileHash: string = '';
  @State originalExtension: string = '';
  @State isProcessing: boolean = false;
  @State decryptionStatus: string = '';
  @State decryptionDetails: string = '';

  @State keyFileName: string = '';
  @State keyFileUri: string = '';
  @State textFileUri: string = '';
  @State textFileName: string = '';
  @State keyString: string = '';
  @State cipherText: string = '';
  @State plainText: string = '';
  @State message: string = '';
  @State decryptedFileUri: string = '';
  @State createKeyUri: string = '';
  @State encryptedFileUri: string = '';
  @State metadataInfo: string = '';
  @State fileSize: number = 0;

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
              Text('文件解密')
                .fontSize(24)
                .fontWeight(600)
                .fontColor($r('sys.color.font_primary'))
                .lineHeight(32)
              
              Text('安全可靠的加密文件解密工具')
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
                  // 加密文件选择项
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
                      this.selectEncryptedFileAndRead();
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

          // 加密文件信息显示区域
          GridRow() {
            GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
              Column() {
                // 标题栏
                Row() {
                  Row() {
                    Text('📊')
                      .fontSize(18)
                      .margin({ right: 8 })
                    Text('加密文件信息')
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
                        Text(this.cipherText ? this.cipherText : '暂无加密文件信息')
                          .fontSize(15)
                          .fontWeight(400)
                          .fontColor($r('sys.color.font_primary'))
                          .textAlign(TextAlign.Start)
                          .lineHeight(22)

                        if (this.metadataInfo) {
                          Divider()
                            .color(0xF0F0F0)
                            .strokeWidth(1)
                            .margin({ top: 4, bottom: 4 })

                          Text(this.metadataInfo)
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

          // 解密结果显示区域
          GridRow() {
            GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
              Column() {
                // 标题栏
                Row() {
                  Row() {
                    Text('✅')
                      .fontSize(18)
                      .margin({ right: 8 })
                    Text('解密结果')
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
                        Text(this.decryptionStatus || '等待解密操作')
                          .fontSize(15)
                          .fontWeight(500)
                          .fontColor(this.decryptionStatus.includes('成功') ? 0xFF4CAF50 : 
                                    this.decryptionStatus.includes('失败') ? 0xFFF44336 : 
                                    $r('sys.color.font_primary'))
                          .textAlign(TextAlign.Start)
                          .lineHeight(22)

                        if (this.decryptionDetails) {
                          Divider()
                            .color(0xF0F0F0)
                            .strokeWidth(1)
                            .margin({ top: 4, bottom: 4 })

                          Text(this.decryptionDetails)
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

          // 文件详细信息显示区域
          GridRow() {
            GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
              Column() {
                // 标题栏
                Row() {
                  Row() {
                    Text('ℹ️')
                      .fontSize(18)
                      .margin({ right: 8 })
                    Text('文件详细信息')
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
            // 检测元数据按钮
            Button() {
              Row() {
                Text('🔍')
                  .fontSize(18)
                  .margin({ right: 8 })
                Text('检测元数据')
                  .fontSize(16)
                  .fontWeight(600)
                  .lineHeight(22)
                  .fontColor($r('sys.color.comp_background_list_card'))
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
            }
            .borderRadius(24)
            .type(ButtonType.Capsule)
            .width('100%')
            .height(52)
            .backgroundColor($r('sys.color.brand'))
            .enabled(!this.isProcessing && !!this.textFileUri)
            .opacity((!this.isProcessing && !!this.textFileUri) ? 1.0 : 0.5)
            .shadow({
              radius: 8,
              color: 0x33000000,
              offsetX: 0,
              offsetY: 2
            })
            .onClick(() => {
              this.detectMetadataHeader();
            })

            // 解密按钮
            Button() {
              Row() {
                if (this.isProcessing) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('sys.color.comp_background_list_card'))
                    .margin({ right: 8 })
                } else {
                  Text('🔓')
                    .fontSize(18)
                    .margin({ right: 8 })
                }
                Text($r('app.string.decrypt'))
                  .fontSize(16)
                  .fontWeight(600)
                  .lineHeight(22)
                  .fontColor($r('sys.color.comp_background_list_card'))
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
            }
            .borderRadius(24)
            .id('decryptionBtn')
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
                this.decryptWithMetadata();
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
   * 选择并读取AES密钥文件
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

        const readResult = await TextFileManager.readTextFile(this.keyFileUri);
        if (readResult.success) {
          try {
            const decoder = new util.TextDecoder('utf-8');
            const uint8 = new Uint8Array(readResult.data as ArrayBuffer);
            const dataSlice = uint8.slice(0, 2000);
            this.keyString = decoder.decode(dataSlice);
          } catch (decodeError) {
            Logger.error(TAG, `Binary decode error: ${decodeError.code}`);
            this.keyString = "密钥内容预览不可用";
          }
          windowClass.getUIContext().getPromptAction().showToast({ message: '密钥文件读取成功' });
        } else {
          windowClass.getUIContext().getPromptAction().showToast({ message: '密钥文件读取失败' });
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({ message: '未选择文件' });
      }
    } catch (error) {
      Logger.error(TAG, `selectAesKeyFileAndRead failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({ message: '选择文件失败，请重试' });
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 选择并读取加密文件（增强版本，支持元数据检测）
   */
  async selectEncryptedFileAndRead() {
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

        const readResult = await TextFileManager.readTextFile(this.textFileUri);
        if (readResult.success) {
          const data = readResult.data as ArrayBuffer;
          this.cipherText = `加密数据大小: ${data.byteLength} 字节（二进制）`;
          this.fileSize = data.byteLength;

          // 自动检测元数据头
          await this.detectMetadataHeader();
          windowClass.getUIContext().getPromptAction().showToast({ message: '加密文件读取成功' });
        } else {
          windowClass.getUIContext().getPromptAction().showToast({ message: '文件读取失败' });
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({ message: '未选择文件' });
      }
    } catch (error) {
      Logger.error(TAG, `selectEncryptedFileAndRead failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({ message: '选择文件失败，请重试' });
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 检测元数据头
   */
  async detectMetadataHeader() {
    this.isProcessing = true;
    try {
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);

      if (!this.textFileUri) {
        windowClass.getUIContext().getPromptAction().showToast({ message: '请先选择加密文件' });
        return;
      }

      const separationResult = await this.separateMetadataAndCipherData(this.textFileUri);
      if (separationResult.success && separationResult.metadata) {
        const metadata = separationResult.metadata;
        this.metadataInfo = `检测到元数据头:\n` +
          `原始格式: ${metadata.originalExtension}\n` +
          `文件大小: ${this.formatFileSize(metadata.fileSize || 0)}\n` +
          `加密时间: ${new Date(metadata.timestamp).toLocaleString()}\n` +
          `版本: v${metadata.version}`;

        this.originalExtension = metadata.originalExtension;

        windowClass.getUIContext().getPromptAction().showToast({
          message: `检测到元数据头，原始格式: ${metadata.originalExtension}`
        });

        Logger.info(TAG, `元数据头检测成功: ${JSON.stringify(metadata)}`);
      } else {
        this.metadataInfo = '未检测到有效的元数据头，可能为旧版本加密文件';
        windowClass.getUIContext().getPromptAction().showToast({
          message: '未检测到元数据头'
        });
      }
    } catch (error) {
      Logger.error(TAG, `detectMetadataHeader failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({
        message: '元数据检测失败'
      });
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 分离元数据头和加密数据
   */
  private async separateMetadataAndCipherData(fileUri: string): Promise<MetadataParseResult> {
    try {
      // 读取文件数据
      const readResult = await TextFileManager.readTextFile(fileUri);
      if (!readResult.success || !readResult.data) {
        return { success: false, message: '文件读取失败' };
      }

      const fileData = readResult.data as ArrayBuffer;

      return MetadataHelper.parseDataWithMetadata(fileData);

    } catch (error) {
      Logger.error(TAG, `separateMetadataAndCipherData failed: ${error.message}`);
      return {
        success: false,
        message: `元数据分离失败: ${error.message}`
      };
    }
  }

  /**
   * 创建解密文件并写入内容（支持元数据恢复）
   */
  async createDecryptedFileAndWrite(decryptedData: ArrayBuffer | string, originalExtension?: string): Promise<boolean> {
    this.isProcessing = true;
    try {
      let context = this.getUIContext().getHostContext() as common.UIAbilityContext;
      let documentPicker = new picker.DocumentViewPicker(context);

      let documentSaveOptions = new picker.DocumentSaveOptions();

      // 使用元数据中的原始扩展名或默认扩展名
      const finalExtension = originalExtension || 'decrypted';
      const baseName = MetadataHelper.getFileBaseName(this.textFileName);
      const newFileName = `${baseName}_decrypted.${finalExtension}`;

      documentSaveOptions.newFileNames = [newFileName];

      let saveUris = await documentPicker.save(documentSaveOptions);
      const windowClass = await window.getLastWindow(context);

      if (saveUris && saveUris.length > 0) {
        this.decryptedFileUri = saveUris[0];

        const writeResult = await TextFileManager.writeTextFile(this.decryptedFileUri, decryptedData);
        if (writeResult.success) {
          const message = originalExtension ?
            `解密文件保存成功，已恢复为原始格式: ${originalExtension}` :
            '解密文件保存成功';

          windowClass.getUIContext().getPromptAction().showToast({ message });
          return true;
        } else {
          windowClass.getUIContext().getPromptAction().showToast({ message: '文件保存失败' });
          return false;
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({ message: '文件保存取消' });
        return false;
      }
    } catch (error) {
      Logger.error(TAG, `createDecryptedFileAndWrite failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({ message: '文件保存失败' });
      return false;
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 增强的解密功能 - 支持元数据头解析
   */
  async decryptWithMetadata() {
    this.isProcessing = true;
    const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);

    // 重置解密结果
    this.decryptionStatus = '';
    this.decryptionDetails = '';
    this.plainText = '';

    if (this.textFileUri === '' || this.keyString === '') {
      windowClass.getUIContext().getPromptAction().showToast({ message: $r('app.string.null_message') });
      this.isProcessing = false;
      return;
    }

    try {
      // 分离元数据头和加密数据
      const separationResult = await this.separateMetadataAndCipherData(this.textFileUri);

      if (!separationResult.data) {
        windowClass.getUIContext().getPromptAction().showToast({ message: '加密数据提取失败' });
        this.isProcessing = false;
        return;
      }

      let decryptionResult: string | BinaryDecryptResult | null = null;
      let originalExtension: string | undefined;

      if (separationResult.success && separationResult.metadata) {
        // 有元数据头的情况
        originalExtension = separationResult.metadata.originalExtension;
        this.originalExtension = originalExtension;

        Logger.info(TAG, `使用元数据头信息解密，原始格式: ${originalExtension}`);

        // 检查是否有 Tag
        if (separationResult.tag && separationResult.tag.byteLength > 0) {
          Logger.info(TAG, `使用元数据中的 Tag 进行解密，Tag大小: ${separationResult.tag.byteLength} 字节`);

          // 使用从元数据中提取的 Tag
          decryptionResult = await this.CryptoOperation.aesConvertAndDecryptBinary(
            this.keyString, separationResult.data, separationResult.tag
          );
        } else {
          Logger.warn(TAG, '元数据中没有 Tag，使用空 Tag 解密（兼容旧版本）');

          // 没有 Tag 的情况（兼容旧版本）
          const emptyTag = new ArrayBuffer(0);
          decryptionResult = await this.CryptoOperation.aesConvertAndDecryptBinary(
            this.keyString, separationResult.data, emptyTag
          );
        }
      } else {
        // 无元数据头的情况，尝试传统解密
        Logger.info(TAG, '未检测到元数据头，使用传统解密方法');

        const readResult = await TextFileManager.readTextFile(this.textFileUri);
        if (!readResult.success) {
          windowClass.getUIContext().getPromptAction().showToast({ message: '文件读取失败' });
          this.isProcessing = false;
          return;
        }

        // 二进制数据 - 尝试使用空 Tag
        const emptyTag = new ArrayBuffer(0);
        decryptionResult = await this.CryptoOperation.aesConvertAndDecryptBinary(
          this.keyString, readResult.data as ArrayBuffer, emptyTag
        );
      }

      // 处理解密结果
      const decryptTime = new Date().toLocaleString();

      // 检查解密结果是否成功
      let isDecryptionSuccessful = false;
      let decryptedData: ArrayBuffer | string | undefined;
      let dataSize: number = 0;

      if (decryptionResult) {
        // 使用类型断言明确告诉 TypeScript 变量的类型
        if (typeof decryptionResult === 'string') {
          // 字符串类型的解密结果
          const stringResult = decryptionResult as string;
          isDecryptionSuccessful = true;
          decryptedData = stringResult;
          dataSize = stringResult.length;
          this.plainText = stringResult.substring(0, 1000);
        } else {
          // BinaryDecryptResult 类型的解密结果
          const binaryResult = decryptionResult as BinaryDecryptResult;
          if (binaryResult.success && binaryResult.data) {
            isDecryptionSuccessful = true;
            decryptedData = binaryResult.data;
            dataSize = binaryResult.data.byteLength;

            // 将二进制数据转换为字符串预览
            try {
              const decoder = new util.TextDecoder('utf-8');
              this.plainText = decoder.decode(new Uint8Array(decryptedData).slice(0, 1000));
            } catch (error) {
              this.plainText = '二进制内容预览不可用';
            }
          } else {
            // BinaryDecryptResult 类型的失败结果
            isDecryptionSuccessful = false;
            this.decryptionStatus = '❌ 解密失败';
            this.decryptionDetails = `解密过程出错\n解密时间: ${decryptTime}\n错误: ${binaryResult.error || '未知错误'}`;

            windowClass.getUIContext().getPromptAction().showToast({
              message: $r('app.string.decrypt_fail')
            });
          }
        }
      }

      if (isDecryptionSuccessful && decryptedData) {
        this.decryptionStatus = '✅ 解密成功';
        this.decryptionDetails = `文件解密完成\n解密时间: ${decryptTime}\n处理数据: ${dataSize} 字节`;

        // 自动验证哈希（如果元数据中包含 hash 字段）
        try {
          let decryptedBuf: ArrayBuffer;
          if (typeof decryptedData === 'string') {
            decryptedBuf = new util.TextEncoder().encode(decryptedData).buffer;
          } else {
            decryptedBuf = decryptedData as ArrayBuffer;
          }

          if (separationResult && separationResult.metadata && separationResult.metadata.hash) {
            const expectedHash = MetadataHelper.base64ToArrayBuffer(separationResult.metadata.hash!);
            const ok = await this.CryptoOperation.verifyDataIntegrity(decryptedBuf, expectedHash, 'SHA256');
            if (ok) {
              this.decryptionDetails += '\n哈希验证: ✅ 匹配';
            } else {
              this.decryptionDetails += '\n哈希验证: ⚠️ 不匹配！文件可能被篡改或损坏';
              this.decryptionStatus = '⚠️ 哈希校验失败';
            }
          }
        } catch (e) {
          Logger.warn(TAG, `哈希验证失败: ${e.message}`);
          this.decryptionDetails += '\n哈希验证: 无法执行';
        }

        // 保存解密文件
        const saveSuccess = await this.createDecryptedFileAndWrite(decryptedData, originalExtension);
        if (saveSuccess) {
          windowClass.getUIContext().getPromptAction().showToast({
            message: $r('app.string.decrypt_success')
          });

          Logger.info(TAG, `文件解密成功，原始格式: ${originalExtension || '未知'}, 解密大小: ${dataSize} 字节`);
        }
      } else if (!isDecryptionSuccessful) {
        // 解密失败的情况（decryptionResult 为 null 或其他失败情况）
        this.decryptionStatus = '❌ 解密失败';
        this.decryptionDetails = `解密过程出错\n解密时间: ${decryptTime}\n请检查密钥是否正确`;

        windowClass.getUIContext().getPromptAction().showToast({
          message: $r('app.string.decrypt_fail')
        });
      }

    } catch (error) {
      Logger.error(TAG, `decryptWithMetadata failed, ${error.code}, ${error.message}`);
      this.decryptionStatus = '⚠️ 解密过程出错';
      this.decryptionDetails = `解密过程中发生错误: ${error.message}\n请检查密钥和文件格式`;

      windowClass.getUIContext().getPromptAction().showToast({
        message: $r('app.string.decrypt_fail')
      });
    } finally {
      this.isProcessing = false;
    }
  }

  // 手动哈希计算功能已移除；加密/解密时自动计算并验证哈希

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
}