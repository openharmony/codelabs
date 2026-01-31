/*
 * Copyright (c) 2025 YourCompany. All rights reserved.
 * 文件名: example.cpp
 * 作者:邢欣
 * 版本: 1.0
 * 日期: 2025-11-03
 */
// 绘制位置信息类

export default class Position {
  /**
   * 是否起始点
   */
  isFirstPosition: boolean;

  /**
   * 是否结束点
   */
  isEndPosition: boolean;

  /**
   * x轴
   */
  positionX: number;

  /**
   * y轴
   */
  positionY: number;

  constructor(isFirstPosition: boolean, isEndPosition: boolean, positionX: number, positionY: number) {
    this.isFirstPosition = isFirstPosition;
    this.isEndPosition = isEndPosition;
    this.positionX = positionX;
    this.positionY = positionY;
  }
}