/****************************************************************************/
/* apps/examples/w25fs/w25fs_main.c                                         */
/*                                                                          */
/* W25Q64 SPI NOR Flash filesystem demo application.                        */
/*                                                                          */
/* - Probes the W25Q64 over SPI, registers it as /dev/mtdblockN and mounts  */
/*   a FAT filesystem at CONFIG_W25Q64_MOUNTPOINT (default /mnt/w25q64).    */
/*   If the device is blank (mount fails with EINVAL), offer "w25fs format". */
/*                                                                          */
/* Licensed to the Apache Software Foundation (ASF) under one or more       */
/* contributor license agreements.                                          */
/* SPDX-License-Identifier: Apache-2.0                                      */
/****************************************************************************/

#include <nuttx/config.h>

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/spi/spi.h>
#include <nuttx/mtd/mtd.h>
#include <nuttx/fs/fs.h>
#include <syslog.h>

#ifdef CONFIG_EXAMPLES_W25FS

/* BES SPI bus initializer from prebuilt libbeschip_ap.a                    */

extern struct spi_dev_s *bes_spibus_initialize(int port);

/* mkfatfs API from apps/fsutils (needs CONFIG_FSUTILS_MKFATFS=y)           */

#if defined(CONFIG_FSUTILS_MKFATFS)
#include <fsutils/mkfatfs.h>
#endif

#ifndef CONFIG_W25Q64_SPI_PORT
#  define CONFIG_W25Q64_SPI_PORT 0
#endif

#ifndef CONFIG_W25Q64_MOUNTPOINT
#  define CONFIG_W25Q64_MOUNTPOINT "/mnt/w25q64"
#endif

#undef SPI_NO_CS

#define W25_PROBE_FREQ          1000000 /* conservative raw probe clock */
#define W25_JEDEC_CMD           0x9f

/****************************************************************************
 * Name: w25fs_probe_raw_id
 *
 * Description:
 *   Low level JEDEC ID probe for diagnostics, so that when w25_initialize()
 *   reports 00/00 we can tell whether MISO is floating (0x00 / 0xff) or the
 *   chip answers with a real EF xx xx pattern.
 *
 ****************************************************************************/

static void w25fs_probe_raw_id(void)
{
  struct spi_dev_s *spi;
  uint8_t tx[5];
  uint8_t rx[5];
  int i;

  spi = bes_spibus_initialize(CONFIG_W25Q64_SPI_PORT);
  if (spi == NULL)
    {
      fprintf(stderr, "w25fs: probe: cannot init bus %d\n",
              CONFIG_W25Q64_SPI_PORT);
      return;
    }

  SPI_SETBITS(spi, 8);
  SPI_SETFREQUENCY(spi, W25_PROBE_FREQ);
  SPI_SETMODE(spi, SPIDEV_MODE0);

  memset(tx, 0, sizeof(tx));
  memset(rx, 0, sizeof(rx));
  tx[0] = W25_JEDEC_CMD;

  SPI_SELECT(spi, 0, true);
  SPI_EXCHANGE(spi, tx, rx, sizeof(tx));
  SPI_SELECT(spi, 0, false);

  printf("w25fs: raw JEDEC response on SPI%d CS0:", CONFIG_W25Q64_SPI_PORT);
  for (i = 0; i < (int)sizeof(rx); i++)
    {
      printf(" %02X", rx[i]);
    }

  printf("\n");
  if (rx[1] == 0x40 && rx[2] >= 0x10)
    {
      printf("w25fs: JEDEC looks valid (0x40 series) - check driver id "
             "table/port mapping\n");
    }
  else if (rx[1] == 0x00 && rx[2] == 0x00 && rx[3] == 0x00)
    {
      printf("w25fs: MISO stuck low - check wiring (CS/GND/VCC) or try "
             "CONFIG_W25Q64_SPI_PORT 1\n");
    }
  else if (rx[1] == 0xff && rx[2] == 0xff && rx[3] == 0xff)
    {
      printf("w25fs: MISO floating high - chip missing/no power\n");
    }
}


/* Winbond W25Q64: 8MB / 64KB blocks                                        */

#define MTD_BLOCK_MINOR       1      /* /dev/mtdblock1 */
#define DEV_PATH          "/dev/mtdblock1"

/****************************************************************************
 * Name: w25fs_board_init
 *
 * Description:
 *   Initialise the SPI bus, the W25 MTD driver, register a block device,
 *   and mount a FAT volume. Returns OK when content is mountable.
 *
 ****************************************************************************/

