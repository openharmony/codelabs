/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

import type window from '@ohos.window';
import Log from '../utils/LoggerUtil';
import { CommonConstants } from '../common/constant/CommonConstants';

const TAG: string = 'common_ScreenManager';

type SystemBarKeys = 'status' | 'navigation';

/**
 * Column size.
 */
export enum ColumnSize {
  COLUMN_ONE_POINT_FIVE = 1.5,
  COLUMN_TWO = 2,
  COLUMN_FOUR = 4,
  COLUMN_SIX = 6,
  COLUMN_EIGHT = 8,
  COLUMN_TWELVE = 12
}

/**
 * Screen width.
 */
enum ScreenWidth {
  WIDTH_MEDIUM = 520,
  WIDTH_LARGE = 840
}

/**
 * Window mode.
 */
enum WindowMode {
  UNDEFINED = 1,
  FULLSCREEN,
  PRIMARY,
  SECONDARY,
  FLOATING
}

export class ScreenManager {
  static readonly DEFAULT_WIDTH: number = 1920;
  static readonly DEFAULT_HEIGHT: number = 1080;
  private static readonly SPLIT_THRESHOLD = 1.7;
  private winWidth = 0.0;
  private winHeight = 0.0;
  private statusBarHeight = 0;
  private naviBarHeight = 0;
  private leftBlank: [number, number, number, number] = [0, 0, 0, 0];
  private mainWindow = undefined;
  private columns: number = ColumnSize.COLUMN_FOUR;

  // Default orientation for Pc
  private horizontal: boolean = true;

  // Default sidebar for Pc
  private sidebar: boolean = true;
  private windowMode = WindowMode.UNDEFINED;

  private constructor() {
    Log.info(TAG, 'constructor');
  }

  /**
   * Get screen instance.
   * @returns
   */
  static getInstance(): ScreenManager {
    if (AppStorage.Get(CommonConstants.APP_KEY_SCREEN_MANAGER) == null) {
      AppStorage.SetOrCreate(CommonConstants.APP_KEY_SCREEN_MANAGER, new ScreenManager());
    }
    let manager: ScreenManager = AppStorage.Get(CommonConstants.APP_KEY_SCREEN_MANAGER);
    return manager;
  }

  /**
   * Init.
   * @param win
   * @returns
   */
  async initializationSize(win: window.Window): Promise<void> {
    this.mainWindow = win;
    let properties = await win.getProperties();

    AppStorage.SetOrCreate<boolean>(CommonConstants.IS_FULL_SCREEN_KEY, properties.isFullScreen);
    // Area data obtained through the system interface,
    // There is a possibility that the area data is incorrect.
    const statusBarHeight = properties && properties.windowRect ? properties.windowRect.top : this.statusBarHeight;
    AppStorage.SetOrCreate<number>('statusBarHeight', statusBarHeight);
    return new Promise<void>((resolve, reject) => {
      if (!properties || !properties.windowRect) {
        reject();
      }
      let size = properties.windowRect;
      Log.info(TAG, `display screen windowRect: ${JSON.stringify(size)}`);
      this.winWidth = px2vp(size.width);
      this.winHeight = px2vp(size.height);
      Log.info(TAG, `display screen windowRect px2vp: ${this.winWidth} ${this.winHeight}`);
      if (this.winWidth < ScreenWidth.WIDTH_MEDIUM) {
        this.columns = ColumnSize.COLUMN_FOUR;
      } else if (this.winWidth >= ScreenWidth.WIDTH_MEDIUM && this.winWidth < ScreenWidth.WIDTH_LARGE) {
        this.columns = ColumnSize.COLUMN_EIGHT;
      } else {
        this.columns = ColumnSize.COLUMN_TWELVE;
      }
      resolve();
    });
  }

  /**
   * Get window width.
   * unit: vp.
   */
  getWinWidth(): number {
    return this.winWidth;
  }

  /**
   * Get window height.
   * unit: vp.
   */
  getWinHeight(): number {
    return this.winHeight;
  }

