export { TAGS, AudioPlayerState, VideoRecorderState, CaptureMode, AudioRecordState, MediaType, FrameConfigInfo,
    MediaOperationAsset, MediaPlayerState, MediaPlaySpeed, VideoPlayerGestureType, StatusChangedListener,
    EncodeAction, MediaAsset, MediaSourceType, AudioCaptureAction, AudioRenderAction, AudioPlaySpeed,
    DecodeAction, CodecState }

interface StatusChangedListener {
    (state, extra): void
}

enum TAGS {
    CAMERA_SERVICE = '[CameraService]',
    VIDEO_DECODER = '[VideoDecoder]',
    VIDEO_ENCODER = '[VideoEncoder]',
    VIDEO_RECORDER = '[VideoRecorder]',
    MEDIA_PLAYER = '[MediaPlayer]',
    AUDIO_RECORDER = '[AudioRecorder]',
    AUDIO_PLAYER = '[AudioPlayer]',
    AUDIO_CAPTURE = '[AudioCapture]',
    MEDIA_PLAYER_LIFECYCLE = '[MediaPlayerLifeCycle]'
}

enum MediaPlaySpeed {
    SPEED_FORWARD_0_75_X = 0,
    SPEED_FORWARD_1_00_X = 1,
    SPEED_FORWARD_1_25_X = 2,
    SPEED_FORWARD_1_75_X = 3,
    SPEED_FORWARD_2_00_X = 4,
}

enum VideoPlayerGestureType {
    IDLE,
    PROGRESS_CONTROL,
    VOLUME_CONTROL,
    BRIGHT_CONTROL
}

enum MediaPlayerState {
    IDLE,
    LOAD,
    PREPARED,
    START,
    PLAY,
    PAUSE,
    STOP,
    ERROR,
    FINISH,
    BUFFERING_START,
    BUFFERING_END,
    BUFFERING_PERCENT,
    CACHED_DURATION,
    SIZE_CHANGED,
}

enum MediaSourceType {
    DEFAULT,
    MEDIA_LIB_FILE,
    ABSOLUTE_PATH_FILE,
    RAWFILE_FILE
}

enum AudioPlayerState {
    IDLE,
    LOAD,
    PLAY,
    PAUSE,
    STOP,
    ERROR,
    FINISH,
    PROGRESS_SPEED,
    TIME_UPDATE,
    VOLUME_CHANGE
}

enum VideoRecorderState {
    IDLE,
    CONFIGURED,
    START,
    PAUSE,
    STOP,
    RESET,
    RELEASE,
    ERROR
}

enum AudioPlaySpeed {
    RENDER_RATE_NORMAL = 0,
    RENDER_RATE_DOUBLE = 1,
    RENDER_RATE_HALF = 2,
}

enum CaptureMode {
    PHOTO,
    VIDEO,
    ENCODE,
    MULTI
}

enum AudioRecordState {
    IDLE,
    PREPARE,
    START,
    COLLECT_BUFFER,
    PAUSE,
    STOP,
    RELEASE,
    RESET,
    ERROR
}

enum MediaType {
    FILE = 1,
    IMAGE = 3,
    VIDEO = 4,
    AUDIO = 5,
}

enum EncodeAction {
    INIT,
    START,
    STOP,
    TERMINATE
}

enum DecodeAction {
    INIT,
    START,
    LOAD_BUFFER,
    STOP,
    TERMINATE
}

enum CodecState {
    IDLE,
    PREPARED,
    STARTED,
    CODE_CHANGE,
    CODE_CHANGING,
    CODE_CHANGED,
    STOP,
    RESET,
    RELEASE,
    ERROR
}

enum AudioCaptureAction {
    INIT,
    START,
    PAUSE,
    RESUME,
    STOP,
    TERMINATE
}

enum AudioRenderAction {
    INIT,
    START,
    PAUSE,
    SPEED,
    STOP,
    TERMINATE
}


class MediaOperationAsset {
    private operationId: number = -1
    private asset: any

    public setFd(fd: number) {
        this.operationId = fd
    }

    public getFd() {
        return this.operationId
    }

    public setAsset(asset: any) {
        this.asset = asset
    }

    public getAsset() {
        return this.asset
    }
}

class MediaAsset {
    private title: string
    private source

    public setTitle(playTitle: string) {
        this.title = playTitle
    }

    public getTitle(): string {
        return this.title
    }

    public setSource(src) {
        this.source = src
    }

    public getSource() {
        return this.source
    }
}

class FrameConfigInfo {
    private index
    private timeMs
    private offset
    private length
    private flags

    public setIndex(index) {
        this.index = index
    }

    public getIndex() {
        return this.index
    }

    public setTimeMs(timeMs) {
        this.timeMs = timeMs
    }

    public getTimeMs() {
        return this.timeMs
    }

    public setOffset(offset) {
        this.offset = offset
    }

    public getOffset() {
        return this.offset
    }

    public setLength(length) {
        this.length = length
    }

    public getLength() {
        return this.length
    }

    public setFlags(flags) {
        this.flags = flags
    }

    public getFlags() {
        return this.flags
    }
}