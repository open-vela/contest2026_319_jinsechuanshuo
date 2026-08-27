/****************************************************************************
 * vendor/bes/boards/best1700_ep/aos_evb/src/spi_imu.c
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
#include <nuttx/sensors/bmi160.h>
#include <syslog.h>

/* BES SPI bus initialization function from libbeschip_ap.a */

extern struct spi_dev_s *bes_spibus_initialize(int port);

#ifdef CONFIG_SPI_IMU

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef CONFIG_BES_SPI_IMU_CSNUM
#  define CONFIG_BES_SPI_IMU_CSNUM 1
#endif

#ifndef CONFIG_BES_SPI_IMU_DEVNAME
#  define CONFIG_BES_SPI_IMU_DEVNAME "/dev/accel0"
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: bes_spi_imu_initialize
 *
 * Description:
 *   Initialize and register the BMI160 SPI IMU driver.
 *   Uses SPI0 CS1 for IMU connection.
 *
 ****************************************************************************/

int bes_spi_imu_initialize(void)
{
  struct spi_dev_s *spi;
  static bool initialized = false;
  int ret;

  /* Have we already initialized? */

  if (!initialized)
    {
      /* No.. Get the SPI port driver
       * Note: We use the same SPI bus as Flash, but different CS (CS1)
       */

      spi = bes_spibus_initialize(CONFIG_BES_SPI_IMU_CSNUM);
      if (!spi)
        {
          syslog(LOG_ERR, "ERROR: Failed to initialize SPI port for IMU "
                 "(CS%d)\n", CONFIG_BES_SPI_IMU_CSNUM);
          return -ENODEV;
        }

      /* Register the BMI160 SPI IMU driver */

      ret = bmi160_register(CONFIG_BES_SPI_IMU_DEVNAME, spi);
      if (ret < 0)
        {
          syslog(LOG_ERR, "ERROR: Failed to register BMI160 driver: %d\n",
                 ret);
          return ret;
        }

      syslog(LOG_INFO, "INFO: BMI160 IMU registered at %s (SPI0 CS%d)\n",
             CONFIG_BES_SPI_IMU_DEVNAME, CONFIG_BES_SPI_IMU_CSNUM);

      /* Now we are initialized */

      initialized = true;
    }

  return OK;
}

#endif /* CONFIG_SPI_IMU */
