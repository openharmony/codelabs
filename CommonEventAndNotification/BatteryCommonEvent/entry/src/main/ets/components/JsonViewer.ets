import { JsonUtil } from '../utils/JsonUtil';

interface PlainObject {}

@Component
export struct JsonViewer {
  obj: PlainObject = {};
  maxLines: number = 40;
  expanded: boolean = false;

  private getText(): string {
    const text = JsonUtil.safeStringify(this.obj as Object, 2);
    if (this.expanded) {return text};
    const lines = text.split('\n');
    if (lines.length <= this.maxLines) {return text};
    return lines.slice(0, this.maxLines).join('\n') + '\n...';
  }

  build() {
    Column({ space: 8 }) {
      Row() {
        Text('Raw JSON').fontSize(14).fontWeight(FontWeight.Medium).layoutWeight(1);
        Text(this.expanded ? 'Collapse' : 'Expand')
          .fontSize(12)
          .opacity(0.75)
          .onClick(() => { this.expanded = !this.expanded; });
      }
      Scroll() {
        Text(this.getText())
          .fontSize(12)
          .fontFamily('monospace')
          .lineHeight(18);
      }
      .height(this.expanded ? 360 : 220)
      .backgroundColor(0x0F000000)
      .borderRadius(10)
      .padding(12);
    }
  }
}
