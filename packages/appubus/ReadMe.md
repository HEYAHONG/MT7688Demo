# 说明 

本软件包为ubus客户端连接程序库,主要使用C++封装常用的ubus功能。

本软件主要采用CMake管理源代码,源代码目录为[src](src)。

 本软件包依赖如下软件包：

- C/C++运行时库：libstdcpp libc librt
- pthread线程库：libpthread
- libubus
- libubox
- libjson-c
- libblobmsg-json
- jsoncpp

主要功能如下：

- 自动连接ubus守护进程。

 
