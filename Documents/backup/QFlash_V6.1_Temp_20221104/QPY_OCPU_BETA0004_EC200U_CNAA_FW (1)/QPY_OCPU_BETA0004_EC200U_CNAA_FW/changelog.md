## Release History：
**[QPY_OCPU_BETA0004_EC200U_CNAA_FW] 2024-03-11**

* ZN
* 1、合入驱动修改点
* 2、FOTA下载未返回0前，断电开机，重新进行FOTA升级期望正常返回0
* 3、增加EC200U流控配置
* 4、增加W5500功能支持
* 5、解决W5500在终端模式下无法进行DNS解析的问题
* 6、解决8910平台读取短信转储的问题
* 7、解决展锐平台8910的热插拔功能，断电不保存的问题


* EN
* 1. Updated the USB drive to repl interfaces
* 2. If the FOTA download does not return 0, power off the device and restart the FOTA upgrade. Expect the FOTA upgrade to return 0
* 3. Added EC200U flow control configuration
* 4. Enabled W5500 function support
* 5. Resolved the problem that W5500 cannot perform DNS resolution in terminal mode
* 6. Resolved the problem that 8910 platform reads SMS dump
* 7. Resolved the hot swap function of Unisoc 8910, the problem of not saving power


**[QPY_OCPU_V0003_EC200U_CNAA_FW] 2023-06-14**

* ZN
* 1、优化CH395网络速率
* 2、新增dhcp server功能
* 3、修复USBnet开启之后，实网IP获取显示错误的问题
* 4、增加耳机检测机制（仅支持EC600U）
* 5、解决烧录版本后，首次以默认ECM模式open后，usbnet.open返回值不正确的问题
* 6、解决camera预览异常问题
* 7、解决SD卡格式化之后创建和之前相同name的文件失败的问题
* 8、解决关机闹钟概率性失效问题
* 9、解决获取不到IPV6地址的问题
* 10、解决关闭热插拔功能时返回-1的问题
* 11、修复net.setApn;dataCall.setPDPContext设置用户名密码为空字符串时，设置后不生效问题
* 12、修复初始化网卡后，重新拨号后，DNS解析失败问题
* 13、修复GSM网络下出现net.getSignal查询rssi=0的情况
* 14、修复执行recv过程中，close socket导致dump的问题
* 15、解决使用atcmd模块接口发送AT概率性发生dump问题
* 16、修复SD卡不close直接remove导致空间泄露的问题


* EN
* 1. Optimize the network rate of the CH395
* 2. Added the dhcp server function
* 3. Fixed an error in obtaining real network IP after USBnet is enabled
* 4. Added headset detection mechanism (EC600U only)
* 5. Fixed an issue where the usbnet.open return value was incorrect after the first open in default ECM mode after burning the version
* 6. Resolved the camera preview exception
* 7. Resolved an issue where a file with the same name as before failed to be created after SD card formatting
* 8. Resolve the problem that the shutdown alarm is likely to fail
* 9. Solve the problem that the IPV6 address cannot be obtained
* 10. Resolved the problem that -1 is returned when the hot swap function is disabled
* 11. Fixed net.setApn; dataCall.setPDPContext Does not take effect after the user name and password are set to empty strings
* 12. Fixed DNS resolution failure after NIC initialization and dial-up again
* 13. Fix the situation in which net.getSignal queries rssi=0 on the GSM network
* 14. Fixed an issue where the socket was closed during recv execution, causing dump
* 15. Solve the problem that probability dump occurs when atcmd is used to send ats
* 16. Fixed space leakage caused by removing SD card without close



**[QPY_OCPU_V0002_EC200U_CNAA_FW] 2022-12-26**

