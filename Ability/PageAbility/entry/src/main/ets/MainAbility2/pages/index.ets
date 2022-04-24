import featureAbility from '@ohos.ability.featureAbility';

@Entry
@Component
struct Index {
    onBackPress() {
    //启动ability
    let str = {
      "want": {
        "bundleName": "com.example.pageabilityopenh",
        "abilityName": "com.example.pageabilityopenh.MainAbility",
      },
    };
    featureAbility.startAbility(str)
      .then((data) => {
        console.info('Operation successful. Data: ' + JSON.stringify(data))
      }).catch((error) => {
      console.error('Operation failed. Cause: ' + JSON.stringify(error));
    })
    return true;
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Button() {
        Text('This is ability2 index page')
          .fontSize(25)
          .fontWeight(FontWeight.Medium)
          .margin({left:20,right:20})
          .fontColor(Color.White)
      }
      .padding({ top: 20, left: 20, right: 20, bottom: 20 })
    }
    .width('100%')
    .height('100%')
  }
}