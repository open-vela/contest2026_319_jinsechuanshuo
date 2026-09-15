/****************************************************************************
 * apps/examples/elderly_bes/ui_manager.c
 *
 * Page manager: handles page creation, switching, and lifecycle.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "ui_manager.h"
#include "ui_index.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static lv_obj_t *g_pages[PAGE_COUNT];
static lv_obj_t *g_layer;     /* Top-level layer for overlays */
static page_id_t g_current = PAGE_INDEX;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void ui_manager_init(void)
{
  lv_obj_t *scr = lv_screen_active();

  /* Set black background */

  lv_obj_set_style_bg_color(scr, COLOR_BG, 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
  lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

  /* Create the content layer */

  g_layer = lv_obj_create(scr);
  lv_obj_set_size(g_layer, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_center(g_layer);
  lv_obj_set_style_bg_opa(g_layer, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(g_layer, 0, 0);
  lv_obj_set_style_pad_all(g_layer, 0, 0);
  lv_obj_remove_flag(g_layer, LV_OBJ_FLAG_SCROLLABLE);

  /* Create all pages */

  g_pages[PAGE_INDEX] = ui_index_create(g_layer);

  /* Hide all except the first page */

  for (int i = 0; i < PAGE_COUNT; i++)
    {
      if (i != PAGE_INDEX)
        {
          lv_obj_add_flag(g_pages[i], LV_OBJ_FLAG_HIDDEN);
        }
    }

  g_current = PAGE_INDEX;
}

void ui_manager_switch_page(page_id_t page)
{
  if (page >= PAGE_COUNT || page == g_current)
    {
      return;
    }

  /* Hide current page */

  lv_obj_add_flag(g_pages[g_current], LV_OBJ_FLAG_HIDDEN);

  /* Show target page */

  lv_obj_remove_flag(g_pages[page], LV_OBJ_FLAG_HIDDEN);

  g_current = page;
}

lv_obj_t *ui_manager_get_page(page_id_t page)
{
  if (page < PAGE_COUNT)
    {
      return g_pages[page];
    }

  return NULL;
}

void ui_manager_refresh(void)
{
  switch (g_current)
    {
      case PAGE_INDEX:
        ui_index_update_vitals();
        ui_index_update_steps();
        break;

      default:
        break;
    }
}

lv_obj_t *ui_manager_get_layer(void)
{
  return g_layer;
}
