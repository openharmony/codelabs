/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 *
 */

/**
 * 音频录制与播放服务
 * 封装 HarmonyOS 音频能力
 */
import { media } from '@kit.MediaKit'
import { fileIo } from '@kit.CoreFileKit'
import { Context, abilityAccessCtrl, Permissions, common } from '@kit.AbilityKit'
import { Logger } from '../common/utils/Logger'
import { Recording, RecordingInit } from '../model/Recording'

const TAG = 'AudioService'

/**
 * 录音状态
 */
export enum RecordingState {
  IDLE = 'idle',
  RECORDING = 'recording',
  PAUSED = 'paused'
}

/**
 * 播放状态
 */
export enum PlaybackState {
  IDLE = 'idle',
  PLAYING = 'playing',
  PAUSED = 'paused'
}

/**
 * 录音回调接口
 */
export interface RecordingCallback {
  onDurationUpdate?: (duration: number) => void
  onRecordingComplete?: (recording: Recording) => void
  onError?: (error: string) => void
}

/**
 * 播放回调接口
 */
export interface PlaybackCallback {
  onProgressUpdate?: (currentTime: number, duration: number) => void
  onPlaybackComplete?: () => void
  onError?: (error: string) => void
}

/**
 * 音频服务类
 */
export class AudioService {
  private static instance: AudioService | null = null
  private context: Context | null = null
  private avRecorder: media.AVRecorder | null = null
  private avPlayer: media.AVPlayer | null = null
  // 录音状态
  private recordingState: RecordingState = RecordingState.IDLE
  private recordingStartTime: number = 0
  private recordingDuration: number = 0
  private currentRecordingPath: string = ''
  private durationTimer: number = -1
  private recordingCallback: RecordingCallback | null = null
  // 播放状态
  private playbackState: PlaybackState = PlaybackState.IDLE
  private currentPlayingId: string = ''
  private playbackCallback: PlaybackCallback | null = null
  private progressTimer: number = -1

  private constructor() {
  }

  static getInstance(): AudioService {
    if (!AudioService.instance) {
      AudioService.instance = new AudioService()
    }
    return AudioService.instance
  }

  /**
   * 初始化服务
   */
  async initialize(context: Context): Promise<void> {
    this.context = context
    Logger.info(TAG, 'AudioService initialized')
  }

  /**
   * 获取录音存储目录
   */
  private getRecordingDir(): string {
    if (!this.context) {
      return ''
    }
    return this.context.filesDir + '/recordings'
  }

  /**
   * 确保录音目录存在
   */
  private async ensureRecordingDir(): Promise<void> {
    const dir = this.getRecordingDir()
    try {
      await fileIo.access(dir)
    } catch {
      await fileIo.mkdir(dir)
      Logger.info(TAG, `Created recording directory: ${dir}`)
    }
  }

  /**
   * 生成录音文件名
   */
  private generateFileName(): string {
    const timestamp = Date.now()
    return `recording_${timestamp}.m4a`
  }

  /**
   * 检查并请求麦克风权限
   */
  private async checkAndRequestMicrophonePermission(): Promise<boolean> {
    if (!this.context) {
      Logger.error(TAG, 'Context is null, cannot request permission')
      return false
    }

    const permission: Permissions = 'ohos.permission.MICROPHONE'

    try {
      const atManager = abilityAccessCtrl.createAtManager()

      // 检查当前权限状态
      const grantStatus = await atManager.checkAccessToken(
        (this.context as common.UIAbilityContext).applicationInfo.accessTokenId,
        permission
      )

      if (grantStatus === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
        Logger.info(TAG, 'Microphone permission already granted')
        return true
      }

      // 请求权限
      const result = await atManager.requestPermissionsFromUser(
        this.context as common.UIAbilityContext,
        [permission]
      )

      const isGranted = result.authResults[0] === 0
      Logger.info(TAG, `Microphone permission request result: ${isGranted}`)
      return isGranted
    } catch (error) {
      Logger.error(TAG, `Failed to check/request permission: ${JSON.stringify(error)}`)
      return false
    }
  }

  // ==================== 录音功能 ====================

