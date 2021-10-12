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

#ifndef __OAL_LITEOS_NETBUFF_H__
#define __OAL_LITEOS_NETBUFF_H__

#ifndef __KERNEL__
#include <endian.h>
#endif
#include "hdf_netbuf.h"
#include "oal_err_wifi.h"
#include "oal_util.h"

#define oal_init_netbuf_stru()
#define oal_malloc_netbuf_stru(size)
#define oal_free_netbuf_stru(p)

#define oal_host2net_short(_x)          htons(_x)
#define oal_net2host_short(_x)          ntohs(_x)
#define oal_host2net_long(_x)           htonl(_x)
#define oal_net2host_long(_x)           ntohl(_x)
#define oal_high_half_byte(a)           (((a) & 0xF0) >> 4)
#define oal_low_half_byte(a)            ((a) & 0x0F)

typedef uint32_t oal_gfp_enum_uint8;
typedef NetBuf oal_netbuf_stru;
typedef NetBufQueue oal_netbuf_head_stru;

typedef struct oal_netbuf_stru_tag {
    struct oal_netbuf_stru_tag *next;
    uint8_t                     mem_state_flag;         /* 内存块状态 */
    uint8_t                     subpool_id       : 4;   /* 记录所属子池id */
    uint8_t                     is_high_priority : 1;
    uint8_t                     bit_resv         : 3;
    uint16_t                    us_index;
} oal_dev_netbuf_stru;

typedef struct oal_ip_header {
#if (_PRE_LITTLE_CPU_ENDIAN == _PRE_CPU_ENDIAN)            /* LITTLE_ENDIAN */
    uint8_t     us_ihl: 4,
             version_ihl: 4;
#else
    uint8_t     version_ihl: 4,
             us_ihl: 4;
#endif
    uint8_t    tos;
    uint16_t   us_tot_len;
    uint16_t   us_id;
    uint16_t   us_frag_off;
    uint8_t    ttl;
    uint8_t    protocol;
    uint16_t   us_check;
    uint32_t   saddr;
    uint32_t   daddr;
    /* The options start here */
} oal_ip_header_stru;

typedef struct oal_tcp_header {
    uint16_t  us_sport;
    uint16_t  us_dport;
    uint32_t  seqnum;
    uint32_t  acknum;
    uint8_t   offset;
    uint8_t   flags;
    uint16_t  us_window;
    uint16_t  us_check;
    uint16_t  us_urgent;
} oal_tcp_header_stru;

#ifdef _PRE_WLAN_FEATURE_FLOWCTL
hi_void oal_netbuf_get_txtid(oal_netbuf_stru *netbuf, uint8_t *puc_tos);
#endif
hi_u8 oal_netbuf_is_tcp_ack(oal_ip_header_stru *ip_hdr);
hi_u8 oal_netbuf_is_icmp(const oal_ip_header_stru *ip_hdr);


#define oal_netbuf_list_num(q)              NetBufQueueSize(q)
#define oal_netbuf_list_len(q)              NetBufQueueSize(q)
#define oal_netbuf_list_empty(q)            NetBufQueueIsEmpty(q)
#define oal_free_netbuf_list(q)             NetBufQueueClear(q)

#define oal_netbuf_queue_num(q)             NetBufQueueSize(q)
#define OAL_NETBUF_QUEUE_TAIL(q, nb)        NetBufQueueEnqueue(q, nb)
#define OAL_NETBUF_QUEUE_HEAD_INIT(q)       NetBufQueueInit(q)
#define OAL_NETBUF_DEQUEUE(q)               NetBufQueueDequeue(q)
#define oal_netbuf_head_init(q)             NetBufQueueInit(q)
#define oal_netbuf_peek(q)                  NetBufQueueAtHead(q)
#define oal_netbuf_tail(q)                  NetBufQueueAtTail(q)
#define oal_netbuf_get_buf_num(q)           NetBufQueueSize(q)

#define oal_dev_alloc_skb(size)             NetBufAlloc(size)
#define oal_netbuf_free(nb)                 NetBufFree(nb)
#define oal_netbuf_expand_head(nb, head, tail, mask)   NetBufResizeRoom(nb, head, tail)
#define oal_netbuf_realloc_tailroom(nb, tail)   (NetBufResizeRoom(nb, 0, tail) == 0 ? nb : NULL)
#define oal_netbuf_concat(nb, cnb)           NetBufConcat(nb, cnb)

#define oal_netbuf_list_head_init(q)        NetBufQueueInit(q)
#define oal_netbuf_add_to_list_tail(nb, q)  NetBufQueueEnqueue(q, nb)
#define oal_netbuf_addlist(q, nb)           NetBufQueueEnqueueHead(q, nb)
#define oal_netbuf_delist(q)                NetBufQueueDequeue(q)
#define oal_netbuf_delist_tail(q)           NetBufQueueDequeueTail(q)
#define oal_netbuf_list_tail(q, nb)         NetBufQueueEnqueue(q, nb)
#define oal_netbuf_list_purge(q)            NetBufQueueClear(q)


