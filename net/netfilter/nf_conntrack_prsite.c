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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/ctype.h>
#include <linux/inet.h>
#include <net/checksum.h>
#include <net/tcp.h>
#include <net/udp.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <linux/netfilter/nf_conntrack_prsite.h>

MODULE_AUTHOR("China Heary <c60023298@huawei.com>");
MODULE_DESCRIPTION("http connection tracking helper");
MODULE_LICENSE("GPL");
MODULE_ALIAS("nf_conntrack_http");
MODULE_ALIAS_NFCT_HELPER("http");


#define MAX_PORTS 8
static u_int16_t ports[MAX_PORTS];
static unsigned int ports_c = 0;
#define HTTP_MAX_EXPECTED 20
#define HTTP_TIME_OUT 180 * 60   /* 180 Minutes */

module_param_array(ports, ushort, &ports_c, 0400);

#define HTTP_PORT 80
#define HTTP_PORT2 8080

MODULE_PARM_DESC(ports, "port numbers of http servers");

#if 0
 #define DEBUGP(format, args...) printk(format, ## args)
#else
 #define DEBUGP(format, args...)
#endif

static struct nf_conntrack_helper http[MAX_PORTS] __read_mostly;
static char http_names[MAX_PORTS][10] __read_mostly;

static const struct nf_conntrack_expect_policy http_exp_policy =
{
    .max_expected =      1,
    .timeout      = 5 * 60,
};


void (*http_nat_help_hook)(struct nf_conn *ct, int dir, unsigned int hooknum, struct sk_buff *pskb) ;

EXPORT_SYMBOL_GPL(http_nat_help_hook);

static int nf_conntrack_http_help(struct sk_buff *      pskb,
                                  unsigned int           hooknum,
                                  struct nf_conn *       ct,
                                  enum ip_conntrack_info ctinfo)
{
    int dir = CTINFO2DIR(ctinfo);

    DEBUGP("nf_conntrack_http_help===============>:%d,hooknum:%d\n", dir, hooknum);
    if (http_nat_help_hook)
    {
        DEBUGP("http_nat_help_hook dir is %d  \n",dir);
        (*http_nat_help_hook)(ct, dir, hooknum, pskb);
    }

    return NF_ACCEPT;
}



static void nf_conntrack_http_fini(void)
{
    int i;

    for (i = 0; i < ports_c; i++)
    {
        DEBUGP("unregistering helper for port %d\n", ports[i]);
        nf_conntrack_helper_unregister(&http[i]);
    }
}

static int __init nf_conntrack_http_init(void)
{
    int i, ret = 0;
    char *tmpname;

    if (!ports[0])
    {
        ports[ports_c++] = HTTP_PORT;
    }

    if (!ports[1])
    {
        ports[ports_c++] = HTTP_PORT2;
    }                                 //����֧��8080�˿�

    for (i = 0; (i < 1) && ports[i]; i++)
    {
        memset(&http[i], 0, sizeof(struct nf_conntrack_helper));
        http[i].tuple.src.l3num = AF_INET;
        http[i].tuple.src.u.tcp.port = htons(80);
        http[i].tuple.dst.protonum = IPPROTO_TCP;
        http[i].expect_policy = &http_exp_policy;
        http[i].me   = THIS_MODULE;
        http[i].help = nf_conntrack_http_help;

        tmpname = &http_names[i][0];
        if (ports[i] == HTTP_PORT)
        {
            sprintf(tmpname, "http");
        }
        else
        {
            sprintf(tmpname, "http-%d", i);
        }

        http[i].name = tmpname;

        DEBUGP("nf_conntrack_http: registering for port %d: name %s\n",
               ports[i], http[i].name);
        ret = nf_conntrack_helper_register(&http[i]);

        if (ret)
        {
            printk("nf_conntrack_http: unable to register for port %d\n",
                   ports[i]);
            nf_conntrack_http_fini();
            return ret;
        }

        ports_c++;
    }

    return ret;
}

module_init(nf_conntrack_http_init);
module_exit(nf_conntrack_http_fini);
