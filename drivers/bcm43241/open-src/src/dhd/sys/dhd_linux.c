/*
 * Broadcom Dongle Host Driver (DHD), Linux-specific network interface
 * Basically selected code segments from usb-cdc.c and usb-rndis.c
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
 * $Id: dhd_linux.c 348330 2012-08-01 14:27:31Z $
 */
/*****************************************************************************************
<���ⵥ��>        <��  ��>  <�޸�ʱ��> <�汾> <�޸�����>
******************************************************************************************/

#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/rtnetlink.h>
#include <linux/etherdevice.h>
#include <linux/random.h>
#include <linux/spinlock.h>
#include <linux/ethtool.h>
#include <linux/fcntl.h>
#include <linux/fs.h>

#include <asm/uaccess.h>
#include <asm/unaligned.h>

#include <epivers.h>
#include <bcmutils.h>
#include <bcmendian.h>
#include <bcmdevs.h>

#include <proto/ethernet.h>
#include <dngl_stats.h>
#include <dhd.h>
#include <dhd_bus.h>
#include <dhd_proto.h>
#include <dhd_dbg.h>
#if HNDCTF
#include <linux/if.h>
#include <linux/if_vlan.h>
#include <linux/fake/ctf/hndctf.h>
#include <linux/ip.h>
ctf_t* cih_dhd __attribute__ ((aligned(32))) = NULL;/* ctf instance handle */
osl_t *fake_osh_wifi = NULL;
#endif /* HNDCTF */
extern int g_ifContrlWan;
#ifdef CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>
#endif
#ifdef WL_CFG80211
#include <wl_cfg80211.h>
#endif

#ifdef WLBTAMP
#include <proto/802.11_bta.h>
#include <proto/bt_amp_hci.h>
#include <dhd_bta.h>
#endif

extern void wifi_get_nvmac_addr(unsigned char *s1);
static int set_mac_add1_or2(struct ether_addr *pmacaddr);
extern void set_get_sta_num_func(int func);
//added for dial on demand 2012-5-4
struct net_device * g_wifi_dev = NULL;

static int g_ap_sta_num = 0;

extern bool g_txoff;
unsigned long g_time_stamp = 0;
#ifdef WLMEDIA_HTSF
#include <linux/time.h>
#include <htsf.h>

#define HTSF_MINLEN 200    /* min. packet length to timestamp */
#define HTSF_BUS_DELAY 150 /* assume a fix propagation in us  */
#define TSMAX  1000        /* max no. of timing record kept   */
#define NUMBIN 34

static uint32 tsidx = 0;
static uint32 htsf_seqnum = 0;
uint32 tsfsync;
struct timeval tsync;
static uint32 tsport = 5010;

typedef struct histo_ {
	uint32 bin[NUMBIN];
} histo_t;

#if !ISPOWEROF2(DHD_SDALIGN)
#error DHD_SDALIGN is not a power of 2!
#endif

static histo_t vi_d1, vi_d2, vi_d3, vi_d4;
#endif /* WLMEDIA_HTSF */

#ifdef HNDCTF
void
et_ctf_detach_dhd(ctf_t *ci, void *arg)
{
    cih_dhd = NULL;
    return;
}

int
et_ctf_forward_dhd(struct sk_buff *skb)
{
    if (!CTF_ENAB(cih_dhd))
    {
        return (BCME_ERROR);
    }

    /* try cut thru first */
    if (ctf_forward(cih_dhd, skb, skb->dev) != BCME_ERROR)
    {
        return (BCME_OK);
    }
    /* clear skipct flag before sending up */
    PKTCLRSKIPCT(NULL /* et->osh*/, skb);

    return (BCME_ERROR);
}

#endif /* HNDCTF */

/* enable HOSTIP cache update from the host side when an eth0:N is up */
#define AOE_IP_ALIAS_SUPPORT 1

#ifdef BCM_FD_AGGR
#include <bcm_rpc.h>
#include <bcm_rpc_tp.h>
#endif
#ifdef PROP_TXSTATUS
#include <wlfc_proto.h>
#include <dhd_wlfc.h>
#endif
#include <wl_android.h>
#ifdef ARP_OFFLOAD_SUPPORT
void aoe_update_host_ipv4_table(dhd_pub_t *dhd_pub, u32 ipa, bool add);
static int dhd_device_event(struct notifier_block *this,
	unsigned long event,
	void *ptr);

static struct notifier_block dhd_notifier = {
	.notifier_call = dhd_device_event
};
#endif /* ARP_OFFLOAD_SUPPORT */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && defined(CONFIG_PM_SLEEP)
#include <linux/suspend.h>
volatile bool dhd_mmc_suspend = FALSE;
DECLARE_WAIT_QUEUE_HEAD(dhd_dpc_wait);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && defined(CONFIG_PM_SLEEP) */

#if defined(OOB_INTR_ONLY)
extern void dhd_enable_oob_intr(struct dhd_bus *bus, bool enable);
#endif /* defined(OOB_INTR_ONLY) */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
MODULE_LICENSE("GPL v2");
#endif /* LinuxVer */

#ifdef BCMDBUS
#include <dbus.h>
extern bool dhd_bus_download_firmware(struct dhd_bus *bus, osl_t *osh,
	char *firmware_path, char *nvram_path);
extern int dhd_bus_init(dhd_pub_t *dhdp, bool enforce_mutex);
extern void dhd_bus_stop(struct dhd_bus *bus, bool enforce_mutex);
extern void dhd_bus_unregister(void);
#ifdef PROP_TXSTATUS
extern void dhd_wlfc_txcomplete(dhd_pub_t *dhd, void *txp, bool success);
#endif
#else
#include <dhd_bus.h>
#endif /* BCMDBUS */

#ifdef BCM_FD_AGGR
#define DBUS_RX_BUFFER_SIZE_DHD(net)	(BCM_RPC_TP_DNGL_AGG_MAX_BYTE)
#else
#ifndef PROP_TXSTATUS
#define DBUS_RX_BUFFER_SIZE_DHD(net)    4096/*(net->mtu + net->hard_header_len + dhd->pub.hdrlen + 2048)*/ /* fuxiaowei: avoid memory corruption */
#else
#define DBUS_RX_BUFFER_SIZE_DHD(net)    4096/*(net->mtu + net->hard_header_len + dhd->pub.hdrlen + 128 + 2048)*/ /* fuxiaowei: avoid memory corruption */
#endif
#endif /* BCM_FD_AGGR */

#if LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 15)
const char *
print_tainted()
{
	return "";
}
#endif	/* LINUX_VERSION_CODE == KERNEL_VERSION(2, 6, 15) */

/* Linux wireless extension support */
#if defined(CONFIG_WIRELESS_EXT)
#include <wl_iw.h>
extern wl_iw_extra_params_t  g_wl_iw_params;
#endif /* defined(CONFIG_WIRELESS_EXT) */

#if defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
extern int dhdcdc_set_ioctl(dhd_pub_t *dhd, int ifidx, uint cmd, void *buf, uint len);
extern int dhd_get_dtim_skip(dhd_pub_t *dhd);
#endif /* defined(CONFIG_HAS_EARLYSUSPEND) */

#ifdef PKT_FILTER_SUPPORT
extern void dhd_pktfilter_offload_set(dhd_pub_t * dhd, char *arg);
extern void dhd_pktfilter_offload_enable(dhd_pub_t * dhd, char *arg, int enable, int master_mode);
#endif

/* Interface control information */
typedef struct dhd_if {
	struct dhd_info *info;			/* back pointer to dhd_info */
	/* OS/stack specifics */
	struct net_device *net;
	struct net_device_stats stats;
	int 			idx;			/* iface idx in dongle */
	dhd_if_state_t	state;			/* interface state */
	uint 			subunit;		/* subunit */
	uint8			mac_addr[ETHER_ADDR_LEN];	/* assigned MAC address */
	bool			attached;		/* Delayed attachment when unset */
	bool			txflowcontrol;	/* Per interface flow control indicator */
	char			name[IFNAMSIZ+1]; /* linux interface name */
	uint8			bssidx;			/* bsscfg index for the interface */
	bool			set_multicast;
	bool			event2cfg80211;	/* To determine if pass event to cfg80211 */
} dhd_if_t;

#ifdef WLMEDIA_HTSF
typedef struct {
	uint32 low;
	uint32 high;
} tsf_t;

typedef struct {
	uint32 last_cycle;
	uint32 last_sec;
	uint32 last_tsf;
	uint32 coef;     /* scaling factor */
	uint32 coefdec1; /* first decimal  */
	uint32 coefdec2; /* second decimal */
} htsf_t;

typedef struct {
	uint32 t1;
	uint32 t2;
	uint32 t3;
	uint32 t4;
} tstamp_t;

static tstamp_t ts[TSMAX];
static tstamp_t maxdelayts;
static uint32 maxdelay = 0, tspktcnt = 0, maxdelaypktno = 0;

#endif  /* WLMEDIA_HTSF */

extern void test_tx_restart_timer(int n);
/* Local private structure (extension of pub) */
typedef struct dhd_info {
#if defined(CONFIG_WIRELESS_EXT)
	wl_iw_t		iw;		/* wireless extensions state (must be first) */
#endif /* defined(CONFIG_WIRELESS_EXT) */

	dhd_pub_t pub;

	/* For supporting multiple interfaces */
	dhd_if_t *iflist[DHD_MAX_IFS];

	struct semaphore proto_sem;
#ifdef PROP_TXSTATUS
	spinlock_t	wlfc_spinlock;
#ifdef BCMDBUS
	ulong 		wlfc_lock_flags;
#endif
#endif /* PROP_TXSTATUS */
#ifdef WLMEDIA_HTSF
	htsf_t  htsf;
#endif
	wait_queue_head_t ioctl_resp_wait;
	struct timer_list timer;
	bool wd_timer_valid;
	struct tasklet_struct tasklet;
	spinlock_t	sdlock;
	spinlock_t	txqlock;
	spinlock_t	dhd_lock;
#ifdef BCMDBUS
	ulong		txqlock_flags;
#else
	bool dhd_tasklet_create;
#endif /* BCMDBUS */
	tsk_ctl_t	thr_sysioc_ctl;

	/* Wakelocks */
#if defined(CONFIG_HAS_WAKELOCK) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	struct wake_lock wl_wifi;   /* Wifi wakelock */
	struct wake_lock wl_rxwake; /* Wifi rx wakelock */
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)) && 0
	/* net_device interface lock, prevent race conditions among net_dev interface
	 * calls and wifi_on or wifi_off
	 */
	struct mutex dhd_net_if_mutex;
#endif
	spinlock_t wakelock_spinlock;
	int wakelock_counter;
	int wakelock_timeout_enable;

	/* Thread to issue ioctl for multicast */
	unsigned char set_macaddress;
	struct ether_addr macvalue;
	wait_queue_head_t ctrl_wait;
	atomic_t pend_8021x_cnt;
	dhd_attach_states_t dhd_state;

#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif /* CONFIG_HAS_EARLYSUSPEND */

#ifdef ARP_OFFLOAD_SUPPORT
	u32 pend_ipaddr;
#endif /* ARP_OFFLOAD_SUPPORT */
#ifdef BCM_FD_AGGR
	void *rpc_th;
	void *rpc_osh;
	struct timer_list rpcth_timer;
	bool rpcth_timer_active;
	bool fdaggr;
#endif
} dhd_info_t;

/* Flag to indicate if we should download firmware on driver load */
uint dhd_download_fw_on_driverload = TRUE;

typedef struct
{
    unsigned int txoff_drop_cnt;
    int netif_autostop;
    dhd_info_t *dhd;
}v7_dhd_ranger_t;

v7_dhd_ranger_t v7_dhd_ranger;

int v7_dhd_ranger_init();

/* Definitions to provide path to the firmware and nvram
 * example nvram_path[MOD_PARAM_PATHLEN]="/projects/wlan/nvram.txt"
 */
char firmware_path[MOD_PARAM_PATHLEN];
char nvram_path[MOD_PARAM_PATHLEN];

int op_mode = 0;
module_param(op_mode, int, 0644);

/* Spawn a thread for system ioctls (set mac, set mcast) */
uint dhd_sysioc = TRUE;
module_param(dhd_sysioc, uint, 0);

/* Error bits */
module_param(dhd_msg_level, int, 0);

#if !defined(BCMDBUS)
/* load firmware and/or nvram values from the filesystem */
module_param_string(firmware_path, firmware_path, MOD_PARAM_PATHLEN, 0660);
module_param_string(nvram_path, nvram_path, MOD_PARAM_PATHLEN, 0);

/* Watchdog interval */
uint dhd_watchdog_ms = 10;
module_param(dhd_watchdog_ms, uint, 0);

#if defined(DHD_DEBUG)
/* Console poll interval */
uint dhd_console_ms = 250;
module_param(dhd_console_ms, uint, 0644);
#endif /* defined(DHD_DEBUG) */

uint dhd_slpauto = TRUE;
module_param(dhd_slpauto, uint, 0);

/* ARP offload agent mode : Enable ARP Host Auto-Reply and ARP Peer Auto-Reply */
uint dhd_arp_mode = 0xb;
module_param(dhd_arp_mode, uint, 0);

/* ARP offload enable */
uint dhd_arp_enable = TRUE;
module_param(dhd_arp_enable, uint, 0);

#ifdef PKT_FILTER_SUPPORT
/* Global Pkt filter enable control */
uint dhd_pkt_filter_enable = TRUE;
module_param(dhd_pkt_filter_enable, uint, 0);
#endif

/* Pkt filter init setup */
uint dhd_pkt_filter_init = 0;
module_param(dhd_pkt_filter_init, uint, 0);

/* Pkt filter mode control */
#ifdef GAN_LITE_NAT_KEEPALIVE_FIlTER
uint dhd_master_mode = FALSE;
#else
uint dhd_master_mode = TRUE;
#endif /* GAL_LITE_NAT_KEEPALIVE_FILTER */
module_param(dhd_master_mode, uint, 1);

#endif /* BCMDBUS */
/* Control fw roaming */
uint dhd_roam_disable = 1;

/* Control radio state */
uint dhd_radio_up = 1;

/* Network inteface name */
char iface_name[IFNAMSIZ] = {"WiFi0"};
module_param_string(iface_name, iface_name, IFNAMSIZ, 0);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
#define DAEMONIZE(a) daemonize(a); \
	allow_signal(SIGKILL); \
	allow_signal(SIGTERM);
#else /* Linux 2.4 (w/o preemption patch) */
#define RAISE_RX_SOFTIRQ() \
	cpu_raise_softirq(smp_processor_id(), NET_RX_SOFTIRQ)
#define DAEMONIZE(a) daemonize(); \
	do { if (a) \
		strncpy(current->comm, a, MIN(sizeof(current->comm), (strlen(a) + 1))); \
	} while (0);
#endif /* LINUX_VERSION_CODE  */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
#define BLOCKABLE()	(!in_atomic())
#else
#define BLOCKABLE()	(!in_interrupt())
#endif

/* The following are specific to the SDIO dongle */

/* IOCTL response timeout */
int dhd_ioctl_timeout_msec = IOCTL_RESP_TIMEOUT;

/* Idle timeout for backplane clock */
int dhd_idletime = DHD_IDLETIME_TICKS;
module_param(dhd_idletime, int, 0);

/* Use polling */
uint dhd_poll = FALSE;
module_param(dhd_poll, uint, 0);

/* Use interrupts */
uint dhd_intr = TRUE;
module_param(dhd_intr, uint, 0);

/* SDIO Drive Strength (in milliamps) */
uint dhd_sdiod_drive_strength = 6;
module_param(dhd_sdiod_drive_strength, uint, 0);




/* Version string to report */
#ifdef DHD_DEBUG
#ifndef SRCBASE
#define SRCBASE        "drivers/net/wireless/bcmdhd"
#endif
#define DHD_COMPILED "\nCompiled in " SRCBASE
#else
#define DHD_COMPILED
#endif /* DHD_DEBUG */

static char dhd_version[] = "Dongle Host Driver, version " EPI_VERSION_STR
#ifdef DHD_DEBUG
"\nCompiled in " SRCBASE " on " __DATE__ " at " __TIME__
#endif
;
static void dhd_net_if_lock_local(dhd_info_t *dhd);
static void dhd_net_if_unlock_local(dhd_info_t *dhd);

#ifdef WLMEDIA_HTSF
void htsf_update(dhd_info_t *dhd, void *data);
tsf_t prev_tsf, cur_tsf;

uint32 dhd_get_htsf(dhd_info_t *dhd, int ifidx);
static int dhd_ioctl_htsf_get(dhd_info_t *dhd, int ifidx);
static void dhd_dump_latency(void);
static void dhd_htsf_addtxts(dhd_pub_t *dhdp, void *pktbuf);
static void dhd_htsf_addrxts(dhd_pub_t *dhdp, void *pktbuf);
static void dhd_dump_htsfhisto(histo_t *his, char *s);
#endif /* WLMEDIA_HTSF */

/* Monitor interface */
int dhd_monitor_init(void *dhd_pub);
int dhd_monitor_uninit(void);
#define RX_FLOW_MAX_RATE        (90 * 1000 * 1000) /* bit/s */
#define RX_FLOW_CALC_TIME       (5)
#define RX_FLOW_MAX_PKT_NUM     ((RX_FLOW_MAX_RATE * RX_FLOW_CALC_TIME) / (12 * 1000 * HZ))
static unsigned long sg_dhd_last_jiffies = 0;

#if defined(CONFIG_WIRELESS_EXT)
struct iw_statistics *dhd_get_wireless_stats(struct net_device *dev);
#endif /* defined(CONFIG_WIRELESS_EXT) */

#ifndef BCMDBUS
static void dhd_dpc(ulong data);
#endif
/* forward decl */
extern int dhd_wait_pend8021x(struct net_device *dev);

#ifdef TOE
#ifndef BDC
#error TOE requires BDC
#endif /* !BDC */
static int dhd_toe_get(dhd_info_t *dhd, int idx, uint32 *toe_ol);
static int dhd_toe_set(dhd_info_t *dhd, int idx, uint32 toe_ol);
#endif /* TOE */
#ifdef BCMDBUS
int dhd_dbus_txdata(dhd_pub_t *dhdp, void *pktbuf);
#endif

static int dhd_wl_host_event(dhd_info_t *dhd, int *ifidx, void *pktdata,
                             wl_event_msg_t *event_ptr, void **data_ptr);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && defined(CONFIG_PM_SLEEP)
static int dhd_sleep_pm_callback(struct notifier_block *nfb, unsigned long action, void *ignored)
{
	int ret = NOTIFY_DONE;

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 39))
	switch (action) {
	case PM_HIBERNATION_PREPARE:
	case PM_SUSPEND_PREPARE:
		dhd_mmc_suspend = TRUE;
		ret = NOTIFY_OK;
		break;
	case PM_POST_HIBERNATION:
	case PM_POST_SUSPEND:
		dhd_mmc_suspend = FALSE;
		ret = NOTIFY_OK;
		break;
	}
	smp_mb();
#endif
	return ret;
}

static struct notifier_block dhd_sleep_pm_notifier = {
	.notifier_call = dhd_sleep_pm_callback,
	.priority = 0
};
extern int register_pm_notifier(struct notifier_block *nb);
extern int unregister_pm_notifier(struct notifier_block *nb);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && defined(CONFIG_PM_SLEEP) */

static void dhd_set_packet_filter(int value, dhd_pub_t *dhd)
{
#ifdef PKT_FILTER_SUPPORT
	DHD_TRACE(("%s: %d\n", __FUNCTION__, value));
	/* 1 - Enable packet filter, only allow unicast packet to send up */
	/* 0 - Disable packet filter */
	if (dhd_pkt_filter_enable && !dhd->dhcp_in_progress) {
		int i;

		for (i = 0; i < dhd->pktfilter_count; i++) {
			dhd_pktfilter_offload_set(dhd, dhd->pktfilter[i]);
			dhd_pktfilter_offload_enable(dhd, dhd->pktfilter[i],
				value, dhd_master_mode);
		}
	}
#endif
}

#if defined(CONFIG_HAS_EARLYSUSPEND)
static int dhd_set_suspend(int value, dhd_pub_t *dhd)
{
	int power_mode = PM_MAX;
	/* wl_pkt_filter_enable_t	enable_parm; */
	char iovbuf[32];
	int bcn_li_dtim = 3;

	DHD_TRACE(("%s: enter, value = %d in_suspend=%d\n",
		__FUNCTION__, value, dhd->in_suspend));

	if (dhd && dhd->up) {
		if (value && dhd->in_suspend) {
#ifdef PKT_FILTER_SUPPORT
				dhd->early_suspended = 1;
#endif
				/* Kernel suspended */
				DHD_ERROR(("%s: force extra Suspend setting \n", __FUNCTION__));

				dhd_wl_ioctl_cmd(dhd, WLC_SET_PM, (char *)&power_mode,
				                 sizeof(power_mode), TRUE, 0);

				/* Enable packet filter, only allow unicast packet to send up */
				dhd_set_packet_filter(1, dhd);

				/* If DTIM skip is set up as default, force it to wake
				 * each third DTIM for better power savings.  Note that
				 * one side effect is a chance to miss BC/MC packet.
				 */
				bcn_li_dtim = dhd_get_dtim_skip(dhd);
				bcm_mkiovar("bcn_li_dtim", (char *)&bcn_li_dtim,
					4, iovbuf, sizeof(iovbuf));
				dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), TRUE, 0);

			} else {
#ifdef PKT_FILTER_SUPPORT
				dhd->early_suspended = 0;
#endif
				/* Kernel resumed  */
				DHD_TRACE(("%s: Remove extra suspend setting \n", __FUNCTION__));

				power_mode = PM_FAST;
				dhd_wl_ioctl_cmd(dhd, WLC_SET_PM, (char *)&power_mode,
				                 sizeof(power_mode), TRUE, 0);

				/* disable pkt filter */
				dhd_set_packet_filter(0, dhd);

				/* restore pre-suspend setting for dtim_skip */
				bcm_mkiovar("bcn_li_dtim", (char *)&dhd->dtim_skip,
					4, iovbuf, sizeof(iovbuf));

				dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), TRUE, 0);
			}
	}

	return 0;
}
static int dhd_netif_rx_flow_control(void)
{
    static unsigned int s_rx_pkt = 0;

    s_rx_pkt++;
    if (time_after_eq(jiffies, sg_dhd_last_jiffies + RX_FLOW_CALC_TIME))
    {
        sg_dhd_last_jiffies = jiffies;
        s_rx_pkt = 0;
        return 0;
    }
    else
    {
        if (s_rx_pkt > RX_FLOW_MAX_PKT_NUM)
        {
            return 1;
        }
    }
    return 0;
}

static void dhd_suspend_resume_helper(struct dhd_info *dhd, int val)
{
	dhd_pub_t *dhdp = &dhd->pub;

	DHD_OS_WAKE_LOCK(dhdp);
	/* Set flag when early suspend was called */
	dhdp->in_suspend = val;
	if ((!dhdp->suspend_disable_flag) && (dhd_check_ap_wfd_mode_set(dhdp) == FALSE))
		dhd_set_suspend(val, dhdp);
	DHD_OS_WAKE_UNLOCK(dhdp);
}

static void dhd_early_suspend(struct early_suspend *h)
{
	struct dhd_info *dhd = container_of(h, struct dhd_info, early_suspend);

	DHD_TRACE(("%s: enter\n", __FUNCTION__));

	if (dhd)
		dhd_suspend_resume_helper(dhd, 1);
}

static void dhd_late_resume(struct early_suspend *h)
{
	struct dhd_info *dhd = container_of(h, struct dhd_info, early_suspend);

	DHD_TRACE(("%s: enter\n", __FUNCTION__));

	if (dhd)
		dhd_suspend_resume_helper(dhd, 0);
}
#endif /* defined(CONFIG_HAS_EARLYSUSPEND) */

/*
 * Generalized timeout mechanism.  Uses spin sleep with exponential back-off until
 * the sleep time reaches one jiffy, then switches over to task delay.  Usage:
 *
 *      dhd_timeout_start(&tmo, usec);
 *      while (!dhd_timeout_expired(&tmo))
 *              if (poll_something())
 *                      break;
 *      if (dhd_timeout_expired(&tmo))
 *              fatal();
 */

void
dhd_timeout_start(dhd_timeout_t *tmo, uint usec)
{
	tmo->limit = usec;
	tmo->increment = 0;
	tmo->elapsed = 0;
	tmo->tick = 1000000 / HZ;
}

int
dhd_timeout_expired(dhd_timeout_t *tmo)
{
	/* Does nothing the first call */
	if (tmo->increment == 0) {
		tmo->increment = 1;
		return 0;
	}

	if (tmo->elapsed >= tmo->limit)
		return 1;

	/* Add the delay that's about to take place */
	tmo->elapsed += tmo->increment;

	if (tmo->increment < tmo->tick) {
		OSL_DELAY(tmo->increment);
		tmo->increment *= 2;
		if (tmo->increment > tmo->tick)
			tmo->increment = tmo->tick;
	} else {
		wait_queue_head_t delay_wait;
		DECLARE_WAITQUEUE(wait, current);
		int pending;
		init_waitqueue_head(&delay_wait);
		add_wait_queue(&delay_wait, &wait);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(1);
		pending = signal_pending(current);
		remove_wait_queue(&delay_wait, &wait);
		set_current_state(TASK_RUNNING);
		if (pending)
			return 1;	/* Interrupted */
	}

	return 0;
}

int
dhd_net2idx(dhd_info_t *dhd, struct net_device *net)
{
	int i = 0;

	ASSERT(dhd);
	while (i < DHD_MAX_IFS) {
		if (dhd->iflist[i] && (dhd->iflist[i]->net == net))
			return i;
		i++;
	}

	return DHD_BAD_IF;
}

