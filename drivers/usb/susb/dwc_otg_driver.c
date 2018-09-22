/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg/linux/drivers/dwc_otg_driver.c $
 * $Revision: #91 $
 * $Date: 2011/10/24 $
 * $Change: 1871159 $
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */

/** @file
 * The dwc_otg_driver module provides the initialization and cleanup entry
 * points for the DWC_otg driver. This module will be dynamically installed
 * after Linux is booted using the insmod command. When the module is
 * installed, the dwc_otg_driver_init function is called. When the module is
 * removed (using rmmod), the dwc_otg_driver_cleanup function is called.
 *
 * This module also defines a data structure for the dwc_otg_driver, which is
 * used in conjunction with the standard ARM lm_device structure. These
 * structures allow the OTG driver to comply with the standard Linux driver
 * model in which devices and drivers are registered with a bus driver. This
 * has the benefit that Linux can expose attributes of the driver and device
 * in its special sysfs file system. Users can then read or write files in
 * this file system to perform diagnostics on the driver components or the
 * device.
 */

#include <linux/wait.h>

#include "dwc_otg_os_dep.h"
#include "dwc_os.h"
#include "dwc_otg_dbg.h"
#include "dwc_otg_driver.h"
#include "dwc_otg_attr.h"
#include "dwc_otg_core_if.h"
#include "dwc_otg_pcd_if.h"
#include "dwc_otg_hcd_if.h"

/* @debug */
#include "dwc_otg_cil.h"
#include "dwc_otg_hcd.h"
#include "dwc_otg_regs.h"
/* @end */

#if defined(FEATURE_HSIC_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
wait_queue_head_t hsic_disconnect_wait;
unsigned int hsic_disconnect = 0;
extern void DRV_SynopHsicBusSuspend(dwc_otg_core_if_t *core_if);
extern void DRV_SynopHsicBusResume(dwc_otg_core_if_t *core_if);
#endif


#include <generated/FeatureConfig.h>   /*syb*/
#include "BSP.h"

#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
#include <mach/pwrctrl/BSP_PWC_SLEEPMGR.h>
#include <linux/gpio.h>
#include <linux/netlink.h>
#include <mach/common/bsp_memory.h>

#define BALONG_GPIO_USB_VBUS_PIN    (27)
#define BALONG_GPIO_USB_VBUS        BALONG_GPIO_0(BALONG_GPIO_USB_VBUS_PIN)
#define GPIO_VBUS_NAME              "Vbus_enable"

#define VBUS_OPEN                   (1)
#define VBUS_CLOSE                  (0)

extern void uw_charger_set_soft_mode(void);
extern void uw_charger_set_bypass_mode(void);
extern int gpio_direction_output(unsigned gpio, int value);
extern int32_t sc_mem_crg_sc_ioremap(void);

static dwc_otg_core_if_t  * g_core_if = NULL;

static struct work_struct g_temper_check;

#endif

#if (FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))

#warning [duxj] FEATURE_OTG == FEATURE_ON!!!!!!!!!!!!!
extern int jusb_entry(void *args, void *core_if);
extern void hw_jungo_init();

int hw_dwc_otg_init();

#endif

#define DWC_DRIVER_VERSION	"2.94a 27-OCT-2011"
#define DWC_DRIVER_DESC		"HS OTG USB Controller driver"

static const char dwc_driver_name[] = "dwc_otg";
static dwc_otg_device_t *gdwc_otg_device = NULL;

extern int pcd_init(
#ifdef LM_INTERFACE
			   struct lm_device *_dev
#elif  defined(PCI_INTERFACE)
			   struct pci_dev *_dev
#endif
    );
extern int hcd_init(
#ifdef LM_INTERFACE
			   struct lm_device *_dev
#elif  defined(PCI_INTERFACE)
			   struct pci_dev *_dev
#endif
    );

extern int pcd_remove(
#ifdef LM_INTERFACE
			     struct lm_device *_dev
#elif  defined(PCI_INTERFACE)
			     struct pci_dev *_dev
#endif
    );

extern void hcd_remove(
#ifdef LM_INTERFACE
			      struct lm_device *_dev
#elif  defined(PCI_INTERFACE)
			      struct pci_dev *_dev
#endif
    );

extern void dwc_otg_adp_start(dwc_otg_core_if_t * core_if, uint8_t is_host);

/*-------------------------------------------------------------------------*/
/* Encapsulate the module parameter settings */

struct dwc_otg_driver_module_params {
	int32_t opt;
	int32_t otg_cap;
	int32_t dma_enable;
	int32_t dma_desc_enable;
	int32_t dma_burst_size;
	int32_t speed;
	int32_t host_support_fs_ls_low_power;
	int32_t host_ls_low_power_phy_clk;
	int32_t enable_dynamic_fifo;
	int32_t data_fifo_size;
	int32_t dev_rx_fifo_size;
	int32_t dev_nperio_tx_fifo_size;
	uint32_t dev_perio_tx_fifo_size[MAX_PERIO_FIFOS];
	int32_t host_rx_fifo_size;
	int32_t host_nperio_tx_fifo_size;
	int32_t host_perio_tx_fifo_size;
	int32_t max_transfer_size;
	int32_t max_packet_count;
	int32_t host_channels;
	int32_t dev_endpoints;
	int32_t phy_type;
	int32_t phy_utmi_width;
	int32_t phy_ulpi_ddr;
	int32_t phy_ulpi_ext_vbus;
	int32_t i2c_enable;
	int32_t ulpi_fs_ls;
	int32_t ts_dline;
	int32_t en_multiple_tx_fifo;
	uint32_t dev_tx_fifo_size[MAX_TX_FIFOS];
	uint32_t thr_ctl;
	uint32_t tx_thr_length;
	uint32_t rx_thr_length;
	int32_t pti_enable;
	int32_t mpi_enable;
	int32_t lpm_enable;
	int32_t ic_usb_cap;
	int32_t ahb_thr_ratio;
	int32_t power_down;
	int32_t reload_ctl;
	int32_t dev_out_nak;
	int32_t cont_on_bna;
	int32_t ahb_single;
	int32_t otg_ver;
	int32_t adp_enable;
};

static struct dwc_otg_driver_module_params dwc_otg_module_params = {
	.opt = -1,
	.otg_cap = -1,
	.dma_enable = -1,
	.dma_desc_enable = -1,
	.dma_burst_size = -1,
	.speed = -1,
	.host_support_fs_ls_low_power = -1,
	.host_ls_low_power_phy_clk = -1,
	.enable_dynamic_fifo = -1,
	.data_fifo_size = -1,
	.dev_rx_fifo_size = -1,
	.dev_nperio_tx_fifo_size = -1,
	.dev_perio_tx_fifo_size = {
				   /* dev_perio_tx_fifo_size_1 */
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1,
				   -1
				   /* 15 */
				   },
	.host_rx_fifo_size = -1,
	.host_nperio_tx_fifo_size = -1,
	.host_perio_tx_fifo_size = -1,
	.max_transfer_size = -1,
	.max_packet_count = -1,
	.host_channels = -1,
	.dev_endpoints = -1,
	.phy_type = -1,
	.phy_utmi_width = -1,
	.phy_ulpi_ddr = -1,
	.phy_ulpi_ext_vbus = -1,
	.i2c_enable = -1,
	.ulpi_fs_ls = -1,
	.ts_dline = -1,
	.en_multiple_tx_fifo = -1,
	.dev_tx_fifo_size = {
			     /* dev_tx_fifo_size */
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1,
			     -1
			     /* 15 */
			     },
	.thr_ctl = -1,
	.tx_thr_length = -1,
	.rx_thr_length = -1,
	.pti_enable = -1,
	.mpi_enable = -1,
	.lpm_enable = -1,
	.ic_usb_cap = -1,
	.ahb_thr_ratio = -1,
	.power_down = -1,
	.reload_ctl = -1,
	.dev_out_nak = -1,
	.cont_on_bna = -1,
	.ahb_single = -1,
	.otg_ver = -1,
	.adp_enable = -1,
};

/**
 * This function shows the Driver Version.
 */
static ssize_t version_show(struct device_driver *dev, char *buf)
{
	return snprintf(buf, sizeof(DWC_DRIVER_VERSION) + 2, "%s\n",
			DWC_DRIVER_VERSION);
}

static DRIVER_ATTR(version, S_IRUGO, version_show, NULL);

/**
 * Global Debug Level Mask.
 */
uint32_t g_dbg_lvl = 0;		/* OFF */

/**
 * This function shows the driver Debug Level.
 */
static ssize_t dbg_level_show(struct device_driver *drv, char *buf)
{
	return sprintf(buf, "0x%0x\n", g_dbg_lvl);
}

/**
 * This function stores the driver Debug Level.
 */
static ssize_t dbg_level_store(struct device_driver *drv, const char *buf,
			       size_t count)
{
	g_dbg_lvl = simple_strtoul(buf, NULL, 16);
	return count;
}

static DRIVER_ATTR(debuglevel, S_IRUGO | S_IWUSR, dbg_level_show,
		   dbg_level_store);

/**
 * This function is called during module intialization
 * to pass module parameters to the DWC_OTG CORE.
 */
