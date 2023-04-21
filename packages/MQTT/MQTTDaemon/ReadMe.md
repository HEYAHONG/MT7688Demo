# 说明 

本软件包为MQTT守护进程，主要功能为连接MQTT服务器。

本软件主要采用CMake管理源代码,源代码目录为[src](src)。

 本软件包依赖如下软件包：

- C/C++运行时库：libstdcpp libc librt
- pthread线程库：libpthread
- appnetlib
- argtable3
- libmosquitto-ssl

# 配置参数

本章节中，配置表示形如key=value的数据，其中key表示键名,value表示值。

## 守护进程命令行参数

本程序采用argtable3分析命令行获取配置参数。

主要包含文件配置与单独配置，单独配置可覆盖配置文件中的配置。

### 文件配置

配置文件可通过-f或者--file=传递给守护进程。最多1个配置文件。配置文件中每行表示一个单独配置。

### 单独配置

单独配置可通过-s或者--setting=传递给守护进程。最多256个单独配置。单独配置可覆盖配置文件中的参数。

## openwrt中的配置参数 

openwrt中的配置文件为uci配置文件，可方便使用luci的cbi模块生成网页配置，本软件包中默认的uci配置文件目录为 [/etc/config](files/etc/config) 。

在守护进程的启动脚本中，会通过脚本将uci配置文件与其它一些设置写入/tmp中的一个文件,并通过命令行参数在启动守护进程时传递给守护进程。init脚本目录为 [/etc/init.d](files/etc/init.d) 。
