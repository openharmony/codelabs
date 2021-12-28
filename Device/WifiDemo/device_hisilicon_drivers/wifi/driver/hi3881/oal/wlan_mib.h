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

#ifndef __WLAN_MIB_H__
#define __WLAN_MIB_H__

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hi_types.h"
#include "oal_err_wifi.h"
#include "wlan_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#define WLAN_MIB_TOKEN_STRING_MAX_LENGTH    32    /* 支持与网管兼容的token的字符串最长长度，此定义后续可能与版本相关 */
#define WLAN_HT_MCS_BITMASK_LEN             10    /* MCS bitmask长度为77位，加上3个保留位 */

/* Number of Cipher Suites Implemented */
#define WLAN_PAIRWISE_CIPHER_SUITES         2
#define WLAN_AUTHENTICATION_SUITES          2

#define MAC_PAIRWISE_CIPHER_SUITES_NUM      2   /* 表征WPA2有CCMP和TKIP两种加密套件，WPA有WEP和WEP104两种加密套件 */
#define MAC_AUTHENTICATION_SUITE_NUM        2   /* 表示RSN 认证方法数 */

/* ****************************************************************************
  3 枚举定义
**************************************************************************** */
/* RowStatus ::= TEXTUAL-CONVENTION                                         */
/* The status column has six  values:`active', `notInService',`notReady',   */
/* `createAndGo', `createAndWait', `destroy'  as described in   rfc2579     */
typedef enum {
    WLAN_MIB_ROW_STATUS_ACTIVE = 1,
    WLAN_MIB_ROW_STATUS_NOT_INSERVICE,
    WLAN_MIB_ROW_STATUS_NOT_READY,
    WLAN_MIB_ROW_STATUS_CREATE_AND_GO,
    WLAN_MIB_ROW_STATUS_CREATE_AND_WAIT,
    WLAN_MIB_ROW_STATUS_DEATROY,

    WLAN_MIB_ROW_STATUS_BUTT
} wlan_mib_row_status_enum;
typedef hi_u8 wlan_mib_row_status_enum_uint8;

typedef enum {
    WLAN_MIB_PWR_MGMT_MODE_ACTIVE     = 1,
    WLAN_MIB_PWR_MGMT_MODE_PWRSAVE    = 2,

    WLAN_MIB_PWR_MGMT_MODE_BUTT
} wlan_mib_pwr_mgmt_mode_enum;
typedef hi_u8 wlan_mib_pwr_mgmt_mode_enum_uint8;

/* dot11AuthenticationAlgorithm INTEGER, {openSystem : (1),sharedKey : (2), */
/* fastBSSTransition : (3),simultaneousAuthEquals : (4) }                   */
typedef enum {
    WLAN_MIB_AUTH_ALG_OPEN_SYS          = 1,
    WLAN_MIB_AUTH_ALG_SHARE_KEY         = 2,
    WLAN_MIB_AUTH_ALG_FAST_BSS_TRANS    = 3,
    WLAN_MIB_AUTH_ALG_SIM_AUTH_EQUAL    = 4,

    WLAN_MIB_AUTH_ALG_BUTT
} wlan_mib_auth_alg_enum;
typedef hi_u8 wlan_mib_auth_alg_enum_uint8;

/* dot11DesiredBSSType INTEGER, {infrastructure : (1), independent : (2), any : (3) } */
typedef enum {
    WLAN_MIB_DESIRED_BSSTYPE_INFRA       = 1,
    WLAN_MIB_DESIRED_BSSTYPE_INDEPENDENT = 2,
    WLAN_MIB_DESIRED_BSSTYPE_ANY         = 3,

    WLAN_MIB_DESIRED_BSSTYPE_BUTT
} wlan_mib_desired_bsstype_enum;
typedef hi_u8 wlan_mib_desired_bsstype_enum_uint8;

/* dot11RSNAConfigGroupRekeyMethod  INTEGER { disabled : (1), timeBased : (2),  */
/* packetBased : (3), timepacketBased : (4)  */
typedef enum {
    WLAN_MIB_RSNACFG_GRPREKEY_DISABLED = 1,
    WLAN_MIB_RSNACFG_GRPREKEY_TIMEBASED = 2,
    WLAN_MIB_RSNACFG_GRPREKEY_PACKETBASED = 3,
    WLAN_MIB_RSNACFG_GRPREKEY_TIMEPACKETBASED = 4,

    WLAN_MIB_RSNACFG_GRPREKEY_BUTT
} wlan_mib_rsna_cfg_grp_rekey_enum;
typedef hi_u8 wlan_mib_rsna_cfg_grp_rekey_enum_uint8;

/* dot11RMRqstType INTEGER {channelLoad : (3),noiseHistogram : (4),beacon : (5),  */
/* frame : (6),staStatistics : (7),lci : (8),transmitStream : (9),pause : (255) } */
typedef enum {
    WLAN_MIB_RMRQST_TYPE_CH_LOAD         = 3,
    WLAN_MIB_RMRQST_TYPE_NOISE_HISTOGRAM = 4,
    WLAN_MIB_RMRQST_TYPE_BEACON          = 5,
    WLAN_MIB_RMRQST_TYPE_FRAME           = 6,
    WLAN_MIB_RMRQST_TYPE_STA_STATISTICS  = 7,
    WLAN_MIB_RMRQST_TYPE_LCI             = 8,
    WLAN_MIB_RMRQST_TYPE_TRANS_STREAM    = 9,
    WLAN_MIB_RMRQST_TYPE_PAUSE           = 255,

    WLAN_MIB_RMRQST_TYPE_BUTT
} wlan_mib_rmrqst_type_enum;
typedef hi_u16 wlan_mib_rmrqst_type_enum_uint16;

/* dot11RMRqstBeaconRqstMode INTEGER{  passive(0), active(1),beaconTable(2) */
typedef enum {
    WLAN_MIB_RMRQST_BEACONRQST_MODE_PASSIVE      = 0,
    WLAN_MIB_RMRQST_BEACONRQST_MODE_ACTIVE       = 1,
    WLAN_MIB_RMRQST_BEACON_MODE_BEACON_TABLE = 2,

    WLAN_MIB_RMRQST_BEACONRQST_MODE_BUTT
} wlan_mib_rmrqst_beaconrqst_mode_enum;
typedef hi_u8 wlan_mib_rmrqst_beaconrqst_mode_enum_uint8;

/* dot11RMRqstBeaconRqstDetail INTEGER {noBody(0),fixedFieldsAndRequestedElements(1),allBody(2) } */
typedef enum {
    WLAN_MIB_RMRQST_BEACONRQST_DETAIL_NOBODY           = 0,
    WLAN_MIB_RMRQST_BEACONRQST_DETAIL_FIXED_FLDANDELMT = 1,
    WLAN_MIB_RMRQST_BEACONRQST_DETAIL_ALLBODY          = 2,
    WLAN_MIB_RMRQST_BEACONRQST_DETAIL_BUTT
} wlan_mib_rmrqst_beaconrqst_detail_enum;
typedef hi_u8 wlan_mib_rmrqst_beaconrqst_detail_enum_uint8;

/* dot11RMRqstFrameRqstType INTEGER { frameCountRep(1) } */
typedef enum {
    WLAN_MIB_RMRQST_FRAMERQST_TYPE_FRAME_COUNTREP = 1,

    WLAN_MIB_RMRQST_FRAMERQST_TYPE_BUTT
} wlan_mib_rmrqst_famerqst_type_enum;
typedef hi_u8 wlan_mib_rmrqst_famerqst_type_enum_uint8;

/* dot11RMRqstBeaconReportingCondition INTEGER {afterEveryMeasurement : (0), */
/* rcpiAboveAbsoluteThreshold : (1),rcpiBelowAbsoluteThreshold : (2),        */
/* rsniAboveAbsoluteThreshold : (3),rsniBelowAbsoluteThreshold : (4),        */
/* rcpiAboveOffsetThreshold : (5),rcpiBelowOffsetThreshold : (6),            */
/* rsniAboveOffsetThreshold : (7),rsniBelowOffsetThreshold : (8),            */
/* rcpiInBound : (9),rsniInBound : (10) }                                    */
typedef enum {
    WLAN_MIB_RMRQST_BEACONRPT_CDT_AFTER_EVERY_MEAS            = 0,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCPI_ABOVE_ABS_THRESHOLD    = 1,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCPI_BELOW_ABS_THRESHOLD    = 2,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCNI_ABOVE_ABS_THRESHOLD    = 3,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCNI_BELOW_ABS_THRESHOLD    = 4,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCPI_ABOVE_OFFSET_THRESHOLD = 5,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCPI_BELOW_OFFSET_THRESHOLD = 6,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCNI_ABOVE_OFFSET_THRESHOLD = 7,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCNI_BELOW_OFFSET_THRESHOLD = 8,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCPI_IN_BOUND               = 9,
    WLAN_MIB_RMRQST_BEACONRPT_CDT_RCNI_IN_BOUND               = 10,

    WLAN_MIB_RMRQST_BEACONRPT_CDT_BUTT
} wlan_mib_rmrqst_beaconrpt_cdt_enum;
typedef hi_u8 wlan_mib_rmrqst_beaconrpt_cdt_enum_uint8;

/* dot11RMRqstSTAStatRqstGroupID OBJECT-TYPE                             */
/* SYNTAX INTEGER {                                                      */
/* dot11CountersTable(0),dot11MacStatistics(1),                          */
/* dot11QosCountersTableforUP0(2),dot11QosCountersTableforUP1(3),        */
/* dot11QosCountersTableforUP2(4),dot11QosCountersTableforUP3(5),        */
/* dot11QosCountersTableforUP4(6),dot11QosCountersTableforUP5(7),        */
/* dot11QosCountersTableforUP6(8),dot11QosCountersTableforUP7(9),        */
/* bSSAverageAccessDelays(10),dot11CountersGroup3Tablefor31(11),         */
/* dot11CountersGroup3Tablefor32(12),dot11CountersGroup3Tablefor33(13),  */
/* dot11CountersGroup3Tablefor34(14),dot11CountersGroup3Tablefor35(15),  */
/* dot11RSNAStatsTable(16)}                                              */
typedef enum {
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_COUNTER_TABLE          = 0,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_MAC_STATS              = 1,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_QOS_COUNTER_TABLE_UP0  = 2,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_QOS_COUNTER_TABLE_UP1  = 3,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_QOS_COUNTER_TABLE_UP2  = 4,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_QOS_COUNTER_TABLE_UP3  = 5,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_QOS_COUNTER_TABLE_UP4  = 6,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_QOS_COUNTER_TABLE_UP5  = 7,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_QOS_COUNTER_TABLE_UP6  = 8,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_QOS_COUNTER_TABLE_UP7  = 9,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_BSS_AVERG_ACCESS_DELAY = 10,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_COUNTER_GRP3_FOR31     = 11,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_COUNTER_GRP3_FOR32     = 12,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_COUNTER_GRP3_FOR33     = 13,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_COUNTER_GRP3_FOR34     = 14,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_COUNTER_GRP3_FOR35     = 15,
    WLAN_MIB_RMRQST_STASTATRQST_GRPID_RSNA_STATS_TABLE       = 16,

    WLAN_MIB_RMRQST_STASTATRQST_GRPID_BUTT
} wlan_mib_rmrqst_stastatrqst_grpid_enum;
typedef hi_u8 wlan_mib_rmrqst_stastatrqst_grpid_enum_uint8;

/* dot11RMRqstLCIRqstSubject OBJECT-TYPE  */
/* SYNTAX INTEGER { local(0), remote(1) } */
typedef enum {
    WLAN_MIB_RMRQST_LCIRQST_SUBJECT_LOCAL  = 0,
    WLAN_MIB_RMRQST_LCIRQST_SUBJECT_REMOTE = 1,

    WLAN_MIB_RMRQST_LCIRQST_SUBJECT_BUTT
} wlan_mib_rmrqst_lcirpt_subject_enum;
typedef hi_u8 wlan_mib_rmrqst_lcirpt_subject_enum_uint8;

/* dot11RMRqstLCIAzimuthType OBJECT-TYPE                  */
/* SYNTAX INTEGER { frontSurfaceofSta(0), radioBeam(1) }  */
typedef enum {
    WLAN_MIB_RMRQST_LCIAZIMUTH_TYPE_FRONT_SURFACE_STA  = 0,
    WLAN_MIB_RMRQST_LCIAZIMUTH_TYPE_RADIO_BEAM         = 1,

    WLAN_MIB_RMRQST_LCIAZIMUTH_TYPE_BUTT
} wlan_mib_rmrqst_lciazimuth_type_enum;
typedef hi_u8 wlan_mib_rmrqst_lciazimuth_type_enum_uint8;

/* dot11RMRqstChannelLoadReportingCondition OBJECT-TYPE   */
/* SYNTAX INTEGER {                                       */
/* afterEveryMeasurement(0),                              */
/* chanLoadAboveReference(1),                             */
/* chanLoadBelowReference(2) }                            */
typedef enum {
    WLAN_MIB_RMRQST_CH_LOADRPT_CDT_AFTER_EVERY_MEAS  = 0,
    WLAN_MIB_RMRQST_CH_LOADRPT_CDT_CH_LOAD_ABOVE_REF = 1,
    WLAN_MIB_RMRQST_CH_LOADRPT_CDT_CH_LOAD_BELOW_REF = 2,

    WLAN_MIB_RMRQST_CH_LOADRPT_CDT_BUTT
} wlan_mib_rmrqst_ch_loadrpt_cdt_type_enum;
typedef hi_u8 wlan_mib_rmrqst_ch_loadrpt_cdt_type_enum_uint8;

/* dot11RMRqstNoiseHistogramReportingCondition OBJECT-TYPE  */
/* SYNTAX INTEGER {                                         */
/* afterEveryMeasurement(0),                                */
/* aNPIAboveReference(1),                                   */
/* aNPIBelowReference(2) }                                  */
typedef enum {
    WLAN_MIB_RMRQST_NOISE_HISTGRPT_CDT_AFTER_EVERY_MEAS = 0,
    WLAN_MIB_RMRQST_NOISE_HISTGRPT_CDT_ANPI_ABOVE_REF   = 1,
    WLAN_MIB_RMRQST_NOISE_HISTGRPT_CDT_ANPI_BELOW_REF   = 2,

    WLAN_MIB_RMRQST_NOISE_HISTGRPT_CDT_BUTT
} wlan_mib_rmrqst_noise_histgrpt_cdt_type_enum;
typedef hi_u8 wlan_mib_rmrqst_noise_histgrpt_cdt_type_enum_uint8;

/* dot11LCIDSEAltitudeType OBJECT-TYPE              */
/* SYNTAX INTEGER { meters(1), floors(2), hagm(3) } */
typedef enum {
    WLAN_MIB_LCI_DSEALTITUDE_TYPE_METERS = 1,
    WLAN_MIB_LCI_DSEALTITUDE_TYPE_FLOORS = 2,
    WLAN_MIB_LCI_DSEALTITUDE_TYPE_HAGM   = 3,

    WLAN_MIB_LCI_DSEALTITUDE_TYPE_BUTT
} wlan_mib_lci_dsealtitude_type_enum;
typedef hi_u8 wlan_mib_lci_dsealtitude_type_enum_uint8;

/* dot11MIMOPowerSave OBJECT-TYPE                    */
/* SYNTAX INTEGER { static(1), dynamic(2), mimo(3) } */
typedef enum {
    WLAN_MIB_MIMO_POWER_SAVE_STATIC  = 1,
    WLAN_MIB_MIMO_POWER_SAVE_DYNAMIC = 2,
    WLAN_MIB_MIMO_POWER_SAVE_MIMO    = 3,

    WLAN_MIB_MIMO_POWER_SAVE_BUTT
} wlan_mib_mimo_power_save_enum;
typedef hi_u8 wlan_mib_mimo_power_save_enum_uint8;

/* dot11MaxAMSDULength OBJECT-TYPE            */
/* SYNTAX INTEGER { short(3839), long(7935) } */
typedef enum {
    WLAN_MIB_MAX_AMSDU_LENGTH_SHORT = 3839,
    WLAN_MIB_MAX_AMSDU_LENGTH_LONG  = 7935,

    WLAN_MIB_MAX_AMSDU_LENGTH_BUTT
} wlan_mib_max_amsdu_lenth_enum;
typedef hi_u16 wlan_mib_max_amsdu_lenth_enum_uint16;

/* dot11MCSFeedbackOptionImplemented OBJECT-TYPE         */
/* SYNTAX INTEGER { none(0), unsolicited (2), both (3) } */
typedef enum {
    WLAN_MIB_MCS_FEEDBACK_OPT_IMPLT_NONE = 0,
    WLAN_MIB_MCS_FEEDBACK_OPT_IMPLT_UNSOLICITED = 2,
    WLAN_MIB_MCS_FEEDBACK_OPT_IMPLT_BOTH        = 3,

    WLAN_MIB_MCS_FEEDBACK_OPT_IMPLT_BUTT
} wlan_mib_mcs_feedback_opt_implt_enum;
typedef hi_u8 wlan_mib_mcs_feedback_opt_implt_enum_uint8;

/* dot11LocationServicesLIPReportIntervalUnits OBJECT-TYPE */
/* SYNTAX INTEGER {                                        */
/* hours(0),                                               */
/* minutes(1),                                             */
/* seconds(2),                                             */
/* milliseconds(3)                                         */
typedef enum {
    WLAN_MIB_LCT_SERVS_LIPRPT_INTERVAL_UNIT_HOURS      = 0,
    WLAN_MIB_LCT_SERVS_LIPRPT_INTERVAL_UNIT_MINUTES    = 1,
    WLAN_MIB_LCT_SERVS_LIPRPT_INTERVAL_UNIT_SECONDS    = 2,
    WLAN_MIB_LCT_SERVS_LIPRPT_INTERVAL_UNIT_MILLISECDS = 3,

    WLAN_MIB_LCT_SERVS_LIPRPT_INTERVAL_UNIT_BUTT
} wlan_mib_lct_servs_liprpt_interval_unit_enum;
typedef hi_u8 wlan_mib_lct_servs_liprpt_interval_unit_enum_uint8;

/* dot11WirelessMGTEventType OBJECT-TYPE */
/* SYNTAX INTEGER {                      */
/* transition(0),                        */
/* rsna(1),                              */
/* peerToPeer(2),                        */
/* wnmLog(3),                            */
/* vendorSpecific(221)                   */
typedef enum {
    WLAN_MIB_WIRELESS_MGT_EVENT_TYPE_TRANSITION      = 0,
    WLAN_MIB_WIRELESS_MGT_EVENT_TYPE_RSNA            = 1,
    WLAN_MIB_WIRELESS_MGT_EVENT_TYPE_PEERTOPEER      = 2,
    WLAN_MIB_WIRELESS_MGT_EVENT_TYPE_WNMLOG          = 3,
    WLAN_MIB_WIRELESS_MGT_EVENT_TYPE_VENDOR_SPECIFIC = 221,

    WLAN_MIB_WIRELESS_MGT_EVENT_TYPE_BUTT
} wlan_mib_wireless_mgt_event_type_enum;
typedef hi_u8 wlan_mib_wireless_mgt_event_type_enum_uint8;

/* dot11WirelessMGTEventStatus OBJECT-TYPE */
/* SYNTAX INTEGER {                        */
/* successful(0),                          */
/* requestFailed(1),                       */
/* requestRefused(2),                      */
/* requestIncapable(3),                    */
/* detectedFrequentTransition(4)           */
typedef enum {
    WLAN_MIB_WIRELESS_MGT_EVENT_STATUS_SUCC                = 0,
    WLAN_MIB_WIRELESS_MGT_EVENT_STATUS_RQST_FAIL           = 1,
    WLAN_MIB_WIRELESS_MGT_EVENT_STATUS_RQST_REFUSE         = 2,
    WLAN_MIB_WIRELESS_MGT_EVENT_STATUS_RQST_INCAP          = 3,
    WLAN_MIB_WIRELESS_MGT_EVENT_STATUS_DETECT_FREQ_TRANSIT = 4,

    WLAN_MIB_WIRELESS_MGT_EVENT_STATUS_BUTT
} wlan_mib_wireless_mgt_event_status_enum;
typedef hi_u8 wlan_mib_wireless_mgt_event_status_enum_uint8;

/* dot11WirelessMGTEventTransitionReason OBJECT-TYPE   */
/* SYNTAX INTEGER {                                    */
/* unspecified(0),                                     */
/* excessiveFrameLossRatesPoorConditions(1),           */
/* excessiveDelayForCurrentTrafficStreams(2),          */
/* insufficientQosCapacityForCurrentTrafficStreams(3), */
/* firstAssociationToEss(4),                           */
/* loadBalancing(5),                                   */
/* betterApFound(6),                                   */
/* deauthenticatedDisassociatedFromPreviousAp(7),      */
/* certificateToken(8),                                */
/* apFailedIeee8021XEapAuthentication(9),              */
/* apFailed4wayHandshake(10),                          */
/* excessiveDataMICFailures(11),                       */
/* exceededFrameTransmissionRetryLimit(12),            */
/* ecessiveBroadcastDisassociations(13),               */
/* excessiveBroadcastDeauthentications(14),            */
/* previousTransitionFailed(15)                        */
typedef enum {
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_UNSPEC                           = 0,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_EXCES_FRAME_LOSSRATE_POORCDT     = 1,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_EXCES_DELAY_CURRT_TRAFIC_STRMS   = 2,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_INSUF_QOS_CAP_CURRT_TRAFIC_STRMS = 3,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_FIRST_ASSO_ESS                   = 4,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_LOAD_BALANCE                     = 5,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_BETTER_AP_FOUND                  = 6,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_DEAUTH_DISASSO_FROM_PRE_AP       = 7,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_CERTIF_TOKEN                     = 8,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_AP_FAIL_IEEE8021X_EAP_AUTH       = 9,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_AP_FAIL_4WAY_HANDSHAKE           = 10,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_EXCES_DATA_MIC_FAIL              = 11,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_EXCEED_FRAME_TRANS_RETRY_LIMIT   = 12,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_EXCES_BROAD_DISASSO              = 13,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_EXCES_BROAD_DISAUTH              = 14,
    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_PRE_TRANSIT_FAIL                 = 15,

    WLAN_MIB_WIRELESS_MGT_EVENT_TRANSIT_REASON_BUTT
} wlan_mib_wireless_mgt_event_transit_reason_enum;
typedef hi_u8 wlan_mib_wireless_mgt_event_transit_reason_enum_uint8;

/* dot11WNMRqstType OBJECT-TYPE */
/* SYNTAX INTEGER {             */
/* mcastDiagnostics(0),         */
/* locationCivic(1),            */
/* locationIdentifier(2),       */
/* event(3),                    */
/* dignostic(4),                */
/* locationConfiguration(5),    */
/* bssTransitionQuery(6),       */
/* bssTransitionRqst(7),        */
/* fms(8),                      */
/* colocInterference(9)         */
typedef enum {
    WLAN_MIB_WNM_RQST_TYPE_MCAST_DIAG        = 0,
    WLAN_MIB_WNM_RQST_TYPE_LOCATION_CIVIC    = 1,
    WLAN_MIB_WNM_RQST_TYPE_LOCATION_IDTIF    = 2,
    WLAN_MIB_WNM_RQST_TYPE_EVENT             = 3,
    WLAN_MIB_WNM_RQST_TYPE_DIAGOSTIC         = 4,
    WLAN_MIB_WNM_RQST_TYPE_LOCATION_CFG      = 5,
    WLAN_MIB_WNM_RQST_TYPE_BSS_TRANSIT_QUERY = 6,
    WLAN_MIB_WNM_RQST_TYPE_BSS_TRANSIT_RQST  = 7,
    WLAN_MIB_WNM_RQST_TYPE_FMS               = 8,
    WLAN_MIB_WNM_RQST_TYPE_COLOC_INTERF      = 9,

    WLAN_MIB_WNM_RQST_TYPE_BUTT
} wlan_mib_wnm_rqst_type_enum;
typedef hi_u8 wlan_mib_wnm_rqst_type_enum_uint8;

/* dot11WNMRqstLCRRqstSubject OBJECT-TYPE */
/* SYNTAX INTEGER {                       */
/* local(0),                              */
/* remote(1)                              */
typedef enum {
    WLAN_MIB_WNM_RQST_LCRRQST_SUBJ_LOCAL  = 0,
    WLAN_MIB_WNM_RQST_LCRRQST_SUBJ_REMOTE = 1,

    WLAN_MIB_WNM_RQST_LCRRQST_SUBJ_BUTT
} wlan_mib_wnm_rqst_lcrrqst_subj_enum;
typedef hi_u8 wlan_mib_wnm_rqst_lcrrqst_subj_enum_uint8;

/* dot11WNMRqstLCRIntervalUnits OBJECT-TYPE */
/* SYNTAX INTEGER {                         */
/* seconds(0),                              */
/* minutes(1),                              */
/* hours(2)                                 */
typedef enum {
    WLAN_MIB_WNM_RQST_LCR_INTERVAL_UNIT_SECOND  = 0,
    WLAN_MIB_WNM_RQST_LCR_INTERVAL_UNIT_MINUTE  = 1,
    WLAN_MIB_WNM_RQST_LCR_INTERVAL_UNIT_HOUR    = 2,

    WLAN_MIB_WNM_RQST_LCR_INTERVAL_UNIT_BUTT
} wlan_mib_wnm_rqst_lcr_interval_unit_enum;
typedef hi_u8 wlan_mib_wnm_rqst_lcr_interval_unit_enum_uint8;

/* dot11WNMRqstLIRRqstSubject OBJECT-TYPE */
/* SYNTAX INTEGER {                       */
/* local(0),                              */
/* remote(1)                              */
typedef enum {
    WLAN_MIB_WNM_RQST_LIRRQST_SUBJ_LOCAL   = 0,
    WLAN_MIB_WNM_RQST_LIRRQST_SUBJ_REMOTE  = 1,

    WLAN_MIB_WNM_RQST_LIRRQST_SUBJ_BUTT
} wlan_mib_wnm_rqst_lirrqst_subj_enum;
typedef hi_u8 wlan_mib_wnm_rqst_lirrqst_subj_enum_uint8;

/* dot11WNMRqstLIRIntervalUnits OBJECT-TYPE */
/* SYNTAX INTEGER {                         */
/* seconds(0),                              */
/* minutes(1),                              */
/* hours(2)                                 */
typedef enum {
    WLAN_MIB_WNM_RQST_LIR_INTERVAL_UNIT_SECOND  = 0,
    WLAN_MIB_WNM_RQST_LIR_INTERVAL_UNIT_MINUTE  = 1,
    WLAN_MIB_WNM_RQST_LIR_INTERVAL_UNIT_HOUR    = 2,

    WLAN_MIB_WNM_RQST_LIR_INTERVAL_UNIT_BUTT
} wlan_mib_wnm_rqst_lir_interval_unit_enum;
typedef hi_u8 wlan_mib_wnm_rqst_lir_interval_unit_enum_uint8;

/* dot11WNMRqstEventType OBJECT-TYPE */
/* SYNTAX INTEGER {                      */
/* transition(0),                        */
/* rsna(1),                              */
/* peerToPeer(2),                        */
/* wnmLog(3),                            */
/* vendorSpecific(221)                   */
typedef enum {
    WLAN_MIB_WNM_RQST_EVENT_TYPE_TRANSITION      = 0,
    WLAN_MIB_WNM_RQST_EVENT_TYPE_RSNA            = 1,
    WLAN_MIB_WNM_RQST_EVENT_TYPE_PEERTOPEER      = 2,
    WLAN_MIB_WNM_RQST_EVENT_TYPE_WNMLOG          = 3,
    WLAN_MIB_WNM_RQST_EVENT_TYPE_VENDOR_SPECIFIC = 221,

    WLAN_MIB_WNM_RQST_EVENT_TYPE_BUTT
} wlan_mib_wnm_rqst_event_tpye_enum;
typedef hi_u8 wlan_mib_wnm_rqst_event_type_enum_uint8;