static int set_parameters(dwc_otg_core_if_t * core_if)
{
	int retval = 0;
	int i;

	if (dwc_otg_module_params.otg_cap != -1) {
		retval +=
		    dwc_otg_set_param_otg_cap(core_if,
					      dwc_otg_module_params.otg_cap);
	}
	if (dwc_otg_module_params.dma_enable != -1) {
		retval +=
		    dwc_otg_set_param_dma_enable(core_if,
						 dwc_otg_module_params.
						 dma_enable);
	}
	if (dwc_otg_module_params.dma_desc_enable != -1) {
		retval +=
		    dwc_otg_set_param_dma_desc_enable(core_if,
						      dwc_otg_module_params.
						      dma_desc_enable);
	}
	if (dwc_otg_module_params.opt != -1) {
		retval +=
		    dwc_otg_set_param_opt(core_if, dwc_otg_module_params.opt);
	}
	if (dwc_otg_module_params.dma_burst_size != -1) {
		retval +=
		    dwc_otg_set_param_dma_burst_size(core_if,
						     dwc_otg_module_params.
						     dma_burst_size);
	}
	if (dwc_otg_module_params.host_support_fs_ls_low_power != -1) {
		retval +=
		    dwc_otg_set_param_host_support_fs_ls_low_power(core_if,
								   dwc_otg_module_params.
								   host_support_fs_ls_low_power);
	}
	if (dwc_otg_module_params.enable_dynamic_fifo != -1) {
		retval +=
		    dwc_otg_set_param_enable_dynamic_fifo(core_if,
							  dwc_otg_module_params.
							  enable_dynamic_fifo);
	}
	if (dwc_otg_module_params.data_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_data_fifo_size(core_if,
						     dwc_otg_module_params.
						     data_fifo_size);
	}
	if (dwc_otg_module_params.dev_rx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_dev_rx_fifo_size(core_if,
						       dwc_otg_module_params.
						       dev_rx_fifo_size);
	}
	if (dwc_otg_module_params.dev_nperio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_dev_nperio_tx_fifo_size(core_if,
							      dwc_otg_module_params.
							      dev_nperio_tx_fifo_size);
	}
	if (dwc_otg_module_params.host_rx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_rx_fifo_size(core_if,
							dwc_otg_module_params.host_rx_fifo_size);
	}
	if (dwc_otg_module_params.host_nperio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_nperio_tx_fifo_size(core_if,
							       dwc_otg_module_params.
							       host_nperio_tx_fifo_size);
	}
	if (dwc_otg_module_params.host_perio_tx_fifo_size != -1) {
		retval +=
		    dwc_otg_set_param_host_perio_tx_fifo_size(core_if,
							      dwc_otg_module_params.
							      host_perio_tx_fifo_size);
	}
	if (dwc_otg_module_params.max_transfer_size != -1) {
		retval +=
		    dwc_otg_set_param_max_transfer_size(core_if,
							dwc_otg_module_params.
							max_transfer_size);
	}
	if (dwc_otg_module_params.max_packet_count != -1) {
		retval +=
		    dwc_otg_set_param_max_packet_count(core_if,
						       dwc_otg_module_params.
						       max_packet_count);
	}
	if (dwc_otg_module_params.host_channels != -1) {
		retval +=
		    dwc_otg_set_param_host_channels(core_if,
						    dwc_otg_module_params.
						    host_channels);
	}
	if (dwc_otg_module_params.dev_endpoints != -1) {
		retval +=
		    dwc_otg_set_param_dev_endpoints(core_if,
						    dwc_otg_module_params.
						    dev_endpoints);
	}
	if (dwc_otg_module_params.phy_type != -1) {
		retval +=
		    dwc_otg_set_param_phy_type(core_if,
					       dwc_otg_module_params.phy_type);
	}
	if (dwc_otg_module_params.speed != -1) {
		retval +=
		    dwc_otg_set_param_speed(core_if,
					    dwc_otg_module_params.speed);
	}
	if (dwc_otg_module_params.host_ls_low_power_phy_clk != -1) {
		retval +=
		    dwc_otg_set_param_host_ls_low_power_phy_clk(core_if,
								dwc_otg_module_params.
								host_ls_low_power_phy_clk);
	}
	if (dwc_otg_module_params.phy_ulpi_ddr != -1) {
		retval +=
		    dwc_otg_set_param_phy_ulpi_ddr(core_if,
						   dwc_otg_module_params.
						   phy_ulpi_ddr);
	}
	if (dwc_otg_module_params.phy_ulpi_ext_vbus != -1) {
		retval +=
		    dwc_otg_set_param_phy_ulpi_ext_vbus(core_if,
							dwc_otg_module_params.
							phy_ulpi_ext_vbus);
	}
	if (dwc_otg_module_params.phy_utmi_width != -1) {
		retval +=
		    dwc_otg_set_param_phy_utmi_width(core_if,
						     dwc_otg_module_params.
						     phy_utmi_width);
	}
	if (dwc_otg_module_params.ulpi_fs_ls != -1) {
		retval +=
		    dwc_otg_set_param_ulpi_fs_ls(core_if,
						 dwc_otg_module_params.ulpi_fs_ls);
	}
	if (dwc_otg_module_params.ts_dline != -1) {
		retval +=
		    dwc_otg_set_param_ts_dline(core_if,
					       dwc_otg_module_params.ts_dline);
	}
	if (dwc_otg_module_params.i2c_enable != -1) {
		retval +=
		    dwc_otg_set_param_i2c_enable(core_if,
						 dwc_otg_module_params.
						 i2c_enable);
	}
	if (dwc_otg_module_params.en_multiple_tx_fifo != -1) {
		retval +=
		    dwc_otg_set_param_en_multiple_tx_fifo(core_if,
							  dwc_otg_module_params.
							  en_multiple_tx_fifo);
	}
	for (i = 0; i < 15; i++) {
		if (dwc_otg_module_params.dev_perio_tx_fifo_size[i] != -1) {
			retval +=
			    dwc_otg_set_param_dev_perio_tx_fifo_size(core_if,
								     dwc_otg_module_params.
								     dev_perio_tx_fifo_size
								     [i], i);
		}
	}

	for (i = 0; i < 15; i++) {
		if (dwc_otg_module_params.dev_tx_fifo_size[i] != -1) {
			retval += dwc_otg_set_param_dev_tx_fifo_size(core_if,
								     dwc_otg_module_params.
								     dev_tx_fifo_size
								     [i], i);
		}
	}
	if (dwc_otg_module_params.thr_ctl != -1) {
		retval +=
		    dwc_otg_set_param_thr_ctl(core_if,
					      dwc_otg_module_params.thr_ctl);
	}
	if (dwc_otg_module_params.mpi_enable != -1) {
		retval +=
		    dwc_otg_set_param_mpi_enable(core_if,
						 dwc_otg_module_params.
						 mpi_enable);
	}
	if (dwc_otg_module_params.pti_enable != -1) {
		retval +=
		    dwc_otg_set_param_pti_enable(core_if,
						 dwc_otg_module_params.
						 pti_enable);
	}
	if (dwc_otg_module_params.lpm_enable != -1) {
		retval +=
		    dwc_otg_set_param_lpm_enable(core_if,
						 dwc_otg_module_params.
						 lpm_enable);
	}
	if (dwc_otg_module_params.ic_usb_cap != -1) {
		retval +=
		    dwc_otg_set_param_ic_usb_cap(core_if,
						 dwc_otg_module_params.
						 ic_usb_cap);
	}
	if (dwc_otg_module_params.tx_thr_length != -1) {
		retval +=
		    dwc_otg_set_param_tx_thr_length(core_if,
						    dwc_otg_module_params.tx_thr_length);
	}
	if (dwc_otg_module_params.rx_thr_length != -1) {
		retval +=
		    dwc_otg_set_param_rx_thr_length(core_if,
						    dwc_otg_module_params.
						    rx_thr_length);
	}
	if (dwc_otg_module_params.ahb_thr_ratio != -1) {
		retval +=
		    dwc_otg_set_param_ahb_thr_ratio(core_if,
						    dwc_otg_module_params.ahb_thr_ratio);
	}
	if (dwc_otg_module_params.power_down != -1) {
		retval +=
		    dwc_otg_set_param_power_down(core_if,
						 dwc_otg_module_params.power_down);
	}
	if (dwc_otg_module_params.reload_ctl != -1) {
		retval +=
		    dwc_otg_set_param_reload_ctl(core_if,
						 dwc_otg_module_params.reload_ctl);
	}

	if (dwc_otg_module_params.dev_out_nak != -1) {
		retval +=
			dwc_otg_set_param_dev_out_nak(core_if,
			dwc_otg_module_params.dev_out_nak);
	}

	if (dwc_otg_module_params.cont_on_bna != -1) {
		retval +=
			dwc_otg_set_param_cont_on_bna(core_if,
			dwc_otg_module_params.cont_on_bna);
	}

	if (dwc_otg_module_params.ahb_single != -1) {
		retval +=
			dwc_otg_set_param_ahb_single(core_if,
			dwc_otg_module_params.ahb_single);
	}

	if (dwc_otg_module_params.otg_ver != -1) {
		retval +=
		    dwc_otg_set_param_otg_ver(core_if,
					      dwc_otg_module_params.otg_ver);
	}
	if (dwc_otg_module_params.adp_enable != -1) {
		retval +=
		    dwc_otg_set_param_adp_enable(core_if,
						 dwc_otg_module_params.
						 adp_enable);
	}
	return retval;
}

#if defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)
static void DRV_SynopSetupOtgParameters(dwc_otg_core_if_t * core_if)
{
    /* OTG Support Hibernation feature */
    dwc_otg_set_param_power_down(core_if, 2);
    /* TODO: Add Related Feature Here */
}

static void DRV_SynopSetupHsicParameters(dwc_otg_core_if_t * core_if)
{
    /* HSIC Not Support Hibernation feature */
    dwc_otg_set_param_power_down(core_if, 0);
    /* TODO: Add Related Feature Here */
}
#endif

/**
 * This function is the top level interrupt handler for the Common
 * (Device and host modes) interrupts.
 */
static irqreturn_t dwc_otg_common_irq(int irq, void *dev)
{
	int32_t retval = IRQ_NONE;

	retval = dwc_otg_handle_common_intr(dev);
	if (retval != 0) {
		S3C2410X_CLEAR_EINTPEND();
	}
	return IRQ_RETVAL(retval);
}

/**
 * This function is called when a lm_device is unregistered with the
 * dwc_otg_driver. This happens, for example, when the rmmod command is
 * executed. The device may or may not be electrically present. If it is
 * present, the driver stops device processing. Any resources used on behalf
 * of this device are freed.
 *
 * @param _dev
 */
