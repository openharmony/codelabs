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
 * @file iot_gpio_ex.h
 *
 * @brief Declares the extended GPIO interface functions.
 *
 * These functions are used for settings GPIO pulls and driver strength. \n
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef IOT_GPIO_EX_H
#define IOT_GPIO_EX_H


/**
 * @brief Enumerates the functions of GPIO hardware pin 0.
 */
typedef enum {
    /** GPIO0 function */
    IOT_GPIO_FUNC_GPIO_0_GPIO,
    /** Functions of UART1 TXD */
    IOT_GPIO_FUNC_GPIO_0_UART1_TXD = 2,
    /** SPI1 CK function */
    IOT_GPIO_FUNC_GPIO_0_SPI1_CK,
    /** Functions of JTAG TD0 */
    IOT_GPIO_FUNC_GPIO_0_JTAG_TDO,
    /** PWM3 OUT function */
    IOT_GPIO_FUNC_GPIO_0_PWM3_OUT,
    /** I2C1 SDA function */
    IOT_GPIO_FUNC_GPIO_0_I2C1_SDA,
} WifiIotIoFuncGpio0;

/**
 * @brief Enumerates the functions of GPIO hardware pin 1.
 */
typedef enum {
    /** GPIO1 function */
    IOT_GPIO_FUNC_GPIO_1_GPIO,
    /** UART1 RXD function */
    IOT_GPIO_FUNC_GPIO_1_UART1_RXD = 2,
    /** SPI1 RXD function */
    IOT_GPIO_FUNC_GPIO_1_SPI1_RXD,
    /** JTAG TCKfunction */
    IOT_GPIO_FUNC_GPIO_1_JTAG_TCK,
    /** PWM4 OUT function */
    IOT_GPIO_FUNC_GPIO_1_PWM4_OUT,
    /** I2C1 SCL function */
    IOT_GPIO_FUNC_GPIO_1_I2C1_SCL,
    /** BT FREQ function */
    IOT_GPIO_FUNC_GPIO_1_BT_FREQ,
} WifiiIotIoFuncGpio1;

/**
 * @brief Enumerates the functions of GPIO hardware pin 2.
 */
typedef enum {
    /** GPIO2 function */
    IOT_GPIO_FUNC_GPIO_2_GPIO,
    /** UART1 RTS function */
    IOT_GPIO_FUNC_GPIO_2_UART1_RTS_N = 2,
    /** SPI1 TXD function */
    IOT_GPIO_FUNC_GPIO_2_SPI1_TXD,
    /** JTAG TRSTN function */
    IOT_GPIO_FUNC_GPIO_2_JTAG_TRSTN,
    /** PWM2 OUT function */
    IOT_GPIO_FUNC_GPIO_2_PWM2_OUT,
    /** SSI CLK function */
    IOT_GPIO_FUNC_GPIO_2_SSI_CLK = 7,
} WifiIotIoFuncGpio2;

/**
 * @brief Enumerates the functions of GPIO hardware pin 3.
 */
typedef enum {
    /** GPIO3 function */
    IOT_GPIO_FUNC_GPIO_3_GPIO,
    /** UART0 TXD function */
    IOT_GPIO_FUNC_GPIO_3_UART0_TXD,
    /** UART1 CTS function */
    IOT_GPIO_FUNC_GPIO_3_UART1_CTS_N,
    /** SPI CSN function */
    IOT_GPIO_FUNC_GPIO_3_SPI1_CSN,
    /** JTAG TDI function */
    IOT_GPIO_FUNC_GPIO_3_JTAG_TDI,
    /** PWM5 OUT function */
    IOT_GPIO_FUNC_GPIO_3_PWM5_OUT,
    /** I2C1 SDA function */
    IOT_GPIO_FUNC_GPIO_3_I2C1_SDA,
    /** SSI DATA function */
    IOT_GPIO_FUNC_GPIO_3_SSI_DATA,
} WifiIotIoFuncGpio3;

/**
 * @brief Enumerates the functions of GPIO hardware pin 4.
 */
typedef enum {
    /** GPIO4 function */
    IOT_GPIO_FUNC_GPIO_4_GPIO,
    /** UART0 RXD function */
    IOT_GPIO_FUNC_GPIO_4_UART0_RXD = 2,
    /** JTAG TMS function */
    IOT_GPIO_FUNC_GPIO_4_JTAG_TMS = 4,
    /** PWM1 OUT function */
    IOT_GPIO_FUNC_GPIO_4_PWM1_OUT,
    /** I2C1 SCL function */
    IOT_GPIO_FUNC_GPIO_4_I2C1_SCL,
} WifiIotIoFuncGpio4;

/**
 * @brief Enumerates the functions of GPIO hardware pin 5.
 */