/* dot11WNMRqstDiagType OBJECT-TYPE */
/* SYNTAX INTEGER {                 */
/* cancelRequest(0),                */
/* manufacturerInfoStaRep(1),       */
/* configurationProfile(2),         */
/* associationDiag(3),              */
/* ieee8021xAuthDiag(4),            */
/* vendorSpecific(221)              */
typedef enum {
    WLAN_MIB_WNM_RQST_DIAG_TYPE_CANCEL_RQST          = 0,
    WLAN_MIB_WNM_RQST_DIAG_TYPE_MANUFCT_INFO_STA_REP = 1,
    WLAN_MIB_WNM_RQST_DIAG_TYPE_CFG_PROFILE          = 2,
    WLAN_MIB_WNM_RQST_DIAG_TYPE_ASSO_DIAG            = 3,
    WLAN_MIB_WNM_RQST_DIAG_TYPE_IEEE8021X_AUTH_DIAG  = 4,
    WLAN_MIB_WNM_RQST_DIAG_TYPE_VENDOR_SPECIFIC      = 221,

    WLAN_MIB_WNM_RQST_DIAG_TYPE_BUTT
} wlan_mib_wnm_rqst_diag_type_enum;
typedef hi_u8 wlan_mib_wnm_rqst_diag_type_enum_uint8;

/* dot11WNMRqstDiagCredentials OBJECT-TYPE */
/* SYNTAX INTEGER {                        */
/* none(0),                                */
/* preSharedKey(1),                        */
/* usernamePassword(2),                    */
/* x509Certificate(3),                     */
/* otherCertificate(4),                    */
/* oneTimePassword(5),                     */
/* token(6)                                */
typedef enum {
    WLAN_MIB_WNM_RQST_DIAG_CREDENT_NONT               = 0,
    WLAN_MIB_WNM_RQST_DIAG_CREDENT_PRE_SHAREKEY       = 1,
    WLAN_MIB_WNM_RQST_DIAG_CREDENT_USERNAME_PASSWORD  = 2,
    WLAN_MIB_WNM_RQST_DIAG_CREDENT_X509_CTERTIFICATE  = 3,
    WLAN_MIB_WNM_RQST_DIAG_CREDENT_OTHER_CTERTIFICATE = 4,
    WLAN_MIB_WNM_RQST_DIAG_CREDENT_ONE_TIME_PASSWORD  = 5,
    WLAN_MIB_WNM_RQST_DIAG_CREDENT_TOKEN              = 6,

    WLAN_MIB_WNM_RQST_DIAG_CREDENT_BUTT
} wlan_mib_wnm_rqst_diag_credent_enum;
typedef hi_u8 wlan_mib_wnm_rqst_diag_credent_enum_uint8;

/* dot11WNMRqstBssTransitQueryReason OBJECT-TYPE       */
/* SYNTAX INTEGER {                                    */
/* unspecified(0),                                     */
/* excessiveFrameLossRatesPoorConditions(1),           */
/* excessiveDelayForCurrentTrafficStreams(2),          */
/* insufficientQosCapacityForCurrentTrafficStreams(3), */
/* firstAssociationToEss(4),                           */
/* loadBalancing(5),                                   */
/* betterApFound(6),                                   */
/* deauthenticatedDisassociatedFromPreviousAp(7),      */
/* apFailedIeee8021XEapAuthentication(8),              */
/* apFailed4wayHandshake(9),                           */
/* receivedTooManyReplayCounterFailures(10),           */
/* receivedTooManyDataMICFailures(11),                 */
/* exceededMaxNumberOfRetransmissions(12),             */
/* receivedTooManyBroadcastDisassociations(13),        */
/* receivedTooManyBroadcastDeauthentications(14),      */
/* previousTransitionFailed(15),                       */
/* lowRSSI(16)                                         */
typedef enum {
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_UNSPEC                            = 0,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_EXCES_FRAME_LOSSRATE_POORCDT      = 1,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_EXCES_DELAY_CURRT_TRAFIC_STRMS    = 2,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_INSUF_QOS_CAP_CURRT_TRAFIC_STRMS  = 3,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_FIRST_ASSO_ESS                    = 4,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_LOAD_BALANCE                      = 5,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_BETTER_AP_FOUND                   = 6,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_DEAUTH_DISASSO_FROM_PRE_AP        = 7,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_AP_FAIL_IEEE8021X_EAP_AUTH        = 8,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_AP_FAIL_4WAY_HANDSHAKE            = 9,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_RECEIVE_TOOMANY_REPLAY_COUNT_FAIL = 10,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_RECEIVE_TOOMANY_DATA_MIC_FAIL     = 11,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_EXCEED_MAXNUM_RETANS              = 12,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_RECEIVE_TOOMANY_BRDCAST_DISASSO   = 13,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_RECEIVE_TOOMANY_BRDCAST_DEAUTH    = 14,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_PRE_TRANSIT_FAIL                  = 15,
    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_LOW_RSSI                          = 16,

    WLAN_MIB_WNM_RQST_BSS_TRANSIT_QUERY_REASON_BUTT
} wlan_mib_wnm_rqst_bss_transit_query_reason_enum;
typedef hi_u8 wlan_mib_wnm_rqst_bss_transit_query_reason_enum_uint8;

/* dot11WNMEventTransitRprtEventStatus OBJECT-TYPE */
/* SYNTAX INTEGER {                                */
/* successful(0),                                  */
/* requestFailed(1),                               */
/* requestRefused(2),                              */
/* requestIncapable(3),                            */
/* detectedFrequentTransition(4)                   */
typedef enum {
    WLAN_MIB_WNM_EVENT_TRANSIT_RPRT_EVENT_STATUS_SUCC                = 0,
    WLAN_MIB_WNM_EVENT_TRANSIT_RPRT_EVENT_STATUS_RQST_FAIL           = 1,
    WLAN_MIB_WNM_EVENT_TRANSIT_RPRT_EVENT_STATUS_RQST_REFUSE         = 2,
    WLAN_MIB_WNM_EVENT_TRANSIT_RPRT_EVENT_STATUS_RQST_INCAP          = 3,
    WLAN_MIB_WNM_EVENT_TRANSIT_RPRT_EVENT_STATUS_DETECT_FREQ_TRANSIT = 4,

    WLAN_MIB_WNM_EVENT_TRANSIT_RPRT_EVENT_STATUS_BUTT
} wlan_mib_wnm_event_transit_rprt_event_status_enum;
typedef hi_u8 wlan_mib_wnm_event_transit_rprt_event_status_enum_uint8;

/* dot11WNMEventTransitRprtTransitReason OBJECT-TYPE   */
/* SYNTAX INTEGER {                                    */
/* unspecified(0),                                     */
/* excessiveFrameLossRatesPoorConditions(1),           */
/* excessiveDelayForCurrentTrafficStreams(2),          */
/* insufficientQosCapacityForCurrentTrafficStreams(3), */
/* firstAssociationToEss(4),                           */
/* loadBalancing(5),                                   */
/* betterApFound(6),                                   */
/* deauthenticatedDisassociatedFromPreviousAp(7),      */
/* apFailedIeee8021XEapAuthentication(8),              */
/* apFailed4wayHandshake(9),                           */
/* receivedTooManyReplayCounterFailures(10),           */
/* receivedTooManyDataMICFailures(11),                 */
/* exceededMaxNumberOfRetransmissions(12),             */
/* receivedTooManyBroadcastDisassociations(13),        */
/* receivedTooManyBroadcastDeauthentications(14),      */
/* previousTransitionFailed(15),                       */
/* lowRSSI(16)                                         */
typedef enum {
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_UNSPEC                            = 0,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_EXCES_FRAME_LOSSRATE_POORCDT      = 1,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_EXCES_DELAY_CURRT_TRAFIC_STRMS    = 2,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_INSUF_QOS_CAP_CURRT_TRAFIC_STRMS  = 3,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_FIRST_ASSO_ESS                    = 4,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_LOAD_BALANCE                      = 5,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_BETTER_AP_FOUND                   = 6,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_DEAUTH_DISASSO_FROM_PRE_AP        = 7,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_AP_FAIL_IEEE8021X_EAP_AUTH        = 8,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_AP_FAIL_4WAY_HANDSHAKE            = 9,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_RECEIVE_TOOMANY_REPLAY_COUNT_FAIL = 10,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_RECEIVE_TOOMANY_DATA_MIC_FAIL     = 11,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_EXCEED_MAXNUM_RETANS              = 12,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_RECEIVE_TOOMANY_BRDCAST_DISASSO   = 13,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_RECEIVE_TOOMANY_BRDCAST_DEAUTH    = 14,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_PRE_TRANSIT_FAIL                  = 15,
    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_LOW_RSSI                          = 16,

    WLAN_MIB_WNM_EVENT_TRANSITRPRT_TRANSIT_REASON_BUTT
} wlan_mib_wnm_event_transitrprt_transit_reason_enum;
typedef hi_u8 wlan_mib_wnm_event_transitrprt_transit_reason_enum_uint8;

/* dot11WNMEventRsnaRprtEventStatus OBJECT-TYPE */
/* SYNTAX INTEGER {                             */
/* successful(0),                               */
/* requestFailed(1),                            */
/* requestRefused(2),                           */
/* requestIncapable(3),                         */
/* detectedFrequentTransition(4)                */
typedef enum {
    WLAN_MIB_WNM_EVENT_RSNARPRT_EVENT_STATUS_SUCC                = 0,
    WLAN_MIB_WNM_EVENT_RSNARPRT_EVENT_STATUS_RQST_FAIL           = 1,
    WLAN_MIB_WNM_EVENT_RSNARPRT_EVENT_STATUS_RQST_REFUSE         = 2,
    WLAN_MIB_WNM_EVENT_RSNARPRT_EVENT_STATUS_RQST_INCAP          = 3,
    WLAN_MIB_WNM_EVENT_RSNARPRT_EVENT_STATUS_DETECT_FREQ_TRANSIT = 4,

    WLAN_MIB_WNM_EVENT_RSNARPRT_EVENT_STATUS_BUTT
} wlan_mib_wnm_event_rsnarprt_event_status_enum;
typedef hi_u8 wlan_mib_wnm_event_rsnarprt_event_status_enum_uint8;

/* dot11APLCIDatum OBJECT-TYPE */
/* SYNTAX INTEGER {            */
/* wgs84 (1),                  */
/* nad83navd88 (2),            */
/* nad93mllwvd (3)             */
typedef enum {
    WLAN_MIB_AP_LCI_DATUM_WGS84        = 1,
    WLAN_MIB_AP_LCI_DATUM_NAD83_NAVD88 = 2,
    WLAN_MIB_AP_LCI_DATUM_NAD93_MLLWVD = 3,

    WLAN_MIB_AP_LCI_DATUM_BUTT
} wlan_mib_ap_lci_datum_enum;
typedef hi_u8 wlan_mib_ap_lci_datum_enum_uint8;

/* dot11APLCIAzimuthType OBJECT-TYPE */
/* SYNTAX INTEGER {                  */
/* frontSurfaceOfSTA(0),             */
/* radioBeam(1) }                    */
typedef enum {
    WLAN_MIB_AP_LCI_AZIMUTH_TYPE_FRONT_SURFACE_STA = 0,
    WLAN_MIB_AP_LCI_AZIMUTH_TYPE_RADIO_BEAM        = 1,

    WLAN_MIB_AP_LCI_AZIMUTH_TYPE_BUTT
} wlan_mib_ap_lci_azimuth_type_enum;
typedef hi_u8 wlan_mib_ap_lci_azimuth_type_enum_uint8;

/* dot11HTProtection 枚举定义 */
typedef enum {
    WLAN_MIB_HT_NO_PROTECTION           = 0,
    WLAN_MIB_HT_NONMEMBER_PROTECTION    = 1,
    WLAN_MIB_HT_20MHZ_PROTECTION        = 2,
    WLAN_MIB_HT_NON_HT_MIXED            = 3,

    WLAN_MIB_HT_PROTECTION_BUTT
} wlan_mib_ht_protection_enum;
typedef hi_u8 wlan_mib_ht_protection_enum_uint8;

/* VHT Capabilities Info field 最大MPDU长度枚举 */
typedef enum {
    WLAN_MIB_VHT_MPDU_3895  = 0,
    WLAN_MIB_VHT_MPDU_7991  = 1,
    WLAN_MIB_VHT_MPDU_11454 = 2,

    WLAN_MIB_VHT_MPDU_LEN_BUTT
} wlan_mib_vht_mpdu_len_enum;
typedef hi_u8 wlan_mib_vht_mpdu_len_enum_uint8;

/* VHT Capabilities Info field 支持带宽枚举 */
typedef enum {
    WLAN_MIB_VHT_SUPP_WIDTH_80          = 0,    /* 不支持160或者80+80 */
    WLAN_MIB_VHT_SUPP_WIDTH_160         = 1,    /* 支持160 */
    WLAN_MIB_VHT_SUPP_WIDTH_80PLUS80    = 2,    /* 支持160和80+80 */

    WLAN_MIB_VHT_SUPP_WIDTH_BUTT
} wlan_mib_vht_supp_width_enum;
typedef hi_u8 wlan_mib_vht_supp_width_enum_uint8;

/* ****************************************************************************
    配置命令 ID
    第一段  MIB 类配置
    第二段  非MIB类配置
**************************************************************************** */
typedef enum {
    /* ***********************************************************************
        第一段 MIB 类配置
    ************************************************************************ */
    /* *********************dot11smt OBJECT IDENTIFIER ::= { ieee802dot11 1 }************************* */
    /* --  dot11StationConfigTable ::= { dot11smt 1 } */
    WLAN_CFGID_STATION_ID                 = 0,              /* dot11StationID MacAddress, MAC地址 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_BSS_TYPE                   = 4,              /* dot11DesiredBSSType INTEGER, */
#endif
    WLAN_CFGID_SSID                       = 5,              /* dot11DesiredSSID OCTET STRING, SIZE(0..32) */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_BEACON_INTERVAL            = 6,              /* dot11BeaconPeriod Unsigned32, */
    WLAN_CFGID_DTIM_PERIOD                = 7,              /* dot11DTIMPeriod Unsigned32, */
#endif
    WLAN_CFGID_UAPSD_EN                   = 11,             /* dot11APSDOptionImplemented TruthValue, */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_SMPS_MODE                  = 12,             /* dot11MIMOPowerSave INTEGER, */
    WLAN_CFGID_SMPS_EN                    = 13,
#endif
/* --  dot11PrivacyTable ::= { dot11smt 5 } */
/* ***********************dot11mac OBJECT IDENTIFIER ::= { ieee802dot11 2 } ************************* */
/* --  dot11OperationTable ::= { dot11mac 1 } */
/* --  dot11CountersTable ::= { dot11mac 2 }  */
/* --  dot11EDCATable ::= { dot11mac 4 } */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_EDCA_TABLE_CWMIN                = 41,            /* dot11EDCATableCWmin Unsigned32 */
    WLAN_CFGID_EDCA_TABLE_CWMAX                = 42,            /* dot11EDCATableCWmax Unsigned32 */
    WLAN_CFGID_EDCA_TABLE_AIFSN                = 43,            /* dot11EDCATableAIFSN Unsigned32 */
    WLAN_CFGID_EDCA_TABLE_TXOP_LIMIT           = 44,            /* dot11EDCATableTXOPLimit Unsigned32 */
    WLAN_CFGID_EDCA_TABLE_MSDU_LIFETIME        = 45,            /* dot11EDCATableMSDULifetime Unsigned32 */
    WLAN_CFGID_EDCA_TABLE_MANDATORY            = 46,            /* dot11EDCATableMandatory TruthValue */
#endif
    /* --  dot11QAPEDCATable ::= { dot11mac 5 } */
    WLAN_CFGID_QEDCA_TABLE_CWMIN           = 51,           /* dot11QAPEDCATableCWmin Unsigned32 */
    WLAN_CFGID_QEDCA_TABLE_CWMAX           = 52,           /* dot11QAPEDCATableCWmax Unsigned32 */
    WLAN_CFGID_QEDCA_TABLE_AIFSN           = 53,           /* dot11QAPEDCATableAIFSN Unsigned32 */
    WLAN_CFGID_QEDCA_TABLE_TXOP_LIMIT      = 54,           /* dot11QAPEDCATableTXOPLimit Unsigned32 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_QEDCA_TABLE_MSDU_LIFETIME   = 55,           /* dot11QAPEDCATableMSDULifetime Unsigned32 */
    WLAN_CFGID_QEDCA_TABLE_MANDATORY       = 56,           /* dot11QAPEDCATableMandatory TruthValue */
    /* ************************dot11res OBJECT IDENTIFIER ::= { ieee802dot11 3 } ************************* */
    /* ************************dot11phy OBJECT IDENTIFIER ::= { ieee802dot11 4 } ************************ */
    /* --  dot11PhyHRDSSSTable ::= { dot11phy 12 } */
    WLAN_CFGID_SHORT_PREAMBLE = 60, /* dot11ShortPreambleOptionImplemented TruthValue */
#endif
    /* --  dot11PhyERPTable ::= { dot11phy 14 } */
    /* --  dot11PhyHTTable  ::= { dot11phy 15 } */
    WLAN_CFGID_SHORTGI                    = 80,         /* dot11ShortGIOptionInTwentyActivated TruthValue */
    WLAN_CFGID_CURRENT_CHANEL             = 82,         /* dot11CurrentChannel Unsigned32 */

/* --  dot11PhyVHTTable  ::= { dot11phy 23 }  -- */
/* ***********************dot11Conformance OBJECT IDENTIFIER ::= { ieee802dot11 5 } ******************* */
/* ***********************dot11imt         OBJECT IDENTIFIER ::= {ieee802dot11 6} ********************* */
/* ***********************dot11MSGCF       OBJECT IDENTIFIER ::= { ieee802dot11 7} ******************** */
#ifdef _PRE_WLAN_FEATURE_MESH
    /* --  dot11MeshSTAConfigTable ::= { dot11smt 23 } */
    WLAN_CFGID_MESHID                     = 91,         /* dot11MeshID OCTET STRING,   (SIZE(0..32))       */
    WLAN_CFGID_ACCEPT_PEER                = 92,         /* dot11MeshAcceptingAdditionalPeerings TruthValue */
#endif

    /* ***********************************************************************
        第二段 非MIB 类配置
    ************************************************************************ */
    /* 以下为ioctl下发的命令 */
    WLAN_CFGID_ADD_VAP             = 100,               /* 创建VAP */
    WLAN_CFGID_START_VAP           = 101,               /* 启用VAP */
    WLAN_CFGID_DEL_VAP             = 102,               /* 删除VAP */
    WLAN_CFGID_DOWN_VAP            = 103,               /* 停用VAP */
    WLAN_CFGID_MODE                = 105,               /* 模式: 包括协议 频段 带宽 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_CONCURRENT          = 106,               /* 设置并发用户数 */
    WLAN_CFGID_ADDR_FILTER         = 107,               /* 地址过滤 */
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_PROT_MODE           = 108,               /* 保护模式 */
    WLAN_CFGID_AUTH_MODE           = 109,               /* 认证模式 */
#endif
    WLAN_CFGID_TX_POWER            = 113,               /* 传输功率 */
    WLAN_CFGID_VAP_INFO            = 114,               /* 打印vap参数信息 */

    WLAN_CFGID_VAP_STATE_SYN       = 115,
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_BANDWIDTH           = 116,
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_STOP_SCHED_SCAN     = 118,
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    WLAN_CFGID_STA_VAP_INFO_SYN    = 119,
#endif
    /* wpa-wpa2 */
    WLAN_CFGID_ADD_KEY             = 120,
    WLAN_CFGID_DEFAULT_KEY         = 121,
    WLAN_CFGID_REMOVE_KEY          = 123,
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_GET_KEY             = 125,
#endif
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
    WLAN_CFGID_SET_REKEY           = 127,
#endif
    /* wep */
    WLAN_CFGID_REMOVE_WEP_KEY      = 131,
    WLAN_CFGID_ADD_WEP_ENTRY       = 132,
    /* 认证加密模式配置 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_BTCOEX_INFO         = 152,               /* 打印共存维测信息 */
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_REPORT_VAP_INFO     = 159,               /* 上报vap信息 */
#endif
#endif
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_GET_MPDU_NUM                 = 172,      /* 获取mpdu数目 */
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_ADD_USER                     = 178,      /* 添加用户配置命令 */
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_AMPDU_START                  = 180,      /* 开启AMPDU的配置命令 */
#endif
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_AMPDU_END                    = 181,      /* 关闭AMPDU的配置命令 */
#endif
#ifdef _PRE_WLAN_FEATURE_SIGMA
    WLAN_CFGID_ADDBA_REQ                    = 184,      /* 建立BA会话的配置命令 */
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_DELBA_REQ                    = 185,      /* 删除BA会话的配置命令 */
    WLAN_CFGID_SET_RANDOM_MAC_ADDR_SCAN     = 193,      /* 设置随机mac addr扫描开关 */
#endif
#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_START_SCAN                   = 204,      /* 触发初始扫描 */
    WLAN_CFGID_START_JOIN                   = 205,      /* 触发加入认证并关联 */
    WLAN_CFGID_START_DEAUTH                 = 206,      /* 触发去认证 */
#endif
#endif
    WLAN_CFGID_MINIMIZE_BOOT_CURRET         = 207,      /* 降低开机电流 */
    WLAN_CFGID_KICK_USER                    = 208,      /* 去关联1个用户 */
#if defined(_PRE_WLAN_FEATURE_HIPRIV) && defined(_PRE_WLAN_FEATURE_INTRF_MODE)
    WLAN_CFGID_INTRF_MODE_ON                = 209,      /* 开启或关闭干扰模式功能 */
#endif
#if defined (_PRE_WLAN_FEATURE_HIPRIV) || defined (_PRE_WLAN_FEATURE_SIGMA)
    WLAN_CFGID_AMPDU_TX_ON                  = 212,      /* 开启或关闭ampdu发送功能 */
#endif
#if defined (_PRE_WLAN_FEATURE_HIPRIV) || defined (_PRE_WLAN_FEATURE_SIGMA)
    WLAN_CFGID_AMSDU_TX_ON                  = 213,      /* 开启或关闭amsdu发送功能 */
#endif
#if defined _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SEND_BAR                     = 215,      /* 指定用户的指定tid发送bar */
    WLAN_CFGID_LIST_CHAN                    = 217,      /* 列举支持的管制域信道 */
#endif
#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_REGDOMAIN_PWR                = 218,      /* 设置管制域功率 */
#endif
    WLAN_CFGID_TXBF_SWITCH                  = 219,      /* 开启或关闭TXBF发送功能 */
#endif
#if defined (_PRE_WLAN_FEATURE_HIPRIV) || defined (_PRE_WLAN_FEATURE_SIGMA)
    WLAN_CFGID_FRAG_THRESHOLD_REG           = 221,      /* 设置分片门限长度 */
#endif
#ifdef _PRE_WLAN_FEATURE_STA_PM
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_PSM_PARAM                = 231,      /* STA 低功耗tbtt offset/listen interval配置 */
#endif
    WLAN_CFGID_SET_STA_PM_ON                = 232,      /* STA低功耗开关接口 */
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_STA_HW_PS_MODE           = 233,      /* 可配置深睡浅睡 */
#endif
#endif
#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_PACKET_XMIT                  = 240,      /* 发指定个数的报文 */
    WLAN_CFGID_VAP_PKT_STAT                 = 242,      /* vap统计信息 */
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_AMSDU_AMPDU_SWITCH           = 246,
#endif
    WLAN_CFGID_COUNTRY                      = 247,      /* 设置国家码管制域信息 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_TID                          = 248,      /* 读取最新接收到数据帧的TID */
#endif
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_UAPSD_DEBUG                  = 250,      /* UAPSD维测信息 */
#endif
    WLAN_CFGID_ALG_PARAM                    = 254,      /* 算法参数配置 */
    WLAN_CFGID_GET_MAC_FROM_EFUSE           = 259,       /* 将mac地址返回host侧 */
    WLAN_CFGID_MFG_TEST                     = 260,       /* 产测信息打印到host测 */
    WLAN_CFGID_REPORT_CAL_DATA              = 263,      /* 产测信息返回到host测 */
    WLAN_CFGID_RX_FCS_INFO                  = 261,      /* 打印接收帧FCS解析信息 */
    WLAN_CFGID_CLEAN_SCAN_RESULT            = 262,      /* 扫描清除驱动结果 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_ACS_CONFIG                   = 286,      /* ACS命令 */
#endif
    WLAN_CFGID_SCAN_ABORT                   = 287,      /* 扫描终止 */
    /* 以下命令为cfg80211下发的命令(通过内核) */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_CFG80211_START_SCHED_SCAN        = 288,         /* 内核下发启动PNO调度扫描命令 */
    WLAN_CFGID_CFG80211_STOP_SCHED_SCAN         = 289,         /* 内核下发停止PNO调度扫描命令 */
#endif
    WLAN_CFGID_CFG80211_START_SCAN              = 290,         /* 内核下发启动扫描命令 */
    WLAN_CFGID_CFG80211_START_CONNECT           = 291,         /* 内核下发启动JOIN(connect)命令 */
    WLAN_CFGID_CFG80211_SET_CHANNEL             = 292,         /* 内核下发设置信道命令 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_CFG80211_SET_WIPHY_PARAMS        = 293,         /* 内核下发设置wiphy 结构体命令 */
#endif
    WLAN_CFGID_CFG80211_CONFIG_BEACON           = 295,         /* 内核下发设置VAP信息 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_ACS_PARAM                        = 297,         /* ACS命令 */
    WLAN_CFGID_SET_ACS_CMD                      = 298,
    WLAN_CFGID_RADARTOOL                        = 310,         /* DFS配置命令 */
    /* BEGIN:以下命令为开源APP 程序下发的私有命令 */
    WLAN_CFGID_GET_ASSOC_REQ_IE           = 311,         /* hostapd 获取ASSOC REQ 帧信息 */
#endif
    WLAN_CFGID_SET_RTS_THRESHHOLD         = 313,         /* hostapd 设置RTS 门限 */
    WLAN_CFGID_SET_WPS_P2P_IE             = 314,         /* wpa_supplicant 设置WPS/P2P 信息元素到VAP */
    /* END:以下命令为开源APP 程序下发的私有命令 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_DYNC_TXPOWER        = 318,           /* 动态功率校准开关 */
#endif
    WLAN_CFGID_USER_INFO           = 320,           /* 用户信息 */
    WLAN_CFGID_SET_DSCR            = 321,           /* 配置用户信息 */
    WLAN_CFGID_SET_RATE            = 322,           /* 设置non-HT速率 */
    WLAN_CFGID_SET_MCS             = 323,           /* 设置HT速率 */
    WLAN_CFGID_SET_BW              = 327,           /* 设置带宽 */
    WLAN_CFGID_SET_ALWAYS_TX       = 328,           /* 设置常发模式 */
    WLAN_CFGID_SET_ALWAYS_RX       = 329,           /* 设置常发模式 */
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    WLAN_CFGID_REG_INFO            = 332,           /* 寄存器地址信息 */
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_REG_WRITE           = 333,           /* 写入寄存器信息 */
#endif
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_OPEN_ADDR4          = 336,           /* mac头为4地址 */
#endif
    WLAN_CFGID_WMM_SWITCH          = 338,           /* 打开或者关闭wmm */
#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_HIDE_SSID           = 339,           /* 打开或者关闭隐藏ssid */
#endif
#endif
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_CHIP_TEST_OPEN      = 340,           /* 打开芯片验证开关 */
#endif
#ifdef _PRE_WLAN_FEATURE_BW_HIEX
    WLAN_CFGID_SET_SELFCTS         = 341,           /* 设置窄带切宽带发送selfcts参数 */
#endif

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP_DEBUG /* edca优化算法调试命令,实际业务不使用,宏屏蔽 */
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_EDCA_OPT_SWITCH_AP    = 344,         /* 设置AP打开edca_opt模式 */
    WLAN_CFGID_EDCA_OPT_CYCLE_AP     = 345,         /* 设置AP的edca调整周期 */
    WLAN_CFGID_EDCA_OPT_SWITCH_STA   = 346,         /* 设置STA的edca优化开关 */
#endif
#endif
    WLAN_CFGID_PROTECTION_UPDATE_STA_USER = 348, /* 增加用户更新保护模式 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_SET_RTS_PARAM               = 350,      /* 配置RTS速率 */
    WLAN_CFGID_UPDTAE_PROT_TX_PARAM        = 351,      /* 更新保护模式相关的发送参数 */
    WLAN_CFGID_SET_PROTECTION              = 352,
#endif
#ifdef _PRE_WLAN_FEATURE_OFFLOAD_FLOWCTL
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_FLOWCTL_PARAM           = 353,      /* 设置流控相关参数 */
    WLAN_CFGID_GET_FLOWCTL_STAT            = 354,      /* 获取流控相关状态信息 */
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_MIB                 = 360,      /* 配置mib */
    WLAN_CFGID_GET_MIB                 = 361,      /* 获取mib */
#endif
#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_AUTO_PROTECTION     = 362,      /* 设置auto protection开关 */
#endif
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    WLAN_CFGID_SET_OBSS_MIB            = 363,      /* 配置obss mib */
#endif
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_GET_ALL_REG_VALUE       = 376,      /* 获取所有寄存器的值 */
    WLAN_CFGID_REPORT_AMPDU_STAT       = 377,      /* ampdu各种流程统计 */
    WLAN_CFGID_WIFI_EN           = 460,              /* WIFI使能开关 */
    WLAN_CFGID_PM_INFO           = 461,              /* PM信息 */
    WLAN_CFGID_PM_EN             = 462,              /* PM开关 */
#endif
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_AGGR_NUM          = 474,     /* 设置聚合个数 */
#endif
    WLAN_CFGID_SET_STBC_CAP          = 477,     /* 设置STBC能力 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_SCAN_TEST             = 482,     /* 扫描模块测试命令 */
#endif
    WLAN_CFGID_QUERY_STATION_STATS   = 483,        /* 信息上报查询命令 */
    WLAN_CFGID_CONNECT_REQ           = 484,
#ifdef _PRE_DEBUG_MODE
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFIGD_BGSCAN_ENABLE         = 485,      /* 禁用背景扫描命令 */
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    WLAN_CFGID_CFG80211_REMAIN_ON_CHANNEL           = 490,  /* 停止在指定信道 */
    WLAN_CFGID_CFG80211_CANCEL_REMAIN_ON_CHANNEL    = 491,  /* 取消停止在指定信道 */
#endif

#ifdef _PRE_WLAN_FEATURE_STA_PM
    WLAN_CFGID_SET_PS_MODE            = 494,     /* 设置pspoll mode */
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_PSM_OFFSET         = 495,
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_STA_UAPSD
    WLAN_CFGID_SET_UAPSD_PARA         = 496,      /* 设置UAPSD参数 */
#endif

    WLAN_CFGID_CFG80211_MGMT_TX        = 498,  /* 发送管理帧 */
    WLAN_CFGID_CFG80211_MGMT_TX_STATUS = 499,
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    WLAN_CFGID_THRUPUT_INFO            = 501, /* 吞吐量数据由dmac同步到hmac */
#endif
#ifdef _PRE_WLAN_FEATURE_P2P
    WLAN_CFGID_SEND_P2P_ACTION         = 503,      /* 通知驱动发送Action 帧 */
    WLAN_CFGID_SET_P2P_PS_OPS          = 504,      /* 配置P2P OPS节能 */
    WLAN_CFGID_SET_P2P_PS_NOA          = 505,      /* 配置P2P NOA节能 */
    WLAN_CFGID_SET_P2P_STATUS          = 508,      /* 配置P2P 状态 */
#endif
    WLAN_CFGID_UAPSD_UPDATE            = 510,
#ifdef _PRE_WLAN_RF_110X_CALI_DPD
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_START_DPD                = 511,
#endif
#endif

#ifdef _PRE_WLAN_FEATURE_PROMIS
    WLAN_CFGID_MONITOR_EN               = 522,
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    WLAN_CFGID_NSS                      = 524,   /* 空间流信息的同步 */
#endif
#ifdef _PRE_WLAN_FEATURE_ARP_OFFLOAD
    WLAN_CFGID_ARP_OFFLOAD_SETTING      = 526,   /* 使能 ARP offload */
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_ARP_OFFLOAD_SHOW_INFO    = 527,   /* 打印 Device 侧的 IP 地址 */
#endif
#endif
#ifdef _PRE_WLAN_FEATURE_DHCP_OFFLOAD
    WLAN_CFGID_DHCP_OFFLOAD_SETTING     = 528,   /* 使能 DHCP offload */
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    WLAN_CFGID_CFG_VAP_H2D              = 529,  /* 配置vap下发device事件 */
    WLAN_CFGID_HOST_DEV_INIT            = 530,  /* 下发初始化host dev init事件 */
    WLAN_CFGID_HOST_DEV_EXIT            = 531,  /* 下发去初始化host dev exit事件 */
#endif

    WLAN_CFGID_SET_MAX_USER             = 538,   /* 设置最大用户数 */
    WLAN_CFGID_GET_STA_LIST             = 539,   /* 设置最大用户数 */

#ifdef _PRE_WLAN_FEATURE_BTCOEX
    WLAN_CFGID_BTCOEX_RX_DELBA_TRIGGER  = 540,
#endif

#ifdef _PRE_WLAN_FEATURE_WAPI
    WLAN_CFGID_ADD_WAPI_KEY             = 543,
#endif

    WLAN_CFGID_SET_PM_SWITCH            = 570,  /* 全局低功耗使能去使能 */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_DESTROY_VAP              = 574,
#endif
#ifdef _PRE_WLAN_FEATURE_WOW
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_HOST_SLEEP_EN            = 575,      /* set FW no send any frame to driver */
    WLAN_CFGID_SET_WOW                  = 576,      /* Enable/disable WOW events */
    WLAN_CFGID_WOW_ACTIVATE_EN          = 577,      /* Enable/disable WOW */
    WLAN_CFGID_SET_WOW_PATTERN          = 578,      /* set wow pattern */
#endif
    WLAN_CFGID_HOST_SLEEP_NOTIFY        = 579,      /* Host sleep request & wakeup notify */
    WLAN_CFGID_SET_WOW_PARAM            = 580,      /* set wow param to dmac */
    WLAN_CFGID_WOW_SET_SSID             = 581,      /* set wow ssid wakeup */
    WLAN_CFGID_SET_DEV_STATE            = 582,      /* host set device state */
#endif /* end of _PRE_WLAN_FEATURE_WOW */
    /* HISI-CUSTOMIZE */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_SET_ALL_LOG_LEVEL,                   /* 配置所有vap log level */
    WLAN_CFGID_SET_CHN_EST_CTRL,                    /* 配置WIFI PHY寄存器 */
#endif
    WLAN_CFGID_SET_PM_CFG_PARAM,                    /* 配置时钟 */
    WLAN_CFGID_SET_CUS_RF,                          /* RF定制化 */
    WLAN_CFGID_SET_CUS_DTS_CALI,                    /* DTS校准定制化 */
    WLAN_CFGID_SET_CUS_NVRAM_PARAM,                 /* NVRAM参数定制化 */
    WLAN_CFGID_SET_CUS_FCC_TX_PWR,                  /* FCC发送功率定制化 */
#ifdef _PRE_XTAL_FREQUENCY_COMPESATION_ENABLE
    WLAN_CFGID_SET_FREQ_COMP,                       /* 高温频偏补偿 */
#endif
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_LAUCH_CAP,                           /* 读取设备发射能力 */
#endif

#ifdef _PRE_WLAN_FEATURE_TX_CLASSIFY_LAN_TO_WLAN
    WLAN_CFGID_TX_CLASSIFY_LAN_TO_WLAN_SWITCH       = 614,         /* 设置业务识别功能开关 */
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    WLAN_CFGID_REDUCE_SAR               = 615,      /* 通过降低发射功率来降低SAR */
#endif
    WLAN_CFGID_RSSI                     = 616,      /* 查询对端RSSI */
#ifdef _PRE_DEBUG_MODE
    WLAN_CFGID_VENDOR_IE                = 617,      /* 添加VENDOR IE */
#endif
    WLAN_CFGID_CUSTOM_PKT               = 618,      /* 发送用户定制报文 */
#ifdef _PRE_WLAN_FEATURE_CSI
    WLAN_CFGID_CSI_SWITCH               = 620,      /* 设置1131开关 */
    WLAN_CFGID_CSI_SET_CONFIG           = 621,
#endif
    WLAN_CFGID_SET_RETRY_LIMIT          = 630,      /* vap重传参数配置 */
#ifdef _PRE_WLAN_FEATURE_MESH
    WLAN_CFGID_SET_MESH_STA             = 632,      /* Wpa_supplicant设置Mesh User用户状态 */
    WLAN_CFGID_SEND_MESH_ACTION         = 633,      /* 通知驱动发送Action 帧 */
    WLAN_CFGID_UNICAST_DATA_TX_INFO     = 634,      /* DMAC上报单播数据帧信息到HMAC */
#endif
    WLAN_CFGID_REPORT_USER_ASSOC_INFO   = 635,      /* 上报lwip用户关联信息 */
#ifdef _PRE_WLAN_FEATURE_MESH
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_REPORT_TIMES_LIMIT   = 636,      /* 设置驱动上报发送次数的限制参数time_limit */
    WLAN_CFGID_SET_REPORT_CNT_LIMIT     = 637,      /* 设置驱动上报发送次数的限制参数cnt_limit */
#endif
    WLAN_CFGID_SET_BEACON_PRIORITY      = 638,      /* Mesh 协议栈设置节点优先级(Hisi-Optimization) */
    WLAN_CFGID_SET_MNID                 = 639,      /* mesh 设置mnid(mesh node id) */
#ifdef _PRE_WLAN_FEATURE_HIPRIV
    WLAN_CFGID_SET_MBR_EN               = 640,      /* mesh 标识该vap是否为MBR */
#endif
    WLAN_CFGID_ADD_MESH_USER            = 641,      /* 添加mesh用户 */
    WLAN_CFGID_SET_NEW_PEER_CONFIG_EN   = 642,      /* 设置使能new peer candidate */
    WLAN_CFGID_SET_MESH_ACCEPT_STA      = 643,      /* 设置允许sta接入 */
    WLAN_CFGID_SET_MESH_USER_GTK        = 644,      /* 设置Mesh用户组播密钥 */
    WLAN_CFGID_GET_MESH_NODE_INFO       = 645,      /* 获取Mesh节点信息 */
#endif
#ifdef FEATURE_DAQ
    WLAN_CFGID_DATA_ACQ_START     = 690,       /* 数据采集 */
    WLAN_CFGID_DATA_ACQ_STATUS    = 691,
    WLAN_CFGID_DATA_ACQ_REPORT    = 692,
#endif
#ifdef _PRE_WLAN_FEATURE_ANY_ROM
    WLAN_CFGID_ANY_SEND_PEER_DATA      = 700,       /* 向点对点ANY对端设备发送数据 */
    WLAN_CFGID_ANY_ADD_PEER_INFO       = 701,       /* 添加ANY对端设备信息 */
    WLAN_CFGID_ANY_DEL_PEER_INFO       = 702,       /* 删除ANY对端设备信息 */
    WLAN_CFGID_ANY_TX_STATUS           = 703,       /* 发送状态上报消息 */
    WLAN_CFGID_ANY_INIT                = 704,       /* ANY初始化 */
    WLAN_CFGID_ANY_DEINIT              = 705,       /* ANY去初始化 */
    WLAN_CFGID_ANY_SCAN                = 706,       /* 扫描ANY对端 */
    WLAN_CFGID_ANY_FETCH_PEER_INFO     = 708,       /* 通过索引号查询对端信息,仅供调试测试使用，正式版本需要删除 */
    WLAN_CFGID_SET_ANY                 = 709,       /* 通知any设置和取消信息 */
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    WLAN_CFGID_BTCOEX_ENABLE            = 710,      /* wifi/bt共存使能/禁止 */
#endif
#ifdef _PRE_WLAN_FEATURE_REKEY_OFFLOAD
    WLAN_CFGID_REKEY_OFFLOAD_SET_SWITCH = 711,
#endif
#ifdef _PRE_WLAN_FEATURE_MFG_TEST
    WLAN_CFGID_SET_EFUSE_MAC            = 712,
    WLAN_CFGID_GET_EFUSE_MAC            = 713,
    WLAN_CFGID_SET_DATAEFUSE            = 714,
    WLAN_CFGID_CAL_BAND_POWER           = 715,      /* 带宽功率补偿 */
    WLAN_CFGID_CAL_RATE_POWER           = 716,      /* 速率功率补偿 */
    WLAN_CFGID_CAL_FREQ                 = 717,      /* 常温频偏补偿 */
#endif
    WLAN_CFGID_SET_CCA_TH               = 718,
    WLAN_CFGID_SET_TX_PWR_OFFSET        = 719,
    WLAN_CFGID_GET_CAL_DATA             = 720,
    WLAN_CFGID_GET_EFUSE_MAC_ADDR       = 721,
    WLAN_CFGID_NOTIFY_GET_TX_PARAMS     = 722,
    WLAN_CFGID_REPORT_TX_PARAMS         = 723,
    /************************************************************************
        第三段 非MIB的内部数据同步，需要严格受控
    *************************************************************************/
    WLAN_CFGID_SET_MULTI_USER          = 2000,
    WLAN_CFGID_USR_INFO_SYN            = 2001,
    WLAN_CFGID_USER_ASOC_STATE_SYN     = 2002,
    WLAN_CFGID_INIT_SECURTIY_PORT      = 2004,
    WLAN_CFGID_USER_RATE_SYN           = 2005,
    WLAN_CFGID_UPDATE_OPMODE           = 2006,   /* 更新opmode的相关信息 */

    WLAN_CFGID_USER_CAP_SYN            = 2007,   /* hmac向dmac同步mac user的cap能力信息 */

    WLAN_CFGID_SUSPEND_ACTION_SYN      = 2008,

    WLAN_CFGID_BUTT,
} wlan_cfgid_enum;
typedef hi_u16 wlan_cfgid_enum_uint16;