static void dwc_otg_driver_remove(
#ifdef LM_INTERFACE
					 struct lm_device *_dev
#elif defined(PCI_INTERFACE)
					 struct pci_dev *_dev
#endif
    )
{
#ifdef LM_INTERFACE
	dwc_otg_device_t *otg_dev = lm_get_drvdata(_dev);
#elif defined(PCI_INTERFACE)
	dwc_otg_device_t *otg_dev = pci_get_drvdata(_dev);
#endif

	DWC_DEBUGPL(DBG_ANY, "%s(%p)\n", __func__, _dev);

	if (!otg_dev) {
		/* Memory allocation for the dwc_otg_device failed. */
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev NULL!\n", __func__);
		return;
	}

#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    if (DWC_SYNOP_CORE_OTG == _dev->id)
    {
        flush_work(&g_temper_check);
        cancel_work_sync(&g_temper_check);        
    }
#endif
    
#ifndef DWC_DEVICE_ONLY
	if (otg_dev->hcd) {
		hcd_remove(_dev);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->hcd NULL!\n", __func__);
		return;
	}
#endif

#ifndef DWC_HOST_ONLY
	if (otg_dev->pcd) {
		pcd_remove(_dev);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->pcd NULL!\n", __func__);
		return;
	}
#endif
	/*
	 * Free the IRQ
	 */
	if (otg_dev->common_irq_installed) {
		free_irq(_dev->irq, otg_dev);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: There is no installed irq!\n", __func__);
		return;
	}

	if (otg_dev->core_if) {
		dwc_otg_cil_remove(otg_dev->core_if);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->core_if NULL!\n", __func__);
		return;
	}

	/*
	 * Remove the device attributes
	 */
	dwc_otg_attr_remove(_dev);

	/*
	 * Return the memory.
	 */
	if (otg_dev->os_dep.base) {
		iounmap(otg_dev->os_dep.base);
	}
	DWC_FREE(otg_dev);

	/*
	 * Clear the drvdata pointer.
	 */
#ifdef LM_INTERFACE
	lm_set_drvdata(_dev, 0);
#elif defined(PCI_INTERFACE)
    release_mem_region(otg_dev->os_dep.rsrc_start, otg_dev->os_dep.rsrc_len);
	pci_set_drvdata(_dev, 0);
#endif
}

#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
/******************************************************************************
Function:       dwc_otg_vbus_switch
Description:    ���ݴ����dev_id��open�����رջ����Ӧ��VBUS��
                Ŀǰ��dev_id=USB_CTRL_ID_OTGʱ���Ƕ�VBUS�򿪻�رղ�����
Input:          @dev_id
                @open
Output:         NONE
Return:         NONE
Others:         NONE
******************************************************************************/
int dwc_otg_vbus_switch(uint32_t dev_id , uint8_t open)
{
    int status; 
    uint8_t id = 0;

    id = (uint8_t)(dev_id & USB_CTRL_ID_MASK);
    switch (id)
    {
        case USB_CTRL_ID_OTG:
        {
            DWC_PRINTF("OTG  vbus  open=%d \n", open);
            if (!open)
            {
                /*close Vbus*/
                gpio_direction_output(BALONG_GPIO_USB_VBUS, VBUS_CLOSE);
            }
            else
            {
                /*open Vbus*/
                status  = gpio_direction_output(BALONG_GPIO_USB_VBUS, VBUS_OPEN);
                printk("gpio output status = %d\n",status);
            }
            break;
        }

        case  USB_CTRL_ID_UICC:
        {
            DWC_PRINTF("UICC not used yet!\n");
            break;
        }

        default:
            DWC_PRINTF("Invalid dev_id=0x%x\n", dev_id);
            return 1;
    }

    return 0;    
}

/******************************************************************************
Function:       dwc_otg_wake_up_LCD
Description:    ����Ϣ��Ӧ�ó��򣬻��ѵ���LCD��
Input:          NONE
Output:         NONE
Return:         NONE
Others:         NONE
******************************************************************************/
void dwc_otg_wake_up_LCD()
{
    DEVICE_EVENT stusbEvent = {DEVICE_ID_USB, 0, 0, {0}};

    printk("wake up LCD, light LCD.\n");
    stusbEvent.device_id = DEVICE_ID_KEY;
    stusbEvent.event_code = (int)KEY_WAKEUP;
    stusbEvent.len = 0;  
    (void)device_event_report(&stusbEvent,sizeof(DEVICE_EVENT));
}

/******************************************************************************
Function:       dwc_otg_send_AF10_state_to_app
Description:    ��AF10��λ����Ϣ���͸�Ӧ�ò㣬
                Ӧ�ò��������AF10�Ƿ���λ�������Ƿ�ر��Զ��ػ�����
                AF10��λ����ֹ�Զ��ػ�����
                AF10����λ�������Զ��ػ�����
Input:          @AF10_state
                    AF10_STATE_UNPLUG   AF10����λ
                    AF10_STATE_PLUGIN   AF10��λ
Output:         NONE
Return:         NONE
Others:         NONE
******************************************************************************/
void dwc_otg_send_AF10_state_to_app(unsigned int AF10_state)
{
    DEVICE_EVENT stusbEvent = {DEVICE_ID_USB, 0, 0, {0}};

    printk("send AF10 state(0x%x) to application layer.\n", AF10_state);
    stusbEvent.device_id = DEVICE_ID_USB;
    stusbEvent.len = 0;  

    switch (AF10_state)
    {
        case AF10_STATE_PLUGIN:
            stusbEvent.event_code = (int)USB_AF10_ATTACH;
            break;

        case AF10_STATE_UNPLUG:
            stusbEvent.event_code = (int)USB_AF10_REMOVE;
            break;
#if 0
        case AF10_DEVICE_ATTACH:
          stusbEvent.device_id = DEVICE_ID_BATTERY;
          stusbEvent.event_code = (int)BAT_EXTCHARGING_ON_START;
          break;
          
        case AF10_DEVICE_REMOVE:
          stusbEvent.device_id = DEVICE_ID_BATTERY;
          stusbEvent.event_code = (int)BAT_EXTCHARGING_ON_STOP;          
          break;
#endif          
        default:
            printk("Incorrect AF10 state 0x%x!\n", AF10_state);
            return;
            break;
    }

    (void)device_event_report(&stusbEvent, sizeof(DEVICE_EVENT));
    return;
}

/******************************************************************************
Function:       dwc_otg_discharge_ctrl
Description:    �ṩ�����ģ��ʵ�ֹ��±���,�򿪻�رն�����Ĺ��ܡ�
Input:          @discharge_status
                    CTRL_DISCHARGE_ON���������繦��
                    CTRL_DISCHARGE_OFF�رն����ŵ繦��
Output:         NONE
Return:         NONE
Others:         NONE
******************************************************************************/
void dwc_otg_discharge_ctrl(CTRL_DISCHARGE_ENUM_LIST discharge_status)
{
    DWC_PRINTF("%s: discharge_status %d\n", __FUNCTION__, discharge_status);

    switch (discharge_status)
    {
        case CTRL_DISCHARGE_ON:
            dwc_otg_vbus_switch(USB_CTRL_ID_OTG, VBUS_OPEN);
            break;
        case CTRL_DISCHARGE_OFF:
            dwc_otg_vbus_switch(USB_CTRL_ID_OTG, VBUS_CLOSE);
            break;
        default:
            DWC_ERROR("unkown discharge_status %d\n", discharge_status);
            break;
    }

    return;
}

void dwc_otg_discharge_open()
{
    dwc_otg_discharge_ctrl(CTRL_DISCHARGE_ON);
}

void dwc_otg_discharge_close()
{
    dwc_otg_discharge_ctrl(CTRL_DISCHARGE_OFF);
}

static void extcharger_proc(struct work_struct *work)
{
    BSP_CHGC_ExtCharger_event_api();
}

/******************************************************************************
Function:       dwc_otg_set_AF10_state
Description:    ��AF10�β�ʱ���ô˺�������AF10��״̬�������״̬�ϱ���
                ��AF10��״̬��־д�빲���ڴ�
Input:          @AF10_state     AF10��״̬
Output:         NONE
Return:         NONE
Others:         NONE
******************************************************************************/
void dwc_otg_set_AF10_state(unsigned int AF10_state)
{
    /*������߲���AF10ʱ������LCD��Ļ*/
    dwc_otg_wake_up_LCD();

    /*������߲���AF10ʱ��֪ͨӦ�ò㣬��ʵ�ֲ���AF10��ֹ�Զ��ػ�����*/
    dwc_otg_send_AF10_state_to_app(AF10_state);

    /*��AF10�Ƿ���λ��־д�빲���ڴ�*/
    DWC_PRINTF("dwc otg set AF10 state 0x%x\n", AF10_state);
    *(volatile BSP_U32 *)(MEMORY_AXI_AF10_ADDR) = AF10_state;
    
    schedule_work (&g_temper_check);
}

/******************************************************************************
Function:       dwc_otg_get_AF10_state
Description:    ��ȡ�����ڴ���AF10״̬��־����ӡ
Input:          NONE
Output:         NONE
Return:         NONE
Others:         NONE
******************************************************************************/
BSP_U32 dwc_otg_get_AF10_state(void)
{
    BSP_U32 AF10_state;

    AF10_state = *(volatile BSP_U32 *)(MEMORY_AXI_AF10_ADDR);
    DWC_DEBUGPL(DBG_ANY, "dwc otg get AF10 state 0x%x\n", AF10_state);

    return AF10_state;
}
#endif

/**
 * This function is called when an lm_device is bound to a
 * dwc_otg_driver. It creates the driver components required to
 * control the device (CIL, HCD, and PCD) and it initializes the
 * device. The driver components are stored in a dwc_otg_device
 * structure. A reference to the dwc_otg_device is saved in the
 * lm_device. This allows the driver to access the dwc_otg_device
 * structure on subsequent calls to driver methods for this device.
 *
 * @param _dev Bus device
 */