#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

#ifdef _PRE_LWIP_ZERO_COPY
#define PBUF_ZERO_COPY_RESERVE 36
oal_netbuf_stru *oal_pbuf_netbuf_alloc(uint32_t len);
#endif

#define ETH_P_CONTROL 0x0016 /* Card specific control frames */

#define L1_CACHE_BYTES    (1 << 5)
#define SKB_DATA_ALIGN(X) (((X) + (L1_CACHE_BYTES - 1)) & ~(L1_CACHE_BYTES - 1))

#ifndef NET_SKB_PAD
#define NET_SKB_PAD 64 // max(32, L1_CACHE_BYTES)
#endif

#define oal_netbuf_next(nb) (nb->dlist.next == NULL ? NULL : CONTAINER_OF(nb->dlist.next, NetBuf, dlist))
#define oal_netbuf_prev(nb) (nb->dlist.prev == NULL ? NULL : CONTAINER_OF(nb->dlist.prev, NetBuf, dlist))
static inline void set_oal_netbuf_next(NetBuf *nb, NetBuf *new)
{
    (nb)->dlist.next = (new == NULL ? NULL : &new->dlist);
}

static inline void set_oal_netbuf_prev(NetBuf *nb, NetBuf *new)
{
    (nb)->dlist.prev = (new == NULL ? NULL : &new->dlist);
}

#define oal_netbuf_head_next(q)             ((q)->dlist.next)
#define oal_netbuf_head_prev(q)             ((q)->dlist.prev)

#define oal_netbuf_set_protocol(nb, p)      // ((nb)->protocol)
#define oal_netbuf_priority(nb)             0 // ((nb)->rsv[oal_netbuf_cb_size()-1])
#define oal_netbuf_data(nb)                 NetBufGetAddress(nb, E_DATA_BUF)
#define oal_netbuf_header(nb)               NetBufGetAddress(nb, E_DATA_BUF)
#define oal_netbuf_payload(nb)              NetBufGetAddress(nb, E_DATA_BUF)
#define oal_netbuf_cb(nb)                   ((nb)->rsv)
#define oal_netbuf_cb_size()                (sizeof(((NetBuf*)0)->rsv))
#define oal_netbuf_len(nb)                  NetBufGetDataLen(nb)
#define OAL_NETBUF_TAIL(nb)                 NetBufGetAddress(nb, E_TAIL_BUF)

#define oal_netbuf_put(nb, len)             NetBufPush(nb, E_DATA_BUF, len)
#define oal_netbuf_push(nb, len)            NetBufPop(nb, E_HEAD_BUF, len)
#define oal_netbuf_pull(nb, len)            NetBufPush(nb, E_HEAD_BUF, len)
#define oal_netbuf_reserve(nb, l)           do { \
                                                NetBufPop(nb, E_TAIL_BUF, l); \
                                                NetBufPop(nb, E_DATA_BUF, l); \
                                            } while(0)
#define oal_netbuf_get(nb)                  (nb)
#define oal_netbuf_unshare(nb)              (nb)
#define oal_netbuf_headroom(nb)             NetBufGetRoom(nb, E_HEAD_BUF)
#define oal_netbuf_tailroom(nb)             NetBufGetRoom(nb, E_TAIL_BUF)

#define oal_netbuf_trim(nb, l)              if (nb->dataLen > l && nb->bufs[E_DATA_BUF].len > l) { \
                                                NetBufPush(nb, E_TAIL_BUF, l); \
                                            }
#define oal_netbuf_set_len(nb,l)            if (nb->dataLen > l) { \
                                                NetBufPush(nb, E_TAIL_BUF, nb->dataLen - l); \
                                            } else { \
                                                NetBufPush(nb, E_DATA_BUF, (l - nb->dataLen)); \
                                            }

#define oal_netbuf_list_next(nb)            oal_netbuf_next(nb)
#define oal_netbuf_init(nb, len)             oal_netbuf_set_len(nb, len)

#define oal_netbuf_splice_init(add, q)      NetBufQueueConcat(q, add)
#define oal_netbuf_copy_queue_mapping(to, from)
#define oal_skb_set_queue_mapping(nb, mapping)   nb->qmap = mapping

#define oal_skb_queue_walk_safe(q, nb, tmp) DLIST_FOR_EACH_ENTRY_SAFE(nb, tmp, &q->dlist, NetBuf, dlist)
#define oal_netbuf_append(q, nb, prev)      do { \
                                                DListInsertHead(&nb->dlist, &prev->dlist); \
                                                (q)->size++; \
                                            } while(0)

