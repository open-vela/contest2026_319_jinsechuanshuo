/****************************************************************************
 * Contest 2026 - Screen Test App
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

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#undef NEED_BOARDINIT

#if defined(CONFIG_BOARDCTL) && !defined(CONFIG_NSH_ARCHINIT)
#  define NEED_BOARDINIT 1
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main
 *
 * Description:
 *   Screen test application - displays text and basic graphics on LCD
 *
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  lv_nuttx_dsc_t info;
  lv_nuttx_result_t result;

  printf("Screen Test App - BES2800BP\n");
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
  printf("Displaying test pattern...\n");

  /* Set background color to dark blue */

  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

  /* Create title label */

  lv_obj_t *title = lv_label_create(lv_screen_active());
  lv_label_set_text(title, "BES2800BP Screen Test");
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_28, LV_PART_MAIN);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

  /* Create info label */

  lv_obj_t *info_label = lv_label_create(lv_screen_active());
  lv_label_set_text(info_label, "Team: 金色传说\nPlatform: best1700_ep\nLVGL Demo");
  lv_obj_set_style_text_color(info_label, lv_color_hex(0xcccccc), LV_PART_MAIN);
  lv_obj_set_style_text_font(info_label, &lv_font_montserrat_20, LV_PART_MAIN);
  lv_obj_align(info_label, LV_ALIGN_CENTER, 0, 0);

  /* Create a colored rectangle */

  lv_obj_t *rect = lv_obj_create(lv_screen_active());
  lv_obj_set_size(rect, 200, 100);
  lv_obj_set_style_bg_color(rect, lv_color_hex(0xff6b35), LV_PART_MAIN);
  lv_obj_set_style_radius(rect, 10, LV_PART_MAIN);
  lv_obj_align(rect, LV_ALIGN_BOTTOM_MID, 0, -50);

  /* Create rectangle label */

  lv_obj_t *rect_label = lv_label_create(rect);
  lv_label_set_text(rect_label, "openvela");
  lv_obj_set_style_text_color(rect_label, lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_center(rect_label);

  printf("Test pattern displayed. Running for 30 seconds...\n");

  /* Run for 30 seconds */

  for (int i = 0; i < 3000; i++)
    {
      uint32_t idle = lv_timer_handler();
      idle = idle ? idle : 1;
      usleep(idle * 1000);
    }

  printf("Screen test completed.\n");

  lv_nuttx_deinit(&result);
  lv_deinit();

  return 0;
}
