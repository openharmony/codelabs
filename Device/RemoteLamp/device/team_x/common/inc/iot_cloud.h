/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
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

 
#ifndef __IOT_CLOUD_H__
#define __IOT_CLOUD_H__

#include <stdbool.h>
#include <iot_demo_def.h>

/**
 * @brief Initialize the cloud sdk
 * @return 0 success while others failed
 */
int CLOUD_Init(void);
/**
 * @brief Do deinitialize the cloud sdk
 * @return 0 success while others failed
 */
int CLOUD_Deinit(void);

/**
 * @brief Send collected data to Cloud Platform
 * @param jsonString, which means this has been formated as the profile defines
 * @return Returns 0 success while others failed
*/
int CLOUD_ReportMsg(const char *jsonString);

/**
 * @brief Connect to the Cloud Platform
 * @param deviceID Indicats the deviceID create in the iot platform
 * @param devicePwd Indicates the corresponding to the deviceID
 * @param serverIP Indicates the ip of the iot platform
 * @param serverPort Indicates the port correspond to the ip
 * @param cmdCallBack Indicates command callback and will be called if any message comes
 * @return Returns 0 success while others failed
*/
int CLOUD_Connect(const char *deviceID, const char *devicePwd, \
    const char *serverIP, const char *serverPort);

/**
 * @brief Disconnect from the Cloud Platform
 * @return 0 success while others failed
*/
int CLOUD_Disconnect(void);

/**
 * @brief use this is a call back function implemented by the demo
 * @param jsonString indicated the jsonString received from the iot_cloud
 * @return Returns 0 success while -1 failed
 */
int CLOUD_CommandCallBack(const char *jsonString);


/**
 * @brief functions and data for the syntax format
 *
 */

// enum all the data type for the oc profile
typedef enum {
    IOT_PROFILE_KEY_DATATYPE_INT = 0,
    IOT_PROFILE_KEY_DATATYPE_LONG,
    IOT_PROFILE_KEY_DATATYPE_FLOAT,
    IOT_PROFILE_KEY_DATATYPE_DOUBLE,
    IOT_PROFILE_KEY_DATATYPE_STRING,
    IOT_PROFILE_KEY_DATATYPE_LAST,
}IotProfileDataType;


typedef struct {
    void                 *nxt;   ///< ponit to the next key
    char                 *key;
    IotProfileDataType    type;
    void                 *value;
}IotProfileKV;


typedef struct {
    void *nxt;
    char *serviceID; ///< the service id in the profile, which could not be NULL
    char *eventTime; ///< eventtime, which could be NULL means use the platform time
    IotProfileKV *propertyLst; ///< the property in the profile, which could not be NULL
} IotProfileService;


/**
 * @brief Package the profile to json string mode, and you should free it manually
 * @param serviceLst, profile services
 * @return Returns the formates json string or NULL if failed
 *
 */
char *IoTProfilePackage(IotProfileService *serviceLst);

char *IotNotificationPackage(int type, const char *enString, const char *chString);

typedef enum {
    NOTIFY_TYPE_NORMAL = 0,
    NOTIFY_TYPE_SECONDARY,
    NOTIFY_TYPE_URGENT,
    NOTIFY_TYPE_LAST
} NOTIFY_TYPE;

int CLOUD_ReportNotification(int type, const char *enString, const char *chString);

#endif /* __IOT_CLOUD_H__ */