struct net_device * dhd_idx2net(void *pub, int ifidx)
{
	struct dhd_pub *dhd_pub = (struct dhd_pub *)pub;
	struct dhd_info *dhd_info;

	if (!dhd_pub || ifidx < 0 || ifidx >= DHD_MAX_IFS)
		return NULL;
	dhd_info = dhd_pub->info;
	if (dhd_info && dhd_info->iflist[ifidx])
		return dhd_info->iflist[ifidx]->net;
	return NULL;
}

int
dhd_ifname2idx(dhd_info_t *dhd, char *name)
{
	int i = DHD_MAX_IFS;

	ASSERT(dhd);

	if (name == NULL || *name == '\0')
		return 0;

	while (--i > 0)
		if (dhd->iflist[i] && !strncmp(dhd->iflist[i]->name, name, IFNAMSIZ))
				break;

	DHD_TRACE(("%s: return idx %d for \"%s\"\n", __FUNCTION__, i, name));

	return i;	/* default - the primary interface */
}

char *
dhd_ifname(dhd_pub_t *dhdp, int ifidx)
{
	dhd_info_t *dhd = (dhd_info_t *)dhdp->info;

	ASSERT(dhd);

	if (ifidx < 0 || ifidx >= DHD_MAX_IFS) {
		DHD_ERROR(("%s: ifidx %d out of range\n", __FUNCTION__, ifidx));
		return "<if_bad>";
	}

	if (dhd->iflist[ifidx] == NULL) {
		DHD_ERROR(("%s: null i/f %d\n", __FUNCTION__, ifidx));
		return "<if_null>";
	}

	if (dhd->iflist[ifidx]->net)
		return dhd->iflist[ifidx]->net->name;

	return "<if_none>";
}

uint8 *
dhd_bssidx2bssid(dhd_pub_t *dhdp, int idx)
{
	int i;
	dhd_info_t *dhd = (dhd_info_t *)dhdp;

	ASSERT(dhd);
	for (i = 0; i < DHD_MAX_IFS; i++)
	if (dhd->iflist[i] && dhd->iflist[i]->bssidx == idx)
		return dhd->iflist[i]->mac_addr;

	return NULL;
}

#ifdef BCMDBUS

#if 0
#define DBUS_NRXQ	50
#define DBUS_NTXQ	50
#else
#define DBUS_NRXQ   100
#define DBUS_NTXQ   100
#endif
static void
dhd_dbus_send_complete(void *handle, void *info, int status)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;
	void *pkt = info;

	if (dhd == NULL)
		return;

	if (status == DBUS_OK) {
		dhd->pub.dstats.tx_packets++;
	} else {
		DHD_ERROR(("TX error=%d\n", status));
		dhd->pub.dstats.tx_errors++;
	}
#ifdef PROP_TXSTATUS
	if (dhd->pub.wlfc_state) {
		dhd_wlfc_txcomplete(&dhd->pub, pkt, status == 0);
	} else
#else
    dhd_txcomplete(&dhd->pub, pkt, TRUE);

#endif /* PROP_TXSTATUS */
	if (pkt) {
		PKTFREE(dhd->pub.osh, pkt, TRUE);
	}
}

static void
dhd_dbus_recv_pkt(void *handle, void *pkt, int len)
{
    dhd_info_t *dhd = (dhd_info_t *)handle;
    int ifidx = 0;

    if (dhd == NULL)
        return;

    /* If the protocol uses a data header, check and remove it */
    if (dhd_prot_hdrpull(&dhd->pub, &ifidx, pkt, NULL, NULL) != 0) {
        DHD_ERROR(("rx protocol error\n"));
        PKTFREE(dhd->pub.osh, pkt, FALSE);
        dhd->pub.rx_errors++;
        return;
    }
    PKTSETLEN(dhd->pub.osh, pkt, len - 8);
    dhd_rx_frame(&dhd->pub, ifidx, pkt, 1, 0);
}

static void
dhd_dbus_recv_buf(void *handle, uint8 *buf, int len)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;
	void *pkt;

	if (dhd == NULL)
		return;

	if ((pkt = PKTGET(dhd->pub.osh, len, FALSE)) == NULL) {
		DHD_ERROR(("PKTGET (rx) failed=%d\n", len));
		return;
	}

	bcopy(buf, PKTDATA(dhd->pub.osh, pkt), len);
	dhd_dbus_recv_pkt(dhd, pkt, len);
}

static void
dhd_dbus_txflowcontrol(void *handle, bool onoff)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;
#ifdef PROP_TXSTATUS
	dhd_pub_t *dhdp;
#endif
	if (dhd == NULL)
		return;

#ifdef PROP_TXSTATUS
	dhdp = &dhd->pub;

	if (dhdp->wlfc_state &&
	    ((athost_wl_status_info_t*)dhdp->wlfc_state)->proptxstatus_mode != WLFC_FCMODE_NONE) {
		/* do nothing */
	} else
#endif
	dhd_txflowcontrol(&dhd->pub, ALL_INTERFACES, onoff);
}

static void
dhd_dbus_errhandler(void *handle, int err)
{
}

static void
dhd_dbus_ctl_complete(void *handle, int type, int status)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;

	if (dhd == NULL)
		return;

	if (type == DBUS_CBCTL_READ) {
		if (status == DBUS_OK)
			dhd->pub.rx_ctlpkts++;
		else
			dhd->pub.rx_ctlerrs++;
	} else if (type == DBUS_CBCTL_WRITE) {
		if (status == DBUS_OK)
			dhd->pub.tx_ctlpkts++;
		else
			dhd->pub.tx_ctlerrs++;
	}

	dhd_prot_ctl_complete(&dhd->pub);
}

static void
dhd_dbus_state_change(void *handle, int state)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;

	if (dhd == NULL)
		return;

	if (state == DBUS_STATE_DOWN) {
		DHD_ERROR(("%s: DBUS is down\n", __FUNCTION__));
		dhd->pub.busstate = DHD_BUS_DOWN;
	} else {
		DHD_ERROR(("%s: DBUS current state=%d\n", __FUNCTION__, state));
	}
}

static void *
dhd_dbus_pktget(void *handle, uint len, bool send)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;
	void *p = NULL;

	if (dhd == NULL)
		return NULL;

	if (send == TRUE) {
		dhd_os_sdlock_txq(&dhd->pub);
		p = PKTGET(dhd->pub.osh, len, TRUE);
		dhd_os_sdunlock_txq(&dhd->pub);
	} else {
		dhd_os_sdlock_rxq(&dhd->pub);
		p = PKTGET(dhd->pub.osh, len, FALSE);
		dhd_os_sdunlock_rxq(&dhd->pub);
	}

	return p;
}

static void
dhd_dbus_pktfree(void *handle, void *p, bool send)
{
	dhd_info_t *dhd = (dhd_info_t *)handle;

	if (dhd == NULL)
		return;
    if(in_irq() || irqs_disabled())
    {
        if (send == TRUE)
        {
            PKTFREE(dhd->pub.osh, p, TRUE);
        }
        else
        {
            PKTFREE(dhd->pub.osh, p, FALSE);
        }
    }
    else
    {
        if (send == TRUE) {
            dhd_os_sdlock_txq(&dhd->pub);
            PKTFREE(dhd->pub.osh, p, TRUE);
            dhd_os_sdunlock_txq(&dhd->pub);
        } else {
            dhd_os_sdlock_rxq(&dhd->pub);
            PKTFREE(dhd->pub.osh, p, FALSE);
            dhd_os_sdunlock_rxq(&dhd->pub);
        }
    }
}

#ifdef BCM_FD_AGGR

static void
dbus_rpcth_tx_complete(void *ctx, void *pktbuf, int status)
{
	dhd_info_t *dhd = (dhd_info_t *)ctx;
	void *tmp;

	while (pktbuf && dhd) {
		tmp = PKTNEXT(dhd->osh, pktbuf);
		PKTSETNEXT(dhd->osh, pktbuf, NULL);
		dhd_dbus_send_complete(ctx, pktbuf, status);
		pktbuf = tmp;
	}
}
static void
dbus_rpcth_rx_pkt(void *context, rpc_buf_t *rpc_buf)
{
	dhd_dbus_recv_pkt(context, rpc_buf);
}

static void
dbus_rpcth_rx_aggrpkt(void *context, void *rpc_buf)
{
	dhd_info_t *dhd = (dhd_info_t *)context;

	if (dhd == NULL)
		return;

	/* all the de-aggregated packets are delivered back to function dbus_rpcth_rx_pkt()
	* as cloned packets
	*/
	bcm_rpc_dbus_recv_aggrpkt(dhd->rpc_th, rpc_buf,
		bcm_rpc_buf_len_get(dhd->rpc_th, rpc_buf));

	/* free the original packet */
	dhd_dbus_pktfree(context, rpc_buf, FALSE);
}

static void
dbus_rpcth_rx_aggrbuf(void *context, uint8 *buf, int len)
{
	dhd_info_t *dhd = (dhd_info_t *)context;

	if (dhd == NULL)
		return;

	if (dhd->fdaggr) {
		bcm_rpc_dbus_recv_aggrbuf(dhd->rpc_th, buf, len);
	}
	else {
		dhd_dbus_recv_buf(context, buf, len);
	}

}

static void
dhd_rpcth_watchdog(ulong data)
{
	dhd_info_t *dhd = (dhd_info_t *)data;

	if (dhd->pub.dongle_reset) {
		return;
	}

	dhd->rpcth_timer_active = FALSE;
	/* release packets in the aggregation queue */
	bcm_rpc_tp_watchdog(dhd->rpc_th);
}

static int
dhd_fdaggr_ioctl(dhd_pub_t *dhd_pub, int ifindex, wl_ioctl_t *ioc, void *buf, int len)
{
	int bcmerror = 0;
	void *rpc_th;

	rpc_th = dhd_pub->info->rpc_th;

	if (!strcmp("rpc_agg", ioc->buf)) {
		uint32 rpc_agg;
		uint32 rpc_agg_host;
		uint32 rpc_agg_dngl;

		if (ioc->set) {
			memcpy(&rpc_agg, ioc->buf + strlen("rpc_agg") + 1, sizeof(uint32));
			rpc_agg_host = rpc_agg & BCM_RPC_TP_HOST_AGG_MASK;
			if (rpc_agg_host)
				bcm_rpc_tp_agg_set(rpc_th, rpc_agg_host, TRUE);
			else
				bcm_rpc_tp_agg_set(rpc_th, BCM_RPC_TP_HOST_AGG_MASK, FALSE);
			bcmerror = dhd_wl_ioctl(dhd_pub, ifindex, ioc, buf, len);
			if (bcmerror < 0) {
				DHD_ERROR(("usb aggregation not supported\n"));
			} else {
				dhd_pub->info->fdaggr = (rpc_agg != 0);
			}
		} else {
			rpc_agg_host = bcm_rpc_tp_agg_get(rpc_th);
			bcmerror = dhd_wl_ioctl(dhd_pub, ifindex, ioc, buf, len);
			if (!bcmerror) {
				memcpy(&rpc_agg_dngl, buf, sizeof(uint32));
				rpc_agg = (rpc_agg_host & BCM_RPC_TP_HOST_AGG_MASK) |
					(rpc_agg_dngl & BCM_RPC_TP_DNGL_AGG_MASK);
				memcpy(buf, &rpc_agg, sizeof(uint32));
			}
		}
	} else if (!strcmp("rpc_host_agglimit", ioc->buf)) {
		uint8 sf;
		uint16 bytes;
		uint32 agglimit;

		if (ioc->set) {
			memcpy(&agglimit, ioc->buf + strlen("rpc_host_agglimit") + 1,
				sizeof(uint32));
			sf = agglimit >> 16;
			bytes = agglimit & 0xFFFF;
			bcm_rpc_tp_agg_limit_set(rpc_th, sf, bytes);
		} else {
			bcm_rpc_tp_agg_limit_get(rpc_th, &sf, &bytes);
			agglimit = (uint32)((sf << 16) + bytes);
			memcpy(buf, &agglimit, sizeof(uint32));
		}

	} else {
		bcmerror = dhd_wl_ioctl(dhd_pub, ifindex, ioc, buf, len);
	}
	return bcmerror;
}
#endif /* BCM_FD_AGGR */

static dbus_callbacks_t dhd_dbus_cbs = {
#ifdef BCM_FD_AGGR
	dbus_rpcth_tx_complete,
	dbus_rpcth_rx_aggrbuf,
	dbus_rpcth_rx_aggrpkt,
#else
	dhd_dbus_send_complete,
	dhd_dbus_recv_buf,
	dhd_dbus_recv_pkt,
#endif
	dhd_dbus_txflowcontrol,
	dhd_dbus_errhandler,
	dhd_dbus_ctl_complete,
	dhd_dbus_state_change,
	dhd_dbus_pktget,
	dhd_dbus_pktfree
};
#ifdef DHD_MORE_DUMP
int dbus_dump(const dbus_pub_t *dpub, struct bcmstrbuf *strbuf);
#endif
void
dhd_bus_dump(dhd_pub_t *dhdp, struct bcmstrbuf *strbuf)
{
	bcm_bprintf(strbuf, "Bus USB\n");
#ifdef DHD_MORE_DUMP
	dbus_dump(dhdp->dbus, strbuf);
#endif
}

void
dhd_bus_clearcounts(dhd_pub_t *dhdp)
{
}

bool
dhd_bus_dpc(struct dhd_bus *bus)
{
	return FALSE;
}

int
dhd_dbus_txdata(dhd_pub_t *dhdp, void *pktbuf)
{

    /*ֻ�ǰ����ص����ش�ӡ����*/
    if (dhdp->txoff)
    {
        v7_dhd_ranger.txoff_drop_cnt++;     
        DHD_TRACE(("%s: 200: txoff_drop_cnt = %u\n", __FUNCTION__, v7_dhd_ranger.txoff_drop_cnt));

        DHD_TRACE(("%s: 300: netif_queue stop = %d\n", __FUNCTION__, netif_queue_stopped(dhdp->info->iflist[0]->net)));

        if(v7_dhd_ranger.netif_autostop == 1)
        {
            netif_stop_queue(dhdp->info->iflist[0]->net);
        }

        /* tempary to reuse the SDIO number */
        return BCME_EPERM;
    }
#ifdef BCM_FD_AGGR
	if (((dhd_info_t *)(dhdp->info))->fdaggr)

	{
		dhd_info_t *dhd;
		int ret;
		dhd = (dhd_info_t *)(dhdp->info);
		ret = bcm_rpc_tp_buf_send(dhd->rpc_th, pktbuf);
		if (dhd->rpcth_timer_active == FALSE) {
			dhd->rpcth_timer_active = TRUE;
			mod_timer(&dhd->rpcth_timer, jiffies + BCM_RPC_TP_HOST_TMOUT * HZ / 1000);
		}
		return ret;
	} else
#endif /* BCM_FD_AGGR */
	return dbus_send_txdata(dhdp->dbus, pktbuf);
}

#endif /* BCMDBUS */

static void
_dhd_set_multicast_list(dhd_info_t *dhd, int ifidx)
{
	struct net_device *dev;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
	struct netdev_hw_addr *ha;
#else
	struct dev_mc_list *mclist;
#endif
	uint32 allmulti, cnt;

	wl_ioctl_t ioc;
	char *buf, *bufp;
	uint buflen;
	int ret;

#ifdef MCAST_LIST_ACCUMULATION
	int i;
	uint32 cnt_iface[DHD_MAX_IFS];
	cnt = 0;
	allmulti = 0;

	for (i = 0; i < DHD_MAX_IFS; i++) {
		if (dhd->iflist[i]) {
			dev = dhd->iflist[i]->net;
#else
			ASSERT(dhd && dhd->iflist[ifidx]);
			dev = dhd->iflist[ifidx]->net;
#endif /* MCAST_LIST_ACCUMULATION */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
			netif_addr_lock_bh(dev);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
#ifdef MCAST_LIST_ACCUMULATION
			cnt_iface[i] = netdev_mc_count(dev);
			cnt += cnt_iface[i];
#else
			cnt = netdev_mc_count(dev);
#endif /* MCAST_LIST_ACCUMULATION */
#else
#ifdef MCAST_LIST_ACCUMULATION
			cnt += dev->mc_count;
#else
			cnt = dev->mc_count;
#endif /* MCAST_LIST_ACCUMULATION */
#endif /* LINUX_VERSION_CODE */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
			netif_addr_unlock_bh(dev);
#endif

			/* Determine initial value of allmulti flag */
#ifdef MCAST_LIST_ACCUMULATION
			allmulti |= (dev->flags & IFF_ALLMULTI) ? TRUE : FALSE;
		}
	}
#else
	allmulti = (dev->flags & IFF_ALLMULTI) ? TRUE : FALSE;
#endif /* MCAST_LIST_ACCUMULATION */


	/* Send down the multicast list first. */


	buflen = sizeof("mcast_list") + sizeof(cnt) + (cnt * ETHER_ADDR_LEN);
	if (!(bufp = buf = MALLOC(dhd->pub.osh, buflen))) {
		DHD_ERROR(("%s: out of memory for mcast_list, cnt %d\n",
		           dhd_ifname(&dhd->pub, ifidx), cnt));
		return;
	}

	strncpy(bufp, "mcast_list", sizeof("mcast_list"));
	bufp += sizeof("mcast_list");

	cnt = htol32(cnt);
	memcpy(bufp, &cnt, sizeof(cnt));
	bufp += sizeof(cnt);

#ifdef MCAST_LIST_ACCUMULATION
	for (i = 0; i < DHD_MAX_IFS; i++) {
		if (dhd->iflist[i]) {
			DHD_TRACE(("_dhd_set_multicast_list: ifidx %d\n", i));
			dev = dhd->iflist[i]->net;
#endif /* MCAST_LIST_ACCUMULATION */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
			netif_addr_lock_bh(dev);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
			netdev_for_each_mc_addr(ha, dev) {
#ifdef MCAST_LIST_ACCUMULATION
				if (!cnt_iface[i])
#else
				if (!cnt)
#endif /* MCAST_LIST_ACCUMULATION */
					break;
				memcpy(bufp, ha->addr, ETHER_ADDR_LEN);
				bufp += ETHER_ADDR_LEN;
#ifdef MCAST_LIST_ACCUMULATION
				DHD_TRACE(("_dhd_set_multicast_list: cnt "
					"%d %02x:%02x:%02x:%02x:%02x:%02x\n",
					cnt_iface[i],  ha->addr[0],  ha->addr[1],
					ha->addr[2],  ha->addr[3],  ha->addr[4],
					ha->addr[5]));
				cnt_iface[i]--;
#else
				cnt--;
#endif /* MCAST_LIST_ACCUMULATION */
	}
#else
#ifdef MCAST_LIST_ACCUMULATION
			for (mclist = dev->mc_list; (mclist && (cnt_iface[i] > 0));
				cnt_iface[i]--, mclist = mclist->next) {
#else
			for (mclist = dev->mc_list; (mclist && (cnt > 0));
				cnt--, mclist = mclist->next) {
#endif /* MCAST_LIST_ACCUMULATION */
				memcpy(bufp, (void *)mclist->dmi_addr, ETHER_ADDR_LEN);
				bufp += ETHER_ADDR_LEN;
			}
#endif /* LINUX_VERSION_CODE */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)
			netif_addr_unlock_bh(dev);
#endif
#ifdef MCAST_LIST_ACCUMULATION
		}
	}
#endif /* MCAST_LIST_ACCUMULATION */

	memset(&ioc, 0, sizeof(ioc));
	ioc.cmd = WLC_SET_VAR;
	ioc.buf = buf;
	ioc.len = buflen;
	ioc.set = TRUE;

	ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len);
	if (ret < 0) {
		DHD_ERROR(("%s: set mcast_list failed, cnt %d\n",
			dhd_ifname(&dhd->pub, ifidx), cnt));
		allmulti = cnt ? TRUE : allmulti;
	}

	MFREE(dhd->pub.osh, buf, buflen);

	/* Now send the allmulti setting.  This is based on the setting in the
	 * net_device flags, but might be modified above to be turned on if we
	 * were trying to set some addresses and dongle rejected it...
	 */

	buflen = sizeof("allmulti") + sizeof(allmulti);
	if (!(buf = MALLOC(dhd->pub.osh, buflen))) {
		DHD_ERROR(("%s: out of memory for allmulti\n", dhd_ifname(&dhd->pub, ifidx)));
		return;
	}
	allmulti = htol32(allmulti);

	if (!bcm_mkiovar("allmulti", (void*)&allmulti, sizeof(allmulti), buf, buflen)) {
		DHD_ERROR(("%s: mkiovar failed for allmulti, datalen %d buflen %u\n",
		           dhd_ifname(&dhd->pub, ifidx), (int)sizeof(allmulti), buflen));
		MFREE(dhd->pub.osh, buf, buflen);
		return;
	}


	memset(&ioc, 0, sizeof(ioc));
	ioc.cmd = WLC_SET_VAR;
	ioc.buf = buf;
	ioc.len = buflen;
	ioc.set = TRUE;

	ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len);
	if (ret < 0) {
		DHD_ERROR(("%s: set allmulti %d failed\n",
		           dhd_ifname(&dhd->pub, ifidx), ltoh32(allmulti)));
	}

	MFREE(dhd->pub.osh, buf, buflen);

	/* Finally, pick up the PROMISC flag as well, like the NIC driver does */

#ifdef MCAST_LIST_ACCUMULATION
	allmulti = 0;
	for (i = 0; i < DHD_MAX_IFS; i++) {
		if (dhd->iflist[i]) {
			dev = dhd->iflist[i]->net;
			allmulti |= (dev->flags & IFF_PROMISC) ? TRUE : FALSE;
		}
	}
#else
	allmulti = (dev->flags & IFF_PROMISC) ? TRUE : FALSE;
#endif /* MCAST_LIST_ACCUMULATION */
	/* BEGIN: Added by Benson, 2012/8/28/12:15   PN:43241 bring up */
	/* Disable promisc mode , it is useless for MiFi product */
	allmulti = FALSE;

	allmulti = htol32(allmulti);

	memset(&ioc, 0, sizeof(ioc));
	ioc.cmd = WLC_SET_PROMISC;
	ioc.buf = &allmulti;
	ioc.len = sizeof(allmulti);
	ioc.set = TRUE;

	ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len);
	if (ret < 0) {
		DHD_ERROR(("%s: set promisc %d failed\n",
		           dhd_ifname(&dhd->pub, ifidx), ltoh32(allmulti)));
	}
}

#ifdef CUSTOMER_HW4
int
#else
static int
#endif
_dhd_set_mac_address(dhd_info_t *dhd, int ifidx, struct ether_addr *addr)
{
	char buf[32];
	wl_ioctl_t ioc;
	int ret;

	if (!bcm_mkiovar("cur_etheraddr", (char*)addr, ETHER_ADDR_LEN, buf, 32)) {
		DHD_ERROR(("%s: mkiovar failed for cur_etheraddr\n", dhd_ifname(&dhd->pub, ifidx)));
		return -1;
	}
	memset(&ioc, 0, sizeof(ioc));
	ioc.cmd = WLC_SET_VAR;
	ioc.buf = buf;
	ioc.len = 32;
	ioc.set = TRUE;

	ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len);
	if (ret < 0) {
		DHD_ERROR(("%s: set cur_etheraddr failed\n", dhd_ifname(&dhd->pub, ifidx)));
	} else {
		memcpy(dhd->iflist[ifidx]->net->dev_addr, addr, ETHER_ADDR_LEN);
		memcpy(dhd->pub.mac.octet, addr, ETHER_ADDR_LEN);
	}

	return ret;
}

#ifdef SOFTAP
extern struct net_device *ap_net_dev;
extern tsk_ctl_t ap_eth_ctl; /* ap netdev heper thread ctl */
#endif

