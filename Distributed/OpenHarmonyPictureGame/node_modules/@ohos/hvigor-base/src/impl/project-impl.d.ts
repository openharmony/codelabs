import { DefaultModule, Module, Project } from "./hvigor-module";
import { DefaultModuleImpl } from "./default-module-impl";
/**
 * hvigor工程的基础root module
 *
 * @since 2021/12/27
 */
export declare class ProjectImpl extends DefaultModuleImpl implements Project {
    private readonly _projectStructureFile;
    private readonly _subProjects;
    constructor(moduleName: string, moduleDir: string);
    findModuleByName(moduleName: string): Module | undefined;
    getProject(): Project;
    getSubProjects(): Map<string, Module>;
    addSubProject(module: Module): void;
    getAllSubProjects(): DefaultModule[];
}
