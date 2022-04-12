/**
 * Hvigor扩展数据模型的接口类型定义
 *
 * @since 2022/2/19
 */
export interface ToolingModelBean {
    modelId: string;
    buildModel(): string | undefined;
}
