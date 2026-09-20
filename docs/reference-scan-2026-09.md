# 参考工程调研汇总（2026-09）— 取长补短行动清单

来源：用户在 `/mnt/hgfs/share/bes/参考工程/` 新增的 4 个同期团队工程 + 官方参考 vela_2800bp。
方法：5 个并行只读调查，逐仓核对（本文件即结论沉淀）。

## 各队一句话 + 值得抄的点

| 队 | 作品 | 一句话 |
|---|---|---|
| 041 ArcherMind | 参考基线 | 官方最小改动路线：BES 官方板卡 + 一行 defconfig；烧录/manifest 结构与我们一致 |
| 392 大创万联 | tswatch 手表 | 静态 apps[] 表 + 轻页面栈 / exercise 三态页 + lv_arc 倒计时 / png2lvgl.py 图标管线 / 40ms 心电 lv_line |
| 416 电子农民工 | 朝夕 AI 管家 | **CJK 字体子集管线**（gen_cjk_fonts + 缺字构建闸门）/ ntc_patch.py 二进制补丁预案 / 烧录 SOP / DNS+LittleFS patch / UI↔Agent 用 /data 文件解耦 |
| 435 UdifyFun | Apollia 乐器 Hub | **lvgl-v9-round-ui 硬规则**（动画签名/尺寸缓存/圆屏角点 ρ+半对角≤R-4/单线程）/ 库符号取证三步法 / 能力边界表（BTH/USB 死路、APC1 可用）/ uart-midi 串口桥思路 / 模范设计文档 |
| 495 轻云之上 | 语音 AI 手表 | 页面懒创建 + lv_scr_load_anim / **lvgl_dispatch.c 跨线程派发器** / icons 徽标 .c+.png 成对 / FreeType 懒加载 CJK 字体 / SCREEN_UNLOADED 清理 |

对我们不可复用：416 的 AI agent 资产、392 的 emmc 镜像（非分区表）、495 的通话/会议页面（无传感器数据页）。

## 已落地（本轮）

1. `.claude/skills/lvgl-v9-round-ui/SKILL.md` —— 435 圆屏硬规则 + 反模式表 + 参考文件指针（本仓库已实测同款板，规则直接适用）。
2. `tools/flash_ap.sh` —— 烧录一键脚本（文件前置检查 + SYNC/RESET 提示 + 成功标志判定），默认 921600 稳档。

## 排期（按对得分价值排序）

- [ ] **P1 训练页（识别）**：借 392 exercise 三态页模板（ready→countdown→running）改造成网球版训练页；加到 tileview 作为第 3 页；数据源 MPU6500（`imu` app 已就绪）。
- [ ] **P2 跨线程派发器**：仿 495 `lvgl_dispatch.c`（环形队列 + 5ms lv_timer drain），为 IMU 采样线程 → UI 更新铺路，避免非 UI 线程碰 LVGL 的竞态。
- [ ] **P3 中文金色标签**：移植 416 的 gen_cjk_fonts（PS1→bash）+ check_cjk_coverage 闸门，用系统黑体子集，把"金色传说"等标签换成中文并加金色。前置：node + @lvgl/lv-font-converter（lv_font_conv）。
- [ ] **P4 网球图标**：jpg/png → LVGL C 数组管线（392 png2lvgl.py 思路，小图 ARGB8565/ARGB1565），网球/球拍/得分 icon。
- [ ] **P5 设计文档与 AI 日志**：按 435 docs/apollia_design.md 结构补我们的设计文档 + AI Coding 闭环记录（现象→假设→取证→修复→回归）。
- [ ] **P6 应急预案**：收编 416 ntc_patch.py + 烧录 SOP 到 docs/（启动 assert 循环时用）。
- [ ] （放后）DNS/LittleFS/网络：仅当引入 AI/联网功能时按 416 patches/0003 执行。
