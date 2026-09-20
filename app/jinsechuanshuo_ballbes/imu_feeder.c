/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/imu_feeder.c
 *
 * Background MPU6500 sampling thread for the tennis watch UI.
 * Reads raw accel/gyro frames from /dev/imu0 at 10 Hz, maps the accel
 * norm onto a "ball speed"-style metric, and counts swings with a
 * rise/decay + hysteresis heuristic on the acceleration magnitude.
 *
 * No LVGL calls from here — read/write shared volatile state only.
 * The UI rule (#4 in .claude/skills/lvgl-v9-round-ui) applies: the worker
 * thread must never touch LVGL objects.
 ****************************************************************************/

#include <nuttx/config.h>

#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <nuttx/spi/spi.h>
#include <nuttx/sensors/mpu60x0.h>

#include "imu_feeder.h"

#ifndef CONFIG_EXAMPLES_IMU_DEVPATH
#  define CONFIG_EXAMPLES_IMU_DEVPATH "/dev/imu0"
#endif

#define FEEDER_LOOP_MS      200                    /* 5 Hz sampling loop */
#define SWING_RISE_MS2      16.0f                  /* |a| rise threshold */
#define SWING_FALL_MS2      8.0f                   /* |a| decay threshold */
#define SWING_RESET_MS      300                    /* swing cadence floor */
#define SPEED_MAX_KMH       250
#define ACCEL_SCALE         (9.80665f / 4096.0f)   /* +-8g FS default */

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

static struct imu_live_s g_live;
static bool g_started;

static pthread_t g_thread;

/****************************************************************************
 * Name: imu_feeder_worker
 *
 * Description:
 *   Sample the imu char device on a slow loop, update g_live. Exits when
 *   never exit (loop-restarted) — never calls LVGL.
 *
 ****************************************************************************/

static FAR void *imu_feeder_thread(FAR void *arg)
{
  int fd;
  struct imu_raw_frame_s raw;
  float mag = 0.0f;
  float peak = 0.0f;
  bool rising = false;
  int since_swing = 0;

  fd = open(CONFIG_EXAMPLES_IMU_DEVPATH, O_RDONLY);
  if (fd < 0)
    {
      fprintf(stderr, "imu_feeder: cannot open %s: %d\n",
              CONFIG_EXAMPLES_IMU_DEVPATH, errno);
      return NULL;
    }

  while (true)
    {
      ssize_t ret = read(fd, (FAR char *)&raw, sizeof(raw));

      if (ret == sizeof(raw))
        {
          float ax = raw.x_accel * ACCEL_SCALE;
          float ay = raw.y_accel * ACCEL_SCALE;
          float az = raw.z_accel * ACCEL_SCALE;

          mag = sqrtf(ax * ax + ay * ay + az * az);
          if (since_swing > 0)
            {
              since_swing--;
            }

          /* Swing detection: rise above the threshold then fall below it
           * counts as one swing (with a cadence bed). */
          /* start rising */

          if (!rising && mag >= SWING_RISE_MS2)
            {
              rising = true;
              peak = mag;
            }

          /* keep tracking the apex */

          else if (rising && mag > peak)
            {
              peak = mag;
            }

          /* fell back: lock in some swing */

          else if (rising && mag < SWING_FALL_MS2)
            {
              rising = false;

              if (since_swing == 0)
                {
                  int v = (int)(peak * 8.0f);   /* peak -> 0..250 km/h */
                  if (v > SPEED_MAX_KMH)
                    {
                      v = SPEED_MAX_KMH;
                    }

                  g_live.speed_kmh = v;
                  g_live.swings++;
                  g_live.live = true;
                  since_swing = SWING_RESET_MS / FEEDER_LOOP_MS;
                }
              peak = 0.0f;
            }

          /* still motions: keep the temperature diagnostic */

          g_live.t_c_x10 = (int)((float)raw.temp / 3.4f + 365.3f);
        }

      usleep(FEEDER_LOOP_MS * 1000);
    }

  close(fd);
  return NULL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

bool imu_feeder_start(void)
{
  int ret;

  if (g_started)
    {
      return true;
    }

  if (access(CONFIG_EXAMPLES_IMU_DEVPATH, F_OK) != 0)
    {
      return false;              /* sensor not wired yet */
    }


  ret = pthread_create(&g_thread, NULL, imu_feeder_thread, NULL);
  if (ret != 0)
    {
      fprintf(stderr, "imu_feeder: pthread_create failed: %d\n", ret);
      return false;
    }

  g_started = true;
  return true;
}

const struct imu_live_s *imu_feeder_live(void)
{
  return &g_live;
}
