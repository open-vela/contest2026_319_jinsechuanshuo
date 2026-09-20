##############################################################################
# apps/examples/w25fs/CMakeLists.txt
#
# "w25fs" W25Q64 SPI flash filesystem demo — mirrors apps/examples/hello
#
##############################################################################

if(CONFIG_EXAMPLES_W25FS)
  nuttx_add_application(
    MODULE
    ${CONFIG_EXAMPLES_W25FS}
    NAME
    w25fs
    SRCS
    w25fs_main.c
    STACKSIZE
    2048
    PRIORITY
    100)
endif()
