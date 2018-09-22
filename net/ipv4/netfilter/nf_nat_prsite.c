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
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#include <linux/netfilter.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <linux/netfilter/nf_conntrack_prsite.h>
#include <net/checksum.h>
#include <net/tcp.h>
#include <net/udp.h>




MODULE_LICENSE("GPL");
MODULE_AUTHOR("China c60023298 <c60023298@huawei.com>");
MODULE_DESCRIPTION("http NAT helper");
MODULE_ALIAS("nf_nat_http");

struct list_head g_urlport;

int g_port_hashkey[AFFINED_ADDR_BUF_MAX];
EXPORT_SYMBOL(g_port_hashkey);

static struct http_response g_response[] =
{
    {HTTP_RSPNS_OK,               200, "OK"               },
    {HTTP_RSPNS_REDIR_PMNT,       301, "Moved Permanently"},
    {HTTP_RSPNS_REDIR_TEMP,       302, "Redirection"      },

    {HTTP_RSPNS_REDIR_UNMODIFIED, 304, "Not Modified"     },
    {HTTP_RSPNS_REDIR_FORBIDDEN,  403, "Forbidden"        },
    {HTTP_RSPNS_BUTT,             0, NULL               }
};

#if 0
 #define PRSITE_DEBUGP(format, args...) printk(format, ## args)
#else
 #define PRSITE_DEBUGP(format, args...)
#endif
extern int (*prsite_add_hook)(char __user * optval);
extern int (*prsite_del_hook)(char __user * optval);


/*************** start of 添加静态IP地址，默认Computer。add by c60023298 ***************/

struct prsite_url_info g_stPrsiteUrlInfo = {0};
static int sum = 0;

#if 0
 #define PRSITE_STATIC_DEBUGP(format, args...) do {printk(format, ## args);} while (0)
#else
 #define PRSITE_STATIC_DEBUGP(format, args...)
#endif
static spinlock_t requestlock;

struct http_response* http_get_response_define(unsigned int index)
{
    if (index >= HTTP_RSPNS_BUTT)
    {
        return NULL;
    }

    return &g_response[index];
}


void http_put_request_info()
{
    struct list_head *pos, *q;
    struct http_request_info *tmp;
    spin_lock_bh(&requestlock);
    list_for_each_safe(pos, q, &g_urlport)
    {   
        tmp = list_entry(pos, struct http_request_info, list);
        if (tmp)
        {
            atomic_dec(&tmp->refcnt);
            list_del(pos);
            kfree(tmp);
        }
    }
    sum = 0;
    spin_unlock_bh(&requestlock);
    return;
}


int http_add_url_port(struct http_request_info *prequestinfo)
{
    if (NULL == prequestinfo)
    {
        return 0;
    }

    if (sum > 30)
    {
        http_put_request_info();
        sum = 0;
    }
    spin_lock_bh(&requestlock);
    list_add(&prequestinfo->list, &g_urlport);
    spin_unlock_bh(&requestlock);
    sum++;
    return 1;
}

char* http_get_request_url
(    short            sport,
     short        dport,
     unsigned int daddr)
{
    struct list_head *pos;
    struct http_request_info *pinfo = NULL;

    list_for_each_entry(pinfo, &g_urlport,list)
    {
        if ((pinfo->sport == dport)
            && (pinfo->dport == sport)
            && (pinfo->daddr == daddr))
        {
            atomic_inc(&pinfo->refcnt);
            return pinfo->url;
        }
    }

    return NULL;
}

