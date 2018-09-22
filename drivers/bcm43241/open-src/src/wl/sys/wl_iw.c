/*
 * Linux Wireless Extensions support
 *
 * Copyright (C) 2012, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: wl_iw.c 343901 2012-07-10 20:13:09Z $

 */

#if defined(USE_IW)
#define LINUX_PORT

#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>

#include <bcmutils.h>
#include <bcmendian.h>
#include <proto/ethernet.h>

#include <linux/if_arp.h>
#include <asm/uaccess.h>


typedef const struct si_pub	si_t;
#include <wlioctl.h>
#include <dhd_dbg.h>
#include <wl_dbg.h>
#include <wl_iw.h>
#include <wldev_common.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
#include <linux/rtnetlink.h>
#endif
#if defined(SOFTAP)
struct net_device *ap_net_dev = NULL;
EXPORT_SYMBOL(ap_net_dev);
tsk_ctl_t ap_eth_ctl;  
#endif 

extern bool wl_iw_conn_status_str(uint32 event_type, uint32 status,
	uint32 reason, char* stringBuf, uint buflen);

uint wl_msg_level = WL_ERROR_VAL;
static char *g_wps_probe_req_ie=NULL;
static int g_wps_probe_req_ie_len;

#define MAX_WLIW_IOCTL_LEN 1024


#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i
#define htodchanspec(i) i
#define dtohchanspec(i) i

extern struct iw_statistics *dhd_get_wireless_stats(struct net_device *dev);
extern int dhd_wait_pend8021x(struct net_device *dev);

#if WIRELESS_EXT < 19
#define IW_IOCTL_IDX(cmd)	((cmd) - SIOCIWFIRST)
#define IW_EVENT_IDX(cmd)	((cmd) - IWEVFIRST)
#endif 


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
#define DAEMONIZE(a) daemonize(a); \
	allow_signal(SIGKILL); \
	allow_signal(SIGTERM);
#else 
#define RAISE_RX_SOFTIRQ() \
	cpu_raise_softirq(smp_processor_id(), NET_RX_SOFTIRQ)
#define DAEMONIZE(a) daemonize(); \
	do { if (a) \
		strncpy(current->comm, a, MIN(sizeof(current->comm), (strlen(a) + 1))); \
	} while (0);
#endif 

#define ISCAN_STATE_IDLE   0
#define ISCAN_STATE_SCANING 1


#define WLC_IW_ISCAN_MAXLEN   2048
typedef struct iscan_buf {
	struct iscan_buf * next;
	char   iscan_buf[WLC_IW_ISCAN_MAXLEN];
} iscan_buf_t;

typedef struct iscan_info {
	struct net_device *dev;
	struct timer_list timer;
	uint32 timer_ms;
	uint32 timer_on;
	int    iscan_state;
	iscan_buf_t * list_hdr;
	iscan_buf_t * list_cur;

	
	long sysioc_pid;
	struct semaphore sysioc_sem;
	struct completion sysioc_exited;
#if 0
    char ioctlbuf[WLC_IOCTL_SMLEN];
#else
    char ioctlbuf[WLC_IOCTL_MEDLEN];
    wl_iscan_params_t *iscan_ex_params_p;
    int iscan_ex_param_size;
    uint iscan_timer_expired;
#endif
} iscan_info_t;
iscan_info_t *g_iscan = NULL;
static void wl_iw_timerfunc(ulong data);
static void wl_iw_set_event_mask(struct net_device *dev);
static int wl_iw_iscan(iscan_info_t *iscan, wlc_ssid_t *ssid, uint16 action);


typedef struct priv_link {
	wl_iw_t *wliw;
} priv_link_t;

#ifdef CSCAN
static int
wl_iw_set_cscan(
    struct net_device *dev,
    struct iw_request_info *info,
    union iwreq_data *wrqu,
    char *extra
);

static int
iwpriv_set_cscan(struct net_device *dev, struct iw_request_info *info,
                 union iwreq_data *wrqu, char *ext);

#endif


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
#define WL_DEV_LINK(dev)       (priv_link_t*)(dev->priv)
#else
#define WL_DEV_LINK(dev)       (priv_link_t*)netdev_priv(dev)
#endif


#define IW_DEV_IF(dev)          ((wl_iw_t*)(WL_DEV_LINK(dev))->wliw)

static void swap_key_from_BE(
	        wl_wsec_key_t *key
)
{
	key->index = htod32(key->index);
	key->len = htod32(key->len);
	key->algo = htod32(key->algo);
	key->flags = htod32(key->flags);
	key->rxiv.hi = htod32(key->rxiv.hi);
	key->rxiv.lo = htod16(key->rxiv.lo);
	key->iv_initialized = htod32(key->iv_initialized);
}

static void swap_key_to_BE(
	        wl_wsec_key_t *key
)
{
	key->index = dtoh32(key->index);
	key->len = dtoh32(key->len);
	key->algo = dtoh32(key->algo);
	key->flags = dtoh32(key->flags);
	key->rxiv.hi = dtoh32(key->rxiv.hi);
	key->rxiv.lo = dtoh16(key->rxiv.lo);
	key->iv_initialized = dtoh32(key->iv_initialized);
}

static int
dev_wlc_ioctl(
	struct net_device *dev,
	int cmd,
	void *arg,
	int len
)
{
	struct ifreq ifr;
	wl_ioctl_t ioc;
	mm_segment_t fs;
	int ret;

	memset(&ioc, 0, sizeof(ioc));
	ioc.cmd = cmd;
	ioc.buf = arg;
	ioc.len = len;

	strcpy(ifr.ifr_name, dev->name);
	ifr.ifr_data = (caddr_t) &ioc;
    if ( cmd == WLC_GET_VAR || WLC_SET_VAR == cmd ) {
        WL_INFORM(("The command is %s,cmd %d\n",(char*)arg,cmd));
    }
#ifndef LINUX_HYBRID
	
	dev_open(dev);
#endif

	fs = get_fs();
	set_fs(get_ds());
#if defined(WL_USE_NETDEV_OPS)
	ret = dev->netdev_ops->ndo_do_ioctl(dev, &ifr, SIOCDEVPRIVATE);
#else
	ret = dev->do_ioctl(dev, &ifr, SIOCDEVPRIVATE);
#endif
	set_fs(fs);

	return ret;
}



static int
dev_wlc_intvar_set(
	struct net_device *dev,
	char *name,
	int val)
{
	char buf[WLC_IOCTL_SMLEN];
	uint len;

	val = htod32(val);
	len = bcm_mkiovar(name, (char *)(&val), sizeof(val), buf, sizeof(buf));
	ASSERT(len);

	return (dev_wlc_ioctl(dev, WLC_SET_VAR, buf, len));
}

static int
dev_iw_iovar_setbuf(
	struct net_device *dev,
	char *iovar,
	void *param,
	int paramlen,
	void *bufptr,
	int buflen)
{
	int iolen;

	iolen = bcm_mkiovar(iovar, param, paramlen, bufptr, buflen);
	ASSERT(iolen);
	BCM_REFERENCE(iolen);

	return (dev_wlc_ioctl(dev, WLC_SET_VAR, bufptr, iolen));
}

static int
dev_iw_iovar_getbuf(
	struct net_device *dev,
	char *iovar,
	void *param,
	int paramlen,
	void *bufptr,
	int buflen)
{
	int iolen;

	iolen = bcm_mkiovar(iovar, param, paramlen, bufptr, buflen);
	ASSERT(iolen);
	BCM_REFERENCE(iolen);

	return (dev_wlc_ioctl(dev, WLC_GET_VAR, bufptr, buflen));
}

#if WIRELESS_EXT > 17
static int
dev_wlc_bufvar_set(
	struct net_device *dev,
	char *name,
	char *buf, int len)
{
	char *ioctlbuf;
	uint buflen;
	int error;

	ioctlbuf = kmalloc(MAX_WLIW_IOCTL_LEN, GFP_KERNEL);
	if (!ioctlbuf)
		return -ENOMEM;

	buflen = bcm_mkiovar(name, buf, len, ioctlbuf, MAX_WLIW_IOCTL_LEN);
	ASSERT(buflen);
	error = dev_wlc_ioctl(dev, WLC_SET_VAR, ioctlbuf, buflen);

	kfree(ioctlbuf);
	return error;
}
#endif 



static int
dev_wlc_bufvar_get(
	struct net_device *dev,
	char *name,
	char *buf, int buflen)
{
	char *ioctlbuf;
	int error;

	uint len;

	ioctlbuf = kmalloc(MAX_WLIW_IOCTL_LEN, GFP_KERNEL);
	if (!ioctlbuf)
		return -ENOMEM;
	len = bcm_mkiovar(name, NULL, 0, ioctlbuf, MAX_WLIW_IOCTL_LEN);
	ASSERT(len);
	BCM_REFERENCE(len);
	error = dev_wlc_ioctl(dev, WLC_GET_VAR, (void *)ioctlbuf, MAX_WLIW_IOCTL_LEN);
	if (!error)
		bcopy(ioctlbuf, buf, buflen);

	kfree(ioctlbuf);
	return (error);
}



static int
dev_wlc_intvar_get(
	struct net_device *dev,
	char *name,
	int *retval)
{
	union {
		char buf[WLC_IOCTL_SMLEN];
		int val;
	} var;
	int error;

	uint len;
	uint data_null;

	len = bcm_mkiovar(name, (char *)(&data_null), 0, (char *)(&var), sizeof(var.buf));
	ASSERT(len);
	error = dev_wlc_ioctl(dev, WLC_GET_VAR, (void *)&var, len);

	*retval = dtoh32(var.val);

	return (error);
}


#if WIRELESS_EXT < 13
struct iw_request_info
{
	__u16		cmd;		
	__u16		flags;		
};

typedef int (*iw_handler)(struct net_device *dev, struct iw_request_info *info,
	void *wrqu, char *extra);
#endif 

#if WIRELESS_EXT > 12
static int
wl_iw_set_leddc(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu,
	char *extra
)
{
	int dc = *(int *)extra;
	int error;

	error = dev_wlc_intvar_set(dev, "leddc", dc);
	return error;
}

static int
wl_iw_set_vlanmode(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu,
	char *extra
)
{
	int mode = *(int *)extra;
	int error;

	mode = htod32(mode);
	error = dev_wlc_intvar_set(dev, "vlan_mode", mode);
	return error;
}

static int
wl_iw_set_pm(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu,
	char *extra
)
{
	int pm = *(int *)extra;
	int error;

	pm = htod32(pm);
	error = dev_wlc_ioctl(dev, WLC_SET_PM, &pm, sizeof(pm));
	return error;
}
#endif 

int
wl_iw_send_priv_event(
	struct net_device *dev,
	char *flag
)
{
	union iwreq_data wrqu;
	char extra[IW_CUSTOM_MAX + 1];
	int cmd;

	cmd = IWEVCUSTOM;
	memset(&wrqu, 0, sizeof(wrqu));
	if (strlen(flag) > sizeof(extra))
		return -1;

	strcpy(extra, flag);
	wrqu.data.length = strlen(extra);
	wireless_send_event(dev, cmd, &wrqu, extra);
	WL_TRACE(("Send IWEVCUSTOM Event as %s\n", extra));

	return 0;
}

