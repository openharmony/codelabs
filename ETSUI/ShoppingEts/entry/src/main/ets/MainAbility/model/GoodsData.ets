let NextId = 0;
export class GoodsData {
  id: string;
  title: string;
  content: string;
  price: number;
  imgSrc: Resource;

  constructor(title: string, content: string, price: number, imgSrc: Resource) {
    this.id = `${NextId++}`;
    this.title = title;
    this.content = content;
    this.price = price;
    this.imgSrc = imgSrc;
  }
}

export class IconImage {
  id: string;
  imgSrc: Resource;

  constructor(imgSrc: Resource) {
    this.id = `${NextId++}`;
    this.imgSrc = imgSrc;
  }
}