import router from '@ohos.router';
import type common from '@ohos.app.ability.common';
import { BatteryEventRepository } from '../repository/BatteryEventRepository';
import { KeyValueRow } from '../components/KeyValueRow';
import { JsonViewer } from '../components/JsonViewer';
import { BatteryFormat } from '../utils/BatteryFormat';
import { TimeUtil } from '../utils/TimeUtil';
import type { BatteryEvent } from '../model/BatteryEvent';

export interface DetailParams {
  eventId?: string;
}

@Entry
@Component
export struct Detail {
  @State event: BatteryEvent | null = null;

  aboutToAppear() {
    const params = router.getParams() as Object as DetailParams;
    const id = params && params.eventId ? params.eventId : '';

    const ctx = getContext(this) as common.UIAbilityContext;
    BatteryEventRepository.inst.ensureInit(ctx).then(() => {
      const all = BatteryEventRepository.inst.getHistory();
      for (let i = 0; i < all.length; i++) {
        if (all[i].id === id) {
          this.event = all[i];
          break;
        }
      }
    }).catch(() => {
      const all = BatteryEventRepository.inst.getHistory();
      for (let i = 0; i < all.length; i++) {
        if (all[i].id === id) {
          this.event = all[i];
          break;
        }
      }
    });
  }

  private back(): void {
    router.back();
  }

  build() {
    Column() {
      Row() {
        Button('← Back').onClick(() => this.back());
        Text('Detail')
          .fontSize(18)
          .fontWeight(FontWeight.Medium)
          .margin({ left: 12 });
        Blank();
      }.padding({ left: 16, right: 16, top: 16, bottom: 10 });

      if (!this.event) {
        Column() { Text('No event found.').opacity(0.7); }
          .padding(16);
      } else {
        Text(TimeUtil.formatTime(this.event!.timeMs))
          .fontSize(14)
          .opacity(0.75)
          .padding({ left: 16, right: 16 });

        Column({ space: 8 }) {
          KeyValueRow({ k: 'SOC', v: `${this.event!.snapshot.soc}%` });
          KeyValueRow({ k: 'Charge', v: BatteryFormat.chargeText(this.event!.snapshot.chargeState) });
          KeyValueRow({ k: 'Plug', v: BatteryFormat.pluggedText(this.event!.snapshot.pluggedType) });
          KeyValueRow({ k: 'Temp', v: BatteryFormat.tempText(this.event!.snapshot.temperatureC) });
          KeyValueRow({ k: 'Volt', v: BatteryFormat.voltText(this.event!.snapshot.voltageV) });
          KeyValueRow({ k: 'Health', v: BatteryFormat.healthText(this.event!.snapshot.healthState) });
          KeyValueRow({ k: 'Present', v: this.event!.snapshot.present ? 'true' : 'false' });
          KeyValueRow({ k: 'Tech', v: this.event!.snapshot.technology || '--' });
        }
        .padding({ left: 16, right: 16 })
        .backgroundColor(0x08000000)
        .borderRadius(12)
        .margin({ left: 16, right: 16 });

        JsonViewer({ obj: this.event!.raw }).padding({ left: 16, right: 16 });
      }
    }
    .height('100%');
  }

}