int http_isdigit(char *cp)
{
    char *p = NULL;

    if (NULL == cp)
    {
        return 0;
    }

    /*'0' ascii is 0x30, '9' ascii is 0x39*/
    for (p = cp; (0 != *p) && (*p >= 0x30) && (*p <= 0x39); p++)
    {
        ;
    }

    if ((0 == *p) && (p != cp))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int http_atoi(char *cp)
{
    char *tmp = NULL;
    int a, ret;

    if (NULL == cp)
    {
        return 0;
    }

    tmp = cp + strlen(cp);

    for (a = 1, ret = 0; tmp != cp; a *= 10, tmp--)
    {
        /*'0' ascii is 0x30*/
        ret += (*(tmp - 1) - 0x30) * a;
    }

    return ret;
}

void http_tracecheck_timeout(unsigned long ptr)
{
    struct affined_addr *lan_addr = (struct affined_addr*)ptr;

    PRSITE_DEBUGP("http_tracecheck_timeout ======>\n");
    spin_lock_bh(&lan_addr->lock);
    if (lan_addr->idle > HTTP_TRACE_TIMEOUT)
    {
        lan_addr->state = AFFINED_ADDR_UNFORCE;
        del_timer(&lan_addr->timer);
    }
    else
    {
        lan_addr->idle += HTTP_TRACE_CHECK_TIMEOUT;
        mod_timer(&lan_addr->timer, jiffies + HTTP_TRACE_CHECK_TIMEOUT * 60 * HZ);
    }

    spin_unlock_bh(&lan_addr->lock);
}

int http_set_appdata(struct sk_buff *pskb, void *pdata, int len)
{
    struct iphdr *iph = ip_hdr(pskb);
    struct tcphdr *tcph = NULL;
    u_int32_t tcplen;
    u_int32_t datalen;
    struct sk_buff *newskb = NULL;

    PRSITE_DEBUGP("http_set_appdata ======>\n");

    if (NULL == pskb || (NULL == pdata))
    {
        return 0;
    }

    if (skb_shinfo(pskb)->frag_list)
    {
        newskb = skb_copy(pskb, GFP_ATOMIC);
        if (!newskb)
        {
            return 0;
        }

        kfree_skb(pskb);
        pskb = newskb;
    }

    if ((pskb->end - pskb->data) < len)
    {
        struct sk_buff * skb2 = skb_copy_expand(pskb, 0, len, GFP_ATOMIC);
        if (NULL == skb2)
        {
            return 0;
        }

        kfree_skb(pskb);
        pskb = skb2;
    }

    memset(pskb->data + iph->ihl * 4 + sizeof(struct tcphdr),
           0, pskb->len - iph->ihl * 4 - sizeof(struct tcphdr));

    memcpy(pskb->data + iph->ihl * 4 + sizeof(struct tcphdr),
           pdata, len);

    iph  = iph;
    tcph = (void *)iph + iph->ihl * 4;
    tcph->doff = 5;

    pskb->tail = pskb->data + iph->ihl * 4 + sizeof(struct tcphdr) + len;

    iph->tot_len = htons(iph->ihl * 4 + sizeof(struct tcphdr) + len);
    iph->check = 0;
    iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);

    pskb->len = pskb->tail - pskb->data;

    /* fix checksum information */
    tcplen  = pskb->len - iph->ihl * 4;
    datalen = tcplen - tcph->doff * 4;
    pskb->csum = csum_partial((char *)tcph + tcph->doff * 4, datalen, 0);

    tcph->check = 0;
    tcph->check = tcp_v4_check(tcplen, iph->saddr, iph->daddr,
                               csum_partial((char *)tcph, tcph->doff * 4,
                                            pskb->csum));

    return 1;
}

int http_get_httphead(struct sk_buff *skb, char *pdata)
{
    struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = NULL;

    if ((NULL == skb) || (NULL == pdata))
    {
        return 0;
    }

    memset(pdata, 0, HTTP_PROTO_HEAD_BUF_MAX);
    tcph = (void *)iph + iph->ihl * 4;

    PRSITE_DEBUGP("HTTP TcpHdrLength: <%d>\n", tcph->doff * 4);

    /* get tcp header */
    if (0 != skb_copy_bits(skb, iph->ihl * 4 + tcph->doff * 4, pdata, HTTP_PROTO_HEAD_BUF_MAX))
    {
        return 0;
    }

    return 1;
}

int http_get_response_code(struct sk_buff *skb, int *code)
{
    char http_data[HTTP_PROTO_HEAD_BUF_MAX] = "";
    char *code_head = NULL;
    char *code_tail = NULL;
    int ret = 0;

    if ((NULL == skb) || (NULL == code))
    {
        return ret;
    }

    PRSITE_DEBUGP("http_get_response_code ======>\n");
    if (!http_get_httphead(skb, http_data))
    {
        return ret;
    }

    //PRSITE_DEBUGP("HTTP ALG: <%s>\n", http_data);

    if (strncmp(http_data, HTTP_PROTO_NAME, strlen(HTTP_PROTO_NAME)) != 0)
    {
        PRSITE_DEBUGP("HTTP ALG: http_get_response_code===>1");
        goto local_return;
    }

    /* 'HTTP/1.1 200 OK' */
    if (NULL == (code_head = strchr(http_data, ' ')))
    {
        PRSITE_DEBUGP("HTTP ALG: http_get_response_code===>2");
        goto local_return;
    }

    code_head++;
    if (NULL == (code_tail = strchr(code_head, ' ')))
    {
        goto local_return;
    }
    else
    {
        *code_tail = 0;
    }

    if (http_isdigit(code_head))
    {
        *code = http_atoi(code_head);
        ret = 1;
    }

    PRSITE_DEBUGP("HTTP ALG: http response code <%d>\n", *code);

local_return:
    return ret;
}

