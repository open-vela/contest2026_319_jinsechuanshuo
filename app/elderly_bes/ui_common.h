/****************************************************************************
 * apps/examples/elderly_bes/ui_common.h
 *
 * Common definitions for the Elderly Health Care LVGL application.
 * Colors, screen dimensions, shared data structures, and page IDs.
 ****************************************************************************/

#ifndef __UI_COMMON_H
#define __UI_COMMON_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <lvgl/lvgl.h>
#include <nuttx/config.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Screen dimensions (circular AMOLED display) */

#define SCREEN_WIDTH        CONFIG_EXAMPLES_ELDERLY_BES_SCREEN_WIDTH
#define SCREEN_HEIGHT       CONFIG_EXAMPLES_ELDERLY_BES_SCREEN_HEIGHT
#define SCREEN_RADIUS       (SCREEN_WIDTH < SCREEN_HEIGHT ? SCREEN_WIDTH / 2 : SCREEN_HEIGHT / 2)

/* Safe margin for circular display */

#define SAFE_MARGIN         50
#define CONTENT_WIDTH       (SCREEN_WIDTH - 2 * SAFE_MARGIN)

/* Color palette (dark theme for OLED) */

#define COLOR_BG            lv_color_hex(0x000000)
#define COLOR_CARD_BG       lv_color_hex(0x1A1A1A)
#define COLOR_CARD_BG2      lv_color_hex(0x222222)
#define COLOR_TEXT_PRIMARY   lv_color_hex(0xFFFFFF)
#define COLOR_TEXT_SECONDARY lv_color_hex(0xAAAAAA)
#define COLOR_TEXT_DIM       lv_color_hex(0x666666)
#define COLOR_RED            lv_color_hex(0xFF3B30)
#define COLOR_RED_DARK       lv_color_hex(0xCC2F26)
#define COLOR_GREEN          lv_color_hex(0x34C759)
#define COLOR_BLUE           lv_color_hex(0x007AFF)
#define COLOR_BLUE_DARK      lv_color_hex(0x0055CC)
#define COLOR_YELLOW         lv_color_hex(0xFFCC00)
#define COLOR_ORANGE         lv_color_hex(0xFF9500)
#define COLOR_PROGRESS_BG    lv_color_hex(0x333333)
#define COLOR_BORDER         lv_color_hex(0x333333)

/* Vital sign normal ranges */

#define HR_LOW              55
#define HR_HIGH             100
#define SPO2_LOW            95
#define TEMP_LOW            35.5f
#define TEMP_HIGH           37.3f
#define DAILY_STEPS_GOAL    6000

/* SOS countdown seconds */

#define SOS_COUNTDOWN_SEC   10

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

/* Page identifiers */

typedef enum
{
  PAGE_INDEX = 0,
  PAGE_COUNT
} page_id_t;

/* Vital sign data */

typedef struct
{
  int32_t  heart_rate;       /* bpm */
  int32_t  spo2;             /* percentage */
  float    temperature;      /* celsius */
  int32_t  steps;            /* daily step count */
  int32_t  battery;          /* battery percentage */
  bool     device_online;    /* device connection status */
  uint64_t timestamp;        /* last update timestamp (ms) */
} vital_data_t;

/* Application global context */

typedef struct
{
  vital_data_t      vitals;
  page_id_t         current_page;
  bool              network_connected;
} app_context_t;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* Global context accessor */

app_context_t *app_get_context(void);

/* Vital sign status helpers */

const char *get_hr_status(int32_t hr);
lv_color_t get_hr_status_color(int32_t hr);
const char *get_spo2_status(int32_t spo2);
lv_color_t get_spo2_status_color(int32_t spo2);
const char *get_temp_status(float temp);
lv_color_t get_temp_status_color(float temp);

#endif /* __UI_COMMON_H */