static int
wl_iw_config_commit(
	struct net_device *dev,
	struct iw_request_info *info,
	void *zwrq,
	char *extra
)
{
	wlc_ssid_t ssid;
	int error;
	struct sockaddr bssid;

	WL_TRACE(("%s: SIOCSIWCOMMIT\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_SSID, &ssid, sizeof(ssid))))
		return error;

	ssid.SSID_len = dtoh32(ssid.SSID_len);

	if (!ssid.SSID_len)
		return 0;

	bzero(&bssid, sizeof(struct sockaddr));
	if ((error = dev_wlc_ioctl(dev, WLC_REASSOC, &bssid, ETHER_ADDR_LEN))) {
		WL_ERROR(("%s: WLC_REASSOC failed (%d)\n", __FUNCTION__, error));
		return error;
	}

	return 0;
}

static int
wl_iw_get_name(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *cwrq,
	char *extra
)
{
	int phytype, err;
	uint band[3];
	char cap[5];

	WL_TRACE(("%s: SIOCGIWNAME\n", dev->name));

	cap[0] = 0;
	if ((err = dev_wlc_ioctl(dev, WLC_GET_PHYTYPE, &phytype, sizeof(phytype))) < 0)
		goto done;
	if ((err = dev_wlc_ioctl(dev, WLC_GET_BANDLIST, band, sizeof(band))) < 0)
		goto done;

	band[0] = dtoh32(band[0]);
	switch (phytype) {
		case WLC_PHY_TYPE_A:
			strcpy(cap, "a");
			break;
		case WLC_PHY_TYPE_B:
			strcpy(cap, "b");
			break;
		case WLC_PHY_TYPE_LP:
		case WLC_PHY_TYPE_G:
			if (band[0] >= 2)
				strcpy(cap, "abg");
			else
				strcpy(cap, "bg");
			break;
		case WLC_PHY_TYPE_N:
			if (band[0] >= 2)
				strcpy(cap, "abgn");
			else
				strcpy(cap, "bgn");
			break;
	}
done:
	snprintf(cwrq->name, IFNAMSIZ, "IEEE 802.11%s", cap);
	return 0;
}

static int
wl_iw_set_freq(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_freq *fwrq,
	char *extra
)
{
	int error, chan;
	uint sf = 0;

	WL_TRACE(("%s: SIOCSIWFREQ\n", dev->name));

	
	if (fwrq->e == 0 && fwrq->m < MAXCHANNEL) {
		chan = fwrq->m;
	}

	
	else {
		
		if (fwrq->e >= 6) {
			fwrq->e -= 6;
			while (fwrq->e--)
				fwrq->m *= 10;
		} else if (fwrq->e < 6) {
			while (fwrq->e++ < 6)
				fwrq->m /= 10;
		}
	
	if (fwrq->m > 4000 && fwrq->m < 5000)
		sf = WF_CHAN_FACTOR_4_G; 

		chan = wf_mhz2channel(fwrq->m, sf);
	}
	chan = htod32(chan);
	if ((error = dev_wlc_ioctl(dev, WLC_SET_CHANNEL, &chan, sizeof(chan))))
		return error;

	
	return -EINPROGRESS;
}

static int
wl_iw_get_freq(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_freq *fwrq,
	char *extra
)
{
	channel_info_t ci;
	int error;

	WL_TRACE(("%s: SIOCGIWFREQ\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_CHANNEL, &ci, sizeof(ci))))
		return error;

	
	fwrq->m = dtoh32(ci.hw_channel);
	fwrq->e = dtoh32(0);
	return 0;
}

static int
wl_iw_set_mode(
	struct net_device *dev,
	struct iw_request_info *info,
	__u32 *uwrq,
	char *extra
)
{
	int infra = 0, ap = 0, error = 0;
    int apsta = 0;
	WL_TRACE(("%s: SIOCSIWMODE\n", dev->name));

	switch (*uwrq) {
	case IW_MODE_MASTER:
		infra = ap = 1;
		break;
	case IW_MODE_ADHOC:
	case IW_MODE_AUTO:
		break;
	case IW_MODE_INFRA:
		infra = 1;
		break;
	default:
		return -EINVAL;
	}
	infra = htod32(infra);
	ap = htod32(ap);
    /* enable apsta mode */
    dev_wlc_intvar_get(dev, "apsta", &apsta);

    if ( apsta == 1 ) {
        if ((error = dev_wlc_ioctl(dev, WLC_SET_INFRA, &infra, sizeof(infra)))){
            return error;
        }
    } else {
        if ((error = dev_wlc_ioctl(dev, WLC_SET_INFRA, &infra, sizeof(infra))) ||
            (error = dev_wlc_ioctl(dev, WLC_SET_AP, &ap, sizeof(ap))))
            return error;
    }
	return -EINPROGRESS;
}

static int
wl_iw_get_mode(
	struct net_device *dev,
	struct iw_request_info *info,
	__u32 *uwrq,
	char *extra
)
{
	int error, infra = 0, ap = 0;

	WL_TRACE(("%s: SIOCGIWMODE\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_INFRA, &infra, sizeof(infra))) ||
	    (error = dev_wlc_ioctl(dev, WLC_GET_AP, &ap, sizeof(ap))))
		return error;

	infra = dtoh32(infra);
	ap = dtoh32(ap);
    /*
    *uwrq = infra ? ap ? IW_MODE_MASTER : IW_MODE_INFRA : IW_MODE_ADHOC;
    */
    *uwrq = infra ? IW_MODE_INFRA : IW_MODE_ADHOC;
	return 0;
}

static int
wl_iw_get_range(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	struct iw_range *range = (struct iw_range *) extra;
	static int channels[MAXCHANNEL+1];
	wl_uint32_list_t *list = (wl_uint32_list_t *) channels;
	wl_rateset_t rateset;
	int error, i, k;
	uint sf, ch;

	int phytype;
	int bw_cap = 0, sgi_tx = 0, nmode = 0;
	channel_info_t ci;
	uint8 nrate_list2copy = 0;
	uint16 nrate_list[4][8] = { {13, 26, 39, 52, 78, 104, 117, 130},
		{14, 29, 43, 58, 87, 116, 130, 144},
		{27, 54, 81, 108, 162, 216, 243, 270},
		{30, 60, 90, 120, 180, 240, 270, 300}};

	WL_TRACE(("%s: SIOCGIWRANGE\n", dev->name));

	if (!extra)
		return -EINVAL;

	dwrq->length = sizeof(struct iw_range);
	memset(range, 0, sizeof(*range));

	
	range->min_nwid = range->max_nwid = 0;

	
	list->count = htod32(MAXCHANNEL);
	if ((error = dev_wlc_ioctl(dev, WLC_GET_VALID_CHANNELS, channels, sizeof(channels))))
		return error;
	for (i = 0; i < dtoh32(list->count) && i < IW_MAX_FREQUENCIES; i++) {
		range->freq[i].i = dtoh32(list->element[i]);

		ch = dtoh32(list->element[i]);
		if (ch <= CH_MAX_2G_CHANNEL)
			sf = WF_CHAN_FACTOR_2_4_G;
		else
			sf = WF_CHAN_FACTOR_5_G;

		range->freq[i].m = wf_channel2mhz(ch, sf);
		range->freq[i].e = 6;
	}
	range->num_frequency = range->num_channels = i;

	
	range->max_qual.qual = 5;
	
	range->max_qual.level = 0x100 - 200;	
	
	range->max_qual.noise = 0x100 - 200;	
	
	range->sensitivity = 65535;

#if WIRELESS_EXT > 11
	
	range->avg_qual.qual = 3;
	
	range->avg_qual.level = 0x100 + WL_IW_RSSI_GOOD;
	
	range->avg_qual.noise = 0x100 - 75;	
#endif 

	
	if ((error = dev_wlc_ioctl(dev, WLC_GET_CURR_RATESET, &rateset, sizeof(rateset))))
		return error;
	rateset.count = dtoh32(rateset.count);
	range->num_bitrates = rateset.count;
	for (i = 0; i < rateset.count && i < IW_MAX_BITRATES; i++)
		range->bitrate[i] = (rateset.rates[i] & 0x7f) * 500000; 
	dev_wlc_intvar_get(dev, "nmode", &nmode);
	if ((error = dev_wlc_ioctl(dev, WLC_GET_PHYTYPE, &phytype, sizeof(phytype))))
		return error;

	if (nmode == 1 && ((phytype == WLC_PHY_TYPE_SSN) || (phytype == WLC_PHY_TYPE_LCN) ||
		(phytype == WLC_PHY_TYPE_LCN40))) {
		dev_wlc_intvar_get(dev, "mimo_bw_cap", &bw_cap);
		dev_wlc_intvar_get(dev, "sgi_tx", &sgi_tx);
		dev_wlc_ioctl(dev, WLC_GET_CHANNEL, &ci, sizeof(channel_info_t));
		ci.hw_channel = dtoh32(ci.hw_channel);

		if (bw_cap == 0 ||
			(bw_cap == 2 && ci.hw_channel <= 14)) {
			if (sgi_tx == 0)
				nrate_list2copy = 0;
			else
				nrate_list2copy = 1;
		}
		if (bw_cap == 1 ||
			(bw_cap == 2 && ci.hw_channel >= 36)) {
			if (sgi_tx == 0)
				nrate_list2copy = 2;
			else
				nrate_list2copy = 3;
		}
		range->num_bitrates += 8;
		for (k = 0; i < range->num_bitrates; k++, i++) {
			
			range->bitrate[i] = (nrate_list[nrate_list2copy][k]) * 500000;
		}
	}

	
	if ((error = dev_wlc_ioctl(dev, WLC_GET_PHYTYPE, &i, sizeof(i))))
		return error;
	i = dtoh32(i);
	if (i == WLC_PHY_TYPE_A)
		range->throughput = 24000000;	
	else
		range->throughput = 1500000;	

	
	range->min_rts = 0;
	range->max_rts = 2347;
	range->min_frag = 256;
	range->max_frag = 2346;

	range->max_encoding_tokens = DOT11_MAX_DEFAULT_KEYS;
	range->num_encoding_sizes = 4;
	range->encoding_size[0] = WEP1_KEY_SIZE;
	range->encoding_size[1] = WEP128_KEY_SIZE;
#if WIRELESS_EXT > 17
	range->encoding_size[2] = TKIP_KEY_SIZE;
#else
	range->encoding_size[2] = 0;
#endif
	range->encoding_size[3] = AES_KEY_SIZE;

	
	range->min_pmp = 0;
	range->max_pmp = 0;
	range->min_pmt = 0;
	range->max_pmt = 0;
	range->pmp_flags = 0;
	range->pm_capa = 0;

	
	range->num_txpower = 2;
	range->txpower[0] = 1;
	range->txpower[1] = 255;
	range->txpower_capa = IW_TXPOW_MWATT;

#if WIRELESS_EXT > 10
	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 19;

	
	range->retry_capa = IW_RETRY_LIMIT;
	range->retry_flags = IW_RETRY_LIMIT;
	range->r_time_flags = 0;
	
	range->min_retry = 1;
	range->max_retry = 255;
	
	range->min_r_time = 0;
	range->max_r_time = 0;
#endif 

#if WIRELESS_EXT > 17
	range->enc_capa = IW_ENC_CAPA_WPA;
	range->enc_capa |= IW_ENC_CAPA_CIPHER_TKIP;
	range->enc_capa |= IW_ENC_CAPA_CIPHER_CCMP;
	range->enc_capa |= IW_ENC_CAPA_WPA2;
#if (defined(BCMSUP_PSK) && defined(WLFBT))
	
	range->enc_capa |= IW_ENC_CAPA_4WAY_HANDSHAKE;
#endif 

	
	IW_EVENT_CAPA_SET_KERNEL(range->event_capa);
	
	IW_EVENT_CAPA_SET(range->event_capa, SIOCGIWAP);
	IW_EVENT_CAPA_SET(range->event_capa, SIOCGIWSCAN);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVTXDROP);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVMICHAELMICFAILURE);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVASSOCREQIE);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVASSOCRESPIE);
	IW_EVENT_CAPA_SET(range->event_capa, IWEVPMKIDCAND);

#if WIRELESS_EXT >= 22 && defined(IW_SCAN_CAPA_ESSID)
	
	range->scan_capa = IW_SCAN_CAPA_ESSID;
#endif
#endif 

	return 0;
}
int
wl_format_ssid(char* ssid_buf, uint8* ssid, int ssid_len)
{
    int i, c;
    char *p = ssid_buf;

    if (ssid_len > 32) ssid_len = 32;

    for (i = 0; i < ssid_len; i++) {
        c = (int)ssid[i];
        if (c == '\\') {
            *p++ = '\\';
            *p++ = '\\';
        } else if (isprint((uchar)c)) {
            *p++ = (char)c;
        } else {
            p += sprintf(p, "\\x%02X", c);
        }
    }
    *p = '\0';

    return p - ssid_buf;
}


static int
wl_iw_get_rssi(
    struct net_device *dev,
    struct iw_request_info *info,
    union iwreq_data *wrqu,
    char *extra
)
{
    static int rssi = 0;
    static wlc_ssid_t ssid = {0};
    int error = 0;
    char *p = extra;
    static char ssidbuf[SSID_FMT_BUF_LEN];
    scb_val_t scb_val;

    net_os_wake_lock(dev);

    bzero(&scb_val, sizeof(scb_val_t));

    error = dev_wlc_ioctl(dev, WLC_GET_RSSI, &scb_val, sizeof(scb_val_t));
    if (error) {
        WL_ERROR(("%s: Fails %d\n", __FUNCTION__, error));
        net_os_wake_unlock(dev);
        return error;
    }
    rssi = dtoh32(scb_val.val);

    error = dev_wlc_ioctl(dev, WLC_GET_SSID, &ssid, sizeof(ssid));
    if (!error) {
        ssid.SSID_len = dtoh32(ssid.SSID_len);
        wl_format_ssid(ssidbuf, ssid.SSID, dtoh32(ssid.SSID_len));
    }

    p += snprintf(p, MAX_WX_STRING, "%s rssi %d ", ssidbuf, rssi);
    wrqu->data.length = p - extra + 1;

    net_os_wake_unlock(dev);
    return error;
}

static int
wl_iw_del_wps_probe_req_ie(
    struct net_device *dev,
    struct iw_request_info *info,
    union iwreq_data *wrqu,
    char *extra
)
{
    int ret;
    vndr_ie_setbuf_t *ie_delbuf;

    if (g_wps_probe_req_ie) {
        ie_delbuf = (vndr_ie_setbuf_t *)(g_wps_probe_req_ie + strlen("vndr_ie "));
        strncpy(ie_delbuf->cmd, "del", 3);
        ie_delbuf->cmd[3] = '\0';

        ret = dev_wlc_ioctl(dev, WLC_SET_VAR, g_wps_probe_req_ie, g_wps_probe_req_ie_len);
        if (ret) {
            WL_ERROR(("ioctl failed %d \n", ret));
        }

        kfree(g_wps_probe_req_ie);
        g_wps_probe_req_ie = NULL;
        g_wps_probe_req_ie_len = 0;
    }

    return 0;
}

static int
wl_iw_add_wps_probe_req_ie(
    struct net_device *dev,
    struct iw_request_info *info,
    union iwreq_data *wrqu,
    char *extra
)
{
    char *str_ptr = NULL;
    char *bufptr = NULL;
    uint buflen, datalen, iecount, pktflag, iolen, total_len;
    int ret = 0;
    vndr_ie_setbuf_t *ie_setbuf = NULL;

    if (!g_wps_probe_req_ie) {
        ret = -1;
        str_ptr = extra;
        str_ptr += WPS_PROBE_REQ_IE_CMD_LENGTH;
        datalen = wrqu->data.length - WPS_PROBE_REQ_IE_CMD_LENGTH;



        buflen = sizeof(vndr_ie_setbuf_t) + datalen - sizeof(vndr_ie_t);
        ie_setbuf = (vndr_ie_setbuf_t *)kmalloc(buflen, GFP_KERNEL);
        if (!ie_setbuf) {
            WL_ERROR(("memory alloc failure ie_setbuf\n"));
            return ret;
        }

        memset(ie_setbuf, 0x00, buflen);


        strncpy(ie_setbuf->cmd, "add", VNDR_IE_CMD_LEN - 1);
        ie_setbuf->cmd[VNDR_IE_CMD_LEN - 1] = '\0';


        iecount = htod32(1);
        memcpy((void *)&ie_setbuf->vndr_ie_buffer.iecount, &iecount, sizeof(int));


        pktflag = 0x10;
        memcpy((void *)&ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].pktflag,
            &pktflag, sizeof(uint32));

        memcpy((void *)&ie_setbuf->vndr_ie_buffer.vndr_ie_list[0].vndr_ie_data,
            str_ptr, datalen);

        total_len = strlen("vndr_ie ") + buflen;
        bufptr = (char *)kmalloc(total_len, GFP_KERNEL);
        if (!bufptr) {
            WL_ERROR(("memory alloc failure bufptr\n"));
            goto fail;
        }

        iolen = bcm_mkiovar("vndr_ie", (char *)ie_setbuf, buflen, bufptr, total_len);
        if (iolen == 0) {
            WL_ERROR(("Buffer length is illegal\n"));
            goto fail2;
        }

        ret = dev_wlc_ioctl(dev, WLC_SET_VAR, bufptr, iolen);
        if (ret) {
            WL_ERROR(("ioctl failed\n"));
            goto fail2;
        }

        g_wps_probe_req_ie = (char *)kmalloc(iolen, GFP_KERNEL);
        if (!g_wps_probe_req_ie) {
            WL_ERROR(("memory alloc failure g_wps_probe_req_ie\n"));
            goto fail2;
}

        memcpy(g_wps_probe_req_ie, bufptr, iolen);
        g_wps_probe_req_ie_len = iolen;
    }

fail2:
    if (bufptr) {
        kfree(bufptr);
        bufptr = NULL;
    }
fail:
    if (ie_setbuf) {
        kfree(ie_setbuf);
        ie_setbuf = NULL;
    }
    return ret;
}


static int
wl_iw_set_priv(
    struct net_device *dev,
    struct iw_request_info *info,
    struct iw_point *dwrq,
    char *ext
)
{
    int ret = 0;
    char * extra;

    if (!(extra = kmalloc(dwrq->length, GFP_KERNEL)))
        return -ENOMEM;

    if (copy_from_user(extra, dwrq->pointer, dwrq->length)) {
        kfree(extra);
        return -EFAULT;
    }

    WL_TRACE(("%s: SIOCSIWPRIV request %s, info->cmd:%x, info->flags:%d\n dwrq->length:%d \n",
        dev->name, extra, info->cmd, info->flags, dwrq->length));

    net_os_wake_lock(dev);

    if (dwrq->length && extra) {
        if (strnicmp(extra, "RSSI", strlen("RSSI")) == 0)
            ret = wl_iw_get_rssi(dev, info, (union iwreq_data *)dwrq, extra);
#if 0
        else if (strnicmp(extra, "LINKSPEED", strlen("LINKSPEED")) == 0)
            ret = wl_iw_get_link_speed(dev, info, (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, "MACADDR", strlen("MACADDR")) == 0)
            ret = wl_iw_get_macaddr(dev, info, (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, "COUNTRY", strlen("COUNTRY")) == 0)
            ret = wl_iw_set_country(dev, info, (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, BAND_GET_CMD, strlen(BAND_GET_CMD)) == 0)
            ret = wl_iw_get_band(dev, info, (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, BAND_SET_CMD, strlen(BAND_SET_CMD)) == 0)
            ret = wl_iw_set_band(dev, info, (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, DTIM_SKIP_GET_CMD, strlen(DTIM_SKIP_GET_CMD)) == 0)
            ret = wl_iw_get_dtim_skip(dev, info, (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, DTIM_SKIP_SET_CMD, strlen(DTIM_SKIP_SET_CMD)) == 0)
            ret = wl_iw_set_dtim_skip(dev, info, (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, SETSUSPEND_CMD, strlen(SETSUSPEND_CMD)) == 0)
            ret = wl_iw_set_suspend(dev, info, (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, TXPOWER_SET_CMD, strlen(TXPOWER_SET_CMD)) == 0)
            ret = wl_iw_set_txpower(dev, info, (union iwreq_data *)dwrq, extra);
#endif  
        else if (strnicmp(extra, CSCAN_COMMAND, strlen(CSCAN_COMMAND)) == 0)
            ret = wl_iw_set_cscan(dev, info, (union iwreq_data *)dwrq, extra); 
        else if (strnicmp(extra, WPS_ADD_PROBE_REQ_IE_CMD,
            strlen(WPS_ADD_PROBE_REQ_IE_CMD)) == 0)
            ret = wl_iw_add_wps_probe_req_ie(dev, info,
                (union iwreq_data *)dwrq, extra);
        else if (strnicmp(extra, WPS_DEL_PROBE_REQ_IE_CMD,
            strlen(WPS_DEL_PROBE_REQ_IE_CMD)) == 0)
            ret = wl_iw_del_wps_probe_req_ie(dev, info,
                (union iwreq_data *)dwrq, extra);
        else {
            WL_ERROR(("Unknown PRIVATE command %s - ignored\n", extra));
            snprintf(extra, MAX_WX_STRING, "OK");
            dwrq->length = strlen("OK") + 1;
        }
    }

    net_os_wake_unlock(dev);

    if (extra) {
        if (copy_to_user(dwrq->pointer, extra, dwrq->length)) {
            kfree(extra);
            return -EFAULT;
        }

        kfree(extra);
    }

    return ret;
}

static int
rssi_to_qual(int rssi)
{
	if (rssi <= WL_IW_RSSI_NO_SIGNAL)
		return 0;
	else if (rssi <= WL_IW_RSSI_VERY_LOW)
		return 1;
	else if (rssi <= WL_IW_RSSI_LOW)
		return 2;
	else if (rssi <= WL_IW_RSSI_GOOD)
		return 3;
	else if (rssi <= WL_IW_RSSI_VERY_GOOD)
		return 4;
	else
		return 5;
}

static int
wl_iw_set_spy(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_iw_t *iw = IW_DEV_IF(dev);
	struct sockaddr *addr = (struct sockaddr *) extra;
	int i;

	WL_TRACE(("%s: SIOCSIWSPY\n", dev->name));

	if (!extra)
		return -EINVAL;

	iw->spy_num = MIN(ARRAYSIZE(iw->spy_addr), dwrq->length);
	for (i = 0; i < iw->spy_num; i++)
		memcpy(&iw->spy_addr[i], addr[i].sa_data, ETHER_ADDR_LEN);
	memset(iw->spy_qual, 0, sizeof(iw->spy_qual));

	return 0;
}

static int
wl_iw_get_spy(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_iw_t *iw = IW_DEV_IF(dev);
	struct sockaddr *addr = (struct sockaddr *) extra;
	struct iw_quality *qual = (struct iw_quality *) &addr[iw->spy_num];
	int i;

	WL_TRACE(("%s: SIOCGIWSPY\n", dev->name));

	if (!extra)
		return -EINVAL;

	dwrq->length = iw->spy_num;
	for (i = 0; i < iw->spy_num; i++) {
		memcpy(addr[i].sa_data, &iw->spy_addr[i], ETHER_ADDR_LEN);
		addr[i].sa_family = AF_UNIX;
		memcpy(&qual[i], &iw->spy_qual[i], sizeof(struct iw_quality));
		iw->spy_qual[i].updated = 0;
	}

	return 0;
}

static int
wl_iw_set_wap(
	struct net_device *dev,
	struct iw_request_info *info,
	struct sockaddr *awrq,
	char *extra
)
{
	int error = -EINVAL;

	WL_TRACE(("%s: SIOCSIWAP\n", dev->name));

	if (awrq->sa_family != ARPHRD_ETHER) {
		WL_ERROR(("%s: Invalid Header...sa_family\n", __FUNCTION__));
		return -EINVAL;
	}

	
	if (ETHER_ISBCAST(awrq->sa_data) || ETHER_ISNULLADDR(awrq->sa_data)) {
		scb_val_t scbval;
		bzero(&scbval, sizeof(scb_val_t));
		if ((error = dev_wlc_ioctl(dev, WLC_DISASSOC, &scbval, sizeof(scb_val_t)))) {
			WL_ERROR(("%s: WLC_DISASSOC failed (%d).\n", __FUNCTION__, error));
		}
		return 0;
	}
	
	
	if ((error = dev_wlc_ioctl(dev, WLC_REASSOC, awrq->sa_data, ETHER_ADDR_LEN))) {
		WL_ERROR(("%s: WLC_REASSOC failed (%d).\n", __FUNCTION__, error));
		return error;
	}

	return 0;
}

static int
wl_iw_get_wap(
	struct net_device *dev,
	struct iw_request_info *info,
	struct sockaddr *awrq,
	char *extra
)
{
	WL_TRACE(("%s: SIOCGIWAP\n", dev->name));

	awrq->sa_family = ARPHRD_ETHER;
	memset(awrq->sa_data, 0, ETHER_ADDR_LEN);

	
	(void) dev_wlc_ioctl(dev, WLC_GET_BSSID, awrq->sa_data, ETHER_ADDR_LEN);

	return 0;
}

