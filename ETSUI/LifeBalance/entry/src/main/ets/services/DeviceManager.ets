// import distributedHardware from '@ohos.distributedHardware';
//
// let deviceManager = distributedHardware.getDeviceManager();
//
// deviceManager.addDeviceDiscoveryCallback({
//   onDeviceFound(device: distributedHardware.DeviceInfo) {
//     console.log(`发现设备：${device.deviceName} (ID: ${device.deviceId})`);
//     if (device.deviceType === distributedHardware.DeviceType.PHONE) {
//     }
//   },
//   onDeviceLost(device: distributedHardware.DeviceInfo) {
//     console.log(`设备断开：${device.deviceName}`);
//   }
// }, 'multi_device_group');
//
// deviceManager.startDiscovery('multi_device_group');