/* ****************************************************************************
  4 全局变量声明
**************************************************************************** */
/* ****************************************************************************
  5 消息头定义
**************************************************************************** */
/* ****************************************************************************
  6 消息定义
**************************************************************************** */
/* ****************************************************************************
  7 STRUCT定义
**************************************************************************** */
/* 当前MIB表项还未稳定，暂不需要4字节对齐，待后续MIB项稳定后再来调整这些结构体 */
/* 是否需要该结构体表示变长的字符串，待确定 */
typedef struct {
    hi_u16 us_octet_nums; /* 字节长度，此值为0时表示没有字符串信息 */
    hi_u16 resv;
    hi_u8 *puc_octec_string; /* 字符串起始地址 */
} wlan_mib_octet_string_stru;

typedef struct {
    hi_u8 mcs_nums;
    hi_u8 resv[3]; /* 3:resv */
    hi_u8 *mcs_set;
} wlan_mib_ht_op_mcs_set_stru; /* dot11HTOperationalMCSSet */

/* dot11LocationServicesLocationIndicationIndicationParameters OBJECT-TYPE */
/* SYNTAX OCTET STRING (SIZE (1..255))                                     */
/* MAX-ACCESS read-create                                                  */
/* STATUS current                                                          */
/* DESCRIPTION                                                             */
/* "This attribute indicates the location Indication Parameters used for   */
/* transmitting Location Track Notification Frames."                       */
/* ::= { dot11LocationServicesEntry 15}                                    */
typedef struct {
    hi_u8 para_nums;
    hi_u8 resv[3]; /* 3:resv */
    hi_u8 *para;
} wlan_mib_local_serv_location_ind_ind_para_stru;

/* Start of dot11smt OBJECT IDENTIFIER ::= { ieee802dot11 1 } */
typedef struct {
    hi_u8 auc_dot11_station_id[WLAN_MAC_ADDR_LEN];                 /* dot11StationID MacAddress, */
    hi_u8 auc_p2p0_dot11_station_id[WLAN_MAC_ADDR_LEN];            /* P2P0 dot11StationID MacAddress, */
    hi_u32 dot11_authentication_response_time_out;                 /* dot11AuthenticationResponseTimeOut Unsigned32, */
    wlan_mib_pwr_mgmt_mode_enum_uint8 dot11_power_management_mode; /* dot11PowerManagementMode INTEGER, */
    hi_u8 auc_dot11_desired_ssid[32 + 1];                          /* dot11DesiredSSID OCTET STRING, SIZE(0..32) */
    wlan_mib_desired_bsstype_enum_uint8 dot11_desired_bss_type;    /* dot11DesiredBSSType INTEGER, */
    hi_u32 dot11_beacon_period;                                    /* dot11BeaconPeriod Unsigned32, */
    hi_u32 dot11_dtim_period;                                      /* dot11DTIMPeriod Unsigned32, */
    hi_u32 dot11_association_response_time_out;                    /* dot11AssociationResponseTimeOut Unsigned32, */
    hi_u8 dot11_multi_domain_capability_activated;                 /* dot11MultiDomainCapabilityActivated TruthValue, */
    hi_u8 dot11_spectrum_management_implemented;                   /* dot11SpectrumManagementImplemented TruthValue, */
    hi_u8 dot11_spectrum_management_required;                      /* dot11SpectrumManagementRequired TruthValue, */
    hi_u8 dot11_qos_option_implemented;                            /* dot11QosOptionImplemented TruthValue, */
    hi_u8 dot11_immediate_block_ack_option_implemented;  /* dot11ImmediateBlockAckOptionImplemented TruthValue, */
    hi_u8 dot11_delayed_block_ack_option_implemented;    /* dot11DelayedBlockAckOptionImplemented TruthValue, */
    hi_u8 dot11_apsd_option_implemented;                 /* dot11APSDOptionImplemented TruthValue, */
    hi_u8 dot11_qbss_load_implemented;                   /* dot11QBSSLoadImplemented TruthValue, */
    hi_u8 dot11_radio_measurement_activated;             /* dot11RadioMeasurementActivated TruthValue, */
    hi_u8 dot11_extended_channel_switch_activated;       /* dot11ExtendedChannelSwitchActivated  TruthValue, */
    hi_u32 dot11_association_sa_query_maximum_timeout;   /* dot11AssociationSAQueryMaximumTimeout Unsigned32, */
    hi_u32 dot11_association_sa_query_retry_timeout;     /* dot11AssociationSAQueryRetryTimeout Unsigned32, */
    hi_u8 dot11_high_throughput_option_implemented;      /* dot11HighThroughputOptionImplemented TruthValue, */
    hi_u8 dot11_mesh_activated;                          /* dot11MeshActivated TruthValue, */
    hi_u8 dot11_operating_mode_notification_implemented; /* dot11OperatingModeNotificationImplemented TruthValue */
} wlan_mib_dot11_station_config_entry_stru;              /* Dot11StationConfigEntry */

typedef struct {
    wlan_mib_auth_alg_enum_uint8 dot11_authentication_algorithm; /* dot11AuthenticationAlgorithm INTEGER, */
    hi_u8 dot11_authentication_algorithms_activated;             /* dot11AuthenticationAlgorithmsActivated TruthValue */
} wlan_mib_dot11_authentication_algorithms_entry_stru;           /* Dot11AuthenticationAlgorithmsEntry */

/* **************************************************************************** */
/* dot11WEPDefaultKeys  TABLE - members of Dot11WEPDefaultKeysEntry           */
/* **************************************************************************** */
/* Conceptual table for WEP default keys. This table contains the four WEP    */
/* default secret key values corresponding to the four possible KeyID values. */
/* The WEP default secret keys are logically WRITE-ONLY. Attempts to read the */
/* entries in this table return unsuccessful status and values of null or 0.  */
/* The default value of each WEP default key is null.                         */
typedef struct {
    hi_u8 auc_dot11_wep_default_key_value[WLAN_WEP_SIZE_OFFSET + 1]; /* dot11WEPDefaultKeyValue WEPKeytype  */
} wlan_mib_dot11_wep_default_keys_entry_stru;

/* **************************************************************************** */
/* dot11WEPKeyMappings  TABLE - members of Dot11WEPKeyMappingsEntry           */
/* **************************************************************************** */
/* Conceptual table for WEP Key Mappings. The MIB supports the ability to     */
/* share a separate WEP key for each RA/TA pair. The Key Mappings Table con-  */
/* tains zero or one entry for each MAC address and contains two fields for   */
/* each entry: WEPOn and the corresponding WEP key. The WEP key mappings are  */
/* logically WRITE-ONLY. Attempts to read the entries in this table return    */
/* unsuccessful status and values of null or 0. The default value for all     */
/* WEPOn fields is false                                                      */
typedef struct {
    /* dot11WEPKeyMappingAddress MacAddress, */
    hi_u8 auc_dot11_wep_key_mapping_address[WLAN_MAC_ADDR_LEN];
    hi_u8 dot11_wep_key_mapping_wep_on; /* dot11WEPKeyMappingWEPOn TruthValue, */
    /* dot11WEPKeyMappingValue WEPKeytype, */
    hi_u8 auc_dot11_wep_key_mapping_value[WLAN_MAX_WEP_STR_SIZE];
} wlan_mib_dot11_wep_key_mappings_entry_stru;

/* **************************************************************************** */
/* dot11Privacy TABLE - members of Dot11PrivacyEntry                          */
/* **************************************************************************** */
/* Group containing attributes concerned with IEEE 802.11 Privacy. Created    */
/* as a table to allow multiple instantiations on an agent.                   */
typedef struct {
    hi_u8 dot11_privacy_invoked;                  /* dot11PrivacyInvoked TruthValue,                 */
    hi_u8 dot11_wep_default_key_id;               /* dot11WEPDefaultKeyID Unsigned8,                 */
    hi_u8 dot11_rsna_activated;                   /* dot11RSNAActivated TruthValue,                  */
    hi_u8 dot11_rsna_preauthentication_activated; /* dot11RSNAPreauthenticationActivated TruthValue  */
    hi_u8 dot11_rsnamfpc;                         /* dot11RSNAManagementFramerProtectionCapbility TruthValue  */
    hi_u8 dot11_rsnamfpr;                         /* dot11RSNAManagementFramerProtectionRequired TruthValue  */
} wlan_mib_dot11_privacy_entry_stru;

/* **************************************************************************** */
/* dot11MultiDomainCapability TABLE - members of Dot11MultiDomainCapabilityEntry            */
/* **************************************************************************** */
/* GThis (conceptual) table of attributes for cross-domain mobility           */
typedef struct {
    hi_u32 dot11_multi_domain_capability_index;  /* dot11MultiDomainCapabilityIndex Unsigned32, */
    hi_u32 dot11_first_channel_number;           /* dot11FirstChannelNumber Unsigned32,         */
    hi_u32 dot11_numberof_channels;              /* dot11NumberofChannels Unsigned32,           */
    hi_s32 l_dot11_maximum_transmit_power_level; /* dot11MaximumTransmitPowerLevel Integer32    */
} wlan_mib_dot11_multi_domain_capability_entry_stru;

/* **************************************************************************** */
/* dot11SpectrumManagement TABLE - members of dot11SpectrumManagementEntry    */
/* **************************************************************************** */
/* An entry (conceptual row) in the Spectrum Management Table.                */
/* IfIndex - Each 802.11 interface is represented by an ifEntry. Interface    */
/* tables in this MIB are indexed by ifIndex.                                 */
typedef struct {
    hi_u32 dot11_spectrum_management_index;          /* dot11SpectrumManagementIndex Unsigned32,      */
    hi_s32 l_dot11_mitigation_requirement;           /* dot11MitigationRequirement Integer32,         */
    hi_u32 dot11_channel_switch_time;                /* dot11ChannelSwitchTime Unsigned32,            */
    hi_s32 l_dot11_power_capability_max_implemented; /* dot11PowerCapabilityMaxImplemented Integer32, */
    hi_s32 l_dot11_power_capability_min_implemented; /* dot11PowerCapabilityMinImplemented Integer32  */
} wlan_mib_dot11_spectrum_management_entry_stru;

/* **************************************************************************** */
/* dot11RSNAConfig TABLE (RSNA and TSN) - members of dot11RSNAConfigEntry     */
/* **************************************************************************** */
/* An entry in the dot11RSNAConfigTable                                       */
typedef struct {
    hi_u32 dot11_rsna_config_version;           /* dot11RSNAConfigVersion Unsigned32, */
    hi_u8  dot11_rsna_config_group_cipher;       /* dot11RSNAConfigGroupCipher OCTET STRING, SIZE(4) */
    hi_u32 dot11_rsna_config_group_cipher_size; /* dot11RSNAConfigGroupCipherSize Unsigned32, */
    /* dot11RSNAConfigNumberOfPTKSAReplayCountersImplemented Unsigned32, */
    hi_u32 dot11_rsna_config_number_of_ptksa_replay_counters_implemented;
    /* dot11RSNAAuthenticationSuiteSelected OCTET STRING,SIZE(4) */
    hi_u8 dot11_rsna_authentication_suite_selected;
    hi_u8 dot11_rsna_pairwise_cipher_requested; /* dot11RSNAPairwiseCipherRequested OCTET STRING, SIZE(4) */
    hi_u8 dot11_rsna_group_cipher_requested;    /* dot11RSNAGroupCipherRequested OCTET STRING, SIZE(4) */
    /* dot11RSNAConfigNumberOfGTKSAReplayCountersImplemented Unsigned32, */
    hi_u32 dot11_rsna_config_number_of_gtksa_replay_counters_implemented;
} wlan_mib_dot11_rsna_config_entry_stru;

/* *************************************************************************************** */
/* dot11RSNAConfigPairwiseCiphers TABLE - members of dot11RSNAConfigPairwiseCiphersEntry */
/* *************************************************************************************** */
/* This table lists the pairwise ciphers supported by this entity. It allows  */
/* enabling and disabling of each pairwise cipher by network management. The  */
/* pairwise cipher suite list in the RSN element is formed using the informa- */
/* tion in this table.                                                        */
typedef struct {
    /* dot11RSNAConfigPairwiseCipherImplemented OCTET STRING, */
    hi_u8 dot11_rsna_config_pairwise_cipher_implemented;
    hi_u8 dot11_rsna_config_pairwise_cipher_activated; /* dot11RSNAConfigPairwiseCipherActivated TruthValue, */
} wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru;

/* ************************************************************************************************* */
/* dot11RSNAConfigAuthenticationSuites TABLE - members of Dot11RSNAConfigAuthenticationSuitesEntry */
/* ************************************************************************************************* */
/* This table lists the AKM suites supported by this entity. Each AKM suite */
/* can be individually enabled and disabled. The AKM suite list in the RSN  */
/* element is formed using the information in this table                    */
typedef struct {
    /* dot11RSNAConfigAuthenticationSuiteImplemented  OCTET STRING, SIZE(4) */
    hi_u8 dot11_rsna_config_authentication_suite_implemented;
    hi_u8 dot11_rsna_config_authentication_suite_activated; /* dot11RSNAConfigAuthenticationSuiteActivated TruthValue */
} wlan_mib_dot11_rsna_cfg_authentication_suites_en_stru;

/* ************************************************************************** */
/* dot11RSNAStats TABLE - members of Dot11RSNAStatsEntry                    */
/* ************************************************************************** */
/* This table maintains per-STA statistics in an RSN. The entry with        */
/* dot11RSNAStatsSTAAddress equal to FF-FF-FF-FF-FF-FF contains statistics  */
/* for group addressed traffic                                              */
typedef struct {
    hi_u32 dot11_rsna_stats_index;                      /* dot11RSNAStatsIndex Unsigned32, */
    hi_u8  auc_dot11_rsna_stats_sta_address[6];         /* dot11RSNAStatsSTAAddress MacAddress, SIZE(0..6) */
    hi_u8  dot11_rsna_stats_selected_pairwise_cipher;   /* dot11RSNAStatsSelectedPairwiseCipher OCTET STRING, SIZE(4) */
    hi_u8  resv;
    hi_u32 dot11_rsna_stats_version;                    /* dot11RSNAStatsVersion Unsigned32, */
    hi_u32 dot11_rsna_stats_cmacicv_errors;             /* dot11RSNAStatsCMACICVErrors Counter32, */
    hi_u32 dot11_rsna_stats_cmac_replays;               /* dot11RSNAStatsCMACReplays Counter32, */
} wlan_mib_dot11_rsna_stats_entry_stru;

