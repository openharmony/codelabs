/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup NetcfgService
 * @{
 *
 * @brief Provides the network configuration service. Two network configuration modes are available:
 * Neighbor Awareness Networking (NAN) and software enabled access point (SoftAP).
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file network_config_service.h
 *
 * @brief Defines functions of the <b>NetcfgService</b> module.
 *
 * You can use the functions to: \n
 * <ul>
 *   <li>Start the network configuration service.</li>
 *   <li>Stop the network configuration service.</li>
 *   <li>Register callbacks for events of the network configuration service.</li>
 *   <li>Notify the network configuration result.</li>
 * </ul>
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef NETWORK_CONFIG_SERVICE_H
#define NETWORK_CONFIG_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Represents the maximum length (32 bytes) of a SoftAP hotspot name.
 *
 */
#define SSID_MAX_LEN 32

/**
 * @brief Represents the maximum length (64 bytes) of a SoftAP hotspot password.
 *
 */
#define PWD_MAX_LEN 64

/**
 * @brief Represents the maximum length (32 bytes) of a Wi-Fi Service Set Identifier (SSID).
 *
 */
#define WIFI_SSID_MAX_LEN 32

/**
 * @brief Represents the maximum length (64 bytes) of the password for Wi-Fi configuration.
 *
 */
#define WIFI_PWD_MAX_LEN 64

/**
 * @brief Represents the maximum length (32 bytes) of the pre-shared key for Wi-Fi configuration.
 *
 */
#define WIFI_PSK_LEN 32

/**
 * @brief Represents the maximum length (64 bytes) of the Basic Service Set Identifiers (BSSID) \n
 *        for Wi-Fi configuration.
 *
 */
#define WIFI_BSSID_LEN 6

/**
 * @brief Enumerates Wi-Fi encryption types.
 *
 */
enum WifiPairwise {
    /** UNKNOWN */
    WIFI_PARIWISE_UNKNOWN,
    /** AES */
    WIFI_PAIRWISE_AES,
    /** TKIP */
    WIFI_PAIRWISE_TKIP,
    /** TKIP and AES */
    WIFI_PAIRWISE_TKIP_AES_MIX
};

/**
 * @brief Enumerates Wi-Fi authentication types.
 *
 */
enum WifiAuthMode {
    /** Open */
    WIFI_SECURITY_OPEN,
    /** WEP */
    WIFI_SECURITY_WEP,
    /** WPA2-PSK */
    WIFI_SECURITY_WPA2PSK,
    /** WPA-PSK and WPA2-PSK */
    WIFI_SECURITY_WPAPSK_WPA2PSK_MIX,
    /** WPA-PSK */
    WIFI_SECURITY_WPAPSK,
    /** WPA */
    WIFI_SECURITY_WPA,
    /** WPA2 */
    WIFI_SECURITY_WPA2,
    /** SAE */
    WIFI_SECURITY_SAE,
    /** UNKNOWN */
    WIFI_SECURITY_UNKNOWN
};

/**
 * @brief Enumerates network configuration types.
 *
 */
enum NetCfgType {
    /** SoftAP */
    NETCFG_SOFTAP = 0,
    /** SoftAP and NAN */
    NETCFG_SOFTAP_NAN,
    /** Reserved */
    NETCFG_BUTT,
};

/**
 * @brief Enumerates common error codes of the <b>NetcfgService</b> module.
 *
 */
enum NetworkCfgErrorCode {
    /** Success */
    NETCFG_OK = 0,
    /** Execution failure */
    NETCFG_ERROR = -1,
    /** Invalid device information */
    NETCFG_DEV_INFO_INVALID = -2,
    /** Invalid SoftAP network configuration parameters */
    NETCFG_SOFTAP_PARAM_INVALID = -3,
    /** Incorrect network configuration mode */
    NETCFG_MODE_INVALID = -4,
    /** Invalid power for short-distance transmission */
    NETCFG_POWER_INVALID = -5,
};

/**
 * @brief Enumerates information transmission channels for Wi-Fi configuration.
 *
 */
enum WifiInfoSource {
    /** Wi-Fi network configuration information is transmitted through NAN channels. */
    WIFI_INFO_FROM_NAN = 0,
    /** Wi-Fi network configuration information is transmitted through SoftAP channels. */
    WIFI_INFO_FROM_SOFTAP,
};

/**
 * @brief Enumerates network configuration service statuses.
 *
 */
enum NetCfgStatus {
    /** In NAN mode, blinking or buzzing stops when the network configuration is complete or times out. */
    NETCFG_IDENTIFY_DISABLE  = 0,
    /** In NAN mode, the indicator blinks or the buzzer buzzes when a device is authenticated for access. */
    NETCFG_IDENTIFY_ENABLE = 1,
    /** The device is in network configuration. */
    NETCFG_WORKING = 2
};

/**
 * @brief Defines SoftAP hotspot parameters.
 */
struct SoftAPParam {
    /** (Mandatory) Hotspot name. For details about the maximum length, see {@link SSID_MAX_LEN}. */
    char ssid[SSID_MAX_LEN + 1];
    /** (Optional) Hotspot password. For details about the maximum length, see {@link PWD_MAX_LEN}.
     * If the password is empty, the SoftAP is in open mode.
     * If the password is not empty, the password length must be less than or equal to the value of <b>PWD_MAX_LEN</b>.
     */
    char password[PWD_MAX_LEN + 1];
    /** (Optional) Authentication type
     * If the password is empty, the authentication mode must be open.
     * If the password is specified, the authentication type can be <b>WEP</b>, <b>WPA2-PSK</b>, or both.
     */
    int authType;
    /** (Optional) Encryption type.
     * Currently, <b>UNKNOWN</b>, <b>AES</b>, <b>TKIP</b>, and <b>TKIP and AES</b> encryption types are supported.
     */
    int pairwise;
    /** (Optional) Whether to hide the SSID.
     * <b>0</b> means not to hide the SSID, and <b>1</b> means to hide the SSID.
     */
    int isHidden;
    /** (Optional) Working channel required in the SoftAP mode
     * <b>0</b>: The channel is specified by the system. <b>1</b> to <b>14</b>: The channel is specified by users.
     */
    int channelNum;
};

/**
 * @brief Defines device parameters, which are passed when the application starts the network configuration service.
 */
struct DevInfo {
    /** Device parameter name, which contains a maximum of 32 bytes. */
    const char *key;
    /** Device parameter value, which contains a maximum of 64 bytes. */
    const char *value;
};

/**
 * @brief Defines the SSID name, password, and channel used for Wi-Fi configuration.
 */
struct WifiInfo {
    /** SSID name. For details about the maximum length, see {@link WIFI_SSID_MAX_LEN}. */
    unsigned char ssid[WIFI_SSID_MAX_LEN + 1];
    /** SSID password. For details about the maximum length, see {@link WIFI_PWD_MAX_LEN}. */
    unsigned char pwd[WIFI_PWD_MAX_LEN + 1];
    /** Pre-shared key. For details about the maximum length, see {@link WIFI_PSK_LEN}. */
    unsigned char psk[WIFI_PSK_LEN + 1];
    /** BSSID. For details about the maximum length, see {@link WIFI_BSSID_LEN}. */
    unsigned char bssid[WIFI_BSSID_LEN + 1];
    /** Length of the SSID name */
    unsigned char ssidLen;
    /** Length of the SSID password */
    unsigned char pwdLen;
    /** Length of the pre-shared key */
    unsigned char pskLen;
    /** Length of the BSSID */
    unsigned char bssidLen;
    /** Authentication type used for Wi-Fi configuration
     * If the password is empty, the authentication mode must be open.
     * If the password is specified, the authentication type can be <b>WEP</b>, <b>WPA2-PSK</b>, or both.
     */
    int authMode;
    /** Channel for transmitting information. For details about the value, see {@link WifiInfoSource}. */
    int wifiInfoSrc;
    /** Working channel used for Wi-Fi configuration. The value ranges from 1 to 14. */
    int channelNumber;
};

/**
 * @brief Defines callbacks for network configuration registration.
 */
typedef struct {
    /** Obtains the PIN code of the device. The maximum length of the PIN code is 16 bytes. */
    int (*GetPinCode)(unsigned char *pinCode, unsigned int size, unsigned int *len);
    /** Processes Wi-Fi information and vendor data during network configuration */
    int (*ParseNetCfgData)(const struct WifiInfo *wifiInfo,
        const unsigned char *vendorData, unsigned int vendorDataLen);
    /** Processes application rapid control data in the rapid control phase.
     * The <b>svcId</b> parameter is reserved for device compatibility. The default value is <b>NULL</b>.
     * The <b>mode</b> parameter is reserved. The default value is <b>0</b>.
     */
    int (*RecvRawData)(const char *svcId, unsigned int mode, const char *data);
    /** Notifies the network configuration service status. \n
     * For example, when a device is in NAN mode, this function can be called to control the blinking or buzzer used \n
     * for notifications. \n
     * <b>status</b> indicates the network configuration status. \n
     * The value <b>0</b> means to stop buzzing or blinking. \n
     * The value <b>1</b> means to start buzzing or blinking. \n
     * The value <b>2</b> means that the device is in the network configuration state. \n
     */
    void (*NotifyNetCfgStatus)(enum NetCfgStatus status);
    /** Disconnects the network configuration. \n
     * This function is used to notify the application when an internal error occurs during network configuration \n
     * or a disconnection request is received from the mobile phone. \n
     * The <b>errCode</b> values are as follows: \n
     * <b>0</b>: The application fails to receive the network configuration data. \n
     *           The network configuration service automatically starts. \n
     * <b>1</b>: The application has been notified of the network configuration data and needs to determine \n
     *           whether to stop or restart the network configuration service. \n
     */
    void (*OnDisconnect)(int errCode);
} NetCfgCallback;

/**
 * @brief Sets the transmit channel power for ultra-short-distance transmission \n
 *        before the network configuration service is started.
 *
 * @param power Indicates the power for ultra-short-distance transmission. \n
 *        The value can be <b>-70</b>, <b>-67</b>, <b>-65</b>, <b>-63</b>, <b>-61</b>, <b>-58</b>, <b>-55</b>, \n
 *        <b>-52</b>, <b>-48</b>, <b>-45</b>, or <b>-42</b>. To prevent a mobile phone far away from the device \n
 *        from receiving security information, ensure that the wireless transmit power at any point \n
 *        on the product (including the external antenna) does not exceed -65 dBm.
 * @return Returns the operation result. For details, see {@link NetworkCfgErrorCode}.
 * @since 1.0
 * @version 1.0
 */
int SetSafeDistancePower(signed char power);

/**
 * @brief Sets SoftAP hotspot parameters.
 *
 * @param param Indicates the pointer to the structure of the hotspot parameters. For details, see {@link SoftAPParam}.
 * @return Returns the operation result. For details, see {@link NetworkCfgErrorCode}.
 * @since 1.0
 * @version 1.0
 */
int SetSoftAPParameter(const struct SoftAPParam *param);

/**
 * @brief Registers a callback.
 *
 * Call this function before enabling the network configuration service.
 *
 * @param callback Indicates the pointer to the structure of the callback function. \n
 *        For details, see {@link NetCfgCallback}.
 * @return Returns the operation result. For details, see {@link NetworkCfgErrorCode}.
 * @since 1.0
 * @version 1.0
 */
int RegNetCfgCallback(const NetCfgCallback *callback);

/**
 * @brief Starts a network configuration.
 *
 * Packets with the multicast address 238.238.238.238 and port number 5683 are listened by default.
 * Note: In NAN mode, call this function after the SoftAP is started successfully.
 *
 * @param devInfoList Indicates the pointer to the device information list. For details, see {@link DevInfo}.
 * @param listSize Indicates the number of devices in the device information list.
 * @param mode Indicates the working mode of the network configuration service. For details, see {@link NetcfgPara}.
 * @return Returns the operation result. For details, see {@link NetworkCfgErrorCode}.
 * @since 1.0
 * @version 1.0
 */
int StartNetCfg(const struct DevInfo *devInfoList, unsigned int listSize, enum NetCfgType mode);

/**
 * @brief Stops the network configuration.
 *
 * @return Returns the operation result. For details, see {@link NetworkCfgErrorCode}.
 * @since 1.0
 * @version 1.0
 */
int StopNetCfg(void);

/**
 * @brief Notifies the network configuration result.
 *
 * @param result Indicates the network configuration result.
 * @return Returns the operation result. For details, see {@link NetworkCfgErrorCode}.
 * @since 1.0
 * @version 1.0
 */
int NotifyNetCfgResult(signed int result);

/**
 * @brief Sends data.
 *
 * @param data Indicates the pointer to the data to send. The data is a character string.
 * @return Returns the operation result. For details, see {@link NetworkCfgErrorCode}.
 * @since 1.0
 * @version 1.0
 */
int SendRawData(const char *data);
#ifdef __cplusplus
}
#endif

#endif
