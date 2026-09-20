---
name: lvgl-v9-round-ui
description: BES2800 454x454 圆形 OLED 上写 LVGL v9 UI 的硬规则与几何模板（源自多个同期参考工程的踩坑结论，本仓库实测同样适用）。
---

# LVGL v9 圆屏 UI 硬规则（BES2800 454x454）

适用范围：apps/examples/jinsechuanshuo_ballbes（金色传说网球手表 UI）。写/改 UI 前先读本文。

## 4 条硬规则

1. **动画回调签名不可混用**：`lv_anim_exec_xcb_t` 是 `(void *var, int32_t v)`，
   `lv_anim_custom_exec_cb_t` 是 `(lv_anim_t *a, int32_t v)`。强转前必须核对
   `apps/graphics/lvgl/lvgl/src/misc/lv_anim.h` 的原型，混用首个 tick 即堆损坏 crash。
2. **set_size 后不要立即相信 get_width()**：样式/坐标有缓存，读数可能 ≈0。
   显式把尺寸传给布局函数，或先 `lv_obj_update_layout()` 再读。
3. **圆屏安全区**：任何元素外接矩形的最远角点必须满足 `dx²+dy² ≤ (R-4)²`
   （R=227）。极坐标布局用 `ρ + 外接半对角线 ≤ R-4`。
   参考安全带：顶部弧 ρ≤0.72R (60–120°)、底部弧 240°/300°、左右弧 ±30° 起 ρ≤0.80R、
   标题 y=0.02–0.09S。禁用 BOTTOM_LEFT/BOTTOM_RIGHT 等矩形对齐于圆屏四角。
4. **LVGL 只允许主线程操作**：工作线程（IMU/TFLM）把结果写到 volatile/共享缓冲，
   由 UI 主循环的 lv_timer（200–500ms 轮询或 lvgl_dispatch 派发）消费后刷新。

## 已知反模式（我们仓库里要防）

- 在 `LV_EVENT_ALL` 回调里做重活（IO、malloc、长循环）——挪到 lv_timer。
- 页面切换后遗留 lv_timer（页面隐藏仍未 pause）——切换时显式 pause/resume。
- 大图用 ARGB8888 内嵌 C 数组：455×455 全幅 ≈ 800KB Flash，小图可用，
  大图改 `Filesystem`/`ARGB8565` 或从 /emmc 加载。
- 滚动控件与手势判定同方向共存：LVGL 滚动会吃掉手势事件。

## 参考实现（参考工程，只读勿改）

- 圆屏极坐标模板 + 粒子池：参考工程/contest2026_435_UdifyFun/app/apollia_hub/apollia_hub_main.c
- `lv_scr_load_anim` 页面切换模板：参考工程/contest2026_495_qinyunzhishang/app/lvgldemo/menu_page.c:47-105
- 三态预建页（发球前/倒计时/进行中）+ 树脂统计卡：参考工程/contest2026_392_dachuangwanlian/app/tswatch/exercise/src/exercise.c:174-253
