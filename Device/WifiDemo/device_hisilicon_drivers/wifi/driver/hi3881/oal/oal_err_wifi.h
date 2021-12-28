/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __HI_ERR_WIFI_H__
#define __HI_ERR_WIFI_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 软件整体错误码定义 */
typedef enum {
    HI_SUCCESS                              = 0,
    HI_FAIL                                 = 1,                            /* 通用异常，仅在以上异常无法匹配填写此值 */
    HI_CONTINUE                             = 2,
    HI_INVALID                              = 3,
    /**************************************************************************
        plat pm exception
    ************************************************************************* */
    HI_ERR_CODE_PM_BASE                     = 10,
    HI_ERR_CODE_ALREADY_OPEN                = (HI_ERR_CODE_PM_BASE + 0),    /* 已经打开 */
    HI_ERR_CODE_ALREADY_CLOSE               = (HI_ERR_CODE_PM_BASE + 1),    /* 已经关闭 */

    /* *************************************************************************
        system exception
    ************************************************************************* */
    HI_ERR_CODE_SYS_BASE                     = 100,
    HI_ERR_CODE_PTR_NULL                     = (HI_ERR_CODE_SYS_BASE + 0),  /* 指针检查为空 */
    HI_ERR_CODE_ARRAY_OVERFLOW               = (HI_ERR_CODE_SYS_BASE + 1),  /* 数组下标越界 */
    HI_ERR_CODE_DIV_ZERO                     = (HI_ERR_CODE_SYS_BASE + 2),  /* 除0操作 */          /* 2: SYS_BASE + 2 */
    HI_ERR_CODE_ALLOC_MEM_FAIL               = (HI_ERR_CODE_SYS_BASE + 3),  /* 申请内存失败 */     /* 3: SYS_BASE + 3 */
    HI_ERR_CODE_FREE_MEM_FAIL                = (HI_ERR_CODE_SYS_BASE + 4),  /* 4: SYS_BASE offset 4 */
    HI_ERR_CODE_START_TIMRE_FAIL             = (HI_ERR_CODE_SYS_BASE + 5),  /* 启动定时器失败 */   /* 5: SYS_BASE + 5 */
    HI_ERR_CODE_RESET_INPROGRESS             = (HI_ERR_CODE_SYS_BASE + 6),  /* 复位处理中 */       /* 6: SYS_BASE + 6 */
    /* mac_device_struz找不到 */
    HI_ERR_CODE_MAC_DEVICE_NULL              = (HI_ERR_CODE_SYS_BASE + 7),  /* 7: SYS_BASE + 7 */
    HI_ERR_CODE_MAGIC_NUM_FAIL               = (HI_ERR_CODE_SYS_BASE + 8),  /* 魔法数字检测失败 */ /* 8: SYS_BASE + 8 */
    HI_ERR_CODE_NETBUF_INDEX_CHANGE          = (HI_ERR_CODE_SYS_BASE + 9),  /* netbuf 被篡改 */    /* 9: SYS_BASE + 9 */
    HI_ERR_CODE_CFG_REG_TIMEOUT              = (HI_ERR_CODE_SYS_BASE + 10), /* 配置寄存器超时 */ /* 10: SYS_BASE + 10 */
    HI_ERR_CODE_CFG_REG_ERROR                = (HI_ERR_CODE_SYS_BASE + 11), /* 配置寄存器错误 */ /* 11: SYS_BASE + 11 */
    /* 链表不为空，一般出现于链表清除动作后 */
    HI_ERR_CODE_LIST_NOT_EMPTY_ERROR         = (HI_ERR_CODE_SYS_BASE + 12), /* 12: SYS_BASE + 12 */

    /* system error最大值 */
	HI_ERR_SYS_BUTT                          = (HI_ERR_CODE_SYS_BASE + 199), /* 199: SYS_BASE + 199 */

    /* *************************************************************************
        resv func result
    ************************************************************************** */
    HI_ERR_CODE_RESV_FUNC_BASE               = 300,
    HI_ERR_CODE_RESV_FUNC_REPLACE            = (HI_ERR_CODE_RESV_FUNC_BASE + 0), /* 替代执行,原函数代码废弃 */
    HI_ERR_CODE_RESV_FUNC_ADD                = (HI_ERR_CODE_RESV_FUNC_BASE + 1), /* 新增执行,原函数代码保留 */

    HI_ERR_CODE_RESV_FUNC_BUTT               = (HI_ERR_CODE_RESV_FUNC_BASE + 99), /* 99: FUNC_BASE + 99 */

    /* *************************************************************************
        config exception
    ************************************************************************** */
    HI_ERR_CODE_CONFIG_BASE                 = 1000,
    HI_ERR_CODE_INVALID_CONFIG              = (HI_ERR_CODE_CONFIG_BASE + 0), /* 无效配置 */
    HI_ERR_CODE_CONFIG_UNSUPPORT            = (HI_ERR_CODE_CONFIG_BASE + 1), /* 配置不支持 */
    HI_ERR_CODE_CONFIG_EXCEED_SPEC          = (HI_ERR_CODE_CONFIG_BASE + 2), /* 配置超过规格 */ /* 2: CONFIG_BASE + 2 */
    HI_ERR_CODE_CONFIG_TIMEOUT              = (HI_ERR_CODE_CONFIG_BASE + 3), /* 配置超时 */     /* 3: CONFIG_BASE + 3 */
    HI_ERR_CODE_CONFIG_BUSY                 = (HI_ERR_CODE_CONFIG_BASE + 4), /* 4: CONFIG_BASE + 4 */
    /* HMAC和DMAC创建vap时的index不一致 */
    HI_ERR_CODE_ADD_VAP_INDX_UNSYNC         = (HI_ERR_CODE_CONFIG_BASE + 5), /* 5: CONFIG_BASE + 5 */
    /* HMAC和DMAC创建multi user时的index不一致 */
    HI_ERR_CODE_ADD_MULTI_USER_INDX_UNSYNC  = (HI_ERR_CODE_CONFIG_BASE + 6), /* 6: CONFIG_BASE + 6 */
    /* 用户资源已经释放，重复释放 */
    HI_ERR_CODE_USER_RES_CNT_ZERO           = (HI_ERR_CODE_CONFIG_BASE + 7), /* 7: CONFIG_BASE + 7 */

    /* 配置错误最大值 */
	HI_ERR_CODE_CONFIG_BUTT                 = (HI_ERR_CODE_CONFIG_BASE + 99), /* 99: CONFIG_BASE + 99 */

    /* *************************************************************************
        MSG exception
    ************************************************************************* */
    HI_ERR_CODE_MSG_BASE                  = 1100,
    HI_ERR_CODE_MSG_TYPE_ERR              = (HI_ERR_CODE_MSG_BASE + 0),   /* 消息类型解析错误 */
    HI_ERR_CODE_MSG_NOT_CMPTBL_WITH_STATE = (HI_ERR_CODE_MSG_BASE + 1),   /* 消息与所处的状态不一致 */
    HI_ERR_CODE_MSG_IE_MISS               = (HI_ERR_CODE_MSG_BASE + 2),   /* 消息IE缺失 */ /* 2: MSG_BASE + 2 */
    HI_ERR_CODE_MSG_IE_VALUE_ERR          = (HI_ERR_CODE_MSG_BASE + 3),   /* 消息IE的值错误 */ /* 3: MSG_BASE + 3 */
    /* ipc内部消息发送队列列已满 */
    HI_ERR_CODE_IPC_QUEUE_FULL            = (HI_ERR_CODE_MSG_BASE + 4),   /* 4: MSG_BASE + 4 */
    HI_ERR_CODE_MSG_NOT_FIND_STA_TAB      = (HI_ERR_CODE_MSG_BASE + 5),   /* 消息找不到状态表 */ /* 5: MSG_BASE + 5 */
    HI_ERR_CODE_MSG_NOT_FIND_ACT_TAB      = (HI_ERR_CODE_MSG_BASE + 6),   /* 消息找不到动作表 */ /* 6: MSG_BASE +6 */
    /* 消息对应的处理函数为NULL */
    HI_ERR_CODE_MSG_ACT_FUN_NULL          = (HI_ERR_CODE_MSG_BASE + 7),   /* 7: MSG_BASE + 7 */
    HI_ERR_CODE_MSG_LENGTH_ERR            = (HI_ERR_CODE_MSG_BASE + 8),   /* 消息长度错误 */ /* 8: MSG_BASE + 8 */

    HI_ERR_CODE_MSG_BUTT                  = (HI_ERR_CODE_MSG_BASE + 99),  /* 消息错误最大值 */ /* 99: MSG_BASE + 99 */

    /* *************************************************************************
        文件操作错误码
    ************************************************************************* */
    HI_ERR_CODE_FILE_BASE           = 1200,
    HI_ERR_CODE_OPEN_FILE_FAIL      = (HI_ERR_CODE_FILE_BASE + 0),
    HI_ERR_CODE_WRITE_FILE_FAIL     = (HI_ERR_CODE_FILE_BASE + 1),
    HI_ERR_CODE_READ_FILE_FAIL      = (HI_ERR_CODE_FILE_BASE + 2), /* 2: FILE_BASE + 2 */
    HI_ERR_CODE_CLOSE_FILE_FAIL     = (HI_ERR_CODE_FILE_BASE + 3), /* 3: FILE_BASE + 3 */

    HI_ERR_CODE_FILE_BUTT           = (HI_ERR_CODE_FILE_BASE + 99), /* 文件操作错误最大值 */ /* 99: FILE_BASE + 99 */

    /* *************************************************************************
        各模块自定义错误
     ************************************************************************* */
    /* *************************** 描述符操作错误 **************************** */
    HI_ERR_CODE_DSCR_BASE                     = 10000,
    HI_ERR_CODE_RX_DSCR_AMSDU_DISORDER        = (HI_ERR_CODE_DSCR_BASE + 0),  /* AMSDU对应的描述符乱序 */
    HI_ERR_CODE_RX_DSCR_LOSE                  = (HI_ERR_CODE_DSCR_BASE + 1),  /* 描述符域buf数目不对应 */

    HI_ERR_CODE_DSCR_BUTT                     = (HI_ERR_CODE_DSCR_BASE + 999), /* 999: DSCR_BASE + 999 */

    /* *************************************************************************
        各特性自定义错误,从20000开始，每个特性分配100个
     ************************************************************************* */
    /* *************************** AMSDU特性 ********************************** */
    HI_ERR_CODE_HMAC_AMSDU_BASE               = 20000,
    HI_ERR_CODE_HMAC_AMSDU_DISABLE            = (HI_ERR_CODE_HMAC_AMSDU_BASE + 0),  /* amsdu功能关闭 */
    HI_ERR_CODE_HMAC_MSDU_LEN_OVER            = (HI_ERR_CODE_HMAC_AMSDU_BASE + 1),

    HI_ERR_CODE_HMAC_AMSDU_BUTT               = (HI_ERR_CODE_HMAC_AMSDU_BASE + 999), /* 999: AMSDU_BASE + 999 */

    /* ******************************** 其它 ********************************* */
    /* *************************** 11i 特性 ********************************** */
    HI_ERR_CODE_SECURITY_BASE               = 21000,
    HI_ERR_CODE_SECURITY_KEY_TYPE           = (HI_ERR_CODE_SECURITY_BASE + 0),
    HI_ERR_CODE_SECURITY_KEY_LEN            = (HI_ERR_CODE_SECURITY_BASE + 1),
    HI_ERR_CODE_SECURITY_KEY_ID             = (HI_ERR_CODE_SECURITY_BASE + 2),  /* 2: SECURITY_BASE + 2 */
    HI_ERR_CODE_SECURITY_CHIPER_TYPE        = (HI_ERR_CODE_SECURITY_BASE + 3),  /* 3: SECURITY_BASE + 3 */
    HI_ERR_CODE_SECURITY_BUFF_NUM           = (HI_ERR_CODE_SECURITY_BASE + 4),  /* 4: SECURITY_BASE + 4 */
    HI_ERR_CODE_SECURITY_BUFF_LEN           = (HI_ERR_CODE_SECURITY_BASE + 5),  /* 5: SECURITY_BASE + 5 */
    HI_ERR_CODE_SECURITY_WRONG_KEY          = (HI_ERR_CODE_SECURITY_BASE + 6),  /* 6: SECURITY_BASE + 6 */
    HI_ERR_CODE_SECURITY_USER_INVAILD       = (HI_ERR_CODE_SECURITY_BASE + 7),  /* 7: SECURITY_BASE + 7 */
    HI_ERR_CODE_SECURITY_PARAMETERS         = (HI_ERR_CODE_SECURITY_BASE + 8),  /* 8: SECURITY_BASE + 8 */
    HI_ERR_CODE_SECURITY_AUTH_TYPE          = (HI_ERR_CODE_SECURITY_BASE + 9),  /* 9: SECURITY_BASE + 9 */
    HI_ERR_CODE_SECURITY_CAP                = (HI_ERR_CODE_SECURITY_BASE + 10), /* 10: SECURITY_BASE + 10 */
    HI_ERR_CODE_SECURITY_CAP_MFP            = (HI_ERR_CODE_SECURITY_BASE + 11), /* 11: SECURITY_BASE + 11 */
    HI_ERR_CODE_SECURITY_CAP_BSS            = (HI_ERR_CODE_SECURITY_BASE + 12), /* 12: SECURITY_BASE + 12 */
    HI_ERR_CODE_SECURITY_CAP_PHY            = (HI_ERR_CODE_SECURITY_BASE + 13), /* 13: SECURITY_BASE + 13 */
    HI_ERR_CODE_SECURITY_PORT_INVALID       = (HI_ERR_CODE_SECURITY_BASE + 14), /* 14: SECURITY_BASE + 14 */
    HI_ERR_CODE_SECURITY_MAC_INVALID        = (HI_ERR_CODE_SECURITY_BASE + 15), /* 15: SECURITY_BASE + 15 */
    HI_ERR_CODE_SECURITY_MODE_INVALID       = (HI_ERR_CODE_SECURITY_BASE + 16), /* 16: SECURITY_BASE + 16 */
    HI_ERR_CODE_SECURITY_LIST_FULL          = (HI_ERR_CODE_SECURITY_BASE + 17), /* 17: SECURITY_BASE + 17 */
    HI_ERR_CODE_SECURITY_AGING_INVALID      = (HI_ERR_CODE_SECURITY_BASE + 18), /* 18: SECURITY_BASE + 18 */
    HI_ERR_CODE_SECURITY_THRESHOLD_INVALID  = (HI_ERR_CODE_SECURITY_BASE + 19), /* 19: SECURITY_BASE + 19 */
    HI_ERR_CODE_SECURITY_RESETIME_INVALID   = (HI_ERR_CODE_SECURITY_BASE + 20), /* 20: SECURITY_BASE + 20 */
    HI_ERR_CODE_SECURITY_BUTT               = (HI_ERR_CODE_SECURITY_BASE + 99), /* 99: SECURITY_BASE + 99 */
    /* 错误码整理，先不删除原先的错误码，待全部上完库以后再删除 */
    HI_ERR_CODE_HMAC_SECURITY_BASE              = 21100,
    HI_ERR_CODE_HMAC_SECURITY_KEY_TYPE          = (HI_ERR_CODE_HMAC_SECURITY_BASE + 0),
    HI_ERR_CODE_HMAC_SECURITY_KEY_LEN           = (HI_ERR_CODE_HMAC_SECURITY_BASE + 1),
    HI_ERR_CODE_HMAC_SECURITY_KEY_ID            = (HI_ERR_CODE_HMAC_SECURITY_BASE + 2),   /* 2: SECURITY_BASE + 2 */
    HI_ERR_CODE_HMAC_SECURITY_CHIPER_TYPE       = (HI_ERR_CODE_HMAC_SECURITY_BASE + 3),   /* 3: SECURITY_BASE + 3 */
    HI_ERR_CODE_HMAC_SECURITY_BUFF_NUM          = (HI_ERR_CODE_HMAC_SECURITY_BASE + 4),   /* 4: SECURITY_BASE + 4 */
    HI_ERR_CODE_HMAC_SECURITY_BUFF_LEN          = (HI_ERR_CODE_HMAC_SECURITY_BASE + 5),   /* 5: SECURITY_BASE + 5 */
    HI_ERR_CODE_HMAC_SECURITY_WRONG_KEY         = (HI_ERR_CODE_HMAC_SECURITY_BASE + 6),   /* 6: SECURITY_BASE + 6 */
    HI_ERR_CODE_HMAC_SECURITY_USER_INVAILD      = (HI_ERR_CODE_HMAC_SECURITY_BASE + 7),   /* 7: SECURITY_BASE + 7 */
    HI_ERR_CODE_HMAC_SECURITY_PARAMETERS        = (HI_ERR_CODE_HMAC_SECURITY_BASE + 8),   /* 8: SECURITY_BASE + 8 */
    HI_ERR_CODE_HMAC_SECURITY_AUTH_TYPE         = (HI_ERR_CODE_HMAC_SECURITY_BASE + 9),   /* 9: SECURITY_BASE + 9 */
    HI_ERR_CODE_HMAC_SECURITY_CAP               = (HI_ERR_CODE_HMAC_SECURITY_BASE + 10),  /* 10: SECURITY_BASE + 10 */
    HI_ERR_CODE_HMAC_SECURITY_CAP_MFP           = (HI_ERR_CODE_HMAC_SECURITY_BASE + 11),  /* 11: SECURITY_BASE + 11 */
    HI_ERR_CODE_HMAC_SECURITY_CAP_BSS           = (HI_ERR_CODE_HMAC_SECURITY_BASE + 12),  /* 12: SECURITY_BASE + 12 */
    HI_ERR_CODE_HMAC_SECURITY_CAP_PHY           = (HI_ERR_CODE_HMAC_SECURITY_BASE + 13),  /* 13: SECURITY_BASE + 13 */
    HI_ERR_CODE_HMAC_SECURITY_PORT_INVALID      = (HI_ERR_CODE_HMAC_SECURITY_BASE + 14),  /* 14: SECURITY_BASE + 14 */
    HI_ERR_CODE_HMAC_SECURITY_MAC_INVALID       = (HI_ERR_CODE_HMAC_SECURITY_BASE + 15),  /* 15: SECURITY_BASE + 15 */
    HI_ERR_CODE_HMAC_SECURITY_MODE_INVALID      = (HI_ERR_CODE_HMAC_SECURITY_BASE + 16),  /* 16: SECURITY_BASE + 16 */
    HI_ERR_CODE_HMAC_SECURITY_LIST_FULL         = (HI_ERR_CODE_HMAC_SECURITY_BASE + 17),  /* 17: SECURITY_BASE + 17 */
    HI_ERR_CODE_HMAC_SECURITY_AGING_INVALID     = (HI_ERR_CODE_HMAC_SECURITY_BASE + 18),  /* 18: SECURITY_BASE + 18 */
    HI_ERR_CODE_HMAC_SECURITY_THRESHOLD_INVALID = (HI_ERR_CODE_HMAC_SECURITY_BASE + 19),  /* 19: SECURITY_BASE + 19 */
    HI_ERR_CODE_HMAC_SECURITY_RESETIME_INVALID  = (HI_ERR_CODE_HMAC_SECURITY_BASE + 20),  /* 20: SECURITY_BASE + 20 */
    HI_ERR_CODE_HMAC_SECURITY_BUTT              = (HI_ERR_CODE_HMAC_SECURITY_BASE + 499), /* 499: SECURITY_BASE + 499 */

    /* *************************** wapi 特性 ********************************** */
    HI_ERR_CODE_WAPI_BASE                          = 21600,
    HI_ERR_CODE_WAPI_NETBUFF_LEN_ERR               = (HI_ERR_CODE_WAPI_BASE + 0),
    HI_ERR_CODE_WAPI_DECRYPT_FAIL                  = (HI_ERR_CODE_WAPI_BASE + 1),
    HI_ERR_CODE_WAPI_MIC_CALC_FAIL                 = (HI_ERR_CODE_WAPI_BASE + 2),  /* 2: WAPI_BASE + 2 */
    HI_ERR_CODE_WAPI_ENRYPT_FAIL                   = (HI_ERR_CODE_WAPI_BASE + 3),  /* 3: WAPI_BASE + 3 */
    HI_ERR_CODE_WAPI_MIC_CMP_FAIL                  = (HI_ERR_CODE_WAPI_BASE + 4),  /* 4: WAPI_BASE + 4 */

    HI_ERR_CODE_WAPI_BUTT                          = (HI_ERR_CODE_WAPI_BASE + 99), /* 99: WAPI_BASE + 99 */
    /* ******************************** 其它 ********************************* */
    /* *************************** 11w 特性 ********************************** */
    HI_ERR_CODE_PMF_BASE                      = 22000,
    /* user的bit_pmf_active开关没有使能开启 */
    HI_ERR_CODE_PMF_ACTIVE_DOWN               = (HI_ERR_CODE_PMF_BASE + 0),
    /* hmac_send_sa_query_req函数发送sa query req操作失败 */
    HI_ERR_CODE_PMF_SA_QUERY_REQ_SEND_FAIL    = (HI_ERR_CODE_PMF_BASE + 1),
    /* dot11RSNAProtectedManagementFramesActivated 值为0 */
    HI_ERR_CODE_PMF_DISABLED                  = (HI_ERR_CODE_PMF_BASE + 2),   /* 2: PMF_BASE + 2 */
    /* hmac_start_sa_query函数返回结果失败 */
    HI_ERR_CODE_PMF_SA_QUERY_START_FAIL       = (HI_ERR_CODE_PMF_BASE + 3),   /* 3: PMF_BASE + 3 */
    /* hmac_sa_query_del_user函数,SA query流程删除用户失败 */
    HI_ERR_CODE_PMF_SA_QUERY_DEL_USER_FAIL    = (HI_ERR_CODE_PMF_BASE + 4),   /* 4: PMF_BASE + 4 */
    /* oal_crypto_aes_cmac_encrypt函数，AES_CMAC加密失败 */
    HI_ERR_CODE_PMF_BIP_AES_CMAC_ENCRYPT_FAIL = (HI_ERR_CODE_PMF_BASE + 5),   /* 5: PMF_BASE + 5 */
    /* dmac_bip_crypto函数，bip加密失败 */
    HI_ERR_CODE_PMF_BIP_CRIPTO_FAIL           = (HI_ERR_CODE_PMF_BASE + 6),   /* 6: PMF_BASE + 6 */
    /* oal_crypto_bip_demic函数，bip解密失败 */
    HI_ERR_CODE_PMF_BIP_DECRIPTO_FAIL         = (HI_ERR_CODE_PMF_BASE + 7),   /* 7: PMF_BASE + 7 */
    /* 给的igtk_index 错误 */
    HI_ERR_CODE_PMF_IGTK_INDX_FAIL            = (HI_ERR_CODE_PMF_BASE + 8),   /* 8: PMF_BASE + 8 */
    /* VAP的mfpc&mfpr设置错误 */
    HI_ERR_CODE_PMF_VAP_CAP_FAIL              = (HI_ERR_CODE_PMF_BASE + 9),   /* 9: PMF_BASE + 9 */
    /* VAP的mib dot11RSNAActived设置为OAL_FALES */
    HI_ERR_CODE_PMF_VAP_ACTIVE_DOWN           = (HI_ERR_CODE_PMF_BASE + 10),  /* 10: PMF_BASE + 10 */
    /* igtk不存在或者igtk_id值错误 */
    HI_ERR_CODE_PMF_IGTK_NOT_EXIST            = (HI_ERR_CODE_PMF_BASE + 11),  /* 11: PMF_BASE + 11 */
    /* bip加解密过程错误 */
    HI_ERR_CODE_PMF_ALIGN_ERR                 = (HI_ERR_CODE_PMF_BASE + 12),  /* 12: PMF_BASE + 12 */
    HI_ERR_CODE_PMF_REPLAY_ATTAC              = (HI_ERR_CODE_PMF_BASE + 13),  /* bip重放攻击 */ /* 13: PMF_BASE + 13 */
    /* bip完整性校验检查失败 */
    HI_ERR_CODE_PMF_MMIE_ERR                  = (HI_ERR_CODE_PMF_BASE + 14),  /* 14: PMF_BASE + 14 */
    /* PMF使能收到未加密的单播强健管理帧 */
    HI_ERR_CODE_PMF_NO_PROTECTED_ERROR        = (HI_ERR_CODE_PMF_BASE + 15),  /* 15: PMF_BASE + 15 */

    HI_ERR_CODE_PMF_BUTT                      = (HI_ERR_CODE_PMF_BASE + 999), /* 999: PMF_BASE + 999 */
    /* ******************************** 其它 ********************************* */
    /* **************hostapd/wpa_supplicant事件上报和下发特性 **************** */
    HI_ERR_CODE_CFG80211_BASE               = 23000,
    HI_ERR_CODE_CFG80211_SKB_MEM_FAIL       = (HI_ERR_CODE_CFG80211_BASE + 0),  /* skb不足无法存消息头和内容 */
    HI_ERR_CODE_CFG80211_EMSGSIZE           = (HI_ERR_CODE_CFG80211_BASE + 1),  /* 消息太长,属性无法填充 */
    HI_ERR_CODE_CFG80211_MCS_EXCEED         = (HI_ERR_CODE_CFG80211_BASE + 2),  /* MCS大于32 */
    HI_ERR_CODE_CFG80211_ENOBUFS            = (HI_ERR_CODE_CFG80211_BASE + 3),  /* 3: CFG80211_BASE + 3 */

    HI_ERR_CODE_CFG80211_BUTT               = (HI_ERR_CODE_CFG80211_BASE + 999), /* 999: CFG80211_BASE + 999 */

    /* ******************************** OAL ********************************* */
    HI_ERR_CODE_BASE                    = 24000,

    /* *************************** OAL --- 内存池 *************************** */
    HI_ERR_CODE_MEM_BASE                = (HI_ERR_CODE_BASE + 0),
    HI_ERR_CODE_MEM_GET_POOL_FAIL       = (HI_ERR_CODE_MEM_BASE + 0), /* 获取内存池全局指针失败 */
    HI_ERR_CODE_MEM_ALLOC_CTRL_BLK_FAIL = (HI_ERR_CODE_MEM_BASE + 1), /* 申请块内存失败 */
    /* 获取netbuf subpool id失败 */
    HI_ERR_CODE_MEM_SKB_SUBPOOL_ID_ERR  = (HI_ERR_CODE_MEM_BASE + 2), /* 2: MEM_BASE + 2 */
    HI_ERR_CODE_MEM_GET_CFG_TBL_FAIL    = (HI_ERR_CODE_MEM_BASE + 3), /* 获取内存池配置信息失败 */ /* 3: MEM_BASE + 3 */
    HI_ERR_CODE_MEM_EXCEED_MAX_LEN      = (HI_ERR_CODE_MEM_BASE + 4), /* 内存块最大长度超过限制 */ /* 4: MEM_BASE + 4 */
    HI_ERR_CODE_MEM_EXCEED_SUBPOOL_CNT  = (HI_ERR_CODE_MEM_BASE + 5), /* 子内存池个数超过限制 */ /* 5: MEM_BASE + 5 */
    HI_ERR_CODE_MEM_DOG_TAG             = (HI_ERR_CODE_MEM_BASE + 6), /* 内存被踩 */ /* 6: MEM_BASE + 6 */
    /* 释放了一块已经被释放的内存 */
    HI_ERR_CODE_MEM_ALREADY_FREE        = (HI_ERR_CODE_MEM_BASE + 7), /* 7: MEM_BASE + 7 */
    /* 释放一块引用计数为0的内存 */
    HI_ERR_CODE_MEM_USER_CNT_ERR        = (HI_ERR_CODE_MEM_BASE + 8), /* 8: MEM_BASE + 8 */
    /* 可用内存块数目超过整个子内存池总内存块数 */
    HI_ERR_CODE_MEM_EXCEED_TOTAL_CNT    = (HI_ERR_CODE_MEM_BASE + 9), /* 9: MEM_BASE + 9 */
    /* *************************** OAL --- 事件 *************************** */
    HI_ERR_CODE_EVENT_BASE              = (HI_ERR_CODE_BASE + 100), /* 100: CODE_BASE + 100 */
    HI_ERR_CODE_EVENT_Q_EMPTY           = (HI_ERR_CODE_EVENT_BASE + 0),
    HI_ERR_CODE_EVENT_BUTT                    = (HI_ERR_CODE_BASE + 999), /* 999: CODE_BASE + 999 */

    /* ******************************** OAM ********************************* */
    HI_ERR_CODE_OAM_BASE                    = 25000,

    /**************************** OAM --- event *****************************/
    HI_ERR_CODE_OAM_EVT_BASE               = (HI_ERR_CODE_OAM_BASE + 0),
    HI_ERR_CODE_OAM_EVT_USER_IDX_EXCEED    = (HI_ERR_CODE_OAM_EVT_BASE + 0),    /* 用户索引超过最大值 */
    HI_ERR_CODE_OAM_EVT_FRAME_DIR_INVALID  = (HI_ERR_CODE_OAM_EVT_BASE + 1),    /* 既不是发送流程，也不是接收流程 */
    /* 帧头长度异常 */
    HI_ERR_CODE_OAM_EVT_FR_HDR_LEN_INVALID = (HI_ERR_CODE_OAM_EVT_BASE + 2),    /* 2: EVT_BASE + 2 */
    /* 帧体(包括帧头)长度异常 */
    HI_ERR_CODE_OAM_EVT_FR_LEN_INVALID     = (HI_ERR_CODE_OAM_EVT_BASE + 3),    /* 3: EVT_BASE + 3 */
    /* 描述符长度异常 */
    HI_ERR_CODE_OAM_EVT_DSCR_LEN_INVALID   = (HI_ERR_CODE_OAM_EVT_BASE + 4),    /* 4: EVT_BASE + 4 */

    HI_ERR_CODE_OAM_BUTT                    = (HI_ERR_CODE_OAM_BASE + 999),     /* 999: EVT_BASE + 999 */

    /* ******************************** KeepAlive ********************************* */
    HI_ERR_CODE_KEEPALIVE_BASE             = 26000,
    /* *************************** KeepAlive --- event **************************** */
    HI_ERR_CODE_KEEPALIVE_CONFIG_VAP       = (HI_ERR_CODE_KEEPALIVE_BASE + 1),
    HI_ERR_CODE_KEEPALIVE_BIG_INTERVAL     = (HI_ERR_CODE_KEEPALIVE_BASE + 2), /* 2: KEEPALIVE_BASE + 2 */
    HI_ERR_CODE_KEEPALIVE_SMALL_LIMIT      = (HI_ERR_CODE_KEEPALIVE_BASE + 3), /* 3: KEEPALIVE_BASE + 3 */
    HI_ERR_CODE_KEEPALIVE_INVALID_VAP      = (HI_ERR_CODE_KEEPALIVE_BASE + 4), /* 4: KEEPALIVE_BASE + 4 */
    HI_ERR_CODE_KEEPALIVE_PTR_NULL         = (HI_ERR_CODE_KEEPALIVE_BASE + 5), /* 5: KEEPALIVE_BASE + 5 */

    HI_ERR_CODE_KEEPALIVE_BUTT             = (HI_ERR_CODE_KEEPALIVE_BASE + 999), /* 999: KEEPALIVE_BASE + 999 */

    /* PROXY ARP特性 COMP--skb处理完成; INCOMP--skb没有处理完成，留待后续处理 */
    HI_ERR_CODE_PROXY_ARP_BASE                     = 27000,
    HI_ERR_CODE_PROXY_ARP_INVLD_SKB_INCOMP         = (HI_ERR_CODE_PROXY_ARP_BASE + 0), /* 接收到的SKB异常 */
    HI_ERR_CODE_PROXY_ARP_LEARN_USR_NOTEXIST_COMP  = (HI_ERR_CODE_PROXY_ARP_BASE + 1), /* GARP源地址非本BSS */
    /* GARP源地址学习成功 */
    HI_ERR_CODE_PROXY_ARP_LEARN_USR_COMP           = (HI_ERR_CODE_PROXY_ARP_BASE + 2),  /* 2: ARP_BASE + 2 */
    /* 回 arp reply到本BSS */
    HI_ERR_CODE_PROXY_ARP_REPLY2BSS_COMP           = (HI_ERR_CODE_PROXY_ARP_BASE + 3),  /* 3: ARP_BASE + 3 */
    /* 回 arp reply到ETH */
    HI_ERR_CODE_PROXY_ARP_REPLY2ETH_COMP           = (HI_ERR_CODE_PROXY_ARP_BASE + 4),  /* 4: ARP_BASE + 4 */
    /* 创建SKB失败 */
    HI_ERR_CODE_PROXY_ARP_CREATE_FAIL_COMP         = (HI_ERR_CODE_PROXY_ARP_BASE + 5),  /* 5: ARP_BASE + 5 */
    /* 接收到的SKB异常 */
    HI_ERR_CODE_PROXY_ND_INVLD_SKB1_INCOMP         = (HI_ERR_CODE_PROXY_ARP_BASE + 6),  /* 6: ARP_BASE + 6 */
    /* 接收到的SKB异常 */
    HI_ERR_CODE_PROXY_ND_INVLD_SKB2_INCOMP         = (HI_ERR_CODE_PROXY_ARP_BASE + 7),  /* 7: ARP_BASE + 7 */
    /* 接收到组播的arp reply */
    HI_ERR_CODE_PROXY_ARP_REPLY_MCAST_COMP         = (HI_ERR_CODE_PROXY_ARP_BASE + 8),  /* 8: ARP_BASE + 8 */
    /* 接收到arp reply，转发 */
    HI_ERR_CODE_PROXY_ARP_REPLY_INCOMP             = (HI_ERR_CODE_PROXY_ARP_BASE + 9),  /* 9: ARP_BASE + 9 */
    /* 不是arp req或者reply */
    HI_ERR_CODE_PROXY_ARP_NOT_REQ_REPLY_INCOMP     = (HI_ERR_CODE_PROXY_ARP_BASE + 10), /* 10: ARP_BASE + 10 */
    /* 通过NS学习地址，ap发现该ns的源mac不存在 */
    HI_ERR_CODE_PROXY_ND_LEARN_USR_NOTEXIST_COMP   = (HI_ERR_CODE_PROXY_ARP_BASE + 11), /* 11: ARP_BASE + 11 */
    /* 通过NS学习地址，ap发现该ipv6地址已经记录在hash中 */
    HI_ERR_CODE_PROXY_ND_LEARN_USR_ALREADY_EXIST_INCOMP   = (HI_ERR_CODE_PROXY_ARP_BASE + 12), /* 12: ARP_BASE + 12 */
    /* 通过NS学习地址正常 */
    HI_ERR_CODE_PROXY_ND_LEARN_USR_SUCC_COMP      = (HI_ERR_CODE_PROXY_ARP_BASE + 13),  /* 13: ARP_BASE + 13 */
    /* 通过NS学习地址失败 */
    HI_ERR_CODE_PROXY_ND_LEARN_USR_FAIL_INCOMP    = (HI_ERR_CODE_PROXY_ARP_BASE + 14),  /* 14: ARP_BASE + 14 */
    /* 从NS的icmpv6 opt中获取ll失败 */
    HI_ERR_CODE_PROXY_ND_NS_OPT_INVLD_COMP        = (HI_ERR_CODE_PROXY_ARP_BASE + 15),  /* 15: ARP_BASE + 15 */
    /* NS icmpv6中的target ipv6地址不在hash表中 */
    HI_ERR_CODE_PROXY_ND_NS_FINDUSR_ERR_COMP      = (HI_ERR_CODE_PROXY_ARP_BASE + 16),  /* 16: ARP_BASE + 16 */
    /* 构造NA失败 */
    HI_ERR_CODE_PROXY_ND_NS_CREATE_NA_FAIL_COMP   = (HI_ERR_CODE_PROXY_ARP_BASE + 17),  /* 17: ARP_BASE + 17 */
    /* 收到NS后，AP正常回复NA到BSS */
    HI_ERR_CODE_PROXY_ND_NS_REPLY_NA2BSS_COMP     = (HI_ERR_CODE_PROXY_ARP_BASE + 18),  /* 18: ARP_BASE + 18 */
    /* 非法的NA */
    HI_ERR_CODE_PROXY_ND_NA_INVLD_COMP            = (HI_ERR_CODE_PROXY_ARP_BASE + 19),  /* 19: ARP_BASE + 19 */
    /* 从组播响应的NA中 icmpv6 opt获取ll失败 */
    HI_ERR_CODE_PROXY_ND_NA_MCAST_NOT_LLA_COMP    = (HI_ERR_CODE_PROXY_ARP_BASE + 20),  /* 20: ARP_BASE + 20 */
    /* 从单播响应的NA中 icmpv6 opt获取ll失败 */
    HI_ERR_CODE_PROXY_ND_NA_UCAST_NOT_LLA_INCOMP  = (HI_ERR_CODE_PROXY_ARP_BASE + 21),  /* 21: ARP_BASE + 21 */
    /* NA中携带的ipv6地址冲突 */
    HI_ERR_CODE_PROXY_ND_NA_DUP_ADDR_INCOMP       = (HI_ERR_CODE_PROXY_ARP_BASE + 22),  /* 22: ARP_BASE + 22 */
    /* NA中S标志为0 */
    HI_ERR_CODE_PROXY_ND_NA_UNSOLICITED_COMP      = (HI_ERR_CODE_PROXY_ARP_BASE + 23),  /* 23: ARP_BASE + 23 */
    /* NA中S标志为1 */
    HI_ERR_CODE_PROXY_ND_NA_SOLICITED_INCOMP      = (HI_ERR_CODE_PROXY_ARP_BASE + 24),  /* 24: ARP_BASE + 24 */
    /* 没有携带icmpv6 */
    HI_ERR_CODE_PROXY_ND_NOT_ICMPV6_INCOMP        = (HI_ERR_CODE_PROXY_ARP_BASE + 25),  /* 25: ARP_BASE + 25 */
    /* 不是NS或者NA */
    HI_ERR_CODE_PROXY_ND_ICMPV6_NOT_NSNA_INCOMP   = (HI_ERR_CODE_PROXY_ARP_BASE + 26),  /* 26: ARP_BASE + 26 */
    /* arp中的target ipv4地址不在hash表中 */
    HI_ERR_CODE_PROXY_ARP_FINDUSR_ERR_COMP        = (HI_ERR_CODE_PROXY_ARP_BASE + 27),  /* 27: ARP_BASE + 27 */
    /* 不属于proxy 处理的帧 */
    HI_ERR_CODE_PROXY_OTHER_INCOMP                = (HI_ERR_CODE_PROXY_ARP_BASE + 28),  /* 28: ARP_BASE + 28 */
    /* 收到NS后，AP正常回复NA到ETH */
    HI_ERR_CODE_PROXY_ND_NS_REPLY_NA2ETH_COMP     = (HI_ERR_CODE_PROXY_ARP_BASE + 29),  /* 29: ARP_BASE + 29 */
    HI_ERR_CODE_PROXY_ARP_BUTT                    = (HI_ERR_CODE_PROXY_ARP_BASE + 499), /* 499: ARP_BASE + 499 */

    /* PSM特性 */
    HI_ERR_CODE_PSM_BASE                          = 27500,
    HI_ERR_CODE_PS_QUEUE_OVERRUN                  = (HI_ERR_CODE_PSM_BASE + 0), /* ps队列满 */

    /* ******************************** 队列模块 ********************************* */
    HI_ERR_CODE_QUEUE_BASE                             = 28000,
    HI_ERR_CODE_QUEUE_CNT_ZERO                         = (HI_ERR_CODE_QUEUE_BASE + 0),    /* 队列为空 */

    /* ******************************** SWP CBB模块 ****************************** */
    HI_ERR_CODE_SWP_CBB_BASE                           = 28100,
    HI_ERR_CODE_SWP_CBB_ALREADY_ACTIVE                 = (HI_ERR_CODE_SWP_CBB_BASE + 0),  /* 当前CBB接口已经激活 */
    HI_ERR_CODE_SWP_CBB_INT_REGISTER_FAIL              = (HI_ERR_CODE_SWP_CBB_BASE + 1),  /* 中断处理函数注册失败 */
    /* 数据长度无效 */
    HI_ERR_CODE_SWP_CBB_LENGTH_INVALID                 = (HI_ERR_CODE_SWP_CBB_BASE + 2),  /* 2: CBB_BASE + 2 */
    /* SWP CBB RX或TX缓存已满 */
    HI_ERR_CODE_SWP_CBB_BUFFUR_FULL                    = (HI_ERR_CODE_SWP_CBB_BASE + 3),  /* 3: CBB_BASE + 3 */

    /* ******************************** Type A模块 ******************************* */
    HI_ERR_CODE_TYPE_A_BASE       = 28200,
    HI_ERR_CODE_UID_ERR           = (HI_ERR_CODE_TYPE_A_BASE  + 0),  /* UID 出错  */
    HI_ERR_TIME_OUT_TIMES_BEYOND  = (HI_ERR_CODE_TYPE_A_BASE  + 1),  /* 超时次数超出 */
    HI_ERR_LEVEL_BEYOND           = (HI_ERR_CODE_TYPE_A_BASE  + 2),  /* 级联次数超出 */ /* 2: TYPE_A_BASE + 2 */

    /* ******************************** Type A LISTEN NFC-DEP模块 ******************************* */
    HI_ERR_CODE_NFC_DEP_LISTEN_BASE                    = 28300,

    /* ******************************** Type A POLL NFC-DEP模块 ******************************* */
    HI_ERR_CODE_NFC_DEP_POLL_BASE                      = 28400,

    /* ******************************** NFC-DEP协议模块 ******************************* */
    HI_ERR_CODE_NFC_DEP_BASE                           = 28500,
    HI_ERR_CODE_NFC_DEP_FRAME_TYPE_ERR                 = (HI_ERR_CODE_NFC_DEP_BASE + 0),  /* 帧类型错误 */
    HI_ERR_CODE_NFC_DEP_FRAME_OPCODE_ERR               = (HI_ERR_CODE_NFC_DEP_BASE + 1),  /* 帧操作码错误 */
    /* DID 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_DID_ERR                 = (HI_ERR_CODE_NFC_DEP_BASE + 2),  /* 2: NFC_DEP_BASE + 2 */
    /* GEN INFO flag 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_GEN_INFO_FLAG_ERR       = (HI_ERR_CODE_NFC_DEP_BASE + 3),  /* 3: NFC_DEP_BASE + 3 */
    /* DSI 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_DSI_ERR                 = (HI_ERR_CODE_NFC_DEP_BASE + 4),  /* 4: NFC_DEP_BASE + 4 */
    /* DRI 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_DRI_ERR                 = (HI_ERR_CODE_NFC_DEP_BASE + 5),  /* 5: NFC_DEP_BASE + 5 */
    /* FSL 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_FSL_ERR                 = (HI_ERR_CODE_NFC_DEP_BASE + 6),  /* 6: NFC_DEP_BASE + 6 */
    /* MI 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_MI_ERR                  = (HI_ERR_CODE_NFC_DEP_BASE + 7),  /* 7: NFC_DEP_BASE + 7 */
    /* NAD 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_NAD_ERR                 = (HI_ERR_CODE_NFC_DEP_BASE + 8),  /* 8: NFC_DEP_BASE + 8 */
    /* PNI 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_PNI_ERR                 = (HI_ERR_CODE_NFC_DEP_BASE + 9),  /* 9: NFC_DEP_BASE + 9 */
    /* PAYLOAD 出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_PAYLOAD_ERR             = (HI_ERR_CODE_NFC_DEP_BASE + 10), /* 10: NFC_DEP_BASE + 10 */
    /* sens_res  出错 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_SENS_RES_ERR            = (HI_ERR_CODE_NFC_DEP_BASE + 11), /* 11: NFC_DEP_BASE + 11 */
    /* sens_res 解析，走tag1防冲突处理流程 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_TAG1_PLT_SUCC           = (HI_ERR_CODE_NFC_DEP_BASE + 12), /* 12: NFC_DEP_BASE + 12 */
    /* SDD_REQ 级联值错误 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_CL_ERR                  = (HI_ERR_CODE_NFC_DEP_BASE + 13), /* 13: NFC_DEP_BASE + 13 */
    /* NFCID错误 */
    HI_ERR_CODE_NFC_DEP_NFCID_ERR                      = (HI_ERR_CODE_NFC_DEP_BASE + 14), /* 14: NFC_DEP_BASE + 14 */
    /* Cascade标识错误 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_CASCADE_ERR             = (HI_ERR_CODE_NFC_DEP_BASE + 15), /* 15: NFC_DEP_BASE + 15 */
    /* BCC校验错误 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_BCC_ERR                 = (HI_ERR_CODE_NFC_DEP_BASE + 16), /* 16: NFC_DEP_BASE + 16 */
    /* CT位错误 */
    HI_ERR_CODE_NFC_DEP_TYPE_A_CT_ERR                  = (HI_ERR_CODE_NFC_DEP_BASE + 17), /* 17: NFC_DEP_BASE + 17 */

    /* ******************************** NFC CBB 模块********************************* */
    HI_ERR_CODE_NFC_CBB_BASE                           = 28600,
    HI_ERR_CODE_NFC_RX_CRC_ERR                         = (HI_ERR_CODE_NFC_CBB_BASE + 0),  /* CRC 错误 */
    HI_ERR_CODE_NFC_RX_PTY_ERR                         = (HI_ERR_CODE_NFC_CBB_BASE + 1),  /* PTY 错误 */
    /* BCC 错误 */
    HI_ERR_CODE_NFC_RX_BCC_ERR                         = (HI_ERR_CODE_NFC_CBB_BASE + 2),  /* 2: NFC_CBB_BASE + 2 */
    /* CRPLL 失锁 错误 */
    HI_ERR_CODE_NFC_CRPLL_UNLOCK_FLAG_ERR              = (HI_ERR_CODE_NFC_CBB_BASE + 3),  /* 3: NFC_CBB_BASE + 3 */
    /* FAILING EDGE 错误 */
    HI_ERR_CODE_NFC_LSTNA_FALLING_FALL_ERR             = (HI_ERR_CODE_NFC_CBB_BASE + 4),  /* 4: NFC_CBB_BASE + 4 */
    /* BUFF 错误 */
    HI_ERR_CODE_NFC_RX_BUFF_ERR                        = (HI_ERR_CODE_NFC_CBB_BASE + 5),  /* 5: NFC_CBB_BASE + 5 */
    /* FRAME TYPE 错误 */
    HI_ERR_CODE_NFC_RX_BUFF_FRAME_TYPE_ERR             = (HI_ERR_CODE_NFC_CBB_BASE + 6),  /* 6: NFC_CBB_BASE + 6 */
    /* INT_REGISTER_FAIL 错误 */
    HI_ERR_CODE_CBB_INT_REGISTER_FAIL                  = (HI_ERR_CODE_NFC_CBB_BASE + 7),  /* 7: NFC_CBB_BASE + 7 */
    /* Listen模式，数据发送超时 */
    HI_ERR_CODE_CBB_LSTN_RX2TX_TO                      = (HI_ERR_CODE_NFC_CBB_BASE + 8),  /* 8: NFC_CBB_BASE + 8 */
    /* type f Listen模式，接收数据对应的数据速率错误 */
    HI_ERR_CODE_NFC_RX_LSTN_RATE_ERR                   = (HI_ERR_CODE_NFC_CBB_BASE + 9),  /* 9: NFC_CBB_BASE + 9 */

    /* ******************************** 调度模块 ********************************* */
    HI_ERR_CODE_SCHED_BASE                             = 28700,
    HI_ERR_CODE_SCHED_FSM_EXCEPT_FUN_NULL              = (HI_ERR_CODE_SCHED_BASE + 0),  /* 状态机异常处理函数为NULL */
    HI_ERR_CODE_SCHED_FSM_STA_TAB_NULL                 = (HI_ERR_CODE_SCHED_BASE + 1),  /* 状态机状态表为NULL或无内容 */
    /* 插入队列ID无效 */
    HI_ERR_CODE_SCHED_PUSH_QUEUE_ID_INVALID            = (HI_ERR_CODE_SCHED_BASE + 2),  /* 2: SCHED_BASE + 2 */

    /* ******************************** Tag4B模块 ********************************* */
    HI_ERR_CODE_TAG4B_BASE                = 28800,
    HI_ERR_CODE_TAG4B_NOT_COMPLIANT_14443 = (HI_ERR_CODE_TAG4B_BASE + 0), /* 不兼容14443协议错误 */
    HI_ERR_CODE_TAG4B_OPCODE_ERR          = (HI_ERR_CODE_TAG4B_BASE + 1), /* ATTRIB操作码错误 */
    HI_ERR_CODE_TAG4B_TYPE_B_DID_ERR      = (HI_ERR_CODE_TAG4B_BASE + 2), /* DID错误 */   /* 2: TAG4B_BASE + 2 */
    HI_ERR_CODE_TAG4B_NFCID_ERR           = (HI_ERR_CODE_TAG4B_BASE + 3), /* NFCID错误 */ /* 3: TAG4B_BASE + 3 */
    HI_ERR_CODE_TAG4B_BR_ERR              = (HI_ERR_CODE_TAG4B_BASE + 4), /* 速率错误 */  /* 4: TAG4B_BASE + 4 */
    /* PARAM3 b8-b4不为0 */
    HI_ERR_CODE_TAG4B_PARAM3_MSB_ERR      = (HI_ERR_CODE_TAG4B_BASE + 5), /* 5: TAG4B_BASE + 5 */

    /* ******************************** ISO-DEP协议模块 ********************************* */
    HI_ERR_CODE_ISO_DEP_BASE                           = 28900,
    HI_ERR_CODE_ISO_DEP_IBLOCK_RETRY_ERR               = (HI_ERR_CODE_ISO_DEP_BASE + 0),  /* IBLOCK重传超过最大值错误 */
    /* 发送接收block长度大于FSC错误 */
    HI_ERR_CODE_ISO_DEP_OVER_FSC_ERR                   = (HI_ERR_CODE_ISO_DEP_BASE + 1),
    /* 发送接收block长度大于FSD错误 */
    HI_ERR_CODE_ISO_DEP_OVER_FSD_ERR                   = (HI_ERR_CODE_ISO_DEP_BASE + 2),  /* 2: ISO_DEP_BASE + 2 */
    /* BLOCK类型错误 */
    HI_ERR_CODE_ISO_DEP_BLOCK_TYPE_ERR                 = (HI_ERR_CODE_ISO_DEP_BASE + 3),  /* 3: ISO_DEP_BASE + 3 */
    /* DID错误 */
    HI_ERR_CODE_ISO_DEP_DID_ERR                        = (HI_ERR_CODE_ISO_DEP_BASE + 4),  /* 4: ISO_DEP_BASE + 4 */
    /* NAD错误 */
    HI_ERR_CODE_ISO_DEP_NAD_ERR                        = (HI_ERR_CODE_ISO_DEP_BASE + 5),  /* 5: ISO_DEP_BASE + 5 */
    /* BLOCK NUM错误 */
    HI_ERR_CODE_ISO_DEP_BN_ERR                         = (HI_ERR_CODE_ISO_DEP_BASE + 6),  /* 6: ISO_DEP_BASE + 6 */
    /* R_ACK重传超过最大值错误 */
    HI_ERR_CODE_ISO_DEP_ACK_RETRY_ERR                  = (HI_ERR_CODE_ISO_DEP_BASE + 7),  /* 7: ISO_DEP_BASE + 7 */
    /* R_NAK重传超过最大值错误 */
    HI_ERR_CODE_ISO_DEP_NAK_RETRY_ERR                  = (HI_ERR_CODE_ISO_DEP_BASE + 8),  /* 8: ISO_DEP_BASE + 8 */
    /* S_WTX重传超过最大值错误 */
    HI_ERR_CODE_ISO_DEP_WTX_RETRY_ERR                  = (HI_ERR_CODE_ISO_DEP_BASE + 9),  /* 9: ISO_DEP_BASE + 9 */
    /* S_DSL重传超过最大值错误 */
    HI_ERR_CODE_ISO_DEP_DSL_RETRY_ERR                  = (HI_ERR_CODE_ISO_DEP_BASE + 10), /* 10: ISO_DEP_BASE + 10 */
    /* PBC中fix num错误 */
    HI_ERR_CODE_ISO_DEP_PCB_FIX_NUM_ERR                = (HI_ERR_CODE_ISO_DEP_BASE + 11), /* 11: ISO_DEP_BASE + 11 */
    /* WTXM错误 */
    HI_ERR_CODE_ISO_DEP_WTXM_ERR                       = (HI_ERR_CODE_ISO_DEP_BASE + 12), /* 12: ISO_DEP_BASE + 12 */
    /* 协议错误 */
    HI_ERR_CODE_ISO_DEP_PROTOCOL_ERR                   = (HI_ERR_CODE_ISO_DEP_BASE + 13), /* 13: ISO_DEP_BASE + 13 */
    /* 不可恢复异常 */
    HI_ERR_CODE_ISO_DEP_UNRECOVERABLE_EXCEPTIOM        = (HI_ERR_CODE_ISO_DEP_BASE + 14), /* 14: ISO_DEP_BASE + 14 */

    /* ******************************** TYPE B协议模块 ********************************* */
    HI_ERR_CODE_TYPE_B_BASE                            = 29000,
    HI_ERR_CODE_CUR_SLOT_NUM_ERR                       = (HI_ERR_CODE_TYPE_B_BASE + 1),   /* 时间槽数错误 */
    /* 时间槽数量错误 */
    HI_ERR_CODE_SLOT_NUM_ERR                           = (HI_ERR_CODE_TYPE_B_BASE + 2),   /* 2: YPE_B_BASE + 2 */
    /* SENSB_RES操作码错误 */
    HI_ERR_CODE_TYPE_B_SENSB_RES_OPCODE_ERR            = (HI_ERR_CODE_TYPE_B_BASE + 3),   /* 3: YPE_B_BASE + 3 */
    /* AFI不一致的错误 */
    HI_ERR_CODE_TYPE_B_CR_AFI_ERR                      = (HI_ERR_CODE_TYPE_B_BASE + 4),   /* 4: YPE_B_BASE + 4 */
    /* did值超出范围 */
    HI_ERR_CODE_DID_OVER_ERR                           = (HI_ERR_CODE_TYPE_B_BASE + 5),   /* 5: YPE_B_BASE + 5 */
    /* FSD或者FSC取值错误 */
    HI_ERR_CODE_FSD_FSC_TR0_TR1_TR2_VALUE_ERR          = (HI_ERR_CODE_TYPE_B_BASE + 6),   /* 6: YPE_B_BASE + 6 */
    /* MBL取值不正确 */
    HI_ERR_CODE_MBL_ERR                                = (HI_ERR_CODE_TYPE_B_BASE + 7),   /* 7: YPE_B_BASE + 7 */
    /* ******************************** TAG4A模块 ********************************* */
    HI_ERR_CODE_TAG4A_BASE                             = 29100,
    HI_ERR_CODE_TAG4A_ATS_TL_ERR                       = (HI_ERR_CODE_SCHED_BASE + 0),    /* ATS TL错误 */
    HI_ERR_CODE_TAG4A_PPS_RES_ERR                      = (HI_ERR_CODE_SCHED_BASE + 1),    /* PPS_RES错误 */
    /* PPS_RES DID错误 */
    HI_ERR_CODE_TAG4A_PPS_DID_ERR                      = (HI_ERR_CODE_SCHED_BASE + 2),    /* 2: SCHED_BASE + 2 */
    /* RATS 帧头错误 */
    HI_ERR_CODE_TAG4A_RATS_OPCODE_ERR                  = (HI_ERR_CODE_SCHED_BASE + 3),    /* 3: SCHED_BASE + 3 */
    /* RATS DID错误 */
    HI_ERR_CODE_TAG4A_RATS_DID_ERR                     = (HI_ERR_CODE_SCHED_BASE + 4),    /* 4: SCHED_BASE + 4 */
    /* ******************************** TYPE F协议模块 ********************************* */
    HI_ERR_CODE_TYPE_F_BASE                            = 29200,
    HI_ERR_CODE_TYPE_F_SENSF_RES_OPCODE_ERR            = (HI_ERR_CODE_TYPE_F_BASE + 1),   /* SENSF_RES操作码错误 */
    /* SENSF_REQ操作码错误 */
    HI_ERR_CODE_TYPE_F_SENSF_REQ_OPCODE_ERR            = (HI_ERR_CODE_TYPE_F_BASE + 2),   /* 2: TYPE_F_BASE + 2 */
    /* SENSF_RES带有RD错误 */
    HI_ERR_CODE_TYPE_F_SENSF_RES_WITH_RD_ERR           = (HI_ERR_CODE_TYPE_F_BASE + 3),   /* 3: TYPE_F_BASE + 3 */
    /* ******************************** TAG3协议模块 ********************************* */
    HI_ERR_CODE_TAG3_BASE               = 29300,
    HI_ERR_CODE_TAG3_CUP_CMD_OPCODE_ERR = (HI_ERR_CODE_TAG3_BASE + 1),  /* CUP_CMD帧头错误 */
    HI_ERR_CODE_TAG3_CUP_RES_OPCODE_ERR = (HI_ERR_CODE_TAG3_BASE + 2),  /* CUP_RES帧头错误 */ /* 2: TAG3_BASE + 2 */
    HI_ERR_CODE_TAG3_PAYLOAD_ERR        = (HI_ERR_CODE_TAG3_BASE + 3),  /* PAYLOAD错误 */     /* 3: TAG3_BASE + 3 */

    /* ******************************** NCI协议RF DISCOVERY模块 ********************************* */
    HI_ERR_CODE_RF_DISCOVERY_BASE              = 29400,
    HI_ERR_CODE_RF_DISCOVERY_TECH_TYPE_ERR     = (HI_ERR_CODE_RF_DISCOVERY_BASE + 1), /* 技术类型错误 */
    /* 配置的模式错误 */
    HI_ERR_CODE_RF_DISCOVERY_MODE_ERR          = (HI_ERR_CODE_RF_DISCOVERY_BASE + 2), /* 2: RF_DISCOVERY_BASE + 2 */

    /* ******************************** TECH DETECT ACT模块 ********************************* */
    HI_ERR_CODE_TECH_DETECT_ACT_BASE                   = 29500,
    /* 技术类型都不尽兴侦测的错误 */
    HI_ERR_CODE_TECH_DETECT_ACT_TECH_TYPE_ERR          = (HI_ERR_CODE_TECH_DETECT_ACT_BASE + 1),

    /* ******************************** NCI协议模块********************************* */
    HI_ERR_CODE_NCI_BASE                 = 29600,
    HI_ERR_CODE_NCI_CONFIG_PARAM_INVALID = (HI_ERR_CODE_NCI_BASE + 1), /* 无效的参数 */
    HI_ERR_CODE_NCI_UNKNOWN_MSG          = (HI_ERR_CODE_NCI_BASE + 2), /* 不能识别的命令 */     /* 2: NCI_BASE + 2 */
    HI_ERR_CODE_NCI_PAYLOAD_ERR          = (HI_ERR_CODE_NCI_BASE + 3), /* PAYLOAD错误 */        /* 3: NCI_BASE + 3 */
    /* Dispatch中的函数为NULL */
    HI_ERR_CODE_NCI_DISPATCH_FUN_NULL    = (HI_ERR_CODE_NCI_BASE + 4), /* 4: NCI_BASE + 4 */
    HI_ERR_CODE_NCI_VAL_LEN_TOO_SHORT    = (HI_ERR_CODE_NCI_BASE + 5), /* 存储参数的空间不足 */ /* 5: NCI_BASE + 5 */
    /* 接收的消息组装后存不下 */
    HI_ERR_CODE_NCI_RECV_MSG_TOO_BIG     = (HI_ERR_CODE_NCI_BASE + 6), /* 6: NCI_BASE + 6 */
    HI_ERR_CODE_NCI_PARAM_ID_TOO_BIG     = (HI_ERR_CODE_NCI_BASE + 7), /* 参数的ID超出范围 */   /* 7: NCI_BASE + 7 */
    HI_ERR_CODE_NCI_GID_OID_INVALID      = (HI_ERR_CODE_NCI_BASE + 8), /* NCI消息的GID或OID无效 */ /* 8: NCI_BASE + 8 */
    /* 接收到的NCI Packet是无效的 */
    HI_ERR_CODE_NCI_PACKET_INVALID       = (HI_ERR_CODE_NCI_BASE + 9), /* 9: NCI_BASE + 9 */

    /* ******************************** SHDLC协议模块********************************* */
    HI_ERR_CODE_SHDLC_BASE                             = 29700,
    /* 接收到的帧类型与当前状态不符 */
    HI_ERR_RECV_FRAME_TYPE_DIF_FSM                     = (HI_ERR_CODE_SHDLC_BASE + 1),
    /* 接收到的RSET帧的payload长度超出限定范围 */
    HI_ERR_RECV_RSET_LENGTH                            = (HI_ERR_CODE_SHDLC_BASE + 2), /* 2: SHDLC_BASE + 2 */
    /* 接收到帧的类型未知 */
    HI_ERR_RECV_FRAME_TYPE_UNKNOWN                     = (HI_ERR_CODE_SHDLC_BASE + 3), /* 3: SHDLC_BASE + 3 */
    /* 接收到帧的I帧的payload长度超出限定范围 */
    HI_ERR_RECV_I_FRAME_LENGTH                         = (HI_ERR_CODE_SHDLC_BASE + 4), /* 4: SHDLC_BASE + 4 */

    /* ******************************** HW RESET 模块 ************************************ */
    HI_ERR_CODE_HW_RESET_BASE                          = 30600,
    HI_ERR_CODE_HW_RESET_PHY_SAVE_MEMALLOC             = (HI_ERR_CODE_HW_RESET_BASE + 0),
    HI_ERR_CODE_HW_RESET_MAC_SAVE_MEMALLOC             = (HI_ERR_CODE_HW_RESET_BASE + 1),
    HI_ERR_CODE_HW_RESET_MAC_SAVE_SIZELIMIT            = (HI_ERR_CODE_HW_RESET_BASE + 2), /* 2: HW_RESET_BASE + 2 */
    HI_ERR_CODE_HW_RESET_PHY_SAVE_SIZELIMIT            = (HI_ERR_CODE_HW_RESET_BASE + 3), /* 3: HW_RESET_BASE + 3 */
    /* reset流程中申请tx fake queue失败 */
    HI_ERR_CODE_HW_RESET_TX_QUEUE_MEMALLOC             = (HI_ERR_CODE_HW_RESET_BASE + 4), /* 4: HW_RESET_BASE + 4 */
    /* ******************************** MESH 模块 ************************************ */
    HI_ERR_CODE_MESH_BASE = 31000,
    HI_ERR_CODE_MESH_NOT_ACCEPT_PEER = (HI_ERR_CODE_MESH_BASE + 0),  /* 当前MESH VAP仍无法接受配对连接 */
    HI_ERR_CODE_MESH_NOT_MESH_USER = (HI_ERR_CODE_MESH_BASE + 1),    /* 收到Mesh Action帧时检查用户发现不是Mesh用户 */

    HI_ERR_CODE_BUTT
} hi_err_code_enum;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#include "hi_types.h"
#endif /* end of oal_err_wifi.h */
