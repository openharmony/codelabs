import { Configuration, Level, Logger } from "log4js";
export * from 'log4js';
/**
 * 基于log4js封装的HvigorLogger
 *
 * @since 2022/03/02
 */
export declare class HvigorLogger {
    protected readonly _logger: Logger;
    protected constructor(category?: string);
    /**
     * 获取对于类别的HvigorLogger实例
     *
     * @param category 默认是default
     */
    static getLogger(category?: string): HvigorLogger;
    log(level: Level | string, ...args: any[]): void;
    logArray(level: Level | string, array: any[], title?: string): void;
    debug(message: any, ...args: any[]): void;
    info(message: any, ...args: any[]): void;
    warn(message: any, ...args: any[]): void;
    error(message: any, ...args: any[]): void;
    errorMessageExit(message: string, ...args: any[]): void;
    errorExit(e: Error, message?: string, ...args: any[]): void;
    errorNoExit(e: Error): void;
    setLevel(level: Level): void;
    getLevel(): Level | string;
    configure(configuration: Configuration): void;
    getConfig(): Configuration;
}
