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
        event: '',
        seekingtime: '',
        timeupdatetime: '',
        seekedtime: '',
        isStart: true,
        isfullscreenchange: false,
        duration: '',
    },
    preparedCallback: function (e) {
        this.event = '视频连接成功';
        this.duration = e.duration;
    },
    startCallback: function () {
        this.event = '视频开始播放';
    },
    pauseCallback: function () {
        this.event = '视频暂停播放';
    },
    finishCallback: function () {
        this.event = '视频播放结束';
    },
    errorCallback: function () {
        this.event = '视频播放错误';
    },
    seekingCallback: function (e) {
        this.seekingtime = e.currenttime;
    },
    timeupdateCallback: function (e) {
        this.timeupdatetime = e.currenttime;
    },
    change_start_pause: function () {
        if (this.isStart) {
            this.$element('videoId').pause();
            this.isStart = false;
        } else {
            this.$element('videoId').start();
            this.isStart = true;
        }
    },
    start:async function() {
        let actionData = {
        };
        let target = {
            bundleName: "com.huawei.video",
            abilityName: "com.huawei.video.MainAbility",
            data: actionData
        };

        let result = await FeatureAbility.startAbility(target);
        let ret = JSON.parse(result);
    }
}