* ZH
* 1、增加一路串口UART4
* 2、解决 USBNET 功能开启后无法使用网络问题
* 3、解决MIPI屏在拔掉USB后会花屏的问题
* 4、修复SD卡初始化失败后再次初始化阻塞20s的问题
* 5、添加对CH395Q网卡的支持
* 6、修复流播内存泄漏问题
* 7、当net_mode和net_status引脚作为GPIO使用时，可以禁止网络灯功能
* 8、增加通过4线SPI挂载NOR Flash作为文件系统的功能
* 9、修复wifiScan扫描热点超过30个时出现dump或者卡住的问题
* 10、解决流播时，AMR不传头文件播放失败的问题
* 11、解决TTS播放接口会阻塞160ms左右的问题
* 12、添加参数用于控制MIPI RST极性
* 13、用户可配置开pa时发送的脉冲数
* 14、socket功能优化
* 15、增加webserver功能支持
* 16、解决LVGL运行时，按CTRL+D重启虚拟机导致dump的问题
* 17、解决LVGL影响休眠的问题
* 18、修复queue可能导致死锁的问题
* 19、USBNET模块增加NAT设置和查询功能
* 20、增加uPing和uwebsocket功能
* 21、增加SPI驱动LCD接口
* 22、外挂GNSS数据解析模块性能优化
* 23、checkNet功能模块代码优化
* 24、request模块增加对HTTP1.1版本支持
* 25、阿里云增加对ssl连接的支持
* 26、machine lcd模块增加ISINK背光控制引脚背光挡位控制接口lcd_level_brightness
* 27、修复ussl 双认证密钥错误导致握手失败后重启虚拟机导致dump的问题
* 28、ntptime模块对时接口settime增加一个参数use_rhost让用户设置是否使用备用NTP服务器地址
* 29、machine uart 模块新增read_once()/write_once()接口
* 30、umqtt重连功能优化

* EN
* 1. Add a serial port UART4
* 2. Solve the problem that the network cannot be used after the USBNET function is turned on
* 3. Solve the problem that the MIPI screen will be blurred after unplugging the USB
* 4. Fix the problem that the initialization of the SD card is blocked for 20s after the initialization fails
* 5. Add support for CH395Q network card
* 6. Fix streaming memory leak problem
* 7. When the net_mode and net_status pins are used as GPIO, the network light function can be disabled
* 8. Added the function of mounting NOR Flash as a file system via 4-wire SPI
* 9. Fix the dump or stuck problem when wifiScan scans more than 30 hotspots
* 10. Solve the problem that AMR fails to play the header file when streaming
* 11. Solve the problem that the TTS playback interface will be blocked for about 160ms
* 12. Add parameters to control MIPI RST polarity
* 13. The user can configure the number of pulses sent when the pa is turned on
* 14. Socket function optimization
* 15. Add webserver function support
* 16. Solve the problem of dump caused by pressing CTRL+D to restart the virtual machine when LVGL is running
* 17. Solve the problem that LVGL affects sleep
* 18. Fix the problem that queue may cause deadlock
* 19. The USBNET module adds NAT setting and query functions
* 20. Add uPing and uwebsocket functions
* 21. Add SPI driver LCD interface
* 22. Performance optimization of external GNSS data analysis module
* 23. Code optimization of checkNet function module
* 24. The request module adds support for HTTP1.1 version
* 25. Alibaba Cloud adds support for ssl connections
* 26. The machine lcd module adds the ISINK backlight control pin backlight gear control interface lcd_level_brightness
* 27. Fix the problem of dump caused by restarting the virtual machine after handshake failure due to ussl double authentication key error
* 28. The time synchronization interface settime of the ntptime module adds a parameter use_rhost to allow users to set whether to use the alternate NTP server address
* 29. The machine uart module adds read_once()/write_once() interface
* 30. Optimization of umqtt reconnection function



**[QPY_OCPU_V0001_EC200U_CNAA_FW] 2022-07-07**

