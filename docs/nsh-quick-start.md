# NSH 快速上手指南

**NuttX Shell (NSH)** 是 NuttX RTOS 的命令行交互界面，类似于 Linux 的 Bash 或 Windows 的 CMD。

---

## 一、NSH 是什么？

NSH (NuttX Shell) 是 NuttX 实时操作系统内置的交互式命令行工具，提供：

- **命令执行** - 运行内置应用程序和系统命令
- **文件操作** - ls、cp、mv、rm、cat 等文件管理命令
- **系统信息** - 查看内存、任务、设备等系统状态
- **脚本支持** - 支持简单的 shell 脚本
- **环境变量** - 设置和使用环境变量

---

## 二、连接 NSH

### 1. 硬件连接

```
开发板 (USB) --> PC (串口终端)
```

### 2. 串口工具配置

| 参数 | 值 |
|------|-----|
| 波特率 | 921600 |
| 数据位 | 8 |
| 停止位 | 1 |
| 校验位 | None |
| 流控 | None |

### 3. 常用串口工具

**picocom (推荐)**
```bash
picocom -b 921600 /dev/ttyUSB0
```

**minicom**
```bash
minicom -D /dev/ttyUSB0 -b 921600
```

**PuTTY (Windows)**
- Connection type: Serial
- Serial line: COM3 (根据设备管理器查看)
- Speed: 921600

---

## 三、NSH 基础命令

### 1. 文件系统命令

```bash
# 列出文件
nsh> ls /
nsh> ls -la /mnt/spiflash

# 切换目录
nsh> cd /tmp

# 创建目录
nsh> mkdir /mnt/spiflash/test

# 删除文件
nsh> rm /mnt/spiflash/test.txt

# 复制文件
nsh> cp /etc/init.d/rcS /tmp/rcs_backup

# 查看文件内容
nsh> cat /etc/version

# 创建空文件
nsh> touch /mnt/spiflash/newfile.txt
```

### 2. 系统信息命令

```bash
# 查看任务列表
nsh> ps

# 查看内存使用
nsh> free

# 查看设备列表
nsh> ls /dev

# 查看系统版本
nsh> uname -a

# 查看运行时间
nsh> uptime
```

### 3. 网络命令

```bash
# 查看网络接口
nsh> ifconfig

# 测试网络连接
nsh> ping 8.8.8.8

# 查看路由表
nsh> route
```

### 4. 环境变量

```bash
# 查看所有环境变量
nsh> env

# 设置环境变量
nsh> export MY_VAR=hello

# 使用环境变量
nsh> echo $MY_VAR
```

---

## 四、运行应用程序

### 1. 列出可用应用

```bash
nsh> help
```

### 2. 运行内置应用

```bash
# 运行 hello 示例
nsh> hello

# 运行 LVGL 演示
nsh> lvgl_demo

# 运行带参数的应用
nsh> myapp arg1 arg2
```

### 3. 后台运行

```bash
# 后台运行应用
nsh> myapp &

# 查看后台任务
nsh> jobs
```

---

## 五、文件系统挂载

### 1. 查看已挂载文件系统

```bash
nsh> mount
```

### 2. 挂载 SPI Flash

```bash
# 挂载 FAT 文件系统
nsh> mount -t vfat /dev/smart0 /mnt/spiflash
```

### 3. 挂载 tmpfs

```bash
nsh> mount -t tmpfs tmpfs /tmp
```

---

## 六、调试命令

### 1. 查看系统日志

```bash
nsh> dmesg
```

### 2. 查看任务状态

```bash
nsh> ps aux
```

### 3. 查看内存详情

```bash
nsh> memdump
```

---

## 七、常用快捷键

| 快捷键 | 功能 |
|--------|------|
| Tab | 命令补全 |
| ↑/↓ | 浏览历史命令 |
| Ctrl+C | 终止当前命令 |
| Ctrl+Z | 挂起当前命令 |
| Ctrl+L | 清屏 |

---

## 八、BES2800BP 平台特定命令

### 1. SPI Flash 操作

```bash
# 查看 SPI Flash 设备
nsh> ls /dev/smart*

# 挂载 SPI Flash
nsh> mount -t vfat /dev/smart0 /mnt/spiflash

# 测试读写
nsh> echo "Hello BES" > /mnt/spiflash/test.txt
nsh> cat /mnt/spiflash/test.txt
```

### 2. IMU 传感器读取

```bash
# 查看 IMU 设备
nsh> ls /dev/accel*

# 读取 IMU 数据 (如果有测试命令)
nsh> imu_test
```

### 3. LCD 屏幕操作

```bash
# 查看 framebuffer 设备
nsh> ls /dev/fb*

# 运行 LVGL 演示
nsh> lvgl_demo
```

---

## 九、故障排除

### 1. 串口无响应

- 检查 USB 线是否连接
- 确认串口设备号 (`ls /dev/ttyUSB*`)
- 检查波特率设置是否正确
- 确认用户在 `dialout` 组中

### 2. 命令不存在

- 使用 `help` 查看可用命令
- 检查应用是否在 defconfig 中启用

### 3. 文件系统错误

- 检查设备节点是否存在 (`ls /dev`)
- 确认文件系统已正确挂载 (`mount`)
- 检查分区是否已格式化

---

## 十、示例：完整测试流程

```bash
# 1. 连接串口
picocom -b 921600 /dev/ttyUSB0

# 2. 查看系统信息
nsh> uname -a
nsh> ps

# 3. 测试文件系统
nsh> ls /
nsh> echo "test" > /tmp/test.txt
nsh> cat /tmp/test.txt

# 4. 测试 SPI Flash
nsh> mount -t vfat /dev/smart0 /mnt/spiflash
nsh> ls /mnt/spiflash
nsh> echo "BES2800BP" > /mnt/spiflash/hello.txt
nsh> cat /mnt/spiflash/hello.txt

# 5. 运行示例程序
nsh> hello
nsh> lvgl_demo

# 6. 查看系统状态
nsh> free
nsh> ps
```

---

## 参考资源

- [NuttX 官方文档](https://nuttx.apache.org/docs/)
- [NSH 命令参考](https://nuttx.apache.org/docs/latest/components/nsh.html)
- [BES2800BP 开发指南](./developer-guide.md)

---

**团队**: 金色传说 (Team #319)
**平台**: BES2800BP (best1700_ep)
**最后更新**: 2026-08-31
