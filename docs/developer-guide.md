# BES2800BP (best1700_ep) 开发者指南

**更新日期**: 2026-08-27
**目标平台**: BES2800BP (Cortex-M55 + HiFi4 DSP)
**开发板**: best1700_ep EVB

---

## 1. 硬件概述

### 1.1 芯片规格

| 参数 | 规格 |
|------|------|
| CPU | Cortex-M55 @ 300MHz |
| DSP | HiFi4 @ 400MHz |
| Flash | 4MB Internal + 16MB External SPI Flash |
| RAM | 1MB SRAM |
| 无线 | Wi-Fi 6 + BLE 5.3 |
| 外设 | SPI, I2C, UART, PWM, ADC, USB, eMMC |

### 1.2 开发板外设

| 外设 | 型号 | 接口 | 设备节点 |
|------|------|------|----------|
| SPI Flash | GD25Q128 | SPI0 CS0 | /dev/mtdblock0 |
| IMU | BMI160 | SPI0 CS1 | /dev/accel0 |
| 串口 | - | UART0 | /dev/ttyUSB0 |

---

## 2. 开发环境搭建

### 2.1 系统要求

- Ubuntu 20.04/22.04/24.04 LTS
- 磁盘空间 >= 20GB
- 内存 >= 8GB (推荐 16GB)

### 2.2 依赖安装

```bash
sudo apt update
sudo apt install -y \
  bison flex texinfo libncurses5-dev libncursesw5-dev xxd \
  git gperf automake libtool pkg-config build-essential gperf genromfs \
  libgmp-dev libmpc-dev libmpfr-dev libisl-dev binutils-dev libelf-dev \
  libexpat1-dev gcc-multilib g++-multilib picocom u-boot-tools util-linux \
  python3 python3-pip python3-serial unionfs-fuse cmake ninja-build \
  wget curl unzip
```

### 2.3 串口权限

```bash
sudo usermod -aG dialout $USER
# 重新登录使权限生效
```

---

## 3. 代码获取

### 3.1 仓库结构

```
openvela/
├── nuttx/          # NuttX 内核源码
├── apps/           # 应用程序
├── external/       # 第三方库
├── vendor_bes/     # BES 厂商 SDK (本项目修改重点)
├── prebuilts/      # 预编译工具链
└── .repo/          # repo 工具元数据
```

### 3.2 初始化

```bash
# 初始化 repo
repo init -u ssh://git@github.com/open-vela/manifests.git \
  -b dev-ai-contest-2026 -m openvela.xml

# 同步代码
repo sync -j$(nproc)
```

---

## 4. 编译

### 4.1 加载配置

```bash
cd nuttx
make best1700_ep_ap_defconfig
```

### 4.2 配置选项

关键配置项 (已在 defconfig 中启用):

```
# SPI Flash FAT 文件系统
CONFIG_SPI_FLASH_FATFS=y
CONFIG_BES_SPI_FLASH_CSNUM=0
CONFIG_BES_SPI_FLASH_MOUNTPOINT="/mnt/spiflash"
CONFIG_FTL=y

# IMU 传感器
CONFIG_SPI_IMU=y
CONFIG_BES_SPI_IMU_CSNUM=1
CONFIG_BES_SPI_IMU_DEVNAME="/dev/accel0"
CONFIG_BMI160=y

# 基础外设
CONFIG_SPI=y
CONFIG_SPI_DRIVER=y
CONFIG_MTD=y
CONFIG_MTD_PARTITION=y
CONFIG_FS_FATFS=y
```

### 4.3 编译

```bash
make -j$(nproc)
```

编译产物:
- `nuttx_ap.bin` - 固件二进制 (约 1.4MB)
- `nuttx_ap.elf` - ELF 调试文件
- `nuttx_ap.map` - 链接映射文件

---

## 5. 烧录

### 5.1 硬件连接

```
开发板 Type-C 烧录口 → Linux 主机 USB 口
```

确认串口:
```bash
ls /dev/ttyUSB*
dmesg | grep tty
```

