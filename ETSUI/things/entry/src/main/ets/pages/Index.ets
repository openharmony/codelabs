@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  @State message2: string = 'temp2';
  @State count: number = 0;

  build() {
    RelativeContainer() {
      Text(this.message)
        .id('HelloWorld')
        .fontSize($r('app.float.page_text_font_size'))
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .onClick(() => {
          if (this.count > 0) {
            this.message = this.message2;
          } else {
            this.message = 'Welcome';
            this.count++;
          }
        })

      Button('测试1')
        .onClick(()=>{
          this.message='测试1'
        })
    }

    .height('100%')
    .width('100%')
  }

}