#if WIRELESS_EXT > 17
static int
wl_iw_mlme(
	struct net_device *dev,
	struct iw_request_info *info,
	struct sockaddr *awrq,
	char *extra
)
{
	struct iw_mlme *mlme;
	scb_val_t scbval;
	int error  = -EINVAL;

	WL_TRACE(("%s: SIOCSIWMLME\n", dev->name));

	mlme = (struct iw_mlme *)extra;
	if (mlme == NULL) {
		WL_ERROR(("Invalid ioctl data.\n"));
		return error;
	}

	scbval.val = mlme->reason_code;
	bcopy(&mlme->addr.sa_data, &scbval.ea, ETHER_ADDR_LEN);

	if (mlme->cmd == IW_MLME_DISASSOC) {
		scbval.val = htod32(scbval.val);
		error = dev_wlc_ioctl(dev, WLC_DISASSOC, &scbval, sizeof(scb_val_t));
	}
	else if (mlme->cmd == IW_MLME_DEAUTH) {
		scbval.val = htod32(scbval.val);
		error = dev_wlc_ioctl(dev, WLC_SCB_DEAUTHENTICATE_FOR_REASON, &scbval,
			sizeof(scb_val_t));
	}
	else {
		WL_ERROR(("%s: Invalid ioctl data.\n", __FUNCTION__));
		return error;
	}

	return error;
}
#endif 

static int
wl_iw_get_aplist(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_scan_results_t *list;
	struct sockaddr *addr = (struct sockaddr *) extra;
	struct iw_quality qual[IW_MAX_AP];
	wl_bss_info_t *bi = NULL;
	int error, i;
	uint buflen = dwrq->length;

	WL_TRACE(("%s: SIOCGIWAPLIST\n", dev->name));

	if (!extra)
		return -EINVAL;

	
	list = kmalloc(buflen, GFP_KERNEL);
	if (!list)
		return -ENOMEM;
	memset(list, 0, buflen);
	list->buflen = htod32(buflen);
	if ((error = dev_wlc_ioctl(dev, WLC_SCAN_RESULTS, list, buflen))) {
		WL_ERROR(("%d: Scan results error %d\n", __LINE__, error));
		kfree(list);
		return error;
	}
	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);
	ASSERT(list->version == WL_BSS_INFO_VERSION);

	for (i = 0, dwrq->length = 0; i < list->count && dwrq->length < IW_MAX_AP; i++) {
		bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length)) : list->bss_info;
		ASSERT(((uintptr)bi + dtoh32(bi->length)) <= ((uintptr)list +
			buflen));

		
		if (!(dtoh16(bi->capability) & DOT11_CAP_ESS))
			continue;

		
		memcpy(addr[dwrq->length].sa_data, &bi->BSSID, ETHER_ADDR_LEN);
		addr[dwrq->length].sa_family = ARPHRD_ETHER;
		qual[dwrq->length].qual = rssi_to_qual(dtoh16(bi->RSSI));
		qual[dwrq->length].level = 0x100 + dtoh16(bi->RSSI);
		qual[dwrq->length].noise = 0x100 + bi->phy_noise;

		
#if WIRELESS_EXT > 18
		qual[dwrq->length].updated = IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
#else
		qual[dwrq->length].updated = 7;
#endif 

		dwrq->length++;
	}

	kfree(list);

	if (dwrq->length) {
		memcpy(&addr[dwrq->length], qual, sizeof(struct iw_quality) * dwrq->length);
		
		dwrq->flags = 1;
	}

	return 0;
}

static int
wl_iw_iscan_get_aplist(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_scan_results_t *list;
	iscan_buf_t * buf;
	iscan_info_t *iscan = g_iscan;

	struct sockaddr *addr = (struct sockaddr *) extra;
	struct iw_quality qual[IW_MAX_AP];
	wl_bss_info_t *bi = NULL;
	int i;

	WL_TRACE(("%s: SIOCGIWAPLIST\n", dev->name));

	if (!extra)
		return -EINVAL;

	if ((!iscan) || (iscan->sysioc_pid < 0)) {
		return wl_iw_get_aplist(dev, info, dwrq, extra);
	}

	buf = iscan->list_hdr;
	
	while (buf) {
	    list = &((wl_iscan_results_t*)buf->iscan_buf)->results;
	    ASSERT(list->version == WL_BSS_INFO_VERSION);

	    bi = NULL;
	for (i = 0, dwrq->length = 0; i < list->count && dwrq->length < IW_MAX_AP; i++) {
		bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length)) : list->bss_info;
		ASSERT(((uintptr)bi + dtoh32(bi->length)) <= ((uintptr)list +
			WLC_IW_ISCAN_MAXLEN));

		
		if (!(dtoh16(bi->capability) & DOT11_CAP_ESS))
			continue;

		
		memcpy(addr[dwrq->length].sa_data, &bi->BSSID, ETHER_ADDR_LEN);
		addr[dwrq->length].sa_family = ARPHRD_ETHER;
		qual[dwrq->length].qual = rssi_to_qual(dtoh16(bi->RSSI));
		qual[dwrq->length].level = 0x100 + dtoh16(bi->RSSI);
		qual[dwrq->length].noise = 0x100 + bi->phy_noise;

		
#if WIRELESS_EXT > 18
		qual[dwrq->length].updated = IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
#else
		qual[dwrq->length].updated = 7;
#endif 

		dwrq->length++;
	    }
	    buf = buf->next;
	}
	if (dwrq->length) {
		memcpy(&addr[dwrq->length], qual, sizeof(struct iw_quality) * dwrq->length);
		
		dwrq->flags = 1;
	}

	return 0;
}

#if WIRELESS_EXT > 13
static int
wl_iw_set_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu,
	char *extra
)
{
	wlc_ssid_t ssid;

	WL_TRACE(("%s: SIOCSIWSCAN\n", dev->name));

	
	memset(&ssid, 0, sizeof(ssid));

#if WIRELESS_EXT > 17
	
	if (wrqu->data.length == sizeof(struct iw_scan_req)) {
		if (wrqu->data.flags & IW_SCAN_THIS_ESSID) {
			struct iw_scan_req *req = (struct iw_scan_req *)extra;
			ssid.SSID_len = MIN(sizeof(ssid.SSID), req->essid_len);
			memcpy(ssid.SSID, req->essid, ssid.SSID_len);
			ssid.SSID_len = htod32(ssid.SSID_len);
		}
	}
#endif
	
	(void) dev_wlc_ioctl(dev, WLC_SCAN, &ssid, sizeof(ssid));

	return 0;
}

static int
wl_iw_iscan_set_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	union iwreq_data *wrqu,
	char *extra
)
{
	wlc_ssid_t ssid;
	iscan_info_t *iscan = g_iscan;

	WL_TRACE(("%s: SIOCSIWSCAN\n", dev->name));

	
	if ((!iscan) || (iscan->sysioc_pid < 0)) {
		return wl_iw_set_scan(dev, info, wrqu, extra);
	}
	if (iscan->iscan_state == ISCAN_STATE_SCANING) {
		return 0;
	}

	
	memset(&ssid, 0, sizeof(ssid));

#if WIRELESS_EXT > 17
	
	if (wrqu->data.length == sizeof(struct iw_scan_req)) {
		if (wrqu->data.flags & IW_SCAN_THIS_ESSID) {
			struct iw_scan_req *req = (struct iw_scan_req *)extra;
			ssid.SSID_len = MIN(sizeof(ssid.SSID), req->essid_len);
			memcpy(ssid.SSID, req->essid, ssid.SSID_len);
			ssid.SSID_len = htod32(ssid.SSID_len);
		}
	}
#endif

	iscan->list_cur = iscan->list_hdr;
	iscan->iscan_state = ISCAN_STATE_SCANING;


	wl_iw_set_event_mask(dev);
	wl_iw_iscan(iscan, &ssid, WL_SCAN_ACTION_START);

	iscan->timer.expires = jiffies + iscan->timer_ms*HZ/1000;
	add_timer(&iscan->timer);
	iscan->timer_on = 1;
    iscan->iscan_timer_expired = 1;
	return 0;
}

#if WIRELESS_EXT > 17
static bool
ie_is_wpa_ie(uint8 **wpaie, uint8 **tlvs, int *tlvs_len)
{


	uint8 *ie = *wpaie;

	
	if ((ie[1] >= 6) &&
		!bcmp((const void *)&ie[2], (const void *)(WPA_OUI "\x01"), 4)) {
		return TRUE;
	}

	
	ie += ie[1] + 2;
	
	*tlvs_len -= (int)(ie - *tlvs);
	
	*tlvs = ie;
	return FALSE;
}

static bool
ie_is_wps_ie(uint8 **wpsie, uint8 **tlvs, int *tlvs_len)
{


	uint8 *ie = *wpsie;

	
	if ((ie[1] >= 4) &&
		!bcmp((const void *)&ie[2], (const void *)(WPA_OUI "\x04"), 4)) {
		return TRUE;
	}

	
	ie += ie[1] + 2;
	
	*tlvs_len -= (int)(ie - *tlvs);
	
	*tlvs = ie;
	return FALSE;
}
#endif 


static int
wl_iw_handle_scanresults_ies(char **event_p, char *end,
	struct iw_request_info *info, wl_bss_info_t *bi)
{
#if WIRELESS_EXT > 17
	struct iw_event	iwe;
	char *event;

	event = *event_p;
	if (bi->ie_length) {
		
		bcm_tlv_t *ie;
		uint8 *ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
		int ptr_len = bi->ie_length;

		if ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_RSN_ID))) {
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = ie->len + 2;
			event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)ie);
		}
		ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);

#if defined(WLFBT)
		if ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_MDIE_ID))) {
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = ie->len + 2;
			event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)ie);
		}
		ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
#endif 

		while ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_WPA_ID))) {
			
			if (ie_is_wps_ie(((uint8 **)&ie), &ptr, &ptr_len)) {
				iwe.cmd = IWEVGENIE;
				iwe.u.data.length = ie->len + 2;
				event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)ie);
				break;
			}
		}

		ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
		ptr_len = bi->ie_length;
		while ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_WPA_ID))) {
			if (ie_is_wpa_ie(((uint8 **)&ie), &ptr, &ptr_len)) {
				iwe.cmd = IWEVGENIE;
				iwe.u.data.length = ie->len + 2;
				event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)ie);
				break;
			}
		}

	*event_p = event;
	}

#endif 
	return 0;
}
static int
wl_iw_get_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	channel_info_t ci;
	wl_scan_results_t *list;
	struct iw_event	iwe;
	wl_bss_info_t *bi = NULL;
	int error, i, j;
	char *event = extra, *end = extra + dwrq->length, *value;
	uint buflen = dwrq->length;

	WL_TRACE(("%s: SIOCGIWSCAN\n", dev->name));

	if (!extra)
		return -EINVAL;

	
	if ((error = dev_wlc_ioctl(dev, WLC_GET_CHANNEL, &ci, sizeof(ci))))
		return error;
	ci.scan_channel = dtoh32(ci.scan_channel);
	if (ci.scan_channel)
		return -EAGAIN;

	
	list = kmalloc(buflen, GFP_KERNEL);
	if (!list)
		return -ENOMEM;
	memset(list, 0, buflen);
	list->buflen = htod32(buflen);
	if ((error = dev_wlc_ioctl(dev, WLC_SCAN_RESULTS, list, buflen))) {
		kfree(list);
		return error;
	}
	list->buflen = dtoh32(list->buflen);
	list->version = dtoh32(list->version);
	list->count = dtoh32(list->count);

	ASSERT(list->version == WL_BSS_INFO_VERSION);

	for (i = 0; i < list->count && i < IW_MAX_AP; i++) {
		bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length)) : list->bss_info;
		ASSERT(((uintptr)bi + dtoh32(bi->length)) <= ((uintptr)list +
			buflen));

		
		iwe.cmd = SIOCGIWAP;
		iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
		memcpy(iwe.u.ap_addr.sa_data, &bi->BSSID, ETHER_ADDR_LEN);
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_ADDR_LEN);

		
		iwe.u.data.length = dtoh32(bi->SSID_len);
		iwe.cmd = SIOCGIWESSID;
		iwe.u.data.flags = 1;
		event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, bi->SSID);

		
		if (dtoh16(bi->capability) & (DOT11_CAP_ESS | DOT11_CAP_IBSS)) {
			iwe.cmd = SIOCGIWMODE;
			if (dtoh16(bi->capability) & DOT11_CAP_ESS)
				iwe.u.mode = IW_MODE_INFRA;
			else
				iwe.u.mode = IW_MODE_ADHOC;
			event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_UINT_LEN);
		}

		
		iwe.cmd = SIOCGIWFREQ;
		iwe.u.freq.m = wf_channel2mhz(CHSPEC_CHANNEL(bi->chanspec),
			CHSPEC_CHANNEL(bi->chanspec) <= CH_MAX_2G_CHANNEL ?
			WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
		iwe.u.freq.e = 6;
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_FREQ_LEN);

		
		iwe.cmd = IWEVQUAL;
		iwe.u.qual.qual = rssi_to_qual(dtoh16(bi->RSSI));
		iwe.u.qual.level = 0x100 + dtoh16(bi->RSSI);
		iwe.u.qual.noise = 0x100 + bi->phy_noise;
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_QUAL_LEN);

		
		 wl_iw_handle_scanresults_ies(&event, end, info, bi);

		
		iwe.cmd = SIOCGIWENCODE;
		if (dtoh16(bi->capability) & DOT11_CAP_PRIVACY)
			iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		else
			iwe.u.data.flags = IW_ENCODE_DISABLED;
		iwe.u.data.length = 0;
		event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)event);

		
		if (bi->rateset.count) {
			value = event + IW_EV_LCP_LEN;
			iwe.cmd = SIOCGIWRATE;
			
			iwe.u.bitrate.fixed = iwe.u.bitrate.disabled = 0;
			for (j = 0; j < bi->rateset.count && j < IW_MAX_BITRATES; j++) {
				iwe.u.bitrate.value = (bi->rateset.rates[j] & 0x7f) * 500000;
				value = IWE_STREAM_ADD_VALUE(info, event, value, end, &iwe,
					IW_EV_PARAM_LEN);
			}
			event = value;
		}
	}

	kfree(list);

	dwrq->length = event - extra;
	dwrq->flags = 0;	

	return 0;
}

static int
wl_iw_iscan_get_scan(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_scan_results_t *list;
	struct iw_event	iwe;
	wl_bss_info_t *bi = NULL;
	int ii, j;
	int apcnt;
	char *event = extra, *end = extra + dwrq->length, *value;
	iscan_info_t *iscan = g_iscan;
	iscan_buf_t * p_buf;

	WL_TRACE(("%s: SIOCGIWSCAN\n", dev->name));

	if (!extra)
		return -EINVAL;

	
	if ((!iscan) || (iscan->sysioc_pid < 0)) {
		return wl_iw_get_scan(dev, info, dwrq, extra);
	}

	
	if (iscan->iscan_state == ISCAN_STATE_SCANING)
		return -EAGAIN;

	apcnt = 0;
	p_buf = iscan->list_hdr;
	
	while (p_buf != iscan->list_cur) {
	    list = &((wl_iscan_results_t*)p_buf->iscan_buf)->results;

	    if (list->version != WL_BSS_INFO_VERSION) {
		WL_ERROR(("list->version %d != WL_BSS_INFO_VERSION\n", list->version));
	    }

	    bi = NULL;
	    for (ii = 0; ii < list->count && apcnt < IW_MAX_AP; apcnt++, ii++) {
		bi = bi ? (wl_bss_info_t *)((uintptr)bi + dtoh32(bi->length)) : list->bss_info;
		ASSERT(((uintptr)bi + dtoh32(bi->length)) <= ((uintptr)list +
			WLC_IW_ISCAN_MAXLEN));

		
		if (event + ETHER_ADDR_LEN + bi->SSID_len + IW_EV_UINT_LEN + IW_EV_FREQ_LEN +
			IW_EV_QUAL_LEN >= end)
			return -E2BIG;
		
		iwe.cmd = SIOCGIWAP;
		iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
		memcpy(iwe.u.ap_addr.sa_data, &bi->BSSID, ETHER_ADDR_LEN);
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_ADDR_LEN);

		
		iwe.u.data.length = dtoh32(bi->SSID_len);
		iwe.cmd = SIOCGIWESSID;
		iwe.u.data.flags = 1;
		event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, bi->SSID);

		
		if (dtoh16(bi->capability) & (DOT11_CAP_ESS | DOT11_CAP_IBSS)) {
			iwe.cmd = SIOCGIWMODE;
			if (dtoh16(bi->capability) & DOT11_CAP_ESS)
				iwe.u.mode = IW_MODE_INFRA;
			else
				iwe.u.mode = IW_MODE_ADHOC;
			event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_UINT_LEN);
		}

		
		iwe.cmd = SIOCGIWFREQ;
		iwe.u.freq.m = wf_channel2mhz(CHSPEC_CHANNEL(bi->chanspec),
			CHSPEC_CHANNEL(bi->chanspec) <= CH_MAX_2G_CHANNEL ?
			WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
		iwe.u.freq.e = 6;
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_FREQ_LEN);

		
		iwe.cmd = IWEVQUAL;
		iwe.u.qual.qual = rssi_to_qual(dtoh16(bi->RSSI));
		iwe.u.qual.level = 0x100 + dtoh16(bi->RSSI);
		iwe.u.qual.noise = 0x100 + bi->phy_noise;
		event = IWE_STREAM_ADD_EVENT(info, event, end, &iwe, IW_EV_QUAL_LEN);

		
		wl_iw_handle_scanresults_ies(&event, end, info, bi);

		
		iwe.cmd = SIOCGIWENCODE;
		if (dtoh16(bi->capability) & DOT11_CAP_PRIVACY)
			iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		else
			iwe.u.data.flags = IW_ENCODE_DISABLED;
		iwe.u.data.length = 0;
		event = IWE_STREAM_ADD_POINT(info, event, end, &iwe, (char *)event);

		
		if (bi->rateset.count <= sizeof(bi->rateset.rates)) {
			if (event + IW_MAX_BITRATES*IW_EV_PARAM_LEN >= end)
				return -E2BIG;

			value = event + IW_EV_LCP_LEN;
			iwe.cmd = SIOCGIWRATE;
			
			iwe.u.bitrate.fixed = iwe.u.bitrate.disabled = 0;
			for (j = 0; j < bi->rateset.count && j < IW_MAX_BITRATES; j++) {
				iwe.u.bitrate.value = (bi->rateset.rates[j] & 0x7f) * 500000;
				value = IWE_STREAM_ADD_VALUE(info, event, value, end, &iwe,
					IW_EV_PARAM_LEN);
			}
			event = value;
		}
	    }
	    p_buf = p_buf->next;
	} 

	dwrq->length = event - extra;
	dwrq->flags = 0;	

	return 0;
}