  /**
   * 开始录音
   */
  async startRecording(callback?: RecordingCallback): Promise<boolean> {
    if (this.recordingState === RecordingState.RECORDING) {
      Logger.warn(TAG, 'Already recording')
      return false
    }

    try {
      // 先检查并请求麦克风权限
      const hasPermission = await this.checkAndRequestMicrophonePermission()
      if (!hasPermission) {
        Logger.error(TAG, 'Microphone permission denied')
        if (callback?.onError) {
          callback.onError('麦克风权限被拒绝，请在设置中开启')
        }
        return false
      }

      await this.ensureRecordingDir()

      // 创建 AVRecorder
      this.avRecorder = await media.createAVRecorder()

      // 设置录音配置
      const fileName = this.generateFileName()
      this.currentRecordingPath = `${this.getRecordingDir()}/${fileName}`

      // 打开文件
      const file = fileIo.openSync(this.currentRecordingPath, fileIo.OpenMode.READ_WRITE | fileIo.OpenMode.CREATE)

      const avProfile: media.AVRecorderProfile = {
        audioBitrate: 128000,
        audioChannels: 1,
        audioCodec: media.CodecMimeType.AUDIO_AAC,
        audioSampleRate: 44100,
        fileFormat: media.ContainerFormatType.CFT_MPEG_4A
      }

      const avConfig: media.AVRecorderConfig = {
        audioSourceType: media.AudioSourceType.AUDIO_SOURCE_TYPE_MIC,
        profile: avProfile,
        url: `fd://${file.fd}`
      }

      await this.avRecorder.prepare(avConfig)
      await this.avRecorder.start()

      this.recordingState = RecordingState.RECORDING
      this.recordingStartTime = Date.now()
      this.recordingDuration = 0
      this.recordingCallback = callback || null

      // 启动时长计时器
      this.startDurationTimer()

      Logger.info(TAG, `Recording started: ${this.currentRecordingPath}`)
      return true
    } catch (error) {
      Logger.error(TAG, `Failed to start recording: ${JSON.stringify(error)}`)
      if (callback?.onError) {
        callback.onError('无法开始录音，请检查麦克风权限')
      }
      return false
    }
  }

  /**
   * 暂停录音
   */
  async pauseRecording(): Promise<boolean> {
    if (this.recordingState !== RecordingState.RECORDING || !this.avRecorder) {
      return false
    }

    try {
      await this.avRecorder.pause()
      this.recordingState = RecordingState.PAUSED
      this.stopDurationTimer()
      Logger.info(TAG, 'Recording paused')
      return true
    } catch (error) {
      Logger.error(TAG, `Failed to pause recording: ${JSON.stringify(error)}`)
      return false
    }
  }

  /**
   * 恢复录音
   */
  async resumeRecording(): Promise<boolean> {
    if (this.recordingState !== RecordingState.PAUSED || !this.avRecorder) {
      return false
    }

    try {
      await this.avRecorder.resume()
      this.recordingState = RecordingState.RECORDING
      this.startDurationTimer()
      Logger.info(TAG, 'Recording resumed')
      return true
    } catch (error) {
      Logger.error(TAG, `Failed to resume recording: ${JSON.stringify(error)}`)
      return false
    }
  }

  /**
   * 停止录音并保存
   */
  async stopRecording(): Promise<Recording | null> {
    if (this.recordingState === RecordingState.IDLE || !this.avRecorder) {
      return null
    }

    try {
      this.stopDurationTimer()
      await this.avRecorder.stop()
      await this.avRecorder.release()
      this.avRecorder = null

      // 获取文件信息
      const stat = await fileIo.stat(this.currentRecordingPath)
      const fileName = this.currentRecordingPath.split('/').pop() || ''

      const init: RecordingInit = {
        filePath: this.currentRecordingPath,
        fileName: fileName,
        duration: Math.floor(this.recordingDuration),
        fileSize: stat.size,
        format: 'aac'
      }

      const recording = new Recording(init)

      this.recordingState = RecordingState.IDLE

      if (this.recordingCallback?.onRecordingComplete) {
        this.recordingCallback.onRecordingComplete(recording)
      }

      Logger.info(TAG, `Recording saved: ${recording.id}, duration: ${recording.duration}s`)
      return recording
    } catch (error) {
      Logger.error(TAG, `Failed to stop recording: ${JSON.stringify(error)}`)
      this.recordingState = RecordingState.IDLE
      if (this.recordingCallback?.onError) {
        this.recordingCallback.onError('保存录音失败')
      }
      return null
    }
  }

  /**
   * 取消录音（不保存）
   */
  async cancelRecording(): Promise<void> {
    if (this.recordingState === RecordingState.IDLE) {
      return
    }

    try {
      this.stopDurationTimer()
      if (this.avRecorder) {
        await this.avRecorder.stop()
        await this.avRecorder.release()
        this.avRecorder = null
      }

      // 删除临时文件
      if (this.currentRecordingPath) {
        try {
          await fileIo.unlink(this.currentRecordingPath)
        } catch {
          // 文件可能不存在
        }
      }

      this.recordingState = RecordingState.IDLE
      Logger.info(TAG, 'Recording cancelled')
    } catch (error) {
      Logger.error(TAG, `Failed to cancel recording: ${JSON.stringify(error)}`)
      this.recordingState = RecordingState.IDLE
    }
  }

  /**
   * 启动时长计时器
   */
  private startDurationTimer(): void {
    this.durationTimer = setInterval(() => {
      this.recordingDuration = (Date.now() - this.recordingStartTime) / 1000
      if (this.recordingCallback?.onDurationUpdate) {
        this.recordingCallback.onDurationUpdate(this.recordingDuration)
      }
    }, 100)
  }

  /**
   * 停止时长计时器
   */
  private stopDurationTimer(): void {
    if (this.durationTimer !== -1) {
      clearInterval(this.durationTimer)
      this.durationTimer = -1
    }
  }

  // ==================== 播放功能 ====================