int http_changeto_newurl(struct sk_buff *pskb, char *url)
{
    char http_data[HTTP_PROTO_HEAD_BUF_MAX + 1] = "";
    char *httpver_head = NULL;
    char *httpver_tail = NULL;
    char http_newdata[HTTP_RESPONSE_BUF_MAX + 1] = "";
    int ret = 0;

    PRSITE_DEBUGP("http_changeto_newurl ======>\n");
    if (NULL == pskb || (NULL == url))
    {
        return ret;
    }

    memset(http_data, 0, sizeof(http_data));
    memset(http_newdata, 0, sizeof(http_newdata));

    if (!http_get_httphead(pskb, http_data))
    {
        return ret;
    }

   // PRSITE_DEBUGP("HTTP ALG: http head<%s>\n", http_data);

    /*
    HTTP/1.1 302 Redirection\r\n
    Content-length: 0\r\n
    Location: http://www.huawei.com/index.html\r\n
    Connection: Close\r\n
     \r\n
     */

    if (NULL == (httpver_head = strstr(http_data, HTTP_PROTO_NAME))
        || NULL == (httpver_tail = strchr(httpver_head, ' ')))
    {
        goto local_return;
    }

    *httpver_tail = 0;
    sprintf(http_newdata, "%s %d %s\r\nContent-length: 0\r\nLocation: http://%s?updataredirect=%s\r\nConnection: Close\r\n\r\n",
            httpver_head,
            g_response[HTTP_RSPNS_REDIR_TEMP].code,
            g_response[HTTP_RSPNS_REDIR_TEMP].desc,
            g_stPrsiteUrlInfo.ac_stb_url,url);

    PRSITE_DEBUGP("HTTP ALG: new http response \n%s\n", http_newdata);

    ret = http_set_appdata(pskb, http_newdata, strlen(http_newdata));

local_return:
    return ret;
}

int http_do_redirect(struct sk_buff *pskb)
{
    int ret = 0;
    //struct affined_addr *lan_addr = NULL;
    struct iphdr *iph = ip_hdr(pskb);
    struct tcphdr *tcph = NULL;
    tcph = (void *)iph + iph->ihl * 4;
    char *purl = NULL;

    PRSITE_STATIC_DEBUGP("http_do_redirect ======>1\n");

    PRSITE_STATIC_DEBUGP("http_do_redirect ======>dest:<%u.%u.%u.%u>\n", NIPQUAD(iph->daddr));

    /*添加静态IP地址，默认设备类型为Computer,但是不能为LAN维护地址*/
    if ((iph->saddr == g_stPrsiteUrlInfo.ul_lan_addr)
        || (iph->daddr == g_stPrsiteUrlInfo.ul_lan_addr))
    {
        PRSITE_STATIC_DEBUGP("prsite is lan addr\n g_stPrsiteUrlInfo.ul_lan_addr is %u,iph->saddr is %u,iph->daddr is %u\n"
            ,g_stPrsiteUrlInfo.ul_lan_addr,iph->saddr,iph->daddr);
        return ret;
    }

    if ((iph->saddr & g_stPrsiteUrlInfo.ul_lan_mask)
        == (iph->daddr & g_stPrsiteUrlInfo.ul_lan_mask))
    {
        PRSITE_STATIC_DEBUGP("prsite ipaddr is local address(lan)\n iph->saddr is %u,iph->daddr is %u g_stPrsiteUrlInfo.ul_lan_mask is %u",
            iph->saddr ,iph->daddr ,g_stPrsiteUrlInfo.ul_lan_mask);
        return ret;
    }

    if (0 == strlen(g_stPrsiteUrlInfo.ac_stb_url))
    {
        PRSITE_STATIC_DEBUGP("prsite url is empty\n");
        return ret;
    }

    purl = http_get_request_url(tcph->source,tcph->dest,
                                iph->saddr);
    if (NULL == purl)
    {
        PRSITE_STATIC_DEBUGP("5 member is not exist\n");
        PRSITE_DEBUGP("tcph->source is %u ,tcph->dest is %u,iph->saddr is %u,iph->daddr is %u \n",
            ntohs(tcph->source),ntohs(tcph->dest),iph->saddr,iph->daddr);
        return ret;
    }
    if ((iph->daddr & g_stPrsiteUrlInfo.ul_lan_mask)
        == (g_stPrsiteUrlInfo.ul_lan_addr & g_stPrsiteUrlInfo.ul_lan_mask))
    {
        http_changeto_newurl(pskb, purl);
        ret = 1;
        return ret;
    }
    else
    {
        PRSITE_STATIC_DEBUGP("\n\t prsite ip <%u.%u.%u.%u>", NIPQUAD(iph->daddr));
        return ret;
    }
local_return:
    PRSITE_STATIC_DEBUGP("Go to Local Return\n");
    return ret;
}

