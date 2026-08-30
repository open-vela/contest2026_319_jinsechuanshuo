/****************************************************************************
 * vendor/bes/boards/best1700_ep/src/ap.c
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
#include <arch/board/board.h>
#include <assert.h>
#include <debug.h>
#include <errno.h>
#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <nuttx/config.h>
#include <nuttx/fs/partition.h>
#include <nuttx/lcd/lcd.h>
#include <nuttx/lcd/st7735.h>
#include <nuttx/mmcsd.h>
#include <nuttx/sdio.h>

#ifdef CONFIG_SPI_FLASH_FATFS
extern int bes_spi_flash_fatfs_initialize(void);
#endif

#ifdef CONFIG_SPI_IMU
extern int bes_spi_imu_initialize(void);
#endif

#ifdef CONFIG_BES1700_AP

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void emmc_partition_options(FAR char *part)
{
}

void emmc_init_before_partition(FAR struct sdio_dev_s *dev, FAR char *path)
{
}

void emmc_init_after_partition(FAR struct sdio_dev_s *dev, FAR char *path)
{
#ifdef CONFIG_BOARD_COREDUMP_BLKDEV
    char *devpath = CONFIG_BOARD_COREDUMP_DEVPATH; // for example: /dev/mmcsd0core
    char cmd[64]; // such as: mkgpt write /dev/mmcsd0 mmcsd0fs:3335M mmcsd0core:64M
    snprintf(cmd, sizeof(cmd), "mkgpt write %s %sfs:%dM %s:%dM", path, &path[5], 3335, &devpath[5], 64);

    system(cmd);
#endif
}

#ifdef CONFIG_SPI_FLASH_FATFS
int board_spi_flash_fatfs_initialize(void)
{
  return bes_spi_flash_fatfs_initialize();
}
#endif

#ifdef CONFIG_SPI_IMU
int board_spi_imu_initialize(void)
{
  return bes_spi_imu_initialize();
}
#endif

/****************************************************************************
 * Name: board_late_initialize
 *
 * Description:
 *   Board-specific initialization called after OS initialization.
 *   This is where we initialize SPI Flash and IMU drivers.
 *
 ****************************************************************************/

#ifdef CONFIG_BOARD_LATE_INITIALIZE
void board_late_initialize(void)
{
  int ret;

  syslog(LOG_INFO, "INFO: board_late_initialize called\n");

#ifdef CONFIG_SPI_FLASH_FATFS
  ret = board_spi_flash_fatfs_initialize();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: SPI Flash FATFS init failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_SPI_IMU
  ret = board_spi_imu_initialize();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: SPI IMU init failed: %d\n", ret);
    }
#endif
}
#endif /* CONFIG_BOARD_LATE_INITIALIZE */

#endif /* CONFIG_BES1700_AP */