static int w25fs_mount(bool fformat)
{
  struct spi_dev_s *spi;
  struct mtd_dev_s *mtd;
  int ret;

  spi = bes_spibus_initialize(CONFIG_W25Q64_SPI_PORT);
  if (spi == NULL)
    {
      fprintf(stderr, "w25fs: ERROR: SPI bus %d init failed\n",
              CONFIG_W25Q64_SPI_PORT);
      return -ENODEV;
    }

  mtd = w25_initialize(spi);
  if (mtd == NULL)
    {
      fprintf(stderr, "w25fs: ERROR: w25_initialize() failed\n");
      w25fs_probe_raw_id();
      return -ENODEV;
    }

  /* wrap as block device /dev/mtdblock1 */

  ret = ftl_initialize(MTD_BLOCK_MINOR, mtd);
  if (ret < 0)
    {
      fprintf(stderr, "w25fs: ERROR: ftl_initialize() failed: %d\n", ret);
      return ret;
    }
  else
    {
      printf("w25fs: registered /dev/mtdblock%d\n", MTD_BLOCK_MINOR);
    }

  /* always try mounting first */

  ret = nx_mount(DEV_PATH, CONFIG_W25Q64_MOUNTPOINT, "vfat", 0, NULL);
  if (ret >= 0)
    {
      printf("w25fs: FAT mounted at %s\n", CONFIG_W25Q64_MOUNTPOINT);
      return OK;
    }

  if (!fformat)
    {
      fprintf(stderr, "w25fs: mount failed: %d; run 'w25fs format' to "
              "create a FAT volume\n", -ret);
      return ret;
    }

  /* blank chip - build a fresh FAT filesystem                              */

#ifdef CONFIG_FSUTILS_MKFATFS
  struct fat_format_s fmt = FAT_FORMAT_INITIALIZER;

  printf("w25fs: formatting %s as FAT (please wait)...\n", DEV_PATH);
  ret = mkfatfs(DEV_PATH, &fmt);
  if (ret < 0)
    {
      fprintf(stderr, "w25fs: format failed: %d\n", ret);
      return ret;
    }
#endif

  ret = nx_mount(DEV_PATH, CONFIG_W25Q64_MOUNTPOINT, "vfat", 0, NULL);
  if (ret >= 0)
    {
      printf("w25fs: FAT volume erased and mounted fresh at %s\n",
             CONFIG_W25Q64_MOUNTPOINT);
      return OK;
    }

  fprintf(stderr, "w25fs: mount aborted: %d\n", -ret);
  return ret;
}

/****************************************************************************
 * Name: w25fs_test
 *
 * Description:
 *   Write/read/delete a small file to prove usage of the filesystem.
 *
 ****************************************************************************/

static int w25fs_test(void)
{
  char path[128];
  char buf[128];
  int fd;
  int ret;

  snprintf(path, sizeof(path), "%s/w25fs.txt", CONFIG_W25Q64_MOUNTPOINT);

  /* write */

  fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0)
    {
      fprintf(stderr, "w25fs: can't create %s: %d\n", path, errno);
      return -errno;
    }

  const char *payload = "Hello from W25Q64!\r\n";
  ret = write(fd, payload, strlen(payload));
  close(fd);
  if (ret < 0)
    {
      fprintf(stderr, "w25fs: write failed: %d\n", errno);
      return -errno;
    }

  /* read back */

  fd = open(path, O_RDONLY);
  if (fd < 0)
    {
      fprintf(stderr, "w25fs: can't reopen %s: %d\n", path, errno);
      return -errno;
    }

  memset(buf, 0, sizeof(buf));
  ret = read(fd, buf, sizeof(buf) - 1);
  close(fd);
  if (ret < 0)
    {
      fprintf(stderr, "w25fs: read failed: %d\n", errno);
      return -errno;
    }

  printf("w25fs: wrote & read back %d bytes: \"%s\"\n", ret, buf);

  /* delete */

  ret = unlink(path);
  if (ret < 0)
    {
      fprintf(stderr, "w25fs: unlink failed: %d\n", errno);
      return -errno;
    }

  printf("w25fs: test passed, device works as a filesystem\n");
  return OK;
}

/****************************************************************************
 * Name: w25fs_info
 *
 * Description:
 *   Print flash geometry info found from the MTD dev interface.
 *
 ****************************************************************************/

static int w25fs_info(void)
{
  struct mtd_geometry_s geo;
  int fd;
  int ret;

  fd = open(DEV_PATH, O_RDONLY);
  if (fd < 0)
    {
      fprintf(stderr, "w25fs: can't open %s: %d\n", DEV_PATH, errno);
      return -errno;
    }

  ret = ioctl(fd, MTDIOC_GEOMETRY, (unsigned long)((uintptr_t)&geo));
  close(fd);
  if (ret < 0)
    {
      fprintf(stderr, "w25fs: MTDIOC_GEOMETRY failed: %d\n", errno);
      return -errno;
    }

  printf("w25fs: block size %u, erase block size %u, %u erase blocks "
         "(%u KiB total), model: %s\n",
         geo.blocksize, geo.erasesize, geo.neraseblocks,
         (unsigned)((geo.blocksize * geo.neraseblocks) >> 10),
         geo.model);
  return OK;
}

/****************************************************************************
 * Name: w25fs_format
 *
 * Description:
 *   Erase the whole chip and build a new FAT filesystem on it.
 *
 ****************************************************************************/

static int w25fs_format(void)
{
  fprintf(stderr, "w25fs: formatting is handled 'w25fs' with no argument "
          "when mount fails (auto-format), or 'w25fs format' for force "
          "format & mount\n");
  return w25fs_mount(true);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int w25fs_main(int argc, FAR char *argv[])
{
  const char *cmd = (argc > 1) ? argv[1] : "mount";

  if (strcmp(cmd, "format") == 0)
    {
      return w25fs_format();
    }
  else if (strcmp(cmd, "test") == 0)
    {
      int ret = w25fs_mount(false);
      if (ret < 0)
        {
          return ret;
        }

      return w25fs_test();
    }
  else if (strcmp(cmd, "info") == 0)
    {
      return w25fs_info();
    }
  else if (strcmp(cmd, "mount") == 0)
    {
      return w25fs_mount(false);
    }
  else
    {
      printf("usage: w25fs [mount|format|test|info]\n");
      return -EINVAL;
    }
}

#endif /* CONFIG_EXAMPLES_W25FS */
