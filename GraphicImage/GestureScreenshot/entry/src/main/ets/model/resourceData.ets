export class Resources {
    id: number;
    resourceSrc: string;
    resourceType: number;
    createTime:number;
    updateTime:number;
    constructor(id: number, resourceSrc: string,  resourceType: number, createTime:number,updateTime:number) {
        this.id = id;
        this.resourceSrc = resourceSrc;
        this.resourceType = resourceType;
        this.createTime = createTime;
        this.updateTime = updateTime;
    }
}

const resourceList = [
    {
        'resourceSrc': "common/picture/sun.JPG",
        'resourceType': 1,
        'createTime': new Date().getTime(),
        'updateTime': new Date().getTime()
    }
]

export function getResourcesList(): Array<Resources> {
    let resourcesArr: Array<Resources> = []
    resourceList.forEach(item => {
        resourcesArr.push(new Resources(null, item.resourceSrc, item.resourceType, item.createTime, item.updateTime));
    })
    return resourcesArr;
}