* ZH
* 1、支持quecgnss - 内置GNSS定位功能
* 2、支持QuecPython标准库uos - 基本系统服务组件功能
* 3、支持QuecPython标准库gc - 内存碎片回收功能
* 4、支持QuecPython标准库ubinascii - 二进制与ASCII转换功能
* 5、支持QuecPython标准库ucollections - 集合和容器类型功能
* 6、支持QuecPython标准库urandom - 生成随机数功能
* 7、支持QuecPython标准库math - 数学运算功能
* 8、支持QuecPython标准库usocket - socket通信功能
* 9、支持QuecPython标准库uio - 输入输出流功能
* 10、支持QuecPython标准库ustruct - 打包和解压原始数据类型功能
* 11、支持QuecPython标准库ujson - JSON编码和解码功能
* 12、支持QuecPython标准库utime - 时间相关功能
* 13、支持QuecPython标准库sys/usys - 系统相关功能
* 14、支持QuecPython标准库uzlib - zlib解压缩功能
* 15、支持QuecPython标准库_thread - 多线程功能
* 16、支持QuecPython标准库uhashlib - 哈希算法功能
* 17、支持QuecPython类库example - 执行python脚本功能
* 18、支持QuecPython类库dataCall - 数据拨号功能
* 19、支持QuecPython类库cellLocator - 基站定位功能
* 20、支持QuecPython类库wifilocator - wifi定位功能
* 21、支持QuecPython类库sim - SIM卡相关功能
* 22、支持QuecPython类库sms - 短信功能
* 23、支持QuecPython类库net - 网络相关功能
* 24、支持QuecPython类库checkNet - 等待网络就绪
* 25、支持QuecPython类库fota - 固件升级功能
* 26、支持QuecPython类库app_fota - 用户文件升级功能
* 27、支持QuecPython类库audio - 音频播放功能
* 28、支持QuecPython类库record - 录音功能
* 29、支持QuecPython类库misc - Power相关功能
* 30、支持QuecPython类库misc - PowerKey相关功能
* 31、支持QuecPython类库misc - PWM功能
* 32、支持QuecPython类库misc - ADC功能
* 33、支持QuecPython类库misc - USB插拔检测
* 34、支持QuecPython类库misc - USBNET功能
* 35、支持QuecPython类库modem - 获取设备信息相关功能
* 36、支持QuecPython类库machine - Pin功能
* 37、支持QuecPython类库machine - UART功能
* 38、支持QuecPython类库machine - Timer功能
* 39、支持QuecPython类库machine - ExtInt中断
* 40、支持QuecPython类库machine - RTC功能
* 41、支持QuecPython类库machine - I2C功能
* 42、支持QuecPython类库machine - I2C_simulation功能
* 43、支持QuecPython类库machine - SPI功能
* 44、支持QuecPython类库machine - LCD功能/MIPI
* 45、支持QuecPython类库machine - WDT功能
* 46、支持QuecPython类库machine - KeyPad功能
* 47、支持QuecPython类库pm - 低功耗功能
* 48、支持QuecPython类库ure - 正则功能
* 49、支持QuecPython类库wifiScan - wifi热点扫描功能
* 50、支持QuecPython类库gnss - 外接gnss(L76K)定位数据解析
* 51、支持QuecPython类库camera - 摄像头功能（可选功能）
* 52、支持QuecPython类库qrcode - 二维码显示（可选功能）
* 53、支持QuecPython第三方库aLiYun - 阿里云服务
* 54、支持QuecPython第三方库TenCentYun- 腾讯云服务
* 55、支持QuecPython第三方库request - HTTP
* 56、支持QuecPython第三方库log - 日志
* 57、支持QuecPython第三方库umqtt - MQTT
* 58、支持QuecPython第三方库ntptime - NTP对时
* 59、支持QuecPython第三方库system - 环境配置
* 60、支持QuecPython第三方库ql_fs - 高级文件操作
* 61、支持QuecPython第三方库Queue - 消息队列
* 62、支持QuecPython第三方库sys_bus - 会话总线
* 63、支持QuecPython第三方库uasyncio - 协程
* 64、支持QuecPython第三方库ssl - 加密算法
* 65、支持QuecPython类库TTS
* 66、支持QuecPython类库voiceCall - 电话功能（仅2G模式下可打电话）

