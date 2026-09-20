/****************************************************************************/
/* apps/examples/jinsechuanshuo_ballbes/ui_train.h                          */
/*                                                                          */
/* Golden Legend Tennis - TRAINING page (ready / countdown / running).      */
/* Template after the reference three-state exercise page pattern.          */
/****************************************************************************/

#ifndef __UI_TRAIN_H
#define __UI_TRAIN_H

#include "ui_common.h"

lv_obj_t *ui_train_create(lv_obj_t *parent);

/* Feed fresh IMU-driven stats into the running state (called from the
 * main LVGL loop; sampling itself happens in the caller's timer/run). */

void ui_train_update(void);

#endif /* __UI_TRAIN_H */
