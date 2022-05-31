import TitleBar from '../view/TitleBar'
import net_socket from '@ohos.net.socket';
import logger from '../model/Logger'

@Entry
@Component
struct back {
  @State isConnect: boolean = false;
  @State isLock: boolean = true;
  isListen: boolean = false;
  connectStr: string = '连接tcp'
  disConnectStr: string = '断开tcp'
  sendOpenStr: string = '发送开门指令'
  sendCloseStr: string = '发送关门指令'
  tcp: net_socket.TCPSocket = null;
  promise: Promise<void> = null;

  private AutoFlush() {
    setInterval(function () {
      this.Accept();
    }, 100);
  }

  private SockOnMessage(data) {
    let buffer = data.message
    let dataView = new DataView(buffer)
    let str = ""
    for (let i = 0;i < dataView.byteLength; ++i) {
      str += String.fromCharCode(dataView.getUint8(i))
    }
    logger.log(str)
    if (str.includes('open')) {
      this.isLock = false;
    } else if (str.includes('close')) {
      this.isLock = true;
    }
  }

  private Accept() {
    this.tcp.on('message', buffer => {
      this.SockOnMessage(buffer);
    });
  }

  private Connect() {
    this.tcp = net_socket.constructTCPSocketInstance()
    this.tcp.bind({ address: '0.0.0.0', port: 8554, family: 1 }, err => {
      if (err) {
        logger.log('bind fail');
        return;
      }
      logger.log('bind success');
    })
    this.promise = this.tcp.connect({ address: { address: '192.168.1.2', port: 8556, family: 1 }, timeout: 6000 });
    this.promise.then(() => {
      logger.log('connect success')
      this.isConnect = true;
      this.AutoFlush();
    }).catch(err => {
      logger.log(`connect fail : ${err}`)
      this.isConnect = false
    });
  }

  private DisConnect(){
    this.tcp.close(err => {
      if (err) {
        logger.log('DisConnect fail');
        return;
      }
      logger.log('DisConnect success');
      this.isConnect = false
    })
  }

  private Send(order: string) {
    this.tcp.send({
      data: order
    }, err => {
      if (err) {
        logger.log('send fail');
        return;
      }
      logger.log('send success');
    })
  }

  build() {
    Column() {
      TitleBar()
      Column() {
        if (this.isConnect) {
          if (this.isLock) {
            Image($r('app.media.lock'))
              .objectFit(ImageFit.Contain)
              .width('80%')
              .height('50%')
          } else {
            Image($r('app.media.unlock'))
              .objectFit(ImageFit.Contain)
              .width('80%')
              .height('50%')
          }
          Button(this.sendOpenStr, { type: ButtonType.Normal, stateEffect: true })
            .borderRadius(8)
            .backgroundColor(0x317aff)
            .height('10%')
            .width('80%')
            .margin('1%')
            .onClick(() => {
              logger.log("open click");
              this.Send('open');
            })
          Button(this.sendCloseStr, { type: ButtonType.Normal, stateEffect: true })
            .borderRadius(8)
            .backgroundColor(0x317aff)
            .height('10%')
            .width('80%')
            .margin('1%')
            .onClick(() => {
              logger.log("close click");
              this.Send('close');
            })
          Button(this.disConnectStr, { type: ButtonType.Normal, stateEffect: true })
            .borderRadius(8)
            .backgroundColor(0x317aff)
            .height('10%')
            .width('80%')
            .margin('1%')
            .onClick(() => {
              logger.log("disConnectStr click");
              this.DisConnect();
            })
        } else {
          Button(this.connectStr, { type: ButtonType.Normal, stateEffect: true })
            .borderRadius(8)
            .backgroundColor(0x317aff)
            .height('10%')
            .width('80%')
            .margin('1%')
            .onClick(() => {
              logger.log("connect click");
              this.Connect();
              this.Accept();
            })
        }
      }.width('100%').height('90%').padding({ top: '1%', bottom: '1%' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.Grey)
  }
}