/* ********************************************************************* */
/* dot11OperatingClasses TABLE - members of Dot11OperatingClassesEntry */
/* ********************************************************************* */
/* (Conceptual) table of attributes for operating classes       */
typedef struct {
    hi_u32 dot11_operating_classes_index; /* dot11OperatingClassesIndex Unsigned32,   */
    hi_u32 dot11_operating_class;         /* dot11OperatingClass Unsigned32,          */
    hi_u32 dot11_coverage_class;          /* dot11CoverageClass Unsigned32            */
} wlan_mib_dot11_operating_classes_entry_stru;

/* ********************************************************************* */
/* dot11RMRequest TABLE  - members of dot11RadioMeasurement         */
/* ********************************************************************* */
typedef struct {
    /* dot11RMRequestNextIndex ::= { dot11RMRequest 1 } */
    hi_u32 dot11_rm_request_next_index; /* dot11RMRequestNextIndex  Unsigned32 */

    /*  dot11RMRequestTable OBJECT-TYPE ::= { dot11RMRequest 2 } */
    hi_u32 dot11_rm_rqst_index;                                      /* dot11RMRqstIndex Unsigned32, */
    wlan_mib_row_status_enum_uint8 dot11_rm_rqst_row_status;         /* dot11RMRqstRowStatus RowStatus, */
    hi_u8 auc_dot11_rm_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH]; /* dot11RMRqstToken OCTET STRING, */
    hi_u32 dot11_rm_rqst_repetitions;                                /* dot11RMRqstRepetitions Unsigned32,  */
    hi_u32 dot11_rm_rqst_if_index;                                   /* dot11RMRqstIfIndex InterfaceIndex, */
    wlan_mib_rmrqst_type_enum_uint16 dot11_rm_rqst_type;             /* dot11RMRqstType INTEGER, */
    hi_u8 auc_dot11_rm_rqst_target_add[6];                           /* dot11RMRqstTargetAdd MacAddress, SIZE(0..6)  */
    hi_u32 dot11_rm_rqst_time_stamp;                                 /* dot11RMRqstTimeStamp TimeTicks,   */
    hi_u32 dot11_rm_rqst_chan_number;                                /* dot11RMRqstChanNumber Unsigned32, */
    hi_u32 dot11_rm_rqst_operating_class;                            /* dot11RMRqstOperatingClass Unsigned32,  */
    hi_u32 dot11_rm_rqst_rnd_interval;                               /* dot11RMRqstRndInterval Unsigned32, */
    hi_u32 dot11_rm_rqst_duration;                                   /* dot11RMRqstDuration Unsigned32, */
    hi_u8 dot11_rm_rqst_parallel;                                    /* dot11RMRqstParallel TruthValue, */
    hi_u8 dot11_rm_rqst_enable;                                      /* dot11RMRqstEnable TruthValue, */
    hi_u8 dot11_rm_rqst_request;                                     /* dot11RMRqstRequest TruthValue, */
    hi_u8 dot11_rm_rqst_report;                                      /* dot11RMRqstReport TruthValue, */
    hi_u8 dot11_rm_rqst_duration_mandatory;                          /* dot11RMRqstDurationMandatory TruthValue, */
    wlan_mib_rmrqst_beaconrqst_mode_enum_uint8 dot11_rm_rqst_beacon_rqst_mode; /* dot11RMRqstBeaconRqstMode INTEGER, */
    /* dot11RMRqstBeaconRqstDetail INTEGER, */
    wlan_mib_rmrqst_beaconrqst_detail_enum_uint8 dot11_rm_rqst_beacon_rqst_detail;
    wlan_mib_rmrqst_famerqst_type_enum_uint8 dot11_rm_rqst_frame_rqst_type; /* dot11RMRqstFrameRqstType INTEGER, */
    hi_u8 auc_dot11_rm_rqst_bssid[6]; /* dot11RMRqstBssid MacAddress, SIZE(0..6) */
    hi_u8 dot11_rm_rqst_ssid[32];     /* dot11RMRqstSSID OCTET STRING, SIZE(0..32) */
    /* dot11RMRqstBeaconReportingCondition INTEGER, */
    wlan_mib_rmrqst_beaconrpt_cdt_enum_uint8 dot11_rm_rqst_beacon_reporting_condition;
    hi_s32 l_dot11_rm_rqst_beacon_threshold_offset; /* dot11RMRqstBeaconThresholdOffset Integer32, */
    /* dot11RMRqstSTAStatRqstGroupID INTEGER, */
    wlan_mib_rmrqst_stastatrqst_grpid_enum_uint8 dot11_rm_rqst_sta_stat_rqst_group_id;
    /* dot11RMRqstLCIRqstSubject INTEGER, */
    wlan_mib_rmrqst_lcirpt_subject_enum_uint8 dot11_rm_rqst_lci_rqst_subject;
    hi_u32 dot11_rm_rqst_lci_latitude_resolution;  /* dot11RMRqstLCILatitudeResolution Unsigned32, */
    hi_u32 dot11_rm_rqst_lci_longitude_resolution; /* dot11RMRqstLCILongitudeResolution Unsigned32,  */
    hi_u32 dot11_rm_rqst_lci_altitude_resolution;  /* dot11RMRqstLCIAltitudeResolution Unsigned32, */
    wlan_mib_rmrqst_lciazimuth_type_enum_uint8 dot11_rm_rqst_lci_azimuth_type; /* dot11RMRqstLCIAzimuthType INTEGER, */
    hi_u32 dot11_rm_rqst_lci_azimuth_resolution; /* dot11RMRqstLCIAzimuthResolution Unsigned32, */
    hi_u32 dot11_rm_rqst_pause_time;             /* dot11RMRqstPauseTime Unsigned32, */
    /* dot11RMRqstTransmitStreamPeerQSTAAddress MacAddress, */
    hi_u8 auc_dot11_rm_rqst_transmit_stream_peer_qsta_address[6]; /* SIZE(0..6) */
    /* dot11RMRqstTransmitStreamTrafficIdentifier Unsigned32, */
    hi_u32 dot11_rm_rqst_transmit_stream_traffic_identifier;
    /* dot11RMRqstTransmitStreamBin0Range Unsigned32, */
    hi_u32 dot11_rm_rqst_transmit_stream_bin0_range;
    /* dot11RMRqstTrigdQoSAverageCondition TruthValue, */
    hi_u8 dot11_rm_rqst_trigd_qo_s_average_condition;
    /* dot11RMRqstTrigdQoSConsecutiveCondition TruthValue, */
    hi_u8 dot11_rm_rqst_trigd_qo_s_consecutive_condition;
    /* dot11RMRqstTrigdQoSAverageThreshold Unsigned32, */
    hi_u32 dot11_rm_rqst_trigd_qo_s_average_threshold;
    /* dot11RMRqstTrigdQoSConsecutiveThreshold Unsigned32, */
    hi_u32 dot11_rm_rqst_trigd_qo_s_consecutive_threshold;
    /* dot11RMRqstTrigdQoSDelayThresholdRange Unsigned32, */
    hi_u32 dot11_rm_rqst_trigd_qo_s_delay_threshold_range;
    hi_u32 dot11_rm_rqst_trigd_qo_s_delay_threshold;   /* dot11RMRqstTrigdQoSDelayThreshold Unsigned32, */
    hi_u32 dot11_rm_rqst_trigd_qo_s_measurement_count; /* dot11RMRqstTrigdQoSMeasurementCount Unsigned32, */
    hi_u32 dot11_rm_rqst_trigd_qo_s_timeout;           /* dot11RMRqstTrigdQoSTimeout Unsigned32, */
    /* dot11RMRqstChannelLoadReportingCondition INTEGER, */
    wlan_mib_rmrqst_ch_loadrpt_cdt_type_enum_uint8 dot11_rm_rqst_channel_load_reporting_condition;
    hi_u32 dot11_rm_rqst_channel_load_reference; /* dot11RMRqstChannelLoadReference Unsigned32, */
    /* dot11RMRqstNoiseHistogramReportingCondition INTEGER, */
    wlan_mib_rmrqst_noise_histgrpt_cdt_type_enum_uint8 dot11_rm_rqst_noise_histogram_reporting_condition;
    hi_u32 dot11_rm_rqst_anpi_reference; /* dot11RMRqstAnpiReference Unsigned32, */
    /* dot11RMRqstAPChannelReport OCTET STRING, SIZE(0..255) */
    hi_u8 auc_dot11_rm_rqst_ap_channel_report[255]; /* SIZE(0..255) */
    /* dot11RMRqstSTAStatPeerSTAAddress MacAddress, */
    hi_u8 auc_dot11_rm_rqst_sta_stat_peer_sta_address[6]; /* SIZE(0..6) */
    /* dot11RMRqstFrameTransmitterAddress MacAddress,  */
    hi_u8 auc_dot11_rm_rqst_frame_transmitter_address[6]; /* SIZE(0..6) */
    hi_u8 auc_dot11_rm_rqst_vendor_specific[255];         /* dot11RMRqstVendorSpecific OCTET STRING,SIZE(0..255)  */
    hi_u32 dot11_rm_rqst_sta_stat_trig_meas_count;        /* dot11RMRqstSTAStatTrigMeasCount Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_timeout;           /* dot11RMRqstSTAStatTrigTimeout Unsigned32, */
    /* dot11RMRqstSTAStatTrigCondition OCTET STRING, SIZE(2) */
    hi_u8 auc_dot11_rm_rqst_sta_stat_trig_condition[2]; /* SIZE(2) */
    /* dot11RMRqstSTAStatTrigSTAFailedCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_sta_failed_cnt_thresh;
    /* dot11RMRqstSTAStatTrigSTAFCSErrCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_stafcs_err_cnt_thresh;
    /* dot11RMRqstSTAStatTrigSTAMultRetryCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_sta_mult_retry_cnt_thresh;
    /* dot11RMRqstSTAStatTrigSTAFrameDupeCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_sta_frame_dupe_cnt_thresh;
    /* dot11RMRqstSTAStatTrigSTARTSFailCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_starts_fail_cnt_thresh;
    /* dot11RMRqstSTAStatTrigSTAAckFailCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_sta_ack_fail_cnt_thresh;
    /* dot11RMRqstSTAStatTrigSTARetryCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_sta_retry_cnt_thresh;
    /* dot11RMRqstSTAStatTrigQoSTrigCondition OCTET STRING, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_qo_s_trig_condition;
    /* dot11RMRqstSTAStatTrigQoSFailedCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_qo_s_failed_cnt_thresh;
    /* dot11RMRqstSTAStatTrigQoSRetryCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_qo_s_retry_cnt_thresh;
    /* dot11RMRqstSTAStatTrigQoSMultRetryCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_qo_s_mult_retry_cnt_thresh;
    /* dot11RMRqstSTAStatTrigQoSFrameDupeCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_qo_s_frame_dupe_cnt_thresh;
    /* dot11RMRqstSTAStatTrigQoSRTSFailCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_qo_srts_fail_cnt_thresh;
    /* dot11RMRqstSTAStatTrigQoSAckFailCntThresh  Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_qo_s_ack_fail_cnt_thresh;
    /* dot11RMRqstSTAStatTrigQoSDiscardCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_qo_s_discard_cnt_thresh;
    /* dot11RMRqstSTAStatTrigRsnaTrigCondition OCTET STRING,SIZE(2) */
    hi_u8 auc_dot11_rm_rqst_sta_stat_trig_rsna_trig_condition[2]; /* SIZE(2) */
    /* dot11RMRqstSTAStatTrigRsnaCMACICVErrCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_rsna_cmacicv_err_cnt_thresh;
    /* dot11RMRqstSTAStatTrigRsnaCMACReplayCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_rsna_cmac_replay_cnt_thresh;
    /* dot11RMRqstSTAStatTrigRsnaRobustCCMPReplayCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_rsna_robust_ccmp_replay_cnt_thresh;
    /* dot11RMRqstSTAStatTrigRsnaTKIPICVErrCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_rsna_tkipicv_err_cnt_thresh;
    /* dot11RMRqstSTAStatTrigRsnaTKIPReplayCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_rsna_tkip_replay_cnt_thresh;
    /* dot11RMRqstSTAStatTrigRsnaCCMPDecryptErrCntThresh Unsigned32, */
    hi_u32 dot11_rm_rqst_sta_stat_trig_rsna_ccmp_decrypt_err_cnt_thresh;
    /* dot11RMRqstSTAStatTrigRsnaCCMPReplayCntThresh Unsigned32 */
    hi_u32 dot11_rm_rqst_sta_stat_trig_rsna_ccmp_replay_cnt_thresh;
} wlan_mib_dot11_radio_measurement_stru;

/* ************************************************************************************ */
/* dot11FastBSSTransitionConfig TABLE  - members of Dot11FastBSSTransitionConfigEntry */
/* ************************************************************************************ */
/* The table containing fast BSS transition configuration objects                     */
typedef struct {
    hi_u8 dot11_fast_bss_transition_activated; /* dot11FastBSSTransitionActivated TruthValue, */
    hi_u8 auc_dot11_ft_mobility_domain_id[2];  /* dot11FTMobilityDomainID OCTET STRING,SIZE(2) */
    hi_u8 dot11_ft_over_ds_activated;          /* dot11FTOverDSActivated TruthValue,          */
    hi_u8 dot11_ft_resource_request_supported; /* dot11FTResourceRequestSupported TruthValue, */
} wlan_mib_dot11_fast_bss_transition_config_entry_stru;

/* ********************************************************************** */
/* dot11LCIDSE TABLE  - members of Dot11LCIDSEEntry                     */
/* ********************************************************************** */
/* Group contains conceptual table of attributes for Dependent Station  */
/* Enablement.                                                          */
typedef struct {
    hi_u32 dot11_lcidse_index;                   /* dot11LCIDSEIndex Unsigned32, */
    hi_u32 dot11_lcidse_if_index;                /* dot11LCIDSEIfIndex InterfaceIndex, */
    hi_u32 dot11_lcidse_current_operating_class; /* dot11LCIDSECurrentOperatingClass Unsigned32, */
    hi_u32 dot11_lcidse_latitude_resolution;     /* dot11LCIDSELatitudeResolution Unsigned32, */
    hi_s32 l_dot11_lcidse_latitude_integer;      /* dot11LCIDSELatitudeInteger Integer32, */
    hi_s32 l_dot11_lcidse_latitude_fraction;     /* dot11LCIDSELatitudeFraction Integer32, */
    hi_u32 dot11_lcidse_longitude_resolution;    /* dot11LCIDSELongitudeResolution Unsigned32, */
    hi_s32 l_dot11_lcidse_longitude_integer;     /* dot11LCIDSELongitudeInteger Integer32, */
    hi_s32 l_dot11_lcidse_longitude_fraction;    /* dot11LCIDSELongitudeFraction Integer32, */
    wlan_mib_lci_dsealtitude_type_enum_uint8 dot11_lcidse_altitude_type; /* dot11LCIDSEAltitudeType INTEGER, */
    hi_u8 dot11_reg_loc_agreement;                                       /* dot11RegLocAgreement TruthValue, */
    hi_u8 dot11_reg_loc_dse;                                             /* dot11RegLocDSE TruthValue, */
    hi_u8 dot11_dependent_sta;                                           /* dot11DependentSTA TruthValue, */
    hi_u32 dot11_lcidse_altitude_resolution;                             /* dot11LCIDSEAltitudeResolution Unsigned32, */
    hi_s32 l_dot11_lcidse_altitude_integer;                              /* dot11LCIDSEAltitudeInteger Integer32, */
    hi_s32 l_dot11_lcidse_altitude_fraction;                             /* dot11LCIDSEAltitudeFraction Integer32, */
    hi_u32 dot11_lcidse_datum;                                           /* dot11LCIDSEDatum Unsigned32, */
    hi_u32 dot11_dependent_enablement_identifier; /* dot11DependentEnablementIdentifier Unsigned32, */
    hi_u32 dot11_dse_enablement_time_limit;       /* dot11DSEEnablementTimeLimit Unsigned32, */
    hi_u32 dot11_dse_enablement_fail_hold_time;   /* dot11DSEEnablementFailHoldTime Unsigned32, */
    hi_u32 dot11_dse_renewal_time;                /* dot11DSERenewalTime Unsigned32, */
    hi_u32 dot11_dse_transmit_divisor;            /* dot11DSETransmitDivisor Unsigned32 */
} wlan_mib_dot11_lcidse_entry_stru;

/* ************************************************************************************ */
/* dot11HTStationConfig TABLE  - members of Dot11HTStationConfigEntry                 */
/* ************************************************************************************ */
/* Station Configuration attributes. In tabular form to allow for multiple            */
/* instances on an agent.                                                             */
typedef struct {
    wlan_mib_mimo_power_save_enum_uint8  dot11_mimo_power_save;   /* dot11MIMOPowerSave INTEGER, */
    wlan_mib_max_amsdu_lenth_enum_uint16 dot11_max_amsdu_length; /* dot11MaxAMSDULength INTEGER, */
    hi_u8 dot11_lsig_txop_protection_option_implemented; /* dot11LsigTxopProtectionOptionImplemented TruthValue, */
    hi_u32 dot11_max_rx_ampdu_factor;                    /* dot11MaxRxAMPDUFactor Unsigned32, */
    hi_u32 dot11_minimum_mpdu_start_spacing;             /* dot11MinimumMPDUStartSpacing Unsigned32, */
    hi_u8  dot11_pco_option_implemented;                  /* dot11PCOOptionImplemented TruthValue, */
    /* dot11MCSFeedbackOptionImplemented INTEGER, */
    wlan_mib_mcs_feedback_opt_implt_enum_uint8 dot11_mcs_feedback_option_implemented;
    hi_u8 dot11_ht_control_field_supported;      /* dot11HTControlFieldSupported TruthValue, */
    hi_u8 dot11_rd_responder_option_implemented; /* dot11RDResponderOptionImplemented TruthValue, */
    hi_u32 dot11_transition_time;                /* dot11TransitionTime Unsigned32, */
} wlan_mib_dot11_ht_station_config_entry_stru;

/* ************************************************************************************ */
/* dot11WirelessMgmtOptions TABLE  - members of Dot11WirelessMgmtOptionsEntry         */
/* ************************************************************************************ */
/* Wireless Management attributes. In tabular form to allow for multiple      */
/* instances on an agent. This table only applies to the interface if         */
/* dot11WirelessManagementImplemented is set to true in the                   */
/* dot11StationConfigTable. Otherwise this table should be ignored.           */
/* For all Wireless Management features, an Activated MIB variable is used    */
/* to activate/enable or deactivate/disable the corresponding feature.        */
/* An Implemented MIB variable is used for an optional feature to indicate    */
/* whether the feature is implemented. A mandatory feature does not have a    */
/* corresponding Implemented MIB variable. It is possible for there to be     */
/* multiple IEEE 802.11 interfaces on one agent, each with its unique MAC     */
/* address. The relationship between an IEEE 802.11 interface and an          */
/* interface in the context of the Internet standard MIB is one-to-one.       */
/* As such, the value of an ifIndex object instance can be directly used      */
/* to identify corresponding instances of the objects defined herein.         */
/* ifIndex - Each IEEE 802.11 interface is represented by an ifEntry.         */
/* Interface tables in this MIB module are indexed by ifIndex.                */
typedef struct {
    hi_u8 dot11_mgmt_option_location_activated;         /* dot11MgmtOptionLocationActivated TruthValue, */
    hi_u8 dot11_mgmt_option_fms_implemented;            /* dot11MgmtOptionFMSImplemented TruthValue,  */
    hi_u8 dot11_mgmt_option_fms_activated;              /* dot11MgmtOptionFMSActivated TruthValue, */
    hi_u8 dot11_mgmt_option_events_activated;           /* dot11MgmtOptionEventsActivated TruthValue,  */
    hi_u8 dot11_mgmt_option_diagnostics_activated;      /* dot11MgmtOptionDiagnosticsActivated TruthValue, */
    hi_u8 dot11_mgmt_option_multi_bssid_implemented;    /* dot11MgmtOptionMultiBSSIDImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_multi_bssid_activated;      /* dot11MgmtOptionMultiBSSIDActivated TruthValue, */
    hi_u8 dot11_mgmt_option_tfs_implemented;            /* dot11MgmtOptionTFSImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_tfs_activated;              /* dot11MgmtOptionTFSActivated TruthValue, */
    hi_u8 dot11_mgmt_option_wnm_sleep_mode_implemented; /* dot11MgmtOptionWNMSleepModeImplemented TruthValue,  */
    hi_u8 dot11_mgmt_option_wnm_sleep_mode_activated;   /* dot11MgmtOptionWNMSleepModeActivated TruthValue, */
    hi_u8 dot11_mgmt_option_tim_broadcast_implemented;  /* dot11MgmtOptionTIMBroadcastImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_tim_broadcast_activated;    /* dot11MgmtOptionTIMBroadcastActivated TruthValue, */
    hi_u8 dot11_mgmt_option_proxy_arp_implemented;      /* dot11MgmtOptionProxyARPImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_proxy_arp_activated;        /* dot11MgmtOptionProxyARPActivated TruthValue, */
    hi_u8 dot11_mgmt_option_bss_transition_implemented; /* dot11MgmtOptionBSSTransitionImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_bss_transition_activated;   /* dot11MgmtOptionBSSTransitionActivated TruthValue, */
    /* dot11MgmtOptionQoSTrafficCapabilityImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_qo_s_traffic_capability_implemented;
    /* dot11MgmtOptionQoSTrafficCapabilityActivated TruthValue, */
    hi_u8 dot11_mgmt_option_qo_s_traffic_capability_activated;
    hi_u8 dot11_mgmt_option_ac_station_count_implemented; /* dot11MgmtOptionACStationCountImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_ac_station_count_activated;   /* dot11MgmtOptionACStationCountActivated TruthValue, */
    /* dot11MgmtOptionCoLocIntfReportingImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_co_loc_intf_reporting_implemented;
    /* dot11MgmtOptionCoLocIntfReportingActivated TruthValue, */
    hi_u8 dot11_mgmt_option_co_loc_intf_reporting_activated;
    hi_u8 dot11_mgmt_option_motion_detection_implemented; /* dot11MgmtOptionMotionDetectionImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_motion_detection_activated;   /* dot11MgmtOptionMotionDetectionActivated TruthValue, */
    hi_u8 dot11_mgmt_option_tod_implemented;              /* dot11MgmtOptionTODImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_tod_activated;                /* dot11MgmtOptionTODActivated TruthValue, */
    hi_u8 dot11_mgmt_option_timing_msmt_implemented;      /* dot11MgmtOptionTimingMsmtImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_timing_msmt_activated;        /* dot11MgmtOptionTimingMsmtActivated TruthValue, */
    hi_u8 dot11_mgmt_option_channel_usage_implemented;    /* dot11MgmtOptionChannelUsageImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_channel_usage_activated;      /* dot11MgmtOptionChannelUsageActivated TruthValue, */
    /* dot11MgmtOptionTriggerSTAStatisticsActivated TruthValue, */
    hi_u8 dot11_mgmt_option_trigger_sta_statistics_activated;
    hi_u8 dot11_mgmt_option_ssid_list_implemented; /* dot11MgmtOptionSSIDListImplemented TruthValue,  */
    hi_u8 dot11_mgmt_option_ssid_list_activated;   /* dot11MgmtOptionSSIDListActivated TruthValue, */
    /* dot11MgmtOptionMulticastDiagnosticsActivated TruthValue, */
    hi_u8 dot11_mgmt_option_multicast_diagnostics_activated;
    hi_u8 dot11_mgmt_option_location_tracking_implemented; /* dot11MgmtOptionLocationTrackingImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_location_tracking_activated;   /* dot11MgmtOptionLocationTrackingActivated TruthValue, */
    hi_u8 dot11_mgmt_option_dms_implemented;               /* dot11MgmtOptionDMSImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_dms_activated;                 /* dot11MgmtOptionDMSActivated TruthValue, */
    hi_u8 dot11_mgmt_option_uapsd_coexistence_implemented; /* dot11MgmtOptionUAPSDCoexistenceImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_uapsd_coexistence_activated;   /* dot11MgmtOptionUAPSDCoexistenceActivated  TruthValue, */
    hi_u8 dot11_mgmt_option_wnm_notification_implemented;  /* dot11MgmtOptionWNMNotificationImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_wnm_notification_activated;    /* dot11MgmtOptionWNMNotificationActivated TruthValue, */
    hi_u8 dot11_mgmt_option_utctsf_offset_implemented;     /* dot11MgmtOptionUTCTSFOffsetImplemented TruthValue, */
    hi_u8 dot11_mgmt_option_utctsf_offset_activated;       /* dot11MgmtOptionUTCTSFOffsetActivated TruthValue */
} wlan_mib_dot11_wireless_mgmt_options_entry_stru;

/* dot11LocationServicesLocationIndicationChannelList OBJECT-TYPE         */
/* SYNTAX OCTET STRING (SIZE (2..254))                                    */
/* MAX-ACCESS read-create                                                 */
/* STATUS current                                                         */
/* DESCRIPTION                                                            */
/* "This attribute contains one or more Operating Class and Channel octet */
/* pairs."                                                                */
/* ::= { dot11LocationServicesEntry 12}                                   */
typedef struct {
    hi_u8 channel_nums;
    hi_u8 *channel_list;
} wlan_mib_local_serv_location_ind_ch_list_stru;

/* ****************************************************************************** */
/* dot11LocationServices TABLE  - members of Dot11LocationServicesEntry         */
/* ****************************************************************************** */
/* Identifies a hint for the next value of dot11LocationServicesIndex to be     */
/* used in a row creation attempt for dot11LocationServicesTable. If no new     */
/* rows can be created for some reason, such as memory, processing require-     */
/* ments, etc, the SME shall set this attribute to 0. It shall update this      */
/* attribute to a proper value other than 0 as soon as it is capable of         */
/* receiving new measurement requests. The nextIndex is not necessarily         */
/* sequential nor monotonically increasing.                                     */
typedef struct {
    hi_u32 dot11_location_services_index;             /* dot11LocationServicesIndex  Unsigned32, */
    hi_u8 auc_dot11_location_services_mac_address[6]; /* dot11LocationServicesMACAddress  MacAddress, SIZE(0..6) */
    /* dot11LocationServicesLIPIndicationMulticastAddress MacAddress, */
    hi_u8 auc_dot11_location_services_lip_indication_multicast_address[6]; /* SIZE(0..6) */
    /* dot11LocationServicesLIPReportIntervalUnits INTEGER, */
    wlan_mib_lct_servs_liprpt_interval_unit_enum_uint8 dot11_location_services_lip_report_interval_units;
    /* dot11LocationServicesLIPNormalReportInterval Unsigned32, */
    hi_u32 dot11_location_services_lip_normal_report_interval;
    /* dot11LocationServicesLIPNormalFramesperChannel Unsigned32, */
    hi_u32 dot11_location_services_lip_normal_framesper_channel;
    /* dot11LocationServicesLIPInMotionReportInterval Unsigned32, */
    hi_u32 dot11_location_services_lip_in_motion_report_interval;
    /* dot11LocationServicesLIPInMotionFramesperChannel Unsigned32, */
    hi_u32 dot11_location_services_lip_in_motion_framesper_channel;
    /* dot11LocationServicesLIPBurstInterframeInterval Unsigned32, */
    hi_u32 dot11_location_services_lip_burst_interframe_interval;
    hi_u32 dot11_location_services_lip_tracking_duration; /* dot11LocationServicesLIPTrackingDuration Unsigned32, */
    /* dot11LocationServicesLIPEssDetectionInterval Unsigned32, */
    hi_u32 dot11_location_services_lip_ess_detection_interval;
    /* dot11LocationServicesLocationIndicationChannelList OCTET STRING, */
    wlan_mib_local_serv_location_ind_ch_list_stru dot11_location_services_location_indication_channel_list;
    /* dot11LocationServicesLocationIndicationBroadcastDataRate Unsigned32, */
    hi_u32 dot11_location_services_location_indication_broadcast_data_rate;
    /* dot11LocationServicesLocationIndicationOptionsUsed  OCTET STRING, SIZE(1) */
    hi_u8 dot11_location_services_location_indication_options_used;
    /* dot11LocationServicesLocationIndicationIndicationParameters  OCTET STRING, */
    wlan_mib_local_serv_location_ind_ind_para_stru dot11_location_services_location_indication_indication_parameters;
    hi_u32 dot11_location_services_location_status; /* dot11LocationServicesLocationStatus  Unsigned32 */
} wlan_mib_dot11_location_services_entry_stru;