/*增加强制门户节点*/
int prsite_add(char __user *optval)
{
    int ret = 0;
    struct affined_bind bind   = {0};
    struct power_site   *psite = NULL;

    PRSITE_DEBUGP("prsite_add ======>\n");

    if (copy_from_user(&bind, optval, sizeof(struct affined_bind)))
    {
        ret = 1;
        return ret;
    }

    if (strlen(bind.url) == 0)
    {
        g_stPrsiteUrlInfo.lEnable = 0;
        return ret;
    }
    else
    {
        g_stPrsiteUrlInfo.ul_lan_addr = ntohl(bind.addr);
        g_stPrsiteUrlInfo.ul_lan_mask = ntohl(bind.mask);
        strncpy(g_stPrsiteUrlInfo.ac_stb_url, bind.url, HTTP_URL_MAX);
        g_stPrsiteUrlInfo.lEnable = 1;
    }
    PRSITE_DEBUGP("++++++ ul_lan_addr %u ul_lan_mask %u ac_stb_url %s \n",g_stPrsiteUrlInfo.ul_lan_addr,g_stPrsiteUrlInfo.ul_lan_mask,g_stPrsiteUrlInfo.ac_stb_url);
    INIT_LIST_HEAD(&g_urlport);/*初始化链表头*/

    PRSITE_STATIC_DEBUGP("\n\t add ipaddr <%u.%u.%u.%u>", NIPQUAD(g_stPrsiteUrlInfo.ul_lan_addr));
    PRSITE_STATIC_DEBUGP("\n\t add mask <%u.%u.%u.%u>", NIPQUAD(g_stPrsiteUrlInfo.ul_lan_mask));
    return ret;
}

/*删除强制门户节点*/
int prsite_del(char __user *optval)
{
    PRSITE_DEBUGP("prsite_del ======>\n");
    memset(&g_stPrsiteUrlInfo,0,sizeof(g_stPrsiteUrlInfo));
    return 0;
}


int http_get_request_info(struct sk_buff *skb)
{
    char uri[224]  = {0};
    char host[64] = {0};
    int ret = 0;
    char *p = NULL;
    struct iphdr *iph = NULL;
    struct tcphdr *tcph = NULL;
    unsigned char *data = NULL;
    int i = 0;
    struct http_request_info *requestinfo = NULL;
    if (NULL == skb)
    {
        return ret;
    }
    iph = ip_hdr(skb);
    
    if (NULL == iph)
    {
        return ret;
    }
    if ((iph->saddr == g_stPrsiteUrlInfo.ul_lan_addr)
        || (iph->daddr == g_stPrsiteUrlInfo.ul_lan_addr))
    {
        PRSITE_STATIC_DEBUGP("prsite is lan addr\n");
        return ret;
    }

    if ((iph->saddr & g_stPrsiteUrlInfo.ul_lan_mask)
        == (iph->daddr & g_stPrsiteUrlInfo.ul_lan_mask))
    {
        PRSITE_STATIC_DEBUGP("prsite ipaddr is local address(lan)\n");
        return ret;
    }

    if (0 == strlen(g_stPrsiteUrlInfo.ac_stb_url))
    {
        PRSITE_STATIC_DEBUGP("prsite url is empty\n");
        return ret;
    }
    
    tcph = (void *)iph + iph->ihl * 4;
    
    if (NULL == tcph)
    {
        return ret;
    }

    if (http_get_request_url(tcph->source,tcph->dest,iph->daddr))
    {
        PRSITE_STATIC_DEBUGP("5 member is  exist\n");
        return ret;
    }
    
    data = (void *)tcph + tcph->doff * 4;

    if (NULL == data)
    {
        return ret;
    }

    PRSITE_DEBUGP("http_get_request_info ======>\n");


    if (strncmp(data, HTTP_PROTO_GET, strlen(HTTP_PROTO_GET) != 0))
    {
        return ret;
    }

    p = data + 4;
    while (*p != ' ')
    {
        if (i >= 224)
        {
            break;
        }

        uri[i] = *p;
        i++;
        p++;
    }

    PRSITE_DEBUGP("uri %s\n", uri);
    p = strstr(p, "Host: ");
    if (NULL == p)
    {
        return ret;
    }

    p = p + strlen("Host: ");
    i = 0;
    while ((*p != '\r') && (*p != '\n'))
    {
        if (i >= 64)
        {
            break;
        }

        host[i] = *p;
        i++;
        p++;
    }
    PRSITE_DEBUGP("host %s\n", host);
    requestinfo = kmalloc(sizeof(struct http_request_info), GFP_KERNEL);   
    if (NULL == requestinfo)
    {
        PRSITE_DEBUGP("kmalloc is fail \n");
        return  ret;
    }
    if (0 == strcmp("/",uri))
    {
        snprintf(requestinfo->url, HTTP_URL_MAX,"%s", host);
    }
    else
    {
        snprintf(requestinfo->url, HTTP_URL_MAX,"%s%s", host, uri);
    }
    PRSITE_DEBUGP("%s \n", requestinfo->url);
    requestinfo->sport = tcph->source;
   // requestinfo->saddr = iph->saddr;
    requestinfo->dport = tcph->dest;
    requestinfo->daddr = iph->daddr;
    PRSITE_DEBUGP("requestinfo->sport is %u,requestinfo->dport is %u,requestinfo->daddr is %u \n",
        ntohs(requestinfo->sport),ntohs(requestinfo->dport),requestinfo->daddr);
    http_add_url_port(requestinfo);

    return ret;
}

