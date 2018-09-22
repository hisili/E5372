/************************************************************
  Copyright (C), 2009-2012, Huawei Tech. Co., Ltd.
  FileName:                 hw_rmnet.c
  Version:                  1.0
  Date:                     2011/04/16
  Description:              E587 VNET Interface drv
  Others:                   NA
  Function List:            NA

***********************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/wakelock.h>
#include <linux/icmp.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <linux/in.h>

static struct net_device *hw_rmnet_dev;
static int hw_rmnet_open(struct net_device *dev)
{
	netif_start_queue(dev);
	return 0;
}

static int hw_rmnet_stop(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}


static int hw_rmnet_xmit(struct sk_buff *skb, struct net_device *dev)
{
	/* do nothing just  to release skb */
	dev_kfree_skb_irq(skb);
	return 0;
}


static const struct net_device_ops hw_rmnet_ops = {
	.ndo_open = hw_rmnet_open,
	.ndo_stop = hw_rmnet_stop,
	.ndo_start_xmit = hw_rmnet_xmit,/*lint !e64*/
	.ndo_set_mac_address = eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,	
	.ndo_change_mtu = eth_change_mtu,
};

static void __init hw_rmnet_setup(struct net_device *dev)
{
	ether_setup(dev);
    dev->netdev_ops = &hw_rmnet_ops;
	random_ether_addr(dev->dev_addr);  
}


static int __init hw_rmnet_init(void)
{
	int ret;
	struct net_device *dev;
	
	dev = alloc_netdev(0, "rmnet2", hw_rmnet_setup);

	if (!dev)
       {
   	    return -ENOMEM;
       }
       dev_net_set(dev, &init_net);
	ret = register_netdev(dev);
	if (ret) 
       {
		free_netdev(dev);
		return ret;
	}
	hw_rmnet_dev=dev;
	return 0;
}
static void __exit hw_rmnet_cleanup(void)
{
	unregister_netdev(hw_rmnet_dev);
	free_netdev(hw_rmnet_dev);
}

module_init(hw_rmnet_init);
module_exit(hw_rmnet_cleanup);
