/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/ui_index.h
 *
 * Home/Index page: tennis match stats (Golden Legend theme).
 ****************************************************************************/

#ifndef __UI_INDEX_H
#define __UI_INDEX_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "ui_common.h"

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * Create the index/home page.
 */

lv_obj_t *ui_index_create(lv_obj_t *parent);

/**
 * Update the tennis stat cards (speed / rally / score).
 */

void ui_index_update_vitals(void);

/**
 * Update the serve progress bar.
 */

void ui_index_update_steps(void);

#endif /* __UI_INDEX_H */
