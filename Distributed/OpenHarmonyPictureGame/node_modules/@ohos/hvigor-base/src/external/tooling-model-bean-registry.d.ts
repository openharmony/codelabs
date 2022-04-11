import { ToolingModelBean } from "./tooling-model-bean";
/**
 * Hvigor的数据扩展模型的注册器接口定义
 *
 * @since 2022/2/19
 */
export interface ToolingModelBeanRegistry {
    registry(modelBean: ToolingModelBean): void;
    getModelMap(): Map<string, ToolingModelBean>;
}
