############################################################################
# apps/examples/jinsechuanshuo_ballbes/Makefile
############################################################################

include $(APPDIR)/Make.defs

PROGNAME = jinsechuanshuo_ballbes
PRIORITY  = $(CONFIG_EXAMPLES_JINSECHUANSHUO_BALL_BES_PRIORITY)
STACKSIZE = $(CONFIG_EXAMPLES_JINSECHUANSHUO_BALL_BES_STACKSIZE)
MODULE    = $(CONFIG_EXAMPLES_JINSECHUANSHUO_BALL_BES)

MAINSRC = jinsechuanshuo_ballbes_main.c

CSRCS  = ui_common.c
CSRCS += ui_manager.c
CSRCS += ui_index.c
CSRCS += ui_stats.c
CSRCS += ui_train.c
CSRCS += imu_feeder.c
CSRCS += gl_tennis_22.c
CSRCS += gl_tennis_16.c

include $(APPDIR)/Application.mk