static int dwc_otg_driver_probe(
#ifdef LM_INTERFACE
				       struct lm_device *_dev
#elif defined(PCI_INTERFACE)
				       struct pci_dev *_dev,
				       const struct pci_device_id *id
#endif
    )
{
	int retval = 0;
	dwc_otg_device_t *dwc_otg_device;

#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
  if (DWC_SYNOP_CORE_OTG == _dev->id )
  {



      INIT_WORK (&g_temper_check, extcharger_proc);
  }
#endif  /*#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))*/
	dev_dbg(&_dev->dev, "dwc_otg_driver_probe(%p) id %d\n", _dev,_dev->id);

#ifdef LM_INTERFACE
	dev_dbg(&_dev->dev, "start=0x%08x\n", (unsigned)_dev->resource.start);
#elif defined(PCI_INTERFACE)
	if (!id) {
		DWC_ERROR("Invalid pci_device_id %p", id);
		return -EINVAL;
	}

	if (!_dev || (pci_enable_device(_dev) < 0)) {
		DWC_ERROR("Invalid pci_device %p", _dev);
		return -ENODEV;
	}
	dev_dbg(&_dev->dev, "start=0x%08x\n", (unsigned)pci_resource_start(_dev,0));
	/* other stuff needed as well? */

#endif

	dwc_otg_device = DWC_ALLOC(sizeof(dwc_otg_device_t));

	if (!dwc_otg_device) {
		dev_err(&_dev->dev, "kmalloc of dwc_otg_device failed\n");
		return -ENOMEM;
	}
  
    #if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    if (DWC_SYNOP_CORE_HSIC == _dev->id )
      gdwc_otg_device = dwc_otg_device;//only usbed by hsic
    #endif
    
	memset(dwc_otg_device, 0, sizeof(*dwc_otg_device));
	dwc_otg_device->os_dep.reg_offset = 0xFFFFFFFF;

	/*
	 * Map the DWC_otg Core memory into virtual address space.
	 */
#ifdef LM_INTERFACE
    dwc_otg_device->os_dep.base = ioremap(_dev->resource.start, _dev->resource.end-_dev->resource.start+1);

    printk("start 0x%x, end 0x%x, id 0x%x, irq %d\n", 
        _dev->resource.start, _dev->resource.end, _dev->id, _dev->irq);
  
	if (!dwc_otg_device->os_dep.base) {
		dev_err(&_dev->dev, "ioremap() failed\n");
		DWC_FREE(dwc_otg_device);
		return -ENOMEM;
	}
	dev_dbg(&_dev->dev, "base=0x%08x\n",
		(unsigned)dwc_otg_device->os_dep.base);
#elif defined(PCI_INTERFACE)
	_dev->current_state = PCI_D0;
	_dev->dev.power.power_state = PMSG_ON;

	if (!_dev->irq) {
		DWC_ERROR("Found HC with no IRQ. Check BIOS/PCI %s setup!",
			  pci_name(_dev));
		iounmap(dwc_otg_device->os_dep.base);
		DWC_FREE(dwc_otg_device);
		return -ENODEV;
	}

	dwc_otg_device->os_dep.rsrc_start = pci_resource_start(_dev, 0);
	dwc_otg_device->os_dep.rsrc_len = pci_resource_len(_dev, 0);
	DWC_DEBUGPL(DBG_ANY, "PCI resource: start=%08x, len=%08x\n",
		    (unsigned)dwc_otg_device->os_dep.rsrc_start,
		    (unsigned)dwc_otg_device->os_dep.rsrc_len);
	if (!request_mem_region
	    (dwc_otg_device->os_dep.rsrc_start, dwc_otg_device->os_dep.rsrc_len,
	     "dwc_otg")) {
		dev_dbg(&_dev->dev, "error requesting memory\n");
		iounmap(dwc_otg_device->os_dep.base);
		DWC_FREE(dwc_otg_device);
		return -EFAULT;
	}

	dwc_otg_device->os_dep.base =
	    ioremap_nocache(dwc_otg_device->os_dep.rsrc_start,
			    dwc_otg_device->os_dep.rsrc_len);
	if (dwc_otg_device->os_dep.base == NULL) {
		dev_dbg(&_dev->dev, "error mapping memory\n");
		release_mem_region(dwc_otg_device->os_dep.rsrc_start,
				   dwc_otg_device->os_dep.rsrc_len);
		iounmap(dwc_otg_device->os_dep.base);
		DWC_FREE(dwc_otg_device);
		return -EFAULT;
	}
	dev_dbg(&_dev->dev, "base=0x%p (before adjust) \n",
		dwc_otg_device->os_dep.base);
	dwc_otg_device->os_dep.base = (char *)dwc_otg_device->os_dep.base;
	dev_dbg(&_dev->dev, "base=0x%p (after adjust) \n",
		dwc_otg_device->os_dep.base);
	dev_dbg(&_dev->dev, "%s: mapped PA 0x%x to VA 0x%p\n", __func__,
		(unsigned)dwc_otg_device->os_dep.rsrc_start,
		dwc_otg_device->os_dep.base);

	pci_set_master(_dev);
	pci_set_drvdata(_dev, dwc_otg_device);
#endif

	/*
	 * Initialize driver data to point to the global DWC_otg
	 * Device structure.
	 */
#ifdef LM_INTERFACE
	lm_set_drvdata(_dev, dwc_otg_device);
#endif
	dev_dbg(&_dev->dev, "dwc_otg_device=0x%p\n", dwc_otg_device);

	dwc_otg_device->core_if = dwc_otg_cil_init(dwc_otg_device->os_dep.base, _dev->id);
    
	if (!dwc_otg_device->core_if) {
		dev_err(&_dev->dev, "CIL initialization failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

#if defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)
    dwc_otg_device->core_if->synop_instance = _dev->id;
    if (_dev->id == DWC_SYNOP_CORE_OTG)
    {
        printk(KERN_ERR "++ Current is OTG Controller ++\r\n");
    }
    else if (_dev->id == DWC_SYNOP_CORE_HSIC)
    {
        printk(KERN_ERR "++ Current is HSIC Controller ++\r\n");
    }
#endif

	/*
	 * Attempt to ensure this device is really a DWC_otg Controller.
	 * Read and verify the SNPSID register contents. The value should be
	 * 0x45F42XXX, which corresponds to "OT2", as in "OTG version 2.XX".
	 */

	if ((dwc_otg_get_gsnpsid(dwc_otg_device->core_if) & 0xFFFFF000) !=
	    0x4F542000) {
		dev_err(&_dev->dev, "Bad value for SNPSID: 0x%08x\n",
			dwc_otg_get_gsnpsid(dwc_otg_device->core_if));
		retval = -EINVAL;
		goto fail;
	}

	/*
	 * Validate parameter values.
	 */
	if (set_parameters(dwc_otg_device->core_if)) {
		retval = -EINVAL;
		goto fail;
	}

#if defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)
    if (dwc_otg_device->core_if->synop_instance == DWC_SYNOP_CORE_OTG)
    {
        DRV_SynopSetupOtgParameters(dwc_otg_device->core_if);
    }

    if (dwc_otg_device->core_if->synop_instance == DWC_SYNOP_CORE_HSIC)
    {
        DRV_SynopSetupHsicParameters(dwc_otg_device->core_if);
    }
#endif

#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    if (DWC_SYNOP_CORE_OTG == _dev->id )
    {
        hw_dwc_otg_init();
    }
#endif  /*#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))*/

	/*
	 * Create Device Attributes in sysfs
	 */
	 if (DWC_SYNOP_CORE_HSIC == _dev->id)
	 {
	    dwc_otg_attr_create(_dev);
	 }
    
	/*
	 * Disable the global interrupt until all the interrupt
	 * handlers are installed.
	 */
	dwc_otg_disable_global_interrupts(dwc_otg_device->core_if);

	/*
	 * Install the interrupt handler for the common interrupts before
	 * enabling common interrupts in core_init below.
	 */
	DWC_DEBUGPL(DBG_CIL, "registering (common) handler for irq%d\n",
		    _dev->irq);
	retval = request_irq(_dev->irq, dwc_otg_common_irq,
			     IRQF_SHARED | IRQF_DISABLED | IRQ_LEVEL, "dwc_otg",
			     dwc_otg_device);
	if (retval) {
		DWC_ERROR("request of irq%d failed\n", _dev->irq);
		retval = -EBUSY;
		goto fail;
	} else {
		dwc_otg_device->common_irq_installed = 1;
	}

#ifdef LM_INTERFACE
	/*set_irq_type(_dev->irq, IRQT_LOW);*/
#endif
	/*
	 * Initialize the DWC_otg core.
	 */
	dwc_otg_core_init(dwc_otg_device->core_if);
		
#ifndef DWC_HOST_ONLY
	/*
	 * Initialize the PCD
	 */
	retval = pcd_init(_dev);
	if (retval != 0) {
		DWC_ERROR("pcd_init failed\n");
		dwc_otg_device->pcd = NULL;
		goto fail;
	}	
#endif
#ifndef DWC_DEVICE_ONLY
	/*
	 * Initialize the HCD
	 */
	retval = hcd_init(_dev);
	if (retval != 0) {
		DWC_ERROR("hcd_init failed\n");
		dwc_otg_device->hcd = NULL;
		goto fail;
	}
#endif
#if (FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
	/*
	 * Initialize the Jungo USBWare
	 */
	if (dwc_otg_device->core_if->synop_instance == DWC_SYNOP_CORE_OTG)
	{
        printk(KERN_ERR "++ Initialize jungo usbware ++\r\n");
	}
#endif
#ifdef PCI_INTERFACE
	pci_set_drvdata(_dev, dwc_otg_device);
	dwc_otg_device->os_dep.pcidev = _dev;
#endif

	/*
	 * Enable the global interrupt after all the interrupt
	 * handlers are installed if there is no ADP support else 
	 * perform initial actions required for Internal ADP logic.
	 */
	if (!dwc_otg_get_param_adp_enable(dwc_otg_device->core_if))	
		dwc_otg_enable_global_interrupts(dwc_otg_device->core_if);
	else
		dwc_otg_adp_start(dwc_otg_device->core_if, 
							dwc_otg_is_host_mode(dwc_otg_device->core_if));

#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
    if (dwc_otg_device->core_if->synop_instance == DWC_SYNOP_CORE_HSIC)
    {
        init_waitqueue_head(&hsic_disconnect_wait);
    }
#endif
#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    if (DWC_SYNOP_CORE_OTG == _dev->id)
    {
        if (dwc_otg_is_host_mode(dwc_otg_device->core_if))
        {
            hw_dwc_otg_host_init();
            
            dwc_otg_set_AF10_state(AF10_STATE_PLUGIN);

            if (dwc_otg_af10_device_attached(dwc_otg_device->core_if))
            {
                dwc_otg_set_AF10_state(AF10_DEVICE_ATTACH);    
            }

        }
        if (dwc_otg_is_device_mode(dwc_otg_device->core_if))
        {
            hw_dwc_otg_device_init();
            
            dwc_otg_set_AF10_state(AF10_STATE_UNPLUG);
        }
    }
#endif  /*#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))*/
	return 0;

fail:
	dwc_otg_driver_remove(_dev);
	return retval;
}

void BCM43239_WIFI_PWRCTRL_SAVE(void)
{
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	dwc_otg_core_if_t *core_if;
    dwc_otg_hcd_t *dwc_otg_hcd;
    dwc_irqflags_t flags;
    /* Get the otg Context */
    core_if = gdwc_otg_device->core_if;
    dwc_otg_hcd = (dwc_otg_hcd_t *)core_if->dwc_otg_hcd;
    /* Disable the otg interrupts */
    dwc_otg_disable_global_interrupts(core_if);
    /* Power Down the BCM43239-WIFI */
    BCM43239_WIFI_PowerDown();
    /* Spinlock */
    DWC_SPINLOCK_IRQSAVE(dwc_otg_hcd->lock, &flags);
    /* Disconnect the Synopsys host stack */
    cil_hcd_disconnect(core_if);
    /* Initialize the Off state */
    core_if->lx_state = DWC_OTG_L3;
    /* Stop the host stack */
    cil_hcd_stop(core_if);
    /* Spinunlock */
    DWC_SPINUNLOCK_IRQRESTORE(dwc_otg_hcd->lock, flags);
#if 0
    /* Reset the HSIC controller and phy */
    DRV_HSIC_ControllerPhyCutOff();
    /* Release the HSIC controller and phy */
    DRV_HSIC_ControllerPhyRelease();
    /* Initialize the Synopsys Core again */
	dwc_otg_core_init(core_if);
    /* Initialize the Synopsys Host again */
    dwc_otg_core_host_init(core_if);
    /* Enable the global interrupts */
	dwc_otg_enable_global_interrupts(core_if);
    /* Startup the Synopsys hcd stack */
	/* cil_hcd_start(core_if); */
#endif
    /* Wait the hub thread to complete disconnect event */
    wait_event(hsic_disconnect_wait, hsic_disconnect);
    /* hsic_disconnect won't be handled by interrupt context */
    hsic_disconnect = 0;
#endif
}

void BCM43239_WIFI_PWRCTRL_WAKEUP(void)
{
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	dwc_otg_core_if_t *core_if;
    /* Get the otg Context */
    core_if = gdwc_otg_device->core_if;
    /* Release the BCM43239-WIFI */
    BCM43239_WIFI_Release();
    /* Reset the HSIC controller and phy */
    DRV_HSIC_ControllerPhyCutOff();
    /* Release the HSIC controller and phy */
    DRV_HSIC_ControllerPhyRelease();
    /* Disable the otg interrupts */
    dwc_otg_disable_global_interrupts(core_if);
    /* Initialize the Synopsys Core again */
	dwc_otg_core_init(core_if);
    /* Initialize the Synopsys Host again */
    dwc_otg_core_host_init(core_if);
    /* Enable the global interrupts */
	dwc_otg_enable_global_interrupts(core_if);
    /* Startup the Synopsys hcd stack */
	/* cil_hcd_start(core_if); */
#endif
}

void DRV_SynopHsicReinit(dwc_otg_core_if_t * core_if)
{
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    /* Release the BCM43239-WIFI */
    BCM43239_WIFI_Release();
    /* Reset the HSIC controller and phy */
    DRV_HSIC_ControllerPhyCutOff();
    /* Release the HSIC controller and phy */
    DRV_HSIC_ControllerPhyRelease();
    /* Disable the otg interrupts */
    dwc_otg_disable_global_interrupts(core_if);
    /* Initialize the Synopsys Core again */
	dwc_otg_core_init(core_if);
    /* Initialize the Synopsys Host again */
    dwc_otg_core_host_init(core_if);
    /* Enable the global interrupts */
	dwc_otg_enable_global_interrupts(core_if);
    /* Startup the Synopsys hcd stack */
	/* cil_hcd_start(core_if); */
#endif
}

void BCM43239_WIFI_PWRCTRL_RESTORE(void)
{
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    /* Get the OTG Context */
	dwc_otg_core_if_t *core_if = gdwc_otg_device->core_if;
    dwc_otg_hcd_t *hcd = (dwc_otg_hcd_t *)core_if->dwc_otg_hcd;
    /* Reinitialize the hsic */
    DRV_SynopHsicReinit(core_if);
    /* Power On the BCM43239-WIFI */
    BCM43239_WIFI_PowerOn();
    /* Schedule the hsic-wifi connection timeout timer */
    printk("Schedule the timer handling hsic-wifi connection\n");
    DWC_TIMER_SCHEDULE(hcd->hsic_wifi_conn_timer, 1000 /* 1s */);
#endif
}

void BCM43239_WIFI_PWRCTRL_SUSPEND(void)
{
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    /* Get the otg Context */
	dwc_otg_core_if_t *core_if = gdwc_otg_device->core_if;
    /* Force Bus to Suspend Status */
    DRV_SynopHsicBusSuspend(core_if);
#endif
}

void BCM43239_WIFI_PWRCTRL_RESUME(void)
{
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    /* Get the otg Context */
	dwc_otg_core_if_t *core_if = gdwc_otg_device->core_if;
    /* Force Bus to Resume Status */
    DRV_SynopHsicBusResume(core_if);
#endif
}
/* Export the WIFI Power Ctrl Symbol */
EXPORT_SYMBOL(BCM43239_WIFI_PWRCTRL_SAVE);
EXPORT_SYMBOL(BCM43239_WIFI_PWRCTRL_WAKEUP);
EXPORT_SYMBOL(BCM43239_WIFI_PWRCTRL_RESTORE);
EXPORT_SYMBOL(BCM43239_WIFI_PWRCTRL_SUSPEND);
EXPORT_SYMBOL(BCM43239_WIFI_PWRCTRL_RESUME);
//begin added by duxj
#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
void hw_dwc_otg_record_core_if(dwc_otg_core_if_t *core_if)
{
    printk("%s: enter core_if 0x%p\n", __FUNCTION__, core_if);
    g_core_if = core_if;//only usbed by otg

    return 0;
}

dwc_otg_core_if_t  *hw_dwc_otg_get_core_if(void)
{
    printk("%s: enter core_if 0x%p\n", __FUNCTION__, g_core_if);
    return g_core_if;
}

int hw_dwc_otg_config_vbus_gpio(void)
{
    uint32_t *ios_ctrl_addr = (volatile uint32_t *)((uint32_t)(ioremap(0x90000988,4)));
    uint32_t *ios_ctrl_13 = (volatile uint32_t *)((uint32_t)(ioremap(0x90000834,4)));

    printk("ios_ctrl_addr 0x%p, ios_ctrl_13 0x%p\n", ios_ctrl_addr, ios_ctrl_13);
    printk("*ios_ctrl_addr 0x%x, *ios_ctrl_13 0x%x\n", *ios_ctrl_addr, *ios_ctrl_13);

    /*?????GPIO_0_27?utmiotg_drvvbus*/
	(*ios_ctrl_addr) &= ~(1<<0);

	if (gpio_request(BALONG_GPIO_USB_VBUS, GPIO_VBUS_NAME))
	{	
		printk("%s: request gpio[%d] is busy!", __FUNCTION__, BALONG_GPIO_USB_VBUS);
		return -1;
	}

	printk("%s: config gpio[%d] suceed.", __FUNCTION__, BALONG_GPIO_USB_VBUS);

    return 0;
}

int hw_dwc_otg_init(void)
{    
    dwc_otg_core_if_t *core_if = hw_dwc_otg_get_core_if();

    printk("%s: enter\n", __FUNCTION__);
    hw_dwc_otg_config_vbus_gpio();
    
    core_if->lx_state = DWC_OTG_L0;
}

int hw_dwc_otg_device_init(void)
{
    printk("%s: enter\n", __FUNCTION__);

    gpio_direction_output(BALONG_GPIO_USB_VBUS, VBUS_CLOSE);
    //BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_USB);

    //hw_dwc_otg_lowpwr_enter();

    return 0;
}

int hw_dwc_otg_host_init(void)
{
    printk("%s: enter\n", __FUNCTION__);

    if (SHORT_POWER_OFF_MODE_CLR == 1)
    {
         BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_USB);        
    }
    
      
    if ( SHORT_POWER_OFF_MODE_SET == PWRCTRL_GetShortOffMode())
	    gpio_direction_output(BALONG_GPIO_USB_VBUS, VBUS_CLOSE);
    else
    	gpio_direction_output(BALONG_GPIO_USB_VBUS, VBUS_OPEN);

    return 0;
}

void hw_dwc_otg_lowpwr_exit(void)
{
	gpwrdn_data_t gpwrdn = {.d32 = 0 };
	gpwrdn_data_t gpwrdn_temp = {.d32 = 0 };
	dwc_otg_core_if_t *core_if = hw_dwc_otg_get_core_if();

    printk("%s: enter\n", __FUNCTION__);

	DWC_DEBUGPL(DBG_ANY, "%s called\n", __FUNCTION__);
	gpwrdn_temp.d32 = DWC_READ_REG32(&core_if->core_global_regs->gpwrdn);
	if (core_if->power_down == 2)
	{		
		if (!core_if->hibernation_suspend) {
			DWC_PRINTF("Already exited from Hibernation\n");
			return;
		}
		DWC_DEBUGPL(DBG_ANY, "Exit from hibernation on ID sts change\n");
		/* Switch on the voltage to the core */
		gpwrdn.b.pwrdnswtch = 1;
		DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, gpwrdn.d32, 0);
		dwc_udelay(10);

		/* Reset the core */
		gpwrdn.d32 = 0;
		gpwrdn.b.pwrdnrstn = 1;
		DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, gpwrdn.d32, 0);
		dwc_udelay(10);

		/* Disable power clamps */
		gpwrdn.d32 = 0;
		gpwrdn.b.pwrdnclmp = 1;
		DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, gpwrdn.d32, 0);

		/* Remove reset the core signal */
		gpwrdn.d32 = 0;
		gpwrdn.b.pwrdnrstn = 1;
		DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, 0, gpwrdn.d32);
		dwc_udelay(10);

		/* Disable PMU interrupt */
		gpwrdn.d32 = 0;
		gpwrdn.b.pmuintsel = 1;
		DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, gpwrdn.d32, 0);

		/*Indicates that we are exiting from hibernation */
		core_if->hibernation_suspend = 0;

		/* Disable PMU */
		gpwrdn.d32 = 0;
		gpwrdn.b.pmuactv = 1;
		DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, gpwrdn.d32, 0);
		dwc_udelay(10);

		gpwrdn.d32 = core_if->gr_backup->gpwrdn_local;
		if (gpwrdn.b.dis_vbus == 1) {
			gpwrdn.d32 = 0;
			gpwrdn.b.dis_vbus = 1;
			DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, gpwrdn.d32, 0);
		}
	}

    core_if->lx_state = DWC_OTG_L0;

    printk("%s: Huawei dwc otg Hibernation recovery completes here\n", __FUNCTION__);

	return;
}

