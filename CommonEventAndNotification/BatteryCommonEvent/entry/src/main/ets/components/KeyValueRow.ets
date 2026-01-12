
@Component
export struct KeyValueRow {
  k: string = '';
  v: string = '';
  hint: string = '';

  build() {
    Row() {
      Column({ space: 2 }) {
        Text(this.k).fontSize(14).fontWeight(FontWeight.Medium);
        if (this.hint) {
          Text(this.hint).fontSize(12).opacity(0.65);
        }
      }.layoutWeight(1);

      Text(this.v).fontSize(14).fontWeight(FontWeight.Medium);
    }
    .padding({ top: 10, bottom: 10 })
    .border({ width: { bottom: 1 }, color: 0x14000000 });
  }
}