static void
dhd_op_if(dhd_if_t *ifp)
{
	dhd_info_t	*dhd;
	int ret = 0, err = 0;
#ifdef SOFTAP
	unsigned long flags;
#endif

	if (!ifp || !ifp->info || !ifp->idx)
		return;
	ASSERT(ifp && ifp->info && ifp->idx);	/* Virtual interfaces only */
	dhd = ifp->info;

	DHD_TRACE(("%s: idx %d, state %d\n", __FUNCTION__, ifp->idx, ifp->state));

#ifdef WL_CFG80211
	if (wl_cfg80211_is_progress_ifchange())
			return;

#endif
	switch (ifp->state) {
	case DHD_IF_ADD:
		/*
		 * Delete the existing interface before overwriting it
		 * in case we missed the WLC_E_IF_DEL event.
		 */
		if (ifp->net != NULL) {
			DHD_ERROR(("%s: ERROR: netdev:%s already exists, try free & unregister \n",
			 __FUNCTION__, ifp->net->name));
			netif_stop_queue(ifp->net);
			unregister_netdev(ifp->net);
			free_netdev(ifp->net);
            ifp->net = NULL;
		}
		/* Allocate etherdev, including space for private structure */
		if (!(ifp->net = alloc_etherdev(sizeof(dhd)))) {
			DHD_ERROR(("%s: OOM - alloc_etherdev\n", __FUNCTION__));
			ret = -ENOMEM;
		}
		if (ret == 0) {
			strncpy(ifp->net->name, ifp->name, IFNAMSIZ);
			ifp->net->name[IFNAMSIZ - 1] = '\0';
			memcpy(netdev_priv(ifp->net), &dhd, sizeof(dhd));
#ifdef WL_CFG80211
			if (dhd->dhd_state & DHD_ATTACH_STATE_CFG80211)
				if (!wl_cfg80211_notify_ifadd(ifp->net, ifp->idx, ifp->bssidx,
					(void*)dhd_net_attach)) {
					ifp->state = DHD_IF_NONE;
					ifp->event2cfg80211 = TRUE;
					return;
				}
#endif
			if ((err = dhd_net_attach(&dhd->pub, ifp->idx)) != 0) {
				DHD_ERROR(("%s: dhd_net_attach failed, err %d\n",
					__FUNCTION__, err));
				ret = -EOPNOTSUPP;
			} else {
				DHD_TRACE(("\n ==== pid:%x, net_device for if:%s created ===\n\n",
					current->pid, ifp->net->name));
				ifp->state = DHD_IF_NONE;
			}
		}
		break;
	case DHD_IF_DEL:
		/* Make sure that we don't enter again here if .. */
		/* dhd_op_if is called again from some other context */
		ifp->state = DHD_IF_DELETING;
		if (ifp->net != NULL) {
			DHD_TRACE(("\n%s: got 'DHD_IF_DEL' state\n", __FUNCTION__));
#ifdef WL_CFG80211
			if (dhd->dhd_state & DHD_ATTACH_STATE_CFG80211) {
				wl_cfg80211_ifdel_ops(ifp->net);
			}
#endif
			netif_stop_queue(ifp->net);
			unregister_netdev(ifp->net);
			ret = DHD_DEL_IF;	/* Make sure the free_netdev() is called */

#ifdef WL_CFG80211
			if (dhd->dhd_state & DHD_ATTACH_STATE_CFG80211) {
				wl_cfg80211_notify_ifdel();
			}
#endif
		}
		break;
	case DHD_IF_DELETING:
		break;
    case DHD_IF_CHANGE:
        /* The mac address of the interface changed */
#if defined(DHD_UPDATE_INTF_MAC)
        {
            int bret = 0;
            char buf[128];

            /* Get the device MAC address */
            ifp->state = DHD_IF_NONE;
            strcpy(buf, "cur_etheraddr");
            bret = dhd_wl_ioctl_cmd(&dhd->pub, WLC_GET_VAR, buf, sizeof(buf), FALSE, ifp->idx);
            if (bret < 0)
            {
                DHD_ERROR(("Get mac address for interface %s failed %d\n", ifp->name,bret));

                /* avoid collision */
                dhd->iflist[ifp->idx]->mac_addr[5] += 1;
                /* force locally administered address */
                ETHER_SET_LOCALADDR(&dhd->iflist[ifp->idx]->mac_addr);
            } 
            else 
            {
                DHD_EVENT(("Get mac address %s idx %d\n", ifp->name, ifp->idx));
                memcpy(dhd->iflist[ifp->idx]->mac_addr, buf, ETHER_ADDR_LEN);
                if ( dhd->iflist[ifp->idx]->net )
                {
                    memcpy(dhd->iflist[ifp->idx]->net->dev_addr, buf, ETHER_ADDR_LEN);
                }
            }
        }
#endif
        break;
	default:
		DHD_ERROR(("%s: bad op %d\n", __FUNCTION__, ifp->state));
		ASSERT(!ifp->state);
		break;
	}

	if (ret < 0) {
		ifp->set_multicast = FALSE;
		if (ifp->net) {
			free_netdev(ifp->net);
			ifp->net = NULL;
		}
		dhd->iflist[ifp->idx] = NULL;
#ifdef SOFTAP
		flags = dhd_os_spin_lock(&dhd->pub);
		if (ifp->net == ap_net_dev)
			ap_net_dev = NULL;   /*  NULL  SOFTAP global wl0.1 as well */
		dhd_os_spin_unlock(&dhd->pub, flags);
#endif /*  SOFTAP */
		MFREE(dhd->pub.osh, ifp, sizeof(*ifp));
	}
}

static int
_dhd_sysioc_thread(void *data)
{
	tsk_ctl_t *tsk = (tsk_ctl_t *)data;
	dhd_info_t *dhd = (dhd_info_t *)tsk->parent;


	int i;
#ifdef SOFTAP
	bool in_ap = FALSE;
	unsigned long flags;
#endif

	DAEMONIZE("dhd_sysioc");

	complete(&tsk->completed);

	while (down_interruptible(&tsk->sema) == 0) {
#ifdef MCAST_LIST_ACCUMULATION
		bool set_multicast = FALSE;
#endif /* MCAST_LIST_ACCUMULATION */

		SMP_RD_BARRIER_DEPENDS();
		if (tsk->terminated) {
			break;
		}

#ifdef BGBRD
		if (dhdThreadState == DHD_EXIT_STATE_EXITING)
			break;
#endif
		dhd_net_if_lock_local(dhd);
		DHD_OS_WAKE_LOCK(&dhd->pub);

		for (i = 0; i < DHD_MAX_IFS; i++) {
			if (dhd->iflist[i]) {
				DHD_TRACE(("%s: interface %d\n", __FUNCTION__, i));
#ifdef SOFTAP
				flags = dhd_os_spin_lock(&dhd->pub);
				in_ap = (ap_net_dev != NULL);
				dhd_os_spin_unlock(&dhd->pub, flags);
#endif /* SOFTAP */
				if (dhd->iflist[i] && dhd->iflist[i]->state)
					dhd_op_if(dhd->iflist[i]);

				if (dhd->iflist[i] == NULL) {
					DHD_TRACE(("\n\n %s: interface %d just been removed,"
						"!\n\n", __FUNCTION__, i));
					continue;
				}
#ifdef SOFTAP
				if (in_ap && dhd->set_macaddress == i+1)  {
					DHD_TRACE(("attempt to set MAC for %s in AP Mode,"
						"blocked. \n", dhd->iflist[i]->net->name));
					dhd->set_macaddress = 0;
					continue;
				}

				if (in_ap && dhd->iflist[i]->set_multicast)  {
					DHD_TRACE(("attempt to set MULTICAST list for %s"
					 "in AP Mode, blocked. \n", dhd->iflist[i]->net->name));
					dhd->iflist[i]->set_multicast = FALSE;
					continue;
				}
#endif /* SOFTAP */
				if (dhd->pub.up == 0)
					continue;
				if (dhd->iflist[i]->set_multicast) {
					dhd->iflist[i]->set_multicast = FALSE;
#ifdef MCAST_LIST_ACCUMULATION
					set_multicast = TRUE;
#else
					_dhd_set_multicast_list(dhd, i);
#endif /* MCAST_LIST_ACCUMULATION */

				}
				if (dhd->set_macaddress == i+1) {
					dhd->set_macaddress = 0;
					_dhd_set_mac_address(dhd, i, &dhd->macvalue);
				}
			}
		}
#ifdef MCAST_LIST_ACCUMULATION
		if (set_multicast)
			_dhd_set_multicast_list(dhd, 0);
#endif /* MCAST_LIST_ACCUMULATION */

		DHD_OS_WAKE_UNLOCK(&dhd->pub);
		dhd_net_if_unlock_local(dhd);
	}
	DHD_TRACE(("%s: stopped\n", __FUNCTION__));
	complete_and_exit(&tsk->completed, 0);
}

static int
dhd_set_mac_address(struct net_device *dev, void *addr)
{
	int ret = 0;

	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	struct sockaddr *sa = (struct sockaddr *)addr;
	int ifidx;

	ifidx = dhd_net2idx(dhd, dev);
	if (ifidx == DHD_BAD_IF)
		return -1;

	ASSERT(dhd->thr_sysioc_ctl.thr_pid >= 0);
	memcpy(&dhd->macvalue, sa->sa_data, ETHER_ADDR_LEN);
	dhd->set_macaddress = ifidx+1;
	up(&dhd->thr_sysioc_ctl.sema);

	return ret;
}

static void
dhd_set_multicast_list(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	int ifidx;

	ifidx = dhd_net2idx(dhd, dev);
	if (ifidx == DHD_BAD_IF)
		return;

	ASSERT(dhd->thr_sysioc_ctl.thr_pid >= 0);
	dhd->iflist[ifidx]->set_multicast = TRUE;
	up(&dhd->thr_sysioc_ctl.sema);
}

#ifdef PROP_TXSTATUS
int
dhd_os_wlfc_block(dhd_pub_t *pub)
{
	dhd_info_t *di = (dhd_info_t *)(pub->info);
	ASSERT(di != NULL);
#ifdef BCMDBUS
	spin_lock_irqsave(&di->wlfc_spinlock, di->wlfc_lock_flags);
#else
	spin_lock_bh(&di->wlfc_spinlock);
#endif
	return 1;
}

int
dhd_os_wlfc_unblock(dhd_pub_t *pub)
{
	dhd_info_t *di = (dhd_info_t *)(pub->info);

	(void)di;
	ASSERT(di != NULL);
#ifdef BCMDBUS
	spin_unlock_irqrestore(&di->wlfc_spinlock, di->wlfc_lock_flags);
#else
	spin_unlock_bh(&di->wlfc_spinlock);
#endif
	return 1;
}

const uint8 wme_fifo2ac[] = { 0, 1, 2, 3, 1, 1 };
uint8 prio2fifo[8] = { 1, 0, 0, 1, 2, 2, 3, 3 };
#define WME_PRIO2AC(prio)	wme_fifo2ac[prio2fifo[(prio)]]

#endif /* PROP_TXSTATUS */
int
dhd_sendpkt(dhd_pub_t *dhdp, int ifidx, void *pktbuf)
{
	int ret;
	dhd_info_t *dhd = (dhd_info_t *)(dhdp->info);
	struct ether_header *eh = NULL;

	/* Reject if down */
	if (!dhdp->up || (dhdp->busstate == DHD_BUS_DOWN)) {
		/* free the packet here since the caller won't */
		PKTFREE(dhdp->osh, pktbuf, TRUE);
		return -ENODEV;
	}

	/* Update multicast statistic */
	if (PKTLEN(dhdp->osh, pktbuf) >= ETHER_HDR_LEN) {
		uint8 *pktdata = (uint8 *)PKTDATA(dhdp->osh, pktbuf);
		eh = (struct ether_header *)pktdata;

		if (ETHER_ISMULTI(eh->ether_dhost))
			dhdp->tx_multicast++;
		if (ntoh16(eh->ether_type) == ETHER_TYPE_802_1X)
			atomic_inc(&dhd->pend_8021x_cnt);
	} else {
			PKTFREE(dhd->pub.osh, pktbuf, TRUE);
			return BCME_ERROR;
	}

	/* Look into the packet and update the packet priority */
	if (PKTPRIO(pktbuf) == 0)
		pktsetprio(pktbuf, FALSE);

#ifdef PROP_TXSTATUS
	if (dhdp->wlfc_state) {
		/* store the interface ID */
		DHD_PKTTAG_SETIF(PKTTAG(pktbuf), ifidx);

		/* store destination MAC in the tag as well */
		DHD_PKTTAG_SETDSTN(PKTTAG(pktbuf), eh->ether_dhost);

		/* decide which FIFO this packet belongs to */
		if (ETHER_ISMULTI(eh->ether_dhost))
			/* one additional queue index (highest AC + 1) is used for bc/mc queue */
			DHD_PKTTAG_SETFIFO(PKTTAG(pktbuf), AC_COUNT);
		else
			DHD_PKTTAG_SETFIFO(PKTTAG(pktbuf), WME_PRIO2AC(PKTPRIO(pktbuf)));
	} else
#endif /* PROP_TXSTATUS */
	/* If the protocol uses a data header, apply it */
	dhd_prot_hdrpush(dhdp, ifidx, pktbuf);

	/* Use bus module to send data frame */
#ifdef WLMEDIA_HTSF
	dhd_htsf_addtxts(dhdp, pktbuf);
#endif
#ifdef BCMDBUS
#ifdef PROP_TXSTATUS
	if (dhdp->wlfc_state && ((athost_wl_status_info_t*)dhdp->wlfc_state)->proptxstatus_mode
			!= WLFC_FCMODE_NONE) {
		dhd_os_wlfc_block(dhdp);
		ret = dhd_wlfc_enque_sendq(dhdp->wlfc_state, DHD_PKTTAG_FIFO(PKTTAG(pktbuf)),
			pktbuf);
		dhd_wlfc_commit_packets(dhdp->wlfc_state,  (f_commitpkt_t)dhd_dbus_txdata,
			(void *)dhdp);
		if (((athost_wl_status_info_t*)dhdp->wlfc_state)->toggle_host_if) {
			((athost_wl_status_info_t*)dhdp->wlfc_state)->toggle_host_if = 0;
		}
		dhd_os_wlfc_unblock(dhdp);
	}
	else
		/* non-proptxstatus way */
		ret = dhd_dbus_txdata(dhdp, pktbuf);
#else
	ret = dhd_dbus_txdata(dhdp, pktbuf);
#endif /* PROP_TXSTATUS */
#else
#ifdef PROP_TXSTATUS
	if (dhdp->wlfc_state && ((athost_wl_status_info_t*)dhdp->wlfc_state)->proptxstatus_mode
		!= WLFC_FCMODE_NONE) {
		dhd_os_wlfc_block(dhdp);
		ret = dhd_wlfc_enque_sendq(dhdp->wlfc_state, DHD_PKTTAG_FIFO(PKTTAG(pktbuf)),
			pktbuf);
		dhd_wlfc_commit_packets(dhdp->wlfc_state,  (f_commitpkt_t)dhd_bus_txdata,
			dhdp->bus);
		if (((athost_wl_status_info_t*)dhdp->wlfc_state)->toggle_host_if) {
			((athost_wl_status_info_t*)dhdp->wlfc_state)->toggle_host_if = 0;
		}
		dhd_os_wlfc_unblock(dhdp);
	}
	else
		/* non-proptxstatus way */
	ret = dhd_bus_txdata(dhdp->bus, pktbuf);
#else
	ret = dhd_bus_txdata(dhdp->bus, pktbuf);
#endif /* PROP_TXSTATUS */

#endif /* BCMDBUS */
    if (ret) 
    {
        DHD_TRACE(("%s: 900: free packet during tx ret = %d, stop = %d\n", __FUNCTION__, ret, netif_queue_stopped(dhd->iflist[ifidx]->net)));

        /* we got error, need to free buff */
        PKTFREE(dhdp->osh, pktbuf, TRUE);
    }
	return ret;
}

int
dhd_start_xmit(struct sk_buff *skb, struct net_device *net)
{
	int ret;
	void *pktbuf;
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(net);
	int ifidx;
    dhd_if_t *ifp;
#ifdef WLMEDIA_HTSF
	uint8 htsfdlystat_sz = dhd->pub.htsfdlystat_sz;
#else
	uint8 htsfdlystat_sz = 0;
#endif

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	DHD_OS_WAKE_LOCK(&dhd->pub);

	/* Reject if down */
	if (!dhd->pub.up || (dhd->pub.busstate == DHD_BUS_DOWN)) {
		DHD_ERROR(("%s: xmit rejected pub.up=%d busstate=%d \n",
			__FUNCTION__, dhd->pub.up, dhd->pub.busstate));
		netif_stop_queue(net);
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20))
		return -ENODEV;
#else
		return NETDEV_TX_BUSY;
#endif
	}

	ifidx = dhd_net2idx(dhd, net);
	if (ifidx == DHD_BAD_IF) {
		DHD_ERROR(("%s: bad ifidx %d\n", __FUNCTION__, ifidx));
		netif_stop_queue(net);
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20))
		return -ENODEV;
#else
		return NETDEV_TX_BUSY;
#endif
	}
    /* added colletct stats according to intf */
    ifp = dhd->iflist[ifidx];
    if (ifp == NULL) {
        DHD_ERROR(("%s: ifp is NULL. drop packet\n",
            __FUNCTION__));
        netif_stop_queue(net);
        DHD_OS_WAKE_UNLOCK(&dhd->pub);
        return -ENODEV;
    }

	/* Make sure there's enough room for any header */

    if (skb_headroom(skb) < dhd->pub.hdrlen + htsfdlystat_sz + USB_ADDR_ALIGN) {
		struct sk_buff *skb2;

		DHD_INFO(("%s: insufficient headroom\n",
		          dhd_ifname(&dhd->pub, ifidx)));
		dhd->pub.tx_realloc++;

		skb2 = skb_realloc_headroom(skb, dhd->pub.hdrlen + htsfdlystat_sz);

		dev_kfree_skb(skb);
		if ((skb = skb2) == NULL) {
			DHD_ERROR(("%s: skb_realloc_headroom failed\n",
			           dhd_ifname(&dhd->pub, ifidx)));
			ret = -ENOMEM;
			goto done;
		}
	}

	/* Convert to packet */
	if (!(pktbuf = PKTFRMNATIVE(dhd->pub.osh, skb))) {
		DHD_ERROR(("%s: PKTFRMNATIVE failed\n",
		           dhd_ifname(&dhd->pub, ifidx)));
		dev_kfree_skb_any(skb);
		ret = -ENOMEM;
		goto done;
	}
#ifdef WLMEDIA_HTSF
	if (htsfdlystat_sz && PKTLEN(dhd->pub.osh, pktbuf) >= ETHER_ADDR_LEN) {
		uint8 *pktdata = (uint8 *)PKTDATA(dhd->pub.osh, pktbuf);
		struct ether_header *eh = (struct ether_header *)pktdata;

		if (!ETHER_ISMULTI(eh->ether_dhost) &&
			(ntoh16(eh->ether_type) == ETHER_TYPE_IP)) {
			eh->ether_type = hton16(ETHER_TYPE_BRCM_PKTDLYSTATS);
		}
	}
#endif

	ret = dhd_sendpkt(&dhd->pub, ifidx, pktbuf);


done:
    if (ret){
        dhd->pub.dstats.tx_dropped++;
        /*added by wangweichao, to collect stats*/
        ifp->stats.tx_dropped++;
    }
    else{

        dhd->pub.dstats.tx_bytes += PKTLEN(dhd->pub.osh,pktbuf);
        dhd->pub.tx_packets++;
        /*added to collect stats*/
        ifp->stats.tx_bytes += PKTLEN(dhd->pub.osh,pktbuf);
        ifp->stats.tx_packets++;

    }

	DHD_OS_WAKE_UNLOCK(&dhd->pub);

	/* Return ok: we always eat the packet */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20))
	return 0;
#else
	return NETDEV_TX_OK;
#endif
}

void
dhd_txflowcontrol(dhd_pub_t *dhdp, int ifidx, bool state)
{
	struct net_device *net;
	dhd_info_t *dhd = dhdp->info;
	int i;

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	ASSERT(dhd);
    DHD_INFO(("%s:  %s  -------\n", __FUNCTION__, (state == ON)?"ON":"OFF"));

	if (ifidx == ALL_INTERFACES) {
		/* Flow control on all active interfaces */
		dhdp->txoff = state;
		for (i = 0; i < DHD_MAX_IFS; i++) {
			if (dhd->iflist[i]) {
				net = dhd->iflist[i]->net;
				if (state == ON)
					netif_stop_queue(net);
				else
					netif_wake_queue(net);
			}
		}
	}
	else {
		if (dhd->iflist[ifidx]) {
			net = dhd->iflist[ifidx]->net;
			if (state == ON)
				netif_stop_queue(net);
			else
				netif_wake_queue(net);
		}
	}

    g_txoff = state;
}

#ifdef DHD_RX_DUMP
typedef struct {
	uint16 type;
	const char *str;
} PKTTYPE_INFO;

static const PKTTYPE_INFO packet_type_info[] =
{
	{ ETHER_TYPE_IP, "IP" },
	{ ETHER_TYPE_ARP, "ARP" },
	{ ETHER_TYPE_BRCM, "BRCM" },
	{ ETHER_TYPE_802_1X, "802.1X" },
	{ ETHER_TYPE_WAI, "WAPI" },
	{ 0, ""}
};

static const char *_get_packet_type_str(uint16 type)
{
	int i;
	int n = sizeof(packet_type_info)/sizeof(packet_type_info[1]) - 1;

	for (i = 0; i < n; i++) {
		if (packet_type_info[i].type == type)
			return packet_type_info[i].str;
	}

	return packet_type_info[n].str;
}
#endif /* DHD_RX_DUMP */

void
dhd_rx_frame(dhd_pub_t *dhdp, int ifidx, void *pktbuf, int numpkt, uint8 chan)
{
    struct net_device *net;
    unsigned long time_stamp = 0;
	dhd_info_t *dhd = (dhd_info_t *)dhdp->info;
	struct sk_buff *skb;
	uchar *eth;
	uint len;
	void *data, *pnext = NULL;
	int i;
	dhd_if_t *ifp;
	wl_event_msg_t event;
	int tout = DHD_PACKET_TIMEOUT_MS;

#ifdef DHD_RX_DUMP
#ifdef DHD_RX_FULL_DUMP
	int k;
#endif /* DHD_RX_FULL_DUMP */
	char *dump_data;
	uint16 protocol;
#endif /* DHD_RX_DUMP */
#ifdef HNDCTF
    unsigned int tmp_len = 0;
#endif

	BCM_REFERENCE(tout);
	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	for (i = 0; pktbuf && i < numpkt; i++, pktbuf = pnext) {
#ifdef WLBTAMP
		struct ether_header *eh;
		struct dot11_llc_snap_header *lsh;
#endif

		ifp = dhd->iflist[ifidx];
		if (ifp == NULL) {
			DHD_ERROR(("%s: ifp is NULL. drop packet\n",
				__FUNCTION__));
			PKTFREE(dhdp->osh, pktbuf, TRUE);
			continue;
		}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
		/* Dropping packets before registering net device to avoid kernel panic */
		if (!ifp->net || ifp->net->reg_state != NETREG_REGISTERED) {
			DHD_ERROR(("%s: net device is NOT registered yet. drop packet\n",
			__FUNCTION__));
			PKTFREE(dhdp->osh, pktbuf, TRUE);
			continue;
		}
#endif

        net = dhd->iflist[ifidx]->net;
        if (net) {

            /* check whether txoff == ON */
            if (test_bit(__QUEUE_STATE_XOFF, &(netdev_get_tx_queue(net, 0))->state)) {

                if (g_time_stamp == 0) {

                    /* mark the txoff == ON time */
                    g_time_stamp = jiffies;
                }
            }
            else {

                /* if no txoff == ON, reset the global mark */
                g_time_stamp = 0;
            }

            /* get current time */
            time_stamp = jiffies;

            /* check whether txoff == on > 1s */
            if (g_time_stamp && (time_stamp - g_time_stamp > 1 * HZ)) {

                g_txoff = OFF;
                netif_wake_queue(net);
            }
        }
		pnext = PKTNEXT(dhdp->osh, pktbuf);
		PKTSETNEXT(wl->sh.osh, pktbuf, NULL);

#ifdef WLBTAMP
		eh = (struct ether_header *)PKTDATA(wl->sh.osh, pktbuf);
		lsh = (struct dot11_llc_snap_header *)&eh[1];

		if ((ntoh16(eh->ether_type) < ETHER_TYPE_MIN) &&
		    (PKTLEN(wl->sh.osh, pktbuf) >= RFC1042_HDR_LEN) &&
		    bcmp(lsh, BT_SIG_SNAP_MPROT, DOT11_LLC_SNAP_HDR_LEN - 2) == 0 &&
		    lsh->type == HTON16(BTA_PROT_L2CAP)) {
			amp_hci_ACL_data_t *ACL_data = (amp_hci_ACL_data_t *)
			        ((uint8 *)eh + RFC1042_HDR_LEN);
			ACL_data = NULL;
		}
#endif /* WLBTAMP */

#ifdef PROP_TXSTATUS
		if (dhdp->wlfc_state && PKTLEN(wl->sh.osh, pktbuf) == 0) {
			/* WLFC may send header only packet when
			there is an urgent message but no packet to
			piggy-back on
			*/
			((athost_wl_status_info_t*)dhdp->wlfc_state)->stats.wlfc_header_only_pkt++;
			PKTFREE(dhdp->osh, pktbuf, TRUE);
			continue;
		}
#endif

		skb = PKTTONATIVE(dhdp->osh, pktbuf);
        /* the max size of skb packet is 1522, if size > 1522, drop it. */
        if (net && (skb->len > (net->mtu + net->hard_header_len + dhd->pub.hdrlen))) {

            DHD_ERROR(("%s: receive a large packet, size %u. drop it\n",
                        __FUNCTION__, skb->len));

            PKTFREE(dhdp->osh, pktbuf, TRUE);
            dhdp->rx_errors++;
            dhdp->dstats.rx_errors++;
            continue;
        }
		/* Get the protocol, maintain skb around eth_type_trans()
		 * The main reason for this hack is for the limitation of
		 * Linux 2.4 where 'eth_type_trans' uses the 'net->hard_header_len'
		 * to perform skb_pull inside vs ETH_HLEN. Since to avoid
		 * coping of the packet coming from the network stack to add
		 * BDC, Hardware header etc, during network interface registration
		 * we set the 'net->hard_header_len' to ETH_HLEN + extra space required
		 * for BDC, Hardware header etc. and not just the ETH_HLEN
		 */
		eth = skb->data;
		len = skb->len;

#ifdef DHD_RX_DUMP
		dump_data = skb->data;
		protocol = (dump_data[12] << 8) | dump_data[13];
		DHD_ERROR(("RX DUMP - %s\n", _get_packet_type_str(protocol)));

#ifdef DHD_RX_FULL_DUMP
		if (protocol != ETHER_TYPE_BRCM) {
			for (k = 0; k < skb->len; k++) {
				DHD_ERROR(("%02X ", dump_data[k]));
				if ((k & 15) == 15)
					DHD_ERROR(("\n"));
			}
			DHD_ERROR(("\n"));
		}
#endif /* DHD_RX_FULL_DUMP */

		if (protocol != ETHER_TYPE_BRCM) {
			if (dump_data[0] == 0xFF) {
				DHD_ERROR(("%s: BROADCAST\n", __FUNCTION__));

				if ((dump_data[12] == 8) &&
					(dump_data[13] == 6)) {
					DHD_ERROR(("%s: ARP %d\n",
						__FUNCTION__, dump_data[0x15]));
				}
			} else if (dump_data[0] & 1) {
				DHD_ERROR(("%s: MULTICAST: "
					"%02X:%02X:%02X:%02X:%02X:%02X\n",
					__FUNCTION__, dump_data[0],
					dump_data[1], dump_data[2],
					dump_data[3], dump_data[4],
					dump_data[5]));
			}

			if (protocol == ETHER_TYPE_802_1X) {
				DHD_ERROR(("ETHER_TYPE_802_1X: "
					"ver %d, type %d, replay %d\n",
					dump_data[14], dump_data[15],
					dump_data[30]));
			}
		}

#endif /* DHD_RX_DUMP */

		ifp = dhd->iflist[ifidx];
		if (ifp == NULL)
			ifp = dhd->iflist[0];

		ASSERT(ifp);
		skb->dev = ifp->net;
#ifdef HNDCTF
        tmp_len = skb->len - ETH_HLEN;
        if (unlikely(tmp_len < 0)) 
        {
            tmp_len = 0;
        }
        if (0 == g_ifContrlWan)
        {
            if (cih_dhd  && et_ctf_forward_dhd(skb) != BCME_ERROR)
            {
                dhdp->dstats.rx_bytes += tmp_len;
                dhdp->rx_packets++; /* Local count */
                ifp->stats.rx_bytes += tmp_len;
                ifp->stats.rx_packets++;
                if (skb->pkt_type == PACKET_MULTICAST) 
                {
                    dhd->pub.rx_multicast++;
                    ifp->stats.multicast++;
                }
                continue;
            }
        }
#endif
		skb->protocol = eth_type_trans(skb, skb->dev);

		if (skb->pkt_type == PACKET_MULTICAST) {
			dhd->pub.rx_multicast++;
            /*add to colletc the stats according intf */
            ifp->stats.multicast++;
		}

		skb->data = eth;
		skb->len = len;

#ifdef WLMEDIA_HTSF
		dhd_htsf_addrxts(dhdp, pktbuf);
#endif
		/* Strip header, count, deliver upward */
		skb_pull(skb, ETH_HLEN);

		/* Process special event packets and then discard them */
		if (ntoh16(skb->protocol) == ETHER_TYPE_BRCM) {
			dhd_wl_host_event(dhd, &ifidx,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 22)
			skb->mac_header,
#else
			skb->mac.raw,
#endif
			&event,
			&data);

#ifdef WLBTAMP
			wl_event_to_host_order(&event);
			if (event.event_type == WLC_E_BTA_HCI_EVENT) {
				dhd_bta_doevt(dhdp, data, event.datalen);
			}
			tout = DHD_EVENT_TIMEOUT_MS;
#endif /* WLBTAMP */
		}

		ASSERT(ifidx < DHD_MAX_IFS && dhd->iflist[ifidx]);
		if (dhd->iflist[ifidx] && !dhd->iflist[ifidx]->state)
			ifp = dhd->iflist[ifidx];

		if (ifp->net)
			ifp->net->last_rx = jiffies;

		dhdp->dstats.rx_bytes += skb->len;
		dhdp->rx_packets++; /* Local count */
        /*add to colletc the stats according intf */
        ifp->stats.rx_bytes += skb->len;
        ifp->stats.rx_packets++;

        if (dhd_netif_rx_flow_control()) {
            dev_kfree_skb_any(skb);
        }
        else {
        if (in_interrupt()) {
            netif_rx(skb);
        } else {
            /* If the receive is not processed inside an ISR,
            * the softirqd must be woken explicitly to service
            * the NET_RX_SOFTIRQ.  In 2.6 kernels, this is handled
            * by netif_rx_ni(), but in earlier kernels, we need
            * to do it manually.
            */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
            netif_rx_ni(skb);
#else
            ulong flags;
            netif_rx(skb);
            local_irq_save(flags);
            RAISE_RX_SOFTIRQ();
            local_irq_restore(flags);
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) */
            }
        }
	}
	DHD_OS_WAKE_LOCK_TIMEOUT_ENABLE(dhdp, tout);
}

