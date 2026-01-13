import { RecordStatus } from '../model/Enums';

@ComponentV2
export struct RecorderControls {
  @Param status: RecordStatus = RecordStatus.Idle;
  @Event onRecord?: () => void;
  @Event onPause?: () => void;
  @Event onStop?: () => void;

  private readonly primaryColor: string = '#1f6f6d';
  private readonly secondaryColor: string = '#e7dccb';
  private readonly dangerColor: string = '#ee6c4d';

  build() {
    Row({ space: 12 }) {
      Button(this.status === RecordStatus.Recording ? 'Pause' : (this.status === RecordStatus.Paused ? 'Resume' : 'Record'))
        .width(140)
        .height(44)
        .fontColor('#ffffff')
        .backgroundColor(this.status === RecordStatus.Recording ? this.secondaryColor : this.primaryColor)
        .borderRadius(16)
        .onClick(() => {
          if (this.status === RecordStatus.Idle) {
            this.onRecord && this.onRecord();
            return;
          }
          this.onPause && this.onPause();
        });

      Button('Stop')
        .width(100)
        .height(44)
        .fontColor('#ffffff')
        .backgroundColor(this.dangerColor)
        .borderRadius(16)
        .enabled(this.status !== RecordStatus.Idle)
        .onClick(() => {
          this.onStop && this.onStop();
        });
    }
    .padding({ top: 12, bottom: 12 })
  }
}