typedef struct pbuf                         oal_lwip_buf;

#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

#define ETH_P_CONTROL                           0x0016      /* Card specific control frames */

#define oal_netbuf_next(nb)                     ((nb)->next)
#define oal_netbuf_prev(nb)                     ((nb)->prev)
#define set_oal_netbuf_next(nb, new)            ((nb)->next = new)
#define set_oal_netbuf_prev(nb, new)            ((nb)->prev = new)

#define oal_netbuf_head_next(q)                 ((q)->next)
#define oal_netbuf_head_prev(q)                 ((q)->prev)

#define oal_netbuf_set_protocol(nb, p)          ((nb)->protocol = p)
#define oal_netbuf_priority(nb)                 ((nb)->priority)
#define oal_netbuf_data(nb)                     ((nb)->data)
#define oal_netbuf_header(nb)                   ((nb)->data)
#define oal_netbuf_payload(nb)                  ((nb)->data)
#define oal_netbuf_cb(nb)                       (((nb)->cb))
#define oal_netbuf_cb_size()                    (sizeof(((NetBuf*)0)->cb))
#define oal_netbuf_len(nb)                      ((nb)->len)
#define OAL_NETBUF_TAIL(nb)                     skb_tail_pointer(nb)


#define oal_netbuf_put(nb, len)                 skb_put(nb, len)
#define oal_netbuf_push(nb, len)                skb_push(nb, len)
#define oal_netbuf_pull(nb, len)                skb_pull(nb, len)
#define oal_netbuf_reserve(nb, l)               skb_reserve(nb,l)
#define oal_netbuf_get(nb)                      skb_get(nb)
#define oal_netbuf_unshare(nb, pri)             skb_unshare(nb, pri)
#define oal_netbuf_headroom(nb)                 skb_headroom(nb)
#define oal_netbuf_tailroom(nb)                 skb_tailroom(nb)
#define oal_netbuf_trim(nb, l)                  skb_trim(nb, (nb->len - l))
#define oal_netbuf_set_len(nb, l)               if (nb->len > l) { \
                                                    skb_trim(nb, l); \
                                                } else { \
                                                    skb_put(nb, (l - nb->len)); \
                                                }

#define oal_netbuf_list_next(nb)                oal_netbuf_next(nb)
#define oal_netbuf_init(nb, len)                oal_netbuf_set_len(nb,len) \
                                                nb->protocol = ETH_P_CONTROL;

#define oal_netbuf_splice_init(from, to)        skb_queue_splice_init(from, to)
#define oal_netbuf_copy_queue_mapping(to, from) skb_copy_queue_mapping(to, from)
#define oal_skb_set_queue_mapping(nb, mapping)  skb_set_queue_mapping(nb, mapping)
#define oal_skb_queue_walk_safe(q, nb, tmp)     skb_queue_walk_safe(q, nb, tmp)
#define oal_netbuf_append(q, nb, prev)          __skb_queue_after(q, prev, nb)

#endif

static inline uint32_t oal_netbuf_get_appointed_netbuf(NetBuf *nb, uint8_t num, NetBuf **expect_netbuf)
{
    hi_u8 buf_num;

    if (oal_unlikely((nb == HI_NULL) || (expect_netbuf == HI_NULL))) {
        return HI_ERR_CODE_PTR_NULL;
    }

    *expect_netbuf = HI_NULL;

    for (buf_num = 0; buf_num < num; buf_num++) {
        *expect_netbuf = oal_netbuf_next(nb);

        if (*expect_netbuf == HI_NULL) {
            break;
        }

        nb = *expect_netbuf;
    }

    return HI_SUCCESS;
}

static inline hi_u32 oal_netbuf_copydata(const oal_netbuf_stru *netbuf, hi_u32 offset, hi_void *dst, hi_u32 dst_len,
    hi_u32 len)
{
    if (memcpy_s(dst, dst_len, oal_netbuf_data(netbuf) + offset, len) != EOK) {
        return HI_FAIL;
    }

    return HI_SUCCESS;
}

static inline NetBuf *oal_netbuf_alloc(uint32_t size, uint32_t reserve, uint32_t align)
{
    NetBuf *nb = NULL;

    (void)align;
    nb = NetBufAlloc(size + reserve + NET_SKB_PAD);
    if (nb != NULL) {
        oal_netbuf_reserve(nb, reserve + NET_SKB_PAD);
    }

    return nb;
}

static inline void oal_netbuf_splice_sync(oal_netbuf_head_stru *to, oal_netbuf_head_stru *from)
{
    NetBuf *nb = NULL;

    while (1) {
        nb = NetBufQueueDequeueTail(from);
        if (nb == NULL)
            break;
        NetBufQueueEnqueueHead(to, nb);
    }
}

#endif
