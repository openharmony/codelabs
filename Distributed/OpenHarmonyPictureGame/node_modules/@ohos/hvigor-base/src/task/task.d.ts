import { DefaultModule } from "../impl/hvigor-module";
/**
 * Hvigor可执行任务的基础类
 *
 * @since 2022/1/20
 */
export declare abstract class Task {
    /**
     * 注册task function
     */
    abstract registry(): (cb: () => void) => void;
}
/**
 * Hvigor同步类型任务的基础类
 *
 * @since 2022/1/20
 */
export declare abstract class DefaultSyncTask extends Task {
    private _log;
    protected defaultModule: DefaultModule;
    protected constructor(defaultModule: DefaultModule);
    registry: () => (cb: () => void) => void;
    protected abstract doTaskAction(hvigorModel: DefaultModule): void;
}