void hw_dwc_otg_lowpwr_enter(void)
{
    int pending;
    dsts_data_t dsts;
    gintsts_data_t gintsts;
    dcfg_data_t dcfg;
    dwc_otg_core_if_t *core_if = hw_dwc_otg_get_core_if();

    if (NULL == core_if)
    {
        printk("%s: DWC OTG enter low power failed, core_if is NULL\n", __FUNCTION__);
        return;
    }

    /*������������л���δ��������󣬲�����͹���״̬*/
    pending = DWC_WORKQ_PENDING(core_if->wq_otg);
    if (pending > 1)
    {
        printk("%s: wq_otg is pending[%d], dont enter low power state!\n", 
            __FUNCTION__, pending);
        return;
    }

    printk("%s: Start enter low power state.\n", __FUNCTION__);
    if (dwc_otg_is_device_mode(core_if))
    {
        pcgcctl_data_t pcgcctl;
        gpwrdn_data_t gpwrdn;
        gusbcfg_data_t gusbcfg;

        pcgcctl.d32 = 0;
        gpwrdn.d32 = 0;
        gusbcfg.d32 = 0;

        /* Change to L2(suspend) state */
        core_if->lx_state = DWC_OTG_L2;

        /* Clear interrupt in gintsts */
        gintsts.d32 = 0;
        gintsts.b.usbsuspend = 1;
        DWC_WRITE_REG32(&core_if->core_global_regs->gintsts, 
            gintsts.d32);
        DWC_PRINTF("Start of hibernation completed\n");
        dwc_otg_save_global_regs(core_if);
        dwc_otg_save_dev_regs(core_if);

        gusbcfg.d32 =
            DWC_READ_REG32(&core_if->core_global_regs->gusbcfg);
        if (gusbcfg.b.ulpi_utmi_sel == 1) {
            /* ULPI interface */
            /* Suspend the Phy Clock */
            pcgcctl.d32 = 0;
            pcgcctl.b.stoppclk = 1;
            DWC_MODIFY_REG32(core_if->pcgcctl, 0,
                     pcgcctl.d32);
            dwc_udelay(10);
            gpwrdn.b.pmuactv = 1;
            DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, 
                0, gpwrdn.d32);
        } else {
            /* UTMI+ Interface */
            gpwrdn.b.pmuactv = 1;
            DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, 
                0, gpwrdn.d32);
            dwc_udelay(10);
            pcgcctl.b.stoppclk = 1;
            DWC_MODIFY_REG32(core_if->pcgcctl, 0,
                     pcgcctl.d32);
            dwc_udelay(10);
        }

        /* Set flag to indicate that we are in hibernation */
        core_if->hibernation_suspend = 1;
        DWC_DEBUGPL(DBG_ANY, "core_if->hibernation_suspend = 1\n");
        /* Enable interrupts from wake up logic */
        gpwrdn.d32 = 0;
        gpwrdn.b.pmuintsel = 1;
        DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, 0, gpwrdn.d32);
        dwc_udelay(10);

        /* Unmask device mode interrupts in GPWRDN */
        gpwrdn.d32 = 0;
        gpwrdn.b.rst_det_msk = 1;
        gpwrdn.b.lnstchng_msk = 1;
        gpwrdn.b.sts_chngint_msk = 1;
        DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, 0, gpwrdn.d32);
        dwc_udelay(10);

        /* Enable Power Down Clamp */
        gpwrdn.d32 = 0;
        gpwrdn.b.pwrdnclmp = 1;
        DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, 0, gpwrdn.d32);
        dwc_udelay(10);

        /* Switch off VDD */
        gpwrdn.d32 = 0;
        gpwrdn.b.pwrdnswtch = 1;
        DWC_MODIFY_REG32(&core_if->core_global_regs->gpwrdn, 0, gpwrdn.d32);

        /* Save gpwrdn register for further usage if stschng interrupt */
        core_if->gr_backup->gpwrdn_local =
                    DWC_READ_REG32(&core_if->core_global_regs->gpwrdn);
        DWC_PRINTF("Hibernation completed\n");
        //zhanyaotian add
        BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_USB);
        printk("%s: End enter low power state.\n", __FUNCTION__);
    }
    else
    {
        printk("%s: Not in device mode, dont enter low power state!\n", 
            __FUNCTION__);
    }
}