#endif 


static int
wl_iw_set_essid(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wlc_ssid_t ssid;
	int error;

	WL_TRACE(("%s: SIOCSIWESSID\n", dev->name));

	
	memset(&ssid, 0, sizeof(ssid));
	if (dwrq->length && extra) {
#if WIRELESS_EXT > 20
		ssid.SSID_len = MIN(sizeof(ssid.SSID), dwrq->length);
#else
		ssid.SSID_len = MIN(sizeof(ssid.SSID), dwrq->length-1);
#endif
		memcpy(ssid.SSID, extra, ssid.SSID_len);
		ssid.SSID_len = htod32(ssid.SSID_len);

		if ((error = dev_wlc_ioctl(dev, WLC_SET_SSID, &ssid, sizeof(ssid))))
			return error;
	}
	
	else {
		scb_val_t scbval;
		bzero(&scbval, sizeof(scb_val_t));
		if ((error = dev_wlc_ioctl(dev, WLC_DISASSOC, &scbval, sizeof(scb_val_t))))
			return error;
	}
	return 0;
}

static int
wl_iw_get_essid(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wlc_ssid_t ssid;
	int error;

	WL_TRACE(("%s: SIOCGIWESSID\n", dev->name));

	if (!extra)
		return -EINVAL;

	if ((error = dev_wlc_ioctl(dev, WLC_GET_SSID, &ssid, sizeof(ssid)))) {
		WL_ERROR(("Error getting the SSID\n"));
		return error;
	}

	ssid.SSID_len = dtoh32(ssid.SSID_len);

	
	memcpy(extra, ssid.SSID, ssid.SSID_len);

	dwrq->length = ssid.SSID_len;

	dwrq->flags = 1; 

	return 0;
}

static int
wl_iw_set_nick(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_iw_t *iw = IW_DEV_IF(dev);
	WL_TRACE(("%s: SIOCSIWNICKN\n", dev->name));

	if (!extra)
		return -EINVAL;

	
	if (dwrq->length > sizeof(iw->nickname))
		return -E2BIG;

	memcpy(iw->nickname, extra, dwrq->length);
	iw->nickname[dwrq->length - 1] = '\0';

	return 0;
}

static int
wl_iw_get_nick(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_iw_t *iw = IW_DEV_IF(dev);
	WL_TRACE(("%s: SIOCGIWNICKN\n", dev->name));

	if (!extra)
		return -EINVAL;

	strcpy(extra, iw->nickname);
	dwrq->length = strlen(extra) + 1;

	return 0;
}

static int wl_iw_set_rate(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	wl_rateset_t rateset;
	int error, rate, i, error_bg, error_a;

	WL_TRACE(("%s: SIOCSIWRATE\n", dev->name));

	
	if ((error = dev_wlc_ioctl(dev, WLC_GET_CURR_RATESET, &rateset, sizeof(rateset))))
		return error;

	rateset.count = dtoh32(rateset.count);

	if (vwrq->value < 0) {
		
		rate = rateset.rates[rateset.count - 1] & 0x7f;
	} else if (vwrq->value < rateset.count) {
		
		rate = rateset.rates[vwrq->value] & 0x7f;
	} else {
		
		rate = vwrq->value / 500000;
	}

	if (vwrq->fixed) {
		
		error_bg = dev_wlc_intvar_set(dev, "bg_rate", rate);
		error_a = dev_wlc_intvar_set(dev, "a_rate", rate);

		if (error_bg && error_a)
			return (error_bg | error_a);
	} else {
		
		
		error_bg = dev_wlc_intvar_set(dev, "bg_rate", 0);
		
		error_a = dev_wlc_intvar_set(dev, "a_rate", 0);

		if (error_bg && error_a)
			return (error_bg | error_a);

		
		for (i = 0; i < rateset.count; i++)
			if ((rateset.rates[i] & 0x7f) > rate)
				break;
		rateset.count = htod32(i);

		
		if ((error = dev_wlc_ioctl(dev, WLC_SET_RATESET, &rateset, sizeof(rateset))))
			return error;
	}

	return 0;
}

static int wl_iw_get_rate(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, rate;

	WL_TRACE(("%s: SIOCGIWRATE\n", dev->name));

	
	if ((error = dev_wlc_ioctl(dev, WLC_GET_RATE, &rate, sizeof(rate))))
		return error;
	rate = dtoh32(rate);
	vwrq->value = rate * 500000;

	return 0;
}

static int
wl_iw_set_rts(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, rts;

	WL_TRACE(("%s: SIOCSIWRTS\n", dev->name));

	if (vwrq->disabled)
		rts = DOT11_DEFAULT_RTS_LEN;
	else if (vwrq->value < 0 || vwrq->value > DOT11_DEFAULT_RTS_LEN)
		return -EINVAL;
	else
		rts = vwrq->value;

	if ((error = dev_wlc_intvar_set(dev, "rtsthresh", rts)))
		return error;

	return 0;
}

static int
wl_iw_get_rts(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, rts;

	WL_TRACE(("%s: SIOCGIWRTS\n", dev->name));

	if ((error = dev_wlc_intvar_get(dev, "rtsthresh", &rts)))
		return error;

	vwrq->value = rts;
	vwrq->disabled = (rts >= DOT11_DEFAULT_RTS_LEN);
	vwrq->fixed = 1;

	return 0;
}

static int
wl_iw_set_frag(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, frag;

	WL_TRACE(("%s: SIOCSIWFRAG\n", dev->name));

	if (vwrq->disabled)
		frag = DOT11_DEFAULT_FRAG_LEN;
	else if (vwrq->value < 0 || vwrq->value > DOT11_DEFAULT_FRAG_LEN)
		return -EINVAL;
	else
		frag = vwrq->value;

	if ((error = dev_wlc_intvar_set(dev, "fragthresh", frag)))
		return error;

	return 0;
}

static int
wl_iw_get_frag(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, fragthreshold;

	WL_TRACE(("%s: SIOCGIWFRAG\n", dev->name));

	if ((error = dev_wlc_intvar_get(dev, "fragthresh", &fragthreshold)))
		return error;

	vwrq->value = fragthreshold;
	vwrq->disabled = (fragthreshold >= DOT11_DEFAULT_FRAG_LEN);
	vwrq->fixed = 1;

	return 0;
}

static int
wl_iw_set_txpow(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, disable;
	uint16 txpwrmw;
	WL_TRACE(("%s: SIOCSIWTXPOW\n", dev->name));

	
	disable = vwrq->disabled ? WL_RADIO_SW_DISABLE : 0;
	disable += WL_RADIO_SW_DISABLE << 16;

	disable = htod32(disable);
	if ((error = dev_wlc_ioctl(dev, WLC_SET_RADIO, &disable, sizeof(disable))))
		return error;

	
	if (disable & WL_RADIO_SW_DISABLE)
		return 0;

	
	if (!(vwrq->flags & IW_TXPOW_MWATT))
		return -EINVAL;

	
	if (vwrq->value < 0)
		return 0;

	if (vwrq->value > 0xffff) txpwrmw = 0xffff;
	else txpwrmw = (uint16)vwrq->value;


	error = dev_wlc_intvar_set(dev, "qtxpower", (int)(bcm_mw_to_qdbm(txpwrmw)));
	return error;
}

