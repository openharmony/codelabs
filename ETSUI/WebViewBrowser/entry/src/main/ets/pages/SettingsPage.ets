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

import { preferences } from '@kit.ArkData';
import { promptAction } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';

// 定义 Select 选项接口
interface SelectOption {
  value: string;
}

// Builder函数，用于Navigation路由
@Builder
export function SettingsPageBuilder() {
  SettingsPage();
}

@Component
export struct SettingsPage {
  // 获取全局NavPathStack
  pathStack: NavPathStack = AppStorage.get('pathStack') as NavPathStack;
  // 基础设置
  @State homeUrl: string = 'http://www.huawei.com';
  @State darkMode: boolean = false;
  @State forceDarkMode: boolean = true;
  @State javaScriptEnabled: boolean = true;
  @State imageEnabled: boolean = true;
  @State textZoom: number = 100;
  // 隐私设置
  @State domStorageEnabled: boolean = true;
  @State geolocationEnabled: boolean = true;
  @State clearCacheOnExit: boolean = false;
  // 显示输入对话框的状态
  @State showUrlDialog: boolean = false;
  @State tempHomeUrl: string = '';

  aboutToAppear() {
    this.loadSettings();
  }

  build() {
    NavDestination() {
      Column() {
        // 自定义标题栏
        Row() {
          // 返回按钮
          Image($r('app.media.back'))
            .width(24)
            .height(24)
            .fillColor(this.darkMode ? '#FFFFFF' : '#000000')
            .margin({ left: 16 })
            .onClick(() => {
              this.pathStack.pop();
            })

          Text('设置')
            .fontSize(18)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .fontWeight(FontWeight.Medium)
            .margin({ left: 16 })

          Blank()
        }
        .width('100%')
        .height(56)
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
        .alignItems(VerticalAlign.Center)

        // 内容区域
        Scroll() {
          Column({ space: 0 }) {
            // 常规设置
            this.SettingSection('常规设置')

            // 主页地址
            Row() {
              Text('主页地址')
                .fontSize(16)
                .fontColor(this.darkMode ? '#FFFFFF' : '#000000')

              Blank()

              Text(this.homeUrl)
                .fontSize(16)
                .fontColor('#8E8E93')
                .maxLines(1)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .layoutWeight(1)
                .textAlign(TextAlign.End)
                .margin({ left: 12, right: 8 })

              Text('›')
                .fontSize(20)
                .fontColor('#C7C7CC')
            }
            .width('100%')
            .height(50)
            .padding({ left: 16, right: 16 })
            .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
            .border({ width: { bottom: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
            .onClick(() => {
              this.tempHomeUrl = this.homeUrl;
              this.showUrlDialog = true;
            })

            this.ToggleItem('深色模式', this.darkMode, (value: boolean) => {
              this.darkMode = value;
              this.saveSettings();
              // 通知主页面更新主题
              AppStorage.setOrCreate('darkMode', value);
            })

            // 强制深色模式（仅在深色模式开启时可用）
            Row() {
              Text('强制深色模式')
                .fontSize(16)
                .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
                .opacity(this.darkMode ? 1.0 : 0.5)
                .layoutWeight(1)

              Toggle({ type: ToggleType.Switch, isOn: this.forceDarkMode })
                .selectedColor('#34C759')
                .enabled(this.darkMode)
                .onChange((value: boolean) => {
                  if (this.darkMode) {
                    this.forceDarkMode = value;
                    this.saveSettings();
                    AppStorage.setOrCreate('forceDarkMode', value);
                  }
                })
            }
            .width('100%')
            .height(50)
            .padding({ left: 16, right: 16 })
            .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
            .border({ width: { bottom: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
            .opacity(this.darkMode ? 1.0 : 0.6)

            // 网页设置
            this.SettingSection('网页设置')

            this.ToggleItem('启用 JavaScript', this.javaScriptEnabled, (value: boolean) => {
              this.javaScriptEnabled = value;
              this.saveSettings();
              AppStorage.setOrCreate('javaScriptEnabled', value);
            })

            this.ToggleItem('加载图片', this.imageEnabled, (value: boolean) => {
              this.imageEnabled = value;
              this.saveSettings();
              AppStorage.setOrCreate('imageEnabled', value);
            })

            // 文字缩放
            Column({ space: 8 }) {
              Row() {
                Text('文字缩放')
                  .fontSize(16)
                  .fontColor(this.darkMode ? '#FFFFFF' : '#000000')

                Blank()

                Text(`${this.textZoom}%`)
                  .fontSize(16)
                  .fontColor('#8E8E93')
              }
              .width('100%')

              Slider({
                value: this.textZoom,
                min: 50,
                max: 200,
                step: 10,
                style: SliderStyle.OutSet
              })
                .width('100%')
                .blockColor('#007AFF')
                .trackColor(this.darkMode ? '#48484A' : '#E5E5EA')
                .selectedColor('#007AFF')
                .onChange((value: number) => {
                  this.textZoom = Math.round(value);
                  this.saveSettings();
                  AppStorage.setOrCreate('textZoom', this.textZoom);
                })
            }
            .width('100%')
            .padding({
              left: 16,
              right: 16,
              top: 12,
              bottom: 12
            })
            .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
            .border({ width: { bottom: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })

            // 隐私与安全
            this.SettingSection('隐私与安全')

            this.ToggleItem('启用 Cookie', this.domStorageEnabled, (value: boolean) => {
              this.domStorageEnabled = value;
              this.saveSettings();
              AppStorage.setOrCreate('domStorageEnabled', value);
            })

            this.ToggleItem('允许定位', this.geolocationEnabled, (value: boolean) => {
              this.geolocationEnabled = value;
              this.saveSettings();
              AppStorage.setOrCreate('geolocationEnabled', value);
            })

            this.ToggleItem('退出时清除缓存', this.clearCacheOnExit, (value: boolean) => {
              this.clearCacheOnExit = value;
              this.saveSettings();
            })

            // 数据管理
            this.SettingSection('数据管理')

            // 统计信息
            Row() {
              Text('统计信息')
                .fontSize(16)
                .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
                .layoutWeight(1)

              Text('›')
                .fontSize(20)
                .fontColor('#C7C7CC')
            }
            .width('100%')
            .height(50)
            .padding({ left: 16, right: 16 })
            .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
            .border({ width: { bottom: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
            .onClick(() => {
              this.pathStack.pushPathByName('StatisticsPage', null);
            })

            this.ActionItem('清除缓存', () => {
              this.clearCache();
            })

            this.ActionItem('清除Cookie', () => {
              this.clearCookies();
            })

            // 关于
            this.SettingSection('关于')
            this.InfoItem('应用名称', 'WebView 浏览器')
            this.InfoItem('版本', '1.0.0')
            this.InfoItem('HarmonyOS API', '20')

          }
          .width('100%')
          .padding({ bottom: 20 })
        }
        .layoutWeight(1)
        .scrollBar(BarState.Auto)
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#F2F2F7')
      }
      .width('100%')
      .height('100%')
      .bindContentCover(this.showUrlDialog, this.urlInputDialog(), {
        modalTransition: ModalTransition.DEFAULT
      })
    }
    .hideTitleBar(true)
    .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
    .mode(NavDestinationMode.STANDARD)
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }

  // URL输入对话框
  @Builder
  urlInputDialog() {
    Column() {
      Column({ space: 20 }) {
        Text('设置主页地址')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)

        TextInput({ text: this.tempHomeUrl, placeholder: '请输入网址' })
          .width('100%')
          .height(50)
          .fontSize(16)
          .onChange((value: string) => {
            this.tempHomeUrl = value;
          })

        Row({ space: 12 }) {
          Button('取消')
            .layoutWeight(1)
            .backgroundColor('#F0F0F0')
            .fontColor('#000000')
            .onClick(() => {
              this.showUrlDialog = false;
            })

          Button('确定')
            .layoutWeight(1)
            .backgroundColor('#007AFF')
            .onClick(() => {
              if (this.tempHomeUrl.trim()) {
                let url = this.tempHomeUrl.trim();
                // if (!url.startsWith('http://') && !url.startsWith('https://')) {
                //   url = 'https://' + url;
                // }
                this.homeUrl = url;
                this.saveSettings();
                AppStorage.setOrCreate('homeUrl', url);
                promptAction.openToast({ message: '主页地址已更新' }).catch((err: BusinessError) => {
                  console.error('显示Toast失败:', err);
                });
              }
              this.showUrlDialog = false;
            })
        }
        .width('100%')
      }
      .width('80%')
      .padding(24)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('rgba(0, 0, 0, 0.5)')
    .justifyContent(FlexAlign.Center)
    .onClick(() => {
      this.showUrlDialog = false;
    })
  }

  // 设置分组标题
  @Builder
  SettingSection(title: string) {
    Text(title)
      .fontSize(13)
      .fontColor('#8E8E93')
      .width('100%')
      .padding({
        left: 16,
        right: 16,
        top: 20,
        bottom: 8
      })
      .textCase(TextCase.UpperCase)
  }

  // 开关项
  @Builder
  ToggleItem(label: string, value: boolean, onChange: (value: boolean) => void) {
    Row() {
      Text(label)
        .fontSize(16)
        .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
        .layoutWeight(1)

      Toggle({ type: ToggleType.Switch, isOn: value })
        .selectedColor('#34C759')
        .onChange(onChange)
    }
    .width('100%')
    .height(50)
    .padding({ left: 16, right: 16 })
    .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
    .border({ width: { bottom: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
  }

  // 操作按钮项
  @Builder
  ActionItem(label: string, onClick: () => void) {
    Row() {
      Text(label)
        .fontSize(16)
        .fontColor('#007AFF')
        .layoutWeight(1)
        .textAlign(TextAlign.Center)
    }
    .width('100%')
    .height(50)
    .padding({ left: 16, right: 16 })
    .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
    .border({ width: { bottom: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
    .onClick(onClick)
  }

  // 信息项
  @Builder
  InfoItem(label: string, value: string) {
    Row() {
      Text(label)
        .fontSize(16)
        .fontColor(this.darkMode ? '#FFFFFF' : '#000000')

      Blank()

      Text(value)
        .fontSize(16)
        .fontColor('#8E8E93')
    }
    .width('100%')
    .height(50)
    .padding({ left: 16, right: 16 })
    .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
    .border({ width: { bottom: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
  }

  // 保存设置到本地
  async saveSettings() {
    try {
      const context = this.getUIContext().getHostContext();
      const dataPreferences = preferences.getPreferencesSync(context, { name: 'browser_settings' });

      dataPreferences.putSync('homeUrl', this.homeUrl);
      dataPreferences.putSync('darkMode', this.darkMode);
      dataPreferences.putSync('forceDarkMode', this.forceDarkMode);
      dataPreferences.putSync('javaScriptEnabled', this.javaScriptEnabled);
      dataPreferences.putSync('imageEnabled', this.imageEnabled);
      dataPreferences.putSync('textZoom', this.textZoom);
      dataPreferences.putSync('domStorageEnabled', this.domStorageEnabled);
      dataPreferences.putSync('geolocationEnabled', this.geolocationEnabled);
      dataPreferences.putSync('clearCacheOnExit', this.clearCacheOnExit);

      await dataPreferences.flush();
      console.info('设置已保存');
    } catch (error) {
      console.error(`保存设置失败: ${error}`);
    }
  }

  // 从本地加载设置
  loadSettings() {
    try {
      const context = this.getUIContext().getHostContext();
      const dataPreferences = preferences.getPreferencesSync(context, { name: 'browser_settings' });

      this.homeUrl = dataPreferences.getSync('homeUrl', 'http://www.huawei.com') as string;
      this.darkMode = dataPreferences.getSync('darkMode', false) as boolean;
      this.forceDarkMode = dataPreferences.getSync('forceDarkMode', true) as boolean;
      this.javaScriptEnabled = dataPreferences.getSync('javaScriptEnabled', true) as boolean;
      this.imageEnabled = dataPreferences.getSync('imageEnabled', true) as boolean;
      this.textZoom = dataPreferences.getSync('textZoom', 100) as number;
      this.domStorageEnabled = dataPreferences.getSync('domStorageEnabled', true) as boolean;
      this.geolocationEnabled = dataPreferences.getSync('geolocationEnabled', true) as boolean;
      this.clearCacheOnExit = dataPreferences.getSync('clearCacheOnExit', false) as boolean;

      console.info('设置已加载');
    } catch (error) {
      console.error(`加载设置失败: ${error}`);
    }
  }

  // 清除缓存
  clearCache() {
    try {
      // 通知主页面清除缓存
      AppStorage.setOrCreate('clearCache', Date.now());
      promptAction.openToast({
        message: '缓存已清除',
        duration: 2000
      }).catch((err: BusinessError) => {
        console.error('显示Toast失败:', err);
      });
    } catch (error) {
      console.error(`清除缓存失败: ${error}`);
      promptAction.openToast({
        message: '清除缓存失败',
        duration: 2000
      }).catch((err: BusinessError) => {
        console.error('显示Toast失败:', err);
      });
    }
  }

  // 清除Cookies
  clearCookies() {
    try {
      // 通知主页面清除Cookies
      AppStorage.setOrCreate('clearCookies', Date.now());
      promptAction.openToast({
        message: 'Cookies已清除',
        duration: 2000
      }).catch((err: BusinessError) => {
        console.error('显示Toast失败:', err);
      });
    } catch (error) {
      console.error(`清除Cookies失败: ${error}`);
      promptAction.openToast({
        message: '清除Cookies失败',
        duration: 2000
      }).catch((err: BusinessError) => {
        console.error('显示Toast失败:', err);
      });
    }
  }
}
