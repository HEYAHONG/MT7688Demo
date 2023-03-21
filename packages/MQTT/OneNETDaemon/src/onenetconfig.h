#ifndef ONENETCONFIG_H
#define ONENETCONFIG_H
#ifdef __cplusplus
#include <string>
#include "stdint.h"
#include "stdlib.h"

typedef struct
{
    struct
    {
        std::string serveraddr;
        uint32_t serverport;
        int keeplive;
        std::string ca;
        bool ssl;
        bool checkservername;
    } MQTT;
    struct
    {
        std::string devicename;
        std::string productid;
        std::string accesskey;
    } OneNET;
} OneNETConfig;

OneNETConfig &OneNETConfigDefault();

void OneNETConfigInit();

#endif // __cplusplus

#endif // ONENETCONFIG_H
