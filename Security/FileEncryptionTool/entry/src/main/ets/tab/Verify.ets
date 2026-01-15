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
import { MetadataHelper, MetadataParseResult } from '../util/MetadataHelper';

const TAG: string = '[Crypto_Framework]';

// 验签结果接口
interface VerifyResult {
  success: boolean;
  isValid: boolean;
  message: string;
  algorithm?: string;
  verifyTime?: string;
}

@Component
export struct Verify {
  // 状态变量
  @State isProcessing: boolean = false;
  @State verifyResult: string = '';
  @State verifyDetails: string = '';
  @State keyFileName: string = '';
  @State keyFileUri: string = '';
  @State textFileUri: string = '';
  @State textFileName: string = '';
  @State keyString: string = '';
  @State plainText: string = '';
  @State message: string = '';
  @State signFileUri: string = '';
  @State signFileName: string = '';
  @State signData: string = ''; // 签名数据（Base64或十六进制）
  @State createKeyUri: string = '';
  @State fileHash: string = ''; // 文件哈希值
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
              Text('文件验签')
                .fontSize(24)
                .fontWeight(600)
                .fontColor($r('sys.color.font_primary'))
                .lineHeight(32)
              
              Text('验证文件完整性和真实性')
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
                  // 待验签文件选择项
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

