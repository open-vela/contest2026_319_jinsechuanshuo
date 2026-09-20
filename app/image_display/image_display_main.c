/****************************************************************************
 * Contest 2026 - Image Display App
 * Team: 金色传说 (Team #319)
 * Platform: BES2800BP (best1700_ep)
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/boardctl.h>
#include <lvgl/lvgl.h>
#include <lvgl/demos/lv_demos.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#undef NEED_BOARDINIT

#if defined(CONFIG_BOARDCTL) && !defined(CONFIG_NSH_ARCHINIT)
#  define NEED_BOARDINIT 1
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: create_image_display
 *
 * Description:
 *   Create an image display using LVGL widgets
 *
 ****************************************************************************/

static void create_image_display(void)
{
  /* Set background color to black */

  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN);

  /* Create title label */

  lv_obj_t *title = lv_label_create(lv_screen_active());
  lv_label_set_text(title, "BES2800BP Image Display");
  lv_obj_set_style_text_color(title, lv_color_hex(0x00ff00), LV_PART_MAIN);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  /* Create a canvas for drawing */

  static lv_color_t buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(240, 180)];
  lv_obj_t *canvas = lv_canvas_create(lv_screen_active());
  lv_canvas_set_buffer(canvas, buf, 240, 180, LV_COLOR_FORMAT_NATIVE);

  /* Fill canvas with gradient */

  lv_canvas_fill_bg(canvas, lv_color_hex(0x1a1a2e), LV_OPA_COVER);

  /* Draw some shapes */

  lv_layer_t layer;
  lv_canvas_init_layer(canvas, &layer);

  /* Draw a blue rectangle */

  lv_draw_rect_dsc_t rect_dsc;
  lv_draw_rect_dsc_init(&rect_dsc);
  rect_dsc.bg_color = lv_color_hex(0x0f3460);
  rect_dsc.radius = 10;
  rect_dsc.border_color = lv_color_hex(0x16213e);
  rect_dsc.border_width = 2;

  lv_area_t coords;
  coords.x1 = 20;
  coords.y1 = 20;
  coords.x2 = 220;
  coords.y2 = 80;
  lv_draw_rect(&layer, &rect_dsc, &coords);

  /* Draw a red circle */

  lv_draw_rect_dsc_t circle_dsc;
  lv_draw_rect_dsc_init(&circle_dsc);
  circle_dsc.bg_color = lv_color_hex(0xe94560);
  circle_dsc.radius = LV_RADIUS_CIRCLE;

  coords.x1 = 90;
  coords.y1 = 100;
  coords.x2 = 150;
  coords.y2 = 160;
  lv_draw_rect(&layer, &circle_dsc, &coords);

  /* Draw text on canvas */

  lv_draw_label_dsc_t label_dsc;
  lv_draw_label_dsc_init(&label_dsc);
  label_dsc.color = lv_color_hex(0xffffff);
  label_dsc.font = &lv_font_montserrat_16;

  coords.x1 = 30;
  coords.y1 = 30;
  coords.x2 = 210;
  coords.y2 = 70;
  lv_draw_label(&layer, &label_dsc, &coords, "openvela BES2800BP", NULL);

  lv_canvas_finish_layer(canvas, &layer);

  /* Center the canvas */

  lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 20);

  /* Create bottom label */

  lv_obj_t *bottom = lv_label_create(lv_screen_active());
  lv_label_set_text(bottom, "Canvas Drawing Demo");
  lv_obj_set_style_text_color(bottom, lv_color_hex(0x888888), LV_PART_MAIN);
  lv_obj_align(bottom, LV_ALIGN_BOTTOM_MID, 0, -10);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main
 *
 * Description:
 *   Image display application - demonstrates canvas drawing on LCD
 *
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  lv_nuttx_dsc_t info;
  lv_nuttx_result_t result;

  printf("Image Display App - BES2800BP\n");
  printf("Initializing LVGL...\n");

  if (lv_is_initialized())
    {
      printf("LVGL already initialized!\n");
      return -1;
    }

#ifdef NEED_BOARDINIT
  /* Perform board-specific driver initialization */

  boardctl(BOARDIOC_INIT, 0);
#endif

  lv_init();

  lv_nuttx_dsc_init(&info);

#ifdef CONFIG_LV_USE_NUTTX_LCD
  info.fb_path = "/dev/lcd0";
#endif

#ifdef CONFIG_INPUT_TOUCHSCREEN
  info.input_path = "/dev/input0";
#endif

  lv_nuttx_init(&info, &result);

  if (result.disp == NULL)
    {
      printf("ERROR: Display initialization failed!\n");
      return 1;
    }

  printf("Display initialized successfully!\n");
  printf("Creating image display...\n");

  /* Create the image display */

  create_image_display();

  printf("Image displayed. Running for 30 seconds...\n");

  /* Run for 30 seconds */

  for (int i = 0; i < 3000; i++)
    {
      uint32_t idle = lv_timer_handler();
      idle = idle ? idle : 1;
      usleep(idle * 1000);
    }

  printf("Image display completed.\n");

  lv_nuttx_deinit(&result);
  lv_deinit();

  return 0;
}
