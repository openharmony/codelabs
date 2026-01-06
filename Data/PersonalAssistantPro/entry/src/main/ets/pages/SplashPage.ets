// entry/src/main/ets/pages/SplashPage.ets
import router from '@ohos.router';
import { RdbHelper } from '../common/database/RdbHelper';
import { Logger } from '../common/utils/Logger';
import { AesCryptoUtils } from '../common/utils/AesCryptoUtils';
import { PreferenceService } from '../services/PreferenceService';

@Entry
@Component
struct SplashPage {
  private logger = new Logger('SplashPage');

  aboutToAppear() {
    this.logger.info('SplashPage aboutToAppear');
    this.initApp();
  }

  async initApp() {
    const startTime = new Date().getTime();

    try {
      // 1. 初始化数据库
      this.logger.info('Step 1: Initializing RdbStore...');
      await RdbHelper.getInstance().initRdb(getContext(this));
      this.logger.info('RdbStore initialized successfully.');

      // 2. 初始化首选项服务
      this.logger.info('Step 2: Initializing Preferences...');
      await PreferenceService.getInstance().init(getContext(this));

      // 3. 初始化 AES 密钥
      this.logger.info('Step 3: Initializing Crypto Key...');
      const savedKey = await PreferenceService.getInstance().getAesKey();

      // ✅ 修复：直接调用静态方法，不再使用 getInstance()
      const activeKey = await AesCryptoUtils.initKey(savedKey);

      if (!savedKey || savedKey !== activeKey) {
        await PreferenceService.getInstance().saveAesKey(activeKey);
        this.logger.info('New AES Key generated and saved.');
      } else {
        this.logger.info('AES Key loaded from storage.');
      }

    } catch (err) {
      // 修复 catch 类型报错：使用 JSON.stringify 或断言
      this.logger.error(`App Init failed: ${JSON.stringify(err)}`);
    }

    // 4. 计算延时跳转
    const endTime = new Date().getTime();
    const duration = endTime - startTime;
    const MIN_DELAY = 1500;
    const delay = duration < MIN_DELAY ? (MIN_DELAY - duration) : 0;

    this.logger.info(`Initialization took ${duration}ms, waiting additional ${delay}ms`);

    setTimeout(() => {
      this.jumpToMain();
    }, delay);
  }

  jumpToMain() {
    this.logger.info('Jumping to MainPage...');

    router.replaceUrl({
      url: 'pages/MainPage'
    }, (err) => {
      if (err) {
        this.logger.error(`Jump failed: ${err.code}, ${err.message}`);
      } else {
        this.logger.info('Jump success');
      }
    });
  }

  build() {
    Column() {
      Image($r('app.media.app_icon'))
        .width(100)
        .height(100)
        .objectFit(ImageFit.Contain)
        .margin({ top: '30%' })

      Text('Personal Assistant Pro')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20 })

      LoadingProgress()
        .width(40)
        .height(40)
        .color(Color.Blue)
        .margin({ top: 50 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }
}