* EN
* 1. Support quecgnss - built-in GNSS positioning function
* 2. Support QuecPython standard library uos - basic system service component function
* 3. Support QuecPython standard library gc - memory fragmentation recovery function
* 4. Support QuecPython standard library ubinascii - binary and ASCII conversion function
* 5. Support QuecPython standard library ucollections - collection and container type functions
* 6. Support QuecPython standard library urandom - generate random number function
* 7. Support QuecPython standard library math - math operation function
* 8. Support QuecPython standard library usocket - socket communication function
* 9. Support QuecPython standard library uio - input and output stream function
* 10. Support QuecPython standard library ustruct - function of packing and unpacking primitive data types
* 11. Support QuecPython standard library ujson - JSON encoding and decoding functions
* 12. Support QuecPython standard library utime - time-related functions
* 13. Support QuecPython standard library sys/usys - system related functions
* 14. Support QuecPython standard library uzlib - zlib decompression function
* 15. Support QuecPython standard library _thread - multi-threading function
* 16. Support QuecPython standard library uhashlib - hash algorithm function
* 17. Support QuecPython class library example - execute python script function
* 18. Support QuecPython class library dataCall - data dialing function
* 19. Support QuecPython class library cellLocator - base station positioning function
* 20. Support QuecPython class library wifilocator - wifi positioning function
* 21. Support QuecPython class library sim - SIM card related functions
* 22. Support QuecPython class library sms - SMS function
* 23. Support QuecPython class library net - network related functions
* 24. Support QuecPython class library checkNet - wait for the network to be ready
* 25. Support QuecPython class library fota - firmware upgrade function
* 26. Support QuecPython class library app_fota - user file upgrade function
* 27. Support QuecPython class library audio - audio playback function
* 28. Support QuecPython class library record - recording function
* 29. Support QuecPython class library misc - Power related functions
* 30. Support QuecPython class library misc - PowerKey related functions
* 31. Support QuecPython class library misc - PWM function
* 32. Support QuecPython class library misc - ADC function
* 33. Support QuecPython class library misc - USB plug-in detection
* 34. Support QuecPython class library misc - USBNET function
* 35. Support QuecPython class library modem - get device information related functions
* 36. Support QuecPython class library machine - Pin function
* 37. Support QuecPython class library machine - UART function
* 38. Support QuecPython class library machine - Timer function
* 39. Support QuecPython class library machine - ExtInt interrupt
* 40. Support QuecPython class library machine - RTC function
* 41. Support QuecPython class library machine - I2C function
* 42. Support QuecPython class library machine - I2C_simulation function
* 43. Support QuecPython class library machine - SPI function
* 44. Support QuecPython class library machine - LCD function/MIPI
* 45. Support QuecPython class library machine-WDT function
* 46. Support QuecPython class library machine-KeyPad function
* 47. Support QuecPython class library pm - low power consumption function
* 48. Support QuecPython class library ure - regular function
* 49. Support QuecPython class library wifiScan - wifi hotspot scanning function
* 50. Support QuecPython class library gnss - external gnss (L76K) positioning data analysis
* 51. Support QuecPython class library camera - camera function (optional function)
* 52. Support QuecPython class library qrcode - two-dimensional code display (optional function)
* 53. Support QuecPython third-party library aLiYun - Alibaba Cloud Service
* 54. Support QuecPython third-party library TenCentYun- Tencent Cloud Service
* 55. Support QuecPython third-party library request - HTTP
* 56. Support QuecPython third-party library log - log
* 57. Support QuecPython third-party library umqtt - MQTT
* 58. Support QuecPython third-party library ntptime - NTP time synchronization
* 59. Support QuecPython third-party library system - environment configuration
* 60. Support QuecPython third-party library ql_fs - advanced file operations
* 61. Support QuecPython third-party library Queue - message queue
* 62. Support QuecPython third-party library sys_bus - session bus
* 63. Support QuecPython third-party library uasyncio - coroutine
* 64. Support QuecPython third-party library ssl - encryption algorithm
* 65. Support QuecPython class library TTS
* 66. Support QuecPython class library voiceCall - telephone function (only in 2G mode can make calls)