  /**
   * 播放录音
   */
  async playRecording(recording: Recording, callback?: PlaybackCallback): Promise<boolean> {
    try {
      // 如果正在播放其他录音，先停止
      if (this.playbackState !== PlaybackState.IDLE) {
        await this.stopPlayback()
      }

      // 创建 AVPlayer
      this.avPlayer = await media.createAVPlayer()
      this.playbackCallback = callback || null
      this.currentPlayingId = recording.id

      // 设置回调
      this.avPlayer.on('stateChange', (state: string) => {
        if (state === 'completed') {
          this.onPlaybackComplete()
        }
      })

      this.avPlayer.on('error', (err: Error) => {
        Logger.error(TAG, `Playback error: ${err.message}`)
        if (this.playbackCallback?.onError) {
          this.playbackCallback.onError('播放失败')
        }
        this.stopPlayback()
      })

      // 设置数据源并播放
      this.avPlayer.url = recording.filePath
      await this.avPlayer.prepare()
      await this.avPlayer.play()

      this.playbackState = PlaybackState.PLAYING
      this.startProgressTimer()

      Logger.info(TAG, `Playing: ${recording.id}`)
      return true
    } catch (error) {
      Logger.error(TAG, `Failed to play recording: ${JSON.stringify(error)}`)
      if (callback?.onError) {
        callback.onError('无法播放录音')
      }
      return false
    }
  }

  /**
   * 暂停播放
   */
  async pausePlayback(): Promise<boolean> {
    if (this.playbackState !== PlaybackState.PLAYING || !this.avPlayer) {
      return false
    }

    try {
      await this.avPlayer.pause()
      this.playbackState = PlaybackState.PAUSED
      this.stopProgressTimer()
      Logger.info(TAG, 'Playback paused')
      return true
    } catch (error) {
      Logger.error(TAG, `Failed to pause playback: ${JSON.stringify(error)}`)
      return false
    }
  }

  /**
   * 恢复播放
   */
  async resumePlayback(): Promise<boolean> {
    if (this.playbackState !== PlaybackState.PAUSED || !this.avPlayer) {
      return false
    }

    try {
      await this.avPlayer.play()
      this.playbackState = PlaybackState.PLAYING
      this.startProgressTimer()
      Logger.info(TAG, 'Playback resumed')
      return true
    } catch (error) {
      Logger.error(TAG, `Failed to resume playback: ${JSON.stringify(error)}`)
      return false
    }
  }

  /**
   * 停止播放
   */
  async stopPlayback(): Promise<void> {
    this.stopProgressTimer()

    if (this.avPlayer) {
      try {
        await this.avPlayer.stop()
        await this.avPlayer.release()
      } catch {
        // 忽略错误
      }
      this.avPlayer = null
    }

    this.playbackState = PlaybackState.IDLE
    this.currentPlayingId = ''
    Logger.info(TAG, 'Playback stopped')
  }

  /**
   * 跳转到指定位置
   */
  async seekTo(timeMs: number): Promise<void> {
    if (!this.avPlayer || this.playbackState === PlaybackState.IDLE) {
      return
    }

    try {
      await this.avPlayer.seek(timeMs)
    } catch (error) {
      Logger.error(TAG, `Failed to seek: ${JSON.stringify(error)}`)
    }
  }

  /**
   * 播放完成回调
   */
  private onPlaybackComplete(): void {
    this.stopProgressTimer()
    this.playbackState = PlaybackState.IDLE

    if (this.playbackCallback?.onPlaybackComplete) {
      this.playbackCallback.onPlaybackComplete()
    }

    this.currentPlayingId = ''
  }

  /**
   * 启动进度计时器
   */
  private startProgressTimer(): void {
    this.progressTimer = setInterval(() => {
      if (this.avPlayer && this.playbackCallback?.onProgressUpdate) {
        const currentTime = this.avPlayer.currentTime
        const duration = this.avPlayer.duration
        this.playbackCallback.onProgressUpdate(currentTime, duration)
      }
    }, 100)
  }

  /**
   * 停止进度计时器
   */
  private stopProgressTimer(): void {
    if (this.progressTimer !== -1) {
      clearInterval(this.progressTimer)
      this.progressTimer = -1
    }
  }

  // ==================== 文件操作 ====================

  /**
   * 删除录音文件
   */
  async deleteRecordingFile(filePath: string): Promise<boolean> {
    try {
      await fileIo.unlink(filePath)
      Logger.info(TAG, `Deleted recording file: ${filePath}`)
      return true
    } catch (error) {
      Logger.error(TAG, `Failed to delete recording file: ${JSON.stringify(error)}`)
      return false
    }
  }

  // ==================== 状态查询 ====================

  /**
   * 获取当前录音状态
   */
  getRecordingState(): RecordingState {
    return this.recordingState
  }

  /**
   * 获取当前播放状态
   */
  getPlaybackState(): PlaybackState {
    return this.playbackState
  }

  /**
   * 获取当前正在播放的录音 ID
   */
  getCurrentPlayingId(): string {
    return this.currentPlayingId
  }

  /**
   * 获取当前录音时长
   */
  getCurrentRecordingDuration(): number {
    return this.recordingDuration
  }
}

export const audioService = AudioService.getInstance()
