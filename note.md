# 连接wifi
source /home/root/shell/wifi/alientek_usb_wifi_setup.sh -m station -i Xiaomi_BB93 -p 15659224879 -d wlan0

# ChatGPT Writer
- UI
    - 输入为unicode字符
    - 输出为写字机返回值
    - 展示串口状态

- 核心模块
    - 维护写字机当前状态（笔的坐标）
    - 负责将unicode字符转化为gcode
        - 接收unicode
        - 查数据库得到相应的字符的gcode
        - 结合写字机当前状态组合出正确的一段gcode
    - 输入unicode字符
    - GcodeGenerator
        - 

- 串口输出模块
    - 串口参数固定
    - 输入为gcode
    - 输出给写字机
    - 单独的线程