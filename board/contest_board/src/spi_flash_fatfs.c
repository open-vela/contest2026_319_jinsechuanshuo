/****************************************************************************
 * vendor/bes/boards/best1700_ep/aos_evb/src/spi_flash_fatfs.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/spi/spi.h>
#include <nuttx/mtd/mtd.h>
#include <nuttx/fs/fs.h>

#ifdef CONFIG_SPI_FLASH_FATFS

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef CONFIG_BES_SPI_FLASH_CSNUM
#  define CONFIG_BES_SPI_FLASH_CSNUM 0
#endif

#ifndef CONFIG_BES_SPI_FLASH_MOUNTPOINT
#  define CONFIG_BES_SPI_FLASH_MOUNTPOINT "/mnt/spiflash"
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: bes_spi_flash_fatfs_initialize
 *
 * Description:
 *   Initialize, configure, and mount the SPI Flash with FAT file system.
 *   The FLASH will be mounted at CONFIG_BES_SPI_FLASH_MOUNTPOINT.
 *
 ****************************************************************************/

int bes_spi_flash_fatfs_initialize(void)
{
  struct spi_dev_s *spi;
  struct mtd_dev_s *mtd;
  static bool initialized = false;
  int ret;

  /* Have we already initialized? */

  if (!initialized)
    {
      /* No.. Get the SPI port driver */

      spi = bes_spibus_initialize(CONFIG_BES_SPI_FLASH_CSNUM);
      if (!spi)
        {
          syslog(LOG_ERR, "ERROR: Failed to initialize SPI port %d\n",
                 CONFIG_BES_SPI_FLASH_CSNUM);
          return -ENODEV;
        }

      /* Now bind the SPI interface to the SPI FLASH driver
       * Use gd25 driver for GigaDevice SPI Flash
       * Other options: w25 for Winbond, m25p for Macronix, etc.
       */

      mtd = gd25_initialize(spi, 0);
      if (!mtd)
        {
          syslog(LOG_ERR, "ERROR: Failed to bind SPI port to the "
                 "SPI FLASH driver\n");
          return -ENODEV;
        }

      /* Use the FTL layer to wrap the MTD driver as a block driver
       * at /dev/mtdblockN, where N=minor device number.
       */

      ret = ftl_initialize(0, mtd);
      if (ret < 0)
        {
          syslog(LOG_ERR, "ERROR: Failed to initialize the FTL layer: %d\n",
                 ret);
          return ret;
        }

      /* Mount the FAT file system */

      ret = nx_mount("/dev/mtdblock0", CONFIG_BES_SPI_FLASH_MOUNTPOINT,
                     "vfat", 0, NULL);
      if (ret < 0)
        {
          syslog(LOG_ERR, "ERROR: Failed to mount FAT at %s: %d\n",
                 CONFIG_BES_SPI_FLASH_MOUNTPOINT, ret);
          return ret;
        }

      syslog(LOG_INFO, "INFO: FAT volume %s mount SPI flash success\n",
             CONFIG_BES_SPI_FLASH_MOUNTPOINT);

      /* Now we are initialized */

      initialized = true;
    }

  return OK;
}

#endif /* CONFIG_SPI_FLASH_FATFS */