typedef enum {
    /** GPIO5 function */
    IOT_GPIO_FUNC_GPIO_5_GPIO,
    /** UART1 RXD function */
    IOT_GPIO_FUNC_GPIO_5_UART1_RXD = 2,
    /** SPI0 CSN function */
    IOT_GPIO_FUNC_GPIO_5_SPI0_CSN,
    /** PWM2 OUT function */
    IOT_GPIO_FUNC_GPIO_5_PWM2_OUT = 5,
    /** I2C0 MCLK function */
    IOT_GPIO_FUNC_GPIO_5_I2S0_MCLK,
    /** BT STATUS function */
    IOT_GPIO_FUNC_GPIO_5_BT_STATUS,
} WifiIotIoFuncGpio5;

/**
 * @brief Enumerates the functions of GPIO hardware pin 6.
 */
typedef enum {
    /** GPIO6 function */
    IOT_GPIO_FUNC_GPIO_6_GPIO,
    /** UART1 TXD function */
    IOT_GPIO_FUNC_GPIO_6_UART1_TXD = 2,
    /** SPI0 CK function */
    IOT_GPIO_FUNC_GPIO_6_SPI0_CK,
    /** PWM3 OUT function */
    IOT_GPIO_FUNC_GPIO_6_PWM3_OUT = 5,
    /** I2S0 TX function */
    IOT_GPIO_FUNC_GPIO_6_I2S0_TX,
    /** COEX switch function */
    IOT_GPIO_FUNC_GPIO_6_COEX_SWITCH,
} WifiIotIoFuncGpio6;

/**
 * @brief Enumerates the functions of GPIO hardware pin 7.
 */
typedef enum {
    /** GPIO7 function */
    IOT_GPIO_FUNC_GPIO_7_GPIO,
    /** UART1 CTS function */
    IOT_GPIO_FUNC_GPIO_7_UART1_CTS_N = 2,
    /** SPI0 RXD function */
    IOT_GPIO_FUNC_GPIO_7_SPI0_RXD,
    /** PWM0 OUT function */
    IOT_GPIO_FUNC_GPIO_7_PWM0_OUT = 5,
    /** I2S0 BCLK function */
    IOT_GPIO_FUNC_GPIO_7_I2S0_BCLK,
    /** BT ACTIVE function */
    IOT_GPIO_FUNC_GPIO_7_BT_ACTIVE,
} WifiIotIoFuncGpio7;

/**
 * @brief Enumerates the functions of GPIO hardware pin 8.
 */
typedef enum {
    /** GPIO8 function */
    IOT_GPIO_FUNC_GPIO_8_GPIO,
    /** UART1 RTS function */
    IOT_GPIO_FUNC_GPIO_8_UART1_RTS_N = 2,
    /** SPI0 TXD function */
    IOT_GPIO_FUNC_GPIO_8_SPI0_TXD,
    /** PWM1 OUT function */
    IOT_GPIO_FUNC_GPIO_8_PWM1_OUT = 5,
    /** I2S0 WS function */
    IOT_GPIO_FUNC_GPIO_8_I2S0_WS,
    /** WLAN ACTIVE function */
    IOT_GPIO_FUNC_GPIO_8_WLAN_ACTIVE,
} WifiIotIoFuncGpio8;

/**
 * @brief Enumerates the functions of GPIO hardware pin 9.
 */
typedef enum {
    /** GPIO9 function */
    IOT_GPIO_FUNC_GPIO_9_GPIO,
    /** I2C0 SCL function */
    IOT_GPIO_FUNC_GPIO_9_I2C0_SCL,
    /** UART2 RTS function */
    IOT_GPIO_FUNC_GPIO_9_UART2_RTS_N,
    /** SDIO D2 function */
    IOT_GPIO_FUNC_GPIO_9_SDIO_D2,
    /** SPI0 TXD function */
    IOT_GPIO_FUNC_GPIO_9_SPI0_TXD,
    /** PWM0 OUT function */
    IOT_GPIO_FUNC_GPIO_9_PWM0_OUT,
    /** I2S0 MCLK function */
    IOT_GPIO_FUNC_GPIO_9_I2S0_MCLK = 7,
} WifiIotIoFuncGpio9;

/**
 * @brief Enumerates the functions of GPIO hardware pin 10.
 */
typedef enum {
    /** GPIO10 function */
    IOT_GPIO_FUNC_GPIO_10_GPIO,
    /** I2C0 SDA function */
    IOT_GPIO_FUNC_GPIO_10_I2C0_SDA,
    /** UART2 CTS function */
    IOT_GPIO_FUNC_GPIO_10_UART2_CTS_N,
    /** SDIO D3 function */
    IOT_GPIO_FUNC_GPIO_10_SDIO_D3,
    /** SPI0 CK function */
    IOT_GPIO_FUNC_GPIO_10_SPI0_CK,
    /** PWM1 OUT function */
    IOT_GPIO_FUNC_GPIO_10_PWM1_OUT,
    /** I2S0 TX function */
    IOT_GPIO_FUNC_GPIO_10_I2S0_TX = 7,
} WifiIotIoFuncGpio10;

