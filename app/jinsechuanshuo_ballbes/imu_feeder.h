/****************************************************************************/
/* apps/examples/jinsechuanshuo_ballbes/imu_feeder.h                        */
/*                                                                          */
/* Background MPU6500 sampler shared between the UI (LVGL main loop) and    */
/* a low-priority pthread. The LVGL skill rule applies: the UI thread must  */
/* own all LVGL calls; the feeder only writes to volatile shared state and  */
/* the UI consumes it in its refresh tick.                                  */
/****************************************************************************/

#ifndef __IMU_FEEDER_H
#define __IMU_FEEDER_H

#include <nuttx/config.h>
#include <stdbool.h>

/* Live sample snapshot (updated by the feeder thread at ~10 Hz). UI reads
 * this struct (integer fields only -> single-word atomic on Cortex-M).   */

struct imu_live_s
{
  volatile int     speed_kmh;   /* mapped ball-speed metric 0..250 */
  volatile int     swings;      /* detected swings (net of zero-cross hyst) */
  volatile bool    live;        /* true once the feeder saw a real sample */
  volatile int     t_c_x10;     /* temperature *10 (diagnostics) */
};

/* Start the feeder thread. Non-fatal when no /dev/imu0 is present:
 * returns false and leaves the demo numbers in place. */

bool imu_feeder_start(void);

/* Pointer to the live sample snapshot (never NULL). */

const struct imu_live_s *imu_feeder_live(void);

#endif /* __IMU_FEEDER_H */
