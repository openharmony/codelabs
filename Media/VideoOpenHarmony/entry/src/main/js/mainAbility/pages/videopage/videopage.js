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

export default {
    data: {
        event: '',
        seekingtime: '',
        timeupdatetime: '',
        seekedtime: '',
        isStart: true,
        duration: '',
    },
    preparedCallback(e) {
        this.event = '视频连接成功';
        this.duration = e.duration;
    },
    startCallback() {
        this.event = '视频开始播放';
    },
    pauseCallback() {
        this.event = '视频暂停播放';
    },
    finishCallback() {
        this.event = '视频播放结束';
    },
    errorCallback() {
        this.event = '视频播放错误';
    },
    seekingCallback(e) {
        this.seekingtime = e.currenttime;
    },
    timeupdateCallback(e) {
        this.timeupdatetime = e.currenttime;
    },
    changeStartPause() {
        if (this.isStart) {
            this.$element('videoId').pause();
            this.isStart = false;
        } else {
            this.$element('videoId').start();
            this.isStart = true;
        }
    },
};