                  // RSA公钥文件选择项
                  Row() {
                    Column() {
                      Row() {
                        // 图标区域
                        Row() {
                          Text('🔓')
                            .fontSize(20)
                            .fontWeight(400)
                        }
                        .width(40)
                        .height(40)
                        .borderRadius(8)
                        .backgroundColor(0xFFE3F2FD)
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

                  // 签名文件选择项
                  Row() {
                    Column() {
                      Row() {
                        // 图标区域
                        Row() {
                          Text('✍️')
                            .fontSize(20)
                            .fontWeight(400)
                        }
                        .width(40)
                        .height(40)
                        .borderRadius(8)
                        .backgroundColor(0xFFFFF3E0)
                        .justifyContent(FlexAlign.Center)

                        // 文本区域
                        Column({ space: 4 }) {
                          Text($r('app.string.select_signature_file'))
                            .fontSize(16)
                            .fontWeight(500)
                            .fontColor($r('sys.color.font_primary'))
                            .textAlign(TextAlign.Start)
                            .lineHeight(22)

                          Text(this.signFileName === '' ? $r('app.string.please_choose') : this.signFileName)
                            .fontSize(13)
                            .fontWeight(400)
                            .fontColor(this.signFileName === '' ? $r('sys.color.font_secondary') : $r('sys.color.font_primary'))
                            .textAlign(TextAlign.Start)
                            .lineHeight(18)
                            .maxLines(1)
                            .textOverflow({ overflow: TextOverflow.Ellipsis })
                            .opacity(this.signFileName === '' ? 0.6 : 0.9)
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
                      this.selectSignFileAndRead();
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

          // 文件内容显示区域
          GridRow() {
            GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
              Column() {
                // 标题栏
                Row() {
                  Row() {
                    Text('📝')
                      .fontSize(18)
                      .margin({ right: 8 })
                    Text($r('app.string.text_context'))
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
                          Span(this.plainText || $r('app.string.please_choose'))
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

          // 验签结果显示区域
          if (this.verifyResult) {
            GridRow() {
              GridCol({ span: { xs: 12, sm: 12, md: 12, lg: 12 } }) {
                Column() {
                  // 标题栏
                  Row() {
                    Row() {
                      Text(this.verifyResult === '验签成功' ? '✅' : this.verifyResult === '验签失败' ? '❌' : '⚠️')
                        .fontSize(18)
                        .margin({ right: 8 })
                      Text('验签结果')
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
                          Text(this.verifyResult)
                            .fontSize(16)
                            .fontWeight(600)
                            .fontColor(this.verifyResult === '验签成功' ? 0xFF1E8E3E : 
                                      this.verifyResult === '验签失败' ? 0xFFF44336 : 
                                      $r('sys.color.font_primary'))
                            .textAlign(TextAlign.Start)
                            .lineHeight(24)

                          if (this.verifyDetails) {
                            Divider()
                              .color(0xF0F0F0)
                              .strokeWidth(1)
                              .margin({ top: 4, bottom: 4 })

                            Text(this.verifyDetails)
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
                      .height(120)
                      .scrollBar(BarState.Auto)
                    }
                    .width('100%')
                    .padding({ left: 20, right: 20, top: 8, bottom: 16 })
                  }
                  .width('100%')
                }
                .width('100%')
                .backgroundColor(this.verifyResult === '验签成功' ? 0xFFE8F5E9 : 
                                this.verifyResult === '验签失败' ? 0xFFFFEBEE : 
                                0xFFFFFF)
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
          }

          // 操作按钮区域
          Column({ space: 16 }) {
            // 验签按钮
            Button() {
              Row() {
                if (this.isProcessing) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('sys.color.comp_background_list_card'))
                    .margin({ right: 8 })
                } else {
                  Text('🔍')
                    .fontSize(18)
                    .margin({ right: 8 })
                }
                Text($r('app.string.verify'))
                  .fontSize(16)
                  .fontWeight(600)
                  .lineHeight(22)
                  .fontColor($r('sys.color.comp_background_list_card'))
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
            }
            .borderRadius(24)
            .id('verifyBtn')
            .type(ButtonType.Capsule)
            .width('100%')
            .height(52)
            .backgroundColor($r('sys.color.brand'))
            .enabled(!this.isProcessing && !!this.textFileUri && !!this.keyString && !!this.signData)
            .opacity((!this.isProcessing && !!this.textFileUri && !!this.keyString && !!this.signData) ? 1.0 : 0.5)
            .shadow({
              radius: 8,
              color: 0x33000000,
              offsetX: 0,
              offsetY: 2
            })
            .onClick(() => {
              if (this.textFileUri === '' || this.keyString === '' || this.signData === '') {
                this.getUIContext().getPromptAction().showToast({
                  message: $r('app.string.null_message')
                });
              } else {
                this.verifyFunc();
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
   * 选择并读取RSA公钥文件
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
            // 需要完整公钥内容用于验证，不应截断为前 2000 字节
            this.keyString = decoder.decode(uint8);
          } catch (decodeError) {
            Logger.error(TAG, `Binary decode error: ${decodeError.code}`);
            this.keyString = "密钥内容预览不可用";
          }
          windowClass.getUIContext().getPromptAction().showToast({
            message: '公钥文件读取成功'
          });
        } else {
          windowClass.getUIContext().getPromptAction().showToast({
            message: '公钥文件读取失败'
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
   * 选择并读取待验签的文件
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
   * 选择并读取签名文件
   */
  async selectSignFileAndRead() {
    this.isProcessing = true;
    try {
      let context = this.getUIContext().getHostContext() as common.UIAbilityContext;
      let documentPicker = new picker.DocumentViewPicker(context);
      let documentSelectOptions = new picker.DocumentSelectOptions();

      let uris = await documentPicker.select(documentSelectOptions);
      const windowClass = await window.getLastWindow(context);

      if (uris && uris.length > 0) {
        this.signFileUri = uris[0];
        this.signFileName = this.getFileNameFromUri(this.signFileUri);

        // 读取签名文件
        const readResult = await TextFileManager.readTextFile(this.signFileUri);
        if (readResult.success) {
          const fileData = readResult.data as ArrayBuffer;

          // 尝试解析元数据头
          const parseResult = MetadataHelper.parseDataWithMetadata(fileData);

          if (parseResult.success && parseResult.data) {
            // 提取纯签名数据
            const signatureArray = new Uint8Array(parseResult.data);
            this.signData = this.uint8ArrayToBase64(signatureArray);

            // 显示元数据信息
            if (parseResult.metadata) {
              this.verifyDetails = `检测到签名元数据:\n` +
                `原始格式: ${parseResult.metadata.originalExtension}\n` +
                `签名算法: ${parseResult.metadata.algorithm || '未知'}\n` +
                `签名时间: ${new Date(parseResult.metadata.timestamp).toLocaleString()}`;
            }
          } else {
            // 如果没有元数据头，假设是纯签名数据
            const signatureArray = new Uint8Array(fileData);
            this.signData = this.uint8ArrayToBase64(signatureArray);
          }

          windowClass.getUIContext().getPromptAction().showToast({
            message: '签名文件读取成功'
          });
        } else {
          windowClass.getUIContext().getPromptAction().showToast({
            message: '签名文件读取失败'
          });
        }
      } else {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '未选择文件'
        });
      }
    } catch (error) {
      Logger.error(TAG, `selectSignFileAndRead failed, ${error.code}, ${error.message}`);
      const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);
      windowClass.getUIContext().getPromptAction().showToast({
        message: '选择文件失败，请重试'
      });
    } finally {
      this.isProcessing = false;
    }
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

      // 手动哈希计算已移除；签名/验签流程会自动处理哈希
    } finally {
      this.isProcessing = false;
    }
  }

  /**
   * 验签功能主函数
   */
  async verifyFunc() {
    this.isProcessing = true;
    const windowClass = await window.getLastWindow(this.getUIContext().getHostContext() as common.UIAbilityContext);

    // 重置验签结果
    this.verifyResult = '';
    this.verifyDetails = '';

    if (this.textFileUri === '' || this.keyString === '' || this.signData === '') {
      windowClass.getUIContext().getPromptAction().showToast({
        message: $r('app.string.null_message')
      });
      this.isProcessing = false;
      return;
    }

    try {
      // 读取原始文件数据
      const readResult = await TextFileManager.readTextFile(this.textFileUri);
      if (!readResult.success) {
        windowClass.getUIContext().getPromptAction().showToast({
          message: '文件读取失败'
        });
        this.isProcessing = false;
        return;
      }

      let fileData: ArrayBuffer | string = readResult.data;
      if (typeof fileData !== 'string') {
        fileData = fileData as ArrayBuffer;
      }

      // 准备签名数据（可能是Base64或十六进制字符串）
      let signatureData: ArrayBuffer;
      if (this.isBase64(this.signData)) {
        // Base64编码的签名数据
        signatureData = this.base64ToUint8Array(this.signData).buffer;
      } else if (this.isHexString(this.signData)) {
        // 十六进制编码的签名数据
        signatureData = this.hexStringToUint8Array(this.signData).buffer;
      } else {
        // 假设是原始字符串数据
        const encoder = new util.TextEncoder();
        signatureData = encoder.encode(this.signData).buffer;
      }

      let verifyResult: boolean = false;

      // 根据数据类型选择验签方法 [1,4](@ref)
      if (typeof fileData === 'string') {
        // 字符串数据使用字符串验签方法
        verifyResult = !!(await this.CryptoOperation.rsaConvertAndVerify(
          this.keyString, fileData, this.signData
        ));
      } else {
        // 二进制数据使用二进制验签方法 [6](@ref)
        const binaryVerifyResult = await this.CryptoOperation.rsaConvertAndVerifyBinary(
          this.keyString, fileData as ArrayBuffer, signatureData
        );
        verifyResult = binaryVerifyResult.valid === true;
      }

      // 显示验签结果
      const verifyTime = new Date().toLocaleString();
      if (verifyResult) {
        this.verifyResult = '验签成功';
        this.verifyDetails = `文件完整性验证通过\n验签时间: ${verifyTime}\n文件来源可信`;

        windowClass.getUIContext().getPromptAction().showToast({
          message: $r('app.string.verify_success')
        });

        Logger.info(TAG, `文件验签成功，验签时间: ${verifyTime}`);
      } else {
        this.verifyResult = '验签失败';
        this.verifyDetails = `文件可能已被篡改或签名无效\n验签时间: ${verifyTime}\n建议重新获取原始文件`;

        windowClass.getUIContext().getPromptAction().showToast({
          message: $r('app.string.verify_fail')
        });

        Logger.warn(TAG, `文件验签失败，文件可能已被篡改`);
      }

    } catch (error) {
      Logger.error(TAG, `verify failed, ${error.code}, ${error.message}`);
      this.verifyResult = '验签过程出错';
      this.verifyDetails = `验签过程中发生错误: ${error.message}\n请检查文件格式和密钥是否正确`;

      windowClass.getUIContext().getPromptAction().showToast({
        message: '验签过程发生错误'
      });
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
   * 检查字符串是否为Base64编码
   */
  private isBase64(str: string): boolean {
    try {
      const base64Helper = new util.Base64Helper();
      // 尝试解码，如果成功则认为是有效的Base64
      base64Helper.decodeSync(str);
      return true;
    } catch (err) {
      return false;
    }
  }

  /**
   * 检查字符串是否为十六进制编码
   */
  // 预编译正则表达式提升性能
  private readonly HEX_REGEX: RegExp = new RegExp("^[0-9A-Fa-f]+$");

  private isHexString(str: string): boolean {
    return str.length > 0 && this.HEX_REGEX.test(str);
  }

  /**
   * Base64字符串转换为Uint8Array
   */
  private base64ToUint8Array(base64: string): Uint8Array {
    try {
      const base64Helper = new util.Base64Helper();
      return base64Helper.decodeSync(base64);
    } catch (error) {
      Logger.error(TAG, `Base64解码失败: ${error.message}`);
      return new Uint8Array(0);
    }
  }

  /**
   * 十六进制字符串转换为Uint8Array
   */
  private hexStringToUint8Array(hexString: string): Uint8Array {
    try {
      const bytes = new Uint8Array(hexString.length / 2);
      for (let i = 0; i < hexString.length; i += 2) {
        bytes[i / 2] = parseInt(hexString.substr(i, 2), 16);
      }
      return bytes;
    } catch (error) {
      Logger.error(TAG, `十六进制解码失败: ${error.message}`);
      return new Uint8Array(0);
    }
  }

  /**
   * Uint8Array转换为Base64字符串
   */
  private uint8ArrayToBase64(uint8Array: Uint8Array): string {
    try {
      const base64Helper = new util.Base64Helper();
      return base64Helper.encodeToStringSync(uint8Array);
    } catch (error) {
      Logger.error(TAG, `Base64编码失败: ${error.message}`);
      return '';
    }
  }
}