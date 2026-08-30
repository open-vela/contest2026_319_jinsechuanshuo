# Contest 2026 team 319 board - src
nuttx_add_library(board)
target_sources(board PRIVATE
  board_boot.c
  ap.c
  spi_flash_fatfs.c
  spi_imu.c
)
