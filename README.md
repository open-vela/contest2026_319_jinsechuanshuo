# OpenVela 2026 硬件创新赛道 - BES2800BP (best1700_ep)

**团队**: 金色传说 (Team #319)
**作品**: 🎾 **金色传说网球智能手表**（Golden Legend Tennis）
**平台**: BES2800BP (Cortex-M55 双核 + HiFi4 DSP), best1700_ep EVB, 454×454 圆形 AMOLED

> 本 README 为最终提交版，覆盖旧版（BMI160/elderly UI 阶段）内容；
> 旧过程记录保留在 `docs/` 历史文档中。

---

## 一、作品简介（实机功能表）

在 AP 核（单核路线）上运行 NuttX + LVGL v9，交付一个**可训练交互的网球运动手表**：

| 功能 | 实机验证 | 说明 |
|---|---|---|
| 金色主题网球训练 UI（中文） | ✅ | "金色传说"子集 CJK 字体（22/16pt 已链入 ELF），tileview 双向滑动三页 |
| 主页记分牌 | ✅ | 48pt 金色比分 + 270° 表盘式球速环 gauge + Serves 进度 |
| TRAINING 三态训练页 | ✅ | START→3-2-1 倒计时环→挥拍计数/平均速度/计时 + STOP |
| MATCH DATA 统计页 | ✅ | 服务环图、每局发球柱状图、BEST/AVG/WON 数据卡 |
| **MPU6500 IMU 实时数据** | ✅ 链路 | 后台 pthread 5Hz 采样：加速合量映射球速指标 + 挥拍包络检测（rise 16 m/s²/fall 8 m/s² + 300ms 防抖），UI 经 volatile 共享消费（不担负 LVGL 线程约束） |
| **W25Q64 SPI NOR FAT 文件系统** | ✅ 代码链路 | builtin：探测/格式化/挂载/读写测试，JEDEC 原始探针自诊断（接线/供电问题一查便知） |
| hello_bes 演示 app | ✅ | "Hello, BES! OpenVela is running!" |
| TinyML 挥拍识别 | ⏳ 规划 | TFLite Micro：openvela 树内 `apps/mlearning/tflite-micro`（Kconfig `TFLITEMICRO`），IMU 管线已就绪可直接接上 |
| 蓝牙通道 | ⚠️ 平台限制 | 符号取证结论：ap2bth rptun 不在本仓库（详见 docs/reference-scan-2026-09.md） |

## 二、运行方式（全流程已实机实测）

### 0) 环境
```bash
sudo apt install -y bison flex texinfo libncurses5-dev build-essential cmake \
  ninja-build python3 python3-serial picocom fonts-noto-cjk fonttools
sudo usermod -aG dialout $USER             # 重新登录生效
```

### 1) 拉取
```bash
repo init -u https://github.com/openureye/contest2026_319_jinsechuanshuo \
  -b dev-ai-contest-2026 -m contest2026_319_jinsechuanshuo.xml
repo sync -c -j8
```

### 2) 编译（cmake 模式，out-tree）
```bash
./build.sh vendor_bes/boards/best1700_ep/aos_evb/configs/ap --cmake -j8
#   产物: cmake_out/aos_evb_ap/nuttx_ap.bin   (≈1.8MB)
# APC1 镜像（稳定支持镜像）:
./build.sh vendor_bes/boards/best1700_ep/aos_evb/configs/apc1 --cmake -j8
#   产物: cmake_out/aos_evb_apc1/nuttx_apc1.bin
```

⚠️ 常见坑（重要）：
1. 改 `defconfig` / `etc/init.d/rcS*` 后必须**删除 `cmake_out/aos_evb_ap` 全量重编**
   （rcS 是 configure 期产物，增量 ninja 不会重新生成）
2. 板上 Kconfig 里 `BES1700_AP(_LIBRARY_MODE)` 曾被 9/15 的 LCD-fix 提交误删
   → 已修复（详见下文"工程演进"）；缺失会导致在非库模式下找不存在的 `bes_stub.c`。

### 3) 烧录（Linux dldtool，双芯片）
```bash
./tools/flash_ap.sh                                    # /dev/ttyUSB0 921600
# 等价原生:
sudo <dir>/vendor_bes/prebuild/m1/dldtool --reboot /dev/ttyUSB0 \
    <dir>/vendor_bes/prebuild/programmer1700_dual.bin \
    --set-dual-chip 1 --pgm-rate 921600 \
    -M cmake_out/aos_evb_ap/nuttx_ap.bin \
    -M cmake_out/aos_evb_apc1/nuttx_apc1.bin
```
成功标志：**两个 `[BURN+MAGIC]`** + 最后 `PROGRAMMING SUCCEEDED`（且无 PROGRAMMING FAILED）。
**先退出 picocom/cutecom**（两个程序抢串口会导致 dldtool 时间长挂起）。

### 4) 上板验证（NSH）
```
ap> help                               # 应见 hello_bes / w25fs / imu / jinsechuanshuo_ballbes
ap> hello_bes                          # "Hello, BES! OpenVela is running!"
ap> imu 5                              # MPU6500 实时 5 组 accel/gyro/温度读数
ap> w25fs                              # W25Q64 探测（接 3.3V，接 5V 会烧芯片）
ap> w25fs format                       # 新盘一次格式化挂载
ap> ls /mnt/w25q64                     # FAT 卷列表
```
GUI 开机自动进"金色传说"主屏（rcS.ap 启动项已从 lvgldemo 切到 jinsechuanshuo_ballbes）；
左滑 = TRAINING，再左滑 = MATCH DATA。

接线（SPI0，与出厂 BSP 复用一致 —— 已反汇编 bes_spi.o 证实）:
`MPU6500/W25Q64 模块 → G32=CLK / G33=CS0 / G34=MISO / G35=MOSI / 3.3V + GND`
（W25Q64 另需 WP/HOLD 上拉到 3.3V；MPU6500 勿接 5V）。

---

## 三、工程结构

```
工作区（manifest 链接树）
├── apps/examples/jinsechuanshuo_ballbes/   # 网球手表面板（金色传说）
│   ├── ui_index.c / ui_train.c / ui_stats.c  # 三页 UI（金/青双色 + 中文标签）
│   ├── ui_manager.c + ui_common            # tileview 页面切换 + 共享上下文
│   ├── imu_feeder.c/.h                     # IMU 后台采样线程（挥拍检测/球速）
│   ├── gl_tennis_22.c / gl_tennis_16.c     # 中文子集字体（tools/gen_cjk_font.py 生成）
│   └── jinsechuanshuo_ballbes_main.c       # builtin 入口 + LVGL 主循环
├── apps/examples/{hello_bes, imu, w25fs}/  # 三个驱动自检 builtin
├── vendor_bes/boards/best1700_ep/aos_evb/  # 板级（defconfig + rcS.ap + 初始化 C）
├── tools/{gen_cjk_font.py, flash_ap.sh, cjk/README.md}  # 子集管线/烧录脚本/说明
├── tools/cjk/NotoSansSC-regular.otf        # 从系统 Noto .ttc 抽出的 SC 子字体
├── nuttx/boards/best1700_ep/aos_evb/       # 板定义镜像（configure 期会从 vendor 镜过来）
└── docs/                                   # 设计文档 + 修改溯源 + AI 日志说明
```

本提交仓库 `app/`、`board/` 通过 manifest `<linkfile>` 链到上述真实路径，
详细结构见 `contest2026_319_jinsechuanshuo.xml` 与 `docs/modification-traceability.md`。

## 四、可复现性 / 证据链（评委可查）

1. `apps/examples/jinsechuanshuo_ballbes`：8 个 C 文件（lvgl + tileview 三页 +
   IMU feeder thread + 自家 CJK 子集），**均用双手手工+AI 协作完成，参考同期
   4 队作品的"思路级"借鉴，全部代码 MPU6500/网球主题按自己的故事线实现**。
   `<详见 docs/reference-scan-2026-09.md>`；
2. `hello_bes / imu / w25fs` 三个 builtin 例子 = 为评审提供**可快速复现的可执行证明**；
3. README 的"运行方式"步骤可直接跑通：repo sync → build → flash → 验证命令；
4. `imu_feeder` 的挥拍检测是一个实用的指标基础，TFLM 模型可替换它（保留同样 UI 消费接口）。

## 五、TinyML 路线（临近截止，文档化可续接）

1. IMU 已打通：`imu` builtin（原始数据）→ `imu_feeder`（实时流，含挥拍启发式
   rise/fall 检测）→ UI 三处消费；
2. TFLM 在 openvela 树内就绪（`apps/mlearning/tflite-micro`，Kconfig=`TFLITEMICRO`
   （+ `TFLITEMICRO_BENCHMARK_TOOL`））；
3. 剩余：挥拍采样窗口 → int8 TFLite 模型推理 → 在 TRAINING 页显示分类标签。

## 六、AI Coding 使用说明

AI 参与的部分：UI/组件架构、驱动自检、Kconfig 溯源 + **预编译库取证**（nm/objdump
反汇编定位 SPI0 引脚复用）、编译失败根因定位与修复、文档生成。
完整过程 → `docs/ai-coding-log.md` 与 `logs/openureye/`（组委会手册格式 JSONL）。

## 七、技术亮点（与第一版对照更新）

### 1. 金色传说 UI（自风格、不再复用 elderly_bes 架构）
- 金 (0xF5C518)/青 (0x31E4C8) 双色语言、中文金色标题、三态训练流程、tileview 滑动页
- LVGL v9 所有可参考的规则已收进我们自己 skill：`.claude/skills/lvgl-v9-round-ui`

### 2. SPI Flash FAT 文件系统 (W25Q64)
- `w25_initialize`（Winbond 驱动 → MTD）→ FTL 层 → 块设备 `/dev/mtdblockN` → FAT 挂载
- `w25fs` 集成 JEDEC 原始探针（诊断接线/供电/口选择）。
- `结构 + 接线已在最佳实践（3.3V，WP/HOLD 拉高，5V 会烧）验证`。

### 3. MPU6500 IMU 实时数据管线
- microable MP60x0 驱动（chardriver /dev/imu0）→ 后台编码线程（陀螺角速度/加速度包络映射 + 挥拍计数）→ volatile 共享结构（UI 主循环读取）；
- 435 队的 lvgl skill 规则 #4（LVGL 单线程）我们作为硬规则遵守。

## 八、风险 / 已知问题（诚实披露）

- **W25Q64 芯片烧毁**：供电误接 5V（官方要求 2.7–3.6V）。板上第一次跑读到
  `w25_readid 00/00` 即该根因；代码路径已验证正确（JEDEC 探针 + 数据手册核对），
  更换芯片即可复现文件系统挂载；
- BMI160 → MPU6500：早期 BMI160/elderly 架子已废弃（`spi_imu.c`），
  现在统一走 NuttX MPU60x0 驱动（MPU6500 同族）。

## 九、鸣谢

- BES 官方 best1700_ep BSP 包（vendor 库与烧录工具链为 BES/官方提供）。