void
dhd_event(struct dhd_info *dhd, char *evpkt, int evlen, int ifidx)
{
	/* Linux version has nothing to do */
	return;
}

void
dhd_txcomplete(dhd_pub_t *dhdp, void *txp, bool success)
{
	uint ifidx;
	dhd_info_t *dhd = (dhd_info_t *)(dhdp->info);
	struct ether_header *eh;
	uint16 type;
#ifdef WLBTAMP
	uint len;
#endif

	dhd_prot_hdrpull(dhdp, &ifidx, txp, NULL, NULL);

	eh = (struct ether_header *)PKTDATA(dhdp->osh, txp);
	type  = ntoh16(eh->ether_type);

	if (type == ETHER_TYPE_802_1X)
		atomic_dec(&dhd->pend_8021x_cnt);

#ifdef WLBTAMP
	/* Crack open the packet and check to see if it is BT HCI ACL data packet.
	 * If yes generate packet completion event.
	 */
	len = PKTLEN(dhdp->osh, txp);

	/* Generate ACL data tx completion event locally to avoid SDIO bus transaction */
	if ((type < ETHER_TYPE_MIN) && (len >= RFC1042_HDR_LEN)) {
		struct dot11_llc_snap_header *lsh = (struct dot11_llc_snap_header *)&eh[1];

		if (bcmp(lsh, BT_SIG_SNAP_MPROT, DOT11_LLC_SNAP_HDR_LEN - 2) == 0 &&
		    ntoh16(lsh->type) == BTA_PROT_L2CAP) {

			dhd_bta_tx_hcidata_complete(dhdp, txp, success);
		}
	}
#endif /* WLBTAMP */
}

static struct net_device_stats *
dhd_get_stats(struct net_device *net)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(net);
	dhd_if_t *ifp;
	int ifidx;

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	ifidx = dhd_net2idx(dhd, net);
	if (ifidx == DHD_BAD_IF) {
		DHD_ERROR(("%s: BAD_IF\n", __FUNCTION__));
		return NULL;
	}

	ifp = dhd->iflist[ifidx];
	ASSERT(dhd && ifp);

	if (dhd->pub.up) {
		/* Use the protocol to get dongle stats */
		dhd_prot_dstats(&dhd->pub);
	}
    
#ifndef WIFI_EXP_FEATURE
	/* Copy dongle stats to net device stats */
	ifp->stats.rx_packets = dhd->pub.dstats.rx_packets;
	ifp->stats.tx_packets = dhd->pub.dstats.tx_packets;
	ifp->stats.rx_bytes = dhd->pub.dstats.rx_bytes;
	ifp->stats.tx_bytes = dhd->pub.dstats.tx_bytes;
	ifp->stats.tx_dropped = dhd->pub.dstats.tx_dropped;
	ifp->stats.multicast = dhd->pub.dstats.multicast;
#endif
	ifp->stats.rx_errors = dhd->pub.dstats.rx_errors;
	ifp->stats.tx_errors = dhd->pub.dstats.tx_errors;
	ifp->stats.rx_dropped = dhd->pub.dstats.rx_dropped;

	return &ifp->stats;
}

#ifndef BCMDBUS

static void dhd_watchdog(ulong data)
{
	dhd_info_t *dhd = (dhd_info_t *)data;
	unsigned long flags;

	DHD_OS_WAKE_LOCK(&dhd->pub);
	if (dhd->pub.dongle_reset) {
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
		return;
	}


	dhd_os_sdlock(&dhd->pub);
	/* Call the bus module watchdog */
	dhd_bus_watchdog(&dhd->pub);

	flags = dhd_os_spin_lock(&dhd->pub);
	/* Count the tick for reference */
	dhd->pub.tickcnt++;

	/* Reschedule the watchdog */
	if (dhd->wd_timer_valid)
		mod_timer(&dhd->timer, jiffies + dhd_watchdog_ms * HZ / 1000);
	dhd_os_spin_unlock(&dhd->pub, flags);
	dhd_os_sdunlock(&dhd->pub);
	DHD_OS_WAKE_UNLOCK(&dhd->pub);
}


static void
dhd_dpc(ulong data)
{
	dhd_info_t *dhd;

	dhd = (dhd_info_t *)data;

	/* this (tasklet) can be scheduled in dhd_sched_dpc[dhd_linux.c]
	 * down below , wake lock is set,
	 * the tasklet is initialized in dhd_attach()
	 */
	/* Call bus dpc unless it indicated down (then clean stop) */
	if (dhd->pub.busstate != DHD_BUS_DOWN) {
		if (dhd_bus_dpc(dhd->pub.bus))
			tasklet_schedule(&dhd->tasklet);
		else
			DHD_OS_WAKE_UNLOCK(&dhd->pub);
	} else {
		dhd_bus_stop(dhd->pub.bus, TRUE);
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
	}
}

void
dhd_sched_dpc(dhd_pub_t *dhdp)
{
	dhd_info_t *dhd = (dhd_info_t *)dhdp->info;

	DHD_OS_WAKE_LOCK(dhdp);

	if (dhd->dhd_tasklet_create)
		tasklet_schedule(&dhd->tasklet);
}
#endif /* BCMDBUS */

#ifdef TOE
/* Retrieve current toe component enables, which are kept as a bitmap in toe_ol iovar */
static int
dhd_toe_get(dhd_info_t *dhd, int ifidx, uint32 *toe_ol)
{
	wl_ioctl_t ioc;
	char buf[32];
	int ret;

	memset(&ioc, 0, sizeof(ioc));

	ioc.cmd = WLC_GET_VAR;
	ioc.buf = buf;
	ioc.len = (uint)sizeof(buf);
	ioc.set = FALSE;

	strncpy(buf, "toe_ol", sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';
	if ((ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len)) < 0) {
		/* Check for older dongle image that doesn't support toe_ol */
		if (ret == -EIO) {
			DHD_ERROR(("%s: toe not supported by device\n",
				dhd_ifname(&dhd->pub, ifidx)));
			return -EOPNOTSUPP;
		}

		DHD_INFO(("%s: could not get toe_ol: ret=%d\n", dhd_ifname(&dhd->pub, ifidx), ret));
		return ret;
	}

	memcpy(toe_ol, buf, sizeof(uint32));
	return 0;
}

/* Set current toe component enables in toe_ol iovar, and set toe global enable iovar */
static int
dhd_toe_set(dhd_info_t *dhd, int ifidx, uint32 toe_ol)
{
	wl_ioctl_t ioc;
	char buf[32];
	int toe, ret;

	memset(&ioc, 0, sizeof(ioc));

	ioc.cmd = WLC_SET_VAR;
	ioc.buf = buf;
	ioc.len = (uint)sizeof(buf);
	ioc.set = TRUE;

	/* Set toe_ol as requested */

	strncpy(buf, "toe_ol", sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';
	memcpy(&buf[sizeof("toe_ol")], &toe_ol, sizeof(uint32));

	if ((ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len)) < 0) {
		DHD_ERROR(("%s: could not set toe_ol: ret=%d\n",
			dhd_ifname(&dhd->pub, ifidx), ret));
		return ret;
	}

	/* Enable toe globally only if any components are enabled. */

	toe = (toe_ol != 0);

	strcpy(buf, "toe");
	memcpy(&buf[sizeof("toe")], &toe, sizeof(uint32));

	if ((ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len)) < 0) {
		DHD_ERROR(("%s: could not set toe: ret=%d\n", dhd_ifname(&dhd->pub, ifidx), ret));
		return ret;
	}

	return 0;
}
#endif /* TOE */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
static void
dhd_ethtool_get_drvinfo(struct net_device *net, struct ethtool_drvinfo *info)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(net);

	snprintf(info->driver, sizeof(info->driver), "wl");
	snprintf(info->version, sizeof(info->version), "%lu", dhd->pub.drv_version);
}

struct ethtool_ops dhd_ethtool_ops = {
	.get_drvinfo = dhd_ethtool_get_drvinfo
};
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24) */


#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 2)
static int
dhd_ethtool(dhd_info_t *dhd, void *uaddr)
{
	struct ethtool_drvinfo info;
	char drvname[sizeof(info.driver)];
	uint32 cmd;
#ifdef TOE
	struct ethtool_value edata;
	uint32 toe_cmpnt, csum_dir;
	int ret;
#endif

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	/* all ethtool calls start with a cmd word */
	if (copy_from_user(&cmd, uaddr, sizeof (uint32)))
		return -EFAULT;

	switch (cmd) {
	case ETHTOOL_GDRVINFO:
		/* Copy out any request driver name */
		if (copy_from_user(&info, uaddr, sizeof(info)))
			return -EFAULT;
		strncpy(drvname, info.driver, sizeof(info.driver));
		drvname[sizeof(info.driver)-1] = '\0';

		/* clear struct for return */
		memset(&info, 0, sizeof(info));
		info.cmd = cmd;

		/* if dhd requested, identify ourselves */
		if (strcmp(drvname, "?dhd") == 0) {
			snprintf(info.driver, sizeof(info.driver), "dhd");
			strncpy(info.version, EPI_VERSION_STR, sizeof(info.version) - 1);
			info.version[sizeof(info.version) - 1] = '\0';
		}

		/* otherwise, require dongle to be up */
		else if (!dhd->pub.up) {
			DHD_ERROR(("%s: dongle is not up\n", __FUNCTION__));
			return -ENODEV;
		}

		/* finally, report dongle driver type */
		else if (dhd->pub.iswl)
			snprintf(info.driver, sizeof(info.driver), "wl");
		else
			snprintf(info.driver, sizeof(info.driver), "xx");

		snprintf(info.version, sizeof(info.version), "%lu", dhd->pub.drv_version);
		if (copy_to_user(uaddr, &info, sizeof(info)))
			return -EFAULT;
		DHD_CTL(("%s: given %*s, returning %s\n", __FUNCTION__,
		         (int)sizeof(drvname), drvname, info.driver));
		break;

#ifdef TOE
	/* Get toe offload components from dongle */
	case ETHTOOL_GRXCSUM:
	case ETHTOOL_GTXCSUM:
		if ((ret = dhd_toe_get(dhd, 0, &toe_cmpnt)) < 0)
			return ret;

		csum_dir = (cmd == ETHTOOL_GTXCSUM) ? TOE_TX_CSUM_OL : TOE_RX_CSUM_OL;

		edata.cmd = cmd;
		edata.data = (toe_cmpnt & csum_dir) ? 1 : 0;

		if (copy_to_user(uaddr, &edata, sizeof(edata)))
			return -EFAULT;
		break;

	/* Set toe offload components in dongle */
	case ETHTOOL_SRXCSUM:
	case ETHTOOL_STXCSUM:
		if (copy_from_user(&edata, uaddr, sizeof(edata)))
			return -EFAULT;

		/* Read the current settings, update and write back */
		if ((ret = dhd_toe_get(dhd, 0, &toe_cmpnt)) < 0)
			return ret;

		csum_dir = (cmd == ETHTOOL_STXCSUM) ? TOE_TX_CSUM_OL : TOE_RX_CSUM_OL;

		if (edata.data != 0)
			toe_cmpnt |= csum_dir;
		else
			toe_cmpnt &= ~csum_dir;

		if ((ret = dhd_toe_set(dhd, 0, toe_cmpnt)) < 0)
			return ret;

		/* If setting TX checksum mode, tell Linux the new mode */
		if (cmd == ETHTOOL_STXCSUM) {
			if (edata.data)
				dhd->iflist[0]->net->features |= NETIF_F_IP_CSUM;
			else
				dhd->iflist[0]->net->features &= ~NETIF_F_IP_CSUM;
		}

		break;
#endif /* TOE */

	default:
		return -EOPNOTSUPP;
	}

	return 0;
}
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 2) */

static bool dhd_check_hang(struct net_device *net, dhd_pub_t *dhdp, int error)
{
	return FALSE;
}
#if defined(DHD_UPDATE_INTF_MAC)
extern int dhd_ap_mode;
int dhd_mbss_mode = 0;
#endif
static int
dhd_ioctl_entry(struct net_device *net, struct ifreq *ifr, int cmd)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(net);
	dhd_ioctl_t ioc;
	int bcmerror = 0;
	int buflen = 0;
	void *buf = NULL;
	uint driver = 0;
	int ifidx;
	int ret;

	DHD_OS_WAKE_LOCK(&dhd->pub);


	ifidx = dhd_net2idx(dhd, net);
	DHD_TRACE(("%s: ifidx %d, cmd 0x%04x\n", __FUNCTION__, ifidx, cmd));

	if (ifidx == DHD_BAD_IF) {
		DHD_ERROR(("%s: BAD IF\n", __FUNCTION__));
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
		return -1;
	}

#if defined(CONFIG_WIRELESS_EXT)
	/* linux wireless extensions */
	if ((cmd >= SIOCIWFIRST) && (cmd <= SIOCIWLAST)) {
		/* may recurse, do NOT lock */
		ret = wl_iw_ioctl(net, ifr, cmd);
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
		return ret;
	}
#endif /* defined(CONFIG_WIRELESS_EXT) */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 2)
	if (cmd == SIOCETHTOOL) {
		ret = dhd_ethtool(dhd, (void*)ifr->ifr_data);
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
		return ret;
	}
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 2) */
    if (cmd == SIOCDEVPRIVATE+1) {
        ret = wl_android_priv_cmd(net, ifr, cmd);
        dhd_check_hang(net, &dhd->pub, ret);
        DHD_OS_WAKE_UNLOCK(&dhd->pub);
        return ret;
    }

	if (cmd != SIOCDEVPRIVATE) {
		DHD_OS_WAKE_UNLOCK(&dhd->pub);
		return -EOPNOTSUPP;
	}

	memset(&ioc, 0, sizeof(ioc));

	/* Copy the ioc control structure part of ioctl request */
	if (copy_from_user(&ioc, ifr->ifr_data, sizeof(wl_ioctl_t))) {
		bcmerror = -BCME_BADADDR;
		goto done;
	}

	/* Copy out any buffer passed */
	if (ioc.buf) {
		if (ioc.len == 0) {
			DHD_TRACE(("%s: ioc.len=0, returns BCME_BADARG \n", __FUNCTION__));
			bcmerror = -BCME_BADARG;
			goto done;
		}
		buflen = MIN(ioc.len, DHD_IOCTL_MAXLEN);
		/* optimization for direct ioctl calls from kernel */
		/*
		if (segment_eq(get_fs(), KERNEL_DS)) {
			buf = ioc.buf;
		} else {
		*/
		{
			if (!(buf = (char*)MALLOC(dhd->pub.osh, buflen))) {
				bcmerror = -BCME_NOMEM;
				goto done;
			}
			if (copy_from_user(buf, ioc.buf, buflen)) {
				bcmerror = -BCME_BADADDR;
				goto done;
			}
		}
	}

	/* To differentiate between wl and dhd read 4 more byes */
	if ((copy_from_user(&driver, (char *)ifr->ifr_data + sizeof(wl_ioctl_t),
		sizeof(uint)) != 0)) {
		bcmerror = -BCME_BADADDR;
		goto done;
	}

	if (!capable(CAP_NET_ADMIN)) {
		bcmerror = -BCME_EPERM;
		goto done;
	}

	/* check for local dhd ioctl and handle it */
	if (driver == DHD_IOCTL_MAGIC) {
		bcmerror = dhd_ioctl((void *)&dhd->pub, &ioc, buf, buflen);
		if (bcmerror)
			dhd->pub.bcmerror = bcmerror;
		goto done;
	}

#ifndef BCMDBUS
	/* send to dongle (must be up, and wl). */
	if (dhd->pub.busstate != DHD_BUS_DATA) {
		if ((ret = dhd_bus_start(&dhd->pub)) != 0) {
			DHD_ERROR(("%s: failed with code %d\n", __FUNCTION__, ret));
			bcmerror = BCME_DONGLE_DOWN;
			goto done;
		}
	}

	if (!dhd->pub.iswl) {
		bcmerror = BCME_DONGLE_DOWN;
		goto done;
	}
#endif /* BCMDBUS */
    DHD_CTL(("%s:800: ioc.cmd = %d\n", __FUNCTION__, ioc.cmd));
    if(ioc.buf == NULL)
    {
        DHD_CTL(("%s:1000: ioc.buf = NULL\n", __FUNCTION__));
    }
	/*
	 * Flush the TX queue if required for proper message serialization:
	 * Intercept WLC_SET_KEY IOCTL - serialize M4 send and set key IOCTL to
	 * prevent M4 encryption and
	 * intercept WLC_DISASSOC IOCTL - serialize WPS-DONE and WLC_DISASSOC IOCTL to
	 * prevent disassoc frame being sent before WPS-DONE frame.
	 */
	if (ioc.cmd == WLC_SET_KEY ||
	    (ioc.cmd == WLC_SET_VAR && ioc.buf != NULL &&
	     strncmp("wsec_key", ioc.buf, 9) == 0) ||
	    (ioc.cmd == WLC_SET_VAR && ioc.buf != NULL &&
	     strncmp("bsscfg:wsec_key", ioc.buf, 15) == 0) ||
	    ioc.cmd == WLC_DISASSOC)
		dhd_wait_pend8021x(net);

#ifdef WLMEDIA_HTSF
	if (ioc.buf) {
		/*  short cut wl ioctl calls here  */
		if (strcmp("htsf", ioc.buf) == 0) {
			dhd_ioctl_htsf_get(dhd, 0);
			return BCME_OK;
		}

		if (strcmp("htsflate", ioc.buf) == 0) {
			if (ioc.set) {
				memset(ts, 0, sizeof(tstamp_t)*TSMAX);
				memset(&maxdelayts, 0, sizeof(tstamp_t));
				maxdelay = 0;
				tspktcnt = 0;
				maxdelaypktno = 0;
				memset(&vi_d1.bin, 0, sizeof(uint32)*NUMBIN);
				memset(&vi_d2.bin, 0, sizeof(uint32)*NUMBIN);
				memset(&vi_d3.bin, 0, sizeof(uint32)*NUMBIN);
				memset(&vi_d4.bin, 0, sizeof(uint32)*NUMBIN);
			} else {
				dhd_dump_latency();
			}
			return BCME_OK;
		}
		if (strcmp("htsfclear", ioc.buf) == 0) {
			memset(&vi_d1.bin, 0, sizeof(uint32)*NUMBIN);
			memset(&vi_d2.bin, 0, sizeof(uint32)*NUMBIN);
			memset(&vi_d3.bin, 0, sizeof(uint32)*NUMBIN);
			memset(&vi_d4.bin, 0, sizeof(uint32)*NUMBIN);
			htsf_seqnum = 0;
			return BCME_OK;
		}
		if (strcmp("htsfhis", ioc.buf) == 0) {
			dhd_dump_htsfhisto(&vi_d1, "H to D");
			dhd_dump_htsfhisto(&vi_d2, "D to D");
			dhd_dump_htsfhisto(&vi_d3, "D to H");
			dhd_dump_htsfhisto(&vi_d4, "H to H");
			return BCME_OK;
		}
		if (strcmp("tsport", ioc.buf) == 0) {
			if (ioc.set) {
				memcpy(&tsport, ioc.buf + 7, 4);
			} else {
				DHD_ERROR(("current timestamp port: %d \n", tsport));
			}
			return BCME_OK;
		}
	}
#endif /* WLMEDIA_HTSF */

	if ((ioc.cmd == WLC_SET_VAR || ioc.cmd == WLC_GET_VAR) &&
		ioc.buf != NULL && strncmp("rpc_", ioc.buf, 4) == 0) {
#ifdef BCM_FD_AGGR
		bcmerror = dhd_fdaggr_ioctl(&dhd->pub, ifidx, (wl_ioctl_t *)&ioc, buf, buflen);
#else
		bcmerror = BCME_UNSUPPORTED;
#endif
		goto done;
	}

#if defined(DHD_UPDATE_INTF_MAC)
    if ((ioc.cmd == WLC_SET_VAR && buf != NULL &&
         strncmp("apsta", buf, 5) == 0)/*���ַ���apsta��һλ��0�Ƚ�*/
         || (ioc.cmd == WLC_SET_VAR && buf != NULL &&
         strncmp("mbss", buf, 4) == 0))/*���ַ���mbss��һλ��0�Ƚ�*/
    {
        struct ether_addr macA;
        struct ether_addr macB;
        unsigned char e5_nv_mac[32];/*���ַ������С����Ϊ32*/

        wifi_get_nvmac_addr(e5_nv_mac);

        bcm_ether_atoe(e5_nv_mac, &macA);
        bcm_ether_atoe(e5_nv_mac, &macB);

        set_mac_add1_or2(&macA);
        dhd_ap_mode = *(int *)((char*)buf + 6);/*��ȡapsta��һλֵ*/
        dhd_mbss_mode = *(int *)((char*)buf + 5);/*��ȡmbss��һλֵ*/

        if(dhd_ap_mode || dhd_mbss_mode)
        {
            _dhd_set_mac_address(dhd, ifidx,&macB);
        }
        else
        {
            _dhd_set_mac_address(dhd, ifidx,&macA);
        }
    }
#endif
	bcmerror = dhd_wl_ioctl(&dhd->pub, ifidx, (wl_ioctl_t *)&ioc, buf, buflen);