static int
wl_iw_get_txpow(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, disable, txpwrdbm;
	uint8 result;

	WL_TRACE(("%s: SIOCGIWTXPOW\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_RADIO, &disable, sizeof(disable))) ||
	    (error = dev_wlc_intvar_get(dev, "qtxpower", &txpwrdbm)))
		return error;

	disable = dtoh32(disable);
	result = (uint8)(txpwrdbm & ~WL_TXPWR_OVERRIDE);
	vwrq->value = (int32)bcm_qdbm_to_mw(result);
	vwrq->fixed = 0;
	vwrq->disabled = (disable & (WL_RADIO_SW_DISABLE | WL_RADIO_HW_DISABLE)) ? 1 : 0;
	vwrq->flags = IW_TXPOW_MWATT;

	return 0;
}

#if WIRELESS_EXT > 10
static int
wl_iw_set_retry(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, lrl, srl;

	WL_TRACE(("%s: SIOCSIWRETRY\n", dev->name));

	
	if (vwrq->disabled || (vwrq->flags & IW_RETRY_LIFETIME))
		return -EINVAL;

	
	if (vwrq->flags & IW_RETRY_LIMIT) {
		
#if WIRELESS_EXT > 20
		if ((vwrq->flags & IW_RETRY_LONG) ||(vwrq->flags & IW_RETRY_MAX) ||
			!((vwrq->flags & IW_RETRY_SHORT) || (vwrq->flags & IW_RETRY_MIN))) {
#else
		if ((vwrq->flags & IW_RETRY_MAX) || !(vwrq->flags & IW_RETRY_MIN)) {
#endif 

			lrl = htod32(vwrq->value);
			if ((error = dev_wlc_ioctl(dev, WLC_SET_LRL, &lrl, sizeof(lrl))))
				return error;
		}
		
#if WIRELESS_EXT > 20
		if ((vwrq->flags & IW_RETRY_SHORT) ||(vwrq->flags & IW_RETRY_MIN) ||
			!((vwrq->flags & IW_RETRY_LONG) || (vwrq->flags & IW_RETRY_MAX))) {
#else
		if ((vwrq->flags & IW_RETRY_MIN) || !(vwrq->flags & IW_RETRY_MAX)) {
#endif 

			srl = htod32(vwrq->value);
			if ((error = dev_wlc_ioctl(dev, WLC_SET_SRL, &srl, sizeof(srl))))
				return error;
		}
	}

	return 0;
}

static int
wl_iw_get_retry(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, lrl, srl;

	WL_TRACE(("%s: SIOCGIWRETRY\n", dev->name));

	vwrq->disabled = 0;      

	
	if ((vwrq->flags & IW_RETRY_TYPE) == IW_RETRY_LIFETIME)
		return -EINVAL;

	
	if ((error = dev_wlc_ioctl(dev, WLC_GET_LRL, &lrl, sizeof(lrl))) ||
	    (error = dev_wlc_ioctl(dev, WLC_GET_SRL, &srl, sizeof(srl))))
		return error;

	lrl = dtoh32(lrl);
	srl = dtoh32(srl);

	
	if (vwrq->flags & IW_RETRY_MAX) {
		vwrq->flags = IW_RETRY_LIMIT | IW_RETRY_MAX;
		vwrq->value = lrl;
	} else {
		vwrq->flags = IW_RETRY_LIMIT;
		vwrq->value = srl;
		if (srl != lrl)
			vwrq->flags |= IW_RETRY_MIN;
	}

	return 0;
}
#endif 

static int
wl_iw_set_encode(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_wsec_key_t key;
	int error, val, wsec;

	WL_TRACE(("%s: SIOCSIWENCODE\n", dev->name));

	memset(&key, 0, sizeof(key));

	if ((dwrq->flags & IW_ENCODE_INDEX) == 0) {
		
		for (key.index = 0; key.index < DOT11_MAX_DEFAULT_KEYS; key.index++) {
			val = htod32(key.index);
			if ((error = dev_wlc_ioctl(dev, WLC_GET_KEY_PRIMARY, &val, sizeof(val))))
				return error;
			val = dtoh32(val);
			if (val)
				break;
		}
		
		if (key.index == DOT11_MAX_DEFAULT_KEYS)
			key.index = 0;
	} else {
		key.index = (dwrq->flags & IW_ENCODE_INDEX) - 1;
		if (key.index >= DOT11_MAX_DEFAULT_KEYS)
			return -EINVAL;
	}

	
	wsec = (dwrq->flags & IW_ENCODE_DISABLED) ? 0 : WEP_ENABLED;

	if ((error = dev_wlc_intvar_set(dev, "wsec", wsec)))
		return error;

	
	if (!extra || !dwrq->length || (dwrq->flags & IW_ENCODE_NOKEY)) {
		
		val = htod32(key.index);
		if ((error = dev_wlc_ioctl(dev, WLC_SET_KEY_PRIMARY, &val, sizeof(val))))
			return error;
	} else {
		key.len = dwrq->length;

		if (dwrq->length > sizeof(key.data))
			return -EINVAL;

		memcpy(key.data, extra, dwrq->length);

		key.flags = WL_PRIMARY_KEY;
		switch (key.len) {
		case WEP1_KEY_SIZE:
			key.algo = CRYPTO_ALGO_WEP1;
			break;
		case WEP128_KEY_SIZE:
			key.algo = CRYPTO_ALGO_WEP128;
			break;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14)
		case TKIP_KEY_SIZE:
			key.algo = CRYPTO_ALGO_TKIP;
			break;
#endif
		case AES_KEY_SIZE:
			key.algo = CRYPTO_ALGO_AES_CCM;
			break;
		default:
			return -EINVAL;
		}

		
		swap_key_from_BE(&key);
		if ((error = dev_wlc_ioctl(dev, WLC_SET_KEY, &key, sizeof(key))))
			return error;
	}

	
	val = (dwrq->flags & IW_ENCODE_RESTRICTED) ? 1 : 0;
	val = htod32(val);
	if ((error = dev_wlc_ioctl(dev, WLC_SET_AUTH, &val, sizeof(val))))
		return error;

	return 0;
}

static int
wl_iw_get_encode(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_wsec_key_t key;
	int error, val, wsec, auth;

	WL_TRACE(("%s: SIOCGIWENCODE\n", dev->name));

	
	bzero(&key, sizeof(wl_wsec_key_t));

	if ((dwrq->flags & IW_ENCODE_INDEX) == 0) {
		
		for (key.index = 0; key.index < DOT11_MAX_DEFAULT_KEYS; key.index++) {
			val = key.index;
			if ((error = dev_wlc_ioctl(dev, WLC_GET_KEY_PRIMARY, &val, sizeof(val))))
				return error;
			val = dtoh32(val);
			if (val)
				break;
		}
	} else
		key.index = (dwrq->flags & IW_ENCODE_INDEX) - 1;

	if (key.index >= DOT11_MAX_DEFAULT_KEYS)
		key.index = 0;

	

	if ((error = dev_wlc_ioctl(dev, WLC_GET_WSEC, &wsec, sizeof(wsec))) ||
	    (error = dev_wlc_ioctl(dev, WLC_GET_AUTH, &auth, sizeof(auth))))
		return error;

	swap_key_to_BE(&key);

	wsec = dtoh32(wsec);
	auth = dtoh32(auth);
	
	dwrq->length = MIN(IW_ENCODING_TOKEN_MAX, key.len);

	
	dwrq->flags = key.index + 1;
	if (!(wsec & (WEP_ENABLED | TKIP_ENABLED | AES_ENABLED))) {
		
		dwrq->flags |= IW_ENCODE_DISABLED;
	}
	if (auth) {
		
		dwrq->flags |= IW_ENCODE_RESTRICTED;
	}

	
	if (dwrq->length && extra)
		memcpy(extra, key.data, dwrq->length);

	return 0;
}

static int
wl_iw_set_power(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, pm;

	WL_TRACE(("%s: SIOCSIWPOWER\n", dev->name));

	pm = vwrq->disabled ? PM_OFF : PM_MAX;

	pm = htod32(pm);
	if ((error = dev_wlc_ioctl(dev, WLC_SET_PM, &pm, sizeof(pm))))
		return error;

	return 0;
}

static int
wl_iw_get_power(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error, pm;

	WL_TRACE(("%s: SIOCGIWPOWER\n", dev->name));

	if ((error = dev_wlc_ioctl(dev, WLC_GET_PM, &pm, sizeof(pm))))
		return error;

	pm = dtoh32(pm);
	vwrq->disabled = pm ? 0 : 1;
	vwrq->flags = IW_POWER_ALL_R;

	return 0;
}

#if WIRELESS_EXT > 17
static int
wl_iw_set_wpaie(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *iwp,
	char *extra
)
{
		dev_wlc_bufvar_set(dev, "wpaie", extra, iwp->length);

	return 0;
}

static int
wl_iw_get_wpaie(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *iwp,
	char *extra
)
{
	WL_TRACE(("%s: SIOCGIWGENIE\n", dev->name));
	iwp->length = 64;
	dev_wlc_bufvar_get(dev, "wpaie", extra, iwp->length);
	return 0;
}

static int
wl_iw_set_encodeext(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,
	char *extra
)
{
	wl_wsec_key_t key;
	int error;
	struct iw_encode_ext *iwe;

	WL_TRACE(("%s: SIOCSIWENCODEEXT\n", dev->name));

	memset(&key, 0, sizeof(key));
	iwe = (struct iw_encode_ext *)extra;

	
	if (dwrq->flags & IW_ENCODE_DISABLED) {

	}

	
	key.index = 0;
	if (dwrq->flags & IW_ENCODE_INDEX)
		key.index = (dwrq->flags & IW_ENCODE_INDEX) - 1;

	key.len = iwe->key_len;

	
	if (!ETHER_ISMULTI(iwe->addr.sa_data))
		bcopy((void *)&iwe->addr.sa_data, (char *)&key.ea, ETHER_ADDR_LEN);

	
	if (key.len == 0) {
		if (iwe->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
			WL_WSEC(("Changing the the primary Key to %d\n", key.index));
			
			key.index = htod32(key.index);
			error = dev_wlc_ioctl(dev, WLC_SET_KEY_PRIMARY,
				&key.index, sizeof(key.index));
			if (error)
				return error;
		}
		
		else {
			swap_key_from_BE(&key);
			error = dev_wlc_ioctl(dev, WLC_SET_KEY, &key, sizeof(key));
			if (error)
				return error;
		}
	}
#if (defined(BCMSUP_PSK) && defined(WLFBT))
	
	else if (iwe->alg == IW_ENCODE_ALG_PMK) {
		int j;
		wsec_pmk_t pmk;
		char keystring[WSEC_MAX_PSK_LEN + 1];
		char* charptr = keystring;
		uint len;

		
		for (j = 0; j < (WSEC_MAX_PSK_LEN / 2); j++) {
			sprintf(charptr, "%02x", iwe->key[j]);
			charptr += 2;
		}
		len = strlen(keystring);
		pmk.key_len = htod16(len);
		bcopy(keystring, pmk.key, len);
		pmk.flags = htod16(WSEC_PASSPHRASE);

		error = dev_wlc_ioctl(dev, WLC_SET_WSEC_PMK, &pmk, sizeof(pmk));
		if (error)
			return error;
	}
#endif 

	else {
		if (iwe->key_len > sizeof(key.data))
			return -EINVAL;

		WL_WSEC(("Setting the key index %d\n", key.index));
		if (iwe->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
			WL_WSEC(("key is a Primary Key\n"));
			key.flags = WL_PRIMARY_KEY;
		}

		bcopy((void *)iwe->key, key.data, iwe->key_len);

		if (iwe->alg == IW_ENCODE_ALG_TKIP) {
			uint8 keybuf[8];
			bcopy(&key.data[24], keybuf, sizeof(keybuf));
			bcopy(&key.data[16], &key.data[24], sizeof(keybuf));
			bcopy(keybuf, &key.data[16], sizeof(keybuf));
		}

		
		if (iwe->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID) {
			uchar *ivptr;
			ivptr = (uchar *)iwe->rx_seq;
			key.rxiv.hi = (ivptr[5] << 24) | (ivptr[4] << 16) |
				(ivptr[3] << 8) | ivptr[2];
			key.rxiv.lo = (ivptr[1] << 8) | ivptr[0];
			key.iv_initialized = TRUE;
		}

		switch (iwe->alg) {
			case IW_ENCODE_ALG_NONE:
				key.algo = CRYPTO_ALGO_OFF;
				break;
			case IW_ENCODE_ALG_WEP:
				if (iwe->key_len == WEP1_KEY_SIZE)
					key.algo = CRYPTO_ALGO_WEP1;
				else
					key.algo = CRYPTO_ALGO_WEP128;
				break;
			case IW_ENCODE_ALG_TKIP:
				key.algo = CRYPTO_ALGO_TKIP;
				break;
			case IW_ENCODE_ALG_CCMP:
				key.algo = CRYPTO_ALGO_AES_CCM;
				break;
			default:
				break;
		}
		swap_key_from_BE(&key);

		dhd_wait_pend8021x(dev);

		error = dev_wlc_ioctl(dev, WLC_SET_KEY, &key, sizeof(key));
		if (error)
			return error;
	}
	return 0;
}


#if WIRELESS_EXT > 17
struct {
	pmkid_list_t pmkids;
	pmkid_t foo[MAXPMKID-1];
} pmkid_list;
static int
wl_iw_set_pmksa(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	struct iw_pmksa *iwpmksa;
	uint i;
	char eabuf[ETHER_ADDR_STR_LEN];
	pmkid_t * pmkid_array = pmkid_list.pmkids.pmkid;

	WL_TRACE(("%s: SIOCSIWPMKSA\n", dev->name));
	iwpmksa = (struct iw_pmksa *)extra;
	bzero((char *)eabuf, ETHER_ADDR_STR_LEN);
	if (iwpmksa->cmd == IW_PMKSA_FLUSH) {
		WL_TRACE(("wl_iw_set_pmksa - IW_PMKSA_FLUSH\n"));
		bzero((char *)&pmkid_list, sizeof(pmkid_list));
	}
	if (iwpmksa->cmd == IW_PMKSA_REMOVE) {
		pmkid_list_t pmkid, *pmkidptr;
		pmkidptr = &pmkid;
		bcopy(&iwpmksa->bssid.sa_data[0], &pmkidptr->pmkid[0].BSSID, ETHER_ADDR_LEN);
		bcopy(&iwpmksa->pmkid[0], &pmkidptr->pmkid[0].PMKID, WPA2_PMKID_LEN);
		{
			uint j;
			WL_TRACE(("wl_iw_set_pmksa,IW_PMKSA_REMOVE - PMKID: %s = ",
				bcm_ether_ntoa(&pmkidptr->pmkid[0].BSSID,
				eabuf)));
			for (j = 0; j < WPA2_PMKID_LEN; j++)
				WL_TRACE(("%02x ", pmkidptr->pmkid[0].PMKID[j]));
			WL_TRACE(("\n"));
		}
		for (i = 0; i < pmkid_list.pmkids.npmkid; i++)
			if (!bcmp(&iwpmksa->bssid.sa_data[0], &pmkid_array[i].BSSID,
				ETHER_ADDR_LEN))
				break;
		for (; i < pmkid_list.pmkids.npmkid; i++) {
			bcopy(&pmkid_array[i+1].BSSID,
				&pmkid_array[i].BSSID,
				ETHER_ADDR_LEN);
			bcopy(&pmkid_array[i+1].PMKID,
				&pmkid_array[i].PMKID,
				WPA2_PMKID_LEN);
		}
		pmkid_list.pmkids.npmkid--;
	}
	if (iwpmksa->cmd == IW_PMKSA_ADD) {
		bcopy(&iwpmksa->bssid.sa_data[0],
			&pmkid_array[pmkid_list.pmkids.npmkid].BSSID,
			ETHER_ADDR_LEN);
		bcopy(&iwpmksa->pmkid[0], &pmkid_array[pmkid_list.pmkids.npmkid].PMKID,
			WPA2_PMKID_LEN);
		{
			uint j;
			uint k;
			k = pmkid_list.pmkids.npmkid;
			BCM_REFERENCE(k);
			WL_TRACE(("wl_iw_set_pmksa,IW_PMKSA_ADD - PMKID: %s = ",
				bcm_ether_ntoa(&pmkid_array[k].BSSID,
				eabuf)));
			for (j = 0; j < WPA2_PMKID_LEN; j++)
				WL_TRACE(("%02x ", pmkid_array[k].PMKID[j]));
			WL_TRACE(("\n"));
		}
		pmkid_list.pmkids.npmkid++;
	}
	WL_TRACE(("PRINTING pmkid LIST - No of elements %d\n", pmkid_list.pmkids.npmkid));
	for (i = 0; i < pmkid_list.pmkids.npmkid; i++) {
		uint j;
		WL_TRACE(("PMKID[%d]: %s = ", i,
			bcm_ether_ntoa(&pmkid_array[i].BSSID,
			eabuf)));
		for (j = 0; j < WPA2_PMKID_LEN; j++)
			WL_TRACE(("%02x ", pmkid_array[i].PMKID[j]));
		printf("\n");
	}
	WL_TRACE(("\n"));
	dev_wlc_bufvar_set(dev, "pmkid_info", (char *)&pmkid_list, sizeof(pmkid_list));
	return 0;
}
#endif 

static int
wl_iw_get_encodeext(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	WL_TRACE(("%s: SIOCGIWENCODEEXT\n", dev->name));
	return 0;
}

static int
wl_iw_set_wpaauth(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error = 0;
	int paramid;
	int paramval;
	uint32 cipher_combined;
	int val = 0;
	wl_iw_t *iw = IW_DEV_IF(dev);

	WL_TRACE(("%s: SIOCSIWAUTH\n", dev->name));

	paramid = vwrq->flags & IW_AUTH_INDEX;
	paramval = vwrq->value;

	WL_TRACE(("%s: SIOCSIWAUTH, paramid = 0x%0x, paramval = 0x%0x\n",
		dev->name, paramid, paramval));

	switch (paramid) {

	case IW_AUTH_WPA_VERSION:
		
		if (paramval & IW_AUTH_WPA_VERSION_DISABLED)
			val = WPA_AUTH_DISABLED;
		else if (paramval & (IW_AUTH_WPA_VERSION_WPA))
			val = WPA_AUTH_PSK | WPA_AUTH_UNSPECIFIED;
		else if (paramval & IW_AUTH_WPA_VERSION_WPA2)
			val = WPA2_AUTH_PSK | WPA2_AUTH_UNSPECIFIED;
		WL_TRACE(("%s: %d: setting wpa_auth to 0x%0x\n", __FUNCTION__, __LINE__, val));
		if ((error = dev_wlc_intvar_set(dev, "wpa_auth", val)))
			return error;
		break;

	case IW_AUTH_CIPHER_PAIRWISE:
	case IW_AUTH_CIPHER_GROUP:

		if (paramid == IW_AUTH_CIPHER_PAIRWISE) {
			iw->pwsec = paramval;
		}
		else {
			iw->gwsec = paramval;
		}

		if ((error = dev_wlc_intvar_get(dev, "wsec", &val)))
			return error;

		cipher_combined = iw->gwsec | iw->pwsec;
		val &= ~(WEP_ENABLED | TKIP_ENABLED | AES_ENABLED);
		if (cipher_combined & (IW_AUTH_CIPHER_WEP40 | IW_AUTH_CIPHER_WEP104))
			val |= WEP_ENABLED;
		if (cipher_combined & IW_AUTH_CIPHER_TKIP)
			val |= TKIP_ENABLED;
		if (cipher_combined & IW_AUTH_CIPHER_CCMP)
			val |= AES_ENABLED;

		if (iw->privacy_invoked && !val) {
			WL_WSEC(("%s: %s: 'Privacy invoked' TRUE but clearing wsec, assuming "
			         "we're a WPS enrollee\n", dev->name, __FUNCTION__));
			if ((error = dev_wlc_intvar_set(dev, "is_WPS_enrollee", TRUE))) {
				WL_WSEC(("Failed to set iovar is_WPS_enrollee\n"));
				return error;
			}
		} else if (val) {
			if ((error = dev_wlc_intvar_set(dev, "is_WPS_enrollee", FALSE))) {
				WL_WSEC(("Failed to clear iovar is_WPS_enrollee\n"));
				return error;
			}
		}

		if ((error = dev_wlc_intvar_set(dev, "wsec", val)))
			return error;
#ifdef WLFBT
		if ((paramid == IW_AUTH_CIPHER_PAIRWISE) && (val | AES_ENABLED)) {
			if ((error = dev_wlc_intvar_set(dev, "sup_wpa", 1)))
				return error;
		}
		else if (val == 0) {
			if ((error = dev_wlc_intvar_set(dev, "sup_wpa", 0)))
				return error;
		}
#endif 
		break;

	case IW_AUTH_KEY_MGMT:
		if ((error = dev_wlc_intvar_get(dev, "wpa_auth", &val)))
			return error;

		if (val & (WPA_AUTH_PSK | WPA_AUTH_UNSPECIFIED)) {
			if (paramval & IW_AUTH_KEY_MGMT_PSK)
				val = WPA_AUTH_PSK;
			else
				val = WPA_AUTH_UNSPECIFIED;
		}
		else if (val & (WPA2_AUTH_PSK | WPA2_AUTH_UNSPECIFIED)) {
			if (paramval & IW_AUTH_KEY_MGMT_PSK)
				val = WPA2_AUTH_PSK;
			else
				val = WPA2_AUTH_UNSPECIFIED;
		}
		WL_TRACE(("%s: %d: setting wpa_auth to %d\n", __FUNCTION__, __LINE__, val));
		if ((error = dev_wlc_intvar_set(dev, "wpa_auth", val)))
			return error;
		break;

	case IW_AUTH_TKIP_COUNTERMEASURES:
		dev_wlc_bufvar_set(dev, "tkip_countermeasures", (char *)&paramval, 1);
		break;

	case IW_AUTH_80211_AUTH_ALG:
		
		WL_ERROR(("Setting the D11auth %d\n", paramval));
        if (paramval == IW_AUTH_ALG_OPEN_SYSTEM)                        
            val = 0;      
        else if (paramval == IW_AUTH_ALG_SHARED_KEY)
            val = 1;
        else if (paramval == (IW_AUTH_ALG_OPEN_SYSTEM | IW_AUTH_ALG_SHARED_KEY))
        {                
            val = 3;           
        }
		else
			error = 1;
		if (!error && (error = dev_wlc_intvar_set(dev, "auth", val)))
			return error;
		break;

	case IW_AUTH_WPA_ENABLED:
		if (paramval == 0) {
			val = 0;
			WL_TRACE(("%s: %d: setting wpa_auth to %d\n", __FUNCTION__, __LINE__, val));
			error = dev_wlc_intvar_set(dev, "wpa_auth", val);
			return error;
		}
		else {
			
		}
		break;

	case IW_AUTH_DROP_UNENCRYPTED:
		dev_wlc_bufvar_set(dev, "wsec_restrict", (char *)&paramval, 1);
		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		dev_wlc_bufvar_set(dev, "rx_unencrypted_eapol", (char *)&paramval, 1);
		break;

#if WIRELESS_EXT > 17

	case IW_AUTH_ROAMING_CONTROL:
		WL_TRACE(("%s: IW_AUTH_ROAMING_CONTROL\n", __FUNCTION__));
		
		break;

	case IW_AUTH_PRIVACY_INVOKED: {
		int wsec;

		if (paramval == 0) {
			iw->privacy_invoked = FALSE;
			if ((error = dev_wlc_intvar_set(dev, "is_WPS_enrollee", FALSE))) {
				WL_WSEC(("Failed to clear iovar is_WPS_enrollee\n"));
				return error;
			}
		} else {
			iw->privacy_invoked = TRUE;
			if ((error = dev_wlc_intvar_get(dev, "wsec", &wsec)))
				return error;

			if (!WSEC_ENABLED(wsec)) {
				
				if ((error = dev_wlc_intvar_set(dev, "is_WPS_enrollee", TRUE))) {
					WL_WSEC(("Failed to set iovar is_WPS_enrollee\n"));
					return error;
				}
			} else {
				if ((error = dev_wlc_intvar_set(dev, "is_WPS_enrollee", FALSE))) {
					WL_WSEC(("Failed to clear iovar is_WPS_enrollee\n"));
					return error;
				}
			}
		}
		break;
	}


#endif 


	default:
		break;
	}
	return 0;
}
#define VAL_PSK(_val) (((_val) & WPA_AUTH_PSK) || ((_val) & WPA2_AUTH_PSK))

static int
wl_iw_get_wpaauth(
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_param *vwrq,
	char *extra
)
{
	int error;
	int paramid;
	int paramval = 0;
	int val;
	wl_iw_t *iw = IW_DEV_IF(dev);

	WL_TRACE(("%s: SIOCGIWAUTH\n", dev->name));

	paramid = vwrq->flags & IW_AUTH_INDEX;

	switch (paramid) {
	case IW_AUTH_WPA_VERSION:
		
		if ((error = dev_wlc_intvar_get(dev, "wpa_auth", &val)))
			return error;
		if (val & (WPA_AUTH_NONE | WPA_AUTH_DISABLED))
			paramval = IW_AUTH_WPA_VERSION_DISABLED;
		else if (val & (WPA_AUTH_PSK | WPA_AUTH_UNSPECIFIED))
			paramval = IW_AUTH_WPA_VERSION_WPA;
		else if (val & (WPA2_AUTH_PSK | WPA2_AUTH_UNSPECIFIED))
			paramval = IW_AUTH_WPA_VERSION_WPA2;
		break;

	case IW_AUTH_CIPHER_PAIRWISE:
		paramval = iw->pwsec;
		break;

	case IW_AUTH_CIPHER_GROUP:
		paramval = iw->gwsec;
		break;

	case IW_AUTH_KEY_MGMT:
		
		if ((error = dev_wlc_intvar_get(dev, "wpa_auth", &val)))
			return error;
		if (VAL_PSK(val))
			paramval = IW_AUTH_KEY_MGMT_PSK;
		else
			paramval = IW_AUTH_KEY_MGMT_802_1X;

		break;
	case IW_AUTH_TKIP_COUNTERMEASURES:
		dev_wlc_bufvar_get(dev, "tkip_countermeasures", (char *)&paramval, 1);
		break;

	case IW_AUTH_DROP_UNENCRYPTED:
		dev_wlc_bufvar_get(dev, "wsec_restrict", (char *)&paramval, 1);
		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		dev_wlc_bufvar_get(dev, "rx_unencrypted_eapol", (char *)&paramval, 1);
		break;

	case IW_AUTH_80211_AUTH_ALG:
		
		if ((error = dev_wlc_intvar_get(dev, "auth", &val)))
			return error;
		if (!val)
			paramval = IW_AUTH_ALG_OPEN_SYSTEM;
		else
			paramval = IW_AUTH_ALG_SHARED_KEY;
		break;
	case IW_AUTH_WPA_ENABLED:
		if ((error = dev_wlc_intvar_get(dev, "wpa_auth", &val)))
			return error;
		if (val)
			paramval = TRUE;
		else
			paramval = FALSE;
		break;

#if WIRELESS_EXT > 17

	case IW_AUTH_ROAMING_CONTROL:
		WL_ERROR(("%s: IW_AUTH_ROAMING_CONTROL\n", __FUNCTION__));
		
		break;

	case IW_AUTH_PRIVACY_INVOKED:
		paramval = iw->privacy_invoked;
		break;

#endif 
	}
	vwrq->value = paramval;
	return 0;
}
#endif 

static const iw_handler wl_iw_handler[] =
{
	(iw_handler) wl_iw_config_commit,	
	(iw_handler) wl_iw_get_name,		
	(iw_handler) NULL,			
	(iw_handler) NULL,			
	(iw_handler) wl_iw_set_freq,		
	(iw_handler) wl_iw_get_freq,		
	(iw_handler) wl_iw_set_mode,		
	(iw_handler) wl_iw_get_mode,		
	(iw_handler) NULL,			
	(iw_handler) NULL,			
	(iw_handler) NULL,			
	(iw_handler) wl_iw_get_range,		
    (iw_handler) wl_iw_set_priv,
	(iw_handler) NULL,			
	(iw_handler) NULL,			
	(iw_handler) NULL,			
	(iw_handler) wl_iw_set_spy,		
	(iw_handler) wl_iw_get_spy,		
	(iw_handler) NULL,			
	(iw_handler) NULL,			
	(iw_handler) wl_iw_set_wap,		
	(iw_handler) wl_iw_get_wap,		
#if WIRELESS_EXT > 17
	(iw_handler) wl_iw_mlme,		
#else
	(iw_handler) NULL,			
#endif
	(iw_handler) wl_iw_iscan_get_aplist,	
#if WIRELESS_EXT > 13
	(iw_handler) wl_iw_iscan_set_scan,	
	(iw_handler) wl_iw_iscan_get_scan,	
#else	
	(iw_handler) NULL,			
	(iw_handler) NULL,			
#endif	
	(iw_handler) wl_iw_set_essid,		
	(iw_handler) wl_iw_get_essid,		
	(iw_handler) wl_iw_set_nick,		
	(iw_handler) wl_iw_get_nick,		
	(iw_handler) NULL,			
	(iw_handler) NULL,			
	(iw_handler) wl_iw_set_rate,		
	(iw_handler) wl_iw_get_rate,		
	(iw_handler) wl_iw_set_rts,		
	(iw_handler) wl_iw_get_rts,		
	(iw_handler) wl_iw_set_frag,		
	(iw_handler) wl_iw_get_frag,		
	(iw_handler) wl_iw_set_txpow,		
	(iw_handler) wl_iw_get_txpow,		
#if WIRELESS_EXT > 10
	(iw_handler) wl_iw_set_retry,		
	(iw_handler) wl_iw_get_retry,		
#endif 
	(iw_handler) wl_iw_set_encode,		
	(iw_handler) wl_iw_get_encode,		
	(iw_handler) wl_iw_set_power,		
	(iw_handler) wl_iw_get_power,		
#if WIRELESS_EXT > 17
	(iw_handler) NULL,			
	(iw_handler) NULL,			
	(iw_handler) wl_iw_set_wpaie,		
	(iw_handler) wl_iw_get_wpaie,		
	(iw_handler) wl_iw_set_wpaauth,		
	(iw_handler) wl_iw_get_wpaauth,		
	(iw_handler) wl_iw_set_encodeext,	
	(iw_handler) wl_iw_get_encodeext,	
	(iw_handler) wl_iw_set_pmksa,		
#endif 
};

#if WIRELESS_EXT > 12
enum {
	WL_IW_SET_LEDDC = SIOCIWFIRSTPRIV,
	WL_IW_SET_VLANMODE,
    WL_IW_SET_PM,
    WL_COMBO_SCAN
};

static iw_handler wl_iw_priv_handler[] = {
	wl_iw_set_leddc,
	wl_iw_set_vlanmode,
    wl_iw_set_pm,
    (iw_handler)iwpriv_set_cscan
};

static struct iw_priv_args wl_iw_priv_args[] = {
	{
		WL_IW_SET_LEDDC,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		0,
		"set_leddc"
	},
	{
		WL_IW_SET_VLANMODE,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		0,
		"set_vlanmode"
	},
	{
		WL_IW_SET_PM,
		IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
		0,
		"set_pm"
    },
    { 
        WL_COMBO_SCAN,
        IW_PRIV_TYPE_CHAR | 1024,  
        0,
        "CSCAN"
    },
};

const struct iw_handler_def wl_iw_handler_def =
{
	.num_standard = ARRAYSIZE(wl_iw_handler),
	.num_private = ARRAY_SIZE(wl_iw_priv_handler),
	.num_private_args = ARRAY_SIZE(wl_iw_priv_args),
	.standard = (iw_handler *) wl_iw_handler,
	.private = wl_iw_priv_handler,
	.private_args = wl_iw_priv_args,
#if WIRELESS_EXT >= 19
	get_wireless_stats: dhd_get_wireless_stats,
#endif 
	};
EXPORT_SYMBOL(wl_iw_handler_def);
#endif 

int
wl_iw_ioctl(
	struct net_device *dev,
	struct ifreq *rq,
	int cmd
)
{
	struct iwreq *wrq = (struct iwreq *) rq;
	struct iw_request_info info;
	iw_handler handler;
	char *extra = NULL;
	size_t token_size = 1;
	int max_tokens = 0, ret = 0;

	if (cmd < SIOCIWFIRST ||
		IW_IOCTL_IDX(cmd) >= ARRAYSIZE(wl_iw_handler) ||
		!(handler = wl_iw_handler[IW_IOCTL_IDX(cmd)]))
		return -EOPNOTSUPP;

	switch (cmd) {

	case SIOCSIWESSID:
	case SIOCGIWESSID:
	case SIOCSIWNICKN:
	case SIOCGIWNICKN:
		max_tokens = IW_ESSID_MAX_SIZE + 1;
		break;

	case SIOCSIWENCODE:
	case SIOCGIWENCODE:
#if WIRELESS_EXT > 17
	case SIOCSIWENCODEEXT:
	case SIOCGIWENCODEEXT:
#endif
		max_tokens = IW_ENCODING_TOKEN_MAX;
		break;

	case SIOCGIWRANGE:
		max_tokens = sizeof(struct iw_range);
		break;

	case SIOCGIWAPLIST:
		token_size = sizeof(struct sockaddr) + sizeof(struct iw_quality);
		max_tokens = IW_MAX_AP;
		break;

#if WIRELESS_EXT > 13
	case SIOCGIWSCAN:
	if (g_iscan)
		max_tokens = wrq->u.data.length;
	else
		max_tokens = IW_SCAN_MAX_DATA;
		break;
#endif 

	case SIOCSIWSPY:
		token_size = sizeof(struct sockaddr);
		max_tokens = IW_MAX_SPY;
		break;

	case SIOCGIWSPY:
		token_size = sizeof(struct sockaddr) + sizeof(struct iw_quality);
		max_tokens = IW_MAX_SPY;
		break;
	default:
		break;
	}

	if (max_tokens && wrq->u.data.pointer) {
		if (wrq->u.data.length > max_tokens)
			return -E2BIG;

		if (!(extra = kmalloc(max_tokens * token_size, GFP_KERNEL)))
			return -ENOMEM;

		if (copy_from_user(extra, wrq->u.data.pointer, wrq->u.data.length * token_size)) {
			kfree(extra);
			return -EFAULT;
		}
	}

	info.cmd = cmd;
	info.flags = 0;

	ret = handler(dev, &info, &wrq->u, extra);

	if (extra) {
		if (copy_to_user(wrq->u.data.pointer, extra, wrq->u.data.length * token_size)) {
			kfree(extra);
			return -EFAULT;
		}

		kfree(extra);
	}

	return ret;
}
EXPORT_SYMBOL(wl_iw_ioctl);


bool
wl_iw_conn_status_str(uint32 event_type, uint32 status, uint32 reason,
	char* stringBuf, uint buflen)
{
	typedef struct conn_fail_event_map_t {
		uint32 inEvent;			
		uint32 inStatus;		
		uint32 inReason;		
		const char* outName;	
		const char* outCause;	
	} conn_fail_event_map_t;

	
#	define WL_IW_DONT_CARE	9999
	const conn_fail_event_map_t event_map [] = {
		
		
		{WLC_E_SET_SSID,     WLC_E_STATUS_SUCCESS,   WL_IW_DONT_CARE,
		"Conn", "Success"},
		{WLC_E_SET_SSID,     WLC_E_STATUS_NO_NETWORKS, WL_IW_DONT_CARE,
		"Conn", "NoNetworks"},
		{WLC_E_SET_SSID,     WLC_E_STATUS_FAIL,      WL_IW_DONT_CARE,
		"Conn", "ConfigMismatch"},
		{WLC_E_PRUNE,        WL_IW_DONT_CARE,        WLC_E_PRUNE_ENCR_MISMATCH,
		"Conn", "EncrypMismatch"},
		{WLC_E_PRUNE,        WL_IW_DONT_CARE,        WLC_E_RSN_MISMATCH,
		"Conn", "RsnMismatch"},
		{WLC_E_AUTH,         WLC_E_STATUS_TIMEOUT,   WL_IW_DONT_CARE,
		"Conn", "AuthTimeout"},
		{WLC_E_AUTH,         WLC_E_STATUS_FAIL,      WL_IW_DONT_CARE,
		"Conn", "AuthFail"},
		{WLC_E_AUTH,         WLC_E_STATUS_NO_ACK,    WL_IW_DONT_CARE,
		"Conn", "AuthNoAck"},
		{WLC_E_REASSOC,      WLC_E_STATUS_FAIL,      WL_IW_DONT_CARE,
		"Conn", "ReassocFail"},
		{WLC_E_REASSOC,      WLC_E_STATUS_TIMEOUT,   WL_IW_DONT_CARE,
		"Conn", "ReassocTimeout"},
		{WLC_E_REASSOC,      WLC_E_STATUS_ABORT,     WL_IW_DONT_CARE,
		"Conn", "ReassocAbort"},
		{WLC_E_PSK_SUP,      WLC_SUP_KEYED,          WL_IW_DONT_CARE,
		"Sup", "ConnSuccess"},
		{WLC_E_PSK_SUP,      WL_IW_DONT_CARE,        WL_IW_DONT_CARE,
		"Sup", "WpaHandshakeFail"},
		{WLC_E_DEAUTH_IND,   WL_IW_DONT_CARE,        WL_IW_DONT_CARE,
		"Conn", "Deauth"},
		{WLC_E_DISASSOC_IND, WL_IW_DONT_CARE,        WL_IW_DONT_CARE,
		"Conn", "DisassocInd"},
		{WLC_E_DISASSOC,     WL_IW_DONT_CARE,        WL_IW_DONT_CARE,
		"Conn", "Disassoc"}
	};

	const char* name = "";
	const char* cause = NULL;
	int i;

	
	for (i = 0;  i < sizeof(event_map)/sizeof(event_map[0]);  i++) {
		const conn_fail_event_map_t* row = &event_map[i];
		if (row->inEvent == event_type &&
		    (row->inStatus == status || row->inStatus == WL_IW_DONT_CARE) &&
		    (row->inReason == reason || row->inReason == WL_IW_DONT_CARE)) {
			name = row->outName;
			cause = row->outCause;
			break;
		}
	}

	
	if (cause) {
		memset(stringBuf, 0, buflen);
		snprintf(stringBuf, buflen, "%s %s %02d %02d",
			name, cause, status, reason);
		WL_TRACE(("Connection status: %s\n", stringBuf));
		return TRUE;
	} else {
		return FALSE;
	}
}

#if (WIRELESS_EXT > 14)

static bool
wl_iw_check_conn_fail(wl_event_msg_t *e, char* stringBuf, uint buflen)
{
	uint32 event = ntoh32(e->event_type);
	uint32 status =  ntoh32(e->status);
	uint32 reason =  ntoh32(e->reason);

	if (wl_iw_conn_status_str(event, status, reason, stringBuf, buflen)) {
		return TRUE;
	} else
	{
		return FALSE;
	}
}
#endif 

#ifndef IW_CUSTOM_MAX
#define IW_CUSTOM_MAX 256 
#endif 

void
wl_iw_event(struct net_device *dev, wl_event_msg_t *e, void* data)
{
#if WIRELESS_EXT > 13
	union iwreq_data wrqu;
	char extra[IW_CUSTOM_MAX + 1];
	int cmd = 0;
	uint32 event_type = ntoh32(e->event_type);
	uint16 flags =  ntoh16(e->flags);
	uint32 datalen = ntoh32(e->datalen);
	uint32 status =  ntoh32(e->status);

	memset(&wrqu, 0, sizeof(wrqu));
	memset(extra, 0, sizeof(extra));

	memcpy(wrqu.addr.sa_data, &e->addr, ETHER_ADDR_LEN);
	wrqu.addr.sa_family = ARPHRD_ETHER;

	switch (event_type) {
	case WLC_E_TXFAIL:
		cmd = IWEVTXDROP;
		break;
#if WIRELESS_EXT > 14
	case WLC_E_JOIN:
	case WLC_E_ASSOC_IND:
	case WLC_E_REASSOC_IND:
		cmd = IWEVREGISTERED;
		break;
	case WLC_E_DEAUTH_IND:
	case WLC_E_DISASSOC_IND:
		cmd = SIOCGIWAP;
		wrqu.data.length = strlen(extra);
		bzero(wrqu.addr.sa_data, ETHER_ADDR_LEN);
		bzero(&extra, ETHER_ADDR_LEN);
		break;

	case WLC_E_LINK:
	case WLC_E_NDIS_LINK:
		cmd = SIOCGIWAP;
		wrqu.data.length = strlen(extra);
		if (!(flags & WLC_EVENT_MSG_LINK)) {
			bzero(wrqu.addr.sa_data, ETHER_ADDR_LEN);
			bzero(&extra, ETHER_ADDR_LEN);
		}
		break;
	case WLC_E_ACTION_FRAME:
		cmd = IWEVCUSTOM;
		if (datalen + 1 <= sizeof(extra)) {
			wrqu.data.length = datalen + 1;
			extra[0] = WLC_E_ACTION_FRAME;
			memcpy(&extra[1], data, datalen);
			WL_TRACE(("WLC_E_ACTION_FRAME len %d \n", wrqu.data.length));
		}
		break;

	case WLC_E_ACTION_FRAME_COMPLETE:
		cmd = IWEVCUSTOM;
		if (sizeof(status) + 1 <= sizeof(extra)) {
			wrqu.data.length = sizeof(status) + 1;
			extra[0] = WLC_E_ACTION_FRAME_COMPLETE;
			memcpy(&extra[1], &status, sizeof(status));
			WL_TRACE(("wl_iw_event status %d  \n", status));
		}
		break;
#endif 
#if WIRELESS_EXT > 17
	case WLC_E_MIC_ERROR: {
		struct	iw_michaelmicfailure  *micerrevt = (struct  iw_michaelmicfailure  *)&extra;
		cmd = IWEVMICHAELMICFAILURE;
		wrqu.data.length = sizeof(struct iw_michaelmicfailure);
		if (flags & WLC_EVENT_MSG_GROUP)
			micerrevt->flags |= IW_MICFAILURE_GROUP;
		else
			micerrevt->flags |= IW_MICFAILURE_PAIRWISE;
		memcpy(micerrevt->src_addr.sa_data, &e->addr, ETHER_ADDR_LEN);
		micerrevt->src_addr.sa_family = ARPHRD_ETHER;

		break;
	}

	case WLC_E_ASSOC_REQ_IE:
		cmd = IWEVASSOCREQIE;
		wrqu.data.length = datalen;
		if (datalen < sizeof(extra))
			memcpy(extra, data, datalen);
		break;

	case WLC_E_ASSOC_RESP_IE:
		cmd = IWEVASSOCRESPIE;
		wrqu.data.length = datalen;
		if (datalen < sizeof(extra))
			memcpy(extra, data, datalen);
		break;

	case WLC_E_PMKID_CACHE: {
		struct iw_pmkid_cand *iwpmkidcand = (struct iw_pmkid_cand *)&extra;
		pmkid_cand_list_t *pmkcandlist;
		pmkid_cand_t	*pmkidcand;
		int count;

		if (data == NULL)
			break;

		cmd = IWEVPMKIDCAND;
		pmkcandlist = data;
		count = ntoh32_ua((uint8 *)&pmkcandlist->npmkid_cand);
		wrqu.data.length = sizeof(struct iw_pmkid_cand);
		pmkidcand = pmkcandlist->pmkid_cand;
		while (count) {
			bzero(iwpmkidcand, sizeof(struct iw_pmkid_cand));
			if (pmkidcand->preauth)
				iwpmkidcand->flags |= IW_PMKID_CAND_PREAUTH;
			bcopy(&pmkidcand->BSSID, &iwpmkidcand->bssid.sa_data,
			      ETHER_ADDR_LEN);
			wireless_send_event(dev, cmd, &wrqu, extra);
			pmkidcand++;
			count--;
		}
		break;
	}
#endif 

	case WLC_E_SCAN_COMPLETE:
#if WIRELESS_EXT > 14
		cmd = SIOCGIWSCAN;
#endif
		WL_TRACE(("event WLC_E_SCAN_COMPLETE\n"));
		if ((g_iscan) && (g_iscan->sysioc_pid >= 0) &&
			(g_iscan->iscan_state != ISCAN_STATE_IDLE))
			up(&g_iscan->sysioc_sem);
		break;

	default:
		
		break;
	}

	if (cmd) {
		if (cmd == SIOCGIWSCAN)
			wireless_send_event(dev, cmd, &wrqu, NULL);
		else
			wireless_send_event(dev, cmd, &wrqu, extra);
	}

#if WIRELESS_EXT > 14
	
	memset(extra, 0, sizeof(extra));
	if (wl_iw_check_conn_fail(e, extra, sizeof(extra))) {
		cmd = IWEVCUSTOM;
		wrqu.data.length = strlen(extra);
		wireless_send_event(dev, cmd, &wrqu, extra);
	}
#endif 

#endif 
}

EXPORT_SYMBOL(wl_iw_event);

int wl_iw_get_wireless_stats(struct net_device *dev, struct iw_statistics *wstats)
{
	int res = 0;
	wl_cnt_t cnt;
	int phy_noise;
	int rssi;
	scb_val_t scb_val;

	phy_noise = 0;
	if ((res = dev_wlc_ioctl(dev, WLC_GET_PHY_NOISE, &phy_noise, sizeof(phy_noise))))
		goto done;

	phy_noise = dtoh32(phy_noise);
	WL_TRACE(("wl_iw_get_wireless_stats phy noise=%d\n *****", phy_noise));

	scb_val.val = 0;
	if ((res = dev_wlc_ioctl(dev, WLC_GET_RSSI, &scb_val, sizeof(scb_val_t))))
		goto done;

	rssi = dtoh32(scb_val.val);
	WL_TRACE(("wl_iw_get_wireless_stats rssi=%d ****** \n", rssi));
	if (rssi <= WL_IW_RSSI_NO_SIGNAL)
		wstats->qual.qual = 0;
	else if (rssi <= WL_IW_RSSI_VERY_LOW)
		wstats->qual.qual = 1;
	else if (rssi <= WL_IW_RSSI_LOW)
		wstats->qual.qual = 2;
	else if (rssi <= WL_IW_RSSI_GOOD)
		wstats->qual.qual = 3;
	else if (rssi <= WL_IW_RSSI_VERY_GOOD)
		wstats->qual.qual = 4;
	else
		wstats->qual.qual = 5;

	
	wstats->qual.level = 0x100 + rssi;
	wstats->qual.noise = 0x100 + phy_noise;
#if WIRELESS_EXT > 18
	wstats->qual.updated |= (IW_QUAL_ALL_UPDATED | IW_QUAL_DBM);
#else
	wstats->qual.updated |= 7;
#endif 

#if WIRELESS_EXT > 11
	WL_TRACE(("wl_iw_get_wireless_stats counters=%d\n *****", (int)sizeof(wl_cnt_t)));

	memset(&cnt, 0, sizeof(wl_cnt_t));
	res = dev_wlc_bufvar_get(dev, "counters", (char *)&cnt, sizeof(wl_cnt_t));
	if (res)
	{
		WL_ERROR(("wl_iw_get_wireless_stats counters failed error=%d ****** \n", res));
		goto done;
	}

	cnt.version = dtoh16(cnt.version);
	if (cnt.version != WL_CNT_T_VERSION) {
		WL_TRACE(("\tIncorrect version of counters struct: expected %d; got %d\n",
			WL_CNT_T_VERSION, cnt.version));
		goto done;
	}

	wstats->discard.nwid = 0;
	wstats->discard.code = dtoh32(cnt.rxundec);
	wstats->discard.fragment = dtoh32(cnt.rxfragerr);
	wstats->discard.retries = dtoh32(cnt.txfail);
	wstats->discard.misc = dtoh32(cnt.rxrunt) + dtoh32(cnt.rxgiant);
	wstats->miss.beacon = 0;

	WL_TRACE(("wl_iw_get_wireless_stats counters txframe=%d txbyte=%d\n",
		dtoh32(cnt.txframe), dtoh32(cnt.txbyte)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxfrmtoolong=%d\n", dtoh32(cnt.rxfrmtoolong)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxbadplcp=%d\n", dtoh32(cnt.rxbadplcp)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxundec=%d\n", dtoh32(cnt.rxundec)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxfragerr=%d\n", dtoh32(cnt.rxfragerr)));
	WL_TRACE(("wl_iw_get_wireless_stats counters txfail=%d\n", dtoh32(cnt.txfail)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxrunt=%d\n", dtoh32(cnt.rxrunt)));
	WL_TRACE(("wl_iw_get_wireless_stats counters rxgiant=%d\n", dtoh32(cnt.rxgiant)));

#endif 

done:
	return res;
}
EXPORT_SYMBOL(wl_iw_get_wireless_stats);

static void
wl_iw_timerfunc(ulong data)
{
	iscan_info_t *iscan = (iscan_info_t *)data;
	iscan->timer_on = 0;
	if (iscan->iscan_state != ISCAN_STATE_IDLE) {
		WL_TRACE(("timer trigger\n"));
        iscan->iscan_timer_expired = 1;
		up(&iscan->sysioc_sem);
	}
}

static void
wl_iw_set_event_mask(struct net_device *dev)
{
	char eventmask[WL_EVENTING_MASK_LEN];
	char iovbuf[WL_EVENTING_MASK_LEN + 12];	

	dev_iw_iovar_getbuf(dev, "event_msgs", "", 0, iovbuf, sizeof(iovbuf));
	bcopy(iovbuf, eventmask, WL_EVENTING_MASK_LEN);
	setbit(eventmask, WLC_E_SCAN_COMPLETE);
	dev_iw_iovar_setbuf(dev, "event_msgs", eventmask, WL_EVENTING_MASK_LEN,
		iovbuf, sizeof(iovbuf));

}

static int
wl_iw_iscan_prep(wl_scan_params_t *params, wlc_ssid_t *ssid)
{
	int err = 0;

	memcpy(&params->bssid, &ether_bcast, ETHER_ADDR_LEN);
	params->bss_type = DOT11_BSSTYPE_ANY;
	params->scan_type = 0;
	params->nprobes = -1;
	params->active_time = -1;
	params->passive_time = -1;
	params->home_time = -1;
	params->channel_num = 0;

	params->nprobes = htod32(params->nprobes);
	params->active_time = htod32(params->active_time);
	params->passive_time = htod32(params->passive_time);
	params->home_time = htod32(params->home_time);
	if (ssid && ssid->SSID_len)
		memcpy(&params->ssid, ssid, sizeof(wlc_ssid_t));

	return err;
}

static int
wl_iw_iscan(iscan_info_t *iscan, wlc_ssid_t *ssid, uint16 action)
{
	int params_size = (WL_SCAN_PARAMS_FIXED_SIZE + OFFSETOF(wl_iscan_params_t, params));
	wl_iscan_params_t *params;
	int err = 0;

	if (ssid && ssid->SSID_len) {
		params_size += sizeof(wlc_ssid_t);
	}
	params = (wl_iscan_params_t*)kmalloc(params_size, GFP_KERNEL);
	if (params == NULL) {
		return -ENOMEM;
	}
	memset(params, 0, params_size);
	ASSERT(params_size < WLC_IOCTL_SMLEN);

	err = wl_iw_iscan_prep(&params->params, ssid);

	if (!err) {
		params->version = htod32(ISCAN_REQ_VERSION);
		params->action = htod16(action);
		params->scan_duration = htod16(0);

		
		(void) dev_iw_iovar_setbuf(iscan->dev, "iscan", params, params_size,
			iscan->ioctlbuf, WLC_IOCTL_SMLEN);
	}

	kfree(params);
	return err;
}

static uint32
wl_iw_iscan_get(iscan_info_t *iscan)
{
	iscan_buf_t * buf;
	iscan_buf_t * ptr;
	wl_iscan_results_t * list_buf;
	wl_iscan_results_t list;
	wl_scan_results_t *results;
	uint32 status;

	
	if (iscan->list_cur) {
		buf = iscan->list_cur;
		iscan->list_cur = buf->next;
	}
	else {
		buf = kmalloc(sizeof(iscan_buf_t), GFP_KERNEL);
		if (!buf)
			return WL_SCAN_RESULTS_ABORTED;
		buf->next = NULL;
		if (!iscan->list_hdr)
			iscan->list_hdr = buf;
		else {
			ptr = iscan->list_hdr;
			while (ptr->next) {
				ptr = ptr->next;
			}
			ptr->next = buf;
		}
	}
	memset(buf->iscan_buf, 0, WLC_IW_ISCAN_MAXLEN);
	list_buf = (wl_iscan_results_t*)buf->iscan_buf;
	results = &list_buf->results;
	results->buflen = WL_ISCAN_RESULTS_FIXED_SIZE;
	results->version = 0;
	results->count = 0;

	memset(&list, 0, sizeof(list));
	list.results.buflen = htod32(WLC_IW_ISCAN_MAXLEN);
	(void) dev_iw_iovar_getbuf(
		iscan->dev,
		"iscanresults",
		&list,
		WL_ISCAN_RESULTS_FIXED_SIZE,
		buf->iscan_buf,
		WLC_IW_ISCAN_MAXLEN);
	results->buflen = dtoh32(results->buflen);
	results->version = dtoh32(results->version);
	results->count = dtoh32(results->count);
	WL_TRACE(("results->count = %d\n", results->count));

	WL_TRACE(("results->buflen = %d\n", results->buflen));
	status = dtoh32(list_buf->status);
	return status;
}

static void wl_iw_send_scan_complete(iscan_info_t *iscan)
{
	union iwreq_data wrqu;

	memset(&wrqu, 0, sizeof(wrqu));

	
	wireless_send_event(iscan->dev, SIOCGIWSCAN, &wrqu, NULL);
}

static int
_iscan_sysioc_thread(void *data)
{
	uint32 status;
	iscan_info_t *iscan = (iscan_info_t *)data;

	DAEMONIZE("iscan_sysioc");

	status = WL_SCAN_RESULTS_PARTIAL;
	while (down_interruptible(&iscan->sysioc_sem) == 0) {
		if (iscan->timer_on) {
			del_timer(&iscan->timer);
			iscan->timer_on = 0;
		}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
		rtnl_lock();
#endif
        if ( iscan->iscan_timer_expired ){
            iscan->iscan_timer_expired = 0;
            wl_iw_iscan(iscan, NULL, WL_SCAN_ACTION_ABORT);
        }
		status = wl_iw_iscan_get(iscan);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
		rtnl_unlock();
#endif

		switch (status) {
			case WL_SCAN_RESULTS_PARTIAL:
				WL_TRACE(("iscanresults incomplete\n"));
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
				rtnl_lock();
#endif
				
				wl_iw_iscan(iscan, NULL, WL_SCAN_ACTION_CONTINUE);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
				rtnl_unlock();
#endif
				
				iscan->timer.expires = jiffies + iscan->timer_ms*HZ/1000;
				add_timer(&iscan->timer);
				iscan->timer_on = 1;
                iscan->iscan_timer_expired = 0;
				break;
			case WL_SCAN_RESULTS_SUCCESS:
				WL_TRACE(("iscanresults complete\n"));
				iscan->iscan_state = ISCAN_STATE_IDLE;
				wl_iw_send_scan_complete(iscan);
				break;
			case WL_SCAN_RESULTS_PENDING:
				WL_TRACE(("iscanresults pending\n"));
				
				iscan->timer.expires = jiffies + iscan->timer_ms*HZ/1000;
				add_timer(&iscan->timer);
				iscan->timer_on = 1;
                iscan->iscan_timer_expired = 0;
				break;
			case WL_SCAN_RESULTS_ABORTED:
				WL_TRACE(("iscanresults aborted\n"));
				iscan->iscan_state = ISCAN_STATE_IDLE;
				wl_iw_send_scan_complete(iscan);
				break;
			default:
				WL_TRACE(("iscanresults returned unknown status %d\n", status));
				break;
		 }
	}
    if (iscan->timer_on) {
        iscan->timer_on = 0;
        iscan->iscan_timer_expired = 0;
        del_timer_sync(&iscan->timer);
    }
	complete_and_exit(&iscan->sysioc_exited, 0);
}

int
wl_iw_attach(struct net_device *dev, void * dhdp)
{
	iscan_info_t *iscan = NULL;
    int params_size=0;

	if (!dev)
		return 0;

	iscan = kmalloc(sizeof(iscan_info_t), GFP_KERNEL);
	if (!iscan)
		return -ENOMEM;
	memset(iscan, 0, sizeof(iscan_info_t));
	iscan->sysioc_pid = -1;
    params_size = (WL_SCAN_PARAMS_FIXED_SIZE + OFFSETOF(wl_iscan_params_t, params)) +
        (WL_NUMCHANNELS * sizeof(uint16)) + WL_SCAN_PARAMS_SSID_MAX * sizeof(wlc_ssid_t);

    iscan->iscan_ex_params_p = (wl_iscan_params_t*)kmalloc(params_size, GFP_KERNEL);

    if (!iscan->iscan_ex_params_p) {
        kfree(iscan);
        DHD_ERROR(("Malloc for scan parameters failed.\n"));
        return -ENOMEM;
    }
    iscan->iscan_ex_param_size = params_size;
	g_iscan = iscan;
	iscan->dev = dev;
	iscan->iscan_state = ISCAN_STATE_IDLE;


    iscan->timer_ms    = 8000;
	init_timer(&iscan->timer);
	iscan->timer.data = (ulong)iscan;
	iscan->timer.function = wl_iw_timerfunc;

	sema_init(&iscan->sysioc_sem, 0);
	init_completion(&iscan->sysioc_exited);
	iscan->sysioc_pid = kernel_thread(_iscan_sysioc_thread, iscan, 0);
	if (iscan->sysioc_pid < 0)
		return -ENOMEM;
	return 0;
}
EXPORT_SYMBOL(wl_iw_attach);

void wl_iw_detach(void)
{
	iscan_buf_t  *buf;
	iscan_info_t *iscan = g_iscan;
	if (!iscan)
		return;
	if (iscan->sysioc_pid >= 0) {
		KILL_PROC(iscan->sysioc_pid, SIGTERM);
		wait_for_completion(&iscan->sysioc_exited);
	}

	while (iscan->list_hdr) {
		buf = iscan->list_hdr->next;
		kfree(iscan->list_hdr);
		iscan->list_hdr = buf;
	}
	kfree(iscan);
	g_iscan = NULL;
}
EXPORT_SYMBOL(wl_iw_detach);
#ifdef CSCAN
/* Android ComboSCAN support */

#define strtoul(nptr, endptr, base) bcm_strtoul((nptr), (endptr), (base))
extern void dhd_print_buf(void *pbuf, int len, int bytes_per_line);


#define PTYPE_STRING 0
#define PTYPE_INTDEC 1   
#define PTYPE_INTHEX 2
#define PTYPE_STR_HEX 3  
static int
hex2num(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

static int
hstr_2_buf(const char *txt, u8 *buf, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        int a, b;

        a = hex2num(*txt++);
        if (a < 0)
            return -1;
        b = hex2num(*txt++);
        if (b < 0)
            return -1;
        *buf++ = (a << 4) | b;
    }

    return 0;
}


static int
get_parameter_from_string(
            char **str_ptr, const char *token,
            int param_type, void  *dst, int param_max_len)
{
    char int_str[7] = "0";
    int parm_str_len;
    char  *param_str_begin;
    char  *param_str_end;
    char  *orig_str = *str_ptr;

    if ((*str_ptr) && !strncmp(*str_ptr, token, strlen(token))) {

        strsep(str_ptr, "=,"); 
        param_str_begin = *str_ptr;
        strsep(str_ptr, "=,"); 

        if (*str_ptr == NULL) {

            parm_str_len = strlen(param_str_begin);
        } else {
            param_str_end = *str_ptr-1;  
            parm_str_len = param_str_end - param_str_begin;
        }

        WL_TRACE((" 'token:%s', len:%d, ", token, parm_str_len));

        if (parm_str_len > param_max_len) {
            WL_ERROR((" WARNING: extracted param len:%d is > MAX:%d\n",
                parm_str_len, param_max_len));

            parm_str_len = param_max_len;
        }

        switch (param_type) {

            case PTYPE_INTDEC: {

                int *pdst_int = dst;
                char *eptr;

                if (parm_str_len > sizeof(int_str))
					 parm_str_len = sizeof(int_str);

                memcpy(int_str, param_str_begin, parm_str_len);

                *pdst_int = simple_strtoul(int_str, &eptr, 10);

                WL_TRACE((" written as integer:%d\n",  *pdst_int));
            }
            break;
            case PTYPE_STR_HEX: {
                u8 *buf = dst;

                param_max_len = param_max_len >> 1;  
                hstr_2_buf(param_str_begin, buf, param_max_len);
                dhd_print_buf(buf, param_max_len, 0);
            }
            break;
            default:

                memcpy(dst, param_str_begin, parm_str_len);
                *((char *)dst + parm_str_len) = 0; 
                WL_ERROR((" written as a string:%s\n", (char *)dst));
            break;

        }

        return 0;
    } else {
        WL_ERROR(("\n %s: ERROR: can't find token:%s in str:%s \n",
            __FUNCTION__, token, orig_str));

    return -1;
    }
}


/*
 *  data parsing from ComboScan tlv list
*/
int
wl_iw_parse_data_tlv(char** list_str, void *dst, int dst_size, const char token,
                     int input_size, int *bytes_left)
{
    char* str = *list_str;
    uint16 short_temp;
    uint32 int_temp;

    if ((list_str == NULL) || (*list_str == NULL) ||(bytes_left == NULL) || (*bytes_left < 0)) {
        DHD_ERROR(("%s error paramters\n", __FUNCTION__));
        return -1;
    }

    /* Clean all dest bytes */
    memset(dst, 0, dst_size);
    while (*bytes_left > 0) {

        if (str[0] != token) {
            DHD_TRACE(("%s NOT Type=%d get=%d left_parse=%d \n",
                __FUNCTION__, token, str[0], *bytes_left));
            return -1;
        }

        *bytes_left -= 1;
        str += 1;

        if (input_size == 1) {
            memcpy(dst, str, input_size);
        }
        else if (input_size == 2) {
            memcpy(dst, (char *)htod16(memcpy(&short_temp, str, input_size)),
                input_size);
        }
        else if (input_size == 4) {
            memcpy(dst, (char *)htod32(memcpy(&int_temp, str, input_size)),
                input_size);
        }

        *bytes_left -= input_size;
        str += input_size;
        *list_str = str;
        return 1;
    }
    return 1;
}

/*
 *  channel list parsing from cscan tlv list
*/
int
wl_iw_parse_channel_list_tlv(char** list_str, uint16* channel_list,
                             int channel_num, int *bytes_left)
{
    char* str = *list_str;
    int idx = 0;

    if ((list_str == NULL) || (*list_str == NULL) ||(bytes_left == NULL) || (*bytes_left < 0)) {
        DHD_ERROR(("%s error paramters\n", __FUNCTION__));
        return -1;
    }

    while (*bytes_left > 0) {

        if (str[0] != CSCAN_TLV_TYPE_CHANNEL_IE) {
            *list_str = str;
            DHD_TRACE(("End channel=%d left_parse=%d %d\n", idx, *bytes_left, str[0]));
            return idx;
        }
        /* Get proper CSCAN_TLV_TYPE_CHANNEL_IE */
        *bytes_left -= 1;
        str += 1;

        if (str[0] == 0) {
            /* All channels */
            channel_list[idx] = 0x0;
        }
        else {
            channel_list[idx] = (uint16)str[0];
            DHD_TRACE(("%s channel=%d \n", __FUNCTION__,  channel_list[idx]));
        }
        *bytes_left -= 1;
        str += 1;

        if (idx++ > 255) {
            DHD_ERROR(("%s Too many channels \n", __FUNCTION__));
            return -1;
        }
    }

    *list_str = str;
    return idx;
}

/*
 *  SSIDs list parsing from cscan tlv list
 */
int
wl_iw_parse_ssid_list_tlv(char** list_str, wlc_ssid_t* ssid, int max, int *bytes_left)
{
    char* str;
    int idx = 0;

    if ((list_str == NULL) || (*list_str == NULL) || (*bytes_left < 0)) {
        DHD_ERROR(("%s error paramters\n", __FUNCTION__));
        return -1;
    }
    str = *list_str;
    while (*bytes_left > 0) {

        if (str[0] != CSCAN_TLV_TYPE_SSID_IE) {
            *list_str = str;
            DHD_TRACE(("nssid=%d left_parse=%d %d\n", idx, *bytes_left, str[0]));
            return idx;
        }

        /* Get proper CSCAN_TLV_TYPE_SSID_IE */
        *bytes_left -= 1;
        str += 1;

        if (str[0] == 0) {
            /* Broadcast SSID */
            ssid[idx].SSID_len = 0;
            memset((char*)ssid[idx].SSID, 0x0, DOT11_MAX_SSID_LEN);
            *bytes_left -= 1;
            str += 1;

            DHD_TRACE(("BROADCAST SCAN  left=%d\n", *bytes_left));
        }
        else if (str[0] <= DOT11_MAX_SSID_LEN) {
            /* Get proper SSID size */
            ssid[idx].SSID_len = str[0];
            *bytes_left -= 1;
            str += 1;

            /* Get SSID */
            if (ssid[idx].SSID_len > *bytes_left) {
                DHD_ERROR(("%s out of memory range len=%d but left=%d\n",
                __FUNCTION__, ssid[idx].SSID_len, *bytes_left));
                return -1;
            }

            memcpy((char*)ssid[idx].SSID, str, ssid[idx].SSID_len);

            *bytes_left -= ssid[idx].SSID_len;
            str += ssid[idx].SSID_len;

            DHD_TRACE(("%s :size=%d left=%d\n",
                (char*)ssid[idx].SSID, ssid[idx].SSID_len, *bytes_left));
        }
        else {
            DHD_ERROR(("### SSID size more that %d\n", str[0]));
            return -1;
        }

        if (idx++ >  max) {
            DHD_ERROR(("%s number of SSIDs more that %d\n", __FUNCTION__, idx));
            return -1;
        }
    }

    *list_str = str;
    return idx;
}

/* Parse a comma-separated list from list_str into ssid array, starting
 * at index idx.  Max specifies size of the ssid array.  Parses ssids
 * and returns updated idx; if idx >= max not all fit, the excess have
 * not been copied.  Returns -1 on empty string, or on ssid too long.
 */
int
wl_iw_parse_ssid_list(char** list_str, wlc_ssid_t* ssid, int idx, int max)
{
    char* str, *ptr;

    if ((list_str == NULL) || (*list_str == NULL))
        return -1;

    for (str = *list_str; str != NULL; str = ptr) {

        /* check for next TAG */
        if (!strncmp(str, GET_CHANNEL, strlen(GET_CHANNEL))) {
            *list_str	 = str + strlen(GET_CHANNEL);
            return idx;
        }

        if ((ptr = strchr(str, ',')) != NULL) {
            *ptr++ = '\0';
        }

        if (strlen(str) > DOT11_MAX_SSID_LEN) {
            DHD_ERROR(("ssid <%s> exceeds %d\n", str, DOT11_MAX_SSID_LEN));
            return -1;
        }

        if (strlen(str) == 0)
            ssid[idx].SSID_len = 0;

        if (idx < max) {
            bcm_strcpy_s((char*)ssid[idx].SSID, sizeof(ssid[idx].SSID), str);
            ssid[idx].SSID_len = strlen(str);
        }
        idx++;
    }
    return idx;
}

/*
 * Parse channel list from iwpriv CSCAN
 */
int
wl_iw_parse_channel_list(char** list_str, uint16* channel_list, int channel_num)
{
    int num;
    int val;
    char* str;
    char* endptr = NULL;

    if ((list_str == NULL)||(*list_str == NULL))
        return -1;

    str = *list_str;
    num = 0;
    while (strncmp(str, GET_NPROBE, strlen(GET_NPROBE))) {
        val = (int)strtoul(str, &endptr, 0);
        if (endptr == str) {
            printf("could not parse channel number starting at"
                " substring \"%s\" in list:\n%s\n",
                str, *list_str);
            return -1;
        }
        str = endptr + strspn(endptr, " ,");

        if (num == channel_num) {
            DHD_ERROR(("too many channels (more than %d) in channel list:\n%s\n",
                channel_num, *list_str));
            return -1;
        }

        channel_list[num++] = (uint16)val;
    }
    *list_str = str;
    return num;
}

static int
wl_iw_combined_scan_set(struct net_device *dev, wlc_ssid_t* ssids_local, int nssid, int nchan)
{
    int params_size = WL_SCAN_PARAMS_FIXED_SIZE + WL_NUMCHANNELS * sizeof(uint16);
    int err = 0;
    char *p;
    int i;
    iscan_info_t *iscan = g_iscan;

    WL_TRACE(("%s nssid=%d nchan=%d\n", __FUNCTION__, nssid, nchan));

    if ((!dev) && (!g_iscan)) {
        WL_ERROR(("%s error exit\n", __FUNCTION__));
        err = -1;
        goto exit;
    }

#ifdef PNO_SUPPORT

    if  (dhd_dev_get_pno_status(dev)) {
        WL_ERROR(("%s: Scan called when PNO is active\n", __FUNCTION__));
    }
#endif 

    params_size += WL_SCAN_PARAMS_SSID_MAX * sizeof(wlc_ssid_t);


    if (nssid > 0) {
        i = OFFSETOF(wl_scan_params_t, channel_list) + nchan * sizeof(uint16);
        i = ROUNDUP(i, sizeof(uint32));
        if (i + nssid * sizeof(wlc_ssid_t) > params_size) {
            printf("additional ssids exceed params_size\n");
            err = -1;
            goto exit;
        }

        p = ((char*)&iscan->iscan_ex_params_p->params) + i;
        memcpy(p, ssids_local, nssid * sizeof(wlc_ssid_t));
        p += nssid * sizeof(wlc_ssid_t);
    } else {
        p = (char*)iscan->iscan_ex_params_p->params.channel_list + nchan * sizeof(uint16);
    }


    iscan->iscan_ex_params_p->params.channel_num =
            htod32((nssid << WL_SCAN_PARAMS_NSSID_SHIFT) |
                   (nchan & WL_SCAN_PARAMS_COUNT_MASK));

    nssid = (uint)
            ((iscan->iscan_ex_params_p->params.channel_num >> WL_SCAN_PARAMS_NSSID_SHIFT) &
             WL_SCAN_PARAMS_COUNT_MASK);


    params_size = (int) (p - (char*)iscan->iscan_ex_params_p + nssid * sizeof(wlc_ssid_t));
    iscan->iscan_ex_param_size = params_size;

    iscan->list_cur = iscan->list_hdr;
    iscan->iscan_state = ISCAN_STATE_SCANING;
    wl_iw_set_event_mask(dev);
    mod_timer(&iscan->timer, jiffies + iscan->timer_ms*HZ/1000);

    iscan->timer_on = 1;
    iscan->iscan_timer_expired = 0;

// #ifdef SCAN_DUMP
#ifdef CSCAN
    {
        int i;
        WL_SCAN(("\n### List of SSIDs to scan ###\n"));
        for (i = 0; i < nssid; i++) {
            if (!ssids_local[i].SSID_len)
                WL_SCAN(("%d: Broadcast scan\n", i));
            else
            WL_SCAN(("%d: scan  for  %s size =%d\n", i,
                ssids_local[i].SSID, ssids_local[i].SSID_len));
        }
        WL_SCAN(("### List of channels to scan ###\n"));
        for (i = 0; i < nchan; i++)
        {
            WL_SCAN(("%d ", iscan->iscan_ex_params_p->params.channel_list[i]));
        }
        WL_SCAN(("\nnprobes=%d\n", iscan->iscan_ex_params_p->params.nprobes));
        WL_SCAN(("active_time=%d\n", iscan->iscan_ex_params_p->params.active_time));
        WL_SCAN(("passive_time=%d\n", iscan->iscan_ex_params_p->params.passive_time));
        WL_SCAN(("home_time=%d\n", iscan->iscan_ex_params_p->params.home_time));
        WL_SCAN(("scan_type=%d\n", iscan->iscan_ex_params_p->params.scan_type));
        WL_SCAN(("\n###################\n"));
    }
#endif 

    if (params_size > WLC_IOCTL_MEDLEN) {
            WL_ERROR(("Set ISCAN for %s due to params_size=%d  \n",
                __FUNCTION__, params_size));
            err = -1;
    }

    if ((err = dev_iw_iovar_setbuf(dev, "iscan", iscan->iscan_ex_params_p,
                                   iscan->iscan_ex_param_size,
                                   iscan->ioctlbuf, sizeof(iscan->ioctlbuf)))) {
        WL_TRACE(("Set ISCAN for %s failed with %d\n", __FUNCTION__, err));
        err = -1;
    }

exit:
    return err;
}


static int
iwpriv_set_cscan(struct net_device *dev, struct iw_request_info *info,
                 union iwreq_data *wrqu, char *ext)
{
    int res;
    char  *extra = NULL;
    iscan_info_t *iscan = g_iscan;
    wlc_ssid_t ssids_local[WL_SCAN_PARAMS_SSID_MAX];
    int nssid = 0;
    int nchan = 0;
    char *str_ptr;

    if (wrqu->data.length == 0) {
        WL_ERROR(("IWPRIV argument len = 0\n"));
        return -EINVAL;
    }

    if (!iscan->iscan_ex_params_p) {
        return -EFAULT;
    }

    if (!(extra = kmalloc(wrqu->data.length+1, GFP_KERNEL)))
        return -ENOMEM;

    if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length)) {
        res = -EFAULT;
        goto exit_proc;
    }

    extra[wrqu->data.length] = 0;
    DHD_BTA(("%s: info->cmd:%x, info->flags:%x, u.data=0x%p, u.len=%d,star:%s\n",
        __FUNCTION__, info->cmd, info->flags,
        wrqu->data.pointer, wrqu->data.length,extra));

    str_ptr = extra;

    if (iscan->iscan_state == ISCAN_STATE_SCANING) {
        WL_TRACE(("%s ISCAN already in progress \n", __FUNCTION__));
        goto exit_proc;
    }

    if (strncmp(str_ptr, GET_SSID, strlen(GET_SSID))) {
        WL_ERROR(("%s Error: extracting SSID='' string\n", __FUNCTION__));
        res = -EINVAL;
        goto exit_proc;
    }

    str_ptr += strlen(GET_SSID);
    nssid = wl_iw_parse_ssid_list(&str_ptr, ssids_local, nssid,
                                  WL_SCAN_PARAMS_SSID_MAX);
    if (nssid == -1) {
        WL_ERROR(("%s wrong ssid list", __FUNCTION__));
        res = -EINVAL;
        goto exit_proc;
    }

    memset(iscan->iscan_ex_params_p, 0, iscan->iscan_ex_param_size);
    ASSERT(iscan->iscan_ex_param_size < WLC_IOCTL_MAXLEN);


    wl_iw_iscan_prep(&iscan->iscan_ex_params_p->params, NULL);
    iscan->iscan_ex_params_p->version = htod32(ISCAN_REQ_VERSION);
    iscan->iscan_ex_params_p->action = htod16(WL_SCAN_ACTION_START);
    iscan->iscan_ex_params_p->scan_duration = htod16(0);


    if ((nchan = wl_iw_parse_channel_list(&str_ptr,
                                          &iscan->iscan_ex_params_p->params.channel_list[0],
                                          WL_NUMCHANNELS)) == -1) {
        WL_ERROR(("%s missing channel list\n", __FUNCTION__));
        res = -EINVAL;
        goto exit_proc;
    }

    get_parameter_from_string(&str_ptr,
                              GET_NPROBE, PTYPE_INTDEC,
                              &iscan->iscan_ex_params_p->params.nprobes, 2);

    get_parameter_from_string(&str_ptr, GET_ACTIVE_ASSOC_DWELL, PTYPE_INTDEC,
                              &iscan->iscan_ex_params_p->params.active_time, 4);

    get_parameter_from_string(&str_ptr, GET_PASSIVE_ASSOC_DWELL, PTYPE_INTDEC,
                              &iscan->iscan_ex_params_p->params.passive_time, 4);

    get_parameter_from_string(&str_ptr, GET_HOME_DWELL, PTYPE_INTDEC,
                              &iscan->iscan_ex_params_p->params.home_time, 4);

    get_parameter_from_string(&str_ptr, GET_SCAN_TYPE, PTYPE_INTDEC,
                              &res, 1);

    iscan->iscan_ex_params_p->params.scan_type=(uint8)res;

    WL_ERROR(("Got my probe %d\n",iscan->iscan_ex_params_p->params.nprobes));
    res = wl_iw_combined_scan_set(dev, ssids_local, nssid, nchan);

exit_proc:
    kfree(extra);

    return res;
}

typedef struct cscan_tlv {
    char prefix;
    char version;
    char subver;
    char reserved;
} cscan_tlv_t;


static int
wl_iw_set_cscan(
    struct net_device *dev,
    struct iw_request_info *info,
    union iwreq_data *wrqu,
    char *extra
)
{
    int res = -1;
    iscan_info_t *iscan = g_iscan;
    wlc_ssid_t ssids_local[WL_SCAN_PARAMS_SSID_MAX];
    int nssid = 0;
    int nchan = 0;
    cscan_tlv_t *cscan_tlv_temp;
    char type;
    char *str_ptr;
    int tlv_size_left;
#ifdef TLV_DEBUG
    int i;
    char tlv_in_example[] = {
        'C', 'S', 'C', 'A', 'N', ' ',
        0x53, 0x01, 0x00, 0x00,
        'S',	  
        0x00, 
        'S',    
        0x04, 
        'B', 'R', 'C', 'M',
        'C',
        0x06, 
        'P', 
        0x94,
        0x11,
        'T',     
        0x01  
    };
#endif 

    WL_TRACE(("\n### %s: info->cmd:%x, info->flags:%x, u.data=0x%p, u.len=%d\n",
        __FUNCTION__, info->cmd, info->flags,
        wrqu->data.pointer, wrqu->data.length));

    net_os_wake_lock(dev);

	if (wrqu->data.length < (strlen(CSCAN_COMMAND) + sizeof(cscan_tlv_t))) {
        WL_ERROR(("%s argument=%d  less %d\n", __FUNCTION__,
                  wrqu->data.length, (int)(strlen(CSCAN_COMMAND) + sizeof(cscan_tlv_t))));
        return -1;
    }

#ifdef TLV_DEBUG
    memcpy(extra, tlv_in_example, sizeof(tlv_in_example));
    wrqu->data.length = sizeof(tlv_in_example);
    for (i = 0; i < wrqu->data.length; i++)
        printf("%02X ", extra[i]);
    printf("\n");
#endif 

    if (iscan->iscan_state == ISCAN_STATE_SCANING) {
        WL_TRACE(("%s ISCAN already in progress \n", __FUNCTION__));
        goto exit_proc;
    }

    str_ptr = extra;
    str_ptr +=  strlen(CSCAN_COMMAND);
    tlv_size_left = wrqu->data.length - strlen(CSCAN_COMMAND);

    cscan_tlv_temp = (cscan_tlv_t *)str_ptr;
    memset(ssids_local, 0, sizeof(ssids_local));

    if ((cscan_tlv_temp->prefix == CSCAN_TLV_PREFIX) &&
        (cscan_tlv_temp->version == CSCAN_TLV_VERSION) &&
        (cscan_tlv_temp->subver == CSCAN_TLV_SUBVERSION))
    {
        str_ptr += sizeof(cscan_tlv_t);
        tlv_size_left  -= sizeof(cscan_tlv_t);


        if ((nssid = wl_iw_parse_ssid_list_tlv(&str_ptr, ssids_local,
            WL_SCAN_PARAMS_SSID_MAX, &tlv_size_left)) <= 0) {
            WL_ERROR(("SSID is not presented or corrupted ret=%d\n", nssid));
            goto exit_proc;
        }
        else {

            memset(iscan->iscan_ex_params_p, 0, iscan->iscan_ex_param_size);


            wl_iw_iscan_prep(&iscan->iscan_ex_params_p->params, NULL);
            iscan->iscan_ex_params_p->version = htod32(ISCAN_REQ_VERSION);
            iscan->iscan_ex_params_p->action = htod16(WL_SCAN_ACTION_START);
            iscan->iscan_ex_params_p->scan_duration = htod16(0);


            while (tlv_size_left > 0)
            {
            type = str_ptr[0];
            switch (type) {
                case CSCAN_TLV_TYPE_CHANNEL_IE:

                    if ((nchan = wl_iw_parse_channel_list_tlv(&str_ptr,
                    &iscan->iscan_ex_params_p->params.channel_list[0],
                    WL_NUMCHANNELS, &tlv_size_left)) == -1) {
                    WL_ERROR(("%s missing channel list\n",
                        __FUNCTION__));
                        goto exit_proc;
                    }
                break;
                case CSCAN_TLV_TYPE_NPROBE_IE:
                    if ((res = wl_iw_parse_data_tlv(&str_ptr,
                        &iscan->iscan_ex_params_p->params.nprobes,
                        sizeof(iscan->iscan_ex_params_p->params.nprobes),
                        type, sizeof(char), &tlv_size_left)) == -1) {
                        WL_ERROR(("%s return %d\n",
                            __FUNCTION__, res));
                            goto exit_proc;
                    }
                    WL_ERROR(("Got all of it %d,raw:%s\n",iscan->iscan_ex_params_p->params.nprobes,str_ptr));
                break;
                case CSCAN_TLV_TYPE_ACTIVE_IE:
                    if ((res = wl_iw_parse_data_tlv(&str_ptr,
                    &iscan->iscan_ex_params_p->params.active_time,
                    sizeof(iscan->iscan_ex_params_p->params.active_time),
                    type, sizeof(short), &tlv_size_left)) == -1) {
                        WL_ERROR(("%s return %d\n",
                        __FUNCTION__, res));
                        goto exit_proc;
                    }
                break;
                case CSCAN_TLV_TYPE_PASSIVE_IE:
                    if ((res = wl_iw_parse_data_tlv(&str_ptr,
                    &iscan->iscan_ex_params_p->params.passive_time,
                    sizeof(iscan->iscan_ex_params_p->params.passive_time),
                    type, sizeof(short), &tlv_size_left)) == -1) {
                        WL_ERROR(("%s return %d\n",
                        __FUNCTION__, res));
                        goto exit_proc;
                    }
                break;
                case CSCAN_TLV_TYPE_HOME_IE:
                    if ((res = wl_iw_parse_data_tlv(&str_ptr,
                    &iscan->iscan_ex_params_p->params.home_time,
                    sizeof(iscan->iscan_ex_params_p->params.home_time),
                    type, sizeof(short), &tlv_size_left)) == -1) {
                        WL_ERROR(("%s return %d\n",
                        __FUNCTION__, res));
                        goto exit_proc;
                    }
                break;
                case CSCAN_TLV_TYPE_STYPE_IE:
                    if ((res = wl_iw_parse_data_tlv(&str_ptr,
                    &iscan->iscan_ex_params_p->params.scan_type,
                    sizeof(iscan->iscan_ex_params_p->params.scan_type),
                    type, sizeof(char), &tlv_size_left)) == -1) {
                    WL_ERROR(("%s return %d\n",
                        __FUNCTION__, res));
                        goto exit_proc;
                    }
                break;

                default :
                    WL_ERROR(("%s get unkwown type %X\n",
                    __FUNCTION__, type));
                    goto exit_proc;
                break;
                }
            } 
            }
        }
        else {
            WL_ERROR(("%s get wrong TLV command\n", __FUNCTION__));
            goto exit_proc;
        }

        res = wl_iw_combined_scan_set(dev, ssids_local, nssid, nchan);

exit_proc:
    net_os_wake_unlock(dev);
    return res;
}

#endif 




#endif 

