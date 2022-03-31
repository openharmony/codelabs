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
import media from '@ohos.multimedia.media';
import { MediaLibOperator, MediaType } from '../media/MediaLibOperator'
import DateTimeUtils from '../../util/DateTimeUtils'

let RecordState = {
  IDLE: -1,
  PREPARE: 0,
  START: 1,
  PAUSE: 2,
  RESUME: 3,
  STOP: 4,
  RELEASE: 5,
  RESET: 6,
  ERROR: 7
}

export { RecordState }

export class AudioRecorder {
  private currentState = RecordState.IDLE;
  private recorder;
  private mediaLibOperator: MediaLibOperator
  private curRecordAsset
  private statusChangedListener;
  private recordConfig = {
    audioEncoder: 3,
    audioEncodeBitRate: 22050,
    audioSampleRate: 22050,
    numberOfChannels: 2,
    format: 6,
    uri: '',
    location: { latitude: 30, longitude: 130 },
  }

  constructor() {
    this.mediaLibOperator = MediaLibOperator.getInstance()
    this.initAudioRecorder();
  }

  initAudioRecorder() {
    this.recorder = media.createAudioRecorder();
    this.setOperateListener();
    this.setStatusListener();
  }

  setOperateListener() {
    this.recorder.on('reset', () => {
      this.currentState = RecordState.RESET;
      if (typeof (this.statusChangedListener) != 'undefined') {
        this.statusChangedListener(RecordState.RESET);
      }
    });
    this.recorder.on('release', () => {
      this.currentState = RecordState.RELEASE;
      if (typeof (this.statusChangedListener) != 'undefined') {
        this.statusChangedListener(RecordState.RELEASE);
      }
    });
    this.recorder.on('error', (err) => {
      console.info('AudioRecorder error callback,err is ' + JSON.stringify(err))
      this.currentState = RecordState.ERROR;
      if (typeof (this.statusChangedListener) != 'undefined') {
        this.statusChangedListener(RecordState.ERROR, err);
      }
    });
  }

  setStatusListener() {
    this.recorder.on('prepare', () => {
      this.recorder.start();
      this.currentState = RecordState.PREPARE;
      if (typeof (this.statusChangedListener) != 'undefined') {
        this.statusChangedListener(RecordState.PREPARE);
      }
    });
    this.recorder.on('start', () => {
      this.currentState = RecordState.START;
      if (typeof (this.statusChangedListener) != 'undefined') {
        this.statusChangedListener(RecordState.START, this.curRecordAsset);
      }
    });
    this.recorder.on('pause', () => {
      this.currentState = RecordState.PAUSE;
      if (typeof (this.statusChangedListener) != 'undefined') {
        this.statusChangedListener(RecordState.PAUSE);
      }
    });
    this.recorder.on('resume', () => {
      this.currentState = RecordState.RESUME;
      if (typeof (this.statusChangedListener) != 'undefined') {
        this.statusChangedListener(RecordState.RESUME);
      }
    });
    this.recorder.on('stop', () => {
      this.currentState = RecordState.STOP;
      this.mediaLibOperator.closeOperationChannel()
      if (typeof (this.statusChangedListener) != 'undefined') {
        this.statusChangedListener(RecordState.STOP);
      }
    });
  }

  start() {
    if (this.currentState !== RecordState.IDLE) {
      this.reset();
    }
    let fileName = "AID_" + DateTimeUtils.getDate() + DateTimeUtils.getTime() + ".mp3"
    this.mediaLibOperator.openCreationOperationChannel(fileName, MediaType.AUDIO).then((operationAsset) => {
      console.info('cwq startOperateNewAsset success,operationAsset is ' + JSON.stringify(operationAsset))
      this.curRecordAsset = operationAsset.asset
      this.recordConfig.uri = "fd://" + operationAsset.operationId.toString()
      this.recorder.prepare(this.recordConfig);
    })
  }

  pause() {
    if (this.currentState === RecordState.START) {
      this.recorder.pause();
    }
  }

  resume() {
    if (this.currentState === RecordState.PAUSE) {
      this.recorder.resume();
    }
  }

  stop() {
    if (this.currentState === RecordState.START || this.currentState === RecordState.RESUME) {
      this.recorder.stop();
    }
  }

  reset() {
    this.stop();
    this.recorder.reset();
  }

  release() {
    this.recorder.release();
  }

  setOnStatusChangedListener(callback) {
    this.statusChangedListener = callback;
  }

  isRecording() {
    return this.currentState === RecordState.START;
  }
}