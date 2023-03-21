#include "onenet.h"
#include "onenetconfig.h"
#include "onenetmqtt.h"

void OneNETInit()
{
    //初始化OneNETConfig
    OneNETConfigInit();

    //初始化OneNETMQTT
    OneNETMQTTInit();
}