/*进行强制门户处理*/
void http_help(struct nf_conn *ct, int dir, unsigned int hooknum, struct sk_buff *pskb)
{
    int http_code = 0;
    struct http_response *prespok    = NULL;
    struct http_response *prespmodi  = NULL;
    struct http_response *pforbidden = NULL;
    PRSITE_DEBUGP("http_help ======>1\n");

    if (!g_stPrsiteUrlInfo.lEnable)
    {
        PRSITE_STATIC_DEBUGP("prsite is not enable\n");
        return;
    }
    if (IP_CT_DIR_ORIGINAL == dir)
    {
        PRSITE_DEBUGP("http_help ======>2\n");
        http_get_request_info(pskb);
    }
    else if (IP_CT_DIR_REPLY == dir)
    {
        PRSITE_DEBUGP("http_help ======>3\n");
        if (http_get_response_code(pskb, &http_code))
        {
            PRSITE_DEBUGP("http_help ======>4\n");
            prespok    = http_get_response_define(HTTP_RSPNS_OK);
            prespmodi  = http_get_response_define(HTTP_RSPNS_REDIR_UNMODIFIED);
            pforbidden = http_get_response_define(HTTP_RSPNS_REDIR_FORBIDDEN);
            PRSITE_DEBUGP("http_help ======>5\n");
            if (((NULL != prespok) && (http_code == prespok->code))
                || ((NULL != prespmodi) && (http_code == prespmodi->code))
                || ((NULL != pforbidden) && (http_code == pforbidden->code)))
            {
                PRSITE_DEBUGP("http_help ======>6\n");
                if (1 == http_do_redirect(pskb))
                {
                    g_stPrsiteUrlInfo.lEnable = 0;
                    PRSITE_DEBUGP("do_redirect success \n");
                    http_put_request_info();
                }
            }
        }
    }
}

static void __exit nf_nat_http_fini(void)
{
    rcu_assign_pointer(prsite_add_hook, NULL);
    rcu_assign_pointer(prsite_del_hook, NULL);
    rcu_assign_pointer(http_nat_help_hook, NULL);

    synchronize_rcu();
}

static int __init nf_nat_http_init(void)
{
    PRSITE_DEBUGP("nf_nat_http_init <*********** start ************>\n");
    BUG_ON(rcu_dereference(prsite_add_hook) != NULL);
    BUG_ON(rcu_dereference(prsite_del_hook) != NULL);
    BUG_ON(rcu_dereference(http_nat_help_hook) != NULL);

    rcu_assign_pointer(prsite_add_hook, prsite_add);
    rcu_assign_pointer(prsite_del_hook, prsite_del);
    rcu_assign_pointer(http_nat_help_hook, http_help);
    return 0;
}

module_init(nf_nat_http_init);
module_exit(nf_nat_http_fini);