/* ******************************************************************************* */
/* dot11WirelessMGTEvent TABLE  - members of Dot11WirelessMGTEventEntry          */
/* ******************************************************************************* */
/* Group contains the current list of WIRELESS Management reports that have      */
/* been received by the MLME. The report tables shall be maintained as FIFO      */
/* to preserve freshness, thus the rows in this table can be deleted for mem-    */
/* ory constraints or other implementation constraints determined by the ven-    */
/* dor. New rows shall have different RprtIndex values than those deleted within */
/* the range limitation of the index. One easy way is to monotonically           */
/* increase the EventIndex for new reports being written in the table*           */
typedef struct {
    hi_u32 dot11_wireless_mgt_event_index;             /* dot11WirelessMGTEventIndex Unsigned32, */
    hi_u8 auc_dot11_wireless_mgt_event_mac_address[6]; /* dot11WirelessMGTEventMACAddress MacAddress, SIZE(0..6) */
    wlan_mib_wireless_mgt_event_type_enum_uint8 dot11_wireless_mgt_event_type; /* dot11WirelessMGTEventType INTEGER, */
    /* dot11WirelessMGTEventStatus INTEGER, */
    wlan_mib_wireless_mgt_event_status_enum_uint8 dot11_wireless_mgt_event_status;
    hi_u8 auc_dot11_wireless_mgt_event_tsf[8];         /* dot11WirelessMGTEventTSF TSFType, SIZE(8)  */
    hi_u8 auc_dot11_wireless_mgt_event_utc_offset[10]; /* dot11WirelessMGTEventUTCOffset OCTET STRING,  SIZE(10) */
    hi_u8 auc_dot11_wireless_mgt_event_time_error[5];  /* dot11WirelessMGTEventTimeError OCTET STRING,  SIZE(5) */
    /* dot11WirelessMGTEventTransitionSourceBSSID MacAddress, */
    hi_u8 auc_dot11_wireless_mgt_event_transition_source_bssid[6]; /* SIZE(0..6) */
    /* dot11WirelessMGTEventTransitionTargetBSSID MacAddress, */
    hi_u8 auc_dot11_wireless_mgt_event_transition_target_bssid[6]; /* SIZE(0..6) */
    hi_u32 dot11_wireless_mgt_event_transition_time;               /* dot11WirelessMGTEventTransitionTime Unsigned32, */
    /* dot11WirelessMGTEventTransitionReason INTEGER, */
    wlan_mib_wireless_mgt_event_transit_reason_enum_uint8 dot11_wireless_mgt_event_transition_reason;
    hi_u32 dot11_wireless_mgt_event_transition_result;      /* dot11WirelessMGTEventTransitionResult Unsigned32, */
    hi_u32 dot11_wireless_mgt_event_transition_source_rcpi; /* dot11WirelessMGTEventTransitionSourceRCPI Unsigned32, */
    hi_u32 dot11_wireless_mgt_event_transition_source_rsni; /* dot11WirelessMGTEventTransitionSourceRSNI Unsigned32, */
    hi_u32 dot11_wireless_mgt_event_transition_target_rcpi; /* dot11WirelessMGTEventTransitionTargetRCPI Unsigned32, */
    hi_u32 dot11_wireless_mgt_event_transition_target_rsni; /* dot11WirelessMGTEventTransitionTargetRSNI Unsigned32, */
    /* dot11WirelessMGTEventRSNATargetBSSID MacAddress */
    hi_u8 auc_dot11_wireless_mgt_event_rsna_target_bssid[6]; /* SIZE(0..6) */
    /* dot11WirelessMGTEventRSNAAuthenticationType OCTET STRING, */
    hi_u8 auc_dot11_wireless_mgt_event_rsna_authentication_type[4]; /* SIZE(0..4) */
    /* dot11WirelessMGTEventRSNAEAPMethod OCTET STRING,SIZE (1..8) */
    hi_u8 auc_dot11_wireless_mgt_event_rsnaeap_method[8]; /* SIZE(0..8) */
    hi_u32 dot11_wireless_mgt_event_rsna_result;          /* dot11WirelessMGTEventRSNAResult Unsigned32, */
    /* dot11WirelessMGTEventRSNARSNElement OCTET STRING,SIZE(0..257) */
    hi_u8 auc_dot11_wireless_mgt_event_rsnarsn_element[257]; /* SIZE(0..257) */
    hi_u8 dot11_wireless_mgt_event_peer_sta_address;         /* dot11WirelessMGTEventPeerSTAAddress MacAddress, */
    hi_u32 dot11_wireless_mgt_event_peer_operating_class;    /* dot11WirelessMGTEventPeerOperatingClass Unsigned32, */
    hi_u32 dot11_wireless_mgt_event_peer_channel_number;     /* dot11WirelessMGTEventPeerChannelNumber Unsigned32, */
    hi_s32 l_dot11_wireless_mgt_event_peer_sta_tx_power;     /* dot11WirelessMGTEventPeerSTATxPower Integer32, */
    hi_u32 dot11_wireless_mgt_event_peer_connection_time;    /* dot11WirelessMGTEventPeerConnectionTime Unsigned32, */
    hi_u32 dot11_wireless_mgt_event_peer_peer_status;        /* dot11WirelessMGTEventPeerPeerStatus Unsigned32, */
    hi_u8 auc_dot11_wireless_mgt_event_wnm_log[2284]; /* dot11WirelessMGTEventWNMLog OCTET STRING SIZE(0..2284) */
} wlan_mib_dot11_wireless_mgt_event_entry_stru;

typedef struct {
    /* dot11WNMRequest OBJECT IDENTIFIER ::= { dot11WirelessNetworkManagement 1 } */
    /* dot11WNMRequestNextIndex Unsigned32(0..4294967295) ::= { dot11WNMRequest 1 } */
    hi_u32 dot11_wnm_request_next_index;

    /* dot11WNMRequestTable ::= { dot11WNMRequest 2 } */
    hi_u32 dot11_wnm_rqst_index;                                      /* dot11WNMRqstIndex Unsigned32, */
    wlan_mib_row_status_enum_uint8 dot11_wnm_rqst_row_status;         /* dot11WNMRqstRowStatus RowStatus, */
    hi_u8 auc_dot11_wnm_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH]; /* dot11WNMRqstToken OCTET STRING, */
    hi_u32 dot11_wnm_rqst_if_index;                                   /* dot11WNMRqstIfIndex InterfaceIndex, */
    wlan_mib_wnm_rqst_type_enum_uint8 dot11_wnm_rqst_type;            /* dot11WNMRqstType INTEGER, */
    hi_u8 auc_dot11_wnm_rqst_target_add[6];                           /* dot11WNMRqstTargetAdd MacAddress, SIZE(0..6) */
    hi_u32 dot11_wnm_rqst_time_stamp;                                 /* dot11WNMRqstTimeStamp TimeTicks, */
    hi_u32 dot11_wnm_rqst_rnd_interval;                               /* dot11WNMRqstRndInterval Unsigned32, */
    hi_u32 dot11_wnm_rqst_duration;                                   /* dot11WNMRqstDuration Unsigned32, */
    hi_u8 auc_dot11_wnm_rqst_mcst_group[6];                           /* dot11WNMRqstMcstGroup MacAddress, SIZE(0..6) */
    hi_u8 dot11_wnm_rqst_mcst_trig_con;                               /* dot11WNMRqstMcstTrigCon OCTET STRING,SIZE(1) */
    hi_u32 dot11_wnm_rqst_mcst_trig_inactivity_timeout; /* dot11WNMRqstMcstTrigInactivityTimeout Unsigned32, */
    hi_u32 dot11_wnm_rqst_mcst_trig_react_delay;        /* dot11WNMRqstMcstTrigReactDelay Unsigned32, */
    wlan_mib_wnm_rqst_lcrrqst_subj_enum_uint8 dot11_wnm_rqst_lcr_rqst_subject; /* dot11WNMRqstLCRRqstSubject INTEGER, */
    /* dot11WNMRqstLCRIntervalUnits INTEGER, */
    wlan_mib_wnm_rqst_lcr_interval_unit_enum_uint8 dot11_wnm_rqst_lcr_interval_units;
    hi_u32 dot11_wnm_rqst_lcr_service_interval; /* dot11WNMRqstLCRServiceInterval Unsigned32, */
    wlan_mib_wnm_rqst_lirrqst_subj_enum_uint8 dot11_wnm_rqst_lir_rqst_subject; /* dot11WNMRqstLIRRqstSubject INTEGER, */
    /* dot11WNMRqstLIRIntervalUnits INTEGER, */
    wlan_mib_wnm_rqst_lir_interval_unit_enum_uint8 dot11_wnm_rqst_lir_interval_units;
    hi_u32 dot11_wnm_rqst_lir_service_interval;                        /* dot11WNMRqstLIRServiceInterval Unsigned32, */
    hi_u32 dot11_wnm_rqst_event_token;                                 /* dot11WNMRqstEventToken Unsigned32, */
    wlan_mib_wnm_rqst_event_type_enum_uint8 dot11_wnm_rqst_event_type; /* dot11WNMRqstEventType INTEGER, */
    hi_u32 dot11_wnm_rqst_event_response_limit;                        /* dot11WNMRqstEventResponseLimit Unsigned32, */
    hi_u8 auc_dot11_wnm_rqst_event_target_bssid[6];        /* dot11WNMRqstEventTargetBssid MacAddress, SIZE(0..6) */
    hi_u8 auc_dot11_wnm_rqst_event_source_bssid[6];        /* dot11WNMRqstEventSourceBssid MacAddress, SIZE(0..6) */
    hi_u32 dot11_wnm_rqst_event_transit_time_thresh;       /* dot11WNMRqstEventTransitTimeThresh Unsigned32, */
    hi_u8 dot11_wnm_rqst_event_transit_match_value;        /* dot11WNMRqstEventTransitMatchValue OCTET STRING,SIZE(1) */
    hi_u32 dot11_wnm_rqst_event_freq_transit_count_thresh; /* dot11WNMRqstEventFreqTransitCountThresh Unsigned32, */
    hi_u32 dot11_wnm_rqst_event_freq_transit_interval;     /* dot11WNMRqstEventFreqTransitInterval Unsigned32, */
    hi_u8 auc_dot11_wnm_rqst_event_rsna_auth_type[4];      /* dot11WNMRqstEventRsnaAuthType OCTET STRING,SIZE(4) */
    hi_u32 dot11_wnm_rqst_eap_type;                        /* dot11WNMRqstEapType Unsigned32, */
    hi_u8 auc_dot11_wnm_rqst_eap_vendor_id[3];             /* dot11WNMRqstEapVendorId OCTET STRING,SIZE(0..3) */
    hi_u8 auc_dot11_wnm_rqst_eap_vendor_type[4];           /* dot11WNMRqstEapVendorType OCTET STRING, SIZE(0..4)  */
    hi_u8 dot11_wnm_rqst_event_rsna_match_value;           /* dot11WNMRqstEventRsnaMatchValue OCTET STRING,SIZE(1) */
    hi_u8 auc_dot11_wnm_rqst_event_peer_mac_address[6];    /* dot11WNMRqstEventPeerMacAddress MacAddress, SIZE(0..6) */
    hi_u32 dot11_wnm_rqst_operating_class;                 /* dot11WNMRqstOperatingClass Unsigned32, */
    hi_u32 dot11_wnm_rqst_chan_number;                     /* dot11WNMRqstChanNumber Unsigned32, */
    hi_u32 dot11_wnm_rqst_diag_token;                      /* dot11WNMRqstDiagToken Unsigned32, */
    wlan_mib_wnm_rqst_diag_type_enum_uint8 dot11_wnm_rqst_diag_type; /* dot11WNMRqstDiagType INTEGER, */
    hi_u32 dot11_wnm_rqst_diag_timeout;                              /* dot11WNMRqstDiagTimeout Unsigned32, */
    hi_u8 auc_dot11_wnm_rqst_diag_bssid[6];                          /* dot11WNMRqstDiagBssid MacAddress, SIZE(0..6) */
    hi_u32 dot11_wnm_rqst_diag_profile_id;                           /* dot11WNMRqstDiagProfileId Unsigned32, */
    /* dot11WNMRqstDiagCredentials INTEGER, */
    wlan_mib_wnm_rqst_diag_credent_enum_uint8 dot11_wnm_rqst_diag_credentials;
    /* dot11WNMRqstLocConfigLocIndParams OCTET STRING,SIZE(16) */
    hi_u8 auc_dot11_wnm_rqst_loc_config_loc_ind_params[16]; /* SIZE(0..16) */
    hi_u8 auc_dot11_wnm_rqst_loc_config_chan_list[252]; /* dot11WNMRqstLocConfigChanList OCTET STRING,SIZE(0..252) */
    hi_u32 dot11_wnm_rqst_loc_config_bcast_rate;        /* dot11WNMRqstLocConfigBcastRate Unsigned32, */
    /* dot11WNMRqstLocConfigOptions OCTET STRING,SIZE(0..255) */
    hi_u8 auc_dot11_wnm_rqst_loc_config_options[255]; /* SIZE(0..255) */
    /* dot11WNMRqstBssTransitQueryReason INTEGER, */
    wlan_mib_wnm_rqst_bss_transit_query_reason_enum_uint8 dot11_wnm_rqst_bss_transit_query_reason;
    hi_u8 dot11_wnm_rqst_bss_transit_req_mode;     /* dot11WNMRqstBssTransitReqMode OCTET STRING, SIZE(1) */
    hi_u32 dot11_wnm_rqst_bss_transit_disoc_timer; /* dot11WNMRqstBssTransitDisocTimer Unsigned32, */

    /* This is a control variable.It is written by an external management entity when making a management
       request. Changes take effect when dot11WNMRqstRowStatus is set to Active.
       This attribute contains a variable-length field formatted in accordance with IETF RFC 3986-2005." */
    /* dot11WNMRqstBssTransitSessInfoURL OCTET STRING, */
    hi_u8 auc_dot11_wnm_rqst_bss_transit_sess_info_url[255]; /* SIZE(0..255) */
    /* dot11WNMRqstBssTransitCandidateList OCTET STRING,SIZE(0..2304) */
    hi_u8 auc_dot11_wnm_rqst_bss_transit_candidate_list[2304]; /* SIZE(0..2304) */
    hi_u8 dot11_wnm_rqst_coloc_interf_auto_enable;             /* dot11WNMRqstColocInterfAutoEnable TruthValue, */
    hi_u32 dot11_wnm_rqst_coloc_interf_rpt_timeout;            /* dot11WNMRqstColocInterfRptTimeout Unsigned32, */
    /* dot11WNMRqstVendorSpecific OCTET STRING, SIZE(0..255) */
    hi_u8 auc_dot11_wnm_rqst_vendor_specific[255]; /* SIZE(0..255) */
    /* dot11WNMRqstDestinationURI OCTET STRING SIZE(0..253) */
    hi_u8 auc_dot11_wnm_rqst_destination_uri[253]; /* SIZE(0..253) */

    /* dot11WNMReport OBJECT IDENTIFIER ::= { dot11WirelessNetworkManagement 2 } */
    /* dot11WNMVendorSpecificReportTable::= { dot11WNMReport 1 }                 */
    hi_u32          dot11_wnm_vendor_specific_rprt_index;         /* dot11WNMVendorSpecificRprtIndex Unsigned32, */
    /* dot11WNMVendorSpecificRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_vendor_specific_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_vendor_specific_rprt_if_index;    /* dot11WNMVendorSpecificRprtIfIndex InterfaceIndex, */
    /* dot11WNMVendorSpecificRprtContent OCTET STRING SIZE(0..255) */
    hi_u8           auc_dot11_wnm_vendor_specific_rprt_content[255]; /* SIZE(0..255) */

    /* dot11WNMMulticastDiagnosticReportTable ::= { dot11WNMReport 2 } */
    hi_u32          dot11_wnm_multicast_diagnostic_rprt_index;   /* dot11WNMMulticastDiagnosticRprtIndex Unsigned32, */
    /* dot11WNMMulticastDiagnosticRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_multicast_diagnostic_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    /* dot11WNMMulticastDiagnosticRprtIfIndex InterfaceIndex, */
    hi_u32          dot11_wnm_multicast_diagnostic_rprt_if_index;
    /* dot11WNMMulticastDiagnosticRprtMeasurementTime TSFType, */
    hi_u8           auc_dot11_wnm_multicast_diagnostic_rprt_measurement_time[8]; /* SIZE(0..8) */
    /* dot11WNMMulticastDiagnosticRprtDuration Unsigned32, */
    hi_u32          dot11_wnm_multicast_diagnostic_rprt_duration;
    /* dot11WNMMulticastDiagnosticRprtMcstGroup MacAddress, */
    hi_u8           auc_dot11_wnm_multicast_diagnostic_rprt_mcst_group[6]; /* SIZE(0..6) */
    /* dot11WNMMulticastDiagnosticRprtReason OCTET STRING, */
    hi_u8           dot11_wnm_multicast_diagnostic_rprt_reason;
    /* dot11WNMMulticastDiagnosticRprtRcvdMsduCount Unsigned32, */
    hi_u32          dot11_wnm_multicast_diagnostic_rprt_rcvd_msdu_count;
    /* dot11WNMMulticastDiagnosticRprtFirstSeqNumber Unsigned32, */
    hi_u32          dot11_wnm_multicast_diagnostic_rprt_first_seq_number;
    /* dot11WNMMulticastDiagnosticRprtLastSeqNumber Unsigned32, */
    hi_u32          dot11_wnm_multicast_diagnostic_rprt_last_seq_number;
    /* dot11WNMMulticastDiagnosticRprtMcstRate Unsigned32 */
    hi_u32          dot11_wnm_multicast_diagnostic_rprt_mcst_rate;

    /* dot11WNMLocationCivicReportTable ::= { dot11WNMReport 3 } */
    hi_u32          dot11_wnm_location_civic_rprt_index;       /* dot11WNMLocationCivicRprtIndex Unsigned32, */
    /* dot11WNMLocationCivicRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_location_civic_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_location_civic_rprt_if_index;    /* dot11WNMLocationCivicRprtIfIndex InterfaceIndex, */

    /* This is a status variable.                                             */
    /* It is written by the SME when a management report is completed.        */
    /* This attribute indicates a variable octet field and contains a list of */
    /* civic address elements in TLV format as defined in IETF RFC 4776-2006  */
    /* dot11WNMLocationCivicRprtCivicLocation OCTET STRING */
    hi_u8 auc_dot11_wnm_location_civic_rprt_civic_location[255]; /* SIZE(0..255) */

    /* dot11WNMLocationIdentifierReportTable ::= { dot11WNMReport 4 } */
    /* dot11WNMLocationIdentifierRprtIndex Unsigned32, */
    hi_u32          dot11_wnm_location_identifier_rprt_index;
    /* dot11WNMLocationIdentifierRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_location_identifier_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    /* dot11WNMLocationIdentifierRprtIfIndex InterfaceIndex, */
    hi_u32          dot11_wnm_location_identifier_rprt_if_index;
    /* dot11WNMLocationIdentifierRprtExpirationTSF TSFType, */
    hi_u8           auc_dot11_wnm_location_identifier_rprt_expiration_tsf[8]; /* SIZE(0..8) */
    /* dot11WNMLocationIdentifierRprtPublicIdUri OCTET STRING */
    hi_u8           dot11_wnm_location_identifier_rprt_public_id_uri;

    /* dot11WNMEventTransitReportTable ::= { dot11WNMReport 5 } */
    hi_u32          dot11_wnm_event_transit_rprt_index; /* dot11WNMEventTransitRprtIndex Unsigned32, */
    /* dot11WNMEventTransitRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_event_transit_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_event_transit_rprt_if_index;       /* dot11WNMEventTransitRprtIfIndex InterfaceIndex, */
    /* dot11WNMEventTransitRprtEventStatus INTEGER, */
    wlan_mib_wnm_event_transit_rprt_event_status_enum_uint8 dot11_wnm_event_transit_rprt_event_status;
    /* dot11WNMEventTransitRprtEventTSF TSFType, */
    hi_u8           auc_dot11_wnm_event_transit_rprt_event_tsf[8];   /* SIZE(0..8) */
    /* dot11WNMEventTransitRprtUTCOffset OCTET STRING, SIZE(10) */
    hi_u8           auc_dot11_wnm_event_transit_rprt_utc_offset[10]; /* SIZE(0..10) */
    /* dot11WNMEventTransitRprtTimeError OCTET STRING, SIZE(5) */
    hi_u8           auc_dot11_wnm_event_transit_rprt_time_error[5];  /* SIZE(0..5) */
    /* dot11WNMEventTransitRprtSourceBssid MacAddress, */
    hi_u8           auc_dot11_wnm_event_transit_rprt_source_bssid[6]; /* SIZE(0..6) */
    /* dot11WNMEventTransitRprtTargetBssid MacAddress, */
    hi_u8           auc_dot11_wnm_event_transit_rprt_target_bssid[6]; /* SIZE(0..6) */
    hi_u32          dot11_wnm_event_transit_rprt_transit_time;   /* dot11WNMEventTransitRprtTransitTime Unsigned32, */
    /* dot11WNMEventTransitRprtTransitReason INTEGER, */
    wlan_mib_wnm_event_transitrprt_transit_reason_enum_uint8  dot11_wnm_event_transit_rprt_transit_reason;
    hi_u32          dot11_wnm_event_transit_rprt_transit_result; /* dot11WNMEventTransitRprtTransitResult Unsigned32, */
    hi_u32          dot11_wnm_event_transit_rprt_source_rcpi;    /* dot11WNMEventTransitRprtSourceRCPI Unsigned32, */
    hi_u32          dot11_wnm_event_transit_rprt_source_rsni;    /* dot11WNMEventTransitRprtSourceRSNI Unsigned32, */
    hi_u32          dot11_wnm_event_transit_rprt_target_rcpi;    /* dot11WNMEventTransitRprtTargetRCPI Unsigned32, */
    hi_u32          dot11_wnm_event_transit_rprt_target_rsni;    /* dot11WNMEventTransitRprtTargetRSNI Unsigned32  */

    /* dot11WNMEventRsnaReportTable ::= { dot11WNMReport 6 } */
    hi_u32          dot11_wnm_event_rsna_rprt_index;               /* dot11WNMEventRsnaRprtIndex Unsigned32, */
    /* dot11WNMEventRsnaRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_event_rsna_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_event_rsna_rprt_if_index;            /* dot11WNMEventRsnaRprtIfIndex InterfaceIndex, */
    /* dot11WNMEventRsnaRprtEventStatus INTEGER, */
    wlan_mib_wnm_event_rsnarprt_event_status_enum_uint8 dot11_wnm_event_rsna_rprt_event_status;
    hi_u8           auc_dot11_wnm_event_rsna_rprt_event_tsf[8]; /* dot11WNMEventRsnaRprtEventTSF TSFType, SIZE(0..8) */
    /* dot11WNMEventRsnaRprtUTCOffset OCTET STRING,SIZE(10) */
    hi_u8           auc_dot11_wnm_event_rsna_rprt_utc_offset[10]; /* SIZE(0..10) */
    /* dot11WNMEventRsnaRprtTimeError OCTET STRING,SIZE(5) */
    hi_u8           auc_dot11_wnm_event_rsna_rprt_time_error[5];  /* SIZE(0..5) */
    /* dot11WNMEventRsnaRprtTargetBssid MacAddress, */
    hi_u8           auc_dot11_wnm_event_rsna_rprt_target_bssid[6];  /* SIZE(0..6) */
    hi_u8           auc_dot11_wnm_event_rsna_rprt_auth_type[4]; /* dot11WNMEventRsnaRprtAuthType OCTET STRING,SIZE(4) */
    /* dot11WNMEventRsnaRprtEapMethod OCTET STRING,SIZE(1..8) */
    hi_u8           auc_dot11_wnm_event_rsna_rprt_eap_method[8]; /* SIZE(0..8) */
    hi_u32          dot11_wnm_event_rsna_rprt_result;          /* dot11WNMEventRsnaRprtResult Unsigned32, */
    hi_u8           dot11_wnm_event_rsna_rprt_rsn_element;     /* dot11WNMEventRsnaRprtRsnElement OCTET STRING */

    /* dot11WNMEventPeerReportTable ::= { dot11WNMReport 7 } */
    hi_u32          dot11_wnm_event_peer_rprt_index;                /* dot11WNMEventPeerRprtIndex Unsigned32, */
    /* dot11WNMEventPeerRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_event_peer_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_event_peer_rprt_if_index;             /* dot11WNMEventPeerRprtIfIndex InterfaceIndex, */
    hi_u8           dot11_wnm_event_peer_rprt_event_status;         /* dot11WNMEventPeerRprtEventStatus INTEGER, */
    hi_u8           auc_dot11_wnm_event_peer_rprt_event_tsf[8]; /* dot11WNMEventPeerRprtEventTSF TSFType, SIZE(0..8) */
    /* dot11WNMEventPeerRprtUTCOffset OCTET STRING,SIZE(10) */
    hi_u8           auc_dot11_wnm_event_peer_rprt_utc_offset[10]; /* SIZE(0..10) */
    /* dot11WNMEventPeerRprtTimeError OCTET STRING,SIZE(5) */
    hi_u8           auc_dot11_wnm_event_peer_rprt_time_error[5];  /* SIZE(0..5) */
    /* dot11WNMEventPeerRprtPeerMacAddress MacAddress, */
    hi_u8           auc_dot11_wnm_event_peer_rprt_peer_mac_address[6]; /* SIZE(0..6) */
    hi_u32          dot11_wnm_event_peer_rprt_operating_class;    /* dot11WNMEventPeerRprtOperatingClass Unsigned32, */
    hi_u32          dot11_wnm_event_peer_rprt_chan_number;        /* dot11WNMEventPeerRprtChanNumber Unsigned32, */
    hi_s32           l_dot11_wnm_event_peer_rprt_sta_tx_power;    /* dot11WNMEventPeerRprtStaTxPower Integer32, */
    hi_u32          dot11_wnm_event_peer_rprt_conn_time;          /* dot11WNMEventPeerRprtConnTime Unsigned32, */
    hi_u8           dot11_wnm_event_peer_rprt_peer_status;        /* dot11WNMEventPeerRprtPeerStatus INTEGER */

    /* dot11WNMEventWNMLogReportTable ::= { dot11WNMReport 8 } */
    hi_u32          dot11_wnm_event_wnm_log_rprt_index;             /* dot11WNMEventWNMLogRprtIndex Unsigned32, */
    /* dot11WNMEventWNMLogRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_event_wnm_log_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_event_wnm_log_rprt_if_index;          /* dot11WNMEventWNMLogRprtIfIndex InterfaceIndex, */
    hi_u8           dot11_wnm_event_wnm_log_rprt_event_status;      /* dot11WNMEventWNMLogRprtEventStatus INTEGER, */
    /* dot11WNMEventWNMLogRprtEventTSF TSFType, */
    hi_u8           auc_dot11_wnm_event_wnm_log_rprt_event_tsf[8];  /* SIZE(0..8) */
    /* dot11WNMEventWNMLogRprtUTCOffset OCTET STRING,SIZE(10) */
    hi_u8           auc_dot11_wnm_event_wnm_log_rprt_utc_offset[10]; /* SIZE(0..10) */
    /* dot11WNMEventWNMLogRprtTimeError OCTET STRING,SIZE(5) */
    hi_u8           auc_dot11_wnm_event_wnm_log_rprt_time_error[5];  /* SIZE(0..5) */
    /* dot11WNMEventWNMLogRprtContent OCTET STRING SIZE(0..2284) */
    hi_u8           auc_dot11_wnm_event_wnm_log_rprt_content[2284];  /* SIZE(0..2284) */

    /* dot11WNMDiagMfrInfoReportTable ::= { dot11WNMReport 9 } */
    hi_u32          dot11_wnm_diag_mfr_info_rprt_index;   /* dot11WNMDiagMfrInfoRprtIndex Unsigned32, */
    /* dot11WNMDiagMfrInfoRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_diag_mfr_info_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_diag_mfr_info_rprt_if_index;        /* dot11WNMDiagMfrInfoRprtIfIndex InterfaceIndex, */
    hi_u8           dot11_wnm_diag_mfr_info_rprt_event_status;    /* dot11WNMDiagMfrInfoRprtEventStatus INTEGER, */
    /* dot11WNMDiagMfrInfoRprtMfrOi OCTET STRING, SIZE(0..5) */
    hi_u8           auc_dot11_wnm_diag_mfr_info_rprt_mfr_oi[5]; /* SIZE(0..5) */
    /* dot11WNMDiagMfrInfoRprtMfrIdString OCTET STRING, (0..255) */
    hi_u8           auc_dot11_wnm_diag_mfr_info_rprt_mfr_id_string[255]; /* SIZE(0..255) */
    /* dot11WNMDiagMfrInfoRprtMfrModelString OCTET STRING, SIZE(0..255) */
    hi_u8           auc_dot11_wnm_diag_mfr_info_rprt_mfr_model_string[255]; /* SIZE(0..255) */
    /* dot11WNMDiagMfrInfoRprtMfrSerialNumberString OCTET STRING, SIZE(0..255) */
    hi_u8           auc_dot11_wnm_diag_mfr_info_rprt_mfr_serial_number_string[255]; /* SIZE(0..255) */
    /* dot11WNMDiagMfrInfoRprtMfrFirmwareVersion OCTET STRING, SIZE(0..255) */
    hi_u8           auc_dot11_wnm_diag_mfr_info_rprt_mfr_firmware_version[255]; /* SIZE(0..255) */
    /* dot11WNMDiagMfrInfoRprtMfrAntennaType OCTET STRING, SIZE(0..255) */
    hi_u8           auc_dot11_wnm_diag_mfr_info_rprt_mfr_antenna_type[255]; /* SIZE(0..255) */
    /* dot11WNMDiagMfrInfoRprtCollocRadioType INTEGER, */
    hi_u8           dot11_wnm_diag_mfr_info_rprt_colloc_radio_type;
    hi_u8           dot11_wnm_diag_mfr_info_rprt_device_type;       /* dot11WNMDiagMfrInfoRprtDeviceType INTEGER, */
    /* dot11WNMDiagMfrInfoRprtCertificateID OCTET STRING SIZE(0..251) */
    hi_u8           auc_dot11_wnm_diag_mfr_info_rprt_certificate_id[251]; /* SIZE(0..251) */

    /* dot11WNMDiagConfigProfReportTable ::= { dot11WNMReport 10 } */
    hi_u32          dot11_wnm_diag_config_prof_rprt_index;          /* dot11WNMDiagConfigProfRprtIndex Unsigned32, */
    /* dot11WNMDiagConfigProfRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_diag_config_prof_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_diag_config_prof_rprt_if_index;   /* dot11WNMDiagConfigProfRprtIfIndex InterfaceIndex, */
    hi_u8           dot11_wnm_diag_config_prof_rprt_event_status;  /* dot11WNMDiagConfigProfRprtEventStatus INTEGER, */
    hi_u32          dot11_wnm_diag_config_prof_rprt_profile_id;    /* dot11WNMDiagConfigProfRprtProfileId Unsigned32, */
    /* dot11WNMDiagConfigProfRprtSupportedOperatingClasses OCTET STRING,SIZE(0..255) */
    hi_u8           auc_dot11_wnm_diag_config_prof_rprt_supported_operating_classes[255]; /* SIZE(0..255) */
    hi_u8           dot11_wnm_diag_config_prof_rprt_tx_power_mode;  /* dot11WNMDiagConfigProfRprtTxPowerMode INTEGER, */
    /* dot11WNMDiagConfigProfRprtTxPowerLevels OCTET STRING,SIZE(1..255) */
    hi_u8           auc_dot11_wnm_diag_config_prof_rprt_tx_power_levels[255]; /* SIZE(0..255) */
    /* dot11WNMDiagConfigProfRprtCipherSuite OCTET STRING, SIZE(4) */
    hi_u8           auc_dot11_wnm_diag_config_prof_rprt_cipher_suite[4]; /* SIZE(0..4) */
    /* dot11WNMDiagConfigProfRprtAkmSuite OCTET STRING, SIZE(4) */
    hi_u8           auc_dot11_wnm_diag_config_prof_rprt_akm_suite[4]; /* SIZE(0..4) */
    /* dot11WNMDiagConfigProfRprtEapType Unsigned32, */
    hi_u32          dot11_wnm_diag_config_prof_rprt_eap_type;
    /* dot11WNMDiagConfigProfRprtEapVendorID OCTET STRING,    SIZE(0..3) */
    hi_u8           auc_dot11_wnm_diag_config_prof_rprt_eap_vendor_id[3]; /* SIZE(0..3) */
    /* dot11WNMDiagConfigProfRprtEapVendorType OCTET STRING,  SIZE(0..4) */
    hi_u8           auc_dot11_wnm_diag_config_prof_rprt_eap_vendor_type[4]; /* SIZE(0..4) */
    /* dot11WNMDiagConfigProfRprtCredentialType INTEGER, */
    hi_u8           dot11_wnm_diag_config_prof_rprt_credential_type;
    /* dot11WNMDiagConfigProfRprtSSID OCTET STRING, SIZE(1..32) */
    hi_u8           auc_dot11_wnm_diag_config_prof_rprt_ssid[32]; /* SIZE(0..32) */
    /* dot11WNMDiagConfigProfRprtPowerSaveMode INTEGER */
    hi_u8           dot11_wnm_diag_config_prof_rprt_power_save_mode;

    /* dot11WNMDiagAssocReportTable ::= { dot11WNMReport 11 } */
    hi_u32          dot11_wnm_diag_assoc_rprt_index;            /* dot11WNMDiagAssocRprtIndex Unsigned32, */
    /* dot11WNMDiagAssocRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_diag_assoc_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_diag_assoc_rprt_if_index;         /* dot11WNMDiagAssocRprtIfIndex InterfaceIndex, */
    hi_u8           dot11_wnm_diag_assoc_rprt_event_status;     /* dot11WNMDiagAssocRprtEventStatus INTEGER, */
    hi_u8           auc_dot11_wnm_diag_assoc_rprt_bssid[6];     /* dot11WNMDiagAssocRprtBssid MacAddress, SIZE(0..6) */
    hi_u32          dot11_wnm_diag_assoc_rprt_operating_class;  /* dot11WNMDiagAssocRprtOperatingClass Unsigned32, */
    hi_u32          dot11_wnm_diag_assoc_rprt_channel_number;   /* dot11WNMDiagAssocRprtChannelNumber Unsigned32, */
    hi_u32          dot11_wnm_diag_assoc_rprt_status_code;      /* dot11WNMDiagAssocRprtStatusCode Unsigned32 */

    /* dot11WNMDiag8021xAuthReportTable ::= { dot11WNMReport 12 } */
    hi_u32          dot11_wnm_diag8021x_auth_rprt_index;        /* dot11WNMDiag8021xAuthRprtIndex Unsigned32, */
    /* dot11WNMDiag8021xAuthRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_diag8021x_auth_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_diag8021x_auth_rprt_if_index;     /* dot11WNMDiag8021xAuthRprtIfIndex InterfaceIndex, */
    hi_u8           dot11_wnm_diag8021x_auth_rprt_event_status; /* dot11WNMDiag8021xAuthRprtEventStatus INTEGER, */
    hi_u8           auc_dot11_wnm_diag8021x_auth_rprt_bssid[6]; /* dot11WNMDiag8021xAuthRprtBssid MacAddress, SIZE(6) */
    /* dot11WNMDiag8021xAuthRprtOperatingClass Unsigned32, */
    hi_u32          dot11_wnm_diag8021x_auth_rprt_operating_class;
    /* dot11WNMDiag8021xAuthRprtChannelNumber Unsigned32, */
    hi_u32          dot11_wnm_diag8021x_auth_rprt_channel_number;
    hi_u32          dot11_wnm_diag8021x_auth_rprt_eap_type;      /* dot11WNMDiag8021xAuthRprtEapType Unsigned32, */
    /* dot11WNMDiag8021xAuthRprtEapVendorID OCTET STRING, (SIZE(0..3) */
    hi_u8           auc_dot11_wnm_diag8021x_auth_rprt_eap_vendor_id[3]; /* SIZE(0..3) */
    /* dot11WNMDiag8021xAuthRprtEapVendorType OCTET STRING, SIZE(0..4) */
    hi_u8           auc_dot11_wnm_diag8021x_auth_rprt_eap_vendor_type[4]; /* SIZE(0..4) */
    /* dot11WNMDiag8021xAuthRprtCredentialType INTEGER, */
    hi_u8           dot11_wnm_diag8021x_auth_rprt_credential_type;
    hi_u32          dot11_wnm_diag8021x_auth_rprt_status_code;    /* dot11WNMDiag8021xAuthRprtStatusCode Unsigned32 */

    /* dot11WNMLocConfigReportTable ::= { dot11WNMReport 13 } */
    hi_u32          dot11_wnm_loc_config_rprt_index;            /* dot11WNMLocConfigRprtIndex Unsigned32, */
    /* dot11WNMLocConfigRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_loc_config_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_loc_config_rprt_if_index;         /* dot11WNMLocConfigRprtIfIndex InterfaceIndex, */
    /* dot11WNMLocConfigRprtLocIndParams OCTET STRING,SIZE(16) */
    hi_u8           auc_dot11_wnm_loc_config_rprt_loc_ind_params[16]; /* SIZE(0..16) */
    /* dot11WNMLocConfigRprtLocIndChanList OCTET STRING, SIZE(0..254) */
    hi_u8           auc_dot11_wnm_loc_config_rprt_loc_ind_chan_list[254]; /* SIZE(0..254) */
    hi_u32          dot11_wnm_loc_config_rprt_loc_ind_bcast_rate; /* dot11WNMLocConfigRprtLocIndBcastRate Unsigned32, */
    /* dot11WNMLocConfigRprtLocIndOptions OCTET STRING,SIZE(0..255) */
    hi_u8           auc_dot11_wnm_loc_config_rprt_loc_ind_options[255]; /* SIZE(0..255) */
    /* dot11WNMLocConfigRprtStatusConfigSubelemId INTEGER, */
    hi_u8           dot11_wnm_loc_config_rprt_status_config_subelem_id;
    hi_u8           dot11_wnm_loc_config_rprt_status_result;      /* dot11WNMLocConfigRprtStatusResult INTEGER, */
    /* dot11WNMLocConfigRprtVendorSpecificRprtContent OCTET STRING SIZE(0..255) */
    hi_u8           auc_dot11_wnm_loc_config_rprt_vendor_specific_rprt_content[255]; /* SIZE(0..255) */

    /* dot11WNMBssTransitReportTable ::= { dot11WNMReport 14 } */
    hi_u32          dot11_wnm_bss_transit_rprt_index;                /* dot11WNMBssTransitRprtIndex Unsigned32, */
    /* dot11WNMBssTransitRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_bss_transit_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_bss_transit_rprt_if_index;             /* dot11WNMBssTransitRprtIfIndex InterfaceIndex, */
    hi_u8           dot11_wnm_bss_transit_rprt_status_code;          /* dot11WNMBssTransitRprtStatusCode INTEGER, */
    /* dot11WNMBssTransitRprtBSSTerminationDelay Unsigned32, */
    hi_u32          dot11_wnm_bss_transit_rprt_bss_termination_delay;
    /* dot11WNMBssTransitRprtTargetBssid MacAddress, */
    hi_u8           auc_dot11_wnm_bss_transit_rprt_target_bssid[6];  /* SIZE(0..6) */
    /* dot11WNMBssTransitRprtCandidateList OCTET STRING SIZE(0..2304) */
    hi_u8           auc_dot11_wnm_bss_transit_rprt_candidate_list[2304]; /* SIZE(0..2304) */
    hi_u32          dot11_wnm_coloc_interf_rprt_index;           /* dot11WNMColocInterfRprtIndex Unsigned32, */
    /* dot11WNMColocInterfRprtRqstToken OCTET STRING, */
    hi_u8           auc_dot11_wnm_coloc_interf_rprt_rqst_token[WLAN_MIB_TOKEN_STRING_MAX_LENGTH];
    hi_u32          dot11_wnm_coloc_interf_rprt_if_index;        /* dot11WNMColocInterfRprtIfIndex InterfaceIndex, */
    hi_u32          dot11_wnm_coloc_interf_rprt_period;          /* dot11WNMColocInterfRprtPeriod Unsigned32, */
    hi_s32           l_dot11_wnm_coloc_interf_rprt_interf_level; /* dot11WNMColocInterfRprtInterfLevel Integer32, */
    hi_u32          dot11_wnm_coloc_interf_rprt_interf_accuracy; /* dot11WNMColocInterfRprtInterfAccuracy Unsigned32, */
    hi_u32          dot11_wnm_coloc_interf_rprt_interf_index;    /* dot11WNMColocInterfRprtInterfIndex Unsigned32, */
    /* dot11WNMColocInterfRprtInterfInterval Integer32, */
    hi_s32           l_dot11_wnm_coloc_interf_rprt_interf_interval;
    /* dot11WNMColocInterfRprtInterfBurstLength Integer32, */
    hi_s32           l_dot11_wnm_coloc_interf_rprt_interf_burst_length;
    /* dot11WNMColocInterfRprtInterfStartTime Integer32, */
    hi_s32           l_dot11_wnm_coloc_interf_rprt_interf_start_time;
    /* dot11WNMColocInterfRprtInterfCenterFreq Integer32, */
    hi_s32           l_dot11_wnm_coloc_interf_rprt_interf_center_freq;
    /* dot11WNMColocInterfRprtInterfBandwidth Unsigned32 */
    hi_u32          dot11_wnm_coloc_interf_rprt_interf_bandwidth;
} wlan_mib_dot11_wireless_network_management_stru;

