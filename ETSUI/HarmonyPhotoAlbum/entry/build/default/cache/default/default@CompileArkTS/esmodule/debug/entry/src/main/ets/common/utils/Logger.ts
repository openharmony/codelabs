import hilog from "@ohos:hilog";
import prompt from "@ohos:promptAction";
const LOGGER_PREFIX: string = 'Electronic Album';
class Logger {
    private domain: number;
    private prefix: string;
    // format Indicates the log format string.
    private format: string = '%{public}s, %{public}s';
    /**
     * constructor.
     *
     * @param prefix Identifies the log tag.
     * @param domain Indicates the service domain, which is a hexadecimal integer ranging from 0x0 to 0xFFFFF
     * @param args Indicates the log parameters.
     */
    constructor(prefix: string = '', domain: number = 0xFF00) {
        this.prefix = prefix;
        this.domain = domain;
    }
    debug(...args: string[]): void {
        hilog.debug(this.domain, this.prefix, this.format, args);
    }
    info(...args: string[]): void {
        hilog.info(this.domain, this.prefix, this.format, args);
    }
    warn(...args: string[]): void {
        hilog.warn(this.domain, this.prefix, this.format, args);
    }
    error(...args: string[]): void {
        hilog.error(this.domain, this.prefix, this.format, args);
    }
    /**
     * 弹出错误提示，对用户友好展示
     */
    showError(message: string) {
        prompt.showToast({
            message: message,
            duration: 3000
        });
        this.error(message);
    }
    /**
     * 通用轻提示
     */
    showToast(message: string) {
        prompt.showToast({
            message: message,
            duration: 2000
        });
    }
}
export default new Logger(LOGGER_PREFIX);