int32_t hw_dwc_otg_is_device_mode(void)
{
    dwc_otg_core_if_t  *core_if = hw_dwc_otg_get_core_if();

    printk("%s: enter, g_core_if 0x%p\n", __FUNCTION__, g_core_if);

    if (!core_if)
    {
        printk("%s: core_if is NULL!\n", __FUNCTION__);
        return 0;
    }

    return dwc_otg_is_device_mode(core_if);
}

void my_dump_reg()
{
    dwc_otg_core_if_t  *core_if = hw_dwc_otg_get_core_if();

    printk("%s: enter, g_core_if 0x%p\n", __FUNCTION__, g_core_if);

    if (!core_if)
    {
        printk("%s: core_if is NULL!\n", __FUNCTION__);
        return;
    }

    dwc_otg_dump_global_registers(core_if);
}
#endif  /*#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))*/
//end added by duxj
/**
 * This structure defines the methods to be called by a bus driver
 * during the lifecycle of a device on that bus. Both drivers and
 * devices are registered with a bus driver. The bus driver matches
 * devices to drivers based on information in the device and driver
 * structures.
 *
 * The probe function is called when the bus driver matches a device
 * to this driver. The remove function is called when a device is
 * unregistered with the bus driver.
 */
#ifdef LM_INTERFACE
static struct lm_driver dwc_otg_driver = {
	.drv = {.name = (char *)dwc_driver_name,},
	.probe = dwc_otg_driver_probe,
	.remove = dwc_otg_driver_remove,
};
#elif defined(PCI_INTERFACE)
static const struct pci_device_id pci_ids[] = { {
						 PCI_DEVICE(0x16c3, 0xabcd),
						 .driver_data =
						 (unsigned long)0xdeadbeef,
						 }, { /* end: all zeroes */ }
};

MODULE_DEVICE_TABLE(pci, pci_ids);

/* pci driver glue; this is a "new style" PCI driver module */
static struct pci_driver dwc_otg_driver = {
	.name = "dwc_otg",
	.id_table = pci_ids,

	.probe = dwc_otg_driver_probe,
	.remove = dwc_otg_driver_remove,

	.driver = {
		   .name = (char *)dwc_driver_name,
		   },
};
#endif

/**
 * This function is called when the dwc_otg_driver is installed with the
 * insmod command. It registers the dwc_otg_driver structure with the
 * appropriate bus driver. This will cause the dwc_otg_driver_probe function
 * to be called. In addition, the bus driver will automatically expose
 * attributes defined for the device and driver in the special sysfs file
 * system.
 *
 * @return
 */
static int __init dwc_otg_driver_init(void)
{
	int retval = 0;
	int error;
	printk(KERN_INFO "%s: version %s\n", dwc_driver_name,
	       DWC_DRIVER_VERSION);
#ifdef LM_INTERFACE
	retval = lm_driver_register(&dwc_otg_driver);
#elif defined(PCI_INTERFACE)
	retval = pci_register_driver(&dwc_otg_driver);
#endif
	if (retval < 0) {
		printk(KERN_ERR "%s retval=%d\n", __func__, retval);
		return retval;
	}
#ifdef LM_INTERFACE
	error = driver_create_file(&dwc_otg_driver.drv, &driver_attr_version);
	error = driver_create_file(&dwc_otg_driver.drv, &driver_attr_debuglevel);
#elif defined(PCI_INTERFACE)
	error = driver_create_file(&dwc_otg_driver.driver, &driver_attr_version);
	error = driver_create_file(&dwc_otg_driver.driver, &driver_attr_debuglevel);
#endif
	return retval;
}

module_init(dwc_otg_driver_init);

/**
 * This function is called when the driver is removed from the kernel
 * with the rmmod command. The driver unregisters itself with its bus
 * driver.
 *
 */
static void __exit dwc_otg_driver_cleanup(void)
{
	printk(KERN_DEBUG "dwc_otg_driver_cleanup()\n");

#ifdef LM_INTERFACE
	driver_remove_file(&dwc_otg_driver.drv, &driver_attr_debuglevel);
	driver_remove_file(&dwc_otg_driver.drv, &driver_attr_version);
	lm_driver_unregister(&dwc_otg_driver);
#elif defined(PCI_INTERFACE)
	driver_remove_file(&dwc_otg_driver.driver, &driver_attr_debuglevel);
	driver_remove_file(&dwc_otg_driver.driver, &driver_attr_version);
	pci_unregister_driver(&dwc_otg_driver);
#endif

	printk(KERN_INFO "%s module removed\n", dwc_driver_name);
}

module_exit(dwc_otg_driver_cleanup);

MODULE_DESCRIPTION(DWC_DRIVER_DESC);
MODULE_AUTHOR("Synopsys Inc.");
MODULE_LICENSE("GPL");

module_param_named(otg_cap, dwc_otg_module_params.otg_cap, int, 0444);
MODULE_PARM_DESC(otg_cap, "OTG Capabilities 0=HNP&SRP 1=SRP Only 2=None");
module_param_named(opt, dwc_otg_module_params.opt, int, 0444);
MODULE_PARM_DESC(opt, "OPT Mode");
module_param_named(dma_enable, dwc_otg_module_params.dma_enable, int, 0444);
MODULE_PARM_DESC(dma_enable, "DMA Mode 0=Slave 1=DMA enabled");

module_param_named(dma_desc_enable, dwc_otg_module_params.dma_desc_enable, int,
		   0444);
MODULE_PARM_DESC(dma_desc_enable,
		 "DMA Desc Mode 0=Address DMA 1=DMA Descriptor enabled");

module_param_named(dma_burst_size, dwc_otg_module_params.dma_burst_size, int,
		   0444);
MODULE_PARM_DESC(dma_burst_size,
		 "DMA Burst Size 1, 4, 8, 16, 32, 64, 128, 256");
module_param_named(speed, dwc_otg_module_params.speed, int, 0444);
MODULE_PARM_DESC(speed, "Speed 0=High Speed 1=Full Speed");
module_param_named(host_support_fs_ls_low_power,
		   dwc_otg_module_params.host_support_fs_ls_low_power, int,
		   0444);