/* ************************************************************************** */
/* dot11MeshSTAConfig TABLE  - members of Dot11MeshSTAConfigEntry           */
/* ************************************************************************** */
/* Mesh Station Configuration attributes. In tabular form to allow for mul- */
/* tiple instances on an agent.                                             */
typedef struct {
    hi_u8 auc_dot11_mesh_id[32 + 1];                 /* dot11MeshID OCTET STRING,   (SIZE(0..32)) */
    hi_u8 dot11_mesh_accepting_additional_peerings;  /* dot11MeshAcceptingAdditionalPeerings TruthValue */
    hi_u8 dot11_mesh_security_activated;             /* dot11MeshSecurityActivated TruthValue,          */
    hi_u8 dot11_mesh_active_authentication_protocol; /* dot11MeshActiveAuthenticationProtocol INTEGER,  */
    hi_u8 dot11_mbca_activated;                      /* dot11MBCAActivated TruthValue,                  */
    hi_u8 resv1[3];                                  /* 3:resv */
    hi_u32 dot11_mesh_beacon_timing_report_interval; /* dot11MeshBeaconTimingReportInterval Unsigned32, */
    hi_u32 dot11_mesh_beacon_timing_report_max_num;  /* dot11MeshBeaconTimingReportMaxNum Unsigned32,   */
} wlan_mib_dot11_mesh_sta_config_entry_stru;

/* *************************************************************************** */
/* dot11MeshHWMPConfig TABLE  - members of Dot11MeshHWMPConfigEntry          */
/* *************************************************************************** */
/* MMesh Station HWMP Configuration attributes. In tabular form to allow for */
/* tmultiple instances on an agent.                                          */
typedef struct {
    hi_u32 dot11_mesh_hwm_pmax_pre_qretries;            /* dot11MeshHWMPmaxPREQretries Unsigned32,           */
    hi_u32 dot11_mesh_hwm_pnet_diameter;                /* dot11MeshHWMPnetDiameter Unsigned32,              */
    hi_u32 dot11_mesh_hwm_pnet_diameter_traversal_time; /* dot11MeshHWMPnetDiameterTraversalTime Unsigned32  */
    hi_u32 dot11_mesh_hwm_ppreq_min_interval;           /* dot11MeshHWMPpreqMinInterval Unsigned32,          */
    hi_u32 dot11_mesh_hwm_pperr_min_interval;           /* dot11MeshHWMPperrMinInterval Unsigned32,          */
    hi_u32 dot11_mesh_hwm_pactive_path_to_root_timeout; /* dot11MeshHWMPactivePathToRootTimeout Unsigned32,  */
    hi_u32 dot11_mesh_hwm_pactive_path_timeout;         /* dot11MeshHWMPactivePathTimeout Unsigned32,        */
    hi_u8  dot11_mesh_hwm_proot_mode;                   /* dot11MeshHWMProotMode INTEGER,                    */
    hi_u8  dot11_mesh_hwm_ptarget_only;                 /* dot11MeshHWMPtargetOnly INTEGER,                  */
    hi_u8  resv1[2];                                    /* 2:resv */
    hi_u32 dot11_mesh_hwm_proot_interval;               /* dot11MeshHWMProotInterval Unsigned32,             */
    hi_u32 dot11_mesh_hwm_prann_interval;               /* dot11MeshHWMPrannInterval Unsigned32,             */
    hi_u32 dot11_mesh_hwm_pmaintenance_interval;        /* dot11MeshHWMPmaintenanceInterval Unsigned32,      */
    hi_u32 dot11_mesh_hwm_pconfirmation_interval;       /* dot11MeshHWMPconfirmationInterval Unsigned32      */
} wlan_mib_dot11_mesh_hwmp_config_entry_stru;

/* ************************************************************************************ */
/* dot11RSNAConfigPasswordValue TABLE  - members of Dot11RSNAConfigPasswordValueEntry */
/* ************************************************************************************ */
/* When SAE authentication is the selected AKM suite,     */
/* this table is used to locate the binary representation */
/* of a shared, secret, and potentially low-entropy word, */
/* phrase, code, or key that will be used as the          */
/* authentication credential between a TA/RA pair.        */
/* This table is logically write-only. Reading this table */
/* returns unsuccessful status or null or zero."          */
typedef struct {
    hi_u32 dot11_rsna_config_password_value_index;    /* dot11RSNAConfigPasswordValueIndex Unsigned32, */
    hi_u8 dot11_rsna_config_password_credential;      /* dot11RSNAConfigPasswordCredential OCTET STRING, */
    hi_u8 auc_dot11_rsna_config_password_peer_mac[6]; /* dot11RSNAConfigPasswordPeerMac MacAddress SIZE(0..6) */
    hi_u8 resv;
} wlan_mib_dot11_rsna_config_password_value_entry_stru;

/* ************************************************************************** */
/* dot11RSNAConfigDLCGroup TABLE  - members of Dot11RSNAConfigDLCGroupEntry */
/* ************************************************************************** */
/* This table gives a prioritized list of domain parameter set   */
/* Identifiers for discrete logarithm cryptography (DLC) groups. */
typedef struct {
    hi_u32 dot11_rsna_config_dlc_group_index;      /* dot11RSNAConfigDLCGroupIndex Unsigned32, */
    hi_u32 dot11_rsna_config_dlc_group_identifier; /* dot11RSNAConfigDLCGroupIdentifier Unsigned32  */
} wlan_mib_dot11_rsna_config_dlc_group_entry_stru;

/* ************************************************************************** */
/* dot11VHTStationConfig TABLE  - members of Dot11VHTStationConfigEntry */
/* ************************************************************************** */
/* Station Configuration attributes. In tabular form to allow for multiple   */
/* instances on an agent. */
typedef struct {
    hi_u32 dot11_max_mpdu_length;                      /* dot11MaxMPDULength INTEGER, */
    hi_u32 dot11_vht_max_rx_ampdu_factor;              /* dot11VHTMaxRxAMPDUFactor Unsigned32, */
    hi_u8 dot11_vht_control_field_supported;           /* dot11VHTControlFieldSupported TruthValue, */
    hi_u8 dot11_vhttxop_power_save_option_implemented; /* dot11VHTTXOPPowerSaveOptionImplemented TruthValue, */
    hi_u16 us_dot11_vht_rx_mcs_map;                    /* dot11VHTRxMCSMap OCTET STRING, */
    hi_u32 dot11_vht_rx_highest_data_rate_supported;   /* dot11VHTRxHighestDataRateSupported Unsigned32, */
    hi_u16 us_dot11_vht_tx_mcs_map;                    /* dot11VHTTxMCSMap OCTET STRING, */
    hi_u16 resv1;
    hi_u32 dot11_vht_tx_highest_data_rate_supported; /* dot11VHTTxHighestDataRateSupported Unsigned32, */
} wlan_mib_dot11_vht_station_config_entry_stru;

/* ************************************************************************************** */
/*          Start of dot11mac OBJECT IDENTIFIER ::= { ieee802dot11 2 }                  */
/*              --  MAC GROUPS                                                          */
/*              --  dot11OperationTable ::= { dot11mac 1 }                              */
/*              --  dot11CountersTable ::= { dot11mac 2 }                               */
/*              --  dot11GroupAddressesTable ::= { dot11mac 3 }                         */
/*              --  dot11EDCATable ::= { dot11mac 4 }                                   */
/*              --  dot11QAPEDCATable ::= { dot11mac 5 }                                */
/*              --  dot11QosCountersTable ::= { dot11mac 6 }                            */
/*              --  dot11ResourceInfoTable    ::= { dot11mac 7 }                        */
/* ************************************************************************************** */
/* ************************************************************************************** */
/* dot11OperationTable OBJECT-TYPE                                                      */
/* SYNTAX SEQUENCE OF Dot11OperationEntry                                               */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Group contains MAC attributes pertaining to the operation of the MAC.          */
/*      This has been implemented as a table in order to allow for multiple             */
/*      instantiations on an agent."                                                    */
/* ::= { dot11mac 1 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_rts_threshold;                             /* dot11RTSThreshold Unsigned32 */
    hi_u32 dot11_fragmentation_threshold;                   /* dot11FragmentationThreshold Unsigned32 */
    wlan_mib_ht_protection_enum_uint8 dot11_ht_protection;  /* dot11HTProtection INTEGER */
    hi_u8 dot11_rifs_mode;                                  /* dot11RIFSMode TruthValue */
    hi_u8 dot11_lsigtxop_full_protection_activated;         /* dot11LSIGTXOPFullProtectionActivated TruthValue */
    hi_u8 dot11_non_gf_entities_present;                    /* dot11NonGFEntitiesPresent TruthValue */
    hi_u8 dot11_forty_m_hz_intolerant;                      /* dot11FortyMHzIntolerant TruthValue */
    hi_u8 dot112040_bss_coexistence_management_support;     /* dot112040BSSCoexistenceManagementSupport TruthValue */
    hi_u8 resv1[2];                                         /* 2:resv */
    hi_u32 dot11_bss_width_trigger_scan_interval;           /* dot11BSSWidthTriggerScanInterval Unsigned32 */
    hi_u32 dot11_bss_width_channel_transition_delay_factor; /* dot11BSSWidthChannelTransitionDelayFactor Unsigned32 */
    hi_u32 dot11_obss_scan_passive_dwell;                   /* dot11OBSSScanPassiveDwell Unsigned32 */
    hi_u32 dot11_obss_scan_active_dwell;                    /* dot11OBSSScanActiveDwell Unsigned32 */
    hi_u32 dot11_obss_scan_passive_total_per_channel;       /* dot11OBSSScanPassiveTotalPerChannel Unsigned32 */
    hi_u32 dot11_obss_scan_active_total_per_channel;        /* dot11OBSSScanActiveTotalPerChannel Unsigned32 */
    hi_u32 dot11_obss_scan_activity_threshold;              /* dot11OBSSScanActivityThreshold Unsigned32 */
} wlan_mib_dot11_operation_entry_stru;

/* ************************************************************************************** */
/* dot11CountersTable OBJECT-TYPE                                                       */
/* SYNTAX SEQUENCE OF Dot11CountersEntry                                                */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Group containing attributes that are MAC counters. Implemented as a table      */
/*      to allow for multiple instantiations on an agent."                              */
/* ::= { dot11mac 2 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_group_received_frame_count;              /* dot11GroupReceivedFrameCount Counter32 */
    hi_u32 dot11_received_amsdu_count;                    /* dot11ReceivedAMSDUCount Counter32 */
    hi_u64 ull_dot11dot11_received_octets_in_amsdu_count; /* dot11ReceivedOctetsInAMSDUCount Counter64 */
    hi_u32 dot11_mpdu_in_received_ampdu_count;            /* dot11MPDUInReceivedAMPDUCount Counter32 */
    hi_u32 resv;
} wlan_mib_dot11_counters_entry_stru;

/* ************************************************************************************** */
/* dot11GroupAddressesTable OBJECT-TYPE                                                 */
/* SYNTAX SEQUENCE OF Dot11GroupAddressesEntry                                          */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "A conceptual table containing a set of MAC addresses identifying the mul-      */
/*      ticast-group addresses for which this STA receives frames. The default          */
/*      value of this attribute is null."                                               */
/* ::= { dot11mac 3 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_group_addresses_index;                          /* dot11GroupAddressesIndex InterfaceIndex */
    hi_u8 auc_dot11_address[6];                                  /* dot11Address MacAddress SIZE(0..6) */
    wlan_mib_row_status_enum_uint8 dot11_group_addresses_status; /* dot11GroupAddressesStatus RowStatus */
    hi_u8 resv;
} wlan_mib_dot11_group_addresses_entry_stru;

/* ************************************************************************************** */
/* dot11EDCATable OBJECT-TYPE                                                           */
/* SYNTAX SEQUENCE OF Dot11EDCAEntry                                                    */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Conceptual table for EDCA default parameter values at a non-AP STA. This       */
/*      table contains the four entries of the EDCA parameters corresponding to         */
/*      four possible ACs. Index 1 corresponds to AC_BK, index 2 to AC_BE, index 3      */
/*      to AC_VI, and index 4 to AC_VO."                                                */
/*      REFERENCE                                                                       */
/*      "IEEE 802.11-<year>, 9.2.4.2"                                                   */
/* ::= { dot11mac 4 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_edca_table_c_wmin;        /* dot11EDCATableCWmin Unsigned32 */
    hi_u32 dot11_edca_table_c_wmax;        /* dot11EDCATableCWmax Unsigned32 */
    hi_u32 dot11_edca_table_aifsn;         /* dot11EDCATableAIFSN Unsigned32 */
    hi_u32 dot11_edca_table_txop_limit;    /* dot11EDCATableTXOPLimit Unsigned32 */
    hi_u32 dot11_edca_table_msdu_lifetime; /* dot11EDCATableMSDULifetime Unsigned32 */
    hi_u8 dot11_edca_table_mandatory;      /* dot11EDCATableMandatory TruthValue */
    hi_u8 resv[3];                         /* 3:resv */
} wlan_mib_dot11_edca_entry_stru;

/* ************************************************************************************** */
/* dot11QAPEDCATable OBJECT-TYPE                                                        */
/* SYNTAX SEQUENCE OF Dot11QAPEDCAEntry                                                 */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Conceptual table for EDCA default parameter values at the AP. This table       */
/*      contains the four entries of the EDCA parameters corresponding to four          */
/*      possible ACs. Index 1 corresponds to AC_BK, index 2 to AC_BE, index 3 to        */
/*      AC_VI, and index 4 to AC_VO."                                                   */
/*      REFERENCE                                                                       */
/*      "IEEE 802.11-<year>, 9.19.2"                                                    */
/* ::= { dot11mac 5 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_qapedca_table_c_wmin;        /* dot11QAPEDCATableCWmin Unsigned32 */
    hi_u32 dot11_qapedca_table_c_wmax;        /* dot11QAPEDCATableCWmax Unsigned32 */
    hi_u32 dot11_qapedca_table_aifsn;         /* dot11QAPEDCATableAIFSN Unsigned32 */
    hi_u32 dot11_qapedca_table_txop_limit;    /* dot11QAPEDCATableTXOPLimit Unsigned32 */
    hi_u32 dot11_qapedca_table_msdu_lifetime; /* dot11QAPEDCATableMSDULifetime Unsigned32 */
    hi_u8 dot11_qapedca_table_mandatory;      /* dot11QAPEDCATableMandatory TruthValue */
    hi_u8 resv[3];                            /* 3:resv */
} wlan_mib_dot11_qapedca_entry_stru;

