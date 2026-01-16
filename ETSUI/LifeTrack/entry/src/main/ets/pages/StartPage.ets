// 跳转页面入口函数
@Builder
export function StartBuilder() {
  Start();
}

@Component
struct Start {
  pathStack: NavPathStack = new NavPathStack();
  aboutToAppear(): void {
    setTimeout(() => {
      this.pathStack.replacePathByName("Dashboard", null, false)
    }, 1000)
  }

  build() {
    NavDestination() {

    }
    // .title('开始页')
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })

  }
}