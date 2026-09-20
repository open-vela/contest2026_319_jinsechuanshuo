/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/ui_train.c
 *
 * Golden Legend Tennis - TRAINING page.
 * Three pre-built states (ready / countdown / running) toggled by HIDDEN
 * flags, so screen switching costs nothing and timers follow page
 * lifecycle (start on RUNNING, stop on READY/exit).
 *
 * Style: dark sport watch, gold + cyan accents. Future: live IMU waveform
 * fed by MPU6500 samples, swings counter, TFLM swing labels.
 ****************************************************************************/

#include "ui_train.h"
#include "ui_manager.h"
#include "imu_feeder.h"

/****************************************************************************
 * Private Types/Data
 ****************************************************************************/

enum train_state_e
{
  TRAIN_READY = 0,
  TRAIN_COUNTDOWN,
  TRAIN_RUNNING,
};

static lv_obj_t *g_page;
static lv_obj_t *g_ready_box;      /* big START button */
static lv_obj_t *g_countdown_arc;
static lv_obj_t *g_countdown_lbl;
static lv_obj_t *g_running_box;
static lv_obj_t *g_swing_lbl;
static lv_obj_t *g_pace_lbl;
static lv_obj_t *g_time_lbl;

static enum train_state_e g_state = TRAIN_READY;
static lv_timer_t *g_tick;
static int g_countdown_left;
static int g_elapsed_100ms;

static int g_swing_count;
static int g_swing_base;
static int g_speed_sum;
static int g_speed_samples;

#define COUNTDOWN_SECONDS     3
#define TRAIN_TARGET_SWINGS   60
#define TICK_MS               200

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static lv_obj_t *stat_chip(lv_obj_t *parent, const char *name,
                            const lv_color_t accent)
{
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_set_size(card, 130, 76);
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

  return card;
}

static void train_set_state(enum train_state_e st)
{
  g_state = st;
}

