#include "onenet.h"
#include "onenetconfig.h"
#include "onenetmqtt.h"
#include "onenetonejson.h"
#include "onenetdevice.h"

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

}
