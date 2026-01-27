import { SportType } from '../model/SportModel';

@Component
export struct SportTypeSelector {
  @Prop selectedType: SportType;
  onSelect: (type: SportType) => void = () => {};

  build() {
    Row() {
      this.TypeButton(SportType.RUNNING, '跑步', '🏃')
      this.TypeButton(SportType.WALKING, '行走', '🚶')
      this.TypeButton(SportType.CYCLING, '骑行', '🚴')
    }
    .width('100%')
    .justifyContent(FlexAlign.SpaceEvenly)
  }

  @Builder
  TypeButton(type: SportType, label: string, icon: string) {
    Column() {
      Text(icon)
        .fontSize(32)
        .margin({ bottom: 8 })
      Text(label)
        .fontSize(14)
        .fontColor(this.selectedType === type ? '#4ECDC4' : '#999999')
    }
    .padding({ top: 16, bottom: 16, left: 24, right: 24 })
    .backgroundColor(this.selectedType === type ? '#4ECDC420' : '#FFFFFF')
    .borderRadius(12)
    .border(this.selectedType === type ? { width: 2, color: '#4ECDC4' } : { width: 1, color: '#E0E0E0' })
    .onClick(() => {
      this.onSelect(type);
    })
  }
}
