/****************************************************************************
 * apps/examples/elderly_bes/ui_index.h
 *
 * Home/Index page: vital signs cards, step progress.
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
 * Update vital sign display values.
 */

void ui_index_update_vitals(void);

/**
 * Update step count display.
 */

void ui_index_update_steps(void);

#endif /* __UI_INDEX_H */
