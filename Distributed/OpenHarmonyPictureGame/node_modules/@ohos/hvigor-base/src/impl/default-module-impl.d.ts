import { DefaultModule, Project } from "./hvigor-module";
import { HvigorPlugin } from "../plugin/plugin";
/**
 * hvigor中module的默认公共父类
 *
 * @since 2022/1/8
 */
export declare abstract class DefaultModuleImpl implements DefaultModule {
    /**
     * 保存对应hvigorfile.js中export的对象
     */
    exported: any;
    protected _plugin: HvigorPlugin | undefined;
    protected readonly _moduleName: string;
    protected readonly _modulePath: string;
    protected readonly _packageJsonPath: string;
    protected readonly _buildFilePath: string;
    protected constructor(moduleName: string, modulePath: string);
    /**
     * 获取构建的hvigorfile.js路径
     */
    getBuildFilePath(): string;
    /**
     * 获取模块的路径
     */
    getModuleDir(): string;
    /**
     * 获取模块的名称
     */
    getName(): string;
    /**
     * 获取模块下的package.json路径
     */
    getPackageJsonPath(): string;
    /**
     * 绑定具体的Plugin对象到Module对象上
     *
     * @param plugin
     * @protected
     */
    bindPlugin(plugin: HvigorPlugin): void;
    /**
     * 获取Module对象的plugin对象
     *
     * @returns {HvigorPlugin | undefined}
     */
    getPlugin(): HvigorPlugin | undefined;
    /**
     * 根据ModuleName获取DefaultModule模型
     *
     * @param moduleName
     */
    abstract findModuleByName(moduleName: string): DefaultModule | undefined;
    /**
     * 获取根项目的Project对象
     */
    abstract getProject(): Project;
}
