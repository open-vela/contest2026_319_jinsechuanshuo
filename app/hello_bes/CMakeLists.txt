# ##############################################################################
# apps/examples/hello_bes/CMakeLists.txt
#
# "Hello BES" example application — mirrors apps/examples/hello/CMakeLists.txt
#
# ##############################################################################

if(CONFIG_EXAMPLES_HELLO_BES)
  nuttx_add_application(
    MODULE
    ${CONFIG_EXAMPLES_HELLO_BES}
    NAME
    hello_bes
    SRCS
    hello_bes_main.c
    STACKSIZE
    2048
    PRIORITY
    100)
endif()
