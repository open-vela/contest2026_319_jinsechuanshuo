/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/ui_index.c
 *
 * Golden Legend Tennis - home page.
 * Own style: big center scoreboard, gold ring speed gauge, serve bar.
 * Layout for 454x454 circular AMOLED display (BES2800).
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "ui_index.h"
#include "ui_manager.h"
#include "imu_feeder.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static lv_obj_t *g_speed_arc;      /* ball-speed ring gauge */
static lv_obj_t *g_speed_lbl;      /* gauge center value */
static lv_obj_t *g_score_lbl;      /* "30 : 15" */
static lv_obj_t *g_set_lbl;        /* "Set 1 · Game 3" */
static lv_obj_t *g_serve_lbl;
static lv_obj_t *g_conn_lbl;
static lv_obj_t *g_battery_lbl;

#define GAUGE_SIZE           170
#define SPEED_GAUGE_MAX      250      /* km/h range */
#define SERVE_GOAL_STR       "120"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 * Top status row: BT link, GL TENNIS title, battery.
 */

static lv_obj_t *create_status_bar(lv_obj_t *parent)
{
  lv_obj_t *bar = lv_obj_create(parent);
  lv_obj_set_size(bar, 280, 28);
  lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, SAFE_MARGIN - 22);
  lv_obj_set_style_bg_color(bar, COLOR_CARD_BG, 0);
  lv_obj_set_style_bg_opa(bar, LV_OPA_40, 0);
  lv_obj_set_style_radius(bar, 14, 0);
  lv_obj_set_style_border_width(bar, 0, 0);
  lv_obj_set_style_pad_all(bar, 0, 0);
  lv_obj_set_style_pad_hor(bar, 10, 0);
  lv_obj_remove_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

  /* Bluetooth status (own style: cyan when connected) */

  g_conn_lbl = lv_label_create(bar);
  lv_label_set_text(g_conn_lbl, LV_SYMBOL_LOOP " BT");
  lv_obj_set_style_text_color(g_conn_lbl, COLOR_CYAN, 0);
  lv_obj_set_style_text_font(g_conn_lbl, &lv_font_montserrat_14, 0);
  lv_obj_align(g_conn_lbl, LV_ALIGN_LEFT_MID, 0, 0);

  /* Center title (金色传说 TENNIS) */

  LV_FONT_DECLARE(gl_tennis_22);

  lv_obj_t *title = lv_label_create(bar);
  lv_label_set_text(title, "\xe9\x87\x91\xe8\x89\xb2\xe4\xbc\xa0\xe8\xaf\xb4 TENNIS");  /* 金色传说 TENNIS */
  lv_obj_set_style_text_color(title, COLOR_GOLD, 0);
  lv_obj_set_style_text_font(title, &gl_tennis_22, 0);
  lv_obj_set_style_text_letter_space(title, 3, 0);
  lv_obj_center(title);

  /* Battery */

  g_battery_lbl = lv_label_create(bar);
  lv_label_set_text(g_battery_lbl, LV_SYMBOL_BATTERY_FULL " 85%");
  lv_obj_set_style_text_color(g_battery_lbl, COLOR_TEXT_PRIMARY, 0);
  lv_obj_set_style_text_font(g_battery_lbl, &lv_font_montserrat_14, 0);
  lv_obj_align(g_battery_lbl, LV_ALIGN_RIGHT_MID, 0, 0);

  return bar;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

