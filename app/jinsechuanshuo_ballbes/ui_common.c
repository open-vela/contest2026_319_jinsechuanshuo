/****************************************************************************
 * apps/examples/jinsechuanshuo_ballbes/ui_common.c
 *
 * Common utility functions for the Golden Legend Tennis LVGL application.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "ui_common.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static app_context_t g_ctx;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

app_context_t *app_get_context(void)
{
  return &g_ctx;
}

const char *get_hr_status(int32_t hr)
{
  if (hr < HR_LOW)
    {
      return "Low";
    }
  else if (hr > HR_HIGH)
    {
      return "High";
    }

  return "Normal";
}

lv_color_t get_hr_status_color(int32_t hr)
{
  if (hr < HR_LOW || hr > HR_HIGH)
    {
      return COLOR_RED;
    }

  return COLOR_GREEN;
}

const char *get_spo2_status(int32_t spo2)
{
  if (spo2 < SPO2_LOW)
    {
      return "Low";
    }

  return "Normal";
}

lv_color_t get_spo2_status_color(int32_t spo2)
{
  if (spo2 < SPO2_LOW)
    {
      return COLOR_RED;
    }

  return COLOR_GREEN;
}

const char *get_temp_status(float temp)
{
  if (temp < TEMP_LOW)
    {
      return "Low";
    }
  else if (temp > TEMP_HIGH)
    {
      return "High";
    }

  return "Normal";
}

lv_color_t get_temp_status_color(float temp)
{
  if (temp < TEMP_LOW || temp > TEMP_HIGH)
    {
      return COLOR_RED;
    }

  return COLOR_GREEN;
}
