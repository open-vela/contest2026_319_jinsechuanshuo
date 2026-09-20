/****************************************************************************/
/* apps/examples/imu/imu_main.c                                             */
/*                                                                          */
/* MPU6500 IMU (Invensense MPU60x0 family) self-test builtin application.   */
/*                                                                          */
/* Registers /dev/imu0 on hardware SPI0 (CS0 = G33), then reads live        */
/* accel/gyro/temperature samples through the mpu60x0 character driver and  */
/* prints them to the console.                                              */
/*                                                                          */
/* Wiring (SPI hardware, EVB pins confirmed by disassembly of the prebuilt  */
/* bes_spi.o):  CLK -> G32   CS -> G33   MISO(SDO) -> G34   MOSI(SDI) -> G35 */
/*                                                                          */
/* SPDX-License-Identifier: Apache-2.0                                      */
/****************************************************************************/

#include <nuttx/config.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/spi/spi.h>
#include <nuttx/sensors/mpu60x0.h>

#ifdef CONFIG_EXAMPLES_IMU

/* BES SPI bus initializer from prebuilt libbeschip_ap.a                     */

extern struct spi_dev_s *bes_spibus_initialize(int port);

#ifndef CONFIG_BES_SPI_IMU_PORT
#  define CONFIG_BES_SPI_IMU_PORT   0
#endif

#ifndef CONFIG_EXAMPLES_IMU_DEVPATH
#  define CONFIG_EXAMPLES_IMU_DEVPATH "/dev/imu0"
#endif

#define SAMPLE_COUNT_DEFAULT   5
#define SAMPLE_IDLE_MS         100

/* Conversion constants (MPU6500 datasheet, default FS_SEL settings):
 *
 *  accel: FS_SEL=2 (+-8g) -> 4096 LSB/g
 *         (Kconfig default MPU60X0_AFS_SEL=2 -> +-8 g  ->  4096 LSB/g)
 *  gyro : FS_SEL=2 (+-1000 deg/s) -> 32.8 LSB/(deg/s)
 *  temp : degC = temp/340.0 + 36.53 (MPU6000/6500 family)
 */

#define ACCEL_LSB_PER_G     4096.0f     /* FS_SEL = 2 */
#define ACCEL_LSB_PER_MS2   9.80665f / 4096.0f
#define GYRO_LSB_PER_DPS    32.8f
#define TEMP_DIV            340.0f
#define TEMP_OFFSET         36.53f

/* The mpu60x0 driver's read() copies a 14-byte raw register frame:
 * x_accel, y_accel, z_accel, temp, x_gyro, y_gyro, z_gyro (int16 each).
 * The driver keeps the type private, so mirror the frame layout here.
 */

struct imu_raw_frame_s
{
  int16_t x_accel;
  int16_t y_accel;
  int16_t z_accel;
  int16_t temp;
  int16_t x_gyro;
  int16_t y_gyro;
  int16_t z_gyro;
} __attribute__((packed));

/****************************************************************************
 * Name: imu_device_register
 *
 * Description:
 *   Bring up the SPI bus and register the MPU6500 as a character device.
 *   The register step only runs when /dev/imu0 is not present yet.
 *
 ****************************************************************************/

static int imu_device_register(void)
{
  struct mpu_config_s mpuc;
  struct spi_dev_s *spi;
  int ret;

  memset(&mpuc, 0, sizeof(mpuc));

  spi = bes_spibus_initialize(CONFIG_BES_SPI_IMU_PORT);
  if (spi == NULL)
    {
      fprintf(stderr, "imu: SPI bus %d init failed\n",
              CONFIG_BES_SPI_IMU_PORT);
      return -ENODEV;
    }

  mpuc.spi      = spi;
  mpuc.spi_devid = 0;              /* SPI0 CS0 = G33 */

  ret = mpu60x0_register(CONFIG_EXAMPLES_IMU_DEVPATH, &mpuc);
  if (ret < 0)
    {
      fprintf(stderr, "imu: mpu60x0_register failed: %d\n", ret);
      return ret;
    }

  printf("imu: MPU6500 registered at %s (SPI bus %d, dev %d)\n",
         CONFIG_EXAMPLES_IMU_DEVPATH, CONFIG_BES_SPI_IMU_PORT,
         mpuc.spi_devid);
  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int imu_main(int argc, FAR char *argv[])
{
  struct imu_raw_frame_s buf;
  int fd;
  int i;
  int n;

  n = (argc > 1) ? atoi(argv[1]) : SAMPLE_COUNT_DEFAULT;
  if (n <= 0)
    {
      n = 1;
    }

  /* register if this is the very first invocation */

  fd = open(CONFIG_EXAMPLES_IMU_DEVPATH, O_RDONLY);
  if (fd < 0)
    {
      int ret = imu_device_register();
      if (ret < 0)
        {
          return ret;
        }

      fd = open(CONFIG_EXAMPLES_IMU_DEVPATH, O_RDONLY);
      if (fd < 0)
        {
          fprintf(stderr, "imu: can't open %s: %d\n",
                  CONFIG_EXAMPLES_IMU_DEVPATH, errno);
          return -errno;
        }
    }

  printf("imu: reading %d sample(s) from %s\n", n, CONFIG_EXAMPLES_IMU_DEVPATH);

  for (i = 0; i < n; i++)
    {
      ssize_t ret = read(fd, (FAR char *)&buf, sizeof(buf));
      if (ret != sizeof(buf))
        {
          fprintf(stderr, "imu: read failed: %d\n", errno);
          close(fd);
          return ret < 0 ? (int)ret : -ENODATA;
        }

      printf("imu[%02d] acc(m/s^2): %7.2f %7.2f %7.2f | "
             "gyro(dps): %7.2f %7.2f %7.2f | T=%5.2f C\n",
             i,
             buf.x_accel * ACCEL_LSB_PER_MS2,
             buf.y_accel * ACCEL_LSB_PER_MS2,
             buf.z_accel * ACCEL_LSB_PER_MS2,
             buf.x_gyro / GYRO_LSB_PER_DPS,
             buf.y_gyro / GYRO_LSB_PER_DPS,
             buf.z_gyro / GYRO_LSB_PER_DPS,
             (float)buf.temp / TEMP_DIV + TEMP_OFFSET);

      usleep(100 * 1000);          /* default sampling, 100ms */
    }

  close(fd);
  printf("imu: self-test complete\n");
  return OK;
}
#endif /* CONFIG_EXAMPLES_IMU */
