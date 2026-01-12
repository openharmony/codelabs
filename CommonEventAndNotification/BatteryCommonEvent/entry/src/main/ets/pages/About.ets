import { APP_NAME } from '../app';
import { TimeUtil } from '../utils/TimeUtil';
import router from '@ohos.router';

@Entry
@Component
export struct About {
  build() {
    Column({ space: 10 }) {
      Row() {
        Text('About').fontSize(20).fontWeight(FontWeight.Bold).layoutWeight(1);
        Button('Back').onClick(() => { router.back(); });
      }.padding({ left: 16, right: 16, top: 10 });

      Column({ space: 8 }) {
        Text(APP_NAME).fontSize(18).fontWeight(FontWeight.Bold);
        Text('A sample for battery common-event subscription + memory check.')
          .opacity(0.75);
        Text(`Build time: ${TimeUtil.formatTime(Date.now())}`).opacity(0.6);
        Text('Tips: Use Settings page to change record mode.').opacity(0.75);
      }
      .padding(16)
      .backgroundColor(0x08000000)
      .borderRadius(12)
      .margin({ left: 16, right: 16 });
    }
    .height('100%');
  }
}