### 5.2 烧录命令

```bash
# 使用 Linux 原生烧录工具
DLDTOOL=vendor_bes/prebuild/m1/dldtool
chmod +x $DLDTOOL

# 烧录 AP 固件
$DLDTOOL -p /dev/ttyUSB0 -b 921600 -f nuttx/nuttx_ap.bin
```

### 5.3 烧录模式

进入烧录模式:
1. 按住开发板上的 BOOT 按钮
2. 按一下 RESET 按钮
3. 松开 BOOT 按钮
4. 执行烧录命令

---

## 6. 调试

### 6.1 串口连接

```bash
picocom -b 921600 /dev/ttyUSB0
```

### 6.2 NSH 命令

```bash
# 查看系统信息
nsh> uname -a
nsh> free

# 查看设备
nsh> ls /dev

# 测试 SPI Flash
nsh> ls /mnt/spiflash
nsh> echo "hello" > /mnt/spiflash/test.txt
nsh> cat /mnt/spiflash/test.txt

# 测试 IMU
nsh> cat /dev/accel0
```

---

## 7. 源码修改说明

### 7.1 新增文件

| 文件 | 说明 |
|------|------|
| `vendor_bes/boards/best1700_ep/aos_evb/src/spi_flash_fatfs.c` | SPI Flash FAT 文件系统初始化 |
| `vendor_bes/boards/best1700_ep/aos_evb/src/spi_imu.c` | BMI160 IMU 驱动初始化 |

### 7.2 修改文件

| 文件 | 修改内容 |
|------|----------|
| `vendor_bes/boards/best1700_ep/aos_evb/src/ap.c` | 添加设备初始化调用 |
| `vendor_bes/boards/best1700_ep/aos_evb/src/Make.defs` | 添加新文件编译规则 |
| `vendor_bes/boards/best1700_ep/common/src/Make.defs` | 移除源文件引用，使用预编译库 |
| `vendor_bes/boards/best1700_ep/aos_evb/configs/ap/defconfig` | 添加新配置项 |
| `vendor_bes/chips/bes/Kconfig` | 添加新配置选项 |

### 7.3 关键改动

#### Make.defs 改动

移除了所有 `CSRCS +=` 引用，改为使用预编译库:

```makefile
# NOTE: All CSRCS below are already compiled into prebuilt SDK libraries
# (libbesboard_ap.a, libbeschip_ap.a, libnx_bestbsp_ap.a).
# Source files are not available in this tree; the libraries are linked instead.
```

#### ap.c 改动

在 `board_late_initialize()` 中添加设备初始化:

```c
#ifdef CONFIG_SPI_FLASH_FATFS
  board_spi_flash_fatfs_initialize();
#endif

#ifdef CONFIG_SPI_IMU
  board_spi_imu_initialize();
#endif
```

---

## 8. 常见问题

### 8.1 编译错误: 找不到源文件

**原因**: Make.defs 中引用了不存在的源文件
**解决**: 移除 `CSRCS +=` 引用，使用预编译库

### 8.2 编译错误: CONFIG_ALLSYMS

**原因**: ALLSYMS 功能与预编译库冲突
**解决**: 在 .config 中禁用 `# CONFIG_ALLSYMS is not set`

### 8.3 烧录失败

**原因**: 未进入烧录模式
**解决**: 按住 BOOT → 按 RESET → 松开 BOOT → 烧录

### 8.4 SPI Flash 挂载失败

**原因**: SPI Flash 未正确初始化
**解决**: 检查 CONFIG_SPI=y, CONFIG_MTD=y 是否启用

---

## 9. 参考资源

- [OpenVela 官方文档](https://github.com/open-vela/docs)
- [NuttX 官方文档](https://nuttx.apache.org/docs/)
- [BES2800BP Datasheet](docs/BES2800BP-ZE7_Datasheet_v0.92.pdf)
- [BMI160 驱动源码](nuttx/drivers/sensors/bmi160.c)

---

**文档维护**: openureye team
**最后更新**: 2026-08-27
