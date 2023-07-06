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
        - void sendData(const QString& data)
        
    - issues
        - when using git lfs, remember git lfs pull
        - QSqlDatabase and QSqlQuery guide, http://katecpp.github.io/sqlite-with-qt/
        - QSqlDatabasePrivate::database: requested database does not belong to the calling thread. 
        Remember to create QSqlDatabase in correct thread.

- 串口输出模块
    - 串口参数固定
    - 输入为gcode
    - 输出给写字机
    - 单独的线程

# OpenAI API
- 网络环境问题
    - 利用clash进行http代理
    - 将external-controller改为0.0.0.0:9090，即可通过局域网中的设备访问
- https://github.com/olrea/openai-cpp
    - libcurl
        - 交叉编译出armv7
        - https://www.jianshu.com/p/50c381b72bda
    - 解决Qt ssl版本兼容问题
        - https://www.howtoforge.com/tutorial/how-to-install-openssl-from-source-on-linux/
    - curl: (60) server certificate verification failed. CAfile: /etc/ssl/certs/ca-certificates.crt CRLfile: none
    More details here: https://curl.haxx.se/docs/sslcerts.html
    ```c++
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); // Temporarily disable peer verification for debugging purposes
    QSslConfiguration::setDefaultConfiguration(sslConfig);
    ```