  /**
   * Get status bar height.
   * @returns
   */
  getStatusBarHeight(): number {
    return this.statusBarHeight;
  }

  /**
   * Get navi bar height.
   * @returns
   */
  getNaviBarHeight(): number {
    return this.naviBarHeight;
  }

  /**
   * Init window mode.
   */
  initWindowMode(): void {
    Log.debug(TAG, `start to initialize photos application window mode: ${this.windowMode}`);
    this.checkWindowMode();
    this.mainWindow && this.setMainWindow();
  }

  /**
   * Determine whether it is split screen mode.
   * @returns
   */
  isSplitMode(): boolean {
    return (WindowMode.PRIMARY === this.windowMode || WindowMode.SECONDARY === this.windowMode);
  }

  /**
   * Check screen mode.
   * @returns
   */
  async checkWindowMode(): Promise<void> {
    let before = this.windowMode;
    let mode = await globalThis.photosWindowStage.getWindowMode();
    Log.info(TAG, `photos application before/current window mode: ${before}/${mode}`);

    if (before == mode) {
      return;
    }
    this.windowMode = mode;
    if (WindowMode.FULLSCREEN == this.windowMode) {
      this.setFullScreen();
    } else {
      this.setSplitScreen();
    }
  }

  /**
   * Set main window.
   */
  setMainWindow(): void {
    Log.debug(TAG, 'setMainWindow');
    this.mainWindow.on('windowSizeChange', (data) => {
      Log.debug(TAG, `windowSizeChange ${JSON.stringify(data)}`);
      try {
        let properties = this.mainWindow.getWindowProperties();
        AppStorage.SetOrCreate<boolean>(CommonConstants.IS_FULL_SCREEN_KEY, properties.isFullScreen);
      } catch (exception) {
        Log.error(TAG, 'Failed to obtain the area. Cause:' + JSON.stringify(exception));
      }
      this.onWinSizeChanged(data);
    })
    this.mainWindow.getProperties().then(prop => {
      Log.info(TAG, `Window prop: ${JSON.stringify(prop)}`);
      this.onWinSizeChanged(prop.windowRect);
    });
  }

  /**
   * Get main window.
   * @returns
   */
  private getMainWindow(): window.Window {
    return AppStorage.Get<window.Window>('mainWindow');
  }

  /**
   * Get avoid area.
   */
  getAvoidArea(): void {
    let topWindow = this.getMainWindow();
    topWindow.getAvoidArea(0, (err, data) => {
      Log.info(TAG, 'Succeeded in obtaining the area. Data:' + JSON.stringify(data));
      this.onLeftBlankChanged(data);
    });
  }

  /**
   * Set full Screen.
   */
  setFullScreen(): void {
    let topWindow = this.getMainWindow();
    Log.debug(TAG, 'getTopWindow start');
    try {
      topWindow.setLayoutFullScreen(true, () => {
        Log.debug(TAG, 'setFullScreen true Succeeded');
        if (AppStorage.Get('deviceType') !== CommonConstants.DEFAULT_DEVICE_TYPE) {
          this.hideStatusBar();
        } else {
          this.setDefaultStatusBarProperties();
        }
      });
    } catch (err) {
      Log.error(TAG, `setFullScreen err: ${err}`);
    }
  }

  /**
   * Set split screen.
   */
  setSplitScreen(): void {
    try {
      this.statusBarHeight = 0;
      this.naviBarHeight = 0;
      this.leftBlank = [0, 0, 0, 0];
    } catch (err) {
      Log.error(TAG, `setSplitScreen err: ${err}`);
    }
  }