MODULE_PARM_DESC(host_support_fs_ls_low_power,
		 "Support Low Power w/FS or LS 0=Support 1=Don't Support");
module_param_named(host_ls_low_power_phy_clk,
		   dwc_otg_module_params.host_ls_low_power_phy_clk, int, 0444);
MODULE_PARM_DESC(host_ls_low_power_phy_clk,
		 "Low Speed Low Power Clock 0=48Mhz 1=6Mhz");
module_param_named(enable_dynamic_fifo,
		   dwc_otg_module_params.enable_dynamic_fifo, int, 0444);
MODULE_PARM_DESC(enable_dynamic_fifo, "0=cC Setting 1=Allow Dynamic Sizing");
module_param_named(data_fifo_size, dwc_otg_module_params.data_fifo_size, int,
		   0444);
MODULE_PARM_DESC(data_fifo_size,
		 "Total number of words in the data FIFO memory 32-32768");
module_param_named(dev_rx_fifo_size, dwc_otg_module_params.dev_rx_fifo_size,
		   int, 0444);
MODULE_PARM_DESC(dev_rx_fifo_size, "Number of words in the Rx FIFO 16-32768");
module_param_named(dev_nperio_tx_fifo_size,
		   dwc_otg_module_params.dev_nperio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(dev_nperio_tx_fifo_size,
		 "Number of words in the non-periodic Tx FIFO 16-32768");
module_param_named(dev_perio_tx_fifo_size_1,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[0], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_1,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_2,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[1], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_2,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_3,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[2], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_3,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_4,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[3], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_4,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_5,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[4], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_5,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_6,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[5], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_6,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_7,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[6], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_7,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_8,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[7], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_8,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_9,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[8], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_9,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_10,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[9], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_10,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_11,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[10], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_11,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_12,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[11], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_12,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_13,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[12], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_13,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_14,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[13], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_14,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_15,
		   dwc_otg_module_params.dev_perio_tx_fifo_size[14], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_15,
		 "Number of words in the periodic Tx FIFO 4-768");
module_param_named(host_rx_fifo_size, dwc_otg_module_params.host_rx_fifo_size,
		   int, 0444);
MODULE_PARM_DESC(host_rx_fifo_size, "Number of words in the Rx FIFO 16-32768");
module_param_named(host_nperio_tx_fifo_size,
		   dwc_otg_module_params.host_nperio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_nperio_tx_fifo_size,
		 "Number of words in the non-periodic Tx FIFO 16-32768");
module_param_named(host_perio_tx_fifo_size,
		   dwc_otg_module_params.host_perio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_perio_tx_fifo_size,
		 "Number of words in the host periodic Tx FIFO 16-32768");
module_param_named(max_transfer_size, dwc_otg_module_params.max_transfer_size,
		   int, 0444);
/** @todo Set the max to 512K, modify checks */
MODULE_PARM_DESC(max_transfer_size,
		 "The maximum transfer size supported in bytes 2047-65535");
module_param_named(max_packet_count, dwc_otg_module_params.max_packet_count,
		   int, 0444);
MODULE_PARM_DESC(max_packet_count,
		 "The maximum number of packets in a transfer 15-511");
module_param_named(host_channels, dwc_otg_module_params.host_channels, int,
		   0444);
MODULE_PARM_DESC(host_channels,
		 "The number of host channel registers to use 1-16");
module_param_named(dev_endpoints, dwc_otg_module_params.dev_endpoints, int,
		   0444);
MODULE_PARM_DESC(dev_endpoints,
		 "The number of endpoints in addition to EP0 available for device mode 1-15");
module_param_named(phy_type, dwc_otg_module_params.phy_type, int, 0444);
MODULE_PARM_DESC(phy_type, "0=Reserved 1=UTMI+ 2=ULPI");
module_param_named(phy_utmi_width, dwc_otg_module_params.phy_utmi_width, int,
		   0444);
MODULE_PARM_DESC(phy_utmi_width, "Specifies the UTMI+ Data Width 8 or 16 bits");
module_param_named(phy_ulpi_ddr, dwc_otg_module_params.phy_ulpi_ddr, int, 0444);
MODULE_PARM_DESC(phy_ulpi_ddr,
		 "ULPI at double or single data rate 0=Single 1=Double");
module_param_named(phy_ulpi_ext_vbus, dwc_otg_module_params.phy_ulpi_ext_vbus,
		   int, 0444);
MODULE_PARM_DESC(phy_ulpi_ext_vbus,
		 "ULPI PHY using internal or external vbus 0=Internal");
module_param_named(i2c_enable, dwc_otg_module_params.i2c_enable, int, 0444);
MODULE_PARM_DESC(i2c_enable, "FS PHY Interface");
module_param_named(ulpi_fs_ls, dwc_otg_module_params.ulpi_fs_ls, int, 0444);
MODULE_PARM_DESC(ulpi_fs_ls, "ULPI PHY FS/LS mode only");
module_param_named(ts_dline, dwc_otg_module_params.ts_dline, int, 0444);
MODULE_PARM_DESC(ts_dline, "Term select Dline pulsing for all PHYs");
module_param_named(debug, g_dbg_lvl, int, 0444);
MODULE_PARM_DESC(debug, "");

module_param_named(en_multiple_tx_fifo,
		   dwc_otg_module_params.en_multiple_tx_fifo, int, 0444);
MODULE_PARM_DESC(en_multiple_tx_fifo,
		 "Dedicated Non Periodic Tx FIFOs 0=disabled 1=enabled");
module_param_named(dev_tx_fifo_size_1,
		   dwc_otg_module_params.dev_tx_fifo_size[0], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_1, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_2,
		   dwc_otg_module_params.dev_tx_fifo_size[1], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_2, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_3,
		   dwc_otg_module_params.dev_tx_fifo_size[2], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_3, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_4,
		   dwc_otg_module_params.dev_tx_fifo_size[3], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_4, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_5,
		   dwc_otg_module_params.dev_tx_fifo_size[4], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_5, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_6,
		   dwc_otg_module_params.dev_tx_fifo_size[5], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_6, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_7,
		   dwc_otg_module_params.dev_tx_fifo_size[6], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_7, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_8,
		   dwc_otg_module_params.dev_tx_fifo_size[7], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_8, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_9,
		   dwc_otg_module_params.dev_tx_fifo_size[8], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_9, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_10,
		   dwc_otg_module_params.dev_tx_fifo_size[9], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_10, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_11,
		   dwc_otg_module_params.dev_tx_fifo_size[10], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_11, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_12,
		   dwc_otg_module_params.dev_tx_fifo_size[11], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_12, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_13,
		   dwc_otg_module_params.dev_tx_fifo_size[12], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_13, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_14,
		   dwc_otg_module_params.dev_tx_fifo_size[13], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_14, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_15,
		   dwc_otg_module_params.dev_tx_fifo_size[14], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_15, "Number of words in the Tx FIFO 4-768");

module_param_named(thr_ctl, dwc_otg_module_params.thr_ctl, int, 0444);
MODULE_PARM_DESC(thr_ctl,
		 "Thresholding enable flag bit 0 - non ISO Tx thr., 1 - ISO Tx thr., 2 - Rx thr.- bit 0=disabled 1=enabled");
module_param_named(tx_thr_length, dwc_otg_module_params.tx_thr_length, int,
		   0444);
MODULE_PARM_DESC(tx_thr_length, "Tx Threshold length in 32 bit DWORDs");
module_param_named(rx_thr_length, dwc_otg_module_params.rx_thr_length, int,
		   0444);
MODULE_PARM_DESC(rx_thr_length, "Rx Threshold length in 32 bit DWORDs");

module_param_named(pti_enable, dwc_otg_module_params.pti_enable, int, 0444);
module_param_named(mpi_enable, dwc_otg_module_params.mpi_enable, int, 0444);
module_param_named(lpm_enable, dwc_otg_module_params.lpm_enable, int, 0444);
MODULE_PARM_DESC(lpm_enable, "LPM Enable 0=LPM Disabled 1=LPM Enabled");
module_param_named(ic_usb_cap, dwc_otg_module_params.ic_usb_cap, int, 0444);
MODULE_PARM_DESC(ic_usb_cap,
		 "IC_USB Capability 0=IC_USB Disabled 1=IC_USB Enabled");
module_param_named(ahb_thr_ratio, dwc_otg_module_params.ahb_thr_ratio, int,
		   0444);
MODULE_PARM_DESC(ahb_thr_ratio, "AHB Threshold Ratio");
module_param_named(power_down, dwc_otg_module_params.power_down, int, 0444);
MODULE_PARM_DESC(power_down, "Power Down Mode");
module_param_named(reload_ctl, dwc_otg_module_params.reload_ctl, int, 0444);
MODULE_PARM_DESC(reload_ctl, "HFIR Reload Control");
module_param_named(dev_out_nak, dwc_otg_module_params.dev_out_nak, int, 0444);
MODULE_PARM_DESC(dev_out_nak, "Enable Device OUT NAK");
module_param_named(cont_on_bna, dwc_otg_module_params.cont_on_bna, int, 0444);
MODULE_PARM_DESC(cont_on_bna, "Enable Enable Continue on BNA");
module_param_named(ahb_single, dwc_otg_module_params.ahb_single, int, 0444);
MODULE_PARM_DESC(ahb_single, "Enable AHB Single Support");
module_param_named(adp_enable, dwc_otg_module_params.adp_enable, int, 0444);
MODULE_PARM_DESC(adp_enable, "ADP Enable 0=ADP Disabled 1=ADP Enabled");
module_param_named(otg_ver, dwc_otg_module_params.otg_ver, int, 0444);
MODULE_PARM_DESC(otg_ver, "OTG revision supported 0=OTG 1.3 1=OTG 2.0");

/** @page "Module Parameters"
 *
 * The following parameters may be specified when starting the module.
 * These parameters define how the DWC_otg controller should be
 * configured. Parameter values are passed to the CIL initialization
 * function dwc_otg_cil_init
 *
 * Example: <code>modprobe dwc_otg speed=1 otg_cap=1</code>
 *

 <table>
 <tr><td>Parameter Name</td><td>Meaning</td></tr>

 <tr>
 <td>otg_cap</td>
 <td>Specifies the OTG capabilities. The driver will automatically detect the
 value for this parameter if none is specified.
 - 0: HNP and SRP capable (default, if available)
 - 1: SRP Only capable
 - 2: No HNP/SRP capable
 </td></tr>

 <tr>
 <td>dma_enable</td>
 <td>Specifies whether to use slave or DMA mode for accessing the data FIFOs.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Slave
 - 1: DMA (default, if available)
 </td></tr>

 <tr>
 <td>dma_burst_size</td>
 <td>The DMA Burst size (applicable only for External DMA Mode).
 - Values: 1, 4, 8 16, 32, 64, 128, 256 (default 32)
 </td></tr>

 <tr>
 <td>speed</td>
 <td>Specifies the maximum speed of operation in host and device mode. The
 actual speed depends on the speed of the attached device and the value of
 phy_type.
 - 0: High Speed (default)
 - 1: Full Speed
 </td></tr>

 <tr>
 <td>host_support_fs_ls_low_power</td>
 <td>Specifies whether low power mode is supported when attached to a Full
 Speed or Low Speed device in host mode.
 - 0: Don't support low power mode (default)
 - 1: Support low power mode
 </td></tr>

 <tr>
 <td>host_ls_low_power_phy_clk</td>
 <td>Specifies the PHY clock rate in low power mode when connected to a Low
 Speed device in host mode. This parameter is applicable only if
 HOST_SUPPORT_FS_LS_LOW_POWER is enabled.
 - 0: 48 MHz (default)
 - 1: 6 MHz
 </td></tr>

 <tr>
 <td>enable_dynamic_fifo</td>
 <td> Specifies whether FIFOs may be resized by the driver software.
 - 0: Use cC FIFO size parameters
 - 1: Allow dynamic FIFO sizing (default)
 </td></tr>

 <tr>
 <td>data_fifo_size</td>
 <td>Total number of 4-byte words in the data FIFO memory. This memory
 includes the Rx FIFO, non-periodic Tx FIFO, and periodic Tx FIFOs.
 - Values: 32 to 32768 (default 8192)

 Note: The total FIFO memory depth in the FPGA configuration is 8192.
 </td></tr>

 <tr>
 <td>dev_rx_fifo_size</td>
 <td>Number of 4-byte words in the Rx FIFO in device mode when dynamic
 FIFO sizing is enabled.
 - Values: 16 to 32768 (default 1064)
 </td></tr>

 <tr>
 <td>dev_nperio_tx_fifo_size</td>
 <td>Number of 4-byte words in the non-periodic Tx FIFO in device mode when
 dynamic FIFO sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>dev_perio_tx_fifo_size_n (n = 1 to 15)</td>
 <td>Number of 4-byte words in each of the periodic Tx FIFOs in device mode
 when dynamic FIFO sizing is enabled.
 - Values: 4 to 768 (default 256)
 </td></tr>

 <tr>
 <td>host_rx_fifo_size</td>
 <td>Number of 4-byte words in the Rx FIFO in host mode when dynamic FIFO
 sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>host_nperio_tx_fifo_size</td>
 <td>Number of 4-byte words in the non-periodic Tx FIFO in host mode when
 dynamic FIFO sizing is enabled in the core.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>host_perio_tx_fifo_size</td>
 <td>Number of 4-byte words in the host periodic Tx FIFO when dynamic FIFO
 sizing is enabled.
 - Values: 16 to 32768 (default 1024)
 </td></tr>

 <tr>
 <td>max_transfer_size</td>
 <td>The maximum transfer size supported in bytes.
 - Values: 2047 to 65,535 (default 65,535)
 </td></tr>

 <tr>
 <td>max_packet_count</td>
 <td>The maximum number of packets in a transfer.
 - Values: 15 to 511 (default 511)
 </td></tr>

 <tr>
 <td>host_channels</td>
 <td>The number of host channel registers to use.
 - Values: 1 to 16 (default 12)

 Note: The FPGA configuration supports a maximum of 12 host channels.
 </td></tr>

 <tr>
 <td>dev_endpoints</td>
 <td>The number of endpoints in addition to EP0 available for device mode
 operations.
 - Values: 1 to 15 (default 6 IN and OUT)

 Note: The FPGA configuration supports a maximum of 6 IN and OUT endpoints in
 addition to EP0.
 </td></tr>

 <tr>
 <td>phy_type</td>
 <td>Specifies the type of PHY interface to use. By default, the driver will
 automatically detect the phy_type.
 - 0: Full Speed
 - 1: UTMI+ (default, if available)
 - 2: ULPI
 </td></tr>

 <tr>
 <td>phy_utmi_width</td>
 <td>Specifies the UTMI+ Data Width. This parameter is applicable for a
 phy_type of UTMI+. Also, this parameter is applicable only if the
 OTG_HSPHY_WIDTH cC parameter was set to "8 and 16 bits", meaning that the
 core has been configured to work at either data path width.
 - Values: 8 or 16 bits (default 16)
 </td></tr>

 <tr>
 <td>phy_ulpi_ddr</td>
 <td>Specifies whether the ULPI operates at double or single data rate. This
 parameter is only applicable if phy_type is ULPI.
 - 0: single data rate ULPI interface with 8 bit wide data bus (default)
 - 1: double data rate ULPI interface with 4 bit wide data bus
 </td></tr>

 <tr>
 <td>i2c_enable</td>
 <td>Specifies whether to use the I2C interface for full speed PHY. This
 parameter is only applicable if PHY_TYPE is FS.
 - 0: Disabled (default)
 - 1: Enabled
 </td></tr>

 <tr>
 <td>ulpi_fs_ls</td>
 <td>Specifies whether to use ULPI FS/LS mode only.
 - 0: Disabled (default)
 - 1: Enabled
 </td></tr>

 <tr>
 <td>ts_dline</td>
 <td>Specifies whether term select D-Line pulsing for all PHYs is enabled.
 - 0: Disabled (default)
 - 1: Enabled
 </td></tr>
 
 <tr>
 <td>en_multiple_tx_fifo</td>
 <td>Specifies whether dedicatedto tx fifos are enabled for non periodic IN EPs.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Disabled
 - 1: Enabled (default, if available)
 </td></tr>

 <tr>
 <td>dev_tx_fifo_size_n (n = 1 to 15)</td>
 <td>Number of 4-byte words in each of the Tx FIFOs in device mode
 when dynamic FIFO sizing is enabled.
 - Values: 4 to 768 (default 256)
 </td></tr>

 <tr>
 <td>tx_thr_length</td>
 <td>Transmit Threshold length in 32 bit double words
 - Values: 8 to 128 (default 64)
 </td></tr>

 <tr>
 <td>rx_thr_length</td>
 <td>Receive Threshold length in 32 bit double words
 - Values: 8 to 128 (default 64)
 </td></tr>

<tr>
 <td>thr_ctl</td>
 <td>Specifies whether to enable Thresholding for Device mode. Bits 0, 1, 2 of 
 this parmater specifies if thresholding is enabled for non-Iso Tx, Iso Tx and
 Rx transfers accordingly.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - Values: 0 to 7 (default 0)
 Bit values indicate:
 - 0: Thresholding disabled
 - 1: Thresholding enabled
 </td></tr>

<tr>
 <td>dma_desc_enable</td>
 <td>Specifies whether to enable Descriptor DMA mode.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Descriptor DMA disabled
 - 1: Descriptor DMA (default, if available)
 </td></tr>

<tr>
 <td>mpi_enable</td>
 <td>Specifies whether to enable MPI enhancement mode.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: MPI disabled (default)
 - 1: MPI enable
 </td></tr>

<tr>
 <td>pti_enable</td>
 <td>Specifies whether to enable PTI enhancement support.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: PTI disabled (default)
 - 1: PTI enable
 </td></tr>

<tr>
 <td>lpm_enable</td>
 <td>Specifies whether to enable LPM support.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: LPM disabled
 - 1: LPM enable (default, if available)
 </td></tr>

<tr>
 <td>ic_usb_cap</td>
 <td>Specifies whether to enable IC_USB capability.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: IC_USB disabled (default, if available)
 - 1: IC_USB enable 
 </td></tr>

<tr>
 <td>ahb_thr_ratio</td>
 <td>Specifies AHB Threshold ratio.
 - Values: 0 to 3 (default 0)
 </td></tr>

<tr>
 <td>power_down</td>
 <td>Specifies Power Down(Hibernation) Mode.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: Power Down disabled (default)
 - 2: Power Down enabled
 </td></tr>
 
 <tr>
 <td>reload_ctl</td>
 <td>Specifies whether dynamic reloading of the HFIR register is allowed during
 run time. The driver will automatically detect the value for this parameter if
 none is specified. In case the HFIR value is reloaded when HFIR.RldCtrl == 1'b0
 the core might misbehave.
 - 0: Reload Control disabled (default)
 - 1: Reload Control enabled
 </td></tr>

 <tr>
 <td>dev_out_nak</td>
 <td>Specifies whether  Device OUT NAK enhancement enabled or no.
 The driver will automatically detect the value for this parameter if
 none is specified. This parameter is valid only when OTG_EN_DESC_DMA == 1�b1.
 - 0: The core does not set NAK after Bulk OUT transfer complete (default)
 - 1: The core sets NAK after Bulk OUT transfer complete
 </td></tr>

 <tr>
 <td>cont_on_bna</td>
 <td>Specifies whether Enable Continue on BNA enabled or no. 
 After receiving BNA interrupt the core disables the endpoint,when the
 endpoint is re-enabled by the application the  
 - 0: Core starts processing from the DOEPDMA descriptor (default)
 - 1: Core starts processing from the descriptor which received the BNA.
 This parameter is valid only when OTG_EN_DESC_DMA == 1�b1.
 </td></tr>

 <tr>
 <td>ahb_single</td>
 <td>This bit when programmed supports SINGLE transfers for remainder data
 in a transfer for DMA mode of operation. 
 - 0: The remainder data will be sent using INCR burst size (default)
 - 1: The remainder data will be sent using SINGLE burst size.
 </td></tr>

<tr>
 <td>adp_enable</td>
 <td>Specifies whether ADP feature is enabled.
 The driver will automatically detect the value for this parameter if none is
 specified.
 - 0: ADP feature disabled (default)
 - 1: ADP feature enabled
 </td></tr>

  <tr>
 <td>otg_ver</td>
 <td>Specifies whether OTG is performing as USB OTG Revision 2.0 or Revision 1.3
 USB OTG device.
 - 0: OTG 2.0 support disabled (default)
 - 1: OTG 2.0 support enabled 
 </td></tr>

*/
