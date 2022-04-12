import { HvigorPlugin } from "../plugin/plugin";
/**
 * hvigor的module模型
 *
 * @since 2021/12/27
 */
export interface DefaultModule {
    /**
     * hvigorfile.js导出对象
     */
    exported: any;
    /**
     * 获取Module名称
     */
    getName: () => string;
    /**
     * 获取Module的hvigorFile.js路径
     */
    getBuildFilePath: () => string;
    /**
     * 获取模块路径
     */
    getModuleDir: () => string;
    /**
     * 获取Module的packageJson
     */
    getPackageJsonPath: () => string;
    /**
     * 获取根项目project
     */
    getProject: () => Project;
    /**
     * 通过名称获取Module
     *
     * @param moduleName
     */
    findModuleByName: (moduleName: string) => DefaultModule | undefined;
    /**
     * 注入plugin对象
     *
     * @param plugin
     */
    bindPlugin: (plugin: HvigorPlugin) => void;
    /**
     * 获取plugin对象
     */
    getPlugin: () => HvigorPlugin | undefined;
}
/**
 * hvigor项目中的根module
 *
 * @since 2022/1/8
 */
export interface Project extends DefaultModule {
    /**
     * 获取子模块
     * 根据name直接获取子模块
     */
    getSubProjects: () => Map<string, DefaultModule>;
    /**
     * 添加子模块
     */
    addSubProject: (module: DefaultModule) => void;
    /**
     * 获取所有的的子模块
     */
    getAllSubProjects: () => DefaultModule[];
}
/**
 * hvigor项目中的子模块module
 *
 * @since 2022/1/8
 */
export declare type Module = DefaultModule;
