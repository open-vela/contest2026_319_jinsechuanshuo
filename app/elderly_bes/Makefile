############################################################################
# apps/examples/elderly_bes/Makefile
############################################################################

include $(APPDIR)/Make.defs

PROGNAME  = elderly
PRIORITY  = $(CONFIG_EXAMPLES_ELDERLY_BES_PRIORITY)
STACKSIZE = $(CONFIG_EXAMPLES_ELDERLY_BES_STACKSIZE)
MODULE    = $(CONFIG_EXAMPLES_ELDERLY_BES)

MAINSRC = elderly_main.c

CSRCS  = ui_common.c
CSRCS += ui_manager.c
CSRCS += ui_index.c

include $(APPDIR)/Application.mk
