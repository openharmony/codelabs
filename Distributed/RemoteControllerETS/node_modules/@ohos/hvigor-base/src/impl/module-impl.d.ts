import { Module, Project } from "./hvigor-module";
import { DefaultModuleImpl } from "./default-module-impl";
/**
 * hvigor工程的基础module
 *
 * @since 2021/12/27
 */
export declare class ModuleImpl extends DefaultModuleImpl implements Module {
    private readonly _project;
    constructor(project: Project, moduleName: string, moduleDir: string);
    findModuleByName(moduleName: string): Module | undefined;
    getProject(): Project;
}
