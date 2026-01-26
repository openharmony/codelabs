/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// 引入必要的模块
import { webview } from '@kit.ArkWeb';
import { BusinessError } from '@kit.BasicServicesKit';
import { preferences } from '@kit.ArkData';
import { dataManager } from '../manager/DataManager';
import { downloadManager } from '../manager/DownloadManager';
import { promptAction } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';
import { picker } from '@kit.CoreFileKit';

// 标签页信息接口
interface TabInfo {
  id: number;
  url: string;
  title: string;
  controller: webview.WebviewController;
  incognitoMode: boolean;
}

@Entry
@Component
struct Index {
  // Navigation导航栈
  pathStack: NavPathStack = new NavPathStack();
  // 初始化 WebviewController
  controller: webview.WebviewController = new webview.WebviewController();
  // 当前URL地址
  @State currentUrl: string = 'https://www.huawei.com';
  // URL输入框的值
  @State inputUrl: string = 'https://www.huawei.com';
  // 是否可以后退
  @State canGoBack: boolean = false;
  // 是否可以前进
  @State canGoForward: boolean = false;
  // 多标签页管理
  @State tabs: Array<TabInfo> = [];
  @State currentTabIndex: number = 0;
  @State showTabList: boolean = false;
  // 设置项
  @State homeUrl: string = 'https://www.huawei.com';
  @State darkMode: boolean = false;
  @State forceDarkMode: boolean = true;
  @State incognitoMode: boolean = false;
  @State javaScriptEnabled: boolean = true;
  @State imageEnabled: boolean = true;
  @State textZoom: number = 100;
  @State domStorageEnabled: boolean = true;
  @State geolocationEnabled: boolean = true;
  @State clearCacheOnExit: boolean = false;
  // 监听设置变化
  @StorageLink('darkMode') storageDarkMode: boolean = false;
  @StorageLink('forceDarkMode') storageForceDarkMode: boolean = true;
  @StorageLink('homeUrl') storageHomeUrl: string = '';
  @StorageLink('javaScriptEnabled') storageJavaScriptEnabled: boolean = true;
  @StorageLink('imageEnabled') storageImageEnabled: boolean = true;
  @StorageLink('textZoom') storageTextZoom: number = 100;
  @StorageLink('clearCache') storageClearCache: number = 0;
  @StorageLink('clearCookies') storageClearCookies: number = 0;
  // 书签状态
  @State isCurrentPageBookmarked: boolean = false;
  @StorageLink('openUrl') storageOpenUrl: string = '';
  @StorageLink('bookmarkChanged') storageBookmarkChanged: number = 0;
  // 加载进度
  @State loadingProgress: number = 0;
  @State isLoading: boolean = false;
  @State pageTitle: string = '新标签页';

  aboutToAppear() {
    // 设置全局NavPathStack供子页面使用
    AppStorage.setOrCreate('pathStack', this.pathStack);
    // 加载设置
    this.loadSettings();

    // 同步初始化DataManager（确保数据立即可用）
    const context = this.getUIContext().getHostContext() as common.UIAbilityContext;
    if (context) {
      dataManager.initSync(context);
      downloadManager.init(context);
    }

    // 初始化第一个标签页
    this.tabs.push({
      id: Date.now(),
      url: this.homeUrl,
      title: '新标签页',
      controller: this.controller,
      incognitoMode: this.incognitoMode
    });

    // Debug: 显示Web组件配置信息
    console.info('=== Web组件配置 ===');
    console.info(`隐私模式: ${this.incognitoMode}`);
    console.info(`深色模式: ${this.darkMode}`);
    console.info(`强制深色: ${this.forceDarkMode}`);
    console.info(`JavaScript: ${this.javaScriptEnabled}`);
    console.info(`主页URL: ${this.homeUrl}`);

    // 监听设置变化
    this.watchSettings();

    // 监听书签变化
    this.watchBookmarkChanges();
  }

  aboutToDisappear() {
    // 检查是否需要清除缓存
    this.checkClearCache();
  }

