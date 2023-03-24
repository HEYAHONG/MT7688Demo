#include "onenet.h"
#include "onenetconfig.h"
#include "onenetmqtt.h"
#include "onenetonejson.h"
#include "onenetdevice.h"
#include "onenetcontext.h"

void OneNETInit()
{

    //初始化OneNETConfig
    OneNETConfigInit();

    //初始化OneNETMQTT
    OneNETMQTTInit();

    //初始化OneJson
    OneNETOneJsonInit();

    //初始化设备
    OneNETDeviceInit();

    //初始化上下文
    OneNETContextInit();

}
