#include "netlib.h"

#ifdef VERSION
#define VERSION_STR MICROTOSTR2(VERSION)
#endif // VERSION

#ifdef PACKAGE_VERSION
#ifdef VERSION_STR
#undef VERSION_STR
#endif // VERSION_STR
#define VERSION_STR MICROTOSTR2(PACKAGE_VERSION)
#endif // PACKAGE_VERSION

#ifndef VERSION_STR
#define VERSION_STR "testing"
#endif // VERSION_STR

/** \brief 获取库版本
 *
 * \return const char* 版本字符串
 *
 */
const char *netlib_getversion()
{
    return VERSION_STR;
}
