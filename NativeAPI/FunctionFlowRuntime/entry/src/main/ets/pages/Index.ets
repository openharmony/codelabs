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
            let ret1 = fun.FfrtSerialQueue();
            let ret2 = fun.FfrtConcurrentQueue();
            let ret3 = libaki.add(3, 5);
            let ret4 = libaki.ProcessFfrtQueue();
            let ret5 = libaki.ComputeFfrtQueue();

            sleep(10000);
            this.message = ret3.toString();
            console.info("FFRT_FUNCTION FfrtSerialQueue ret: ", ret1);
            console.info("FFRT_FUNCTION FfrtConcurrentQueue ret: ", ret2);
            console.info("FFRT_FUNCTION ProcessFfrtQueue ret: ", ret4);
            console.info("FFRT_FUNCTION ComputeFfrtQueue ret: ", ret5);
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
