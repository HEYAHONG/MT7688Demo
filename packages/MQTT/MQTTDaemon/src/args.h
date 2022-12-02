#ifndef ARGS_H
#define ARGS_H
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/** \brief 检查程序参数
 *
 * \param argc int 参数个数
 * \param argv[] char* 参数列表
 *
 */
void args_parse(int argc, char *argv[]);


/** \brief 打印参数
 *
 *
 */
void args_print();


/** \brief 获取参数个数
 *
 * \return size_t  参数个数
 *
 */
size_t args_size();

/** \brief 获取参数键名
 *
 * \param index size_t 从0开始的引索
 * \return const char* 键名，失败返回NULL
 *
 */
const char *args_getkey(size_t index);

/** \brief 获取参数
 *
 * \param key const char* 键名
 * \return const char* 参数数据，失败返回NULL
 *
 */
const char *args_get(const char * key);

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus

#endif // __cplusplus

#endif // ARGS_H
