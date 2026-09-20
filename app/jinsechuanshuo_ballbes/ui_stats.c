/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/ui_stats.c
 *
 * Golden Legend Tennis - match data page (serves ring, per-set bars,
 * rally meters). Style: dark sport watch, gold + cyan accents.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "ui_stats.h"
#include "ui_manager.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static lv_obj_t *g_serve_arc;      /* total serve progress ring */
static lv_obj_t *g_serve_lbl;      /* ring center label */
static lv_obj_t *g_set_bar[3];     /* per-set serve bars */
static lv_obj_t *g_best_lbl;
static lv_obj_t *g_avg_lbl;
static lv_obj_t *g_won_lbl;

#define RING_SIZE            150
#define SERVE_RING_MAX       (SERVES_GOAL)
#define RALLY_SPEED_MAX      40

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 * Create a ring meter (arc) with dead plain background.
 */

static lv_obj_t *serve_ring(lv_obj_t *parent)
{
  lv_obj_t *arc = lv_arc_create(parent);
  lv_arc_set_rotation(arc, 270);
  lv_arc_set_bg_angles(arc, 0, 360);
  lv_arc_set_range(arc, 0, SERVE_RING_MAX);
  lv_arc_set_value(arc, 0);
  lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
  lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(arc, RING_SIZE, RING_SIZE);

  lv_obj_set_style_arc_color(arc, COLOR_PROGRESS_BG, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc, 12, LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc, COLOR_GOLD, LV_PART_INDICATOR);
  lv_obj_set_style_arc_width(arc, 10, LV_PART_INDICATOR);

  return arc;
}

/**
 * Create a small stats chip with a colored top accent bar.
 */