/* ************************************************************************************** */
/* dot11QosCountersTable OBJECT-TYPE                                                    */
/* SYNTAX SEQUENCE OF Dot11QosCountersEntry                                             */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Group containing attributes that are MAC counters implemented as a table       */
/*      to allow for multiple instantiations on an agent."                              */
/* ::= { dot11mac 6 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_qos_counters_index;             /* dot11QosCountersIndex Unsigned32 */
    hi_u32 dot11_qos_transmitted_fragment_count; /* dot11QosTransmittedFragmentCount Counter32 */
    hi_u32 dot11_qos_failed_count;               /* dot11QosFailedCount Counter32 */
    hi_u32 dot11_qos_retry_count;                /* dot11QosRetryCount Counter32 */
    hi_u32 dot11_qos_multiple_retry_count;       /* dot11QosMultipleRetryCount Counter32 */
    hi_u32 dot11_qos_frame_duplicate_count;      /* dot11QosFrameDuplicateCount Counter32 */
    hi_u32 dot11_qos_rts_success_count;          /* dot11QosRTSSuccessCount Counter32 */
    hi_u32 dot11_qos_rts_failure_count;          /* dot11QosRTSFailureCount Counter32 */
    hi_u32 dot11_qos_ack_failure_count;          /* dot11QosACKFailureCount Counter32 */
    hi_u32 dot11_qos_received_fragment_count;    /* dot11QosReceivedFragmentCount Counter32 */
    hi_u32 dot11_qos_transmitted_frame_count;    /* dot11QosTransmittedFrameCount Counter32 */
    hi_u32 dot11_qos_discarded_frame_count;      /* dot11QosDiscardedFrameCount Counter32 */
    hi_u32 dot11_qos_mpd_us_received_count;      /* dot11QosMPDUsReceivedCount Counter32 */
    hi_u32 dot11_qos_retries_received_count;     /* dot11QosRetriesReceivedCount Counter32 */
} wlan_mib_dot11_qos_counters_entry_stru;

/* ************************************************************************************** */
/* dot11ResourceInfoTable OBJECT-TYPE                                                   */
/* SYNTAX SEQUENCE OF Dot11ResourceInfoEntry                                            */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Provides a means of indicating, in data readable from a managed object,        */
/*      information that identifies the source of the implementation."                  */
/*      REFERENCE "IEEE Std 802.1F-1993, A.7. Note that this standard has been with-    */
/*      drawn."                                                                         */
/* ::= { dot11mac 7 }                                                                   */
/* ::= { dot11resAttribute 2 }                                                          */
/* ************************************************************************************** */
typedef struct {
    hi_u8 auc_dot11manufacturer_oui[3];               /* dot11manufacturerOUI OCTET STRING SIZE(0..3) */
    hi_u8 auc_dot11manufacturer_name[128];            /* dot11manufacturerName DisplayString SIZE(0..128) */
    hi_u8 auc_dot11manufacturer_product_name[128];    /* dot11manufacturerProductName DisplayString SIZE(0..128) */
    hi_u8 auc_dot11manufacturer_product_version[128]; /* dot11manufacturerProductVersion DisplayString SIZE(0..128) */
} wlan_mib_dot11_resource_info_entry_stru;

/* ************************************************************************************** */
/*          Start of dot11res    OBJECT IDENTIFIER ::= { ieee802dot11 3 }               */
/*              dot11resAttribute OBJECT IDENTIFIER ::= { dot11res 1 }                  */
/* ************************************************************************************** */
typedef struct {
    hi_u8 auc_dot11_resource_type_id_name[4]; /* dot11ResourceTypeIDName  DisplayString (SIZE(4)) */
    wlan_mib_dot11_resource_info_entry_stru resource_info;
} wlan_mib_dot11res_attribute_stru;

/* ************************************************************************************** */
/*          Start of dot11phy OBJECT IDENTIFIER ::= { ieee802dot11 4 }                  */
/*              --  PHY GROUPS                                                          */
/*              --  dot11PhyOperationTable ::= { dot11phy 1 }                           */
/*              --  dot11PhyAntennaTable ::= { dot11phy 2 }                             */
/*              --  dot11PhyTxPowerTable ::= { dot11phy 3 }                             */
/*              --  dot11PhyFHSSTable ::= { dot11phy 4 }                                */
/*              --  dot11PhyDSSSTable ::= { dot11phy 5 }                                */
/*              --  dot11PhyIRTable ::= { dot11phy 6 }                                  */
/*              --  dot11RegDomainsSupportedTable ::= { dot11phy 7 }                    */
/*              --  dot11AntennasListTable ::= { dot11phy 8 }                           */
/*              --  dot11SupportedDataRatesTxTable ::= { dot11phy 9 }                   */
/*              --  dot11SupportedDataRatesRxTable ::= { dot11phy 10 }                  */
/*              --  dot11PhyOFDMTable ::= { dot11phy 11 }                               */
/*              --  dot11PhyHRDSSSTable ::= { dot11phy 12 }                             */
/*              --  dot11HoppingPatternTable ::= { dot11phy 13 }                        */
/*              --  dot11PhyERPTable ::= { dot11phy 14 }                                */
/*              --  dot11PhyHTTable  ::= { dot11phy 15 }                                */
/*              --  dot11SupportedMCSTxTable ::= { dot11phy 16 }                        */
/*              --  dot11SupportedMCSRxTable ::= { dot11phy 17 }                        */
/*              --  dot11TransmitBeamformingConfigTable ::= { dot11phy 18 }             */
/*              -- dot11PhyVHTTable ::= { dot11phy 23 } (802.11 ac)                     */
/*              -- dot11VHTTransmitBeamformingConfigTable ::= { dot11phy 24 }(802.11 ac) */
/* ************************************************************************************** */
/* ************************************************************************************** */
/* dot11PhyOperationTable OBJECT-TYPE                                                   */
/* SYNTAX SEQUENCE OF Dot11PhyOperationEntry                                            */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "PHY level attributes concerned with operation. Implemented as a table          */
/*      indexed on ifIndex to allow for multiple instantiations on an Agent."           */
/* ::= { dot11phy 1 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u8 dot11_phy_type;            /* dot11PHYType INTEGER */
    hi_u8 dot11_temp_type;           /* dot11TempType INTEGER */
    hi_u8 resv1[2];                  /* 2:resv */
    hi_u32 dot11_current_reg_domain; /* dot11CurrentRegDomain Unsigned32 */
} wlan_mib_dot11_phy_operation_entry_stru;

/* ************************************************************************************** */
/* dot11PhyAntennaTable OBJECT-TYPE                                                     */
/* SYNTAX SEQUENCE OF Dot11PhyAntennaEntry                                              */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Group of attributes for PhyAntenna. Implemented as a table indexed on          */
/*      ifIndex to allow for multiple instances on an agent."                           */
/* ::= { dot11phy 2}                                                                    */
/* ************************************************************************************** */
typedef struct {
    hi_u8 dot11_antenna_selection_option_implemented; /* dot11AntennaSelectionOptionImplemented TruthValue */
    /* dot11TransmitExplicitCSIFeedbackASOptionImplemented TruthValue */
    hi_u8 dot11_transmit_explicit_csi_feedback_as_option_implemented;
    /* dot11TransmitIndicesFeedbackASOptionImplemented TruthValue */
    hi_u8 dot11_transmit_indices_feedback_as_option_implemented;
    /* dot11ExplicitCSIFeedbackASOptionImplemented TruthValue */
    hi_u8 dot11_explicit_csi_feedback_as_option_implemented;
    /* dot11TransmitIndicesComputationASOptionImplemented TruthValue */
    hi_u8 dot11_transmit_indices_computation_as_option_implemented;
    /* dot11ReceiveAntennaSelectionOptionImplemented TruthValue */
    hi_u8 dot11_receive_antenna_selection_option_implemented;
    hi_u8 dot11_transmit_sounding_ppdu_option_implemented; /* dot11TransmitSoundingPPDUOptionImplemented TruthValue */
} wlan_mib_dot11_phy_antenna_entry_stru;

/* ************************************************************************************** */
/* dot11PhyTxPowerTable OBJECT-TYPE                                                     */
/* SYNTAX SEQUENCE OF Dot11PhyTxPowerEntry                                              */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Group of attributes for dot11PhyTxPowerTable. Implemented as a table           */
/*      indexed on STA ID to allow for multiple instances on an Agent."                 */
/* ::= { dot11phy 3}                                                                    */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_number_supported_power_levels_implemented; /* dot11NumberSupportedPowerLevelsImplemented Unsigned32 */
    hi_u32 dot11_tx_power_level1;                           /* dot11TxPowerLevel1 Unsigned32 */
    hi_u32 dot11_tx_power_level2;                           /* dot11TxPowerLevel2 Unsigned32 */
    hi_u32 dot11_tx_power_level3;                           /* dot11TxPowerLevel3 Unsigned32 */
    hi_u32 dot11_tx_power_level4;                           /* dot11TxPowerLevel4 Unsigned32 */
    hi_u32 dot11_tx_power_level5;                           /* dot11TxPowerLevel5 Unsigned32 */
    hi_u32 dot11_tx_power_level6;                           /* dot11TxPowerLevel6 Unsigned32 */
    hi_u32 dot11_tx_power_level7;                           /* dot11TxPowerLevel7 Unsigned32 */
    hi_u32 dot11_tx_power_level8;                           /* dot11TxPowerLevel8 Unsigned32 */
    hi_u32 dot11_current_tx_power_level;                    /* dot11CurrentTxPowerLevel Unsigned32 */
    hi_u8  auc_dot11_tx_power_level_extended;                /* dot11TxPowerLevelExtended OCTET STRING */
    hi_u8  resv1[3];                                         /* 3:resv */
    hi_u32 dot11_current_tx_power_level_extended;           /* dot11CurrentTxPowerLevelExtended Unsigned32 */
} wlan_mib_dot11_phy_tx_power_entry_stru;

/* ************************************************************************************** */
/* dot11PhyFHSSTable OBJECT-TYPE                                                        */
/* SYNTAX SEQUENCE OF Dot11PhyFHSSEntry                                                 */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Group of attributes for dot11PhyFHSSTable. Implemented as a table indexed      */
/*      on STA ID to allow for multiple instances on an Agent."                         */
/* ::= { dot11phy 4 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_hop_time;                            /* dot11HopTime Unsigned32 */
    hi_u32 dot11_current_channel_number;              /* dot11CurrentChannelNumber Unsigned32 */
    hi_u32 dot11_max_dwell_time;                      /* dot11MaxDwellTime Unsigned32 */
    hi_u32 dot11_current_dwell_time;                  /* dot11CurrentDwellTime Unsigned32 */
    hi_u32 dot11_current_set;                         /* dot11CurrentSet Unsigned32 */
    hi_u32 dot11_current_pattern;                     /* dot11CurrentPattern Unsigned32 */
    hi_u32 dot11_current_index;                       /* dot11CurrentIndex Unsigned32 */
    hi_u32 dot11_ehcc_prime_radix;                    /* dot11EHCCPrimeRadix Unsigned32 */
    hi_u32 dot11_ehcc_numberof_channels_family_index; /* dot11EHCCNumberofChannelsFamilyIndex Unsigned32 */
    hi_u8 dot11_ehcc_capability_implemented;          /* dot11EHCCCapabilityImplemented TruthValue */
    hi_u8 dot11_ehcc_capability_activated;            /* dot11EHCCCapabilityActivated TruthValue */
    hi_u8 dot11_hop_algorithm_adopted;                /* dot11HopAlgorithmAdopted INTEGER */
    hi_u8 dot11_random_table_flag;                    /* dot11RandomTableFlag TruthValue */
    hi_u32 dot11_numberof_hopping_sets;               /* dot11NumberofHoppingSets Unsigned32 */
    hi_u32 dot11_hop_modulus;                         /* dot11HopModulus Unsigned32 */
    hi_u32 dot11_hop_offset;                          /* dot11HopOffset Unsigned32 */
} wlan_mib_dot11_phy_fhss_entry_stru;

/* ************************************************************************************** */
/* dot11PhyDSSSTable OBJECT-TYPE                                                        */
/* SYNTAX SEQUENCE OF Dot11PhyDSSSEntry                                                 */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Entry of attributes for dot11PhyDSSSEntry. Implemented as a table indexed      */
/*      on ifIndex to allow for multiple instances on an Agent."                        */
/* ::= { dot11phy 5 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_current_channel; /* dot11CurrentChannel Unsigned32 */
} wlan_mib_dot11_phy_dsss_entry_stru;

/* ************************************************************************************** */
/* dot11PhyIRTable OBJECT-TYPE                                                          */
/* SYNTAX SEQUENCE OF Dot11PhyIREntry                                                   */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Group of attributes for dot11PhyIRTable. Implemented as a table indexed        */
/*      on ifIndex to allow for multiple instances on an Agent."                        */
/* ::= { dot11phy 6 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_cca_watchdog_timer_max; /* dot11CCAWatchdogTimerMax Unsigned32 */
    hi_u32 dot11_cca_watchdog_count_max; /* dot11CCAWatchdogCountMax Unsigned32 */
    hi_u32 dot11_cca_watchdog_timer_min; /* dot11CCAWatchdogTimerMin Unsigned32 */
    hi_u32 dot11_cca_watchdog_count_min; /* dot11CCAWatchdogCountMin Unsigned32 */
} wlan_mib_dot11_phy_ir_entry_stru;

/* ************************************************************************************** */
/* dot11RegDomainsSupportedTable OBJECT-TYPE                                            */
/* SYNTAX SEQUENCE OF Dot11RegDomainsSupportedEntry                                     */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS deprecated                                                                    */
/* DESCRIPTION                                                                          */
/*      "Superceded by dot11OperatingClassesTable.                                      */
/*      There are different operational requirements dependent on the regulatory        */
/*      domain. This attribute list describes the regulatory domains the PLCP and       */
/*      PMD support in this implementation. Currently defined values and their          */
/*      corresponding Regulatory Domains are:                                           */
/*      FCC (USA) = X'10', DOC (Canada) = X'20', ETSI (most of Europe) = X'30',         */
/*      Spain = X'31', France = X'32', Japan = X'40', China = X'50', Other = X'00'"     */
/* ::= { dot11phy 7}                                                                    */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_reg_domains_supported_index;  /* dot11RegDomainsSupportedIndex Unsigned32 */
    hi_u8 dot11_reg_domains_implemented_value; /* dot11RegDomainsImplementedValue INTEGER */
    hi_u8 resv[3];                             /* 3:resv */
} wlan_mib_dot11_reg_domains_supported_entry_stru;

/* ************************************************************************************** */
/* dot11AntennasListTable OBJECT-TYPE                                                   */
/* SYNTAX SEQUENCE OF Dot11AntennasListEntry                                            */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "This table represents the list of antennae. An antenna can be marked to        */
/*      be capable of transmitting, receiving, and/or for participation in receive      */
/*      diversity. Each entry in this table represents a single antenna with its        */
/*      properties. The maximum number of antennae that can be contained in this        */
/*      table is 255."                                                                  */
/* ::= { dot11phy 8 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_antenna_list_index;                 /* dot11AntennaListIndex Unsigned32 */
    hi_u32 dot11_tx_antenna_implemented;             /* dot11TxAntennaImplemented TruthValue */
    hi_u32 dot11_rx_antenna_implemented;             /* dot11RxAntennaImplemented TruthValue */
    hi_u32 dot11_diversity_selection_rx_implemented; /* dot11DiversitySelectionRxImplemented TruthValue */
} wlan_mib_dot11_antennas_list_entry_stru;

/* ************************************************************************************** */
/* dot11SupportedDataRatesTxTable OBJECT-TYPE                                           */
/* SYNTAX SEQUENCE OF Dot11SupportedDataRatesTxEntry                                    */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "The Transmit bit rates supported by the PLCP and PMD, represented by a         */
/*      count from X'02-X'7f, corresponding to data rates in increments of              */
/*      500kbit/s from 1 Mb/s to 63.5 Mb/s subject to limitations of each individ-      */
/*      ual PHY."                                                                       */
/* ::= { dot11phy 9 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_supported_data_rates_tx_index;   /* dot11SupportedDataRatesTxIndex Unsigned32 */
    hi_u32 dot11_implemented_data_rates_tx_value; /* dot11ImplementedDataRatesTxValue Unsigned32 */
} wlan_mib_dot11_supported_data_rates_tx_entry_stru;

/* ************************************************************************************** */
/* dot11SupportedDataRatesRxTable OBJECT-TYPE                                           */
/* SYNTAX SEQUENCE OF Dot11SupportedDataRatesRxEntry                                    */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "The receive bit rates supported by the PLCP and PMD, represented by a          */
/*      count from X'002-X'7f, corresponding to data rates in increments of             */
/*      500kbit/s from 1 Mb/s to 63.5 Mb/s."                                            */
/* ::= { dot11phy 10 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_supported_data_rates_rx_index;   /* dot11SupportedDataRatesRxIndex Unsigned32 */
    hi_u32 dot11_implemented_data_rates_rx_value; /* dot11ImplementedDataRatesRxValue Unsigned32 */
} wlan_mib_dot11_supported_data_rates_rx_entry_stru;

/* ************************************************************************************** */
/* dot11PhyOFDMTable OBJECT-TYPE                                                        */
/* SYNTAX SEQUENCE OF Dot11PhyOFDMEntry                                                 */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Group of attributes for dot11PhyOFDMTable. Implemented as a table indexed      */
/*      on ifindex to allow for multiple instances on an Agent."                        */
/* ::= { dot11phy 11 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_current_frequency;                /* dot11CurrentFrequency Unsigned32 */
    hi_s32 l_dot11_ti_threshold;                   /* dot11TIThreshold Integer32 */
    hi_u32 dot11_frequency_bands_implemented;      /* dot11FrequencyBandsImplemented Unsigned32 */
    hi_u32 dot11_channel_starting_factor;          /* dot11ChannelStartingFactor Unsigned32 */
    hi_u8 dot11_five_m_hz_operation_implemented;   /* dot11FiveMHzOperationImplemented TruthValue */
    hi_u8 dot11_ten_m_hz_operation_implemented;    /* dot11TenMHzOperationImplemented TruthValue */
    hi_u8 dot11_twenty_m_hz_operation_implemented; /* dot11TwentyMHzOperationImplemented TruthValue */
    hi_u8 dot11_phy_ofdm_channel_width;            /* dot11PhyOFDMChannelWidth INTEGER */
    hi_u8 dot11_ofdmccaed_implemented;             /* dot11OFDMCCAEDImplemented  TruthValue */
    hi_u8 dot11_ofdmccaed_required;                /* dot11OFDMCCAEDRequired  TruthValue */
    hi_u32 dot11_ofdmed_threshold;                 /* dot11OFDMEDThreshold  Unsigned32 */
    hi_u8 dot11_sta_transmit_power_class;          /* dot11STATransmitPowerClass INTEGER */
    hi_u8 dot11_acr_type;                          /* dot11ACRType INTEGER */
} wlan_mib_dot11_phy_ofdm_entry_stru;

/* ************************************************************************************** */
/* dot11PhyHRDSSSTable OBJECT-TYPE                                                      */
/* SYNTAX SEQUENCE OF Dot11PhyHRDSSSEntry                                               */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Entry of attributes for dot11PhyHRDSSSEntry. Implemented as a table            */
/*      indexed on ifIndex to allow for multiple instances on an Agent."                */
/* ::= { dot11phy 12 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u8 dot11_short_preamble_option_implemented; /* dot11ShortPreambleOptionImplemented TruthValue */
    hi_u8 dot11_pbcc_option_implemented;           /* dot11PBCCOptionImplemented TruthValue */
    hi_u8 dot11_channel_agility_present;           /* dot11ChannelAgilityPresent TruthValue */
    hi_u8 dot11_channel_agility_activated;         /* dot11ChannelAgilityActivated TruthValue */
} wlan_mib_dot11_phy_hrdsss_entry_stru;

/* ************************************************************************************** */
/* dot11HoppingPatternTable OBJECT-TYPE                                                 */
/* SYNTAX SEQUENCE OF Dot11HoppingPatternEntry                                          */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "The (conceptual) table of attributes necessary for a frequency hopping         */
/*      implementation to be able to create the hopping sequences necessary to          */
/*      operate in the subband for the associated domain country string."               */
/* ::= { dot11phy 13 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_hopping_pattern_index;     /* dot11HoppingPatternIndex Unsigned32 */
    hi_u32 dot11_random_table_field_number; /* dot11RandomTableFieldNumber Unsigned32 */
} wlan_mib_dot11_hopping_pattern_entry_stru;

/* ************************************************************************************** */
/* dot11PhyERPTable OBJECT-TYPE                                                         */
/* SYNTAX SEQUENCE OF Dot11PhyERPEntry                                                  */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Entry of attributes for dot11PhyERPEntry. Implemented as a table indexed       */
/*      on ifIndex to allow for multiple instances on an Agent."                        */
/* ::= { dot11phy 14 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u8 dot11_dsssofdm_option_activated;          /* dot11DSSSOFDMOptionActivated TruthValue */
    hi_u8 dot11_short_slot_time_option_implemented; /* dot11ShortSlotTimeOptionImplemented TruthValue */
    hi_u8 dot11_short_slot_time_option_activated;   /* dot11ShortSlotTimeOptionActivated TruthValue */
} wlan_mib_dot11_phy_erp_entry_stru;

/* ************************************************************************************** */
/* dot11PhyHTTable OBJECT-TYPE                                                          */
/* SYNTAX SEQUENCE OF Dot11PhyHTEntry                                                   */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Entry of attributes for dot11PhyHTTable. Implemented as a table indexed        */
/*      on ifIndex to allow for multiple instances on an Agent."                        */
/* ::= { dot11phy 15 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u8 dot112_g_forty_m_hz_operation_implemented;     /* dot11FortyMHzOperationImplemented TruthValue */
    hi_u8 dot11_short_gi_option_in_twenty_implemented;   /* dot11ShortGIOptionInTwentyImplemented TruthValue */
    hi_u8 dot112_g_short_gi_option_in_forty_implemented; /* dot11ShortGIOptionInFortyImplemented TruthValue */
    hi_u8 dot11_ldpc_coding_option_implemented;          /* dot11LDPCCodingOptionImplemented TruthValue */
    hi_u8 dot11_ldpc_coding_option_activated;            /* dot11LDPCCodingOptionActivated TruthValue */
    hi_u8 dot11_tx_stbc_option_implemented;              /* dot11TxSTBCOptionImplemented TruthValue */
    hi_u8 dot11_tx_stbc_option_activated;                /* dot11TxSTBCOptionActivated TruthValue */
    hi_u8 dot11_rx_stbc_option_implemented;              /* dot11RxSTBCOptionImplemented TruthValue */
} wlan_mib_dot11_phy_ht_entry_stru;

/* ************************************************************************************** */
/* dot11SupportedMCSTxTable OBJECT-TYPE                                                 */
/* SYNTAX SEQUENCE OF Dot11SupportedMCSTxEntry                                          */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "he Transmit MCS supported by the PLCP and PMD, represented by a count          */
/*      from 1 to 127, subject to limitations of each individual PHY."                  */
/* ::= { dot11phy 16 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u8 auc_dot11_supported_mcs_tx_value[WLAN_HT_MCS_BITMASK_LEN]; /* dot11SupportedMCSTxValue Unsigned32 */
} wlan_mib_dot11_supported_mcs_tx_entry_stru;

/* ************************************************************************************** */
/* dot11SupportedMCSRxTable OBJECT-TYPE                                                 */
/* SYNTAX SEQUENCE OF Dot11SupportedMCSRxEntry                                          */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "The receive MCS supported by the PLCP and PMD, represented by a count          */
/*      from 1 to 127, subject to limitations of each individual PHY."                  */
/* ::= { dot11phy 17 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u8 auc_dot11_supported_mcs_rx_value[WLAN_HT_MCS_BITMASK_LEN]; /* dot11SupportedMCSRxValue Unsigned32 */
} wlan_mib_dot11_supported_mcs_rx_entry_stru;

/* ************************************************************************************** */
/* dot11TransmitBeamformingConfigTable OBJECT-TYPE                                      */
/* SYNTAX SEQUENCE OF Dot11TransmitBeamformingConfigEntry                               */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Entry of attributes for dot11TransmitBeamformingConfigTable. Implemented       */
/*      as a table indexed on ifIndex to allow for multiple instances on an             */
/*      Agent."                                                                         */
/* ::= { dot11phy 18 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    /* dot11ReceiveStaggerSoundingOptionImplemented TruthValue */
    hi_u8 dot11_receive_stagger_sounding_option_implemented;
    /* dot11TransmitStaggerSoundingOptionImplemented TruthValue */
    hi_u8 dot11_transmit_stagger_sounding_option_implemented;
    hi_u8 dot11_receive_ndp_option_implemented;  /* dot11ReceiveNDPOptionImplemented TruthValue */
    hi_u8 dot11_transmit_ndp_option_implemented; /* dot11TransmitNDPOptionImplemented TruthValue */
    /* dot11ImplicitTransmitBeamformingOptionImplemented TruthValue */
    hi_u8 dot11_implicit_transmit_beamforming_option_implemented;
    hi_u8 dot11_calibration_option_implemented; /* dot11CalibrationOptionImplemented INTEGER */
    /* dot11ExplicitCSITransmitBeamformingOptionImplemented  TruthValue */
    hi_u8 dot11_explicit_csi_transmit_beamforming_option_implemented;
    /* dot11ExplicitNonCompressedBeamformingMatrixOptionImplemented TruthValue */
    hi_u8 dot11_explicit_non_compressed_beamforming_matrix_option_implemented;
    /* dot11ExplicitTransmitBeamformingCSIFeedbackOptionImplemented INTEGER */
    hi_u8 dot11_explicit_transmit_beamforming_csi_feedback_option_implemented;
    /* dot11ExplicitNonCompressedBeamformingFeedbackOptionImplemented INTEGER */
    hi_u8 dot11_explicit_non_compressed_beamforming_feedback_option_implemented;
    /* dot11ExplicitCompressedBeamformingFeedbackOptionImplemented INTEGER */
    hi_u8 dot11_explicit_compressed_beamforming_feedback_option_implemented;
    hi_u8 resv1;
    /* dot11NumberBeamFormingCSISupportAntenna Unsigned32 */
    hi_u32 dot11_number_beam_forming_csi_support_antenna;
    /* dot11NumberNonCompressedBeamformingMatrixSupportAntenna Unsigned32 */
    hi_u32 dot11_number_non_compressed_beamforming_matrix_support_antenna;
    /* dot11NumberCompressedBeamformingMatrixSupportAntenna  Unsigned32 */
    hi_u32 dot11_number_compressed_beamforming_matrix_support_antenna;
} wlan_mib_dot11_transmit_beamforming_config_entry_stru;

/* ************************************************************************************** */
/* dot11PhyVHTTable OBJECT-TYPE                                                         */
/* SYNTAX SEQUENCE OF Dot11PhyVHTEntry                                                  */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/*      DESCRIPTION                                                                     */
/*      "Entry of attributes for dot11PhyVHTTable. Implemented as a table indexed       */
/*      on ifIndex to allow for multiple instances on an Agent."                        */
/* ::= { dot11phy 23 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u8 dot11_vht_channel_width_option_implemented; /* dot11VHTChannelWidthOptionImplemented INTEGER */
    hi_u8 dot11_vht_short_gi_option_in80_implemented; /* dot11VHTShortGIOptionIn80Implemented TruthValue */
    /* dot11VHTShortGIOptionIn160and80p80Implemented TruthValue */
    hi_u8 dot11_vht_short_gi_option_in160and80p80_implemented;
    hi_u8 dot11_vhtldpc_coding_option_implemented; /* dot11VHTLDPCCodingOptionImplemented TruthValue */
    hi_u8 dot11_vht_tx_stbc_option_implemented;    /* dot11VHTTxSTBCOptionImplemented TruthValue */
    hi_u8 dot11_vht_rx_stbc_option_implemented;    /* dot11VHTRxSTBCOptionImplemented TruthValue */
} wlan_mib_dot11_phy_vht_entry_stru;

