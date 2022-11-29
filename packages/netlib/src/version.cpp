#include "netlib.h"

#ifdef VERSION
#define VERSION_STR MICROTOSTR2(VERSION)
#endif // VERSION
#ifdef PACKAGE_VERSION
#define VERSION_STR MICROTOSTR2(PACKAGE_VERSION)
#endif // PACKAGE_VERSION
#ifndef VERSION
#define VERSION "testing"
#endif // VERSION

const char *netlib_getversion()
{
    return VERSION_STR;
}
