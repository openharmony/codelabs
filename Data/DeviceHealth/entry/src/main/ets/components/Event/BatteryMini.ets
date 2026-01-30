
@Component
export default struct BatteryMiniWidget {
  @Prop batteryLevel: number = 0;

  build() {
    Row() {
      Stack({ alignContent: Alignment.Start }) {
        Rect().width(35).height(16).fill(Color.Transparent).stroke('#CCCCCC').strokeWidth(1.5).radius(3)
        Rect()
          .width(this.batteryLevel > 0 ? (this.batteryLevel / 100) * 31 : 0)
          .height(12)
          .fill(this.batteryLevel <= 20 ? '#FF4500' : '#32CD32')
          .margin({ left: 2 })
          .radius(1)
          .animation({ duration: 500, curve: Curve.EaseInOut })
      }
      Text(`${this.batteryLevel}%`).fontSize(13).margin({ left: 6 }).fontColor('#999999')
    }
  }
}