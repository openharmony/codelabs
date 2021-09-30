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

import {titles, newsData} from '../../../default/common/data/data.js';
import router from '@system.router';
import prompt from '@system.prompt';
import featureAbility from '@ohos.ability.featureAbility';
import RemoteDeviceModel from '../../../model/RemoteDeviceModel.js';

export default {
    data: {
        title: "",
        type: "",
        imgUrl: "",
        reads: "",
        likes: "",
        content: "",
        titleList: titles,
        newsList: newsData,
        isJump: false,
    },
    onReady() {
        this.restoreFromWant();
    },
    restoreFromWant() {
        featureAbility.getWant((error, want) => {
            console.info('MusicPlayer[IndexPage] featureAbility.getWant=' + JSON.stringify(want));
            var status = want.parameters;
            this.title = status.title
            this.type = status.type,
            this.imgUrl = status.imgUrl,
            this.reads = status.reads,
            this.likes = status.likes,
            this.content = status.content

            if (status.remote == "come_from_remote" && !this.isJump) {
                this.isJump = true
                router.push({
                    uri: "pages/detail/detail",
                    params: {
                        "title": this.title,
                        "type": this.type,
                        "imgUrl": this.imgUrl,
                        "reads": this.reads,
                        "likes": this.likes,
                        "content": this.content,
                    }
                });
            }
        });
    },
    changeNewsType: function (e) {
        var type = titles[e.index].name;
        this.newsList = [];
        if (type === "All") {
            this.newsList = newsData;
        } else {
            var newsArray = [];
            for (var news of newsData) {
                if (news.type === type) {
                    newsArray.push(news);
                }
            }
            this.newsList = newsArray;
        }
    },
    itemClick(news) {
        router.push({
            uri: "pages/detail/detail",
            params: {
                "title": news.title,
                "type": news.type,
                "imgUrl": news.imgUrl,
                "reads": news.reads,
                "likes": news.likes,
                "content": news.content,
            }
        });
    }
}
