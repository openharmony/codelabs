/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// 奖品名数组
let arr = ["手环", "耳机", "汽车", "电脑", "平板", "手机", "路由", "手表"];
// 奖品图片数组
let arrImg = ["./img/1-band.jpg", "./img/2-buds.jpg", "./img/3-car.jpg",
    "./img/4-computer.png", "./img/5-pad.jpg", "./img/6-phone.jpg",
    "./img/7-router.jpg", "./img/8-watch.png"];
// 获取奖品列表
let prizes = document.querySelector('.prizes');
// 获取奖品区名称
let prizeText = document.querySelector('#prizeText');
// 获取全部奖品单元格
let ali = document.querySelectorAll('.prizes-li');
// 获取弹窗
let dialog = document.querySelector('.dialog');
// 获取关闭弹窗span
let close = document.querySelector('.close');
// 获取提交按钮
let btn = document.querySelector('.btn');
// 获取弹窗文本
let dialogText = document.querySelector('.dialogText');
// 获取弹窗图片
let dialogImg = document.querySelector('.dialogImg');
// 获取奖品img
let pic = document.querySelectorAll('.pic');
// 抽奖结束文字
let finishText = document.querySelector('.finishText');
// 信息输入框
let infoInput = document.querySelectorAll('.infoInput');

let i = 0; //转到哪个位置
let count = 0; //转圈初始值
let totalCount = 8; //至少转动的总圈数
let speed = 500; //转圈速度，值越大越慢
let initSpeed = 500;
let timer;
let isClick = true;
let index = 3; //指定转到哪个奖品

// 绑定img
for (let j = 0;j < pic.length; j++) {
    pic[j].src = arrImg[j];
}

// 旋转函数，预先随机好结果，通过可变速的旋转动作，到达指定的圈数后，最后落在指定位置
function roll() {
    //速度衰减
    speed -= 50;
    if (speed <= 10) {
        speed = 10;
    }
    //每次调用都去掉全部active类名
    for (let j = 0; j < ali.length; j++) {
        ali[j].classList.remove('active');
    }
    i++;
    //计算转圈次数
    if (i >= ali.length - 1) {
        i = 0;
        count++;
    }

    prizeText.innerHTML = arr[i]; //显示当前奖品名称

    ali[i].classList.add('active'); //添加激活类名，给奖品加样式
    //满足转圈数和指定位置就停止
    if (count >= totalCount && (i + 1) == index) {
        clearTimeout(timer);
        isClick = true;
        speed = initSpeed;
        dialogText.innerHTML = "恭喜获得 " + arr[i] + " 请填写收件地址";
        dialogImg.src = arrImg[i];
        timer = setTimeout(openDialog, 1000); // 等待1s打开弹窗
    } else {
        timer = setTimeout(roll, speed); //不满足条件时调用定时器
        //最后一圈减速
        if (count >= totalCount - 1 || speed <= 50) {
            speed += 100;
        }
    }
}

// 抽奖开始函数
function start() {
    finishText.style.display = "none";
    // 防止抽奖多次触发
    if (isClick) {
        count = 0;
        //随机产生中奖位置
        index = Math.floor(Math.random() * arr.length + 1);
        roll();
        isClick = false;
    }
}

// 打开弹窗
function openDialog() {
    dialog.style.display = "block";
    // 清楚输入框内容
    for (var i = 0;i < infoInput.length; i++) {
        infoInput[i].value  = "";
    }
}

// 关闭弹窗
function closeDialog() {
    dialog.style.display = "none";
}

// 点击 <span> (x), 关闭弹窗
close.onclick = function () {
    closeDialog();
}

// 在用户点击其他地方时，关闭弹窗
window.onclick = function (event) {
    if (event.target == dialog) {
        closeDialog();
    }
}
// 提交按钮
btn.onclick = function () {
    closeDialog();
    finishText.style.display = "block";
}

// 结束语

