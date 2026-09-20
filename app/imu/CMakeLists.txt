##############################################################################
# apps/examples/imu/CMakeLists.txt
#
# "imu" BMI160 motion-sensor self-test builtin
#
##############################################################################

if(CONFIG_EXAMPLES_IMU)
  nuttx_add_application(
    MODULE
    ${CONFIG_EXAMPLES_IMU}
    NAME
    imu
    SRCS
    imu_main.c
    STACKSIZE
    2048
    PRIORITY
    100)
endif()
