import prompt from '@system.prompt';
import featureAbility from '@ohos.ability.featureAbility';

export default {
    data: {
        title: "",
        type: "",
        imgUrl: "",
        reads: "",
        likes: "",
        content: "",
        deviceList: [],
    },
    async toShare() {

//        prompt.showToast({
//            message: "开启分布式能力"
//        });

//
//        var ret = await FeatureAbility.getDeviceList(0);

        this.deviceList = new Array();
        this.deviceList[0] = {
            deviceName: "本机",
            networkId: "0",
            checked: false
        }
//        if (ret.code === 0) {
//            for (var i = 0; i < ret.data.length; i++) {
//                console.info('[DeviceLog] getDeviceList ' + JSON.stringify(ret.data[i]));
//                this.deviceList[i] = {
//                    deviceName: ret.data[i].deviceName,
//                    networkId: ret.data[i].networkId,
//                    checked: false
//                }
//            }
//        }
        this.$element('showDialog').show();
    },
    chooseCancel() {
        console.log("[DeviceLog] chooseCancel")
        this.$element('showDialog').close();
    },
    async chooseComform() {
        this.$element('showDialog').close();

//        for (var i = 0; i < this.deviceList.length; i++) {
//            if (this.deviceList[i].checked) {
//
//                let actionData = {
//                    title: this.title,
//                    type: this.type,
//                    imgUrl: this.imgUrl,
//                    reads: this.reads,
//                    likes: this.likes,
//                    content: this.content
//                };
//
//                let target = {
//                    bundleName: "com.huawei.codedemo",
//                    abilityName: "com.huawei.codedemo.MainAbility",
//                    url: "pages/detail/detail",
//                    networkId: this.deviceList[i].networkId,
//                    data: actionData
//                };
//
//                let result = await FeatureAbility.startAbility(target);
//                let ret = JSON.parse(result);
//                if (ret.code == 0) {
//                    console.log('success');
//                } else {
//                    console.log('cannot start browing service, reason: ' + ret.data);
//                }
//            }
//        }
    },
    selectDevice(index, e) {
        this.deviceList[index].checked = e.checked
    },
}