done:

	if (!bcmerror && buf && ioc.buf) {
		if (copy_to_user(ioc.buf, buf, buflen))
			bcmerror = -EFAULT;
        if(ioc.cmd == WLC_GET_ASSOCLIST)
        {
            struct maclist *mc = (struct maclist *)buf;
            g_ap_sta_num = (int)mc->count;
        }
    }

	if (buf)
		MFREE(dhd->pub.osh, buf, buflen);

	DHD_OS_WAKE_UNLOCK(&dhd->pub);
	return OSL_ERROR(bcmerror);
}
static int set_mac_add1_or2(struct ether_addr *pmacaddr)/*lint !e528*/
{
    int ret = 0;
    int sprint_len = 0;
    char mac_addr_buf[32] = {0};/*���ַ������С���ó�32*/

    printk("%s Enter\n", __FUNCTION__);

    /*����0.1.2.3.4.5�ֱ���mac��ַ��λ��*/
    sprint_len = snprintf(mac_addr_buf, sizeof(mac_addr_buf), 
        "%02x:%02x:%02x:%02x:%02x:%02x", 
        pmacaddr->octet[0], pmacaddr->octet[1], pmacaddr->octet[2], 
        pmacaddr->octet[3], pmacaddr->octet[4], pmacaddr->octet[5]);
    if (0 > sprint_len)
    {
        DHD_ERROR(("Copy MAC str failed \n"));
    }
    printk("Current MAC is %s\n", mac_addr_buf);

    pmacaddr->octet[5] = pmacaddr->octet[5] + 1;//�Ͳ�ͨ��firmware�е��㷨һ��
    pmacaddr->octet[0] = pmacaddr->octet[0] | 2;//�Ͳ�ͨ��firmware�е��㷨һ��

    /*����0.1.2.3.4.5�ֱ���mac��ַ��λ��*/
    sprint_len = snprintf(mac_addr_buf, sizeof(mac_addr_buf), 
        "%02x:%02x:%02x:%02x:%02x:%02x", 
        pmacaddr->octet[0], pmacaddr->octet[1], pmacaddr->octet[2], 
        pmacaddr->octet[3], pmacaddr->octet[4], pmacaddr->octet[5]);
    if (0 > sprint_len)
    {
        DHD_ERROR(("Copy MAC str failed \n"));
    }
    printk("Modified MAC is %s\n", mac_addr_buf);
       
    return ret;

}

#ifdef WL_CFG80211
static int
dhd_cleanup_virt_ifaces(dhd_info_t *dhd)
{
	int i = 1; /* Leave ifidx 0 [Primary Interface] */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	int rollback_lock = FALSE;
#endif

	DHD_TRACE(("%s: Enter \n", __func__));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	/* release lock for unregister_netdev */
	if (rtnl_is_locked()) {
		rtnl_unlock();
		rollback_lock = TRUE;
	}
#endif

	for (i = 1; i < DHD_MAX_IFS; i++) {
		if (dhd->iflist[i]) {
			DHD_TRACE(("Deleting IF: %d \n", i));
			if ((dhd->iflist[i]->state != DHD_IF_DEL) &&
				(dhd->iflist[i]->state != DHD_IF_DELETING)) {
				dhd->iflist[i]->state = DHD_IF_DEL;
				dhd->iflist[i]->idx = i;
				dhd_net_if_lock_local(dhd);
				dhd_op_if(dhd->iflist[i]);
				dhd_net_if_unlock_local(dhd);
			}
		}
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	if (rollback_lock)
		rtnl_lock();
#endif

	return 0;
}
#endif /* WL_CFG80211 */

static int
dhd_stop(struct net_device *net)
{
	int ifidx;
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(net);
	DHD_OS_WAKE_LOCK(&dhd->pub);
	DHD_TRACE(("%s: Enter %p\n", __FUNCTION__, net));
	if (dhd->pub.up == 0) {
		goto exit;
	}
	ifidx = dhd_net2idx(dhd, net);
	BCM_REFERENCE(ifidx);

#ifdef WL_CFG80211
	if (ifidx == 0) {
		wl_cfg80211_down(NULL);

		/*
		 * For CFG80211: Clean up all the left over virtual interfaces
		 * when the primary Interface is brought down. [ifconfig wlan0 down]
		 */
		if ((dhd->dhd_state & DHD_ATTACH_STATE_ADD_IF) &&
			(dhd->dhd_state & DHD_ATTACH_STATE_CFG80211)) {
			dhd_cleanup_virt_ifaces(dhd);
		}
	}
#endif

#ifdef PROP_TXSTATUS
	dhd_wlfc_cleanup(&dhd->pub);
#endif
	/* Set state and stop OS transmissions */
	dhd->pub.up = 0;
	netif_stop_queue(net);

	/* Stop the protocol module */
	dhd_prot_stop(&dhd->pub);

	dhd->pub.hang_was_sent = 0;
	dhd->pub.rxcnt_timeout = 0;
	dhd->pub.txcnt_timeout = 0;
	OLD_MOD_DEC_USE_COUNT;
exit:
	DHD_OS_WAKE_UNLOCK(&dhd->pub);
	return 0;
}

static int
dhd_open(struct net_device *net)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(net);
#ifdef TOE
	uint32 toe_ol;
#endif
	int ifidx;
	int32 ret = 0;
	DHD_OS_WAKE_LOCK(&dhd->pub);
	/* Update FW path if it was changed */
	if (strlen(firmware_path) != 0) {
		if (firmware_path[strlen(firmware_path)-1] == '\n')
			firmware_path[strlen(firmware_path)-1] = '\0';
		strncpy(fw_path, firmware_path, sizeof(fw_path)-1);
		fw_path[sizeof(fw_path)-1] = '\0';
		firmware_path[0] = '\0';
	}


	ifidx = dhd_net2idx(dhd, net);
	DHD_TRACE(("%s: ifidx %d\n", __FUNCTION__, ifidx));

	if (ifidx < 0) {
		DHD_ERROR(("%s: Error: called with invalid IF\n", __FUNCTION__));
		ret = -1;
		goto exit;
	}

	if (!dhd->iflist[ifidx] || dhd->iflist[ifidx]->state == DHD_IF_DEL) {
		DHD_ERROR(("%s: Error: called when IF already deleted\n", __FUNCTION__));
		ret = -1;
		goto exit;
	}

	if (ifidx == 0) {
		atomic_set(&dhd->pend_8021x_cnt, 0);

		if (dhd->pub.busstate != DHD_BUS_DATA) {

#ifndef BCMDBUS
			/* try to bring up bus */
			if ((ret = dhd_bus_start(&dhd->pub)) != 0) {
				DHD_ERROR(("%s: failed with code %d\n", __FUNCTION__, ret));
				ret = -1;
				goto exit;
			}
#else /* BCMDBUS */
			if ((ret = dbus_up(dhd->pub.dbus)) != 0)
				return ret;
			else
				dhd->pub.busstate = DHD_BUS_DATA;

			/* Bus is ready, do any protocol initialization */
			if ((ret = dhd_prot_init(&dhd->pub)) < 0) {
				DHD_ERROR(("%s: failed with code %d\n", __FUNCTION__, ret));
				return ret;
			}
#endif /* BCMDBUS */

		}

		/* dhd_prot_init has been called in dhd_bus_start or wl_android_wifi_on */
		memcpy(net->dev_addr, dhd->pub.mac.octet, ETHER_ADDR_LEN);

#ifdef TOE
		/* Get current TOE mode from dongle */
		if (dhd_toe_get(dhd, ifidx, &toe_ol) >= 0 && (toe_ol & TOE_TX_CSUM_OL) != 0)
			dhd->iflist[ifidx]->net->features |= NETIF_F_IP_CSUM;
		else
			dhd->iflist[ifidx]->net->features &= ~NETIF_F_IP_CSUM;
#endif /* TOE */

#if defined(WL_CFG80211)
		if (unlikely(wl_cfg80211_up(NULL))) {
			DHD_ERROR(("%s: failed to bring up cfg80211\n", __FUNCTION__));
			ret = -1;
			goto exit;
		}
#endif /* WL_CFG80211 */
	}

	/* Allow transmit calls */
	netif_start_queue(net);
	dhd->pub.up = 1;


#ifdef BCMDBGFS
	dhd_dbg_init(&dhd->pub);
#endif

	OLD_MOD_INC_USE_COUNT;
exit:
	DHD_OS_WAKE_UNLOCK(&dhd->pub);
	return ret;
}

int dhd_do_driver_init(struct net_device *net)
{
	dhd_info_t *dhd = NULL;

	if (!net) {
		DHD_ERROR(("Primary Interface not initialized \n"));
		return -EINVAL;
	}

	dhd = *(dhd_info_t **)netdev_priv(net);

	/* If driver is already initialized, do nothing
	 */
	if (dhd->pub.busstate == DHD_BUS_DATA) {
		DHD_TRACE(("Driver already Inititalized. Nothing to do"));
		return 0;
	}

	if (dhd_open(net) < 0) {
		DHD_ERROR(("Driver Init Failed \n"));
		return -1;
	}

	return 0;
}

osl_t *
dhd_osl_attach(void *pdev, uint bustype)
{
	return osl_attach(pdev, bustype, TRUE);
}

void
dhd_osl_detach(osl_t *osh)
{
	if (MALLOCED(osh)) {
		DHD_ERROR(("%s: MEMORY LEAK %d bytes\n", __FUNCTION__, MALLOCED(osh)));
	}
	osl_detach(osh);
#if 0 && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	dhd_registration_check = FALSE;
	up(&dhd_registration_sem);
#if	defined(BCMLXSDMMC)
	up(&dhd_chipup_sem);
#endif
#endif 
}

int
dhd_add_if(dhd_info_t *dhd, int ifidx, void *handle, char *name,
	uint8 *mac_addr, uint32 flags, uint8 bssidx)
{
	dhd_if_t *ifp;

	DHD_TRACE(("%s: idx %d, handle->%p\n", __FUNCTION__, ifidx, handle));

	ASSERT(dhd && (ifidx < DHD_MAX_IFS));

	ifp = dhd->iflist[ifidx];
	if (ifp != NULL) {
		if (ifp->net != NULL) {
			netif_stop_queue(ifp->net);
			unregister_netdev(ifp->net);
			free_netdev(ifp->net);
		}
	} else
		if ((ifp = MALLOC(dhd->pub.osh, sizeof(dhd_if_t))) == NULL) {
			DHD_ERROR(("%s: OOM - dhd_if_t\n", __FUNCTION__));
			return -ENOMEM;
		}

	memset(ifp, 0, sizeof(dhd_if_t));
	ifp->event2cfg80211 = FALSE;
	ifp->info = dhd;
	dhd->iflist[ifidx] = ifp;
	strncpy(ifp->name, name, IFNAMSIZ);
	ifp->name[IFNAMSIZ] = '\0';
	if (mac_addr != NULL)
		memcpy(&ifp->mac_addr, mac_addr, ETHER_ADDR_LEN);

	if (handle == NULL) {
		ifp->state = DHD_IF_ADD;
		ifp->idx = ifidx;
		ifp->bssidx = bssidx;
		ASSERT(dhd->thr_sysioc_ctl.thr_pid >= 0);
		up(&dhd->thr_sysioc_ctl.sema);
	} else
		ifp->net = (struct net_device *)handle;

	if (ifidx == 0) {
		ifp->event2cfg80211 = TRUE;
	}

	return 0;
}

void
dhd_del_if(dhd_info_t *dhd, int ifidx)
{
	dhd_if_t *ifp;

	DHD_TRACE(("%s: idx %d\n", __FUNCTION__, ifidx));

	ASSERT(dhd && ifidx && (ifidx < DHD_MAX_IFS));
	ifp = dhd->iflist[ifidx];
	if (!ifp) {
		DHD_ERROR(("%s: Null interface\n", __FUNCTION__));
		return;
	}

	ifp->state = DHD_IF_DEL;
	ifp->idx = ifidx;
	ASSERT(dhd->thr_sysioc_ctl.thr_pid >= 0);
	up(&dhd->thr_sysioc_ctl.sema);
}
#if defined(DHD_UPDATE_INTF_MAC)
void
dhd_update_mac_if(dhd_info_t *dhd, int ifidx)
{
    dhd_if_t *ifp;

    ASSERT(dhd && ifidx && (ifidx < DHD_MAX_IFS));
    ifp = dhd->iflist[ifidx];
    if (!ifp) 
    {
        DHD_ERROR(("%s: Null interface %d\n", __FUNCTION__,ifidx));
        return;
    }

    DHD_EVENT(("%s: idx %d,is local:%d\n", __FUNCTION__, ifidx,ETHER_IS_LOCALADDR(ifp->mac_addr)));

    if ( !ETHER_IS_LOCALADDR(ifp->mac_addr ) )
    {
        ifp->state = DHD_IF_CHANGE;
        ASSERT(dhd->thr_sysioc_ctl.thr_pid >= 0);
        up(&dhd->thr_sysioc_ctl.sema);
    }
}
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31))
static struct net_device_ops dhd_ops_pri = {
	.ndo_open = dhd_open,
	.ndo_stop = dhd_stop,
	.ndo_get_stats = dhd_get_stats,
	.ndo_do_ioctl = dhd_ioctl_entry,
	.ndo_start_xmit = dhd_start_xmit,
	.ndo_set_mac_address = dhd_set_mac_address,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0))
	.ndo_set_rx_mode = dhd_set_multicast_list,
#else
	.ndo_set_multicast_list = dhd_set_multicast_list,
#endif
};

static struct net_device_ops dhd_ops_virt = {
	.ndo_get_stats = dhd_get_stats,
	.ndo_do_ioctl = dhd_ioctl_entry,
	.ndo_start_xmit = dhd_start_xmit,
	.ndo_set_mac_address = dhd_set_mac_address,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0))
	.ndo_set_rx_mode = dhd_set_multicast_list,
#else
	.ndo_set_multicast_list = dhd_set_multicast_list,
#endif
};
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)) */

dhd_pub_t *
dhd_attach(osl_t *osh, struct dhd_bus *bus, uint bus_hdrlen)
{
	dhd_info_t *dhd = NULL;
	struct net_device *net = NULL;

	dhd_attach_states_t dhd_state = DHD_ATTACH_STATE_INIT;
	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	/* updates firmware nvram path if it was provided as module parameters */
	if (strlen(firmware_path) != 0) {
		strncpy(fw_path, firmware_path, sizeof(fw_path) - 1);
		fw_path[sizeof(fw_path) - 1] = '\0';
	}
	if (strlen(nvram_path) != 0) {
		strncpy(nv_path, nvram_path, sizeof(nv_path) -1);
		nv_path[sizeof(nv_path) -1] = '\0';
	}

	/* Allocate etherdev, including space for private structure */
	if (!(net = alloc_etherdev(sizeof(dhd)))) {
		DHD_ERROR(("%s: OOM - alloc_etherdev\n", __FUNCTION__));
		goto fail;
	}
	dhd_state |= DHD_ATTACH_STATE_NET_ALLOC;

	/* Allocate primary dhd_info */
	if (!(dhd = MALLOC(osh, sizeof(dhd_info_t)))) {
		DHD_ERROR(("%s: OOM - alloc dhd_info\n", __FUNCTION__));
		goto fail;
	}
	memset(dhd, 0, sizeof(dhd_info_t));

#ifndef BCMDBUS
	dhd->dhd_tasklet_create = FALSE;
#endif /* BCMDBUS */
	dhd->thr_sysioc_ctl.thr_pid = DHD_PID_KT_INVALID;
	dhd_state |= DHD_ATTACH_STATE_DHD_ALLOC;

	/*
	 * Save the dhd_info into the priv
	 */
	memcpy((void *)netdev_priv(net), &dhd, sizeof(dhd));
	dhd->pub.osh = osh;

	/* Link to info module */
	dhd->pub.info = dhd;
	/* Link to bus module */
	dhd->pub.bus = bus;
	dhd->pub.hdrlen = bus_hdrlen;

	/* Set network interface name if it was provided as module parameter */
	if (iface_name[0]) {
		int len;
		char ch;
		strncpy(net->name, iface_name, IFNAMSIZ);
		net->name[IFNAMSIZ - 1] = 0;
		len = strlen(net->name);
		ch = net->name[len - 1];
		if ((ch > '9' || ch < '0') && (len < IFNAMSIZ - 2))
			strcat(net->name, "%d");
	}

	if (dhd_add_if(dhd, 0, (void *)net, net->name, NULL, 0, 0) == DHD_BAD_IF)
		goto fail;
	dhd_state |= DHD_ATTACH_STATE_ADD_IF;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31))
	net->open = NULL;
#else
	net->netdev_ops = NULL;
#endif

	sema_init(&dhd->proto_sem, 1);

#ifdef PROP_TXSTATUS
	spin_lock_init(&dhd->wlfc_spinlock);
#if defined(BCMDBUS)
	dhd->pub.wlfc_enabled = TRUE;
#else
	dhd->pub.wlfc_enabled = FALSE;
#endif 
#endif /* PROP_TXSTATUS */

	/* Initialize other structure content */
	init_waitqueue_head(&dhd->ioctl_resp_wait);
	init_waitqueue_head(&dhd->ctrl_wait);

	/* Initialize the spinlocks */
	spin_lock_init(&dhd->sdlock);
	spin_lock_init(&dhd->txqlock);
	spin_lock_init(&dhd->dhd_lock);

	/* Initialize Wakelock stuff */
	spin_lock_init(&dhd->wakelock_spinlock);
	dhd->wakelock_counter = 0;
	dhd->wakelock_timeout_enable = 0;
#ifdef CONFIG_HAS_WAKELOCK
	wake_lock_init(&dhd->wl_wifi, WAKE_LOCK_SUSPEND, "wlan_wake");
	wake_lock_init(&dhd->wl_rxwake, WAKE_LOCK_SUSPEND, "wlan_rx_wake");
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)) && 0
	mutex_init(&dhd->dhd_net_if_mutex);
#endif
	dhd_state |= DHD_ATTACH_STATE_WAKELOCKS_INIT;

	/* Attach and link in the protocol */
	if (dhd_prot_attach(&dhd->pub) != 0) {
		DHD_ERROR(("dhd_prot_attach failed\n"));
		goto fail;
	}
	dhd_state |= DHD_ATTACH_STATE_PROT_ATTACH;

#ifdef WL_CFG80211
	/* Attach and link in the cfg80211 */
	if (unlikely(wl_cfg80211_attach(net, &dhd->pub))) {
		DHD_ERROR(("wl_cfg80211_attach failed\n"));
		goto fail;
	}

	dhd_monitor_init(&dhd->pub);
	dhd_state |= DHD_ATTACH_STATE_CFG80211;
#endif
#if defined(CONFIG_WIRELESS_EXT)
	/* Attach and link in the iw */
	if (!(dhd_state &  DHD_ATTACH_STATE_CFG80211)) {
		if (wl_iw_attach(net, (void *)&dhd->pub) != 0) {
		DHD_ERROR(("wl_iw_attach failed\n"));
		goto fail;
	}
	dhd_state |= DHD_ATTACH_STATE_WL_ATTACH;
	}
#endif /* defined(CONFIG_WIRELESS_EXT) */


#ifndef BCMDBUS
	/* Set up the watchdog timer */
	init_timer(&dhd->timer);
	dhd->timer.data = (ulong)dhd;
	dhd->timer.function = dhd_watchdog;

	/* Set up the bottom half handler */
	tasklet_init(&dhd->tasklet, dhd_dpc, (ulong)dhd);
	dhd->dhd_tasklet_create = TRUE;
#endif /* BCMDBUS */

	if (dhd_sysioc) {
		PROC_START(_dhd_sysioc_thread, dhd, &dhd->thr_sysioc_ctl, 0);
	} else {
		dhd->thr_sysioc_ctl.thr_pid = -1;
	}
	dhd_state |= DHD_ATTACH_STATE_THREADS_CREATED;

	/*
	 * Save the dhd_info into the priv
	 */
	memcpy(netdev_priv(net), &dhd, sizeof(dhd));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && defined(CONFIG_PM_SLEEP)
    //register_pm_notifier(&dhd_sleep_pm_notifier);
#endif /*  (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && defined(CONFIG_PM_SLEEP) */

#ifdef CONFIG_HAS_EARLYSUSPEND
	dhd->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 20;
	dhd->early_suspend.suspend = dhd_early_suspend;
	dhd->early_suspend.resume = dhd_late_resume;
	register_early_suspend(&dhd->early_suspend);
	dhd_state |= DHD_ATTACH_STATE_EARLYSUSPEND_DONE;
#endif

#ifdef ARP_OFFLOAD_SUPPORT
	dhd->pend_ipaddr = 0;
	register_inetaddr_notifier(&dhd_notifier);
#endif /* ARP_OFFLOAD_SUPPORT */
    g_wifi_dev = net;
    DHD_TRACE(("%s: 1100:net name  %s\n", __FUNCTION__, g_wifi_dev->name));
    DHD_TRACE(("%s: 1200:dhd addr %p, dbus state : %d\n", __FUNCTION__, dhd, dhd->pub.busstate));

    v7_dhd_ranger.dhd = dhd;

	dhd_state |= DHD_ATTACH_STATE_DONE;
	dhd->dhd_state = dhd_state;
	return &dhd->pub;

fail:
	if (dhd_state < DHD_ATTACH_STATE_DHD_ALLOC) {
		if (net) free_netdev(net);
	} else {
		DHD_TRACE(("%s: Calling dhd_detach dhd_state 0x%x &dhd->pub %p\n",
			__FUNCTION__, dhd_state, &dhd->pub));
		dhd->dhd_state = dhd_state;
		dhd_detach(&dhd->pub);
		dhd_free(&dhd->pub);
	}

	return NULL;
}

#ifndef BCMDBUS
int
dhd_bus_start(dhd_pub_t *dhdp)
{
	int ret = -1;
	dhd_info_t *dhd = (dhd_info_t*)dhdp->info;
	unsigned long flags;

	ASSERT(dhd);

	DHD_TRACE(("Enter %s:\n", __FUNCTION__));



	/* Start the watchdog timer */
	dhd->pub.tickcnt = 0;
	dhd_os_wd_timer(&dhd->pub, dhd_watchdog_ms);

	/* Bring up the bus */
	if ((ret = dhd_bus_init(&dhd->pub, FALSE)) != 0) {

		DHD_ERROR(("%s, dhd_bus_init failed %d\n", __FUNCTION__, ret));
		return ret;
	}
#if defined(OOB_INTR_ONLY)
	/* Host registration for OOB interrupt */
	if (bcmsdh_register_oob_intr(dhdp)) {
		/* deactivate timer and wait for the handler to finish */

		flags = dhd_os_spin_lock(&dhd->pub);
		dhd->wd_timer_valid = FALSE;
		dhd_os_spin_unlock(&dhd->pub, flags);
		del_timer_sync(&dhd->timer);

		DHD_ERROR(("%s Host failed to register for OOB\n", __FUNCTION__));
		return -ENODEV;
	}

	/* Enable oob at firmware */
	dhd_enable_oob_intr(dhd->pub.bus, TRUE);
#endif /* defined(OOB_INTR_ONLY) */

	/* If bus is not ready, can't come up */
	if (dhd->pub.busstate != DHD_BUS_DATA) {
		flags = dhd_os_spin_lock(&dhd->pub);
		dhd->wd_timer_valid = FALSE;
		dhd_os_spin_unlock(&dhd->pub, flags);
		del_timer_sync(&dhd->timer);
		DHD_ERROR(("%s failed bus is not ready\n", __FUNCTION__));
		return -ENODEV;
	}


#ifdef BCMSDIOH_TXGLOM
	if ((dhd->pub.busstate == DHD_BUS_DATA) && bcmsdh_glom_enabled()) {
		dhd_txglom_enable(dhdp, TRUE);
	}
#endif

#ifdef READ_MACADDR
	dhd_read_macaddr(dhd);
#endif

	/* Bus is ready, do any protocol initialization */
	if ((ret = dhd_prot_init(&dhd->pub)) < 0)
		return ret;

#ifdef WRITE_MACADDR
	dhd_write_macaddr(dhd->pub.mac.octet);
#endif

#ifdef ARP_OFFLOAD_SUPPORT
	if (dhd->pend_ipaddr) {
#ifdef AOE_IP_ALIAS_SUPPORT
		aoe_update_host_ipv4_table(&dhd->pub, dhd->pend_ipaddr, TRUE);
#endif /* AOE_IP_ALIAS_SUPPORT */
		dhd->pend_ipaddr = 0;
	}
#endif /* ARP_OFFLOAD_SUPPORT */

	return 0;
}
#endif /* BCMDBUS */

