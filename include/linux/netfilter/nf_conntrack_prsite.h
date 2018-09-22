/* (C) 2001-2002 Magnus Boden <mb@ozaba.mine.nu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Version: 0.0.7
 *
 * Thu 21 Mar 2002 Harald Welte <laforge@gnumonks.org>
 * 	- Port to newnat API
 *
 * This module currently supports DNAT:
 * iptables -t nat -A PREROUTING -d x.x.x.x -j DNAT --to-dest x.x.x.y
 *
 * and SNAT:
 * iptables -t nat -A POSTROUTING { -j MASQUERADE , -j SNAT --to-source x.x.x.x }
 *
 * It has not been tested with
 * -j SNAT --to-source x.x.x.x-x.x.x.y since I only have one external ip
 * If you do test this please let me know if it works or not.
 *
 */
#ifndef _NF_CONNTRACK_PRSITE_H_
#define  _NF_CONNTRACK_PRSITE_H_

#include <linux/timer.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <asm/atomic.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <net/netfilter/nf_conntrack.h>

#define HTTP_PORT 80

#define HTTP_URL_MAX (256 + 32)
#define HTTP_TRACE_CHECK_TIMEOUT 5 //minutes
#define HTTP_TRACE_TIMEOUT 30 //minutes
#define HTTP_PROTO_HEAD_BUF_MAX 16
#define HTTP_RESPONSE_BUF_MAX (HTTP_URL_MAX + 256)
#define AFFINED_ADDR_BUF_MAX 16

#define AFFINED_ADDR_STATIC_BASE 8

#define HTTP_PROTO_NAME "HTTP"
#define HTTP_PROTO_GET  "GET"



#define AFFINED_ADDR_HASH_KEY(addr) (((addr) ^ ((addr) >> 8) ^ ((addr) >> 16) ^ ((addr) >> 24)) & 0xF)

enum
{
    AFFINED_ADDR_INIT,
    AFFINED_ADDR_UNFORCE,
    AFFINED_ADDR_FORCING,
    AFFINED_ADDR_FORCED
};

enum
{
    HTTP_RSPNS_OK = 0,
    HTTP_RSPNS_REDIR_PMNT,
    HTTP_RSPNS_REDIR_TEMP,

    HTTP_RSPNS_REDIR_UNMODIFIED,
    HTTP_RSPNS_REDIR_FORBIDDEN,
    /* new add here */
    HTTP_RSPNS_BUTT
};

/* start prsite static mac,c60023298,2007-12-24 */
struct prsite_url_info
{
    int          lEnable;
    unsigned int ul_lan_addr;
    unsigned int ul_lan_mask;
    char         ac_stb_url[HTTP_URL_MAX];
    char         ac_computer_url[HTTP_URL_MAX];
    char         ac_wifi_url[HTTP_URL_MAX];
};
/* end prsite static mac,c60023298,2007-12-24 */

//for ioctl
struct affined_bind
{
    unsigned int addr;
    unsigned int mask;
    char         url[HTTP_URL_MAX];
};

struct power_site;
struct affined_addr
{
    struct list_head list;
    spinlock_t       lock;
    atomic_t         refcnt;
    atomic_t         dirty;
    int              state;
    unsigned int     addr;
    unsigned int     mask;

    unsigned long port_no;
    unsigned long        idle;
    struct timer_list    timer;
    struct power_site   *force_to;
};

struct power_site
{
    struct list_head list;
    spinlock_t       lock;
    atomic_t         refcnt;
    atomic_t         dirty;
    char             url[HTTP_URL_MAX];
    unsigned int     addr;
};

struct http_response
{
    int   id;
    int   code;
    char *desc;
};

struct http_request_info
{
    struct list_head list;
    char  url[HTTP_URL_MAX];
    short sport;
    short dport;
    //unsigned int saddr;
    unsigned int daddr;
    atomic_t     refcnt;
};


extern  void (*http_nat_help_hook)(struct nf_conn *ct, int dir, unsigned int hooknum, struct sk_buff *pskb);


#endif
