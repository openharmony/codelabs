/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**
 * @addtogroup wifiiot
 * @{
 *
 * @brief Provides dedicated device operation interfaces on the Wi-Fi module,
 * including ADC, AT, flash, GPIO, I2C, I2S, partition, PWM, SDIO, UART, and watchdog.
 *
 *
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file wifiiot_i2c_ex.h
 *
 * @brief Declares the extended I2C interface functions.
 *
 * These functions are used for I2C baud rate setting and device exception callback. \n
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef IOT_I2C_EX_H
#define IOT_I2C_EX_H


/**
 * @brief Defines I2C data transmission attributes.
 */
typedef struct {
    /** Pointer to the buffer storing data to send */
    unsigned char *sendBuf;
    /** Length of data to send */
    unsigned int  sendLen;
    /** Pointer to the buffer for storing data to receive */
    unsigned char *receiveBuf;
    /** Length of data received */
    unsigned int  receiveLen;
} IotI2cData;


/**
 * @brief Sends data to and receives data responses from an I2C device.
 *
 *
 *
 * @param id Indicates the I2C device ID.
 * @param deviceAddr Indicates the I2C device address.
 * @param i2cData Indicates the pointer to the device descriptor of the data to receive.
 * @return Returns {@link WIFI_IOT_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link wifiiot_errno.h} otherwise.
 * @since 1.0
 * @version 1.0
 */
unsigned int IoTI2cWriteread(unsigned int  id, unsigned short deviceAddr, const IotI2cData *i2cData);



#endif
/** @} */
