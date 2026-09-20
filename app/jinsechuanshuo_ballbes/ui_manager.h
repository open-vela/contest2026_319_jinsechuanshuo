/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/ui_manager.h
 *
 * Page manager: handles page creation, switching, and lifecycle.
 ****************************************************************************/

#ifndef __UI_MANAGER_H
#define __UI_MANAGER_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "ui_common.h"

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * Initialize the UI manager and create all pages.
 */

void ui_manager_init(void);

/**
 * Switch to a different page.
 */

void ui_manager_switch_page(page_id_t page);

/**
 * Get a page object by ID.
 */

lv_obj_t *ui_manager_get_page(page_id_t page);

/**
 * Refresh the current page data.
 */

void ui_manager_refresh(void);

/**
 * Get the content layer object.
 */

lv_obj_t *ui_manager_get_layer(void);

#endif /* __UI_MANAGER_H */