/* ************************************************************************************** */
/* dot11VHTTransmitBeamformingConfigTable OBJECT-TYPE                                   */
/* SYNTAX SEQUENCE OF Dot11VHTTransmitBeamformingConfigEntry                            */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "Entry of attributes for dot11VHTTransmitBeamformingConfigTable. Imple-         */
/*      mented as a table indexed on ifIndex to allow for multiple instances on an      */
/*      Agent."                                                                         */
/*  ::= { dot11phy 24 }                                                                 */
/* ************************************************************************************** */
typedef struct {
    hi_u8 dot11_vhtsu_beamformee_option_implemented; /* dot11VHTSUBeamformeeOptionImplemented TruthValue */
    hi_u8 dot11_vhtsu_beamformer_option_implemented; /* dot11VHTSUBeamformerOptionImplemented TruthValue */
    hi_u8 dot11_vhtmu_beamformee_option_implemented; /* dot11VHTMUBeamformeeOptionImplemented TruthValue */
    hi_u8 dot11_vhtmu_beamformer_option_implemented; /* dot11VHTMUBeamformerOptionImplemented TruthValue */
    hi_u32 dot11_vht_number_sounding_dimensions;     /* dot11VHTNumberSoundingDimensions  Unsigned32 */
    hi_u32 dot11_vht_beamformee_n_tx_support;        /* dot11VHTBeamformeeNTxSupport Unsigned32 */
} wlan_mib_dot11_vht_transmit_beamforming_config_entry_stru;

/* Start of dot11imt OBJECT IDENTIFIER ::= {ieee802dot11 6} */
/* ************************************************************************************** */
/*        Start of dot11imt OBJECT IDENTIFIER ::= {ieee802dot11 6}                      */
/*          -- IMT GROUPS                                                               */
/*          -- dot11BSSIdTable ::= { dot11imt 1 }                                       */
/*          -- dot11InterworkingTable ::= { dot11imt 2 }                                */
/*          -- dot11APLCI ::= { dot11imt 3 }                                            */
/*          -- dot11APCivicLocation ::= { dot11imt 4 }                                  */
/*          -- dot11RoamingConsortiumTable      ::= { dot11imt 5 }                      */
/*          -- dot11DomainNameTable ::= { dot11imt 6 }                                  */
/*          -- Generic Advertisement Service (GAS) Attributes                           */
/*          -- DEFINED AS "The Generic Advertisement Service management                 */
/*          -- object class provides the necessary support for an Advertisement         */
/*          -- service to interwork with external systems."                             */
/*          -- GAS GROUPS                                                               */
/*          -- dot11GASAdvertisementTable       ::= { dot11imt 7 }                      */
/* ************************************************************************************** */
/* ************************************************************************************** */
/* dot11BSSIdTable OBJECT-TYPE                                                          */
/* SYNTAX         SEQUENCE OF Dot11BSSIdEntry                                           */
/* MAX-ACCESS     not-accessible                                                        */
/* STATUS         current                                                               */
/* DESCRIPTION                                                                          */
/*      "This object is a table of BSSIDs contained within an Access Point (AP)."       */
/*  ::= { dot11imt 1 }                                                                  */
/* ************************************************************************************** */
typedef struct {
    hi_u8 auc_dot11_ap_mac_address[6]; /* dot11APMacAddress OBJECT-TYPE MacAddress SIZE(0..6) */
} wlan_mib_dot11_bss_id_entry_stru;

/* ************************************************************************************** */
/* dot11InterworkingTable OBJECT-TYPE                                                   */
/* SYNTAX SEQUENCE OF Dot11InterworkingEntry                                            */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "This table represents the non-AP STAs associated to the AP. An entry is        */
/*      created automatically by the AP when the STA becomes associated to the AP.      */
/*      The corresponding entry is deleted when the STA disassociates. Each STA         */
/*      added to this table is uniquely identified by its MAC address. This table       */
/*      is moved to a new AP following a successful STA BSS transition event."          */
/* ::= { dot11imt 2 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u8 auc_dot11_non_ap_station_mac_address[6];      /* dot11NonAPStationMacAddress MacAddress SIZE(0..6) */
    hi_u8 auc_dot11_non_ap_station_user_identity[255];  /* dot11NonAPStationUserIdentity DisplayString SIZE(0..255) */
    hi_u8 dot11_non_ap_station_interworking_capability; /* dot11NonAPStationInterworkingCapability BITS */
    hi_u8 auc_dot11_non_ap_station_associated_ssid[32]; /* dot11NonAPStationAssociatedSSID OCTET STRING SIZE(0..32) */
    /* dot11NonAPStationUnicastCipherSuite OCTET STRING */
    hi_u8 auc_dot11_non_ap_station_unicast_cipher_suite[4]; /* SIZE(0..4) */
    /* dot11NonAPStationBroadcastCipherSuite OCTET STRING */
    hi_u8 auc_dot11_non_ap_station_broadcast_cipher_suite[4]; /* SIZE(0..4) */
    hi_u8 dot11_non_ap_station_auth_access_categories;        /* dot11NonAPStationAuthAccessCategories BITS */
    hi_u8 dot11_non_ap_station_auth_hccahemm;                 /* dot11NonAPStationAuthHCCAHEMM TruthValue */
    hi_u32 dot11_non_ap_station_auth_max_voice_rate;          /* dot11NonAPStationAuthMaxVoiceRate Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_video_rate;          /* dot11NonAPStationAuthMaxVideoRate Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_best_effort_rate;    /* dot11NonAPStationAuthMaxBestEffortRate Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_background_rate;     /* dot11NonAPStationAuthMaxBackgroundRate Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_voice_octets;        /* dot11NonAPStationAuthMaxVoiceOctets Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_video_octets;        /* dot11NonAPStationAuthMaxVideoOctets Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_best_effort_octets;  /* dot11NonAPStationAuthMaxBestEffortOctets Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_background_octets;   /* dot11NonAPStationAuthMaxBackgroundOctets Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_hccahemm_octets;     /* dot11NonAPStationAuthMaxHCCAHEMMOctets Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_total_octets;        /* dot11NonAPStationAuthMaxTotalOctets Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_hccahemm_rate;       /* dot11NonAPStationAuthMaxHCCAHEMMRate Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_hccahemm_delay;          /* dot11NonAPStationAuthHCCAHEMMDelay Unsigned32 */
    hi_u8 dot11_non_ap_station_auth_source_multicast;         /* dot11NonAPStationAuthSourceMulticast TruthValue */
    hi_u8 dot11_non_ap_station_power_management_mode;         /* dot11NonAPStationPowerManagementMode INTEGER */
    hi_u8 dot11_non_ap_station_auth_dls;                      /* dot11NonAPStationAuthDls TruthValue */
    hi_u8 dot11_non_ap_station_addts_result_code;             /* dot11NonAPStationAddtsResultCode INTEGER */
    /* dot11NonAPStationAuthMaxSourceMulticastRate Unsigned32 */
    hi_u32 dot11_non_ap_station_auth_max_source_multicast_rate;
    hi_u32 dot11_non_ap_station_voice_msdu_count;          /* dot11NonAPStationVoiceMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_voice_msdu_count;  /* dot11NonAPStationDroppedVoiceMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_voice_octet_count;         /* dot11NonAPStationVoiceOctetCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_voice_octet_count; /* dot11NonAPStationDroppedVoiceOctetCount Counter32 */
    hi_u32 dot11_non_ap_station_video_msdu_count;          /* dot11NonAPStationVideoMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_video_msdu_count;  /* dot11NonAPStationDroppedVideoMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_video_octet_count;         /* dot11NonAPStationVideoOctetCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_video_octet_count; /* dot11NonAPStationDroppedVideoOctetCount Counter32 */
    hi_u32 dot11_non_ap_station_best_effort_msdu_count;    /* dot11NonAPStationBestEffortMSDUCount Counter32 */
    /* dot11NonAPStationDroppedBestEffortMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_best_effort_msdu_count;
    hi_u32 dot11_non_ap_station_best_effort_octet_count; /* dot11NonAPStationBestEffortOctetCount Counter32 */
    /* dot11NonAPStationDroppedBestEffortOctetCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_best_effort_octet_count;
    hi_u32 dot11_non_ap_station_background_msdu_count; /* dot11NonAPStationBackgroundMSDUCount Counter32 */
    /* dot11NonAPStationDroppedBackgroundMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_background_msdu_count;
    hi_u32 dot11_non_ap_station_background_octet_count; /* dot11NonAPStationBackgroundOctetCount Counter32 */
    /* dot11NonAPStationDroppedBackgroundOctetCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_background_octet_count;
    hi_u32 dot11_non_ap_station_hccahemmmsdu_count;         /* dot11NonAPStationHCCAHEMMMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_hccahemmmsdu_count; /* dot11NonAPStationDroppedHCCAHEMMMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_hccahemm_octet_count;       /* dot11NonAPStationHCCAHEMMOctetCount Counter32 */
    /* dot11NonAPStationDroppedHCCAHEMMOctetCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_hccahemm_octet_count;
    hi_u32 dot11_non_ap_station_multicast_msdu_count; /* dot11NonAPStationMulticastMSDUCount Counter32 */
    /* dot11NonAPStationDroppedMulticastMSDUCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_multicast_msdu_count;
    hi_u32 dot11_non_ap_station_multicast_octet_count; /* dot11NonAPStationMulticastOctetCount Counter32 */
    /* dot11NonAPStationDroppedMulticastOctetCount Counter32 */
    hi_u32 dot11_non_ap_station_dropped_multicast_octet_count;
    hi_u32 dot11_non_ap_station_vlan_id;          /* dot11NonAPStationVLANId Unsigned32 */
    hi_u8 auc_dot11_non_ap_station_vlan_name[64]; /* dot11NonAPStationVLANName DisplayString SIZE(0..64) */
} wlan_mib_dot11_interworking_entry_stru;

/* ************************************************************************************** */
/* dot11APLCITable OBJECT-TYPE                                                          */
/* SYNTAX         SEQUENCE OF Dot11APLCIEntry                                           */
/* MAX-ACCESS     not-accessible                                                        */
/* STATUS         current                                                               */
/* DESCRIPTION                                                                          */
/*      "This table represents the Geospatial location of the AP as specified in        */
/*      8.4.2.23.10."                                                                   */
/* ::= { dot11imt 3 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_aplci_index;                                         /* dot11APLCIIndex Unsigned32 */
    hi_u32 dot11_aplci_latitude_resolution;                           /* dot11APLCILatitudeResolution Unsigned32 */
    hi_s32 l_dot11_aplci_latitude_integer;                            /* dot11APLCILatitudeInteger Integer32 */
    hi_s32 l_dot11_aplci_latitude_fraction;                           /* dot11APLCILatitudeFraction Integer32 */
    hi_u32 dot11_aplci_longitude_resolution;                          /* dot11APLCILongitudeResolution Unsigned32 */
    hi_s32 l_dot11_aplci_longitude_integer;                           /* dot11APLCILongitudeInteger Integer32 */
    hi_s32 l_dot11_aplci_longitude_fraction;                          /* dot11APLCILongitudeFraction Integer32 */
    hi_u8 dot11_aplci_altitude_type;                                  /* dot11APLCIAltitudeType INTEGER */
    wlan_mib_ap_lci_datum_enum_uint8 dot11_aplci_datum;               /* dot11APLCIDatum INTEGER */
    wlan_mib_ap_lci_azimuth_type_enum_uint8 dot11_aplci_azimuth_type; /* dot11APLCIAzimuthType INTEGER */
    hi_u8 resv;
    hi_u32 dot11_aplci_altitude_resolution; /* dot11APLCIAltitudeResolution Unsigned32 */
    hi_s32 l_dot11_aplci_altitude_integer;  /* dot11APLCIAltitudeInteger Integer32 */
    hi_s32 l_dot11_aplci_altitude_fraction; /* dot11APLCIAltitudeFraction Integer32 */
    hi_u32 dot11_aplci_azimuth_resolution;  /* dot11APLCIAzimuthResolution Unsigned32 */
    hi_s32 l_dot11_aplci_azimuth;           /* dot11APLCIAzimuth Integer32 */
} wlan_mib_dot11_aplci_entry_stru;

/* ************************************************************************************** */
/* dot11APCivicLocationTable OBJECT-TYPE                                                */
/* SYNTAX SEQUENCE OF Dot11ApCivicLocationEntry                                         */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "This table represents the location of the AP in Civic format using the         */
/*      Civic Address Type elements defined in IETF RFC-5139 [B42]."                    */
/* ::= { dot11imt 4 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_ap_civic_location_index;           /* dot11APCivicLocationIndex Unsigned32 */
    hi_u8 auc_dot11_ap_civic_location_country[255]; /* dot11APCivicLocationCountry OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_a1[255];      /* dot11APCivicLocationA1 OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_a2[255];      /* dot11APCivicLocationA2 OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_a3[255];      /* dot11APCivicLocationA3 OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_a4[255];      /* dot11APCivicLocationA4 OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_a5[255];      /* dot11APCivicLocationA5 OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_a6[255];      /* dot11APCivicLocationA6 OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_prd[255];     /* dot11APCivicLocationPrd OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_pod[255];     /* dot11APCivicLocationPod OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_sts[255];     /* dot11APCivicLocationSts OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_hno[255];     /* dot11APCivicLocationHno OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_hns[255];     /* dot11APCivicLocationHns OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_lmk[255];     /* dot11APCivicLocationLmk OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_loc[255];     /* dot11APCivicLocationLoc OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_nam[255];     /* dot11APCivicLocationNam OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_pc[255];      /* dot11APCivicLocationPc OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_bld[255];     /* dot11APCivicLocationBld OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_unit[255];    /* dot11APCivicLocationUnit OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_flr[255];     /* dot11APCivicLocationFlr OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_room[255];    /* dot11APCivicLocationRoom OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_plc[255];     /* dot11APCivicLocationPlc OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_pcn[255];     /* dot11APCivicLocationPcn OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_pobox[255];   /* dot11APCivicLocationPobox OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_addcode[255]; /* dot11APCivicLocationAddcode OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_seat[255];    /* dot11APCivicLocationSeat OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_rd[255];      /* dot11APCivicLocationRd OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_rdsec[255];   /* dot11APCivicLocationRdsec OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_rdbr[255];    /* dot11APCivicLocationRdbr OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_rdsubbr[255]; /* dot11APCivicLocationRdsubbr OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_prm[255];     /* dot11APCivicLocationPrm OCTET STRING SIZE(0..255) */
    hi_u8 auc_dot11_ap_civic_location_pom[255];     /* dot11APCivicLocationPom OCTET STRING SIZE(0..255) */
} wlan_mib_dot11_ap_civic_location_entry_stru;

/* ************************************************************************************** */
/* dot11RoamingConsortiumTable OBJECT-TYPE                                              */
/* SYNTAX SEQUENCE OF Dot11RoamingConsortiumEntry                                       */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "This is a Table of OIs which are to be transmitted in an ANQP Roaming          */
/*      Consortium ANQP-element. Each table entry corresponds to a roaming consor-      */
/*      tium or single SSP. The first 3 entries in this table are transmitted in        */
/*      Beacon and Probe Response frames."                                              */
/* ::= { dot11imt 5 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u8 auc_dot11_roaming_consortium_oi[16]; /* dot11RoamingConsortiumOI OCTET STRING SIZE(0..16) */
    wlan_mib_row_status_enum_uint8 dot11_roaming_consortium_row_status; /* dot11RoamingConsortiumRowStatus RowStatus */
} wlan_mib_dot11_roaming_consortium_entry_stru;

/* ************************************************************************************** */
/* dot11DomainNameTable   OBJECT-TYPE                                                   */
/* SYNTAX                SEQUENCE OF Dot11DomainNameEntry                               */
/* MAX-ACCESS            not-accessible                                                 */
/* STATUS                current                                                        */
/* DESCRIPTION                                                                          */
/*      "This is a table of Domain Names which form the Domain Name list in Access      */
/*      Network Query Protocol. The Domain Name list may be transmitted to a non-       */
/*      AP STA in a GAS Response. Each table entry corresponds to a single Domain       */
/*      Name."                                                                          */
/* ::= { dot11imt 6 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u8 auc_dot11_domain_name[255];                            /* dot11DomainName OCTET STRING SIZE(0..255) */
    wlan_mib_row_status_enum_uint8 dot11_domain_name_row_status; /*  dot11DomainNameRowStatus RowStatus */
    hi_u8 auc_dot11_domain_name_oui[5];                          /* dot11DomainNameOui OCTET STRING (SIZE(3..5)) */
} wlan_mib_dot11_domain_name_entry_stru;

/* ************************************************************************************** */
/* dot11GASAdvertisementTable OBJECT-TYPE                                               */
/* SYNTAX SEQUENCE OF Dot11GASAdvertisementEntry                                        */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "This object is a table of GAS counters that allows for multiple instanti-      */
/*      ations of those counters on an STA."                                            */
/* ::= { dot11imt 7 }                                                                   */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_gas_advertisement_id;            /* dot11GASAdvertisementId Unsigned32 */
    hi_u8  dot11_gas_pause_for_server_response;   /* dot11GASPauseForServerResponse TruthValue */
    hi_u8  resv1[3];                              /* 3:resv */
    hi_u32 dot11_gas_response_timeout;            /* dot11GASResponseTimeout Unsigned32 */
    hi_u32 dot11_gas_comeback_delay;              /* dot11GASComebackDelay Unsigned32 */
    hi_u32 dot11_gas_response_buffering_time;     /* dot11GASResponseBufferingTime Unsigned32 */
    hi_u32 dot11_gas_query_response_length_limit; /* dot11GASQueryResponseLengthLimit Unsigned32 */
    hi_u32 dot11_gas_queries;                     /* dot11GASQueries Counter32 */
    hi_u32 dot11_gas_query_rate;                  /* dot11GASQueryRate Gauge32 */
    hi_u32 dot11_gas_responses;                   /* dot11GASResponses Counter32 */
    hi_u32 dot11_gas_response_rate;               /* dot11GASResponseRate Gauge32 */
    hi_u32 dot11_gas_transmitted_fragment_count;  /* dot11GASTransmittedFragmentCount Counter32 */
    hi_u32 dot11_gas_received_fragment_count;     /* dot11GASReceivedFragmentCount Counter32 */
    hi_u32 dot11_gas_no_request_outstanding;      /* dot11GASNoRequestOutstanding Counter32 */
    hi_u32 dot11_gas_responses_discarded;         /* dot11GASResponsesDiscarded Counter32 */
    hi_u32 dot11_gas_failed_responses;            /* dot11GASFailedResponses Counter32 */
} wlan_mib_dot11_gas_advertisement_entry_stru;

/* ************************************************************************************** */
/*          Start of dot11MSGCF OBJECT IDENTIFIER ::= { ieee802dot11 7}                 */
/*              -- MAC State GROUPS                                                     */
/*              -- dot11MACStateConfigTable ::= { dot11MSGCF 1 }                        */
/*              -- dot11MACStateParameterTable ::= { dot11MSGCF 2 }                     */
/*              -- dot11MACStateESSLinkTable ::= { dot11MSGCF 3 }                       */
/* ************************************************************************************** */
/* ************************************************************************************** */
/* dot11MACStateConfigTable OBJECT-TYPE                                                 */
/* SYNTAX SEQUENCE OF Dot11MACStateConfigEntry                                          */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "This table holds configuration parameters for the 802.11 MAC                   */
/*      State Convergence Function."                                                    */
/* ::= { dot11MSGCF 1 }                                                                 */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_ess_disconnect_filter_interval;   /* dot11ESSDisconnectFilterInterval Unsigned32 */
    hi_u32 dot11_ess_link_detection_hold_interval; /* dot11ESSLinkDetectionHoldInterval Unsigned32 */
    hi_u8 auc_dot11_msceess_link_identifier[38];   /* dot11MSCEESSLinkIdentifier Dot11ESSLinkIdentifier SIZE(0..38) */
    hi_u8 auc_dot11_msce_non_ap_station_mac_address[6]; /* dot11MSCENonAPStationMacAddress MacAddress SIZE(0..6) */
} wlan_mib_dot11_mac_state_config_entry_stru;

/* ************************************************************************************** */
/* dot11MACStateParameterTable OBJECT-TYPE                                              */
/* SYNTAX SEQUENCE OF Dot11MACStateParameterEntry                                       */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS     current                                                                   */
/* DESCRIPTION                                                                          */
/*      "This table holds the current parameters used for each 802.11 network for       */
/*      802.11 MAC convergence functions."                                              */
/* ::= { dot11MSGCF 2 }                                                                 */
/* ************************************************************************************** */
typedef struct {
    hi_u32 dot11_ess_link_down_time_interval;    /* dot11ESSLinkDownTimeInterval Unsigned32 */
    hi_u32 dot11_ess_link_rssi_data_threshold;   /* dot11ESSLinkRssiDataThreshold Unsigned32 */
    hi_u32 dot11_ess_link_rssi_beacon_threshold; /* dot11ESSLinkRssiBeaconThreshold Unsigned32 */
    hi_u32 dot11_ess_link_data_snr_threshold;    /* dot11ESSLinkDataSnrThreshold Unsigned32 */
    hi_u32 dot11_ess_link_beacon_snr_threshold;  /* dot11ESSLinkBeaconSnrThreshold Unsigned32 */
    /* dot11ESSLinkBeaconFrameErrorRateThresholdInteger Unsigned32 */
    hi_u32 dot11_ess_link_beacon_frame_error_rate_threshold_integer;
    /* dot11ESSLinkBeaconFrameErrorRateThresholdFraction Unsigned32 */
    hi_u32 dot11_ess_link_beacon_frame_error_rate_threshold_fraction;
    /* dot11ESSLinkBeaconFrameErrorRateThresholdExponent Unsigned32 */
    hi_u32 dot11_ess_link_beacon_frame_error_rate_threshold_exponent;
    /* dot11ESSLinkFrameErrorRateThresholdInteger Unsigned32 */
    hi_u32 dot11_ess_link_frame_error_rate_threshold_integer;
    /* dot11ESSLinkFrameErrorRateThresholdFraction Unsigned32 */
    hi_u32 dot11_ess_link_frame_error_rate_threshold_fraction;
    /* dot11ESSLinkFrameErrorRateThresholdExponent Unsigned32 */
    hi_u32 dot11_ess_link_frame_error_rate_threshold_exponent;
    hi_u32 dot11_peak_operational_rate;             /* dot11PeakOperationalRate Unsigned32 */
    hi_u32 dot11_minimum_operational_rate;          /* dot11MinimumOperationalRate Unsigned32 */
    hi_u32 dot11_ess_link_data_throughput_integer;  /* dot11ESSLinkDataThroughputInteger Unsigned32 */
    hi_u32 dot11_ess_link_data_throughput_fraction; /* dot11ESSLinkDataThroughputFraction Unsigned32 */
    hi_u32 dot11_ess_link_data_throughput_exponent; /* dot11ESSLinkDataThroughputExponent Unsigned32 */
    hi_u8 auc_dot11_mspeess_link_identifier[38];    /* dot11MSPEESSLinkIdentifier Dot11ESSLinkIdentifier SIZE(0..38) */
    hi_u8 auc_dot11_mspe_non_ap_station_mac_address[6]; /* dot11MSPENonAPStationMacAddress MacAddress SIZE(0..6) */
} wlan_mib_dot11_mac_state_parameter_entry_stru;

/* ************************************************************************************** */
/* dot11MACStateESSLinkDetectedTable OBJECT-TYPE                                        */
/* SYNTAX SEQUENCE OF Dot11MACStateESSLinkDetectedEntry                                 */
/* MAX-ACCESS not-accessible                                                            */
/* STATUS current                                                                       */
/* DESCRIPTION                                                                          */
/*      "This table holds the detected 802.11 network list used for MAC conver-         */
/*      gence functions."                                                               */
/* ::= { dot11MSGCF 3 }                                                                 */
/* ************************************************************************************** */
typedef struct {
    hi_u32  dot11_ess_link_detected_index;                 /* dot11ESSLinkDetectedIndex Unsigned32 */
    hi_s8   ac_dot11_ess_link_detected_network_id[255];   /* dot11ESSLinkDetectedNetworkId OCTET STRING SIZE(0..255) */
    hi_u32  dot11_ess_link_detected_network_detect_time;   /* dot11ESSLinkDetectedNetworkDetectTime Unsigned32 */
    hi_u32  dot11_ess_link_detected_network_modified_time;     /* dot11ESSLinkDetectedNetworkModifiedTime Unsigned32 */
    hi_u8   dot11_ess_link_detected_network_mih_capabilities;  /* dot11ESSLinkDetectedNetworkMIHCapabilities BITS */
    hi_u8   auc_dot11_mseldeess_link_identifier[38];  /* dot11MSELDEESSLinkIdentifier Dot11ESSLinkIdentifier SIZE(38) */
    hi_u8   auc_dot11_mselde_non_ap_station_mac_address[6];  /* dot11MSELDENonAPStationMacAddress  MacAddress SIZE(6) */
} wlan_mib_dot11_mac_state_ess_link_detected_entry_stru;

typedef struct {
    /* **************************************************************************
        dot11smt OBJECT IDENTIFIER ::= { ieee802dot11 1 }
    *************************************************************************** */
    /* --  dot11StationConfigTable ::= { dot11smt 1 } */
    wlan_mib_dot11_station_config_entry_stru wlan_mib_sta_config;

    /* --  dot11AuthenticationAlgorithmsTable ::= { dot11smt 2 } */
    wlan_mib_dot11_authentication_algorithms_entry_stru wlan_mib_auth_alg;

    /* --  dot11WEPDefaultKeysTable ::= { dot11smt 3 } */
    wlan_mib_dot11_wep_default_keys_entry_stru ast_wlan_mib_wep_dflt_key[WLAN_NUM_DOT11WEPDEFAULTKEYVALUE];

    /* --  dot11PrivacyTable ::= { dot11smt 5 } */
    wlan_mib_dot11_privacy_entry_stru wlan_mib_privacy;

    /* --  dot11RSNAConfigTable ::= { dot11smt 9 } */
    wlan_mib_dot11_rsna_config_entry_stru wlan_mib_rsna_cfg;

    /* --  dot11RSNAConfigPairwiseCiphersTable ::= { dot11smt 10 } */
    wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru ast_wlan_mib_rsna_cfg_wpa_pairwise_cipher[WLAN_PAIRWISE_CIPHER_SUITES];
    wlan_mib_dot11_rsna_cfg_pwise_cpher_en_stru ast_wlan_mib_rsna_cfg_wpa2_pairwise_cipher[WLAN_PAIRWISE_CIPHER_SUITES];

    /* --  dot11RSNAConfigAuthenticationSuitesTable      ::= { dot11smt 11 } */
    wlan_mib_dot11_rsna_cfg_authentication_suites_en_stru ast_wlan_mib_rsna_cfg_auth_suite[WLAN_AUTHENTICATION_SUITES];

    /* --  dot11MeshSTAConfigTable ::= { dot11smt 23 } */
    wlan_mib_dot11_mesh_sta_config_entry_stru wlan_mib_mesh_sta_cfg;

    /* **************************************************************************
        dot11mac OBJECT IDENTIFIER ::= { ieee802dot11 2 }
    *************************************************************************** */
    /* --  dot11OperationTable ::= { dot11mac 1 } */
    wlan_mib_dot11_operation_entry_stru wlan_mib_operation;
    /* --  dot11EDCATable ::= { dot11mac 4 } */
    wlan_mib_dot11_edca_entry_stru ast_wlan_mib_edca[WLAN_WME_AC_BUTT];
    /* --  dot11QAPEDCATable ::= { dot11mac 5 } */
    wlan_mib_dot11_qapedca_entry_stru wlan_mib_qap_edac[WLAN_WME_AC_BUTT];

    /* **************************************************************************
        dot11phy OBJECT IDENTIFIER ::= { ieee802dot11 4 }
    *************************************************************************** */
    /* --  dot11PhyHRDSSSTable ::= { dot11phy 12 } */
    wlan_mib_dot11_phy_hrdsss_entry_stru phy_hrdsss;

    /* --  dot11PhyHTTable  ::= { dot11phy 15 } */
    wlan_mib_dot11_phy_ht_entry_stru phy_ht;

    /* --  dot11SupportedMCSTxTable ::= { dot11phy 16 } */
    wlan_mib_dot11_supported_mcs_tx_entry_stru supported_mcstx;

    /* --  dot11SupportedMCSRxTable ::= { dot11phy 17 } */
    wlan_mib_dot11_supported_mcs_rx_entry_stru supported_mcsrx;
} wlan_mib_ieee802dot11_stru;

/* ****************************************************************************
  8 UNION定义
**************************************************************************** */
/* ****************************************************************************
  9 OTHERS定义
**************************************************************************** */
/* ****************************************************************************
  10 函数声明
**************************************************************************** */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of wlan_mib.h */