lv_obj_t *ui_index_create(lv_obj_t *parent)
{
  lv_obj_t *page = lv_obj_create(parent);
  lv_obj_set_size(page, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_opa(page, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(page, 0, 0);
  lv_obj_set_style_pad_all(page, 0, 0);
  lv_obj_remove_flag(page, LV_OBJ_FLAG_SCROLLABLE);

  create_status_bar(page);

  /* Ball speed ring gauge (gold, open at the bottom like a watch dial) */

  g_speed_arc = lv_arc_create(page);
  lv_arc_set_rotation(g_speed_arc, 135);
  lv_arc_set_bg_angles(g_speed_arc, 0, 270);
  lv_arc_set_range(g_speed_arc, 0, SPEED_GAUGE_MAX);
  lv_arc_set_value(g_speed_arc, 0);
  lv_obj_remove_style(g_speed_arc, NULL, LV_PART_KNOB);
  lv_obj_remove_flag(g_speed_arc, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(g_speed_arc, GAUGE_SIZE, GAUGE_SIZE);
  lv_obj_set_style_arc_color(g_speed_arc, COLOR_PROGRESS_BG, LV_PART_MAIN);
  lv_obj_set_style_arc_width(g_speed_arc, 14, LV_PART_MAIN);
  lv_obj_set_style_arc_color(g_speed_arc, COLOR_GOLD, LV_PART_INDICATOR);
  lv_obj_set_style_arc_width(g_speed_arc, 10, LV_PART_INDICATOR);
  lv_obj_align(g_speed_arc, LV_ALIGN_CENTER, 0, -38);

  /* Gauge range hints (0 and max at both ends) */

  lv_obj_t *lo = lv_label_create(page);
  lv_label_set_text(lo, "0");
  lv_obj_set_style_text_color(lo, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(lo, &lv_font_montserrat_14, 0);
  lv_obj_align_to(lo, g_speed_arc, LV_ALIGN_TOP_LEFT, 18, 40);

  lv_obj_t *hi = lv_label_create(page);
  lv_label_set_text(hi, "250");
  lv_obj_set_style_text_color(hi, COLOR_TEXT_DIM, 0);
  lv_obj_set_style_text_font(hi, &lv_font_montserrat_14, 0);
  lv_obj_align_to(hi, g_speed_arc, LV_ALIGN_TOP_RIGHT, -18, 40);

  /* Gauge center: value + unit */

  g_speed_lbl = lv_label_create(page);
  lv_label_set_text(g_speed_lbl, "0\nkm/h");
  lv_obj_set_style_text_color(g_speed_lbl, COLOR_TEXT_PRIMARY, 0);
  lv_obj_set_style_text_font(g_speed_lbl, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_align(g_speed_lbl, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(g_speed_lbl, LV_ALIGN_CENTER, 0, -38);

  /* Big scoreboard (budget: 48pt gold, letter-spaced) */

  g_score_lbl = lv_label_create(page);
  lv_label_set_text(g_score_lbl, "0 : 0");
  lv_obj_set_style_text_color(g_score_lbl, COLOR_GOLD, 0);
  lv_obj_set_style_text_font(g_score_lbl, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_letter_space(g_score_lbl, 8, 0);
  lv_obj_align(g_score_lbl, LV_ALIGN_CENTER, 0, 78);

  /* Sub line: set / game */

  g_set_lbl = lv_label_create(page);
  lv_label_set_text(g_set_lbl, "Set 1 \xc2\xb7"" Game 1");
  lv_obj_set_style_text_color(g_set_lbl, COLOR_CYAN, 0);
  lv_obj_set_style_text_font(g_set_lbl, &lv_font_montserrat_14, 0);
  lv_obj_align(g_set_lbl, LV_ALIGN_CENTER, 0, 122);

  /* Serves progress bar at the bottom */

  g_serve_lbl = lv_label_create(page);
  lv_label_set_text(g_serve_lbl, LV_SYMBOL_LOOP " 0/" SERVE_GOAL_STR
                     " serves");
  lv_obj_set_style_text_color(g_serve_lbl, COLOR_GOLD, 0);
  lv_obj_set_style_text_font(g_serve_lbl, &lv_font_montserrat_14, 0);
  lv_obj_align(g_serve_lbl, LV_ALIGN_BOTTOM_MID, 0, -SAFE_MARGIN + 10);

  /* Demo data first boot */

  app_context_t *ctx = app_get_context();
  ctx->match.ball_speed   = 118;
  ctx->match.scores[0]    = 30;
  ctx->match.scores[1]    = 15;
  ctx->match.serves       = 12;
  ctx->match.battery      = 85;
  ctx->network_connected  = true;

  return page;
}

void ui_index_update_vitals(void)
{
  app_context_t *ctx = app_get_context();
  char buf[64];

  /* speed ring (live IMU metric when the sensor is wired) */

  const struct imu_live_s *live = imu_feeder_live();
  int speed = live->live ? live->speed_kmh : ctx->match.ball_speed;

  snprintf(buf, sizeof(buf), "%d\nkm/h", (int)speed);
  lv_label_set_text(g_speed_lbl, buf);
  if (ctx->match.ball_speed > SPEED_GAUGE_MAX)
    {
      lv_arc_set_value(g_speed_arc, SPEED_GAUGE_MAX);
    }
  else
    {
      lv_arc_set_value(g_speed_arc, ctx->match.ball_speed);
      /* smooth needle (allowed change-rate keeps the sweep animated) */

      lv_arc_set_change_rate(g_speed_arc, 480);   /* units / second */
    }

  /* scoreboard */

  snprintf(buf, sizeof(buf), "%ld : %ld",
           (long)ctx->match.scores[0], (long)ctx->match.scores[1]);
  lv_label_set_text(g_score_lbl, buf);

  /* serve progress */

  snprintf(buf, sizeof(buf), LV_SYMBOL_LOOP " %ld/" SERVE_GOAL_STR
           " serves", (long)ctx->match.serves);
  lv_label_set_text(g_serve_lbl, buf);

  /* battery: red when below 30% */

  snprintf(buf, sizeof(buf), LV_SYMBOL_BATTERY_FULL " %ld%%",
           (long)ctx->match.battery);
  lv_label_set_text(g_battery_lbl, buf);
  lv_obj_set_style_text_color(g_battery_lbl,
      (ctx->match.battery <= 30) ? COLOR_RED : COLOR_TEXT_PRIMARY, 0);
}

void ui_index_update_steps(void)
{
  /* Steps no longer used; game/set labels borrowed from match context. */

  app_context_t *ctx = app_get_context();
  char buf[64];

  snprintf(buf, sizeof(buf), "Set %ld \xc2\xb7"" Game %ld",
           (long)ctx->match.serves_set[0] % 8,
           (long)ctx->match.points_total % 12);
  lv_label_set_text(g_set_lbl, buf);
}
