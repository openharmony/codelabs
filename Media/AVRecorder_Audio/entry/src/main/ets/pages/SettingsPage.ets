import router from '@ohos.router';
import { TopBar } from '../components/TopBar';
import { settingsStore } from '../store/SettingsStore';
import type { RecorderConfig } from '../model/RecorderConfig';
import { validateRecorderConfig } from '../utils/Validators';
import { appState } from '../store/AppState';

const SAMPLE_RATES: number[] = [8000, 16000, 44100, 48000];
const CHANNELS: number[] = [1, 2];
const BIT_RATES: number[] = [64000, 96000, 128000, 192000];
const FORMATS: string[] = ['m4a', 'aac'];

function makeOption(label: string): SelectOption {
  return { value: label };
}

const SAMPLE_RATE_LABELS: string[] = ['8 kHz', '16 kHz', '44.1 kHz', '48 kHz'];
const CHANNEL_LABELS: string[] = ['Mono', 'Stereo'];
const BIT_RATE_LABELS: string[] = ['64 kbps', '96 kbps', '128 kbps', '192 kbps'];
const FORMAT_LABELS: string[] = ['M4A (AAC)', 'AAC'];

const SAMPLE_RATE_OPTIONS: SelectOption[] = SAMPLE_RATE_LABELS.map(label => makeOption(label));
const CHANNEL_OPTIONS: SelectOption[] = CHANNEL_LABELS.map(label => makeOption(label));
const BIT_RATE_OPTIONS: SelectOption[] = BIT_RATE_LABELS.map(label => makeOption(label));
const FORMAT_OPTIONS: SelectOption[] = FORMAT_LABELS.map(label => makeOption(label));

@Entry
@Component
struct SettingsPage {
  @State config: RecorderConfig = settingsStore.getConfig();

  @Builder
  private buildTrailing() {
    Button('Back').onClick(() => router.back());
  }

  aboutToAppear() {
    settingsStore.subscribe((config) => {
      this.config = config;
    });
  }

  private getIndex(values: number[], current: number): number {
    const index = values.indexOf(current);
    return index >= 0 ? index : 0;
  }

  private getFormatIndex(values: string[], current: string): number {
    const index = values.indexOf(current);
    return index >= 0 ? index : 0;
  }

  private getLabel(labels: string[], index: number): string {
    if (index < 0 || index >= labels.length) {
      return labels[0];
    }
    return labels[index];
  }

  private saveSettings(): void {
    const config = settingsStore.getConfig();
    if (!validateRecorderConfig(config)) {
      appState.showToast('Invalid config');
      return;
    }
    appState.showToast('Settings saved');
    router.back();
  }

  build() {
    Column() {
      TopBar({ title: 'Settings', trailing: this.buildTrailing });

      Column({ space: 16 }) {
        Text('Recorder Configuration')
          .fontSize(18)
          .fontWeight(FontWeight.Medium);

        Column({ space: 12 }) {
          Column() {
            Row() {
              Text('Sample Rate (Hz)')
                .fontSize(14)
                .fontColor('#333333');
              Blank();
              Select(SAMPLE_RATE_OPTIONS)
                .selected(this.getIndex(SAMPLE_RATES, this.config.sampleRate))
                .value(this.getLabel(SAMPLE_RATE_LABELS, this.getIndex(SAMPLE_RATES, this.config.sampleRate)))
                .width(160)
                .onSelect((index: number) => {
                  settingsStore.updateSampleRate(SAMPLE_RATES[index]);
                });
            }
            .width('100%')
          }
          .padding(16)
          .width('100%')
          .backgroundColor('#fff8f0')
          .borderRadius(14)
          .border({ width: 1, color: '#e7dccb' })
          .shadow({ radius: 6, color: '#16000000', offsetX: 0, offsetY: 3 });

          Column() {
            Row() {
              Text('Channels')
                .fontSize(14)
                .fontColor('#333333');
              Blank();
              Select(CHANNEL_OPTIONS)
                .selected(this.getIndex(CHANNELS, this.config.channels))
                .value(this.getLabel(CHANNEL_LABELS, this.getIndex(CHANNELS, this.config.channels)))
                .width(160)
                .onSelect((index: number) => {
                  settingsStore.updateChannels(CHANNELS[index]);
                });
            }
            .width('100%')
          }
          .padding(16)
          .width('100%')
          .backgroundColor('#fff8f0')
          .borderRadius(14)
          .border({ width: 1, color: '#e7dccb' })
          .shadow({ radius: 6, color: '#16000000', offsetX: 0, offsetY: 3 });

          Column() {
            Row() {
              Text('Bit Rate')
                .fontSize(14)
                .fontColor('#333333');
              Blank();
              Select(BIT_RATE_OPTIONS)
                .selected(this.getIndex(BIT_RATES, this.config.bitRate))
                .value(this.getLabel(BIT_RATE_LABELS, this.getIndex(BIT_RATES, this.config.bitRate)))
                .width(160)
                .onSelect((index: number) => {
                  settingsStore.updateBitRate(BIT_RATES[index]);
                });
            }
            .width('100%')
          }
          .padding(16)
          .width('100%')
          .backgroundColor('#fff8f0')
          .borderRadius(14)
          .border({ width: 1, color: '#e7dccb' })
          .shadow({ radius: 6, color: '#16000000', offsetX: 0, offsetY: 3 });

          Column() {
            Row() {
              Text('Format')
                .fontSize(14)
                .fontColor('#333333');
              Blank();
              Select(FORMAT_OPTIONS)
                .selected(this.getFormatIndex(FORMATS, this.config.format))
                .value(this.getLabel(FORMAT_LABELS, this.getFormatIndex(FORMATS, this.config.format)))
                .width(160)
                .onSelect((index: number) => {
                  settingsStore.updateFormat(FORMATS[index]);
                });
            }
            .width('100%')
          }
          .padding(16)
          .width('100%')
          .backgroundColor('#fff8f0')
          .borderRadius(14)
          .border({ width: 1, color: '#e7dccb' })
          .shadow({ radius: 6, color: '#16000000', offsetX: 0, offsetY: 3 });
        }
        .width('100%')

        Button('Save')
          .width('100%')
          .margin({ top: 12 })
          .onClick(() => this.saveSettings());
      }
      .padding(20)
    }
    .height('100%')
    .width('100%')
    .backgroundColor('#ffffff');
  }
}
