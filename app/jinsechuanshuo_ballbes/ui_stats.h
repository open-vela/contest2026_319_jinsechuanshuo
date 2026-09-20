/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/ui_stats.h
 *
 * Golden Legend Tennis - stats page (serve history / rally meters).
 ****************************************************************************/

#ifndef __UI_STATS_H
#define __UI_STATS_H

#include "ui_common.h"

/* Create the stats page inside the tileview */

lv_obj_t *ui_stats_create(lv_obj_t *parent);

/* Refresh the stats page from the app context */

void ui_stats_update(void);

#endif /* __UI_STATS_H */