/**
 * @brief Enumerates the functions of GPIO hardware pin 11.
 */
typedef enum {
    /** GPIO11 function */
    IOT_GPIO_FUNC_GPIO_11_GPIO,
    /** UART2 TXD function */
    IOT_GPIO_FUNC_GPIO_11_UART2_TXD = 2,
    /** SDIO CMD function */
    IOT_GPIO_FUNC_GPIO_11_SDIO_CMD,
    /** SDIO RXD function */
    IOT_GPIO_FUNC_GPIO_11_SPI0_RXD,
    /** PWM2 OUT function */
    IOT_GPIO_FUNC_GPIO_11_PWM2_OUT,
    /** RF TX_EN_EXT function */
    IOT_GPIO_FUNC_GPIO_11_RF_TX_EN_EXT,
    /** I2S0 RX function */
    IOT_GPIO_FUNC_GPIO_11_I2S0_RX,
} WifiIotIoFuncGpio11;

/**
 * @brief Enumerates the functions of GPIO hardware pin 12.
 */
typedef enum {
    /** GPIO12 function */
    IOT_GPIO_FUNC_GPIO_12_GPIO,
    /** SUART2 RXD function */
    IOT_GPIO_FUNC_GPIO_12_UART2_RXD = 2,
    /** SDIO CLK function */
    IOT_GPIO_FUNC_GPIO_12_SDIO_CLK,
    /** SDIO CSN function */
    IOT_GPIO_FUNC_GPIO_12_SPI0_CSN,
    /** PWM3 OUT function */
    IOT_GPIO_FUNC_GPIO_12_PWM3_OUT,
    /** RF RX_EN_EXT function */
    IOT_GPIO_FUNC_GPIO_12_RF_RX_EN_EXT,
    /** I2S0 BCLK function */
    IOT_GPIO_FUNC_GPIO_12_I2S0_BCLK,
} WifiIotIoFuncGpio12;

/**
 * @brief Enumerates the functions of GPIO hardware pin 13.
 */
typedef enum {
    /** SSI DATA function */
    IOT_GPIO_FUNC_GPIO_13_SSI_DATA,
    /** UART0 TXD function */
    IOT_GPIO_FUNC_GPIO_13_UART0_TXD,
    /** UART2 RTS function */
    IOT_GPIO_FUNC_GPIO_13_UART2_RTS_N,
    /** SDIO D0 function */
    IOT_GPIO_FUNC_GPIO_13_SDIO_D0,
    /** GPIO13 function */
    IOT_GPIO_FUNC_GPIO_13_GPIO,
    /** PWM4 OUT function */
    IOT_GPIO_FUNC_GPIO_13_PWM4_OUT,
    /** I2C0 SDA function */
    IOT_GPIO_FUNC_GPIO_13_I2C0_SDA,
    /** I2S0 WS function */
    IOT_GPIO_FUNC_GPIO_13_I2S0_WS,
} WifiIotIoFuncGpio13;

/**
 * @brief Enumerates the functions of GPIO hardware pin 14.
 */
typedef enum {
    /** SSI CLK function */
    IOT_GPIO_FUNC_GPIO_14_SSI_CLK,
    /** UART0 RXD function */
    IOT_GPIO_FUNC_GPIO_14_UART0_RXD,
    /** UART2 CTS function */
    IOT_GPIO_FUNC_GPIO_14_UART2_CTS_N,
    /** SDIO D1 function */
    IOT_GPIO_FUNC_GPIO_14_SDIO_D1,
    /** GPIO14 function */
    IOT_GPIO_FUNC_GPIO_14_GPIO,
    /** PWM5 OUT function */
    IOT_GPIO_FUNC_GPIO_14_PWM5_OUT,
    /** I2C0 SCL function */
    IOT_GPIO_FUNC_GPIO_14_I2C0_SCL,
} WifiIotIoFuncGpio14;

/**
 * @brief Enumerates GPIO pull-up or pull-down settings.
 */
typedef enum {
    /** No pull */
    IOT_GPIO_PULL_NONE,
    /** Pull-up */
    IOT_GPIO_PULL_UP,
    /** Pull-down */
    IOT_GPIO_PULL_DOWN,
    /** Maximum value */
    IOT_GPIO_PULL_MAX,
} IotGpioPull;


unsigned int IoTGpioSetFunc(unsigned int id, unsigned char val);

unsigned int IoTGpioSetPull(unsigned int id, IotGpioPull val);

#endif
/** @} */