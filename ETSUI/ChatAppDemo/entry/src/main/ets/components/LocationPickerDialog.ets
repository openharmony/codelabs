import { WORLD_DATA, Province, City } from '../utils/LocationData'

// 顶部定义类型
type ConfirmCallback = (province: string, city: string) => void

@CustomDialog
export struct LocationPickerDialog {
  controller: CustomDialogController
  // 选中的结果回调给父组件
  confirm: ConfirmCallback = (p, c) => {
    console.info(`Default callback: ${p}, ${c}`)
  }
  @State selectedProvinceIndex: number = 0
  @State selectedCityIndex: number = 0

  // 数据源
  private provinces: Province[] = WORLD_DATA["中国"]
  @State currentCities: City[] = WORLD_DATA["中国"][0].cities

  build() {
    Column() {
      // 头部操作栏
      Row() {
        Text($r('app.string.btn_cancel'))
          .fontSize(16)
          .fontColor(Color.Grey)
          .onClick(() => {
            this.controller.close()
          })

        Text($r('app.string.title_select_region'))
          .fontSize(18)
          .fontWeight(FontWeight.Medium)

        Text($r('app.string.btn_confirm'))
          .fontSize(16)
          .fontColor(Color.Blue)
          .onClick(() => {
            // 增加校验逻辑：确保索引在合法范围内
            if (this.selectedProvinceIndex >= 0 && this.selectedProvinceIndex < this.provinces.length) {
              const pName = this.provinces[this.selectedProvinceIndex].name

              if (this.currentCities && this.currentCities.length > this.selectedCityIndex) {
                const cName = this.currentCities[this.selectedCityIndex].name

                // 打印日志
                console.info(`User selected location: ${pName} - ${cName}`)

                // 执行回调
                this.confirm(pName, cName)
                this.controller.close()
              } else {
                console.warn('City index out of bounds')
              }
            }
          })
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .padding(20)

      // 联动选择器
      Row() {
        // 第一列：省份
        TextPicker({ range: this.provinces.map(p => p.name), selected: this.selectedProvinceIndex })
          .layoutWeight(1)
          .onChange((value: string | string[], index: number | number[]) => {
            // 类型收窄，确保取到数字索引
            const idx = Array.isArray(index) ? index[0] : index
            this.selectedProvinceIndex = idx
            // 联动核心：更新第二列的城市数组
            this.currentCities = this.provinces[idx].cities
            this.selectedCityIndex = 0 // 重置城市索引
          })

        // 第二列：城市
        TextPicker({ range: this.currentCities.map(c => c.name), selected: this.selectedCityIndex })
          .layoutWeight(1)
          .onChange((value: string | string[], index: number | number[]) => {
            const idx = Array.isArray(index) ? index[0] : index
            this.selectedCityIndex = idx
          })
      }
      .width('100%')
      .height(250)
    }
    .backgroundColor(Color.White)
    .borderRadius({ topLeft: 20, topRight: 20 })
  }
}