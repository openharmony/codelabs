
@Component
export default struct notify {

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Button()
        .onClick(() => {
          AlertDialog.show(
            {
              message: "电量提示",
              offset: { dx: 0, dy: -10 },
              alignment: DialogAlignment.Bottom,
              confirm: {
                value: "OK",
                action: () => {
                  console.info('Button clicking callback');
                }
              },
              cancel: () => {
                console.info('Closed callbacks');
              }
            }
          );
        })
        .height('5%')
        .width('14%')
    }
  }
}