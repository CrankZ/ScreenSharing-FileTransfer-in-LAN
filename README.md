# ScreenSharing-FileTransfer-in-LAN
## 基于

Qt 5.11.1 (MSVC 2015, 32bit)

Python 3.4.3

## 简介

程序分为 Master端，Slave端。

Master主界面

Slave端没有界面，启动后会有托盘图标

## 主要功能

### 1：屏幕演示（延迟1s以内）

采用 **局域网组播**+**分包传输**+**屏幕差异对比** 等技术

### 2：屏幕监控（可同时监控4台）

### 3：文件传输（速度可达局域网最大速度）

文件传输功能为python实现，主要原理为局域网内搭建HTTP服务器，slave端通过下载方式完成传输操作。

### 4：学生签到

### 5：考试功能（详见：）

## 其他

开机自启

防多开

UAC管理员权限

屏幕演示时，slave端禁止任何操作（鼠标、键盘）

## Thanks

https://github.com/stachenov/quazip

https://zlib.net/