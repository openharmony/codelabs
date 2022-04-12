import { Project } from "./src/impl/hvigor-module.js";
export * from "./src/plugin/plugin.js";
export * from "./src/impl/hvigor-module.js";
export * from "./src/task/task.js";
export * from "./src/options/common-const.js";
export * from "./src/log/hvigor-log.js";
declare const Undertaker: any;
/**
 * 提供前端工程打包的工具的主入口
 *
 * @since 2021/12/09
 */
export declare class Hvigor extends Undertaker {
    constructor();
}
/**
 * 提供前端工程配置信息的数据类
 *
 * @since 2021/12/09
 */
export declare class VigorConfig {
    private _log;
    private _project;
    private _projectDir;
    private _projectConfigFile;
    private _extraConfig;
    constructor();
    /**
     * 返回Project模型
     *
     * @returns {Project | undefined}
     */
    getProject(): Project | undefined;
    /**
     * 返回在命令行中传递的额外参数
     *
     * @returns {Map<string, string>}
     */
    getExtraConfig(): Map<string, string>;
    setExtraConfig(value: Map<string, string>): void;
    /**
     * 以工程根路径下的配置文件，对工程进行初始化
     *
     * @param {string} projectConfigFile 工程级别build-profile.json5的路径
     * @param {string} projectRootDir 工程根路径
     * @returns {Project}
     */
    initRootProject(projectConfigFile: string, projectRootDir: string): Project;
}
export declare const vigorConfigInst: VigorConfig;