static lv_obj_t *stat_chip(lv_obj_t *parent, const char *name,
                            const lv_color_t accent, lv_obj_t **value_lbl)
{
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_set_size(card, 96, 62);
  lv_obj_set_style_bg_color(card, COLOR_CARD_BG, 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(card, 14, 0);
  lv_obj_set_style_border_width(card, 0, 0);
  lv_obj_set_style_pad_all(card, 4, 0);
  lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *chip = lv_obj_create(card);
  lv_obj_set_size(chip, 22, 3);
  lv_obj_align(chip, LV_ALIGN_TOP_MID, 0, 3);
  lv_obj_set_style_bg_color(chip, accent, 0);
  lv_obj_set_style_radius(chip, 2, 0);
  lv_obj_set_style_border_width(chip, 0, 0);

  lv_obj_t *n = lv_label_create(card);
  lv_label_set_text(n, name);
  lv_obj_set_style_text_color(n, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(n, &lv_font_montserrat_14, 0);
  lv_obj_align(n, LV_ALIGN_TOP_MID, 0, 8);

  lv_obj_t *v = lv_label_create(card);
  lv_label_set_text(v, "0");
  lv_obj_set_style_text_color(v, COLOR_TEXT_PRIMARY, 0);
  lv_obj_set_style_text_font(v, &lv_font_montserrat_22, 0);
  lv_obj_align(v, LV_ALIGN_BOTTOM_MID, 0, -6);

  *value_lbl = v;
  return card;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

lv_obj_t *ui_stats_create(lv_obj_t *parent)
{
  lv_obj_t *page = lv_obj_create(parent);
  lv_obj_set_size(page, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_opa(page, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(page, 0, 0);
  lv_obj_set_style_pad_all(page, 0, 0);
  lv_obj_remove_flag(page, LV_OBJ_FLAG_SCROLLABLE);

  /* Page title */

  lv_obj_t *ttl = lv_label_create(page);
  lv_label_set_text(ttl, LV_SYMBOL_LOOP " MATCH DATA");
  lv_obj_set_style_text_color(ttl, COLOR_GOLD, 0);
  lv_obj_set_style_text_font(ttl, &lv_font_montserrat_14, 0);
  lv_obj_align(ttl, LV_ALIGN_TOP_MID, 0, SAFE_MARGIN - 16);

  /* Serve ring */

  g_serve_arc = serve_ring(page);
  lv_obj_align(g_serve_arc, LV_ALIGN_CENTER, 0, -62);

  g_serve_lbl = lv_label_create(page);
  lv_label_set_text(g_serve_lbl, "0/120");
  lv_obj_set_style_text_color(g_serve_lbl, COLOR_TEXT_PRIMARY, 0);
  lv_obj_set_style_text_font(g_serve_lbl, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_align(g_serve_lbl, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(g_serve_lbl, LV_ALIGN_CENTER, 0, -62);

  /* Per-set serve bars */

  int sw = SCREEN_WIDTH - 2 * (SAFE_MARGIN - 30);
  for (int i = 0; i < 3; i++)
    {
      lv_obj_t *row = lv_obj_create(page);
      lv_obj_set_size(row, sw, 16);
      lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
      lv_obj_set_style_border_width(row, 0, 0);
      lv_obj_set_style_pad_all(row, 0, 0);
      lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);

      lv_obj_t *lbl = lv_label_create(row);
      lv_label_set_text_fmt(lbl, "S%d", i + 1);
      lv_obj_set_style_text_color(lbl, COLOR_TEXT_DIM, 0);
      lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
      lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);

      lv_obj_t *bx = lv_bar_create(row);
      lv_obj_set_size(bx, sw - 30, 10);
      lv_obj_align(bx, LV_ALIGN_RIGHT_MID, 0, 0);
      lv_bar_set_range(bx, 0, RALLY_SPEED_MAX);
      lv_obj_set_style_bg_color(bx, COLOR_PROGRESS_BG, 0);
      lv_obj_set_style_radius(bx, 5, 0);
      lv_obj_set_style_bg_color(bx,
          (i == 0) ? COLOR_GOLD : COLOR_CYAN, LV_PART_INDICATOR);
      lv_obj_set_style_radius(bx, 5, LV_PART_INDICATOR);
      lv_obj_set_style_pad_hor(row, 2, 0);

      lv_obj_align(row, LV_ALIGN_CENTER, 0, 40 + i * 22);
      g_set_bar[i] = bx;
    }

  /* Bottom chips: best rally / avg rally / points won */

  lv_obj_t *chk1 = stat_chip(page, "BEST", COLOR_GOLD, &g_best_lbl);
  lv_obj_t *chk2 = stat_chip(page, "AVG",  COLOR_BLUE, &g_avg_lbl);
  lv_obj_t *chk3 = stat_chip(page, "WON",  COLOR_CYAN, &g_won_lbl);
  lv_obj_align(chk1, LV_ALIGN_BOTTOM_MID, -110, -SAFE_MARGIN);
  lv_obj_align(chk2, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_align(chk3, LV_ALIGN_BOTTOM_MID, 96, -SAFE_MARGIN);

  return page;
}

void ui_stats_update(void)
{
  app_context_t *ctx = app_get_context();
  char buf[32];
  int total;
  int i;
  int avg;

  /* Serve ring */

  snprintf(buf, sizeof(buf), "%ld/%ld", (long)ctx->match.serves,
           (long)SERVE_RING_MAX);
  lv_label_set_text(g_serve_lbl, buf);
  lv_arc_set_value(g_serve_arc,
                   (ctx->match.serves > SERVE_RING_MAX) ? SERVE_RING_MAX
                                                      : ctx->match.serves);

  /* Per-set bars */

  total = 0;
  for (i = 0; i < 3; i++)
    {
      if (ctx->match.serves_set[i] > RALLY_SPEED_MAX)
        {
          lv_bar_set_value(g_set_bar[i], RALLY_SPEED_MAX, LV_ANIM_ON);
        }
      else
        {
          lv_bar_set_value(g_set_bar[i], ctx->match.serves_set[i],
                           LV_ANIM_ON);
        }
      total += ctx->match.serves_set[i];
    }

  if (total == 0)
    {
      for (i = 0; i < 3; i++)
        {
          ctx->match.serves_set[i] = 12 + i * 4;
        }

      ctx->match.rally_best = 14;
      ctx->match.points_won = 23;
      ctx->match.points_total = 40;
    }

  /* chips */

  snprintf(buf, sizeof(buf), "%ld", (long)ctx->match.rally_best);
  lv_label_set_text(g_best_lbl, buf);

  avg = 0;
  for (i = 0; i < 3; i++)
    {
      avg += ctx->match.serves_set[i];
    }

  avg = avg / 3;

  snprintf(buf, sizeof(buf), "%d", (int)avg);
  lv_label_set_text(g_avg_lbl, buf);

  snprintf(buf, sizeof(buf), "%ld/%ld", (long)ctx->match.points_won,
           (long)ctx->match.points_total);
  lv_label_set_text(g_won_lbl, buf);
}
