/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/ui_manager.c
 *
 * Page manager: swipeable tileview with home / match-stats pages.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "ui_manager.h"
#include "ui_index.h"
#include "ui_stats.h"
#include "ui_train.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static lv_obj_t *g_layer;          /* Top-level layer for overlays */
static lv_obj_t *g_tileview;       /* swipe pager */
static lv_obj_t *g_pages[PAGE_COUNT];
static page_id_t g_current = PAGE_INDEX;

static bool g_tile_set;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void ui_manager_init(void)
{
  lv_obj_t *scr = lv_screen_active();

  /* Black background */

  lv_obj_set_style_bg_color(scr, COLOR_BG, 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
  lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

  /* Content layer */

  g_layer = lv_obj_create(scr);
  lv_obj_set_size(g_layer, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_center(g_layer);
  lv_obj_set_style_bg_opa(g_layer, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(g_layer, 0, 0);
  lv_obj_set_style_pad_all(g_layer, 0, 0);
  lv_obj_remove_flag(g_layer, LV_OBJ_FLAG_SCROLLABLE);

  /* Swipe pager: two pages side by side */

  g_tileview = lv_tileview_create(g_layer);
  lv_obj_set_size(g_tileview, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_opa(g_tileview, LV_OPA_TRANSP, 0);

  /* tile(0,0): scoreboard home; tile(1,0): match stats */

  g_pages[PAGE_INDEX] = ui_index_create(
      lv_tileview_add_tile(g_tileview, 0, 0, LV_DIR_HOR));
  g_pages[PAGE_TRAIN] = ui_train_create(
      lv_tileview_add_tile(g_tileview, 1, 0, LV_DIR_HOR));
  g_pages[PAGE_STATS] = ui_stats_create(
      lv_tileview_add_tile(g_tileview, 2, 0, LV_DIR_HOR));

  /* Start on the home page */

  g_current = PAGE_INDEX;
}

void ui_manager_switch_page(page_id_t page)
{
  if (page >= PAGE_COUNT)
    {
      return;
    }

  /* tileview handles the visual transition */

  static const int col[PAGE_COUNT] = { 0, 1, 2 };
  lv_tileview_set_tile_by_pos(g_tileview, col[page], 0, false);
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
  ui_index_update_vitals();
  ui_train_update();
  ui_stats_update();
}

lv_obj_t *ui_manager_get_layer(void)
{
  return g_layer;
}