  /**
   * Hide status bar.
   */
  hideStatusBar(): void {
    Log.debug(TAG, 'hideStatusBar start');
    let topWindow = this.getMainWindow();
    Log.debug(TAG, 'getTopWindow start');
    let names = new Array<SystemBarKeys>('navigation');
    Log.debug(TAG, `getTopWindow names: ${names} end`);
    try {
      topWindow.setSystemBarEnable(names, () => {
        Log.debug(TAG, 'hideStatusBar Succeeded');
        topWindow.getAvoidArea(0, async (err, data) => {
          Log.info(TAG, `Succeeded in obtaining the area. Data: ${JSON.stringify(data)}`);
          this.onLeftBlankChanged(data);
          // let barColor = await UiUtil.getResourceString($r('app.color.transparent'));
          let barColor = '#00000000';
          if (!barColor) {
            barColor = '#00000000';
          }
          topWindow.setSystemBarProperties({ navigationBarColor: barColor },
            () => {
              Log.info(TAG, 'setStatusBarColor done');
            });
        });
      });
    } catch (err) {
      Log.error(TAG, `hideStatusBar err: ${err}`);
    }
  }

  /**
   * Set default status bar properties.
   * @returns
   */
  async setDefaultStatusBarProperties(): Promise<void> {
    Log.debug(TAG, 'setStatusBarColor start');
    let topWindow = this.getMainWindow();
    try {
      topWindow.setSystemBarProperties(
        { statusBarColor: CommonConstants.STATUS_BAR_BACKGROUND_COLOR,
          statusBarContentColor: CommonConstants.STATUS_BAR_CONTENT_COLOR }, () => {
        Log.info(TAG, 'setStatusBarColor done');
      });
    } catch (err) {
      Log.error(TAG, `setStatusBarColor err: ${err}`);
    }
  }

  /**
   * Avoid area left.
   * @param area
   */
  private onLeftBlankChanged(area): void {
    if (area == null || area == undefined || area.bottomRect.height == 0) {
      return;
    }
    let leftBlankBefore = {
      status: this.statusBarHeight,
      navi: this.naviBarHeight
    };
    // Area data obtained through the system interface,
    // There is a possibility that the area data is incorrect.
    AppStorage.SetOrCreate<number>('statusBarHeight', area.topRect.height);
    this.statusBarHeight = px2vp(area.topRect.height);
    this.naviBarHeight = px2vp(area.bottomRect.height);
    this.leftBlank = [this.leftBlank[0], this.leftBlank[1], this.leftBlank[2], px2vp(area.bottomRect.height)];
    if (leftBlankBefore.status != this.statusBarHeight || leftBlankBefore.navi != this.naviBarHeight) {
      Log.info(TAG, `leftBlank changed: ${JSON.stringify(leftBlankBefore)}-${JSON.stringify(this.leftBlank)}`);
    }
  }

  /**
   * Listen window size change.
   * @param size
   */
  private onWinSizeChanged(size): void {
    Log.info(TAG, `onWinSizeChanged ${JSON.stringify(size)}`);
    if (size == null || size == undefined) {
      return;
    }
    let isSplitModeBefore = this.isSplitMode();
    this.checkWindowMode();
    let sizeBefore = {
      width: this.winWidth,
      height: this.winHeight
    };
    this.winWidth = px2vp(size.width);
    this.winHeight = px2vp(size.height);
    Log.info(TAG, `onWinSizeChanged px2vp: ${this.winWidth} ${this.winHeight}`);
    if (this.winWidth < ScreenWidth.WIDTH_MEDIUM) {
      this.columns = ColumnSize.COLUMN_FOUR;
    } else if (this.winWidth >= ScreenWidth.WIDTH_MEDIUM && this.winWidth < ScreenWidth.WIDTH_LARGE) {
      this.columns = ColumnSize.COLUMN_EIGHT;
    } else {
      this.columns = ColumnSize.COLUMN_TWELVE;
    }
    let isSplitModeNow = this.isSplitMode();
    if (isSplitModeBefore != isSplitModeNow) {
      Log.info(TAG, `splitMode changed: ${isSplitModeBefore} -> ${isSplitModeNow}`);
    }
    if (sizeBefore.width != this.winWidth || sizeBefore.height != this.winHeight) {
      let newSize = {
        width: this.winWidth,
        height: this.winHeight
      };
      Log.info(TAG, `winSize changed: ${JSON.stringify(sizeBefore)} -> ${JSON.stringify(newSize)}`);
    }
  }
}
