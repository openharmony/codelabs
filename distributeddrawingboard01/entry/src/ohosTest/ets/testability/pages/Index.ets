/*
 * Copyright (c) 2025 xingxin. All rights reserved.
 * 文件名: example.cpp
 * 作者:邢欣
 * 版本: 1.0
 * 日期: 2025-11-03
 */
@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
    }
    .height('100%')
  }
}