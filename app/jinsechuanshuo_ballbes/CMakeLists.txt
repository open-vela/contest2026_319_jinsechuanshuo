##############################################################################
# apps/examples/jinsechuanshuo_ballbes/CMakeLists.txt
#
# Elderly/Golden-Legend tennis themed LVGL watch app — cmake wiring mirrors
# apps/examples/hello (Kconfig handles the rest of the parameters).
#
##############################################################################

if(CONFIG_EXAMPLES_JINSECHUANSHUO_BALL_BES)
  nuttx_add_application(
    MODULE
    ${CONFIG_EXAMPLES_JINSECHUANSHUO_BALL_BES}
    NAME
    jinsechuanshuo_ballbes
    SRCS
    ui_common.c
    ui_index.c
    ui_manager.c
    ui_stats.c
    ui_train.c
    imu_feeder.c
    gl_tennis_22.c
    gl_tennis_16.c
    jinsechuanshuo_ballbes_main.c
    STACKSIZE
    ${CONFIG_EXAMPLES_JINSECHUANSHUO_BALL_BES_STACKSIZE}
    PRIORITY
    ${CONFIG_EXAMPLES_JINSECHUANSHUO_BALL_BES_PRIORITY})
endif()