  build() {
    Navigation(this.pathStack) {
      // 主浏览器页面内容
      Column() {
        // 顶部导航栏
        Column() {
          // 工具栏：前进、后退、刷新按钮
          Row() {
            // 后退按钮
            Button() {
              Image($r('app.media.back'))
                .width(24)
                .height(24)
                .fillColor(this.canGoBack ? '#000000' : '#CCCCCC')
            }
            .width(50)
            .height(44)
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              try {
                if (this.currentTabIndex >= 0 && this.currentTabIndex < this.tabs.length) {
                  const currentTab = this.tabs[this.currentTabIndex];
                  if (currentTab.controller.accessBackward()) {
                    currentTab.controller.backward();
                    this.updateNavigationState();
                  }
                }
              } catch (error) {
                console.error(`后退失败: ${(error as BusinessError).code}, ${(error as BusinessError).message}`);
              }
            })

            // 前进按钮
            Button() {
              Image($r('app.media.forward'))
                .width(24)
                .height(24)
                .fillColor(this.canGoForward ? '#FFFFFF' : '#CCCCCC')
            }
            .width(50)
            .height(44)
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              try {
                if (this.currentTabIndex >= 0 && this.currentTabIndex < this.tabs.length) {
                  const currentTab = this.tabs[this.currentTabIndex];
                  if (currentTab.controller.accessForward()) {
                    currentTab.controller.forward();
                    this.updateNavigationState();
                  }
                }
              } catch (error) {
                console.error(`前进失败: ${(error as BusinessError).code}, ${(error as BusinessError).message}`);
              }
            })

            // 刷新按钮
            Button() {
              Image($r('app.media.refresh'))
                .width(24)
                .height(24)
                .fillColor(this.darkMode ? '#FFFFFF' : '#666666')
            }
            .width(50)
            .height(44)
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              try {
                if (this.currentTabIndex >= 0 && this.currentTabIndex < this.tabs.length) {
                  const currentTab = this.tabs[this.currentTabIndex];
                  currentTab.controller.refresh();
                }
              } catch (error) {
                console.error(`刷新失败: ${(error as BusinessError).code}, ${(error as BusinessError).message}`);
              }
            })

            // 占位，把按钮推到左边
            Blank()
          }
          .width('100%')
          .height(44)
          .padding({ left: 8, right: 8 })

          // URL 输入栏
          Row() {
            TextInput({ text: this.inputUrl, placeholder: '搜索或输入网址' })
              .layoutWeight(1)
              .height(40)
              .fontSize(15)
              .backgroundColor(this.darkMode ? '#3A3A3C' : '#F0F0F0')
              .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
              .placeholderColor(this.darkMode ? '#8E8E93' : '#999999')
              .borderRadius(8)
              .padding({ left: 12, right: 12 })
              .onChange((value: string) => {
                this.inputUrl = value;
              })
              .onSubmit(() => {
                this.loadUrl();
              })

            // 收藏按钮
            Button() {
              Image(this.isCurrentPageBookmarked ? $r('app.media.bookmark_filled') : $r('app.media.bookmark'))
                .width(20)
                .height(20)
                .fillColor(this.isCurrentPageBookmarked ? '#FF9500' : (this.darkMode ? '#FFFFFF' : '#666666'))
            }
            .width(40)
            .height(40)
            .backgroundColor(Color.Transparent)
            .margin({ left: 4 })
            .onClick(() => {
              this.toggleBookmark();
            })

            // 跳转按钮
            Button('跳转')
              .width(60)
              .height(40)
              .fontSize(14)
              .margin({ left: 8 })
              .backgroundColor('#007AFF')
              .borderRadius(8)
              .onClick(() => {
                this.loadUrl();
              })
          }
          .width('100%')
          .padding({ left: 12, right: 12, bottom: 8 })

          // 加载进度条
          if (this.isLoading) {
            Progress({ value: this.loadingProgress, total: 100, type: ProgressType.Linear })
              .width('100%')
              .height(3)
              .color('#007AFF')
              .backgroundColor(Color.Transparent)
              .animation({
                duration: 300,
                curve: Curve.EaseInOut
              })
          }
        }
        .width('100%')
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
        .shadow({ radius: 4, color: '#10000000', offsetY: 2 })

        // Web组件区域 - 使用Stack布局显示所有标签页
        Stack() {
          ForEach(this.tabs, (tab: TabInfo, index: number) => {
            Web({
              src: tab.url,
              controller: tab.controller,
              incognitoMode: tab.incognitoMode
            })
              .width('100%')
              .height('100%')
              .backgroundColor(this.darkMode ? '#1C1C1E' : Color.White)
              .darkMode(this.darkMode ? WebDarkMode.On : WebDarkMode.Off)
              .forceDarkAccess(this.darkMode && this.forceDarkMode)
              .javaScriptAccess(this.javaScriptEnabled)
              .domStorageAccess(this.domStorageEnabled)
              .blockNetwork(!this.imageEnabled)
              .textZoomRatio(this.textZoom)
              .geolocationAccess(this.geolocationEnabled)
              .multiWindowAccess(true)
              .allowWindowOpenMethod(true)
              .visibility(index === this.currentTabIndex ? Visibility.Visible : Visibility.Hidden)
              .onControllerAttached(() => {
                console.info(`=== Web组件已创建 (标签${index}) ===`);
                console.info(`URL: ${tab.url}`);
              })
              .onWindowNew((event) => {
                console.info('收到新窗口请求');
                const newController = new webview.WebviewController();
                this.tabs.push({
                  id: Date.now(),
                  url: 'about:blank',
                  title: '新标签页',
                  controller: newController,
                  incognitoMode: this.incognitoMode
                });
                event.handler.setWebController(newController);
                this.currentTabIndex = this.tabs.length - 1;
              })
              .onPageBegin((event) => {
                if (event && index === this.currentTabIndex) {
                  console.info(`页面开始加载: ${event.url}`);
                  this.currentUrl = event.url;
                  this.inputUrl = event.url;
                  tab.url = event.url;
                  this.isLoading = true;
                  this.loadingProgress = 0;
                }
              })
              .onPageEnd((event) => {
                if (event && index === this.currentTabIndex) {
                  console.info('=== onPageEnd 触发 ===');
                  console.info(`页面加载完成: ${event.url}`);
                  console.info(`当前标签索引: ${index}`);
                  console.info(`当前激活标签索引: ${this.currentTabIndex}`);
                  console.info(`隐私模式: ${tab.incognitoMode}`);

                  this.currentUrl = event.url;
                  this.inputUrl = event.url;
                  tab.url = event.url;
                  try {
                    const title = tab.controller.getTitle();
                    console.info(`页面标题: ${title}`);
                    if (title) {
                      tab.title = title;
                    }
                    // 更新书签状态
                    this.isCurrentPageBookmarked = dataManager.isBookmarked(event.url);

                    // 记录历史（非隐私模式）
                    if (!tab.incognitoMode) {
                      console.info('准备调用 dataManager.addHistory');
                      dataManager.addHistory(event.url, title || event.url);
                      console.info(`历史记录调用完成: ${title} - ${event.url}`);
                    } else {
                      console.info('隐私模式，跳过历史记录');
                    }
                  } catch (error) {
                    console.error('获取标题失败:', error);
                  }
                  this.updateNavigationState();
                } else {
                  if (event) {
                    console.info(`onPageEnd 触发但不是当前标签 - 标签索引: ${index}, 当前激活: ${this.currentTabIndex}`);
                  }
                }
              })
              .onProgressChange((event) => {
                if (event && index === this.currentTabIndex) {
                  this.loadingProgress = event.newProgress;
                  if (event.newProgress >= 100) {
                    this.isLoading = false;
                  }
                }
              })
              .onErrorReceive((event) => {
                if (event && index === this.currentTabIndex) {
                  this.isLoading = false;
                  console.error(`页面加载错误:`);
                  console.error(`  错误码: ${event.error.getErrorCode()}`);
                  console.error(`  错误信息: ${event.error.getErrorInfo()}`);
                  console.error(`  请求URL: ${event.request.getRequestUrl()}`);
                }
              })
              .onSslErrorEventReceive((event) => {
                // 处理SSL证书错误：忽略错误但标记为不安全连接
                if (event && index === this.currentTabIndex) {
                  console.warn(`SSL证书错误: ${event.error}`);

                  // 继续加载但标记为不安全连接
                  event.handler.handleConfirm();
                  console.info('已忽略SSL证书错误，继续加载（不安全连接）');
                  // 提示用户连接不安全
                  promptAction.openToast({
                    message: '⚠️ https失效，部分资源可能无法加载',
                    duration: 2000
                  }).catch((err: BusinessError) => {
                    console.error('显示Toast失败:', err);
                  });
                }
              })
              .onDownloadStart((event) => {
                if (event) {
                  console.info(`=== 下载请求 ===`);
                  console.info(`URL: ${event.url}`);

                  const fileName = event.contentDisposition || 'unknown_file';

                  // 调用下载管理器
                  promptAction.showDialog({
                    title: '开始下载',
                    message: `是否下载该文件？\n${event.contentDisposition || '未知文件名'}`,
                    buttons: [
                      { text: '取消', color: '#FF3B30' },
                      { text: '下载', color: '#007AFF' }
                    ]
                  }).then(async (result) => {
                    if (result.index === 1) {
                      const success = await downloadManager.startDownload(
                        event.url,
                        undefined,
                        event.mimetype,
                        event.contentLength
                      );
                      if (success) {
                        promptAction.showToast({ message: '已加入下载队列' });
                      } if (success) promptAction.showToast({ message: '开始下载' });

                    } else if (result.index === 2) {
                      // 另存为 (选择路径)
                      try {
                        const documentSaveOptions = new picker.DocumentSaveOptions();
                        documentSaveOptions.newFileNames = [fileName]; // 预置文件名

                        const context = getContext(this);
                        const documentViewPicker = new picker.DocumentViewPicker(context);

                        documentViewPicker.save(documentSaveOptions).then(async (documentSaveResult) => {
                          if (documentSaveResult && documentSaveResult.length > 0) {
                            const targetUri = documentSaveResult[0];
                            console.info(`用户选择保存路径: ${targetUri}`);

                            // 启动下载，传入 targetUri
                            const success = await downloadManager.startDownload(
                              event.url,
                              fileName,
                              event.mimetype,
                              event.contentLength,
                              targetUri
                            );

                            if (success) {
                              promptAction.showToast({ message: '开始下载并将在完成后保存' });
                            }
                          }
                        }).catch((err: BusinessError) => {
                          console.error(`Picker failed: ${err.code}, ${err.message}`);
                        });
                      } catch (err) {
                        console.error(`Picker creation failed: ${err}`);
                        promptAction.showToast({ message: '无法打开文件选择器' });
                      }
                    }
                  });
                }
              })
          }, (tab: TabInfo) => tab.id.toString())
        }
        .width('100%')
        .layoutWeight(1)

        // 底部工具栏：Home、书签、下载、标签、设置
        Row() {
          // Home 按钮
          Button() {
            Column() {
              Image($r('app.media.home'))
                .width(24)
                .height(24)
                .fillColor(this.darkMode ? '#FFFFFF' : '#666666')
              Text('主页')
                .fontSize(11)
                .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                .margin({ top: 2 })
            }
          }
          .layoutWeight(1)
          .height(60)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.inputUrl = this.homeUrl;
            this.loadUrl();
          })

          // 分隔线
          Divider()
            .vertical(true)
            .height(40)
            .color('#E0E0E0')

          // 书签按钮
          Button() {
            Column() {
              Image($r('app.media.bookmark'))
                .width(24)
                .height(24)
                .fillColor(this.darkMode ? '#FFFFFF' : '#666666')
              Text('书签')
                .fontSize(11)
                .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                .margin({ top: 2 })
            }
          }
          .layoutWeight(1)
          .height(60)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            // 点击打开书签列表
            this.pathStack.pushPathByName('BookmarkPage', null);
          })

          // 分隔线
          Divider()
            .vertical(true)
            .height(40)
            .color('#E0E0E0')

          // 下载按钮
          Button() {
            Column() {
              Stack() {
                Image($r('app.media.web'))
                  .width(20)
                  .height(20)
                  .fillColor(this.darkMode ? '#FFFFFF' : '#666666')

                Text('↓')
                  .fontSize(10)
                  .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                  .fontWeight(FontWeight.Bold)
                  .margin({ top: 2 })
              }

              Text('下载')
                .fontSize(11)
                .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                .margin({ top: 2 })
            }
          }
          .layoutWeight(1)
          .height(60)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.pathStack.pushPathByName('DownloadPage', null);
          })

          // 分隔线
          Divider()
            .vertical(true)
            .height(40)
            .color('#E0E0E0')

          // 历史按钮
          Button() {
            Column() {
              Image($r('app.media.history'))
                .width(24)
                .height(24)
                .fillColor(this.darkMode ? '#FFFFFF' : '#666666')
              Text('历史')
                .fontSize(11)
                .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                .margin({ top: 2 })
            }
          }
          .layoutWeight(1)
          .height(60)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            // 点击打开历史记录
            this.pathStack.pushPathByName('HistoryPage', null);
          })

          // 分隔线
          Divider()
            .vertical(true)
            .height(40)
            .color('#E0E0E0')

          // 标签按钮
          Button() {
            Column() {
              Stack() {
                Text('□')
                  .fontSize(24)
                  .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                Text(this.tabs.length.toString())
                  .fontSize(10)
                  .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                  .fontWeight(FontWeight.Bold)
              }

              Text('标签')
                .fontSize(11)
                .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                .margin({ top: 2 })
            }
          }
          .layoutWeight(1)
          .height(60)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.showTabList = true;
          })

          // 分隔线
          Divider()
            .vertical(true)
            .height(40)
            .color('#E0E0E0')

          // 设置按钮
          Button() {
            Column() {
              Image($r('app.media.settings'))
                .width(24)
                .height(24)
                .fillColor(this.darkMode ? '#FFFFFF' : '#666666')

              Text('设置')
                .fontSize(11)
                .fontColor(this.darkMode ? '#FFFFFF' : '#666666')
                .margin({ top: 2 })
            }
          }
          .layoutWeight(1)
          .height(60)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            // 使用Navigation跳转
            this.pathStack.pushPathByName('SettingsPage', null);
          })
        }
        .width('100%')
        .backgroundColor(this.darkMode ? '#2C2C2E' : '#FAFAFA')
        .border({ width: { top: 0.5 }, color: this.darkMode ? '#38383A' : '#E0E0E0' })
      }
      .width('100%')
      .height('100%')
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
      .bindContentCover(this.showTabList, this.tabListDialog(), {
        modalTransition: ModalTransition.DEFAULT
      })
    }
    .title('浏览器')
    .titleMode(NavigationTitleMode.Mini)
    .mode(NavigationMode.Stack)
    .hideTitleBar(true)
    .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
    .navBarPosition(NavBarPosition.Start)
  }

  // 标签列表对话框
  @Builder
  tabListDialog() {
    Column() {
      Column() {
        // 标题栏
        Row() {
          Text('标签页')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .layoutWeight(1)

          // 隐私模式开关
          Row({ space: 6 }) {
            if (this.incognitoMode) {
              Image($r('app.media.incognito'))
                .width(16)
                .height(16)
                .fillColor('#8E8E93')
            }
            Text('隐私')
              .fontSize(14)
              .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            Toggle({ type: ToggleType.Switch, isOn: this.incognitoMode })
              .selectedColor('#34C759')
              .width(40)
              .height(24)
              .onChange((value: boolean) => {
                this.incognitoMode = value;
                console.log(`隐私模式已${value ? '开启' : '关闭'}`);
              })
          }
          .margin({ right: 12 })

          Button('新建')
            .fontSize(16)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .onClick(() => {
              this.createNewTab();
            })

          Button('关闭')
            .fontSize(16)
            .backgroundColor(Color.Transparent)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .onClick(() => {
              this.showTabList = false;
            })
        }
        .width('100%')
        .height(56)
        .padding({ left: 16, right: 16 })
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')

        // 标签列表
        Scroll() {
          Column({ space: 12 }) {
            ForEach(this.tabs, (tab: TabInfo, index: number) => {
              Row() {
                // 隐私模式图标
                if (tab.incognitoMode) {
                  Image($r('app.media.incognito'))
                    .width(20)
                    .height(20)
                    .fillColor('#8E8E93')
                    .margin({ right: 8 })
                }

                Column({ space: 4 }) {
                  Row({ space: 4 }) {
                    Text(tab.title || '新标签页')
                      .fontSize(16)
                      .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
                      .fontWeight(index === this.currentTabIndex ? FontWeight.Bold : FontWeight.Normal)
                    if (tab.incognitoMode) {
                      Text('隐私')
                        .fontSize(10)
                        .fontColor('#8E8E93')
                        .padding({
                          left: 4,
                          right: 4,
                          top: 2,
                          bottom: 2
                        })
                        .backgroundColor(this.darkMode ? '#38383A' : '#E5E5EA')
                        .borderRadius(4)
                    }
                  }

                  Text(tab.url)
                    .fontSize(12)
                    .fontColor('#8E8E93')
                    .maxLines(1)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })
                }
                .alignItems(HorizontalAlign.Start)
                .layoutWeight(1)

                // 关闭按钮
                Image($r('app.media.close'))
                  .width(20)
                  .height(20)
                  .fillColor('#FF3B30')
                  .onClick(() => {
                    this.closeTab(index);
                  })
              }
              .width('100%')
              .padding(12)
              .backgroundColor(index === this.currentTabIndex ?
                (this.darkMode ? '#2C2C2E' : '#E5E5EA') :
                (this.darkMode ? '#1C1C1E' : '#FFFFFF'))
              .borderRadius(8)
              .onClick(() => {
                this.switchTab(index);
                this.showTabList = false;
              })
            })
          }
          .width('100%')
          .padding(12)
        }
        .layoutWeight(1)
        .backgroundColor(this.darkMode ? '#000000' : '#F2F2F7')
      }
      .width('90%')
      .height('80%')
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
      .borderRadius(12)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('rgba(0, 0, 0, 0.5)')
    .justifyContent(FlexAlign.Center)
    .onClick(() => {
      this.showTabList = false;
    })
  }

  // 创建新标签页
  createNewTab() {
    console.log('=== 创建新标签页 ===');
    console.log(`当前标签数: ${this.tabs.length}`);
    console.log(`隐私模式: ${this.incognitoMode}`);
    console.log(`主页URL: ${this.homeUrl}`);

    const newController = new webview.WebviewController();
    const newTab: TabInfo = {
      id: Date.now(),
      url: this.homeUrl,
      title: '新标签页',
      controller: newController,
      incognitoMode: this.incognitoMode
    };

    this.tabs.push(newTab);
    console.log(`标签已添加，ID: ${newTab.id}`);
    console.log(`新标签数: ${this.tabs.length}`);

    // 只更新索引和UI，不访问controller（因为Web组件还未创建）
    this.currentTabIndex = this.tabs.length - 1;
    this.currentUrl = newTab.url;
    this.inputUrl = newTab.url;
    this.canGoBack = false;
    this.canGoForward = false;

    console.log(`切换到新标签索引: ${this.currentTabIndex}`);
    this.showTabList = false;
  }

  // 切换标签页
  switchTab(index: number) {
    console.log(`=== 切换标签页 ===`);
    console.log(`目标索引: ${index}`);
    console.log(`当前索引: ${this.currentTabIndex}`);

    if (index >= 0 && index < this.tabs.length) {
      this.currentTabIndex = index;
      const tab = this.tabs[index];
      console.log(`标签ID: ${tab.id}`);
      console.log(`标签URL: ${tab.url}`);
      console.log(`隐私模式: ${tab.incognitoMode}`);

      // 更新显示的URL和输入框
      this.currentUrl = tab.url;
      this.inputUrl = tab.url;

      // 安全地更新导航按钮状态
      try {
        this.canGoBack = tab.controller.accessBackward();
        this.canGoForward = tab.controller.accessForward();
        console.log(`导航状态已更新: 后退=${this.canGoBack}, 前进=${this.canGoForward}`);
      } catch (error) {
        // Web组件还未关联时是正常情况，不显示错误
        console.log('Controller还未关联，跳过导航状态更新');
        this.canGoBack = false;
        this.canGoForward = false;
      }

      console.log('标签切换完成');
    } else {
      console.error(`无效的标签索引: ${index}`);
    }
  }

  // 关闭标签页
  closeTab(index: number) {
    if (this.tabs.length === 1) {
      // 最后一个标签页，不允许关闭
      return;
    }

    this.tabs.splice(index, 1);

    if (this.currentTabIndex >= this.tabs.length) {
      this.currentTabIndex = this.tabs.length - 1;
    }

    if (index <= this.currentTabIndex && this.currentTabIndex > 0) {
      this.currentTabIndex--;
    }

    this.switchTab(this.currentTabIndex);
  }

  // 加载URL的方法
  loadUrl() {
    try {
      let url = this.inputUrl.trim();
      // 如果没有协议头，自动添加https://
      if (url && !url.startsWith('http://') && !url.startsWith('https://')) {
        url = 'https://' + url;
      }
      if (url && this.currentTabIndex >= 0 && this.currentTabIndex < this.tabs.length) {
        const currentTab = this.tabs[this.currentTabIndex];
        currentTab.controller.loadUrl(url);
        this.currentUrl = url;
        currentTab.url = url;
      }
    } catch (error) {
      console.error(`加载URL失败: ${(error as BusinessError).code}, ${(error as BusinessError).message}`);
    }
  }

  // 更新前进后退按钮状态
  updateNavigationState() {
    try {
      if (this.currentTabIndex >= 0 && this.currentTabIndex < this.tabs.length) {
        const currentTab = this.tabs[this.currentTabIndex];
        this.canGoBack = currentTab.controller.accessBackward();
        this.canGoForward = currentTab.controller.accessForward();
      }
    } catch (error) {
      console.error(`更新导航状态失败: ${(error as BusinessError).code}, ${(error as BusinessError).message}`);
      this.canGoBack = false;
      this.canGoForward = false;
    }
  }

  // 加载设置
  loadSettings() {
    try {
      const context = this.getUIContext().getHostContext();
      const dataPreferences = preferences.getPreferencesSync(context, { name: 'browser_settings' });

      this.homeUrl = dataPreferences.getSync('homeUrl', 'https://www.huawei.com') as string;
      this.darkMode = dataPreferences.getSync('darkMode', false) as boolean;
      this.forceDarkMode = dataPreferences.getSync('forceDarkMode', true) as boolean;
      // 隐私模式默认关闭，不从设置中读取（避免被错误保存为true）
      this.incognitoMode = false;
      this.javaScriptEnabled = dataPreferences.getSync('javaScriptEnabled', true) as boolean;
      this.imageEnabled = dataPreferences.getSync('imageEnabled', true) as boolean;
      this.textZoom = dataPreferences.getSync('textZoom', 100) as number;
      this.domStorageEnabled = dataPreferences.getSync('domStorageEnabled', true) as boolean;
      this.geolocationEnabled = dataPreferences.getSync('geolocationEnabled', true) as boolean;
      this.clearCacheOnExit = dataPreferences.getSync('clearCacheOnExit', false) as boolean;

      // 确保隐私模式设置为false并保存
      dataPreferences.putSync('incognitoMode', false);
      dataPreferences.flush();

      // 设置初始URL为主页地址
      this.currentUrl = this.homeUrl;
      this.inputUrl = this.homeUrl;

      console.info(`加载设置完成 - 隐私模式: ${this.incognitoMode}`);
    } catch (error) {
      console.error(`加载设置失败: ${error}`);
    }
  }

  // 监听设置变化
  watchSettings() {
    // 监听深色模式变化
    this.storageDarkMode = this.darkMode;

    // 使用定时器监听设置变化（简化方案）
    setInterval(() => {
      if (this.storageDarkMode !== this.darkMode) {
        this.darkMode = this.storageDarkMode;
      }
      if (this.storageForceDarkMode !== this.forceDarkMode) {
        this.forceDarkMode = this.storageForceDarkMode;
      }
      if (this.storageHomeUrl && this.storageHomeUrl !== this.homeUrl) {
        this.homeUrl = this.storageHomeUrl;
      }
      if (this.storageJavaScriptEnabled !== this.javaScriptEnabled) {
        this.javaScriptEnabled = this.storageJavaScriptEnabled;
      }
      if (this.storageImageEnabled !== this.imageEnabled) {
        this.imageEnabled = this.storageImageEnabled;
      }
      if (this.storageTextZoom !== this.textZoom) {
        this.textZoom = this.storageTextZoom;
      }

      // 处理清除缓存请求
      if (this.storageClearCache > 0) {
        try {
          this.controller.removeCache(true);
        } catch (error) {
          // TODO: Implement error handling.
        }
        console.info('缓存已清除');
        AppStorage.setOrCreate('clearCache', 0);
      }

      // 处理清除Cookies请求
      if (this.storageClearCookies > 0) {
        try {
          webview.WebCookieManager.clearAllCookies();
          console.info('Cookies已清除');
        } catch (err) {
          console.error('清除Cookies失败:', err);
        }
        AppStorage.setOrCreate('clearCookies', 0);
      }

      // 处理从历史/书签页面返回时打开URL
      if (this.storageOpenUrl && this.storageOpenUrl.length > 0) {
        this.inputUrl = this.storageOpenUrl;
        this.loadUrl();
        AppStorage.setOrCreate('openUrl', '');
      }
    }, 500);
  }

  // 监听书签变化
  watchBookmarkChanges() {
    setInterval(() => {
      // 当书签数据变化时，重新检查当前页面的收藏状态
      if (this.storageBookmarkChanged > 0) {
        this.isCurrentPageBookmarked = dataManager.isBookmarked(this.currentUrl);
        console.info(`书签状态已更新: ${this.currentUrl} - ${this.isCurrentPageBookmarked}`);
      }
    }, 100);
  }

  // 检查是否需要清除缓存
  checkClearCache() {
    try {
      if (this.clearCacheOnExit) {
        // 使用controller实例的removeCache方法清除缓存
        this.controller.removeCache(true);
        console.info('已清除所有缓存');
      }
    } catch (error) {
      console.error(`清除缓存失败: ${error}`);
    }
  }

  // 切换书签状态
  async toggleBookmark() {
    if (!this.currentUrl || this.currentUrl === 'about:blank') {
      promptAction.openToast({ message: '无法收藏此页面' }).catch((err: BusinessError) => {
        console.error('显示Toast失败:', err);
      });
      return;
    }

    if (this.isCurrentPageBookmarked) {
      // 取消收藏
      await dataManager.deleteBookmarkByUrl(this.currentUrl);
      this.isCurrentPageBookmarked = false;
      promptAction.openToast({ message: '已取消收藏' }).catch((err: BusinessError) => {
        console.error('显示Toast失败:', err);
      });
    } else {
      // 添加收藏
      const currentTab = this.tabs[this.currentTabIndex];
      const title = currentTab?.title || this.currentUrl;
      const success = await dataManager.addBookmark(this.currentUrl, title);
      if (success) {
        this.isCurrentPageBookmarked = true;
        promptAction.openToast({ message: '已添加到书签' }).catch((err: BusinessError) => {
          console.error('显示Toast失败:', err);
        });
      } else {
        promptAction.openToast({ message: '添加书签失败' }).catch((err: BusinessError) => {
          console.error('显示Toast失败:', err);
        });
      }
    }
  }
}