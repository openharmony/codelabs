import { hilog } from '@kit.PerformanceAnalysisKit';
import { FunctionClass } from 'libentry.so';
import libaki from 'libentry.so';
const DOMAIN = 0x0000;

export const sleep = (delay: number = 200) => new Promise<void>((resolve) => setTimeout(resolve, delay));

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
            fun.FfrtSerialQueue();
            fun.FfrtConcurrentQueue();
            let b = libaki.add(3, 5);
            libaki.ProcessFfrtQueue();
            libaki.ComputeFfrtQueue();
            sleep(10000);
            this.message = b.toString();
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
