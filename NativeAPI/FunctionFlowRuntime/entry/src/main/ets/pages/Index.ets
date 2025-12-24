import { hilog } from '@kit.PerformanceAnalysisKit';
import { FunctionClass } from 'libentry.so';
const DOMAIN = 0x0000;

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize($r('app.float.page_text_font_size'))
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = 'FFRT';
            let fun = new FunctionClass(5);
            fun.FfrtQueue();
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