static void show_state(enum train_state_e st)
{
  /* everything hidden first, then unhide the active panel */

  lv_obj_add_flag(g_countdown_arc, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(g_countdown_lbl, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(g_running_box, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(g_ready_box, LV_OBJ_FLAG_HIDDEN);

  switch (st)
    {
      case TRAIN_COUNTDOWN:
        g_countdown_left = COUNTDOWN_SECONDS;
        lv_label_set_text_fmt(g_countdown_lbl, "%d", g_countdown_left);
        lv_arc_set_value(g_countdown_arc, 0);
        lv_obj_remove_flag(g_countdown_arc, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(g_countdown_lbl, LV_OBJ_FLAG_HIDDEN);
        break;

      case TRAIN_RUNNING:
        g_elapsed_100ms = 0;
        g_speed_sum = 0;
        g_speed_samples = 0;
        g_swing_count = 0;
        g_swing_base = imu_feeder_live()->swings;
        lv_obj_remove_flag(g_running_box, LV_OBJ_FLAG_HIDDEN);
        break;

      case TRAIN_READY:
      default:
        lv_obj_remove_flag(g_ready_box, LV_OBJ_FLAG_HIDDEN);
        break;
    }

  train_set_state(st);
}

/* countdown/running tick, 200 ms */

static void train_tick_cb(lv_timer_t *t)
{
  (void)t;

  if (g_state == TRAIN_COUNTDOWN)
    {
      g_countdown_left--;
      if (g_countdown_left <= 0)
        {
          show_state(TRAIN_RUNNING);
        }
      else
        {
          lv_label_set_text_fmt(g_countdown_lbl, "%d", g_countdown_left);
        }
    }
  else if (g_state == TRAIN_RUNNING)
    {
      g_elapsed_100ms++;

      /* demo feed until the real IMU sampling thread is wired in */

      {
        app_context_t *ctx = app_get_context();

        g_speed_sum += ctx->match.ball_speed;
        g_speed_samples++;
      }

      lv_label_set_text_fmt(g_time_lbl, "%02ld:%02ld",
                            (long)(g_elapsed_100ms / 10 / 60),
                            (long)((g_elapsed_100ms / 10) % 60));
      if (g_swing_count < TRAIN_TARGET_SWINGS)
        {
          g_swing_count++;
        }

      lv_label_set_text_fmt(g_swing_lbl, "%d / %d",
                            g_swing_count, TRAIN_TARGET_SWINGS);
      lv_label_set_text_fmt(g_pace_lbl, "%d km/h",
                            (g_speed_samples > 0) ?
                            (int)(g_speed_sum / g_speed_samples) : 0);
    }
}

static void start_cb(lv_event_t *e)
{
  (void)e;
  show_state(TRAIN_COUNTDOWN);
}

static void stop_cb(lv_event_t *e)
{
  (void)e;
  show_state(TRAIN_READY);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

lv_obj_t *ui_train_create(lv_obj_t *parent)
{
  g_page = lv_obj_create(parent);
  lv_obj_set_size(g_page, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_opa(g_page, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(g_page, 0, 0);
  lv_obj_set_style_pad_all(g_page, 0, 0);
  lv_obj_remove_flag(g_page, LV_OBJ_FLAG_SCROLLABLE);

  /* Title */

  LV_FONT_DECLARE(gl_tennis_16);

  lv_obj_t *ttl = lv_label_create(g_page);
  lv_label_set_text(ttl, "\xe9\x87\x91\xe8\x89\xb2\xe4\xbc\xa0\xe8\xaf\xb4\xe8\xae\xad\xe7\xbb\x83");  /* 金色传说训练 */
  lv_obj_set_style_text_color(ttl, COLOR_GOLD, 0);
  lv_obj_set_style_text_font(ttl, &gl_tennis_16, 0);
  lv_obj_set_style_text_letter_space(ttl, 4, 0);
  lv_obj_align(ttl, LV_ALIGN_TOP_MID, 0, SAFE_MARGIN - 22);

  /* State 1: ready + big start button */

  g_ready_box = lv_obj_create(g_page);
  lv_obj_set_size(g_ready_box, 220, 220);
  lv_obj_center(g_ready_box);
  lv_obj_set_style_bg_opa(g_ready_box, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(g_ready_box, 0, 0);
  lv_obj_set_style_pad_all(g_ready_box, 0, 0);
  lv_obj_remove_flag(g_ready_box, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *start = lv_btn_create(g_ready_box);
  lv_obj_set_size(start, 160, 160);
  lv_obj_center(start);
  lv_obj_set_style_radius(start, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(start, COLOR_GOLD, 0);
  lv_obj_add_event_cb(start, start_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_remove_flag(start, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *start_lbl = lv_label_create(start);
  lv_label_set_text(start_lbl, LV_SYMBOL_PLAY "\nSTART");
  lv_obj_set_style_text_color(start_lbl, COLOR_BG, 0);
  lv_obj_set_style_text_font(start_lbl, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_align(start_lbl, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(start_lbl);

  /* State 2: 3-2-1 countdown, big arc */

  g_countdown_arc = lv_arc_create(g_page);
  lv_arc_set_rotation(g_countdown_arc, 270);
  lv_arc_set_bg_angles(g_countdown_arc, 0, 360);
  lv_arc_set_range(g_countdown_arc, 0, COUNTDOWN_SECONDS);
  lv_arc_set_value(g_countdown_arc, 0);
  lv_obj_remove_style(g_countdown_arc, NULL, LV_PART_KNOB);
  lv_obj_remove_flag(g_countdown_arc, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(g_countdown_arc, 170, 170);
  lv_obj_set_style_arc_color(g_countdown_arc, COLOR_PROGRESS_BG,
                             LV_PART_MAIN);
  lv_obj_set_style_arc_width(g_countdown_arc, 12, LV_PART_MAIN);
  lv_obj_set_style_arc_color(g_countdown_arc, COLOR_CYAN,
                             LV_PART_INDICATOR);
  lv_obj_set_style_arc_width(g_countdown_arc, 10, LV_PART_INDICATOR);
  lv_obj_align(g_countdown_arc, LV_ALIGN_CENTER, 0, 30);
  lv_obj_add_flag(g_countdown_arc, LV_OBJ_FLAG_HIDDEN);

  g_countdown_lbl = lv_label_create(g_page);
  lv_obj_set_style_text_color(g_countdown_lbl, COLOR_CYAN, 0);
  lv_obj_set_style_text_font(g_countdown_lbl, &lv_font_montserrat_48, 0);
  lv_obj_align(g_countdown_lbl, LV_ALIGN_CENTER, 0, 30);
  lv_obj_add_flag(g_countdown_lbl, LV_OBJ_FLAG_HIDDEN);

  /* State 3: running stats */

  g_running_box = lv_obj_create(g_page);
  lv_obj_set_size(g_running_box, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_opa(g_running_box, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(g_running_box, 0, 0);
  lv_obj_set_style_pad_all(g_running_box, 0, 0);
  lv_obj_remove_flag(g_running_box, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *chip1 = stat_chip(g_running_box, "SWINGS", COLOR_GOLD);
  lv_obj_t *chip2 = stat_chip(g_running_box, "PACE", COLOR_CYAN);
  lv_obj_align(chip1, LV_ALIGN_CENTER, -70, -50);
  lv_obj_align(chip2, LV_ALIGN_CENTER, 70, -50);

  g_swing_lbl = lv_label_create(chip1);
  lv_obj_set_style_text_color(g_swing_lbl, COLOR_TEXT_PRIMARY, 0);
  lv_obj_set_style_text_font(g_swing_lbl, &lv_font_montserrat_22, 0);
  lv_obj_align(g_swing_lbl, LV_ALIGN_BOTTOM_MID, 0, -8);

  g_pace_lbl = lv_label_create(chip2);
  lv_obj_set_style_text_color(g_pace_lbl, COLOR_TEXT_PRIMARY, 0);
  lv_obj_set_style_text_font(g_pace_lbl, &lv_font_montserrat_22, 0);
  lv_obj_align(g_pace_lbl, LV_ALIGN_BOTTOM_MID, 0, -6);

  g_time_lbl = lv_label_create(g_running_box);
  lv_label_set_text(g_time_lbl, "00:00");
  lv_obj_set_style_text_color(g_time_lbl, COLOR_TEXT_PRIMARY, 0);
  lv_obj_set_style_text_font(g_time_lbl, &lv_font_montserrat_28, 0);
  lv_obj_align(g_time_lbl, LV_ALIGN_CENTER, 0, 40);

  lv_obj_t *stop = lv_btn_create(g_running_box);
  lv_obj_set_size(stop, 140, 44);
  lv_obj_align(stop, LV_ALIGN_BOTTOM_MID, 0, -SAFE_MARGIN - 10);
  lv_obj_set_style_radius(stop, 22, 0);
  lv_obj_set_style_bg_color(stop, COLOR_CARD_BG, 0);
  lv_obj_set_style_border_color(stop, COLOR_RED, 0);
  lv_obj_set_style_border_width(stop, 2, 0);
  lv_obj_add_event_cb(stop, stop_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *stop_lbl = lv_label_create(stop);
  lv_label_set_text(stop_lbl, "STOP");
  lv_obj_set_style_text_color(stop_lbl, COLOR_RED, 0);
  lv_obj_set_style_text_font(stop_lbl, &lv_font_montserrat_16, 0);
  lv_obj_center(stop_lbl);

  lv_obj_add_flag(g_running_box, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(g_countdown_arc, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(g_countdown_lbl, LV_OBJ_FLAG_HIDDEN);

  /* lifecycle tick: started once, gates by state */

  g_tick = lv_timer_create(train_tick_cb, 200, NULL);

  show_state(TRAIN_READY);
  return g_page;
}

void ui_train_update(void)
{
  /* future: pull live MPU6500 derived values into chips here */
}