int
dhd_preinit_ioctls(dhd_pub_t *dhd)
{
	int ret = 0;
	char eventmask[WL_EVENTING_MASK_LEN];
	char iovbuf[WL_EVENTING_MASK_LEN + 12];	/*  Room for "event_msgs" + '\0' + bitvec  */


	/* Read event_msgs mask */
	bcm_mkiovar("event_msgs", eventmask, WL_EVENTING_MASK_LEN, iovbuf, sizeof(iovbuf));
	if ((ret  = dhd_wl_ioctl_cmd(dhd, WLC_GET_VAR, iovbuf, sizeof(iovbuf), FALSE, 0)) < 0) {
		DHD_ERROR(("%s read Event mask failed %d\n", __FUNCTION__, ret));
		goto done;
	}
	bcopy(iovbuf, eventmask, WL_EVENTING_MASK_LEN);

	/* Setup event_msgs */
	setbit(eventmask, WLC_E_SET_SSID);
	setbit(eventmask, WLC_E_PRUNE);
	setbit(eventmask, WLC_E_AUTH);
	setbit(eventmask, WLC_E_REASSOC);
	setbit(eventmask, WLC_E_REASSOC_IND);
	setbit(eventmask, WLC_E_DEAUTH);
	setbit(eventmask, WLC_E_DEAUTH_IND);
	setbit(eventmask, WLC_E_DISASSOC_IND);
	setbit(eventmask, WLC_E_DISASSOC);
	setbit(eventmask, WLC_E_JOIN);
	setbit(eventmask, WLC_E_ASSOC_IND);
	setbit(eventmask, WLC_E_PSK_SUP);
	setbit(eventmask, WLC_E_LINK);
	setbit(eventmask, WLC_E_NDIS_LINK);
	setbit(eventmask, WLC_E_MIC_ERROR);
	setbit(eventmask, WLC_E_ASSOC_REQ_IE);
	setbit(eventmask, WLC_E_ASSOC_RESP_IE);
	setbit(eventmask, WLC_E_PMKID_CACHE);
//	setbit(eventmask, WLC_E_TXFAIL);
	setbit(eventmask, WLC_E_JOIN_START);
#ifdef DHD_MSGTRACE_OFF
    clrbit(eventmask, WLC_E_TRACE);
#else    
    setbit(eventmask, WLC_E_TRACE);
#endif
	setbit(eventmask, WLC_E_SCAN_COMPLETE);
#ifdef WLMEDIA_HTSF
	setbit(eventmask, WLC_E_HTSFSYNC);
#endif /* WLMEDIA_HTSF */
#ifdef PNO_SUPPORT
	setbit(eventmask, WLC_E_PFN_NET_FOUND);
#endif /* PNO_SUPPORT */
	/* enable dongle roaming event */
#ifdef WL_CFG80211
	setbit(eventmask, WLC_E_ESCAN_RESULT);
	if ((dhd->op_mode & WFD_MASK) == WFD_MASK) {
		setbit(eventmask, WLC_E_ACTION_FRAME_RX);
		setbit(eventmask, WLC_E_ACTION_FRAME_COMPLETE);
		setbit(eventmask, WLC_E_ACTION_FRAME_OFF_CHAN_COMPLETE);
		setbit(eventmask, WLC_E_P2P_PROBREQ_MSG);
		setbit(eventmask, WLC_E_P2P_DISC_LISTEN_COMPLETE);
	}
#endif /* WL_CFG80211 */

	/* Write updated Event mask */
	bcm_mkiovar("event_msgs", eventmask, WL_EVENTING_MASK_LEN, iovbuf, sizeof(iovbuf));
	if ((ret = dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), TRUE, 0)) < 0) {
		DHD_ERROR(("%s Set Event mask failed %d\n", __FUNCTION__, ret));
		goto done;
	}


done:
	return ret;
}


int
dhd_iovar(dhd_pub_t *pub, int ifidx, char *name, char *cmd_buf, uint cmd_len, int set)
{
	char buf[strlen(name) + 1 + cmd_len];
	int len = sizeof(buf);
	wl_ioctl_t ioc;
	int ret;

	len = bcm_mkiovar(name, cmd_buf, cmd_len, buf, len);

	memset(&ioc, 0, sizeof(ioc));

	ioc.cmd = set? WLC_SET_VAR : WLC_GET_VAR;
	ioc.buf = buf;
	ioc.len = len;
	ioc.set = TRUE;

	ret = dhd_wl_ioctl(pub, ifidx, &ioc, ioc.buf, ioc.len);
	if (!set && ret >= 0)
		memcpy(cmd_buf, buf, cmd_len);

	return ret;
}

int dhd_change_mtu(dhd_pub_t *dhdp, int new_mtu, int ifidx)
{
	struct dhd_info *dhd = dhdp->info;
	struct net_device *dev = NULL;

	ASSERT(dhd && dhd->iflist[ifidx]);
	dev = dhd->iflist[ifidx]->net;
	ASSERT(dev);

	if (netif_running(dev)) {
		DHD_ERROR(("%s: Must be down to change its MTU", dev->name));
		return BCME_NOTDOWN;
	}

#define DHD_MIN_MTU 1500
#define DHD_MAX_MTU 1752

	if ((new_mtu < DHD_MIN_MTU) || (new_mtu > DHD_MAX_MTU)) {
		DHD_ERROR(("%s: MTU size %d is invalid.\n", __FUNCTION__, new_mtu));
		return BCME_BADARG;
	}

	dev->mtu = new_mtu;
	return 0;
}

#ifdef ARP_OFFLOAD_SUPPORT
/* add or remove AOE host ip(s) (up to 8 IPs on the interface)  */
void
aoe_update_host_ipv4_table(dhd_pub_t *dhd_pub, u32 ipa, bool add)
{
	u32 ipv4_buf[MAX_IPV4_ENTRIES]; /* temp save for AOE host_ip table */
	int i;
	int ret;

	bzero(ipv4_buf, sizeof(ipv4_buf));

	/* display what we've got */
	ret = dhd_arp_get_arp_hostip_table(dhd_pub, ipv4_buf, sizeof(ipv4_buf));
	DHD_ARPOE(("%s: hostip table read from Dongle:\n", __FUNCTION__));
#ifdef AOE_DBG
	dhd_print_buf(ipv4_buf, 32, 4); /* max 8 IPs 4b each */
#endif
	/* now we saved hoste_ip table, clr it in the dongle AOE */
	dhd_aoe_hostip_clr(dhd_pub);

	if (ret) {
		DHD_ERROR(("%s failed\n", __FUNCTION__));
		return;
	}

	for (i = 0; i < MAX_IPV4_ENTRIES; i++) {
		if (add && (ipv4_buf[i] == 0)) {
				ipv4_buf[i] = ipa;
				add = FALSE; /* added ipa to local table  */
				DHD_ARPOE(("%s: Saved new IP in temp arp_hostip[%d]\n",
				__FUNCTION__, i));
		} else if (ipv4_buf[i] == ipa) {
			ipv4_buf[i]	= 0;
			DHD_ARPOE(("%s: removed IP:%x from temp table %d\n",
				__FUNCTION__, ipa, i));
		}

		if (ipv4_buf[i] != 0) {
			/* add back host_ip entries from our local cache */
			dhd_arp_offload_add_ip(dhd_pub, ipv4_buf[i]);
			DHD_ARPOE(("%s: added IP:%x to dongle arp_hostip[%d]\n\n",
				__FUNCTION__, ipv4_buf[i], i));
		}
	}
#ifdef AOE_DBG
	/* see the resulting hostip table */
	dhd_arp_get_arp_hostip_table(dhd_pub, ipv4_buf, sizeof(ipv4_buf));
	DHD_ARPOE(("%s: read back arp_hostip table:\n", __FUNCTION__));
	dhd_print_buf(ipv4_buf, 32, 4); /* max 8 IPs 4b each */
#endif
}

static int dhd_device_event(struct notifier_block *this,
	unsigned long event,
	void *ptr)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;

	dhd_info_t *dhd;
	dhd_pub_t *dhd_pub;

	if (!ifa)
		return NOTIFY_DONE;

	dhd = *(dhd_info_t **)netdev_priv(ifa->ifa_dev->dev);
	dhd_pub = &dhd->pub;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31))
	if (ifa->ifa_dev->dev->netdev_ops == &dhd_ops_pri) {
#else
	if (ifa->ifa_dev->dev) {
#endif
		switch (event) {
		case NETDEV_UP:
			DHD_ARPOE(("%s: [%s] Up IP: 0x%x\n",
				__FUNCTION__, ifa->ifa_label, ifa->ifa_address));

			if (dhd->pub.busstate != DHD_BUS_DATA) {
				DHD_ERROR(("%s: bus not ready, exit\n", __FUNCTION__));
				if (dhd->pend_ipaddr) {
					DHD_ERROR(("%s: overwrite pending ipaddr: 0x%x\n",
						__FUNCTION__, dhd->pend_ipaddr));
				}
				dhd->pend_ipaddr = ifa->ifa_address;
				break;
			}

#ifdef AOE_IP_ALIAS_SUPPORT
			if (ifa->ifa_label[strlen(ifa->ifa_label)-2] == 0x3a) {
				DHD_ARPOE(("%s:add aliased IP to AOE hostip cache\n",
					__FUNCTION__));
				aoe_update_host_ipv4_table(dhd_pub, ifa->ifa_address, TRUE);
			}
			else
				aoe_update_host_ipv4_table(dhd_pub, ifa->ifa_address, TRUE);
#endif
			break;

		case NETDEV_DOWN:
			DHD_ARPOE(("%s: [%s] Down IP: 0x%x\n",
				__FUNCTION__, ifa->ifa_label, ifa->ifa_address));
			dhd->pend_ipaddr = 0;
#ifdef AOE_IP_ALIAS_SUPPORT
		if (!(ifa->ifa_label[strlen(ifa->ifa_label)-2] == 0x3a)) {
				DHD_ARPOE(("%s: primary interface is down, AOE clr all\n",
				           __FUNCTION__));
				dhd_aoe_hostip_clr(&dhd->pub);
				dhd_aoe_arp_clr(&dhd->pub);
		} else
			aoe_update_host_ipv4_table(dhd_pub, ifa->ifa_address, FALSE);
#else
			dhd_aoe_hostip_clr(&dhd->pub);
			dhd_aoe_arp_clr(&dhd->pub);
#endif
			break;

		default:
			DHD_ARPOE(("%s: do noting for [%s] Event: %lu\n",
				__func__, ifa->ifa_label, event));
			break;
		}
	}
	return NOTIFY_DONE;
}
#endif /* ARP_OFFLOAD_SUPPORT */

int
dhd_net_attach(dhd_pub_t *dhdp, int ifidx)
{
	dhd_info_t *dhd = (dhd_info_t *)dhdp->info;
	struct net_device *net = NULL;
	int err = 0;
	uint8 temp_addr[ETHER_ADDR_LEN] = { 0x00, 0x90, 0x4c, 0x11, 0x22, 0x33 };

	DHD_TRACE(("%s: ifidx %d\n", __FUNCTION__, ifidx));

	ASSERT(dhd && dhd->iflist[ifidx]);

	net = dhd->iflist[ifidx]->net;
	ASSERT(net);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31))
	ASSERT(!net->open);
	net->get_stats = dhd_get_stats;
	net->do_ioctl = dhd_ioctl_entry;
	net->hard_start_xmit = dhd_start_xmit;
	net->set_mac_address = dhd_set_mac_address;
	net->set_multicast_list = dhd_set_multicast_list;
	net->open = net->stop = NULL;
#else
	ASSERT(!net->netdev_ops);
	net->netdev_ops = &dhd_ops_virt;
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31) */

	/* Ok, link into the network layer... */
	if (ifidx == 0) {
		/*
		 * device functions for the primary interface only
		 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31))
		net->open = dhd_open;
		net->stop = dhd_stop;
#else
		net->netdev_ops = &dhd_ops_pri;
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31) */
	} else {
		/*
		 * We have to use the primary MAC for virtual interfaces
		 */
		memcpy(temp_addr, dhd->iflist[ifidx]->mac_addr, ETHER_ADDR_LEN);
		if (!memcmp(temp_addr, dhdp->mac.octet, ETHER_ADDR_LEN)) {
			DHD_TRACE(("%s interface [%s]: set locally administered bit in MAC\n",
			__func__, net->name));
            temp_addr[0] |= 0x02;
            temp_addr[5] += 0x01;
        }
    }

	net->hard_header_len = ETH_HLEN + dhd->pub.hdrlen;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
	net->ethtool_ops = &dhd_ethtool_ops;
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24) */

#if defined(CONFIG_WIRELESS_EXT)
#if WIRELESS_EXT < 19
	net->get_wireless_stats = dhd_get_wireless_stats;
#endif /* WIRELESS_EXT < 19 */
#if WIRELESS_EXT > 12
	net->wireless_handlers = (struct iw_handler_def *)&wl_iw_handler_def;
#endif /* WIRELESS_EXT > 12 */
#endif /* defined(CONFIG_WIRELESS_EXT) */

	dhd->pub.rxsz = DBUS_RX_BUFFER_SIZE_DHD(net);

	memcpy(net->dev_addr, temp_addr, ETHER_ADDR_LEN);
#ifdef HNDCTF
    int msglevel = 1;
    if(!cih_dhd) 
    {
        printk("wan broadcom ctf initdddddd,%d\n",ifidx);
        fake_osh_wifi = 0xdeadbeaf; /* watch here, it might die */
        printk("wan broadcom ctf devname dddddddd%s\n",net->name);
        cih_dhd = ctf_attach(fake_osh_wifi, net->name, &msglevel, et_ctf_detach_dhd, NULL /* et*/ );
        if (cih_dhd)
        {
            printk("\n bcmgenet_desc_rx: ctf attach ok !!!!!! cih = 0x%08x \n", cih_dhd );
        }
    }
#endif /* HNDCTF */
	if ((err = register_netdev(net)) != 0) 
    {
		DHD_ERROR(("couldn't register the net device, err %d\n", err));
		goto fail;
	}
#ifdef HNDCTF
    if (ctf_dev_register(cih_dhd, net, FALSE) != BCME_OK)
    {
        printk("ctf_dev_register() failed\n");
    }
    if (ctf_enable(cih_dhd, net, TRUE,NULL) != BCME_OK)
    {
        printk("ctf_dev_register() failed\n");
    }
    else
    {
        printk("ctf register ok\n");
    }
#endif /* HNDCTF */
	printf("%s: Broadcom Dongle Host Driver\n", net->name);

#if 0 && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	if (ifidx == 0) {
		dhd_registration_check = TRUE;
		up(&dhd_registration_sem);
	}
#endif 
	return 0;

fail:
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31)
	net->open = NULL;
#else
	net->netdev_ops = NULL;
#endif
	return err;
}

void
dhd_bus_detach(dhd_pub_t *dhdp)
{
	dhd_info_t *dhd;

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	if (dhdp) {
		dhd = (dhd_info_t *)dhdp->info;
		if (dhd) {

			/*
			 * In case of Android cfg80211 driver, the bus is down in dhd_stop,
			 *  calling stop again will cuase SD read/write errors.
			 */
			if (dhd->pub.busstate != DHD_BUS_DOWN) {
				/* Stop the protocol module */
				dhd_prot_stop(&dhd->pub);

				/* Stop the bus module */
#ifdef BCMDBUS
				/* Force Dongle terminated */
				if (dhd_wl_ioctl_cmd(dhdp, WLC_TERMINATED, NULL, 0, TRUE, 0) < 0)
					DHD_ERROR(("%s Setting WLC_TERMINATED failed\n",
						__FUNCTION__));
				dbus_stop(dhd->pub.dbus);
				dhd->pub.busstate = DHD_BUS_DOWN;
#else
				dhd_bus_stop(dhd->pub.bus, TRUE);
#endif /* BCMDBUS */
			}

#if defined(OOB_INTR_ONLY)
			bcmsdh_unregister_oob_intr();
#endif /* defined(OOB_INTR_ONLY) */
		}
	}
}


void dhd_detach(dhd_pub_t *dhdp)
{
	dhd_info_t *dhd;
	unsigned long flags;
	int timer_valid = FALSE;

	if (!dhdp)
		return;

	dhd = (dhd_info_t *)dhdp->info;
	if (!dhd)
		return;

	DHD_TRACE(("%s: Enter state 0x%x\n", __FUNCTION__, dhd->dhd_state));

	if (!(dhd->dhd_state & DHD_ATTACH_STATE_DONE)) {
		/* Give sufficient time for threads to start running in case
		 * dhd_attach() has failed
		 */
		osl_delay(1000*100);
	}

#ifdef ARP_OFFLOAD_SUPPORT
	unregister_inetaddr_notifier(&dhd_notifier);
#endif /* ARP_OFFLOAD_SUPPORT */

#if defined(CONFIG_HAS_EARLYSUSPEND)
	if (dhd->dhd_state & DHD_ATTACH_STATE_EARLYSUSPEND_DONE) {
		if (dhd->early_suspend.suspend)
			unregister_early_suspend(&dhd->early_suspend);
	}
#endif /* defined(CONFIG_HAS_EARLYSUSPEND) */


#if defined(CONFIG_WIRELESS_EXT)
	if (dhd->dhd_state & DHD_ATTACH_STATE_WL_ATTACH) {
		/* Detatch and unlink in the iw */
		wl_iw_detach();
	}
#endif /* defined(CONFIG_WIRELESS_EXT) */

	if (dhd->thr_sysioc_ctl.thr_pid >= 0) {
		PROC_STOP(&dhd->thr_sysioc_ctl);
	}

	/* delete all interfaces, start with virtual  */
	if (dhd->dhd_state & DHD_ATTACH_STATE_ADD_IF) {
		int i = 1;
		dhd_if_t *ifp;

		/* Cleanup virtual interfaces */
		for (i = 1; i < DHD_MAX_IFS; i++)
			if (dhd->iflist[i]) {
				dhd->iflist[i]->state = DHD_IF_DEL;
				dhd->iflist[i]->idx = i;
				dhd_op_if(dhd->iflist[i]);
			}

		/*  delete primary interface 0 */
		ifp = dhd->iflist[0];
		ASSERT(ifp);
		ASSERT(ifp->net);
		if (ifp && ifp->net) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 31))
			if (ifp->net->open)
#else
			if (ifp->net->netdev_ops == &dhd_ops_pri)
#endif
			{
				unregister_netdev(ifp->net);
#ifdef HNDCTF
                ctf_dev_unregister(cih_dhd, ifp->net);
                cih_dhd = NULL;
#endif

				free_netdev(ifp->net);
				ifp->net = NULL;
				MFREE(dhd->pub.osh, ifp, sizeof(*ifp));
				dhd->iflist[0] = NULL;
			}
		}
	}

	/* Clear the watchdog timer */
	flags = dhd_os_spin_lock(&dhd->pub);
	timer_valid = dhd->wd_timer_valid;
	dhd->wd_timer_valid = FALSE;
	dhd_os_spin_unlock(&dhd->pub, flags);
	if (timer_valid)
		del_timer_sync(&dhd->timer);

#ifndef BCMDBUS
	if (dhd->dhd_state & DHD_ATTACH_STATE_THREADS_CREATED) {
#endif /* BCMDBUS */
		tasklet_kill(&dhd->tasklet);
#ifndef BCMDBUS
	}
#endif
	if (dhd->dhd_state & DHD_ATTACH_STATE_PROT_ATTACH) {
		dhd_bus_detach(dhdp);

		if (dhdp->prot)
			dhd_prot_detach(dhdp);
	}

#ifdef WL_CFG80211
	if (dhd->dhd_state & DHD_ATTACH_STATE_CFG80211) {
		wl_cfg80211_detach(NULL);
		dhd_monitor_uninit();
	}
#endif

#ifdef BCMDBUS
	if (dhdp->dbus) {
		dbus_detach(dhdp->dbus);
		dhdp->dbus = NULL;
	}
#endif /* BCMDBUS */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && defined(CONFIG_PM_SLEEP)
    //unregister_pm_notifier(&dhd_sleep_pm_notifier);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) && defined(CONFIG_PM_SLEEP) */
	/* && defined(CONFIG_PM_SLEEP) */

	if (dhd->dhd_state & DHD_ATTACH_STATE_WAKELOCKS_INIT) {
#ifdef CONFIG_HAS_WAKELOCK
		dhd->wakelock_counter = 0;
		dhd->wakelock_timeout_enable = 0;
		wake_lock_destroy(&dhd->wl_wifi);
		wake_lock_destroy(&dhd->wl_rxwake);
#endif
	}
}


void
dhd_free(dhd_pub_t *dhdp)
{
	dhd_info_t *dhd;
	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	if (dhdp) {
		int i;
		for (i = 0; i < ARRAYSIZE(dhdp->reorder_bufs); i++) {
			if (dhdp->reorder_bufs[i]) {
				reorder_info_t *ptr;
				uint32 buf_size = sizeof(struct reorder_info);

				ptr = dhdp->reorder_bufs[i];

				buf_size += ((ptr->max_idx + 1) * sizeof(void*));
				DHD_REORDER(("free flow id buf %d, maxidx is %d, buf_size %d\n",
					i, ptr->max_idx, buf_size));

				MFREE(dhdp->osh, dhdp->reorder_bufs[i], buf_size);
				dhdp->reorder_bufs[i] = NULL;
			}
		}
		dhd = (dhd_info_t *)dhdp->info;
		if (dhd)
			MFREE(dhd->pub.osh, dhd, sizeof(*dhd));
	}
}

static void __exit
dhd_module_cleanup(void)
{
	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

    set_get_sta_num_func((unsigned int)NULL);

#ifdef BCMDBUS
	dbus_deregister();
#else
	dhd_bus_unregister();
#endif /* BCMDBUS */


	/* Call customer gpio to turn off power with WL_REG_ON signal */
	dhd_customer_gpio_wlan_ctrl(WLAN_POWER_OFF);
}

#ifdef BCMDBUS

/*
 * hdrlen is space to reserve in pkt headroom for DBUS
 */
static void *
dhd_dbus_probe_cb(void *arg, const char *desc, uint32 bustype, uint32 hdrlen)
{
	osl_t *osh;
	int ret = 0;
	dbus_attrib_t attrib;
	dhd_pub_t *pub = NULL;

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	/* Ask the OS interface part for an OSL handle */
	if (!(osh = dhd_osl_attach(NULL, bustype))) {
		DHD_ERROR(("%s: OSL attach failed\n", __FUNCTION__));
		ret = -ENOMEM;
		goto fail;
	}

	/* Attach to the dhd/OS interface */
	if (!(pub = dhd_attach(osh, NULL /* bus */, hdrlen))) {
		DHD_ERROR(("%s: dhd_attach failed\n", __FUNCTION__));
		ret = -ENXIO;
		goto fail;
	}

	/* Ok, finish the attach to the OS network interface */
	if (dhd_net_attach(pub, 0) != 0) {
		DHD_ERROR(("%s: dhd_net_attach failed\n", __FUNCTION__));
		ret = -ENXIO;
		goto fail;
	}

	pub->dbus = dbus_attach(osh, pub->rxsz, DBUS_NRXQ, DBUS_NTXQ,
		pub->info, &dhd_dbus_cbs, NULL, NULL);
	if (pub->dbus) {
		dbus_get_attrib(pub->dbus, &attrib);
		DHD_ERROR(("DBUS: vid=0x%x pid=0x%x devid=0x%x bustype=0x%x mtu=%d\n",
			attrib.vid, attrib.pid, attrib.devid, attrib.bustype, attrib.mtu));
	} else {
		ret = -ENXIO;
		goto fail;
	}
#ifdef BCM_FD_AGGR
	pub->info->rpc_th = bcm_rpc_tp_attach(osh, (void *)pub->dbus);
	if (!pub->info->rpc_th) {
		DHD_ERROR(("%s: bcm_rpc_tp_attach failed\n", __FUNCTION__));
		ret = -ENXIO;
		goto fail;
	}

	pub->info->rpc_osh = rpc_osl_attach(osh);
	if (!pub->info->rpc_osh) {
		DHD_ERROR(("%s: rpc_osl_attach failed\n", __FUNCTION__));
		bcm_rpc_tp_detach(pub->info->rpc_th);
		pub->info->rpc_th = NULL;
		ret = -ENXIO;
		goto fail;
	}
	/* Set up the aggregation release timer */
	init_timer(&pub->info->rpcth_timer);
	pub->info->rpcth_timer.data = (ulong)pub->info;
	pub->info->rpcth_timer.function = dhd_rpcth_watchdog;
	pub->info->rpcth_timer_active = FALSE;

	bcm_rpc_tp_register_cb(pub->info->rpc_th, NULL, pub->info,
		dbus_rpcth_rx_pkt, pub->info, pub->info->rpc_osh);
#endif /* BCM_FD_AGGR */
#ifdef BCMDBGFS
	dhd_dbg_remove();
#endif

	/* This is passed to dhd_dbus_disconnect_cb */
	return pub->info;
fail:
	/* Release resources in reverse order */
	if (osh) {
		if (pub) {
			dhd_detach(pub);
			dhd_free(pub);
		}
		dhd_osl_detach(osh);
	}

	BCM_REFERENCE(ret);
	return NULL;
}
static void
dhd_dbus_disconnect_cb(void *arg)
{
	dhd_info_t *dhd = (dhd_info_t *)arg;
	dhd_pub_t *pub;
	osl_t *osh;

	if (dhd == NULL)
		return;

	pub = &dhd->pub;
	osh = pub->osh;
#ifdef BCM_FD_AGGR
	del_timer_sync(&dhd->rpcth_timer);
	bcm_rpc_tp_deregister_cb(dhd->rpc_th);
	rpc_osl_detach(dhd->rpc_osh);
	bcm_rpc_tp_detach(dhd->rpc_th);
#endif
	dhd_detach(pub);
	dhd_free(pub);

	if (MALLOCED(osh)) {
		DHD_ERROR(("%s: MEMORY LEAK %d bytes\n", __FUNCTION__, MALLOCED(osh)));
	}
	dhd_osl_detach(osh);
}
#endif /* BCMDBUS */

static int __init
dhd_module_init(void)
{
	int error = 0;

#if 0 && defined(BCMLXSDMMC) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	int retry = POWERUP_MAX_RETRY;
	int chip_up = 0;
#endif 

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));



#if 0 && defined(BCMLXSDMMC) && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	do {
		sema_init(&dhd_chipup_sem, 0);
		dhd_bus_reg_sdio_notify(&dhd_chipup_sem);
		dhd_customer_gpio_wlan_ctrl(WLAN_POWER_ON);
#if defined(CONFIG_WIFI_CONTROL_FUNC)
		if (wl_android_wifictrl_func_add() < 0)
			goto fail_1;
