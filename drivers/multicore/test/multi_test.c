
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include "BSP.h"

#include "icc_test.h"


static ssize_t mtest_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	char *command = NULL;
	int input1 = 0;
	int input2 = 0;
	int input3 = 0;
	
	command = kzalloc(128, GFP_KERNEL);
	if (!command)
	{
		printk("malloc command failed\n");
		return count;
	}

	sscanf(buf, "%s %d %d %d", command, &input1, &input2, &input3);
	printk("command=%s  input1=%d  input2=%d  input3=%d\n", command, input1, input2, input3);

	if (!strncmp(command, "BSP_ICC_ST_INIT_001", strlen("BSP_ICC_ST_INIT_001")))
	{
		printk("begin to test: BSP_ICC_ST_INIT_001\n");
		BSP_ICC_ST_INIT_001();
	}
	else if (!strncmp(command, "BSP_ICC_ST_INIT_002", strlen("BSP_ICC_ST_INIT_002")))
	{
		printk("begin to test: BSP_ICC_ST_INIT_002\n");
		BSP_ICC_ST_INIT_002();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_002", strlen("BSP_ICC_ST_OPEN_CLOSE_002")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_002\n");
		BSP_ICC_ST_OPEN_CLOSE_002();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_003", strlen("BSP_ICC_ST_OPEN_CLOSE_003")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_003\n");
		BSP_ICC_ST_OPEN_CLOSE_003();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_004", strlen("BSP_ICC_ST_OPEN_CLOSE_004")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_004\n");
		BSP_ICC_ST_OPEN_CLOSE_004();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_005", strlen("BSP_ICC_ST_OPEN_CLOSE_005")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_005\n");
		BSP_ICC_ST_OPEN_CLOSE_005();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_006", strlen("BSP_ICC_ST_OPEN_CLOSE_006")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_006\n");
		BSP_ICC_ST_OPEN_CLOSE_006();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_007", strlen("BSP_ICC_ST_OPEN_CLOSE_007")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_007\n");
		BSP_ICC_ST_OPEN_CLOSE_007();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_008", strlen("BSP_ICC_ST_OPEN_CLOSE_008")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_008\n");
		BSP_ICC_ST_OPEN_CLOSE_008();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_009", strlen("BSP_ICC_ST_OPEN_CLOSE_009")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_009\n");
		BSP_ICC_ST_OPEN_CLOSE_009();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_010", strlen("BSP_ICC_ST_OPEN_CLOSE_010")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_010\n");
		BSP_ICC_ST_OPEN_CLOSE_010();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_011", strlen("BSP_ICC_ST_OPEN_CLOSE_011")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_011\n");
		BSP_ICC_ST_OPEN_CLOSE_011();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_012", strlen("BSP_ICC_ST_OPEN_CLOSE_012")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_012\n");
		BSP_ICC_ST_OPEN_CLOSE_012();
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_0013", strlen("BSP_ICC_ST_OPEN_CLOSE_0013")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_0013  input1=%d\n", input1);
		BSP_ICC_ST_OPEN_CLOSE_013(input1);
	}
	else if (!strncmp(command, "BSP_ICC_ST_OPEN_CLOSE_001", strlen("BSP_ICC_ST_OPEN_CLOSE_001")))
	{
		printk("begin to test: BSP_ICC_ST_OPEN_CLOSE_001\n");
		BSP_ICC_ST_OPEN_CLOSE_001();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_STREAM_001", strlen("BSP_ICC_ST_RW_STREAM_001")))
	{
		printk("begin to test: BSP_ICC_ST_RW_STREAM_001\n");
		BSP_ICC_ST_RW_STREAM_001();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_STREAM_002", strlen("BSP_ICC_ST_RW_STREAM_002")))
	{
		printk("begin to test: BSP_ICC_ST_RW_STREAM_002\n");
		BSP_ICC_ST_RW_STREAM_002();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_STREAM_003", strlen("BSP_ICC_ST_RW_STREAM_003")))
	{
		printk("begin to test: BSP_ICC_ST_RW_STREAM_003\n");
		BSP_ICC_ST_RW_STREAM_003();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_STREAM_004", strlen("BSP_ICC_ST_RW_STREAM_004")))
	{
		printk("begin to test: BSP_ICC_ST_RW_STREAM_004\n");
		BSP_ICC_ST_RW_STREAM_004();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_STREAM_005", strlen("BSP_ICC_ST_RW_STREAM_005")))
	{
		printk("begin to test: BSP_ICC_ST_RW_STREAM_005  input1=%d input2=%d\n", input1, input2);
		BSP_ICC_ST_RW_STREAM_005(input1, input2);
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_STREAM_006", strlen("BSP_ICC_ST_RW_STREAM_006")))
	{
		printk("begin to test: BSP_ICC_ST_RW_STREAM_006  input1=%d\n", input1);
		BSP_ICC_ST_RW_STREAM_006(input1);
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_PACKET_001", strlen("BSP_ICC_ST_RW_PACKET_001")))
	{
		printk("begin to test: BSP_ICC_ST_RW_PACKET_001\n");
		BSP_ICC_ST_RW_PACKET_001();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_PACKET_002", strlen("BSP_ICC_ST_RW_PACKET_002")))
	{
		printk("begin to test: BSP_ICC_ST_RW_PACKET_002\n");
		BSP_ICC_ST_RW_PACKET_002();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_PACKET_003", strlen("BSP_ICC_ST_RW_PACKET_003")))
	{
		printk("begin to test: BSP_ICC_ST_RW_PACKET_003\n");
		BSP_ICC_ST_RW_PACKET_003();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_PACKET_004", strlen("BSP_ICC_ST_RW_PACKET_004")))
	{
		printk("begin to test: BSP_ICC_ST_RW_PACKET_004\n");
		BSP_ICC_ST_RW_PACKET_004();
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_PACKET_005", strlen("BSP_ICC_ST_RW_PACKET_005")))
	{
		printk("begin to test: BSP_ICC_ST_RW_PACKET_005  input1=%d input2=%d  input3=%d\n", input1, input2, input3);
		BSP_ICC_ST_RW_PACKET_005(input1, input2, input3);
	}
	else if (!strncmp(command, "BSP_ICC_ST_RW_PACKET_006", strlen("BSP_ICC_ST_RW_PACKET_006")))
	{
		printk("begin to test: BSP_ICC_ST_RW_PACKET_006  input1=%d input2=%d\n", input1, input2);
		BSP_ICC_ST_RW_PACKET_006(input1, input2);
	}
	
	kfree(command);

	return count;
}
static DEVICE_ATTR(mtest, 0666, NULL, mtest_store);


static struct attribute *mtest_attributes[] = {
        &dev_attr_mtest.attr,
	NULL
};

static const struct attribute_group mtest_group = {
	.attrs = mtest_attributes,
};

static int multi_test_probe(struct platform_device *pdev)
{
	int ret = 0;

	if ((ret = sysfs_create_group(&pdev->dev.kobj, &mtest_group)))
		return -1;

	return ret;
}

static int multi_test_remove(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &mtest_group);

	return 0;
}

static struct platform_driver multi_test_driver = {
	.probe	= multi_test_probe,
	.remove	= multi_test_remove,
	.driver	= {
		.name	= "multi_test",
	},
};

static struct platform_device multi_test_device = {
	.name	= "multi_test",
	.id		= -1,
};

static int __init p500_multicore_test_init(void)
{
	int ret = 0;
	printk("***************************************************************\n");
	printk("begin to init mutilcore test: 0000\n");
	
	ret = platform_driver_register(&multi_test_driver);
	if (ret)
		return -1;

	ret = platform_device_register(&multi_test_device);
	if (ret)
		platform_driver_unregister(&multi_test_driver);

	return ret;
}

static void __exit p500_multicore_test_cleanup(void)
{
	platform_driver_unregister(&multi_test_driver);
	platform_device_unregister(&multi_test_device);
}
   

module_init(p500_multicore_test_init);
module_exit(p500_multicore_test_cleanup);

