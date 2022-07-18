/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import MediaAssetBuilder from './MediaAssetBuilder'
import brightness from '@ohos.brightness';
import { StatusChangedListener, MediaPlayerState, TAGS, MediaPlaySpeed, MediaAsset, MediaOperationAsset
} from './MediaConstants';
import media from '@ohos.multimedia.media'
import LogUtils from '../../util/LogUtils'

export default class MediaPlayService {
    private mediaAssetBuilder: MediaAssetBuilder
    private state = MediaPlayerState.IDLE
    private mediaPlayer
    private displaySurfaceId
    private displayNeedChanged: boolean = false
    private isPrepared: boolean
    private volumeValue: number = 0.5
    private brightness: number = 0.5
    private statusChangedListeners: StatusChangedListener[]
    private mediaAsset: MediaAsset
    private mediaLibAsset: MediaOperationAsset

    constructor() {
        this.mediaAssetBuilder = new MediaAssetBuilder()
        this.statusChangedListeners = new Array()
        this.initPlayer()
    }

    private initPlayer() {
        media.createVideoPlayer().then((player) => {
            if (player != null) {
                LogUtils.info(TAGS.MEDIA_PLAYER, 'createVideoPlayer success!');
                this.mediaPlayer = player
                this.setCallback()
            }
        }, this.failureCallback).catch(this.catchCallback)
    }

    /*
    *play/seek/playbackCompleted回调BUFFERING_START和BUFFERING_END
    */
    private setCallback() {
        this.mediaPlayer.on('playbackCompleted', () => {
            this.stepCallback(MediaPlayerState.FINISH, null)
        });
        this.mediaPlayer.on('bufferingUpdate', (infoType, value) => {
            switch (infoType) {
                case media.BufferingInfoType.BUFFERING_START:
                    this.stepCallback(MediaPlayerState.BUFFERING_START, value)
                    break;
                case media.BufferingInfoType.BUFFERING_END:
                    this.stepCallback(MediaPlayerState.BUFFERING_END, value)
                    break;
                case media.BufferingInfoType.BUFFERING_PERCENT:
                    this.stepCallback(MediaPlayerState.BUFFERING_PERCENT, value)
                    break;
                case media.BufferingInfoType.CACHED_DURATION:
                    this.stepCallback(MediaPlayerState.CACHED_DURATION, value)
                    break;
            }
            if (this.getCurrentTime() == this.getDuration()) {
                this.state = MediaPlayerState.FINISH
            } else if (this.mediaPlayer.state == 'playing' || this.mediaPlayer.state == 'prepared') {
                this.state = MediaPlayerState.PLAY
            } else if (this.mediaPlayer.state == 'paused') {
                this.state = MediaPlayerState.PAUSE
            } else if (this.mediaPlayer.state == 'stopped') {
                this.state = MediaPlayerState.STOP
            } else if (this.mediaPlayer.state == 'error') {
                this.state = MediaPlayerState.ERROR
            }
        });
        this.mediaPlayer.on('startRenderFrame', () => {
            this.stepCallback(MediaPlayerState.START, null)
            this.state = MediaPlayerState.PLAY
        });
        this.mediaPlayer.on('videoSizeChanged', (width, height) => {
            this.stepCallback(MediaPlayerState.SIZE_CHANGED, {
                width: width, height: height
            })
        });
        this.mediaPlayer.on('error', (error) => {
            this.stepCallback(MediaPlayerState.ERROR, error)
        });
    }

    private getKeyFrame(ms: number): number{
        return Math.round(ms / 1000) * 1000
    }

    public addStatusChangedListener(listener: StatusChangedListener) {
        this.statusChangedListeners.push(listener)
    }

    public addSurface(surfaceId) {
        this.displaySurfaceId = surfaceId
        this.displayNeedChanged = true
    }

    /*
    *@param asset.playSrc string|number
    *  支持网络路径:http://
    *   hls直播流:m3u8
    *  支持本地路径：data/
    *  支持媒体库fileId
    *  支持项目路径../../resources/rawfile/
    */
    public async loadAsset(asset: MediaAsset, isAutoPlay: boolean, seekMs?: number) {
        await this.stop()
        if (this.mediaAsset == null || this.mediaAsset.getSource() != asset.getSource()) {
            await this.reset()
            this.mediaAsset = await this.mediaAssetBuilder.build(asset)
            this.mediaPlayer.url = this.mediaAssetBuilder.getRealUrl()
            this.stepCallback(MediaPlayerState.LOAD, {
                asset: this.mediaAsset
            })
        }
        if (this.displayNeedChanged) {
            await this.mediaPlayer.setDisplaySurface(this.displaySurfaceId)
            this.displayNeedChanged = false
        }
        this.start(isAutoPlay, seekMs)
    }