#endif /* defined(CONFIG_WIFI_CONTROL_FUNC) */
		if (down_timeout(&dhd_chipup_sem,
			msecs_to_jiffies(POWERUP_WAIT_MS)) == 0) {
			dhd_bus_unreg_sdio_notify();
			chip_up = 1;
			break;
		}
		DHD_ERROR(("\nfailed to power up wifi chip, retry again (%d left) **\n\n",
			retry+1));
		dhd_bus_unreg_sdio_notify();
#if defined(CONFIG_WIFI_CONTROL_FUNC)
		wl_android_wifictrl_func_del();
#endif /* defined(CONFIG_WIFI_CONTROL_FUNC) */
		dhd_customer_gpio_wlan_ctrl(WLAN_POWER_OFF);
	} while (retry-- > 0);

	if (!chip_up) {
		DHD_ERROR(("\nfailed to power up wifi chip, max retry reached, exits **\n\n"));
		return -ENODEV;
	}
#else
	dhd_customer_gpio_wlan_ctrl(WLAN_POWER_ON);
#if defined(CONFIG_WIFI_CONTROL_FUNC)
	if (wl_android_wifictrl_func_add() < 0)
		goto fail_1;
#endif /* defined(CONFIG_WIFI_CONTROL_FUNC) */

#endif 

#if 0 && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	sema_init(&dhd_registration_sem, 0);
#endif 


#ifdef BCMDBUS
	error = dbus_register(0xa5c, 0x48f, dhd_dbus_probe_cb,
		dhd_dbus_disconnect_cb, NULL /* arg */, NULL, NULL);
	if (error == DBUS_ERR_NODEVICE) { /* Device not detected */
		error = DBUS_OK;
	}
#else
	error = dhd_bus_register();
#endif

	if (!error)
		printf("\n%s\n", dhd_version);
	else {
		DHD_ERROR(("%s: sdio_register_driver failed\n", __FUNCTION__));
		goto fail_1;
	}


    set_get_sta_num_func((unsigned int)dhd_ap_get_sta_num);

    v7_dhd_ranger_init();

    sg_dhd_last_jiffies =  jiffies;
	return error;

#if 0 && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
fail_2:
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27)) */

fail_1:


	/* Call customer gpio to turn off power with WL_REG_ON signal */
	dhd_customer_gpio_wlan_ctrl(WLAN_POWER_OFF);

	return error;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
late_initcall(dhd_module_init);
#else
module_init(dhd_module_init);
#endif

module_exit(dhd_module_cleanup);

/*
 * OS specific functions required to implement DHD driver in OS independent way
 */
int
dhd_os_proto_block(dhd_pub_t *pub)
{
	dhd_info_t * dhd = (dhd_info_t *)(pub->info);

	if (dhd) {
		down(&dhd->proto_sem);
		return 1;
	}

	return 0;
}

int
dhd_os_proto_unblock(dhd_pub_t *pub)
{
	dhd_info_t * dhd = (dhd_info_t *)(pub->info);

	if (dhd) {
		up(&dhd->proto_sem);
		return 1;
	}

	return 0;
}

unsigned int
dhd_os_get_ioctl_resp_timeout(void)
{
	return ((unsigned int)dhd_ioctl_timeout_msec);
}

void
dhd_os_set_ioctl_resp_timeout(unsigned int timeout_msec)
{
	dhd_ioctl_timeout_msec = (int)timeout_msec;
}

int
dhd_os_ioctl_resp_wait(dhd_pub_t *pub, uint *condition, bool *pending)
{
	dhd_info_t * dhd = (dhd_info_t *)(pub->info);
	DECLARE_WAITQUEUE(wait, current);
	int timeout = dhd_ioctl_timeout_msec;

	/* Convert timeout in millsecond to jiffies */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 27))
	timeout = msecs_to_jiffies(timeout);
#else
	timeout = timeout * HZ / 1000;
#endif

	/* Wait until control frame is available */
	add_wait_queue(&dhd->ioctl_resp_wait, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

	/* Memory barrier to support multi-processing
	 * As the variable "condition", which points to dhd->rxlen (dhd_bus_rxctl[dhd_sdio.c])
	 * Can be changed by another processor.
	 */
	smp_mb();
	while (!(*condition) && (!signal_pending(current) && timeout)) {
		timeout = schedule_timeout(timeout);
		smp_mb();
	}

	if (signal_pending(current))
		*pending = TRUE;

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&dhd->ioctl_resp_wait, &wait);

	return timeout;
}

int
dhd_os_ioctl_resp_wake(dhd_pub_t *pub)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);

	if (waitqueue_active(&dhd->ioctl_resp_wait)) {
		wake_up_interruptible(&dhd->ioctl_resp_wait);
	}

	return 0;
}

void
dhd_os_wd_timer(void *bus, uint wdtick)
{
#ifndef BCMDBUS
	dhd_pub_t *pub = bus;
	dhd_info_t *dhd = (dhd_info_t *)pub->info;
	unsigned long flags;

	DHD_TRACE(("%s: Enter\n", __FUNCTION__));

	flags = dhd_os_spin_lock(pub);

	/* don't start the wd until fw is loaded */
	if (pub->busstate == DHD_BUS_DOWN) {
		dhd_os_spin_unlock(pub, flags);
		return;
	}

	/* Totally stop the timer */
	if (!wdtick && dhd->wd_timer_valid == TRUE) {
		dhd->wd_timer_valid = FALSE;
		dhd_os_spin_unlock(pub, flags);
		del_timer(&dhd->timer);
		return;
	}

	if (wdtick) {
		dhd_watchdog_ms = (uint)wdtick;
		/* Re arm the timer, at last watchdog period */
		mod_timer(&dhd->timer, jiffies + dhd_watchdog_ms * HZ / 1000);
		dhd->wd_timer_valid = TRUE;
	}
	dhd_os_spin_unlock(pub, flags);
#endif /* BCMDBUS */
}

void *
dhd_os_open_image(char *filename)
{
	struct file *fp;

	fp = filp_open(filename, O_RDONLY, 0);
	/*
	 * 2.6.11 (FC4) supports filp_open() but later revs don't?
	 * Alternative:
	 * fp = open_namei(AT_FDCWD, filename, O_RD, 0);
	 * ???
	 */
	 if (IS_ERR(fp))
		 fp = NULL;

	 return fp;
}

int
dhd_os_get_image_block(char *buf, int len, void *image)
{
	struct file *fp = (struct file *)image;
	int rdlen;

	if (!image)
		return 0;

	rdlen = kernel_read(fp, fp->f_pos, buf, len);
	if (rdlen > 0)
		fp->f_pos += rdlen;

	return rdlen;
}

void
dhd_os_close_image(void *image)
{
	if (image)
		filp_close((struct file *)image, NULL);
}


void
dhd_os_sdlock(dhd_pub_t *pub)
{
	dhd_info_t *dhd;

	dhd = (dhd_info_t *)(pub->info);

#ifndef BCMDBUS
#endif /* BCMDBUS */
	spin_lock_bh(&dhd->sdlock);
}

void
dhd_os_sdunlock(dhd_pub_t *pub)
{
	dhd_info_t *dhd;

	dhd = (dhd_info_t *)(pub->info);

#ifndef BCMDBUS
#endif /* BCMDBUS */
	spin_unlock_bh(&dhd->sdlock);
}

void
dhd_os_sdlock_txq(dhd_pub_t *pub)
{
	dhd_info_t *dhd;

	dhd = (dhd_info_t *)(pub->info);
#ifdef BCMDBUS
	spin_lock_irqsave(&dhd->txqlock, dhd->txqlock_flags);
#else
	spin_lock_bh(&dhd->txqlock);
#endif
}

void
dhd_os_sdunlock_txq(dhd_pub_t *pub)
{
	dhd_info_t *dhd;

	dhd = (dhd_info_t *)(pub->info);
#ifdef BCMDBUS
	spin_unlock_irqrestore(&dhd->txqlock, dhd->txqlock_flags);
#else
	spin_unlock_bh(&dhd->txqlock);
#endif
}

void
dhd_os_sdlock_rxq(dhd_pub_t *pub)
{
}

void
dhd_os_sdunlock_rxq(dhd_pub_t *pub)
{
}

void
dhd_os_sdtxlock(dhd_pub_t *pub)
{
	dhd_os_sdlock(pub);
}

void
dhd_os_sdtxunlock(dhd_pub_t *pub)
{
	dhd_os_sdunlock(pub);
}

#if defined(CONFIG_DHD_USE_STATIC_BUF)
uint8* dhd_os_prealloc(void *osh, int section, uint size)
{
	return (uint8*)wl_android_prealloc(section, size);
}

void dhd_os_prefree(void *osh, void *addr, uint size)
{
}
#endif /* defined(CONFIG_WIFI_CONTROL_FUNC) */

#if defined(CONFIG_WIRELESS_EXT)
struct iw_statistics *
dhd_get_wireless_stats(struct net_device *dev)
{
	int res = 0;
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);

	if (!dhd->pub.up) {
		return NULL;
	}

	res = wl_iw_get_wireless_stats(dev, &dhd->iw.wstats);

	if (res == 0)
		return &dhd->iw.wstats;
	else
		return NULL;
}
#endif /* defined(CONFIG_WIRELESS_EXT) */

static int
dhd_wl_host_event(dhd_info_t *dhd, int *ifidx, void *pktdata,
	wl_event_msg_t *event, void **data)
{
	int bcmerror = 0;
	ASSERT(dhd != NULL);

	bcmerror = wl_host_event(&dhd->pub, ifidx, pktdata, event, data);
	if (bcmerror != BCME_OK)
		return (bcmerror);

#if defined(CONFIG_WIRELESS_EXT)
	if (event->bsscfgidx == 0) {
		/*
		 * Wireless ext is on primary interface only
		 */

	ASSERT(dhd->iflist[*ifidx] != NULL);
	ASSERT(dhd->iflist[*ifidx]->net != NULL);

		if (dhd->iflist[*ifidx]->net) {
		wl_iw_event(dhd->iflist[*ifidx]->net, event, *data);
		}
	}
#endif /* defined(CONFIG_WIRELESS_EXT)  */

#ifdef WL_CFG80211
	if ((ntoh32(event->event_type) == WLC_E_IF) &&
		(((dhd_if_event_t *)*data)->action == WLC_E_IF_ADD))
		/* If ADD_IF has been called directly by wl utility then we
		 * should not report this. In case if ADD_IF was called from
		 * CFG stack, then too this event need not be reported back
		 */
		return (BCME_OK);
	if ((wl_cfg80211_is_progress_ifchange() ||
		wl_cfg80211_is_progress_ifadd()) && (*ifidx != 0)) {
		/*
		 * If IF_ADD/CHANGE operation is going on,
		 *  discard any event received on the virtual I/F
		 */
		return (BCME_OK);
	}

	ASSERT(dhd->iflist[*ifidx] != NULL);
	ASSERT(dhd->iflist[*ifidx]->net != NULL);
	if (dhd->iflist[*ifidx]->event2cfg80211 && dhd->iflist[*ifidx]->net) {
		wl_cfg80211_event(dhd->iflist[*ifidx]->net, event, *data);
	}
#endif /* defined(WL_CFG80211) */

	return (bcmerror);
}

/* send up locally generated event */
void
dhd_sendup_event(dhd_pub_t *dhdp, wl_event_msg_t *event, void *data)
{
	switch (ntoh32(event->event_type)) {
#ifdef WLBTAMP
	/* Send up locally generated AMP HCI Events */
	case WLC_E_BTA_HCI_EVENT: {
		struct sk_buff *p, *skb;
		bcm_event_t *msg;
		wl_event_msg_t *p_bcm_event;
		char *ptr;
		uint32 len;
		uint32 pktlen;
		dhd_if_t *ifp;
		dhd_info_t *dhd;
		uchar *eth;
		int ifidx;

		len = ntoh32(event->datalen);
		pktlen = sizeof(bcm_event_t) + len + 2;
		dhd = dhdp->info;
		ifidx = dhd_ifname2idx(dhd, event->ifname);

		if ((p = PKTGET(dhdp->osh, pktlen, FALSE))) {
			ASSERT(ISALIGNED((uintptr)PKTDATA(dhdp->osh, p), sizeof(uint32)));

			msg = (bcm_event_t *) PKTDATA(dhdp->osh, p);

			bcopy(&dhdp->mac, &msg->eth.ether_dhost, ETHER_ADDR_LEN);
			bcopy(&dhdp->mac, &msg->eth.ether_shost, ETHER_ADDR_LEN);
			ETHER_TOGGLE_LOCALADDR(&msg->eth.ether_shost);

			msg->eth.ether_type = hton16(ETHER_TYPE_BRCM);

			/* BCM Vendor specific header... */
			msg->bcm_hdr.subtype = hton16(BCMILCP_SUBTYPE_VENDOR_LONG);
			msg->bcm_hdr.version = BCMILCP_BCM_SUBTYPEHDR_VERSION;
			bcopy(BRCM_OUI, &msg->bcm_hdr.oui[0], DOT11_OUI_LEN);

			/* vendor spec header length + pvt data length (private indication
			 *  hdr + actual message itself)
			 */
			msg->bcm_hdr.length = hton16(BCMILCP_BCM_SUBTYPEHDR_MINLENGTH +
				BCM_MSG_LEN + sizeof(wl_event_msg_t) + (uint16)len);
			msg->bcm_hdr.usr_subtype = hton16(BCMILCP_BCM_SUBTYPE_EVENT);

			PKTSETLEN(dhdp->osh, p, (sizeof(bcm_event_t) + len + 2));

			/* copy  wl_event_msg_t into sk_buf */

			/* pointer to wl_event_msg_t in sk_buf */
			p_bcm_event = &msg->event;
			bcopy(event, p_bcm_event, sizeof(wl_event_msg_t));

			/* copy hci event into sk_buf */
			bcopy(data, (p_bcm_event + 1), len);

			msg->bcm_hdr.length  = hton16(sizeof(wl_event_msg_t) +
				ntoh16(msg->bcm_hdr.length));
			PKTSETLEN(dhdp->osh, p, (sizeof(bcm_event_t) + len + 2));

			ptr = (char *)(msg + 1);
			/* Last 2 bytes of the message are 0x00 0x00 to signal that there
			 * are no ethertypes which are following this
			 */
			ptr[len+0] = 0x00;
			ptr[len+1] = 0x00;

			skb = PKTTONATIVE(dhdp->osh, p);
			eth = skb->data;
			len = skb->len;

			ifp = dhd->iflist[ifidx];
			if (ifp == NULL)
			     ifp = dhd->iflist[0];

			ASSERT(ifp);
			skb->dev = ifp->net;
			skb->protocol = eth_type_trans(skb, skb->dev);

			skb->data = eth;
			skb->len = len;

			/* Strip header, count, deliver upward */
			skb_pull(skb, ETH_HLEN);

			/* Send the packet */
			if (in_interrupt()) {
				netif_rx(skb);
			} else {
				netif_rx_ni(skb);
			}
		}
		else {
			/* Could not allocate a sk_buf */
			DHD_ERROR(("%s: unable to alloc sk_buf", __FUNCTION__));
		}
		break;
	} /* case WLC_E_BTA_HCI_EVENT */
#endif /* WLBTAMP */

	default:
		break;
	}
}

void dhd_wait_for_event(dhd_pub_t *dhd, bool *lockvar)
{
#if 0 && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
	struct dhd_info *dhdinfo =  dhd->info;
	dhd_os_sdunlock(dhd);
	wait_event_interruptible_timeout(dhdinfo->ctrl_wait, (*lockvar == FALSE), HZ * 2);
	dhd_os_sdlock(dhd);
#endif
	return;
}

void dhd_wait_event_wakeup(dhd_pub_t *dhd)
{
#if 0 && (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
	struct dhd_info *dhdinfo =  dhd->info;
	if (waitqueue_active(&dhdinfo->ctrl_wait))
		wake_up_interruptible(&dhdinfo->ctrl_wait);
#endif
	return;
}


int net_os_set_suspend_disable(struct net_device *dev, int val)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	int ret = 0;

	if (dhd) {
		ret = dhd->pub.suspend_disable_flag;
		dhd->pub.suspend_disable_flag = val;
	}
	return ret;
}

int net_os_set_suspend(struct net_device *dev, int val)
{
	int ret = 0;
#if defined(CONFIG_HAS_EARLYSUSPEND)
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);

	if (dhd) {
		ret = dhd_set_suspend(val, &dhd->pub);
	}
#endif /* defined(CONFIG_HAS_EARLYSUSPEND) */
	return ret;
}

int net_os_set_dtim_skip(struct net_device *dev, int val)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);

	if (dhd)
		dhd->pub.dtim_skip = val;

	return 0;
}

int net_os_rxfilter_add_remove(struct net_device *dev, int add_remove, int num)
{
#ifndef GAN_LITE_NAT_KEEPALIVE_FILTER
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	char *filterp = NULL;
	int ret = 0;

	if (!dhd || (num == DHD_UNICAST_FILTER_NUM))
		return ret;
	if (num >= dhd->pub.pktfilter_count)
		return -EINVAL;
	if (add_remove) {
		switch (num) {
		case DHD_BROADCAST_FILTER_NUM:
			filterp = "101 0 0 0 0xFFFFFFFFFFFF 0xFFFFFFFFFFFF";
			break;
		case DHD_MULTICAST4_FILTER_NUM:
			filterp = "102 0 0 0 0xFFFFFF 0x01005E";
			break;
		case DHD_MULTICAST6_FILTER_NUM:
			filterp = "103 0 0 0 0xFFFF 0x3333";
			break;
		default:
			return -EINVAL;
		}
	}
	dhd->pub.pktfilter[num] = filterp;
	return ret;
#else
	return 0;
#endif /* GAN_LITE_NAT_KEEPALIVE_FILTER */
}

int net_os_set_packet_filter(struct net_device *dev, int val)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	int ret = 0;

	/* Packet filtering is set only if we still in early-suspend and
	 * we need either to turn it ON or turn it OFF
	 * We can always turn it OFF in case of early-suspend, but we turn it
	 * back ON only if suspend_disable_flag was not set
	*/
	if (dhd && dhd->pub.up) {
		if (dhd->pub.in_suspend) {
			if (!val || (val && !dhd->pub.suspend_disable_flag))
				dhd_set_packet_filter(val, &dhd->pub);
		}
	}
	return ret;
}


void
dhd_dev_init_ioctl(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);

	dhd_preinit_ioctls(&dhd->pub);
}

#ifdef PNO_SUPPORT
/* Linux wrapper to call common dhd_pno_clean */
int
dhd_dev_pno_reset(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);

	return (dhd_pno_clean(&dhd->pub));
}


/* Linux wrapper to call common dhd_pno_enable */
int
dhd_dev_pno_enable(struct net_device *dev,  int pfn_enabled)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);

	return (dhd_pno_enable(&dhd->pub, pfn_enabled));
}


/* Linux wrapper to call common dhd_pno_set */
int
dhd_dev_pno_set(struct net_device *dev, wlc_ssid_t* ssids_local, int nssid,
	ushort  scan_fr, int pno_repeat, int pno_freq_expo_max)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);

	return (dhd_pno_set(&dhd->pub, ssids_local, nssid, scan_fr, pno_repeat, pno_freq_expo_max));
}

/* Linux wrapper to get  pno status */
int
dhd_dev_get_pno_status(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);

	return (dhd_pno_get_status(&dhd->pub));
}

#endif /* PNO_SUPPORT */


void dhd_net_if_lock(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	dhd_net_if_lock_local(dhd);
}

void dhd_net_if_unlock(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	dhd_net_if_unlock_local(dhd);
}

static void dhd_net_if_lock_local(dhd_info_t *dhd)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)) && 0
	if (dhd)
		mutex_lock(&dhd->dhd_net_if_mutex);
#endif
}

static void dhd_net_if_unlock_local(dhd_info_t *dhd)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)) && 0
	if (dhd)
		mutex_unlock(&dhd->dhd_net_if_mutex);
#endif
}

unsigned long dhd_os_spin_lock(dhd_pub_t *pub)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);
	unsigned long flags = 0;

	if (dhd)
		spin_lock_irqsave(&dhd->dhd_lock, flags);

	return flags;
}

void dhd_os_spin_unlock(dhd_pub_t *pub, unsigned long flags)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);

	if (dhd)
		spin_unlock_irqrestore(&dhd->dhd_lock, flags);
}

static int
dhd_get_pend_8021x_cnt(dhd_info_t *dhd)
{
	return (atomic_read(&dhd->pend_8021x_cnt));
}

#define MAX_WAIT_FOR_8021X_TX	25

int
dhd_wait_pend8021x(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	int timeout = 10 * HZ / 1000;
	int ntimes = MAX_WAIT_FOR_8021X_TX;
	int pend = dhd_get_pend_8021x_cnt(dhd);

	while (ntimes && pend) {
		if (pend) {
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(timeout);
			set_current_state(TASK_RUNNING);
			ntimes--;
		}
		pend = dhd_get_pend_8021x_cnt(dhd);
	}
	if (ntimes == 0)
		DHD_ERROR(("%s: TIMEOUT\n", __FUNCTION__));
	return pend;
}

#ifdef DHD_DEBUG
int
write_to_file(dhd_pub_t *dhd, uint8 *buf, int size)
{
	int ret = 0;
	struct file *fp;
	mm_segment_t old_fs;
	loff_t pos = 0;

	/* change to KERNEL_DS address limit */
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	/* open file to write */
	fp = filp_open("/tmp/mem_dump", O_WRONLY|O_CREAT, 0640);
	if (!fp) {
		printf("%s: open file error\n", __FUNCTION__);
		ret = -1;
		goto exit;
	}

	/* Write buf to file */
	fp->f_op->write(fp, buf, size, &pos);

exit:
	/* free buf before return */
	MFREE(dhd->osh, buf, size);
	/* close file before return */
	if (fp)
		filp_close(fp, current->files);
	/* restore previous address limit */
	set_fs(old_fs);

	return ret;
}
#endif /* DHD_DEBUG */

int dhd_os_wake_lock_timeout(dhd_pub_t *pub)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);
	unsigned long flags;
	int ret = 0;

	if (dhd) {
		spin_lock_irqsave(&dhd->wakelock_spinlock, flags);
		ret = dhd->wakelock_timeout_enable;
#ifdef CONFIG_HAS_WAKELOCK
		if (dhd->wakelock_timeout_enable)
			wake_lock_timeout(&dhd->wl_rxwake,
				msecs_to_jiffies(dhd->wakelock_timeout_enable));
#endif
		dhd->wakelock_timeout_enable = 0;
		spin_unlock_irqrestore(&dhd->wakelock_spinlock, flags);
	}
	return ret;
}

int net_os_wake_lock_timeout(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	int ret = 0;

	if (dhd)
		ret = dhd_os_wake_lock_timeout(&dhd->pub);
	return ret;
}

int dhd_os_wake_lock_timeout_enable(dhd_pub_t *pub, int val)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);
	unsigned long flags;

	if (dhd) {
		spin_lock_irqsave(&dhd->wakelock_spinlock, flags);
		if (val > dhd->wakelock_timeout_enable)
			dhd->wakelock_timeout_enable = val;
		spin_unlock_irqrestore(&dhd->wakelock_spinlock, flags);
	}
	return 0;
}

int net_os_wake_lock_timeout_enable(struct net_device *dev, int val)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	int ret = 0;

	if (dhd)
		ret = dhd_os_wake_lock_timeout_enable(&dhd->pub, val);
	return ret;
}

int dhd_os_wake_lock(dhd_pub_t *pub)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);
	unsigned long flags;
	int ret = 0;

	if (dhd) {
		spin_lock_irqsave(&dhd->wakelock_spinlock, flags);
#ifdef CONFIG_HAS_WAKELOCK
		if (!dhd->wakelock_counter)
			wake_lock(&dhd->wl_wifi);
#endif
		dhd->wakelock_counter++;
		ret = dhd->wakelock_counter;
		spin_unlock_irqrestore(&dhd->wakelock_spinlock, flags);
	}
	return ret;
}

int net_os_wake_lock(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	int ret = 0;

	if (dhd)
		ret = dhd_os_wake_lock(&dhd->pub);
	return ret;
}

int dhd_os_wake_unlock(dhd_pub_t *pub)
{
	dhd_info_t *dhd = (dhd_info_t *)(pub->info);
	unsigned long flags;
	int ret = 0;

	dhd_os_wake_lock_timeout(pub);
	if (dhd) {
		spin_lock_irqsave(&dhd->wakelock_spinlock, flags);
		if (dhd->wakelock_counter) {
			dhd->wakelock_counter--;
#ifdef CONFIG_HAS_WAKELOCK
			if (!dhd->wakelock_counter)
				wake_unlock(&dhd->wl_wifi);
#endif
			ret = dhd->wakelock_counter;
		}
		spin_unlock_irqrestore(&dhd->wakelock_spinlock, flags);
	}
	return ret;
}

int dhd_os_check_wakelock(void *dhdp)
{
#ifdef CONFIG_HAS_WAKELOCK
	dhd_pub_t *pub = (dhd_pub_t *)dhdp;
	dhd_info_t *dhd;

	if (!pub)
		return 0;
	dhd = (dhd_info_t *)(pub->info);

	if (dhd && wake_lock_active(&dhd->wl_wifi))
		return 1;
#endif
	return 0;
}
int net_os_wake_unlock(struct net_device *dev)
{
	dhd_info_t *dhd = *(dhd_info_t **)netdev_priv(dev);
	int ret = 0;

	if (dhd)
		ret = dhd_os_wake_unlock(&dhd->pub);
	return ret;
}

