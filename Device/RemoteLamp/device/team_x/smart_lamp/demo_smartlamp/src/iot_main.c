#include "ohos_init.h"
#include "cmsis_os2.h"
#include "common.h"
#include "iot_netcfg_nan.h"
#include "oled.h"
#include "defines.h"
#include "lamp.h"
#include "network_config_service.h"

#include "iot_gpio.h"

#define IOT_GPIO_9 9


typedef struct {
    uint8 power_off;
}LampInfo;

static LampInfo g_lamp;

static void ShowPowerStatus(void)
{
    char power_sta[BUF_SIZE] = {0};
    if (sprintf_s(power_sta, BUF_SIZE, "power   %s", g_lamp.power_off == 1 ? "OFF" : "ON") < 0) {
        WINERR("sprintf_s failed! \n");
        return;
    }
    OledShowStr("                           ", strlen("                       "), POWER_XPOS, POWER_YPOS, 1);
    OledShowStr(power_sta, strlen(power_sta), POWER_XPOS, POWER_YPOS, 1);
}

static void LampShowInfo(void)
{
    ShowPowerStatus();
}

static void LampDealPoweroff(int value1, int value2)
{
    (void)value2;
    if (value1 == 0) {
        g_lamp.power_off = 1;		
		IoTGpioSetOutputVal(IOT_GPIO_9, 1);
    } else {
        g_lamp.power_off = 0;
		IoTGpioSetOutputVal(IOT_GPIO_9, 0);
    }
    ShowPowerStatus();
}

static int my_atoi(char *str, int length)
{
    char buf[MESSAGE_LEN] = {0};
    if (length >= MESSAGE_LEN) {
        WINERR("invliad length! \n");
        return -1;
    }

    if (strncpy_s(buf, MESSAGE_LEN, str, length) != 0) {
        WINERR("strncpy_s failed! \n");
        return 0;
    }

    return atoi(buf);
}

#define ARRAYSIZE(a)    (sizeof((a)) / sizeof((a)[0]))

typedef union {
    char msg[MESSAGE_LEN + 1];
    struct {
        char type[MSG_VAL_LEN];
        char value1[MSG_VAL_LEN];
        char value2[MSG_VAL_LEN];
    } msg_segment;
} MsgInfo;

typedef struct {
    int type;
    void (*ProcessFunc)(int value1, int value2);
} MsgProcess;

static MsgInfo g_msgInfo;
static MsgProcess g_msgProcess[] = {
    {MESSAGE_POWER_OFF, LampDealPoweroff}, // 灯开关指令
    {MESSAGE_POWER_OFF_S, LampDealPoweroff},
};

static void LampProcessAppMessage(const char *data, int data_len)
{
    if (data_len != MESSAGE_LEN) {
        WINERR("data len invalid! \n");
        return;
    }

    if (strncpy_s(g_msgInfo.msg, MESSAGE_LEN + 1, data, data_len) != 0) {
        WINERR("strncpy_s failed! \n");
        return;
    }

    for (uint8 i = 0; i < ARRAYSIZE(g_msgProcess); i++) {
        if (g_msgProcess[i].type == my_atoi(g_msgInfo.msg_segment.type, MSG_VAL_LEN)) {
            g_msgProcess[i].ProcessFunc(my_atoi(g_msgInfo.msg_segment.value1, MSG_VAL_LEN),
                                        my_atoi(g_msgInfo.msg_segment.value2, MSG_VAL_LEN));
        }
    }
}

static int LampNetEventHandler(NET_EVENT_TYPE event, void *data)
{
    switch (event) {
        case NET_EVENT_RECV_DATA: // 接收到网络信息(FA发送的消息)
            LampProcessAppMessage((const char *)data, strlen(data)); // 处理对应的信息
            break;
        default:
            break;
    }
    return 0;
}

static void LampInit(void)
{
    DeviceInit(); // 硬件相关初始化（比如I2C的初始化等）
    if (OledInit() != 0) { // OLED初始化
        WINERR("OledInit failed! \n");
    }
}

static void *LampTask(const char *arg)
{
    (void)arg;
    WINDBG("LampTask Enter! \n");
    LampInit();
    (void)memset_s(&g_lamp, sizeof(g_lamp), 0x00, sizeof(g_lamp));

    NetCfgRegister(LampNetEventHandler); // 进入配网状态并注册网络监听事件
    LampShowInfo(); // 显示LED初始状态
}

void LampDemoEntry(void)
{
    IoTGpioInit(IOT_GPIO_9);
    hi_io_set_func (IOT_GPIO_9,0);
    IoTGpioSetDir(IOT_GPIO_9, IOT_GPIO_DIR_OUT);

	osThreadAttr_t attr;
    attr.name = "LampTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = LAMP_TASK_STACK_SIZE;
    attr.priority = LAMP_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)LampTask, NULL, &attr) == NULL) {
        WINERR("Falied to create LampTask!\n");
    }
}

SYS_RUN(LampDemoEntry);