    private start(isAutoPlay: boolean, seekMs?: number) {
        this.mediaPlayer.prepare().then(() => {
            this.isPrepared = true
            this.stepCallback(MediaPlayerState.PREPARED, {
                duration: this.getDuration()
            })
            if (isAutoPlay) {
                this.play(seekMs)
            }
        }).catch((error) => {
            this.stepCallback(MediaPlayerState.ERROR, error)
        });
    }

    public async play(seekMs?: number) {
        if (!this.isPrepared) {
            this.start(true, seekMs)
        } else {
            this.mediaPlayer.play().then(() => {
                if (seekMs) {
                    this.seek(seekMs)
                }
                this.stepCallback(MediaPlayerState.PLAY, null)
            })
        }
    }

    public pause() {
        if (this.isPrepared && this.state == MediaPlayerState.PLAY) {
            this.mediaPlayer.pause().then(() => {
                this.stepCallback(MediaPlayerState.PAUSE, null)
            });
        }
    }

    public resume() {
        if (this.state == MediaPlayerState.PAUSE) {
            this.play()
        }
    }

    public seek(ms) {
        if (this.isPrepared && this.state != MediaPlayerState.ERROR) {
            let seekMode = this.getCurrentTime() < ms ? 0 : 1
            let realTime = (ms <= 0 ? 0 : (ms >= this.getDuration() ? this.getDuration() : this.getKeyFrame(ms)))
            this.mediaPlayer.seek(realTime, seekMode)
        }
    }

    public setSpeed(speed: MediaPlaySpeed) {
        if (this.isPrepared) {
            this.mediaPlayer.setSpeed(speed)
        }
    }

    /*
     *@param vol [0.00-1.00]
     */
    public async setVolume(vol) {
        if (this.isPrepared) {
            let value = (vol <= 0 ? 0 : (vol >= 1 ? 1 : vol))
            await this.mediaPlayer.setVolume(value)
            this.volumeValue = value
        }
    }

    public getVolume() {
        return this.volumeValue
    }

    /*
    *@param bright [0.00-1.00] bright设置为0时屏幕无亮度
    */
    public setBrightness(bright: number) {
        let value = (bright <= 0 ? 0.1 : (bright >= 1 ? 1 : bright))
        brightness.setValue(value * 255)
        this.brightness = bright
    }

    public getBrightness() {
        return this.brightness
    }

    public resizeScreen(width: number, height: number) {
        if (width >= 0 && height >= 0) {
            this.stepCallback(MediaPlayerState.SIZE_CHANGED, {
                width: width, height: height
            })
        }
    }

    public async stop() {
        if (this.isPrepared) {
            await this.mediaPlayer.stop()
            this.stepCallback(MediaPlayerState.STOP, null)
        }
    }

    private async reset() {
        await this.mediaAssetBuilder.release()
        await this.mediaPlayer.reset()
        this.isPrepared = false
    }

    public release() {
        this.stop()
        this.reset()
        this.mediaPlayer.release()
        this.stepCallback(MediaPlayerState.IDLE, null)
        this.statusChangedListeners.length = 0
        this.statusChangedListeners = null
    }

    public getCurrentTime() {
        if (this.isPrepared) {
            return this.mediaPlayer.currentTime
        }
        return 0
    }

    public getDuration() {
        if (this.isPrepared) {
            return this.mediaPlayer.duration
        }
        return 0
    }

    async getTrackDescription() {
        return await this.mediaPlayer.getTrackDescription()
    }

    public getPlayerState() {
        return this.state
    }

    public getMediaAsset(): MediaAsset{
        return this.mediaAsset
    }

    /* 函数调用发生状态改变 */
    private stepCallback(state, extra) {
        LogUtils.info(TAGS.MEDIA_PLAYER, 'stepCallback, state:' + state + ',extra is ' + extra);
        this.state = state
        for (let i = 0; i < this.statusChangedListeners.length; i++) {
            this.statusChangedListeners[i](state, extra)
        }
    }

    failureCallback= (error) => {
        LogUtils.info(TAGS.MEDIA_PLAYER, `error happened,error Name is ${error.name}`);
        LogUtils.info(TAGS.MEDIA_PLAYER, `error happened,error Code is ${error.code}`);
        LogUtils.info(TAGS.MEDIA_PLAYER, `error happened,error Message is ${error.message}`);
    }

    // 当函数调用发生异常时用于上报错误信息
    catchCallback= (error) => {
        LogUtils.info(TAGS.MEDIA_PLAYER, `catch error happened,error Name is ${error.name}`);
        LogUtils.info(TAGS.MEDIA_PLAYER, `catch error happened,error Code is ${error.code}`);
        LogUtils.info(TAGS.MEDIA_PLAYER, `catch error happened,error Message is ${error.message}`);
    }
}

