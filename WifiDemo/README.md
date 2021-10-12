# WiFi Codelabs

## 一、获取源代码

```
make codelabs
cd codelabs
repo init -u https://gitee.com/openharmony/manifest.git -b master --no-repo-verify --depth=1
```
待下载完成后，下载二进制

```
repo forall -c 'git lfs pull'
```
下载编译工具链相关文件

```
build/prebuilts_download.sh
```

## 二、下载WiFi的相关仓并替换
1、下载codelabs仓
```
git clone https://gitee.com/openharmony/codelabs.git
```
2、将WifiDemo/device_hisilicon_drivers目录下的wifi拷贝至codelabs/device/hisilicon/drivers下，替换原有的wifi文件夹
3、将WifiDemo/vendor_hisilicon目录下的wifi拷贝至vendor/hisilicon/Hi3516DV300/hdf_config/khdf下，替换原有的wifi文件夹
4、加入测试文件的拷贝编译

```
#打开 drivers/adapter/uhdf2/ohos.build文件，插入如下
{
  "subsystem": "hdf",
  "parts": {
    "hdf": {
      "module_list": [
        ......
        "//device/hisilicon/drivers/wifi/wifitest:wifi_test",
        ......
      ],
      "test_list": [
         ......
      ]
    }
  }
}
```