int dhd_os_check_if_up(void *dhdp)
{
	dhd_pub_t *pub = (dhd_pub_t *)dhdp;

	if (!pub)
		return 0;
	return pub->up;
}
int dhd_ioctl_entry_local(struct net_device *net, wl_ioctl_t *ioc, int cmd)
{
	int ifidx;
	int ret = 0;
	dhd_info_t *dhd = NULL;

	if (!net || !netdev_priv(net)) {
		DHD_ERROR(("%s invalid parameter\n", __FUNCTION__));
		return -EINVAL;
	}

	dhd = *(dhd_info_t **)netdev_priv(net);
	ifidx = dhd_net2idx(dhd, net);
	if (ifidx == DHD_BAD_IF) {
		DHD_ERROR(("%s bad ifidx\n", __FUNCTION__));
		return -ENODEV;
	}

	DHD_OS_WAKE_LOCK(&dhd->pub);
	ret = dhd_wl_ioctl(&dhd->pub, ifidx, ioc, ioc->buf, ioc->len);
	dhd_check_hang(net, &dhd->pub, ret);
	DHD_OS_WAKE_UNLOCK(&dhd->pub);

	return ret;
}

bool dhd_os_check_hang(dhd_pub_t *dhdp, int ifidx, int ret)
{
	struct net_device *net;

	net = dhd_idx2net(dhdp, ifidx);
	return dhd_check_hang(net, dhdp, ret);
}

#ifdef PROP_TXSTATUS
extern int dhd_wlfc_interface_entry_update(void* state,	ewlfc_mac_entry_action_t action, uint8 ifid,
	uint8 iftype, uint8* ea);
extern int dhd_wlfc_FIFOcreditmap_update(void* state, uint8* credits);

int dhd_wlfc_interface_event(struct dhd_info *dhd,
	ewlfc_mac_entry_action_t action, uint8 ifid, uint8 iftype, uint8* ea)
{
	if (dhd->pub.wlfc_state == NULL)
		return BCME_OK;

	return dhd_wlfc_interface_entry_update(dhd->pub.wlfc_state, action, ifid, iftype, ea);
}

int dhd_wlfc_FIFOcreditmap_event(struct dhd_info *dhd, uint8* event_data)
{
	if (dhd->pub.wlfc_state == NULL)
		return BCME_OK;

	return dhd_wlfc_FIFOcreditmap_update(dhd->pub.wlfc_state, event_data);
}

int dhd_wlfc_event(struct dhd_info *dhd)
{
	return dhd_wlfc_enable(&dhd->pub);
}
#endif /* PROP_TXSTATUS */

#ifdef BCMDBGFS

#include <linux/debugfs.h>

extern uint32 dhd_readregl(void *bp, uint32 addr);
extern uint32 dhd_writeregl(void *bp, uint32 addr, uint32 data);

typedef struct dhd_dbgfs {
	struct dentry	*debugfs_dir;
	struct dentry	*debugfs_mem;
	dhd_pub_t 	*dhdp;
	uint32 		size;
} dhd_dbgfs_t;

dhd_dbgfs_t g_dbgfs;

static int
dhd_dbg_state_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static ssize_t
dhd_dbg_state_read(struct file *file, char __user *ubuf,
                       size_t count, loff_t *ppos)
{
	ssize_t rval;
	uint32 tmp;
	loff_t pos = *ppos;
	size_t ret;

	if (pos < 0)
		return -EINVAL;
	if (pos >= g_dbgfs.size || !count)
		return 0;
	if (count > g_dbgfs.size - pos)
		count = g_dbgfs.size - pos;

	/* Basically enforce aligned 4 byte reads. It's up to the user to work out the details */
	tmp = dhd_readregl(g_dbgfs.dhdp->bus, file->f_pos & (~3));

	ret = copy_to_user(ubuf, &tmp, 4);
	if (ret == count)
		return -EFAULT;

	count -= ret;
	*ppos = pos + count;
	rval = count;

	return rval;
}


static ssize_t
dhd_debugfs_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)
{
	loff_t pos = *ppos;
	size_t ret;
	uint32 buf;

	if (pos < 0)
		return -EINVAL;
	if (pos >= g_dbgfs.size || !count)
		return 0;
	if (count > g_dbgfs.size - pos)
		count = g_dbgfs.size - pos;

	ret = copy_from_user(&buf, ubuf, sizeof(uint32));
	if (ret == count)
		return -EFAULT;

	/* Basically enforce aligned 4 byte writes. It's up to the user to work out the details */
	dhd_writeregl(g_dbgfs.dhdp->bus, file->f_pos & (~3), buf);

	return count;
}


loff_t
dhd_debugfs_lseek(struct file *file, loff_t off, int whence)
{
	loff_t pos = -1;

	switch (whence) {
		case 0:
			pos = off;
			break;
		case 1:
			pos = file->f_pos + off;
			break;
		case 2:
			pos = g_dbgfs.size - off;
	}
	return (pos < 0 || pos > g_dbgfs.size) ? -EINVAL : (file->f_pos = pos);
}

static const struct file_operations dhd_dbg_state_ops = {
	.read   = dhd_dbg_state_read,
	.write	= dhd_debugfs_write,
	.open   = dhd_dbg_state_open,
	.llseek	= dhd_debugfs_lseek
};

static void dhd_dbg_create(void)
{
	if (g_dbgfs.debugfs_dir) {
		g_dbgfs.debugfs_mem = debugfs_create_file("mem", 0644, g_dbgfs.debugfs_dir,
			NULL, &dhd_dbg_state_ops);
	}
}

void dhd_dbg_init(dhd_pub_t *dhdp)
{
	int err;

	g_dbgfs.dhdp = dhdp;
	g_dbgfs.size = 0x20000000; /* Allow access to various cores regs */

	g_dbgfs.debugfs_dir = debugfs_create_dir("dhd", 0);
	if (IS_ERR(g_dbgfs.debugfs_dir)) {
		err = PTR_ERR(g_dbgfs.debugfs_dir);
		g_dbgfs.debugfs_dir = NULL;
		return;
	}

	dhd_dbg_create();

	return;
}

void dhd_dbg_remove(void)
{
	debugfs_remove(g_dbgfs.debugfs_mem);
	debugfs_remove(g_dbgfs.debugfs_dir);

	bzero((unsigned char *) &g_dbgfs, sizeof(g_dbgfs));

}
#endif /* ifdef BCMDBGFS */

#ifdef WLMEDIA_HTSF

static
void dhd_htsf_addtxts(dhd_pub_t *dhdp, void *pktbuf)
{
	dhd_info_t *dhd = (dhd_info_t *)(dhdp->info);
	struct sk_buff *skb;
	uint32 htsf = 0;
	uint16 dport = 0, oldmagic = 0xACAC;
	char *p1;
	htsfts_t ts;

	/*  timestamp packet  */

	p1 = (char*) PKTDATA(dhdp->osh, pktbuf);

	if (PKTLEN(dhdp->osh, pktbuf) > HTSF_MINLEN) {
/*		memcpy(&proto, p1+26, 4);  	*/
		memcpy(&dport, p1+40, 2);
/* 	proto = ((ntoh32(proto))>> 16) & 0xFF;  */
		dport = ntoh16(dport);
	}

	/* timestamp only if  icmp or udb iperf with port 5555 */
/*	if (proto == 17 && dport == tsport) { */
	if (dport >= tsport && dport <= tsport + 20) {

		skb = (struct sk_buff *) pktbuf;

		htsf = dhd_get_htsf(dhd, 0);
		memset(skb->data + 44, 0, 2); /* clear checksum */
		memcpy(skb->data+82, &oldmagic, 2);
		memcpy(skb->data+84, &htsf, 4);

		memset(&ts, 0, sizeof(htsfts_t));
		ts.magic  = HTSFMAGIC;
		ts.prio   = PKTPRIO(pktbuf);
		ts.seqnum = htsf_seqnum++;
		ts.c10    = get_cycles();
		ts.t10    = htsf;
		ts.endmagic = HTSFENDMAGIC;

		memcpy(skb->data + HTSF_HOSTOFFSET, &ts, sizeof(ts));
	}
}

static void dhd_dump_htsfhisto(histo_t *his, char *s)
{
	int pktcnt = 0, curval = 0, i;
	for (i = 0; i < (NUMBIN-2); i++) {
		curval += 500;
		printf("%d ",  his->bin[i]);
		pktcnt += his->bin[i];
	}
	printf(" max: %d TotPkt: %d neg: %d [%s]\n", his->bin[NUMBIN-2], pktcnt,
		his->bin[NUMBIN-1], s);
}

static
void sorttobin(int value, histo_t *histo)
{
	int i, binval = 0;

	if (value < 0) {
		histo->bin[NUMBIN-1]++;
		return;
	}
	if (value > histo->bin[NUMBIN-2])  /* store the max value  */
		histo->bin[NUMBIN-2] = value;

	for (i = 0; i < (NUMBIN-2); i++) {
		binval += 500; /* 500m s bins */
		if (value <= binval) {
			histo->bin[i]++;
			return;
		}
	}
	histo->bin[NUMBIN-3]++;
}

static
void dhd_htsf_addrxts(dhd_pub_t *dhdp, void *pktbuf)
{
	dhd_info_t *dhd = (dhd_info_t *)dhdp->info;
	struct sk_buff *skb;
	char *p1;
	uint16 old_magic;
	int d1, d2, d3, end2end;
	htsfts_t *htsf_ts;
	uint32 htsf;

	skb = PKTTONATIVE(dhdp->osh, pktbuf);
	p1 = (char*)PKTDATA(dhdp->osh, pktbuf);

	if (PKTLEN(osh, pktbuf) > HTSF_MINLEN) {
		memcpy(&old_magic, p1+78, 2);
		htsf_ts = (htsfts_t*) (p1 + HTSF_HOSTOFFSET - 4);
	}
	else
		return;

	if (htsf_ts->magic == HTSFMAGIC) {
		htsf_ts->tE0 = dhd_get_htsf(dhd, 0);
		htsf_ts->cE0 = get_cycles();
	}

	if (old_magic == 0xACAC) {

		tspktcnt++;
		htsf = dhd_get_htsf(dhd, 0);
		memcpy(skb->data+92, &htsf, sizeof(uint32));

		memcpy(&ts[tsidx].t1, skb->data+80, 16);

		d1 = ts[tsidx].t2 - ts[tsidx].t1;
		d2 = ts[tsidx].t3 - ts[tsidx].t2;
		d3 = ts[tsidx].t4 - ts[tsidx].t3;
		end2end = ts[tsidx].t4 - ts[tsidx].t1;

		sorttobin(d1, &vi_d1);
		sorttobin(d2, &vi_d2);
		sorttobin(d3, &vi_d3);
		sorttobin(end2end, &vi_d4);

		if (end2end > 0 && end2end >  maxdelay) {
			maxdelay = end2end;
			maxdelaypktno = tspktcnt;
			memcpy(&maxdelayts, &ts[tsidx], 16);
		}
		if (++tsidx >= TSMAX)
			tsidx = 0;
	}
}

uint32 dhd_get_htsf(dhd_info_t *dhd, int ifidx)
{
	uint32 htsf = 0, cur_cycle, delta, delta_us;
	uint32    factor, baseval, baseval2;
	cycles_t t;

	t = get_cycles();
	cur_cycle = t;

	if (cur_cycle >  dhd->htsf.last_cycle)
		delta = cur_cycle -  dhd->htsf.last_cycle;
	else {
		delta = cur_cycle + (0xFFFFFFFF -  dhd->htsf.last_cycle);
	}

	delta = delta >> 4;

	if (dhd->htsf.coef) {
		/* times ten to get the first digit */
	        factor = (dhd->htsf.coef*10 + dhd->htsf.coefdec1);
		baseval  = (delta*10)/factor;
		baseval2 = (delta*10)/(factor+1);
		delta_us  = (baseval -  (((baseval - baseval2) * dhd->htsf.coefdec2)) / 10);
		htsf = (delta_us << 4) +  dhd->htsf.last_tsf + HTSF_BUS_DELAY;
	}
	else {
		DHD_ERROR(("-------dhd->htsf.coef = 0 -------\n"));
	}

	return htsf;
}

static void dhd_dump_latency(void)
{
	int i, max = 0;
	int d1, d2, d3, d4, d5;

	printf("T1       T2       T3       T4           d1  d2   t4-t1     i    \n");
	for (i = 0; i < TSMAX; i++) {
		d1 = ts[i].t2 - ts[i].t1;
		d2 = ts[i].t3 - ts[i].t2;
		d3 = ts[i].t4 - ts[i].t3;
		d4 = ts[i].t4 - ts[i].t1;
		d5 = ts[max].t4-ts[max].t1;
		if (d4 > d5 && d4 > 0)  {
			max = i;
		}
		printf("%08X %08X %08X %08X \t%d %d %d   %d i=%d\n",
			ts[i].t1, ts[i].t2, ts[i].t3, ts[i].t4,
			d1, d2, d3, d4, i);
	}

	printf("current idx = %d \n", tsidx);

	printf("Highest latency %d pkt no.%d total=%d\n", maxdelay, maxdelaypktno, tspktcnt);
	printf("%08X %08X %08X %08X \t%d %d %d   %d\n",
	maxdelayts.t1, maxdelayts.t2, maxdelayts.t3, maxdelayts.t4,
	maxdelayts.t2 - maxdelayts.t1,
	maxdelayts.t3 - maxdelayts.t2,
	maxdelayts.t4 - maxdelayts.t3,
	maxdelayts.t4 - maxdelayts.t1);
}


static int
dhd_ioctl_htsf_get(dhd_info_t *dhd, int ifidx)
{
	wl_ioctl_t ioc;
	char buf[32];
	int ret;
	uint32 s1, s2;

	struct tsf {
		uint32 low;
		uint32 high;
	} tsf_buf;

	memset(&ioc, 0, sizeof(ioc));
	memset(&tsf_buf, 0, sizeof(tsf_buf));

	ioc.cmd = WLC_GET_VAR;
	ioc.buf = buf;
	ioc.len = (uint)sizeof(buf);
	ioc.set = FALSE;

	strncpy(buf, "tsf", sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';
	s1 = dhd_get_htsf(dhd, 0);
	if ((ret = dhd_wl_ioctl(&dhd->pub, ifidx, &ioc, ioc.buf, ioc.len)) < 0) {
		if (ret == -EIO) {
			DHD_ERROR(("%s: tsf is not supported by device\n",
				dhd_ifname(&dhd->pub, ifidx)));
			return -EOPNOTSUPP;
		}
		return ret;
	}
	s2 = dhd_get_htsf(dhd, 0);

	memcpy(&tsf_buf, buf, sizeof(tsf_buf));
	printf(" TSF_h=%04X lo=%08X Calc:htsf=%08X, coef=%d.%d%d delta=%d ",
		tsf_buf.high, tsf_buf.low, s2, dhd->htsf.coef, dhd->htsf.coefdec1,
		dhd->htsf.coefdec2, s2-tsf_buf.low);
	printf("lasttsf=%08X lastcycle=%08X\n", dhd->htsf.last_tsf, dhd->htsf.last_cycle);
	return 0;
}

void htsf_update(dhd_info_t *dhd, void *data)
{
	static ulong  cur_cycle = 0, prev_cycle = 0;
	uint32 htsf, tsf_delta = 0;
	uint32 hfactor = 0, cyc_delta, dec1 = 0, dec2, dec3, tmp;
	ulong b, a;
	cycles_t t;

	/* cycles_t in inlcude/mips/timex.h */

	t = get_cycles();

	prev_cycle = cur_cycle;
	cur_cycle = t;

	if (cur_cycle > prev_cycle)
		cyc_delta = cur_cycle - prev_cycle;
	else {
		b = cur_cycle;
		a = prev_cycle;
		cyc_delta = cur_cycle + (0xFFFFFFFF - prev_cycle);
	}

	if (data == NULL)
		printf(" tsf update ata point er is null \n");

	memcpy(&prev_tsf, &cur_tsf, sizeof(tsf_t));
	memcpy(&cur_tsf, data, sizeof(tsf_t));

	if (cur_tsf.low == 0) {
		DHD_INFO((" ---- 0 TSF, do not update, return\n"));
		return;
	}

	if (cur_tsf.low > prev_tsf.low)
		tsf_delta = (cur_tsf.low - prev_tsf.low);
	else {
		DHD_INFO((" ---- tsf low is smaller cur_tsf= %08X, prev_tsf=%08X, \n",
		 cur_tsf.low, prev_tsf.low));
		if (cur_tsf.high > prev_tsf.high) {
			tsf_delta = cur_tsf.low + (0xFFFFFFFF - prev_tsf.low);
			DHD_INFO((" ---- Wrap around tsf coutner  adjusted TSF=%08X\n", tsf_delta));
		}
		else
			return; /* do not update */
	}

	if (tsf_delta)  {
		hfactor = cyc_delta / tsf_delta;
		tmp  = 	(cyc_delta - (hfactor * tsf_delta))*10;
		dec1 =  tmp/tsf_delta;
		dec2 =  ((tmp - dec1*tsf_delta)*10) / tsf_delta;
		tmp  = 	(tmp   - (dec1*tsf_delta))*10;
		dec3 =  ((tmp - dec2*tsf_delta)*10) / tsf_delta;

		if (dec3 > 4) {
			if (dec2 == 9) {
				dec2 = 0;
				if (dec1 == 9) {
					dec1 = 0;
					hfactor++;
				}
				else {
					dec1++;
				}
			}
			else
				dec2++;
		}
	}

	if (hfactor) {
		htsf = ((cyc_delta * 10)  / (hfactor*10+dec1)) + prev_tsf.low;
		dhd->htsf.coef = hfactor;
		dhd->htsf.last_cycle = cur_cycle;
		dhd->htsf.last_tsf = cur_tsf.low;
		dhd->htsf.coefdec1 = dec1;
		dhd->htsf.coefdec2 = dec2;
	}
	else {
		htsf = prev_tsf.low;
	}
}

#endif /* WLMEDIA_HTSF */
/**********************************************************
function name: dhd_wifi_config_mode

description: 
   get wifi running mode in kernel
input:
	
output:
-1: error
0: ap mode
1: ap/sta mode
2: mbss mode

	
**********************************************************/
int dhd_wifi_config_mode(dhd_info_t *dhd)
{
    int ret = 0;
    int mode = -1;

    char buf[128] = {0};    
    int *mode_tmp = &buf;
    
    DHD_TRACE(("%s: 50: enter\n", __FUNCTION__));

    //bcm_mkiovar("apsta", (char *)&apsta, 4, iovbuf, sizeof(iovbuf));

    bcm_mkiovar("ap", 0, 0, buf, sizeof(buf));

    //if ((ret  = dhd_wl_ioctl_cmd(&dhd->pub, WLC_GET_VAR, buf, sizeof(buf), FALSE, 0)) < 0) {
    if ((ret  = dhd_wl_ioctl_cmd(&dhd->pub, WLC_GET_AP, buf, sizeof(buf), FALSE, 0)) < 0) {
        DHD_TRACE(("%s: 100: get ap failed%d\n", __FUNCTION__, ret));
        return 0;
    }


    //buf = dtoh32(buf);
    #if 0
    WIFI_TRACE(("%s:150: mode_tmp = %d\n", __FUNCTION__, *mode_tmp));
    
    {
        int i = 0;
        
        WIFI_TRACE(("%s:160: buf content begin\n", __FUNCTION__));
        for(i = 0; i < sizeof(buf); i++)
        {
            WIFI_TRACE(("%x ", buf[i]));
        }
        
        WIFI_TRACE(("\n%s:170: buf content end\n", __FUNCTION__));
    }
    
    
    WIFI_TRACE(("%s:180: mode_tmp = %d\n", __FUNCTION__, *mode_tmp));
    #endif
    if(*mode_tmp == 1)
    {
        //this is default mode, don't need to return
        mode = 0;        
        DHD_TRACE(("%s:200: in ap mode\n", __FUNCTION__));
    }
    else
    {
        DHD_TRACE(("%s:300: not ap mode\n", __FUNCTION__));
        return -1;
    }


    bcm_mkiovar("apsta", 0, 0, buf, sizeof(buf));

    if ((ret  = dhd_wl_ioctl_cmd(&dhd->pub, WLC_GET_VAR, buf, sizeof(buf), FALSE, 0)) < 0) {
        DHD_TRACE(("%s:400: get apsta failed: %d\n", __FUNCTION__, ret));
        return 0;
    }

    //buf = dtoh32(buf);

    if(*mode_tmp == 1)
    {
        mode = 1;
        
        DHD_TRACE(("%s:500:  mode %d\n", __FUNCTION__, mode));
        return mode;
    }

    bcm_mkiovar("mbss", 0, 0, buf, sizeof(buf));

    if ((ret  = dhd_wl_ioctl_cmd(&dhd->pub, WLC_GET_VAR, buf, sizeof(buf), FALSE, 0)) < 0) {
        DHD_TRACE(("%s:600: get mbss failed %d\n", __FUNCTION__, ret));
        return 0;
    }

    //buf = dtoh32(buf);

    if(*mode_tmp == 1)
    {
        mode = 2;
        DHD_TRACE(("%s:700: mode :%d\n", __FUNCTION__, mode));  
        return mode;
    }

    return mode;

}



int dhd_get_interface_assoc_num(dhd_info_t *dhd, int ifindex)
{

    int ret = 0;
    char mac_buf[256] = {0};

    struct maclist *assoc_maclist = (struct maclist *) mac_buf;
    
    
    DHD_TRACE(("%s:100: enter, ifindex = %d\n", __FUNCTION__, ifindex));
    if((ifindex != 0) && (ifindex != 1))
    {
        ifindex = 0;
    }

    //max is 5
    assoc_maclist->count = 5;
    
    // bcm_mkiovar("assoclist", 0, 0, mac_buf, sizeof(mac_buf));

    if ((ret  = dhd_wl_ioctl_cmd(&dhd->pub, WLC_GET_ASSOCLIST, mac_buf, sizeof(mac_buf), FALSE, ifindex)) < 0) {
        DHD_TRACE(("%s:200: read assoclist failed %d\n", __FUNCTION__, ret));
        return -1;
    }

    return assoc_maclist->count;

}


int dhd_ap_get_sta_num()
{
    if(g_wifi_dev == NULL)
    {
        DHD_TRACE(("%s:200: g_wifi_dev == NULL\n", __FUNCTION__));
        return -1;
    }  
    return g_ap_sta_num;
#if 0
    int mode = 0;
    int tmp = 0;
    int mssid_tmp = 0;
    int cnt = 0;

    dhd_info_t *dhd;
    DHD_TRACE(("%s:100: enter\n", __FUNCTION__));

    
    

    dhd = *(dhd_info_t **)netdev_priv(g_wifi_dev);
    DHD_TRACE(("%s:220: dhd addr = %p\n", __FUNCTION__, dhd));
    DHD_TRACE(("%s:230: adpater name %s, dbus state %d\n", __FUNCTION__, g_wifi_dev->name, dhd->pub.busstate));

    mode = dhd_wifi_config_mode(dhd);

    //1: apsta  2: mbss

    if(mode == 0)
    {
        cnt = dhd_get_interface_assoc_num(dhd, 0);
        if(cnt == -1)
        {
            DHD_TRACE(("%s:300: error to get assoc_num\n", __FUNCTION__));

        }
        return cnt; //-1 or correct count
    }
    else if(mode == 1)
    {
        cnt = dhd_get_interface_assoc_num(dhd, 1);
        if(cnt == -1)
        {
            DHD_TRACE(("%s:400: error to get assoc_num\n", __FUNCTION__));
        }
        return cnt; //-1 or correct count
    }
    else if(mode == 2)
    {
        //mbss mode
        tmp = dhd_get_interface_assoc_num(dhd, 0);
        if(tmp == -1)
        {
            DHD_TRACE(("%s:500: error to get assoc_num\n", __FUNCTION__));
            mssid_tmp = -1;
            cnt = 0;
        }
        else
        {
            mssid_tmp = tmp;
            cnt = tmp;
        }

        tmp = dhd_get_interface_assoc_num(dhd, 1);
        if(tmp == -1)
        {
            DHD_TRACE(("%s:600: error to get assoc_num\n", __FUNCTION__));
            if(mssid_tmp == -1)
            {
                //both ssid got bad result
                return -1;
            }
        }
        else
        {
            cnt += tmp;//at least we got one count,  just one ssid is enough to meet the design purpose
        }

        return cnt; //-1 or correct count
    }
    else
    {
        //-1 just return -1
        return -1;
    }
#endif
}

//****************function for ecall******************************


int v7_dhd_ranger_init()
{   
    memset(&v7_dhd_ranger, 0, sizeof(v7_dhd_ranger));
    return 0;
}

void v7_dhd_ranger_dump()
{
    char v7_info[] = "sr74, sbm and em merged version \nCompiled on " __DATE__ " at " __TIME__;
    printk("%s\n",v7_info);

    printk("txoff_drop_cnt = %u\n", v7_dhd_ranger.txoff_drop_cnt);
    printk("netif_autostop = %d\n", v7_dhd_ranger.netif_autostop);
    printk("end to dump\n");

}

int force_set_netif_queue(int start_falg, int ifidx)
{
    if(start_falg == 1)
    {
        netif_stop_queue(v7_dhd_ranger.dhd->iflist[ifidx]->net);
    }
    else if(start_falg == 0)
    {
        netif_wake_queue(v7_dhd_ranger.dhd->iflist[ifidx]->net);
    }
    else
    {        
        DHD_TRACE(("%s: 300: start_falg = %d, ifidx = %d, invalid\n", __FUNCTION__, start_falg, ifidx));
    }
    
    return 0;
}



void force_set_netif_autostop(int flag)
{
    v7_dhd_ranger.netif_autostop = flag;
}

