import router from '@ohos.router';
import { CourseDataService } from '../service/CourseDataService';

@Entry
@Component
struct SplashPage {
  private courseService: CourseDataService = CourseDataService.getInstance();
  @State sloganOpacity: number = 0;
  @State logoScale: number = 0.8;
  @State logoOpacity: number = 0;

  async aboutToAppear() {
    // 1. Initialize Service
    this.courseService.init(getContext(this)).catch((err: Error) => {
      console.error('Failed to init service:', err);
    });

    // 2. Animations
    animateTo({ duration: 800, curve: Curve.EaseOut }, () => {
      this.logoScale = 1;
      this.logoOpacity = 1;
    });

    animateTo({ duration: 800, delay: 300, curve: Curve.EaseOut }, () => {
      this.sloganOpacity = 1;
    });

    // 3. Check Login Status after delay
    setTimeout(() => {
      this.checkLoginAndJump();
    }, 2000); // 2 seconds splash duration
  }

  async checkLoginAndJump() {
    try {
      const isLoggedIn = await this.courseService.isLoggedIn();
      if (isLoggedIn) {
        // User is logged in -> Go to Home
        router.replaceUrl({ url: 'pages/Index' })
          .catch((err: Error) => {
             console.error('Splash navigation to Index failed:', JSON.stringify(err));
             // If Index fails, maybe try Login?
             router.replaceUrl({ url: 'pages/LoginPage' }); 
          });
      } else {
        // User is NOT logged in -> Go to Login Page
        router.replaceUrl({ url: 'pages/LoginPage' })
          .catch((err: Error) => {
             console.error('Splash navigation to LoginPage failed:', JSON.stringify(err));
          });
      }
    } catch (error) {
      console.error('Check login status failed:', error);
      // Fallback to Login Page on error
      router.replaceUrl({ url: 'pages/LoginPage' });
    }
  }

  build() {
    Stack() {
      // Background Elements
      Circle({ width: 240, height: 240 })
        .fill('rgba(0, 125, 255, 0.20)')
        .position({ x: -60, y: -60 })
      
      Circle({ width: 180, height: 180 })
        .fill('rgba(0, 125, 255, 0.15)')
        .position({ x: '85%', y: '65%' })

      Column() {
        // Logo & Title
        Column() {
          Image($r('app.media.startIcon'))
            .width(110)
            .height(110)
            .borderRadius(22)
            .shadow({ radius: 12, color: 'rgba(0, 125, 255, 0.15)', offsetY: 6 })
            .scale({ x: this.logoScale, y: this.logoScale })
            .opacity(this.logoOpacity)
            .margin({ bottom: 24 })
          
          Text('课程表助手')
            .fontSize(30)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 12 })
            .opacity(this.logoOpacity)
            
          Text('让学习更有序，生活更简单')
            .fontSize(16)
            .fontColor('#999999')
            .opacity(this.sloganOpacity)
            .letterSpacing(2)
        }
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        
        // Footer Copyright
        Text('Copyright © 2026 BIT Course Helper')
          .fontSize(12)
          .fontColor('#CCCCCC')
          .margin({ bottom: 32 })
      }
      .width('100%')
      .height('100%')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }
}
