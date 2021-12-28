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

import prompt from '@system.prompt';

export default {
    data: {
        title: "",
        image00: "",
        pictureList: [],
        imageIndexForPosition: []
    },
    onInit() {
        this.onRandom();
    },
    onChange(index) {
        let menu = {
            "1": [2, 4],
            "2": [1, 3, 5],
            "3": [2, 6],
            "4": [1, 5, 7],
            "5": [2, 4, 6, 8],
            "6": [3, 5, 9],
            "7": [4, 8],
            "8": [5, 7, 9],
            "9": [6, 8]
        }
        let clickImage = index + 1;
        let invisibleImage = this.imageIndexForPosition.indexOf(9) + 1;
        let arr = menu[invisibleImage];
        if (!arr.includes(clickImage)) {
            prompt.showToast({
                message: "不相邻"
            });
        } else {
            let temp = this.imageIndexForPosition[invisibleImage - 1];
            this.imageIndexForPosition[invisibleImage - 1] = this.imageIndexForPosition[clickImage - 1];
            this.imageIndexForPosition[clickImage - 1] = temp;

            this.pictureList = [];
            this.imageIndexForPosition.forEach(value => {
                if (value == 9) {
                    this.pictureList.push("--")
                } else {
                    this.pictureList.push("/common/images/picture_0" + value + ".png")
                }
            });
        }
        this.onFinish();
    },
    onFinish() {
        let finalList = [1, 2, 3, 4, 5, 6, 7, 8, 9];
        if (this.equarList(this.imageIndexForPosition, finalList)) {

            this.pictureList = [];
            this.imageIndexForPosition.forEach(value => {
                this.pictureList.push("/common/images/picture_0" + value + ".png")
            });
            prompt.showToast({
                message: "完成拼接"
            });
        }
    },
    equarList(a, b) {
        // 判断数组的长度
        if (a.length !== b.length) {
            return false
        } else {
            // 循环遍历数组的值进行比较
            for (let i = 0; i < a.length; i++) {
                if (a[i] !== b[i]) {
                    return false
                }
            }
            return true;
        }
    },
    onRandom() {
        this.setupRandomPosition();
        this.pictureList = []
        this.imageIndexForPosition.forEach(value => {
            if (value == 9) {
                this.pictureList.push("--")
            } else {
                this.pictureList.push("/common/images/picture_0" + value + ".png")
            }
        });
    },
    setupRandomPosition() {
        let list1 = [5, 4, 3, 9, 1, 8, 6, 7, 2];
        let list2 = [3, 1, 6, 7, 9, 8, 4, 2, 5];
        let list3 = [4, 8, 3, 5, 2, 7, 9, 1, 6];
        let list4 = [4, 3, 5, 2, 8, 7, 6, 1, 9];
        let lists = [list1, list2, list3, list4];
        this.imageIndexForPosition = lists[parseInt(Math.random() * 4)];
    }
}
