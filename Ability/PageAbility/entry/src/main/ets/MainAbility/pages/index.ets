import router from '@system.router';
import featureAbility from '@ohos.ability.featureAbility';
import app from '@system.app'

@Entry
@Component
struct Index {
  onBackPress() {
    app.terminate();
    return true;
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('This is ability1 index page')
        .fontSize(25)
        .fontWeight(FontWeight.Medium)
      Button() {
        Text('Click to ability1 second page')
          .fontSize(25)
          .fontWeight(FontWeight.Medium)
          .margin({ left: 20, right: 20 })
          .fontColor(Color.White)
      }
      .padding({ top: 20, left: 20, right: 20, bottom: 20 })
      .margin({ top: 20 })
      .onClick(() => {
        router.push({ uri: 'pages/second' })
      })

      Button() {
        Text('Click to ability2 index page')
          .fontSize(25)
          .margin({ left: 20, right: 20 })
          .fontColor(Color.White)
      }
      .padding({ top: 20, left: 20, right: 20, bottom: 20 })
      .margin({ top: 20 })
      .onClick(() => {
        //启动ability
        let str = {
          "want": {
            "bundleName": "com.example.pageabilityopenh",
            "abilityName": "com.example.pageabilityopenh.MainAbility2",
          },
        };
        featureAbility.startAbility(str)
          .then((data) => {
            console.info('Operation successful. Data: ' + JSON.stringify(data))
          }).catch((error) => {
          console.error('Operation failed. Cause: ' + JSON.stringify(error));
        })
      })

      Button() {
        Text('Click to ability2 second page')
          .fontSize(25)
          .margin({ left: 20, right: 20 })
          .fontColor(Color.White)
      }
      .padding({ top: 20, left: 20, right: 20, bottom: 20 })
      .margin({ top: 20 })
      .onClick(() => {
        //启动ability
        let str = {
          "want": {
            "bundleName": "com.example.pageabilityopenh",
            "abilityName": "com.example.pageabilityopenh.MainAbility2",
            "parameters": {
              url: 'pages/second'
            }
          },
        };
        featureAbility.startAbility(str)
          .then((data) => {
            console.info('Operation successful. Data: ' + JSON.stringify(data))
          }).catch((error) => {
          console.error('Operation failed. Cause: ' + JSON.stringify(error));
        })
      })
    }
    .width('100%')
    .height('100%')
  }
}