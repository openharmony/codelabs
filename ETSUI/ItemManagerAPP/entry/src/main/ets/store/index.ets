import { ItemStore } from './ItemStore';

//导出全局状态存储实例，在整个应用中共享状态
export const itemStore = ItemStore.getInstance();