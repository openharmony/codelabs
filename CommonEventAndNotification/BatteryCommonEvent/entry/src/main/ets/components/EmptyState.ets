
@Component
export struct EmptyState {
  title: string = 'No data';
  desc: string = '';
  actionText: string = '';
  onAction?: () => void;

  build() {
    Column({ space: 10 }) {
      Text(this.title).fontSize(18).fontWeight(FontWeight.Bold);
      if (this.desc) {
        Text(this.desc).fontSize(14).opacity(0.75)
      }
      if (this.actionText) {
        Button(this.actionText)
          .onClick(() => { this.onAction?.(); });
      }
    }
    .padding(24)
    .alignItems(HorizontalAlign.Center)
    .width('100%');
  }
}
