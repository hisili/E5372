/*
 *  linux/drivers/mmc/core/sd_ops.h
 *
 *  Copyright 2006-2007 Pierre Ossman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */

#ifndef _MMC_SD_OPS_H
#define _MMC_SD_OPS_H

/*at^sd interface 2012-01-16 begin*/
#define SDMMC_BLOCK_BYTE (512)
#define SDMMC_ONE_BLOCK (1)
#define ERROR (-1)
#define TRUE (1)
#define OK (0)
#define SUCCESS (0)
typedef enum
{
  SDMMC_PROCESS_OVER_OR_UNDO = 0,
  SDMMC_PROCESS_IN,
  SDMMC_PROCESS_ERROR,
}SDMMC_PROCESS_STATUS;

/*Error*/
#define SDMMC_NOT_IN           (0)
#define SDMMC_INIT_FAIL        (1)
#define SDMMC_PARAMETER_ERROR  (2)
#define SDMMC_ADDR_ERROR       (3)
#define SDMMC_OTHER_ERROR      (4)
#define SDMMC_NO_ERROR_PARA    (5)

typedef enum
{
 SDMMC_DATA0 = 0,
 SDMMC_DATA1,
 SDMMC_DATA2,
 SDMMC_DATA3,
 SDMMC_DATA_BUTT,
}SDMMC_DATA;
/*Command*/
typedef enum
{
  SDMMC_AT_FORMAT = 0,
  SDMMC_AT_ERASE_ALL,
  SDMMC_AT_ERASE_ADDR,
  SDMMC_AT_WRTIE_ADDR,
  SDMMC_AT_READ,
  SDMMC_AT_BUTT,
}SDMMC_AT_COMMAND;

typedef struct
{
	int ulOp;
	int ulAddr;
	int ulData;
	int *pulErr;
}SDMMC_ATPROCESS_TASK_ARG;


/*at^sd interface 2012-01-16 end*/

int mmc_app_set_bus_width(struct mmc_card *card, int width);
int mmc_send_app_op_cond(struct mmc_host *host, u32 ocr, u32 *rocr);
int mmc_send_if_cond(struct mmc_host *host, u32 ocr);
int mmc_send_relative_addr(struct mmc_host *host, unsigned int *rca);
int mmc_app_send_scr(struct mmc_card *card, u32 *scr);
int mmc_sd_switch(struct mmc_card *card, int mode, int group,
	u8 value, u8 *resp);
int mmc_sd30_switch(struct mmc_card *card, int mode,
	u8 group0Value,u8 group2Value,u8 group3Value,u8 *resp);
int mmc_send_voltage_switch(struct mmc_host *host,u32 *ulResp );/*syb*/
int mmc_send_tuning_pattern(struct mmc_card *card,u8 *resp); /*syb*/
int mmc_send_erase(struct mmc_card *card);
int mmc_sd_send_erase_group_end(struct mmc_card *card,u32 dwEraseEnd);
int mmc_sd_send_erase_group_start(struct mmc_card *card,u32 dwEraseStart);

#endif

