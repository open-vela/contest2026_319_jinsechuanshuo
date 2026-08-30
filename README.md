# OpenVela 2026 硬件创新赛道 - BES2800BP (best1700_ep)

**团队**: 金色传说 (Team #319)
**目标平台**: BES2800BP (Cortex-M55 + HiFi4 DSP)
**开发板**: best1700_ep EVB

---

## 一、作品简介

基于 openvela NuttX RTOS，在已适配的 BES2800BP 平台上进行硬件创新开发，包括：

- ✅ SPI Flash FAT 文件系统集成
- ✅ IMU 传感器驱动 (BMI160)
- ✅ 预编译库适配方案
- ⏳ TinyML 手势识别 (规划中)
- ⏳ BLE 数据传输 (规划中)

---

## 二、选题方向

**AI 硬件产品创新**

基于 BES2800BP 平台的 IMU 传感器和 SPI Flash 存储，开发智能硬件应用，探索 TinyML 在嵌入式设备上的应用。

---

## 三、目录结构

```
contest2026_319_jinsechuanshuo/
├── README.md                    # 本文件
├── src/                         # 源码修改
│   ├── spi_flash_fatfs.c       # SPI Flash FAT 文件系统驱动
│   ├── spi_imu.c               # BMI160 IMU 传感器驱动
│   ├── ap.c                    # 板级初始化 (修改)
│   └── Kconfig                 # 配置选项 (修改)
├── config/                      # 配置文件
│   └── defconfig               # 板级默认配置
├── docs/                        # 文档
│   ├── modification-traceability.md  # 修改溯源文档
│   ├── developer-guide.md      # 开发者指南
│   └── ai-coding-log.md        # AI 编程日志
├── prebuilt/                    # 预编译产物
│   └── nuttx_ap.bin             # 编译固件 (1.4MB)
└── logs/                        # AI Coding 日志
    └── openureye/               # GitHub 用户名
        ├── manifest.json        # 会话清单
        └── <date>/              # 日期目录
            └── claude-code__*.jsonl  # 会话日志
```

---

## 四、运行方式

### 1. 环境准备

```bash
# 安装依赖
sudo apt install -y bison flex texinfo libncurses5-dev build-essential \
  cmake ninja-build python3 python3-serial picocom

# 配置串口权限
sudo usermod -aG dialout $USER
```

### 2. 拉取工程

```bash
# 拉取完整工程
repo init -u https://github.com/openureye/contest2026_319_jinsechuanshuo \
  -b dev-ai-contest-2026 -m contest2026_319_jinsechuanshuo.xml
repo sync -c -j8
```

### 3. 编译

```bash
# 进入 openvela 工作区根目录
cd ..

# 编译
./build.sh contest2026_319_jinsechuanshuo/board/contest_board/configs/nsh -j8
```

### 4. 烧录

```bash
# 使用 Linux 原生烧录工具
./vendor_bes/prebuild/m1/dldtool \
  -p /dev/ttyUSB0 \
  -b 921600 \
  -f nuttx/nuttx_ap.bin
```

### 5. 验证

```bash
# 连接串口
picocom -b 921600 /dev/ttyUSB0

# 在 NSH 中测试
nsh> ls /mnt/spiflash
nsh> echo "hello" > /mnt/spiflash/test.txt
nsh> cat /mnt/spiflash/test.txt
```

---

## 五、AI Coding 使用说明

本项目使用 Claude (Anthropic) 进行 AI 辅助开发，主要应用在：

1. **代码架构设计**: AI 提供了 SPI Flash → MTD → FTL → FAT 初始化流程的设计方案
2. **问题诊断**: AI 快速定位编译问题的根因，如预编译库适配、CONFIG_ALLSYMS 问题
3. **代码实现**: AI 生成了基础代码框架，人工优化错误处理和细节
4. **文档生成**: AI 自动生成了修改溯源文档和开发者指南

完整对话日志见 `logs/openureye/` 目录。

---

## 六、技术亮点

### 1. SPI Flash FAT 文件系统

- 使用 GD25 SPI Flash 驱动
- 通过 FTL 层实现 MTD 到块设备的转换
- 挂载点: `/mnt/spiflash`

### 2. BMI160 IMU 驱动

- 6 轴传感器 (加速度 + 陀螺仪)
- SPI 接口，100Hz 采样率
- 设备节点: `/dev/accel0`

### 3. 预编译库适配

- BES SDK 使用预编译库 (libbesboard_ap.a, libbeschip_ap.a, libnx_bestbsp_ap.a)
- 解决源码不可用的编译问题
- 保留配置头文件，移除源文件引用

---

## 团队信息

- **团队名称**: 金色传说
- **团队编号**: #319
- **GitHub**: [openureye/contest2026_319_jinsechuanshuo](https://github.com/openureye/contest2026_319_jinsechuanshuo)

---

## 许可证

本项目遵循 Apache License 2.0。
# test push
